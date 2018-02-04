#include "histogram.h"

Histogram::Histogram()
    : m_r(256),
      m_g(256),
      m_b(256)
{

}

void Histogram::clear()
{
    m_r.fill(0,256);
    m_g.fill(0,256);
    m_b.fill(0,256);
}
