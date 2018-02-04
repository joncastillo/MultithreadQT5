#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QVector>

class Histogram
{
public:
    Histogram();
    void clear();
    QVector<int> m_r;
    QVector<int> m_g;
    QVector<int> m_b;
};

#endif // HISTOGRAM_H
