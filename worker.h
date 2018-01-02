#ifndef WORKER_H
#define WORKER_H

#include <QQueue>
#include <QWaitCondition>

#include "dispatcher.h"
#include "message.h"

enum class WorkerStatus_e
{
    IDLE,
    BUSY
};

// The Worker is a processing node for calculating the number of pixels for every RGB level.
// Operates in O(N/4) time.
// main --> imagePreprocessor --> Dispatcher --> (Worker Threads)

class Dispatcher;
class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(int workerId, Dispatcher* poDispatcher);
    virtual void postMessage(Message* poMessage);

public slots:
    void start();

signals:
    void finished();

private:
    virtual void run();


    int m_workerId;
    QQueue<Message*> m_tasks;
    WorkerStatus_e m_status;
    QMutex m_messageQueueMutex;
    QMutex m_messageIncomingMutex;
    QWaitCondition m_wc;

    Dispatcher* m_poDispatcher;
};





#endif // WORKER_H
