#ifndef HISTOGRAMCOMPARETOOL_H
#define HISTOGRAMCOMPARETOOL_H

// This helper class contains utilities that compares two histograms.

class HistogramCompareTool
{
public:
    // compare_histograms takes in two histograms and checks whether they are identical.
    // algorithm works by taking chunks of 'color levels' of size <meanSampleSet> from each of the two histograms.
    // these chunks are averaged and checked whether they are within the values specified by the parameter 'tolerance'
    // the test is repeated on every possible index of the histograms.
    //
    // in: expected - expected histogram values. usually precomputed.
    // in: actual - histogram of the same image derived by the module under test.
    // in: meanSampleSet - size of test chunk of color levels. (optional, default: 5 samples)
    // in: tolerance - acceptable deviation in color value amplitude from expected. (optional, default: 5%)
    //
    // returns: true if two histograms are considered identical. false if otherwise.
    static bool compare_histograms(Histogram& expected, Histogram& actual, int meanSampleSet=20, double tolerance=0.05);

private:
    HistogramCompareTool();
    virtual ~HistogramCompareTool();
};

#endif // HISTOGRAMCOMPARETOOL_H
