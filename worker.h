#ifndef WORKER_H
#define WORKER_H


#include <QVector>
#include <QQueue>
#include "histogram.h"
#include "workermessage.h"

class Worker : public QObject {
    Q_OBJECT

public:

    Worker(int id);
    virtual ~Worker();

public slots:
    void receiveChunk(uchar* poRedByte, qsizetype size);
    void start();

signals:

    void chunkProcessed(int id, Histogram* oHistogram);

private:

    QQueue<WorkerMessage> m_oMessageQueue;
    int m_id;
    Histogram m_oHistogram;
};

#endif // WORKER_H
