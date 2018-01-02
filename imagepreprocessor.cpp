#include <QImage>
#include <QFile>
#include <limits>
#include "imagepreprocessor.h"



ImagePreprocessor::ImagePreprocessor(QImage& img)
{
    //img = img.convertToFormat(QImage::Format_RGBA8888_Premultiplied);
    uchar *bits = img.bits();
    qsizetype size = img.sizeInBytes();


    qsizetype maxsize = std::numeric_limits<int>::max();
    if (size > maxsize)
    {
           // vector is limited to MAX_INT
        size = 0;
        bits = nullptr;
    }


    m_imageSize = size;
    m_imageBits = bits;
}

unsigned char* ImagePreprocessor::getBits()
{
    return m_imageBits;
}
unsigned int ImagePreprocessor::getSize()
{
    return m_imageSize;
}
