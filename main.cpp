#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QImage>
#include <QFile>

#include <iostream> // for endl

#include "dispatcher.h"

#define CHUNK_SIZE (5000000)

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Histogram Tool");
    QCoreApplication::setApplicationVersion("2.0");

    // get the commandline arguments
    const QStringList args = app.arguments();

    QCommandLineParser parser;
    parser.setApplicationDescription("RGB Histogram Generator. \n\nParses an image and obtain the colorlevel values. \n\nExample usage:\n\t./histogramtool -f DSC_0278.JPG -o output.txt\n\t./histogramtool -t ");

    QCommandLineOption testOption(QStringList() << "t" << "test",
        QCoreApplication::translate("main", "Run test cases unit and integrated tests in ./testframework/tests"));
    parser.addOption(testOption);

    QCommandLineOption sourceImageFileOption(QStringList() << "f" << "filename",
        QCoreApplication::translate("main", "Input image file path and name."),
        QCoreApplication::translate("main", "filename"));
    parser.addOption(sourceImageFileOption);

    QCommandLineOption outputFileOption(QStringList() << "o" << "outfilename",
        QCoreApplication::translate("main", "Output filename."),
        QCoreApplication::translate("main", "filename"));
    parser.addOption(outputFileOption);

    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    QStringList optionNames = parser.optionNames();



    Histogram oHistogram;
    Dispatcher oDispatcher(&oHistogram, CHUNK_SIZE);

    if (parser.isSet(sourceImageFileOption) && parser.isSet(outputFileOption))
    {
        qInfo() << " ********************************************";
        qInfo() << " *****         Processing image         *****";
        qInfo() << " ********************************************";

        QString sourceImageFilename = parser.value(sourceImageFileOption);
        QImage img;
        img.load(sourceImageFilename);

        img = img.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
        uchar *bits = img.bits();
        qsizetype size = img.sizeInBytes();

        qInfo() << " ********************************************";
        qInfo() << " ***Use dispatcher for parallel processing***";
        qInfo() << " ********************************************";

        oDispatcher.processBits(bits, size);

        qInfo() << " ********************************************";
        qInfo() << " *****  Serializing result to file      *****";
        qInfo() << " ********************************************";

        QString outputFilename = parser.value(outputFileOption);
        QFile outFile(outputFilename);
        outFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        QTextStream oOutFileStream (&outFile);

        oOutFileStream << oHistogram.m_r[0];
        for (int i = 1 ; i < 256 ; i++)
        {
            oOutFileStream << ", ";
            oOutFileStream << oHistogram.m_r[i];
        }
        oOutFileStream << "\r\n";

        oOutFileStream << oHistogram.m_g[0];
        for (int i = 1 ; i < 256 ; i++)
        {
            oOutFileStream << ", ";
            oOutFileStream << oHistogram.m_g[i];
        }
        oOutFileStream << "\r\n";

        oOutFileStream << oHistogram.m_b[0];
        for (int i = 1 ; i < 256 ; i++)
        {
            oOutFileStream << ", ";
            oOutFileStream << oHistogram.m_b[i];
        }
        oOutFileStream << "\r\n";

        outFile.close();

        qInfo() << " ********************************************";
        qInfo() << " *****            Completed             *****";
        qInfo() << " ********************************************";
    }

    else
    {
        parser.showHelp();
    }

    return 0;
}
