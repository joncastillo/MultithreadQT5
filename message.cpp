#include <QVector>
#include <QSharedPointer>

#include "message.h"


Message::Message() :
    m_messageType(MessageType_e::INVALID_MESSAGE_TYPE),
    m_subMessageType(SubMessageType_e::INVALID_SUBMESSAGE_TYPE)
{

}

void Message::setMessageType(MessageType_e messageType)
{
    m_messageType = messageType;
}

void Message::setSubMessageType(SubMessageType_e subMessageType)
{
    m_subMessageType = subMessageType;
}

MessageType_e Message::getMessageType()
{
    return m_messageType;
}

SubMessageType_e Message::getSubMessageType()
{
    return m_subMessageType;
}

Message::~Message()
{
    // ensure deletion of poData
}

CommandMessage::CommandMessage() :
    Message(),
    m_command(CommandType_e::INVALID)
{
    setMessageType(MessageType_e::NODATA__COMMANDMESSAGE);
}

void CommandMessage::setCommand (CommandType_e command)
{
    m_command = command;
}

CommandType_e CommandMessage::getCommand()
{
    return m_command;
}


ByteArrayMessage::ByteArrayMessage():
    Message()
{
    setMessageType(MessageType_e::DATACARRYING__BYTE_ARRAY);
}

void ByteArrayMessage::setData(unsigned char* poSourceData, int length)
{
    m_oData.clear();
    for ( int i = 0 ; i < length ; i++)
        m_oData.push_back(poSourceData[i]);
}
void ByteArrayMessage::setData(unsigned char* poSourceData, int startIndex, int length)
{
    m_oData.resize(length);
    std::copy(poSourceData+startIndex,poSourceData+startIndex+length, m_oData.begin());

}

void ByteArrayMessage::setData(QVector<unsigned char> source, int startIndex, int length)
{
    m_oData.clear();
    for (QVector<unsigned char>::iterator it = source.begin() + startIndex;
         it != source.begin() + length;
         ++it)
    {
        m_oData.push_back(*it);
    }
}

QVector<unsigned char>& ByteArrayMessage::getData()
{
    return m_oData;
}


UIntArrayMessage::UIntArrayMessage():
    Message()
{
    setMessageType(MessageType_e::DATACARRYING__ULONG_ARRAY);
}

void UIntArrayMessage::setData(unsigned int* poSourceData, int length)
{
    for (int i = 0; i < length ; i++)
        m_oData.push_back(poSourceData[i]);
}
void UIntArrayMessage::setData(unsigned int* poSourceData, int startIndex, int length)
{
    m_oData.resize(length);
    std::copy(poSourceData+startIndex,poSourceData+startIndex+length, m_oData.begin());
}

void UIntArrayMessage::setData(QVector<unsigned int> source, int startIndex, int length)
{
    m_oData.clear();
    for (QVector<unsigned int>::iterator it = source.begin() + startIndex;
         it != source.begin() + length;
         ++it)
    {
        m_oData.push_back(*it);
    }
}

void UIntArrayMessage::setData(QVector<unsigned char> source, int startIndex, int length)
{
    m_oData.clear();
    for (QVector<unsigned char>::iterator it = source.begin() + startIndex;
         it != source.begin() + startIndex + length;
         ++it)
    {
        unsigned char value = *it;
        m_oData.push_back((unsigned int)(value));
    }
}

QVector<unsigned int>& UIntArrayMessage::getData()
{
    return m_oData;
}



