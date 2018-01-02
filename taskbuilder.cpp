#include <QFile>
#include <QImage>
#include <QException>
#include <iostream> // for endl
#include <QDebug>
#include "taskbuilder.h"
#include "message.h"
#include "dispatcher.h"
TaskBuilder::TaskBuilder()
    :is_busy(false)
{
    //m_oLogger.write("Constructing TaskBuilder.\n");
    if (!m_poDispatcherWorker)
    {
        m_poDispatcherWorker = new Dispatcher();
        m_poDispatcherThread = new QThread();
        m_poDispatcherWorker->m_poTaskBuilder = this;
        m_poDispatcherWorker->moveToThread(m_poDispatcherThread);

        QObject::connect(m_poDispatcherThread, SIGNAL(started()), m_poDispatcherWorker, SLOT(run()));
        QObject::connect(m_poDispatcherThread, SIGNAL(finished()), m_poDispatcherThread, SLOT(deleteLater()));
        QObject::connect(m_poDispatcherWorker, SIGNAL(finished()), m_poDispatcherThread, SLOT(quit()));
        QObject::connect(m_poDispatcherWorker, SIGNAL(finished()), m_poDispatcherWorker, SLOT(deleteLater()));

        m_poDispatcherThread->start();
        //m_oLogger.write("Dispatcher thread started.\n");
    }
}
TaskBuilder::~TaskBuilder()
{

    finished();
}

void TaskBuilder::createHistogram(QString sourceImageFilename, QString textOutFilename)
{
    //m_oLogger.write("createHistogram called for " + sourceImageFilename +"\n");
    //m_oLogger.write("Output will be written to " + textOutFilename + "\n");

    is_busy=true;
    m_poDispatcherWorker->init();

    m_poOutfile = new QFile(textOutFilename);

    QFile fileHandleSourceImage(sourceImageFilename);
    // Leave file open until bits have been copied to a secure vector location.
    if (!fileHandleSourceImage.open(QIODevice::ReadOnly))
    {
        qInfo() << "Error! unable to open image file!" << endl;
        is_busy = false;
        QException q;
        q.raise();
    }

    QImage img;
    img.load(sourceImageFilename);
    ImagePreprocessor oImagePreprocessor(img);

    unsigned char* bits = oImagePreprocessor.getBits();
    qsizetype size =  oImagePreprocessor.getSize();

    if (size == 0)
    {
        // as qvectors are used the max amount of elements is MAX_INT
        qInfo() << "Error! image may be too large!" << endl;
        is_busy = false;
        QException q;
        q.raise();
    }


    ByteArrayMessage* poMessage = new ByteArrayMessage();
    poMessage->setSubMessageType(SubMessageType_e::TASKBUILDER_ORIGINATED__ENQUEUE_DISPATCHERTASK);
    poMessage->setData(bits, size);
    m_poDispatcherWorker->postMessage(poMessage);
    //m_oLogger.write( "called m_poDispatcherWorker->postMessage(SubMessageType = TASKBUILDER_ORIGINATED__ENQUEUE_DISPATCHERTASK)\n");

    fileHandleSourceImage.close();
}

void TaskBuilder::postMessage(Message* theMessage)
{
    //m_oLogger.write( "postMessage received submessage " + QString::number((int)(theMessage->getSubMessageType())) + "\n");

    if (theMessage->getSubMessageType()==SubMessageType_e::DISPATCHER_ORIGINATED__TASK_COMPLETED)
    {
        UIntArrayMessage* poMessage = dynamic_cast<UIntArrayMessage*>(theMessage);
        QVector<unsigned int>::iterator it = poMessage->getData().begin();

        QString thestring;
        QTextStream line(&thestring);


        line << QString::number(*it);
        it++;
        for (;
             it != poMessage->getData().begin() + 256 ;
             ++it )
        {
            // red channel
            line << ",";
            line << QString::number(*it);
        }
        line << endl;

        //m_oLogger.write( "red channel appended." );


        line << QString::number(*it);
        it++;
        for (;
             it != poMessage->getData().begin() + 256 + 256 ;
             ++it )
        {
            // green channel
            line << ",";
            line << QString::number(*it);
        }
        line << endl;
        //m_oLogger.write( "green channel appended." );

        line << QString::number(*it);
        it++;
        for (;
             it != poMessage->getData().begin() + 256 + 256 +256;
             ++it )
        {
            // blue channel
            line << ",";
            line << QString::number(*it);
        }
        line << endl;
        //m_oLogger.write( "blue channel appended." );

        QString strline = line.readAll();

        m_poOutfile->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        QTextStream myStream (m_poOutfile);
        myStream << strline <<endl;
        m_poOutfile->close();
        delete m_poOutfile;
        m_poOutfile = nullptr;


        is_busy = false;
    }
}
void TaskBuilder::finished()
{
    // shut down dispatcher
    CommandMessage oMessage;
    oMessage.setSubMessageType(SubMessageType_e::TASKBUILDER_ORIGINATED__TERMINATE_DISPATCHER);
    m_poDispatcherWorker->postMessage(&oMessage);

    if (m_poOutfile)
        delete m_poOutfile;
}

