#ifndef TESTEXPECTATION_H
#define TESTEXPECTATION_H

#include <QString>
#include "histogram.h"

#define DIR_TEST ("./testframework/tests")
#define DIR_TEST_INTEGRATEDTESTCASES_PATTERN ("/integrated/01_test_pattern")
#define DIR_TEST_INTEGRATEDTESTCASES_PRECOMPUTED ("/integrated/02_test_precomputed")

// A class containing information for one testcase.

class TestExpectation
{
public:
    /* File path pointing to image where histogram data is being taken from */
    QString imageFilePath;
    /* Histogram, pre-computed and used as test expectations */
    Histogram precomputedHistogram;

    TestExpectation() {}
};


#endif // TESTEXPECTATION_H
