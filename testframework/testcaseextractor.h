#ifndef TESTCASEEXTRACTOR_H
#define TESTCASEEXTRACTOR_H

#include "testexpectation.h"

/* This helper class parses information from the test folder which pertain to testcases.
 * The said information are embedded as filenames for pattern-based tests and inside csv files for precomputed expectations.
 * Precomputations for the histograms were done with the aid of a Photoshop script found here:https://forums.adobe.com/message/7246695.
 */
class TestcaseExtractor
{
public:
    /* Test expectations are retrieved from filenames for pattern based tests */
    static void obtainIntegratedTestCasesPatternType(QVector<TestExpectation>& out);
    /* Test expectations are retrieved from csv files generated via photoshop script */
    static void obtainIntegratedTestCasesPrecomputedType(QVector<TestExpectation>& out);
    static void obtainTestCases(QVector<TestExpectation>& out1, QVector<TestExpectation>& out2);

private:
    TestcaseExtractor();
    virtual ~TestcaseExtractor();
};

#endif // TESTCASEEXTRACTOR_H
