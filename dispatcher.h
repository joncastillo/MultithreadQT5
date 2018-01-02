#ifndef DISPATCHER_H
#define DISPATCHER_H



#include <QMutex>
#include <QMutexLocker>
#include <QVector>
#include <QMap>
#include <QThread>
#include <QWaitCondition>

#include "message.h"
#include "worker.h"

// The dispatcher is responsible for dividing the RGBA byte array into chunks so that they can be
// distributed to each worker thread.
//
// Responsible for creating, starting and managing workers.
//
// inputfile --> (TaskBuilder) --> (Dispatcher) --> Worker Threads
// outputfile <-- (TaskBuilder) <-- (Dispatcher, consolidates) <-- (Worker Thread, chunk) <--

class TaskBuilder;
class Worker;

class Dispatcher:public QObject
{
    Q_OBJECT
public:
    Dispatcher(int chunkSize = 50000);
    virtual ~Dispatcher();
    virtual void registerWorkers();
    virtual void unregisterWorkers();
    virtual void init();
    // entry point for Dispatcher
    virtual void postMessage(Message* poMessage);
    TaskBuilder* m_poTaskBuilder;

public slots:
    void run();

signals:
        void finished();
private:


    QQueue<Message*> m_tasksForProcessing;
    QQueue<Message*> m_tasksForDistribution;
    // consolidatedRedChannel contains the consolidated Red Histogram from each worker.
    QVector<unsigned int> consolidatedRedChannel;
    // consolidatedRedChannel contains the consolidated Green Histogram from each worker.
    QVector<unsigned int> consolidatedGreenChannel;
    // consolidatedRedChannel contains the consolidated Blue Histogram from each worker.
    QVector<unsigned int> consolidatedBlueChannel;

    // m_chunkSize determines the count of pixels to be sent to worker thread.
    int m_chunkSize;
    QMap<int, Worker*> m_mapWorkers;
    QQueue<int> availableWorkerIds;
    QMutex m_messageIncomingMutex;
    QMutex m_messageQueueMutex;
    QWaitCondition m_wc;


};

#endif // DISPATCHER_H
