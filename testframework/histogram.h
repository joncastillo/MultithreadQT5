#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QVector>
#include <QTextStream>
#include <QMessageLogger>
#include <QDebug>
#include <QFile>

/* The Histogram class contains vectors for each color channel. Their indeces pertain to color levels of an 8-bit RGB image*/

class Histogram
{
public:
    // Default Constructor
    Histogram() : m_r(256),m_g(256),m_b(256) {}

    Histogram(QString strFilename)
    {
        QFile inFile(strFilename);
        inFile.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream instream(&inFile);

        QString line;
        line = instream.readAll();
        QStringList channels = line.split("\n");

        QString* redChannel = &(channels[0]);
        QStringList redChannelValues = redChannel->split(",");
        QString* greenChannel = &(channels[1]);
        QStringList greenChannelValues = greenChannel->split(",");
        QString* blueChannel = &(channels[2]);
        QStringList blueChannelValues = blueChannel->split(",");

        for (QString value: redChannelValues)
        {
            m_r.push_back(value.toInt());
        }

        for (QString value: greenChannelValues)
        {
            m_g.push_back(value.toInt());
        }

        for (QString value: blueChannelValues)
        {
            m_b.push_back(value.toInt());
        }
    }


    // Assignment operator
    void operator=(const Histogram& rhs) {
        m_r = rhs.m_r;
        m_g = rhs.m_g;
        m_b = rhs.m_b;
    }
    // The red channel
    QVector<int> m_r;
    // The green channel
    QVector<int> m_g;
    // The blue channel
    QVector<int> m_b;

    QString to_string()
    {
        QString out;

        for (int i = 0; i < 256 ; i++)
        {
            out.append(QString::number(m_r[i]));
            if (i != 255)
                out.append(",");
        }
        out.append("\n");

        for (int i = 0; i < 256 ; i++)
        {
            out.append(QString::number(m_g[i]));
        if (i != 255)
            out.append(",");
        }
        out.append("\n");


        for (int i = 0; i < 256 ; i++)
        {
            out.append(QString::number(m_b[i]));
                        if (i != 255)
            out.append(",");
        }
        out.append("\n");

        return out;
    }

};

#endif // HISTOGRAM_H
