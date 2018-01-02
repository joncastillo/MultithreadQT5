#include <QDebug>
#include <cmath>
#include "histogram.h"

#include "histogramcomparetool.h"

bool HistogramCompareTool::compare_histograms(Histogram& expected, Histogram& actual, int meanSampleSet, double tolerance)
{
#if 0
    int highest = 0;

    // get the highest amplitude in expected:
    for (int i = 0; i < 256 ; i++)
    {
        if (expected.m_r[i] > highest)
            highest = expected.m_r[i];
        if (expected.m_g[i] > highest)
            highest = expected.m_g[i];
        if (expected.m_b[i] > highest)
            highest = expected.m_b[i];
    }

    int toleranceRed = floor(highest * tolerance);
    int toleranceGreen = floor(highest * tolerance);
    int toleranceBlue = floor(highest * tolerance);

    qInfo() << "------------------------------";
    qInfo() << "Peak at" << highest;
#else
    int totalRed = 0;
    int totalGreen = 0;
    int totalBlue = 0;

    for (int i = 0; i < 256 ; i++)
    {
            totalRed += expected.m_r[i];
            totalGreen += expected.m_g[i];
            totalBlue += expected.m_b[i];
    }

    int toleranceRed = ceil((double)totalRed /256);
    int toleranceGreen = ceil((double)totalGreen /256);
    int toleranceBlue = ceil((double)totalBlue /256);

    qInfo() << "------------------------------";
    //qInfo() << "Peaks at R" << highestRed << " G" <<highestGreen << " B" <<highestBlue;
#endif

    qInfo() << "Test tolerance set at " << tolerance * 100 << "%";
    qInfo() << "------------------------------";
    qInfo() << "Red +/- tolerance computed at " << toleranceRed << "units.";

    qInfo() << "Green +/- tolerance computed at " << toleranceGreen << "units.";

    qInfo() << "Blue +/- tolerance computed at " << toleranceBlue << "units.";
    qInfo() << "------------------------------";

    int discrepancy_count = 0;
    int sample_count = 0;

    for (int i = 0 ; i < (256 - meanSampleSet + 1) ; i++)
    {
        sample_count++;
        // obtain average
        unsigned int totalRedExpected = 0;
        unsigned int totalGreenExpected = 0;
        unsigned int totalBlueExpected = 0;

        unsigned int totalRedActual = 0;
        unsigned int totalGreenActual = 0;
        unsigned int totalBlueActual = 0;

        for (int j = 0 ; j <  meanSampleSet ; j++)
        {
            totalRedExpected += expected.m_r[i+j];
            totalGreenExpected += expected.m_g[i+j];
            totalBlueExpected += expected.m_b[i+j];

            totalRedActual += actual.m_r[i+j];
            totalGreenActual += actual.m_g[i+j];
            totalBlueActual += actual.m_b[i+j];
        }
        bool discrepancyInOneChannel = false;
            // test red channel
            double meanRedExpected = totalRedExpected / (double)meanSampleSet;
            double meanRedActual = totalRedActual / (double)meanSampleSet;

            double lowerToleranceRed = meanRedExpected - toleranceRed;
            double upperToleranceRed = meanRedExpected + toleranceRed;

            if (meanRedActual < lowerToleranceRed || meanRedActual > upperToleranceRed)
                discrepancyInOneChannel = true;

            // test green channel
            double meanGreenExpected = totalGreenExpected / (double)meanSampleSet;
            double meanGreenActual = totalGreenActual / (double)meanSampleSet;

            double lowerToleranceGreen = meanGreenExpected - toleranceGreen;
            double upperToleranceGreen = meanGreenExpected + toleranceGreen;

            if (meanGreenActual < lowerToleranceGreen || meanGreenActual > upperToleranceGreen)
                discrepancyInOneChannel = true;

            // test blue channel
            double meanBlueExpected = totalBlueExpected / (double)meanSampleSet;
            double meanBlueActual = totalBlueActual / (double)meanSampleSet;

            double lowerToleranceBlue = meanBlueExpected - toleranceBlue;
            double upperToleranceBlue = meanBlueExpected + toleranceBlue;

            if (meanBlueActual < lowerToleranceBlue || meanBlueActual > upperToleranceBlue)
                discrepancyInOneChannel = true;

            if (discrepancyInOneChannel)
                discrepancy_count++;
    }

    qInfo() << "number of discrepancies (mean-of-means): " << discrepancy_count;
    qInfo() << "number of samples: " << sample_count;

    if (discrepancy_count > (tolerance*sample_count))
    {
        qInfo() << "too many discrepancies! test failed. ";
        qInfo();
        qInfo();
        return false;
    }
    else
    {
        qInfo() << "test passed. ";
        qInfo();
        qInfo();
        return true;
    }
}
