#include "QDebug"
#include "worker.h"
#include "dispatcher.h"
//#include "Logger.h"

Worker::Worker(int workerId, Dispatcher* poDispatcher) :
    m_workerId(workerId),
    m_poDispatcher(poDispatcher)
{

}

void Worker::start()
{
    //m_oLogger.write( "process started." );
    run();
}


void Worker::run()
{



    bool isFinished = false;

    while (1)
    {
        m_messageQueueMutex.lock();

        //m_oLogger.write( "Condition variable sleeps." );
        while (m_tasks.empty())
        {
            m_wc.wait(&m_messageQueueMutex);
        }
        //m_oLogger.write( "Condition variable awakens." );

        Message* poCurrentTask = m_tasks.front();




        // routine for current task:

        switch (poCurrentTask->getSubMessageType())
        {
            case SubMessageType_e::DISPATCHER_ORIGINATED__ENQUEUE_WORKERTASK:
            qInfo()<<" *  Worker " << this->m_workerId << ": received task.";
                {
                    unsigned int channelRed[256];
                    unsigned int channelGreen[256];
                    unsigned int channelBlue[256];

                    memset (channelRed, 0, sizeof(channelRed));
                    memset (channelGreen, 0, sizeof(channelGreen));
                    memset (channelBlue, 0, sizeof(channelRed));

                    UIntArrayMessage* poMessage = static_cast<UIntArrayMessage*> (poCurrentTask);
                    QVector<unsigned int>& message_data = poMessage->getData();

                    //m_oLogger.write( "Processing task..." );
                    // The message is always aligned to start at the R channel:
                    for (int i = 0; i < message_data.size() ; i+=4)
                    {
                        channelRed[ message_data[i+0]]++ ;
                        channelGreen[ message_data[i+1]]++ ;
                        channelBlue[ message_data[i+2]]++ ;
                    }
                    //m_oLogger.write( "Processing completed." );


                    // compose return message:
                    UIntArrayMessage* poResponseMessage= new UIntArrayMessage;
                    poResponseMessage->setSubMessageType(SubMessageType_e::WORKER_ORIGINATED__TASK_COMPLETED);

                    unsigned int dataLength = 1 + 256*3;
                    unsigned int data[dataLength];
                    memset (data, 0, sizeof(data));
                    data[0] = (unsigned int) m_workerId;

                    for (unsigned int i = 0 ; i < 256 ; i++)
                    {
                        data[i+1] = (unsigned int) channelRed[i];
                        data[i+1+256] = (unsigned int) channelGreen[i];
                        data[i+1+512] = (unsigned int) channelBlue[i];
                    }

                    poResponseMessage->setData(data,dataLength);
                    m_poDispatcher->postMessage(poResponseMessage);
                    //m_oLogger.write( "Response message with consolidated data posted." );
                delete poMessage;
        }
                break;

            case SubMessageType_e::DISPATCHER_ORIGINATED__TERMINATE_WORKER:
                {
                    CommandMessage* poMessage = static_cast<CommandMessage*> (poCurrentTask);
                    isFinished = true;

                    delete poMessage;
        }
                break;

            default:
                delete poCurrentTask;
                break;
        }


        m_tasks.pop_front();
        m_messageQueueMutex.unlock();

        if (isFinished)
            emit finished();
    }
}

void Worker::postMessage(Message* poMessage) {
    //m_oLogger.write( "Message received." );
    m_messageQueueMutex.lock();

    switch (poMessage->getSubMessageType())
    {
        case SubMessageType_e::DISPATCHER_ORIGINATED__ENQUEUE_WORKERTASK:
        {
            //m_oLogger.write( "Submessage type: DISPATCHER_ORIGINATED__ENQUEUE_WORKERTASK" );
            UIntArrayMessage* poMessageWithData = dynamic_cast<UIntArrayMessage*>(poMessage);
            UIntArrayMessage* task = new UIntArrayMessage();

            task -> setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__ENQUEUE_WORKERTASK);
            // copy message so that only internal message will be processed. this simplifies deletion.
            task -> setData(poMessageWithData->getData(),0,poMessageWithData->getData().size());
            m_tasks.push_back(task); // pass through
            //m_oLogger.write( "Task queued." );
            break;
        }
        case SubMessageType_e::DISPATCHER_ORIGINATED__TERMINATE_WORKER:
        {
            //m_oLogger.write( "Submessage type: DISPATCHER_ORIGINATED__TERMINATE_WORKER" );
            CommandMessage* poCommandMessage = new CommandMessage;
            poCommandMessage->setSubMessageType(SubMessageType_e::DISPATCHER_ORIGINATED__TERMINATE_WORKER);
            poCommandMessage->setCommand(CommandType_e::TERMINATE);
            m_tasks.push_back(poCommandMessage); // pass through
            //m_oLogger.write( "Task queued." );
        }
        default:
            break;
    }
    m_wc.wakeOne();
    //m_oLogger.write( "Notified condition variable." );
    m_messageQueueMutex.unlock();        
}

// void Worker::finished() // signal (dont implement in cpp)
