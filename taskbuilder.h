#ifndef TASKBUILDER_H
#define TASKBUILDER_H
#include <QThread>


#include "message.h"
#include "imagepreprocessor.h"

class Dispatcher;

// The taskbuilder is the main manager of the threaded process.
// Starts the Dispatcher and sends tasks to it for processing.
// Also receives the consolidated results from the Dispatcher.

// inputfile --> (TaskBuilder) --> (Dispatcher) --> Worker Threads
// outputfile <-- (TaskBuilder) <-- (Dispatcher, consolidates) <-- (Worker Thread, chunk) <--

class TaskBuilder
{
public:

    TaskBuilder();
    virtual ~TaskBuilder();
    virtual void createHistogram(QString sourceImageFilename, QString targetOutFilename);
    virtual void postMessage(Message* poMessage);
    bool is_busy;
    void finished();

private:

    Dispatcher* m_poDispatcherWorker;
    QThread* m_poDispatcherThread;
    QFile* m_poOutfile;
};

#endif // TASKBUILDER_H
