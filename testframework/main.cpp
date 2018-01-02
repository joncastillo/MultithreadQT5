#include <iostream>

#include <QCoreApplication>
#include <QFile>

#include "testcaseextractor.h"
#include "testexpectation.h"
#include "histogram.h"
#include "histogramcomparetool.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVector<TestExpectation> testListPatternType;
    QVector<TestExpectation> testListPreComputedType;
    TestcaseExtractor::obtainTestCases(testListPatternType,testListPreComputedType);

    for (auto oTestExpectation : testListPatternType)
    {

        QFile testImage(oTestExpectation.imageFilePath);
        testImage.open(QIODevice::ReadOnly);
        //todo: not implemented: histogramTool
#if 0
        Histogram out = histogramTool(testImage);

        if (!HistogramCompareTool::compare_histograms(out,oTestExpectation.precomputedHistogram))
        {
            // test failed!
        }

#endif
    }

    for (auto oTestExpectation : testListPreComputedType)
    {
        QFile testImage(oTestExpectation.imageFilePath);
        testImage.open(QIODevice::ReadOnly);
        //todo: not implemented: histogramTool
#if 0
        Histogram out = histogramTool(testImage);

        if (!HistogramCompareTool::compare_histograms(out,oTestExpectation.precomputedHistogram))
        {
            // test failed!
        }
#endif

    }

    return a.exec();
}


//todo 5millionx5million pixel photo with 3 RGB dots.
