#include <QTextStream>
#include <QThread>
#include <QDebug>
#include <QObject>
#include <QMutexLocker>

#include "dispatcher.h"

Dispatcher::Dispatcher(Histogram* poHistogram, int chunkSize)
    :m_poHistogram(poHistogram)
{
    setChunkSize(chunkSize);
}

Dispatcher::~Dispatcher()
{
;
}

void Dispatcher::setChunkSize(int chunkSize)
{
    m_chunkSize = chunkSize;
}

int Dispatcher::getChunkSize() const
{
    return m_chunkSize;
}
void Dispatcher::processBits(uchar* bits, qsizetype size)
{

    qInfo() << " ********************************************";
    qInfo() << " *****         Creating threads         *****";
    qInfo() << " ********************************************";

    int optimalThreadCount = QThread::idealThreadCount();
    // create and start threads

    for (int worker_id = 0 ; worker_id < optimalThreadCount; worker_id++)
    {
        QThread* poThread = new QThread();
        Worker* poWorker = new Worker(worker_id);


        QObject::connect(poThread, SIGNAL(started()), poWorker, SLOT(start()));
        QObject::connect(poWorker, SIGNAL(destroyed()), poThread, SLOT(quit()));
        QObject::connect(poWorker, SIGNAL(chunkProcessed(int, Histogram*)),
                                   this, SLOT(chunkProcessed(int,Histogram*) ),
                                   Qt::DirectConnection);

        poWorker->moveToThread(poThread);
        poThread->start();

        m_allWorkerList.push_back(poWorker);
        m_availableWorkerList.push_back(worker_id);
   }

    qInfo() << " ********************************************";
    qInfo() << " *****      Dispatching tasks           *****";
    qInfo() << " ********************************************";
    qInfo() << "";

    qsizetype i = 0;

    while (i < size)
    {
        {
            QMutexLocker lock(&m_mutexAvailableWorkerList);

            while (m_availableWorkerList.empty())
            {
                m_wc.wait(&m_mutexAvailableWorkerList);
            }


            int workerId = m_availableWorkerList.pop();

            if ((i+m_chunkSize*4) < size)
            {
                (m_allWorkerList[workerId])->receiveChunk(&(bits[i]),m_chunkSize*4);
                i+=m_chunkSize*4;
            }
            else
            {
                (m_allWorkerList[workerId])->receiveChunk(&(bits[i]),size-i);
                i+=size-i;
            }
        }
    }

    qInfo() << " ********************************************";
    qInfo() << " *****Awaiting completion of all threads*****";
    qInfo() << " ********************************************";
    m_mutexAvailableWorkerList.lock();
    while (m_availableWorkerList.size() != optimalThreadCount)
        m_wc.wait(&m_mutexAvailableWorkerList);
    m_mutexAvailableWorkerList.unlock();

}

// slot
void Dispatcher::chunkProcessed(int thread_id, Histogram* poHistogram)
{
    qInfo() << "Dispatcher receives results from thread #" << thread_id;

    {
        QMutexLocker lock(&m_mutexConsolidatedHistogram);

        for (int i=0; i <256; i++)
        {
            m_poHistogram->m_r[i] += poHistogram->m_r[i];
            m_poHistogram->m_g[i] += poHistogram->m_g[i];
            m_poHistogram->m_b[i] += poHistogram->m_b[i];
        }
    }

    {
        QMutexLocker lock(&m_mutexAvailableWorkerList);
        m_availableWorkerList.push_back(thread_id);
        m_wc.wakeOne();
    }
}

