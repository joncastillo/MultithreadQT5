#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QObject>
#include <QVector>
#include <QStack>
#include <QMutex>
#include <QWaitCondition>

#include <worker.h>
#include <histogram.h>
class Dispatcher: public QObject
{
    Q_OBJECT

public:
    Dispatcher(Histogram* poHistogram, int chunkSize);
    virtual ~Dispatcher();
    void setChunkSize(int chunkSize);
    int getChunkSize() const;
    void processBits(uchar* bits, qsizetype size);

public slots:
    void chunkProcessed(int thread_id, Histogram* poHistogram);

signals:
    ;

private:
    int m_chunkSize;
    Histogram* m_poHistogram;

    QStack<int> m_availableWorkerList;
    QVector<Worker*> m_allWorkerList;

    QMutex m_mutexConsolidatedHistogram;
    QMutex m_mutexAvailableWorkerList;
    QWaitCondition m_wc;
};

#endif // DISPATCHER_H
