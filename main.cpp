#include <QCoreApplication>
#include <QImage>
#include <QStringList>
#include <QVector>
#include <QRgb>
#include <iostream>
#include <sstream>
#include <QMap>
#include <iomanip>
#include <QMessageLogger>
#include <QCommandLineParser>
#include "taskbuilder.h"
#include "testframework/testcaseextractor.h"
#include "testframework/testexpectation.h"

#include "testframework/histogram.h"
#include <QMutex>
#include <QWaitCondition>
#include "testframework/histogramcomparetool.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Histogram Tool");
    QCoreApplication::setApplicationVersion("1.0");

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

    if (parser.isSet(sourceImageFileOption) && parser.isSet(outputFileOption))
    {
        qInfo() << " ********************************************";
        qInfo() << " *****         Processing image         *****";
        qInfo() << " ********************************************";

        TaskBuilder oTaskBuilder;

        // create histogram file using module under test:
        qInfo() << " Reading image...";
        oTaskBuilder.createHistogram(parser.value(sourceImageFileOption),parser.value(outputFileOption));

        // prevent test framework from proceeding while previous test is being run.
        while (oTaskBuilder.is_busy) ;

        // create histogram from MUT's output file:
        Histogram actualHistogram(parser.value(outputFileOption));

        qDebug() << actualHistogram.to_string();
    }

    else if (parser.isSet(testOption))
    {
        int totalpass = 0;
        int totalfail = 0;

        qInfo() << " ********************************************";
        qInfo() << " *****     Starting Automated Tests     *****";
        qInfo() << " ********************************************";
        qInfo() << "";
        QVector<TestExpectation> testCaseExpectationIntegratedTestPatternType;
        QVector<TestExpectation> testCaseExpectationIntegratedTestPrecomputedType;

        // extract precomputed histogram from testcases:
        TestcaseExtractor::obtainTestCases(testCaseExpectationIntegratedTestPatternType, testCaseExpectationIntegratedTestPrecomputedType);

        for (TestExpectation exp: testCaseExpectationIntegratedTestPrecomputedType)
        {
            qDebug().noquote() << "Testing: ";
            qDebug().noquote() << exp.imageFilePath;
            qDebug().noquote() << "Expected: ";
            qDebug().noquote() << exp.precomputedHistogram.to_string();
            qDebug().noquote() << "Actual: ";

            TaskBuilder oTaskBuilder;

            // create histogram file using module under test:
            oTaskBuilder.createHistogram(exp.imageFilePath,"testOutput.txt");

            // prevent test framework from proceeding while previous test is being run.
            while (oTaskBuilder.is_busy) ;

            // create histogram from MUT's output file:
            Histogram actualHistogram("testOutput.txt");

            qDebug().noquote() << actualHistogram.to_string();
            bool passed = HistogramCompareTool::compare_histograms(exp.precomputedHistogram, actualHistogram);

            if (passed)
                totalpass++;
            else
                totalfail++;

        }

        for (TestExpectation exp: testCaseExpectationIntegratedTestPatternType)
        {
            qDebug().noquote() << "Testing: ";
            qDebug().noquote() << exp.imageFilePath;
            qDebug().noquote() << "Expected: ";
            qDebug().noquote() << exp.precomputedHistogram.to_string();

            // create histogram using module under test:
            TaskBuilder oTaskBuilder;
            try
            {
                oTaskBuilder.createHistogram(exp.imageFilePath,"testoutput.txt");
            }
            catch (...)
            {
                totalfail++;
                continue;
            }

            // prevent reading output file until current has completed.
            while (oTaskBuilder.is_busy) ;

            Histogram actualHistogram("testoutput.txt");

            qDebug().noquote() << "Actual: ";
            qDebug().noquote() << actualHistogram.to_string();
            bool passed = HistogramCompareTool::compare_histograms(exp.precomputedHistogram, actualHistogram);
            if (passed)
                totalpass++;
            else
                totalfail++;
        }

        qInfo() << " ********************************************";
        qInfo() << " **** Automated Integration Test results ****";
        qInfo() << " ********************************************";
        qInfo() << " Total Tests:  " << totalpass+totalfail;
        qInfo() << " Passed Tests: " << totalpass;
        qInfo() << " Failed Tests: " << totalfail;

    }
    else
    {
        parser.showHelp();
    }

    //return app.exec();
return 0;

}

// uchar must be converted to vector for autoptr
