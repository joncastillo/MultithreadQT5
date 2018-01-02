#include <QObject>

#include <QMutexLocker>
#include <QMutex>
#include <QTextStream>
#include <QWaitCondition>
#include <QMap>

#include <QDebug>
#include "dispatcher.h"
#include "taskbuilder.h"

#include <iostream>
Dispatcher::~Dispatcher()
{}
Dispatcher::Dispatcher(int chunkSize)
    : m_chunkSize(chunkSize)
{
    registerWorkers();
    init();
}

void Dispatcher::init()
{
    consolidatedRedChannel.clear();
    consolidatedGreenChannel.clear();
    consolidatedBlueChannel.clear();

    consolidatedRedChannel.resize(256);
    consolidatedGreenChannel.resize(256);
    consolidatedBlueChannel.resize(256);
}
void Dispatcher::registerWorkers()
{
    // determine optimal worker threads:
    int optimalThreadCount = QThread::idealThreadCount();
    //m_oLogger.write( "Optimal thread count is " + QString::number( optimalThreadCount) + ".\n");

    // create and start threads

    for (int i = 0 ; i < optimalThreadCount; i++)
    {
        QThread* poThread = new QThread();
        Worker* poWorker = new Worker(i, this);
        int worker_id = i;

        poWorker->moveToThread(poThread);
        QObject::connect(poThread, SIGNAL(started()), poWorker, SLOT(start()));
        QObject::connect(poThread, SIGNAL(finished()), poThread, SLOT(deleteLater()));

        QObject::connect(poWorker, SIGNAL(finished()), poThread, SLOT(quit()));
        QObject::connect(poWorker, SIGNAL(finished()), poWorker, SLOT(deleteLater()));

        m_mapWorkers[worker_id] = poWorker;


        poThread->start();
        //m_oLogger.write( "Worker id " + QString::number( worker_id) + " Created, registered and started.\n");

        availableWorkerIds.push_back(i);
        //m_oLogger.write( "Worker id " + QString::number(worker_id) + " Marked as available.\n");
    }
}

void Dispatcher::unregisterWorkers()
{
    for (QMap<int, Worker*>::iterator it = m_mapWorkers.begin();
         it != m_mapWorkers.end();
         ++it)
    {

        if (it.value())
        {
            CommandMessage oMessage;
            oMessage.setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__TERMINATE_WORKER);
            it.value()->postMessage(&oMessage);
        }
    }
}

// message entry point for Dispatcher
void Dispatcher::postMessage(Message* poMessage)
{
    //m_oLogger.write("Message received.\n");

    m_messageQueueMutex.lock();

    switch (poMessage->getSubMessageType() )
    {

        case SubMessageType_e::TASKBUILDER_ORIGINATED__ENQUEUE_DISPATCHERTASK:
        {
            //m_oLogger.write("Message subtype: TASKBUILDER_ORIGINATED__ENQUEUE_DISPATCHERTASK\n");

            ByteArrayMessage* poMessageWithData = dynamic_cast<ByteArrayMessage*>(poMessage);
            unsigned int rawDataSize = poMessageWithData->getData().size();

            unsigned int fourByteChunkSize = m_chunkSize*4;
            unsigned int i = 0;

            for (; (i+fourByteChunkSize) < rawDataSize;  i += fourByteChunkSize)
            {
                UIntArrayMessage* task = new UIntArrayMessage;
                task->setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__WORKERTASK_READY);
                task->setData(poMessageWithData->getData(), i, fourByteChunkSize);
                m_tasksForDistribution.push_back(task);

            }

            if (i < rawDataSize )
            {
                UIntArrayMessage* task = new UIntArrayMessage;
                task->setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__WORKERTASK_READY);
                task->setData(poMessageWithData->getData(), i, rawDataSize-i);
                m_tasksForDistribution.push_back(task);
            }
        }
        break;

        case SubMessageType_e::WORKER_ORIGINATED__TASK_COMPLETED:
        {
            //m_oLogger.write("Message subtype: WORKER_ORIGINATED__TASK_COMPLETED\n");
            UIntArrayMessage* poMessageWithData = dynamic_cast<UIntArrayMessage*>(poMessage);
            int worker_id = (poMessageWithData->getData())[0];
            QVector<unsigned int>::iterator itRedChannel = poMessageWithData->getData().begin()+1;
            QVector<unsigned int>::iterator itGreenChannel = itRedChannel+256;
            QVector<unsigned int>::iterator itBlueChannel = itGreenChannel+256;

            for (int i = 0; i < 256; i++)
            {
                consolidatedRedChannel[i] = consolidatedRedChannel[i]+itRedChannel[i];
                consolidatedGreenChannel[i] = consolidatedGreenChannel[i]+itGreenChannel[i];
                consolidatedBlueChannel[i] = consolidatedBlueChannel[i]+itBlueChannel[i];
            }

            availableWorkerIds.push_back(worker_id);


            CommandMessage* poCommandMessage = new CommandMessage();
            poCommandMessage->setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__WORKER_AVAILABLE);
            m_tasksForProcessing.push_back(poCommandMessage);

            if (availableWorkerIds.size()>=m_mapWorkers.size() && m_tasksForDistribution.empty())
            {
                qInfo()<<"All workers available, no tasks left for processing.";

                //m_oLogger.write("All workers available, no tasks left for processing.\n");
                UIntArrayMessage oMessage;
                oMessage.setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__TASK_COMPLETED);

                QVector<unsigned int> consolidatedRGBChannel;

                consolidatedRGBChannel.append(consolidatedRedChannel);
                consolidatedRGBChannel.append(consolidatedGreenChannel);
                consolidatedRGBChannel.append(consolidatedBlueChannel);

                oMessage.setData(consolidatedRGBChannel,0,256*3);

                m_poTaskBuilder->postMessage(&oMessage);
                //m_oLogger.write("Consolidated RGB data posted back to task builder.\n");
            }
            else
            {
                //send worker avail message
            }
        }
        break;

        default:
            break;
    }
    m_wc.wakeOne();
    m_messageQueueMutex.unlock();
}

void Dispatcher::run()
{
    bool isFinished = false;
    while (1)
    {
        //QMutexLocker locker(&m_messageQueueMutex);
        m_messageQueueMutex.lock();
        {
            ////m_oLogger.write("Dispatcher consumer sleeps.\n";
            while (m_tasksForDistribution.empty() && m_tasksForProcessing.empty())
            {
                m_wc.wait(&m_messageQueueMutex);
            }
            ////m_oLogger.write("Consumer awakens.\n";

            if ( !m_tasksForProcessing.empty() )
            {
                Message* poCurrentTask = m_tasksForProcessing.front();
                SubMessageType_e submessageType = poCurrentTask->getSubMessageType();

                if (submessageType == SubMessageType_e::TASKBUILDER_ORIGINATED__TERMINATE_DISPATCHER)
                {
                    //m_oLogger.write("message subtype TASKBUILDER_ORIGINATED__TERMINATE_DISPATCHER.\n");
                    CommandMessage* poMessageWithData = dynamic_cast<CommandMessage*>(poCurrentTask);
                    delete poMessageWithData;

                    unregisterWorkers();
                    isFinished = true;
                }
                else if (submessageType == SubMessageType_e::DISPATCHER_ORIGINATED__WORKER_AVAILABLE)
                {
                    //m_oLogger.write("message subtype DISPATCHER_ORIGINATED__WORKER_AVAILABLE.\n");
                    CommandMessage* poMessageWithData = dynamic_cast<CommandMessage*>(poCurrentTask);
                    delete poMessageWithData;
                }
                else
                {
                    //m_oLogger.write("Warning! An unhandled message has been discarded!\n");
                    delete poCurrentTask;
                }

                m_tasksForProcessing.pop_front();
            }


            if ( !m_tasksForDistribution.empty() )
            {
                if ( !availableWorkerIds.empty() )
                {
                    //m_oLogger.write("Consumer attempts to process a task for distribution.\n");

                    int currentWorkerId=availableWorkerIds.front();
                    availableWorkerIds.pop_front();
                    UIntArrayMessage* poMessageWithData = dynamic_cast<UIntArrayMessage*>(m_tasksForDistribution.front());
                    UIntArrayMessage oMessageForWorker;
                    oMessageForWorker.setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__ENQUEUE_WORKERTASK);
                    oMessageForWorker.setData(poMessageWithData->getData(), 0, poMessageWithData->getData().size());

                    //m_oLogger.write("Forwarding task to Worker ID " + QString::number(currentWorkerId) + ".\n");
                    m_mapWorkers[currentWorkerId]->postMessage(&oMessageForWorker);

                    delete poMessageWithData;
                    m_tasksForDistribution.pop_front();

                }
            }
        }
        m_messageQueueMutex.unlock();
    }
    emit finished();
}

