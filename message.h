#ifndef MESSAGE_H
#define MESSAGE_H
#include <QSharedPointer>
#include <QVector>

// Message classes used for inter-module communication.
// ByteArrayMessage carries a payload of vector<unsigned char>
// UIntArrayMessage carries a payload of vector<UInt>

enum class MessageType_e
{
    INVALID_MESSAGE_TYPE,
    NODATA__COMMANDMESSAGE,
    DATACARRYING__BYTE_ARRAY,
    DATACARRYING__ULONG_ARRAY
};

enum class SubMessageType_e
{
    INVALID_SUBMESSAGE_TYPE,
    TASKBUILDER_ORIGINATED__ENQUEUE_DISPATCHERTASK,
    TASKBUILDER_ORIGINATED__TERMINATE_DISPATCHER,
    DISPATCHER_ORIGINATED__WORKERTASK_READY,
    DISPATCHER_ORIGINATED__ENQUEUE_WORKERTASK,
    DISPATCHER_ORIGINATED__WORKER_AVAILABLE,
    DISPATCHER_ORIGINATED__TERMINATE_WORKER,
    DISPATCHER_ORIGINATED__TASK_COMPLETED,    
    WORKER_ORIGINATED__TASK_COMPLETED
};

enum class CommandType_e
{
    INVALID,
    RUN,
    TERMINATE
};

class Message
{
public:
    Message();    
    virtual void setMessageType(MessageType_e messageType) final;
    virtual void setSubMessageType(SubMessageType_e messageType) final;
    virtual MessageType_e getMessageType() final;
    virtual SubMessageType_e getSubMessageType() final;

    virtual ~Message(); // ensure deletion of poData

private:
    MessageType_e m_messageType;
    SubMessageType_e m_subMessageType;
};


class CommandMessage: public Message
{
public:
    CommandMessage();
    void setCommand (CommandType_e command);
    CommandType_e getCommand();
private:
    CommandType_e m_command;
};

class ByteArrayMessage: public Message
{
public:
    ByteArrayMessage();
    void setData(unsigned char* poSourceData, int length); // use auto_ptr for safety. or data must be copied.
    void setData(unsigned char* poSourceData, int startIndex, int length); // use auto_ptr for safety. or data must be copied.
    void setData(QVector<unsigned char> source, int startIndex, int length);
    QVector<unsigned char>& getData();

private:
    QVector<unsigned char> m_oData;
};

class UIntArrayMessage: public Message
{
public:
    UIntArrayMessage();
    void setData(unsigned int* poSourceData, int length); // use auto_ptr for safety. or data must be copied.
    void setData(unsigned int* poSourceData, int startIndex, int length); // use auto_ptr for safety. or data must be copied.
    void setData(QVector<unsigned int> source,  int startIndex, int length);
    void setData(QVector<unsigned char> source, int startIndex, int length);

    QVector<unsigned int>& getData();

private:
    QVector<unsigned int> m_oData;
};


#endif // MESSAGE_H
