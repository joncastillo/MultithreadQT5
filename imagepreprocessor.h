#ifndef IMAGEPREPROCESSOR_H
#define IMAGEPREPROCESSOR_H

#include <QFile>
#include <QTextStream>
#include "dispatcher.h"
#include "message.h"

// The ImagePreprocessor class converts an image file into RGB bits and sends them to the Dispatcher.
// Once the dispatcher has completed its task, it returns a message to the ImageProcessor containing
// the consolidated RGB count of each worker thread.

// main --> (imagePreprocessor) --> Dispatcher --> Worker Threads

class ImagePreprocessor
{
public:
    ImagePreprocessor(QImage& img);
    unsigned char* getBits();
    unsigned int getSize();

private:

    unsigned int m_imageSize;
    uchar* m_imageBits;

    QFile imagefileHandle;
    QFile outfileHandle;

};

#endif // IMAGEPREPROCESSOR_H
