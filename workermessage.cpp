#include "workermessage.h"

WorkerMessage::WorkerMessage(uchar* poRedByte, qsizetype size)
    : m_poRedByte(poRedByte),
      m_size(size)
{}

uchar* WorkerMessage::getPoRedByte() const
{
    return m_poRedByte;
}

qsizetype WorkerMessage::getSize() const
{
    return m_size;
}
