#include <QThread>
#include <QDebug>
#include "worker.h"


Worker::Worker(int id)
    :m_id(id),
     m_oHistogram()
{
    m_oMessageQueue.clear();
}

Worker::~Worker()
{
}

void Worker::receiveChunk(uchar* poRedByte, qsizetype size)
{
    WorkerMessage oMessage(poRedByte,size);
    m_oMessageQueue.push_back(oMessage);
}


void Worker::start()
{
    qInfo() << "Thread # " << m_id << " started";
    while (1)
    {
        while (m_oMessageQueue.empty()) {
            QThread::msleep(1000);
        }

        WorkerMessage oMessage = m_oMessageQueue.front();
        m_oMessageQueue.pop_front();

        uchar* poRedByte = oMessage.getPoRedByte();
        qsizetype size = oMessage.getSize();

        qInfo() << "Thread # " << m_id << " is requested to process " << poRedByte << "with size " << size;

        m_oHistogram.clear();

        for (int i = 0; i < size; i+=4)
        {
            int redChannel = *(poRedByte+i+0);
            int greenChannel = *(poRedByte+i+1);
            int blueChannel = *(poRedByte+i+2);
             int alphaChannel = *(poRedByte+i+3);

            m_oHistogram.m_r[redChannel]++;
            m_oHistogram.m_g[greenChannel]++;
            m_oHistogram.m_b[blueChannel]++;
        }

        emit chunkProcessed(m_id, &m_oHistogram);

    }

}
