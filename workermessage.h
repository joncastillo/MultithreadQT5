#ifndef WORKERMESSAGE_H
#define WORKERMESSAGE_H

#include <QImage>
class WorkerMessage
{
public:
    WorkerMessage(uchar* poRedByte, qsizetype size);
    uchar* getPoRedByte() const;
    qsizetype getSize() const;
private:
    uchar* m_poRedByte;
    qsizetype m_size;
};

#endif // WORKERMESSAGE_H
