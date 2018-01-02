#include <QDir>
#include <QFileInfoList>
#include <QVector>
#include <QRegularExpression>
#include <QTextStream>

#include "testcaseextractor.h"

void TestcaseExtractor::obtainIntegratedTestCasesPatternType(QVector<TestExpectation>& out)
{
    QDir dirTestRoot(DIR_TEST);
    QDir dirTestIntegratedTest01(dirTestRoot.absolutePath() + DIR_TEST_INTEGRATEDTESTCASES_PATTERN);

    QFileInfoList dirTestList = dirTestIntegratedTest01.entryInfoList();
    for (QFileInfoList::iterator it = dirTestList.begin(); it != dirTestList.end(); ++it)
    {
        QDir subdir(it->filePath());
        QFileInfoList dirSubTestList = subdir.entryInfoList();

        for (QFileInfoList::iterator it2 = dirSubTestList.begin(); it2 != dirSubTestList.end(); ++it2)
        {
            QString filename = it2->fileName();
            if ( it2->fileName().endsWith(".jpg") ||
                 it2->fileName().endsWith(".tif") ||
                 it2->fileName().endsWith(".gif") ||
                 it2->fileName().endsWith(".bmp") ||
                 it2->fileName().endsWith(".png"))
            {

                //std::cout << "Reading image: " << it2->fileName().toStdString() << std::endl;

                TestExpectation oTestInfo;
                QString filepath = it2->absoluteFilePath();
                QString baseFilename = it2->baseName();
                oTestInfo.imageFilePath = filepath;

                Histogram precomputedHistogram;

                QRegularExpression re("^(\\d+)((?:_(?:\\d+(?:[RGB]\\d+)*))*)");
                QRegularExpressionMatch match = re.match(baseFilename);
                if ( !match.hasMatch())
                    continue;
                QString matchgroup0 = match.captured(0);
                QString matchgroup1 = match.captured(1);
                QString matchgroup2 = match.captured(2);

                int rows = matchgroup1.toInt();

                QRegularExpression re2("(\\d+)(?:R(\\d{3})(\\d{3})?)?(?:G(\\d{3})(\\d{3})?)?(?:B(\\d{3})(\\d{3})?)?");
                // Group 1: Number of columns
                // Group 2: Red Channel Gradient 0
                // Group 3: Red Channel Gradient 1
                // Group 4: Green Channel Gradient 0
                // Group 5: Green Channel Gradient 1
                // Group 6: Blue Channel Gradient 0
                // Group 7: Blue Channel Gradient 1

                QRegularExpressionMatchIterator i = re2.globalMatch(matchgroup2);
                while (i.hasNext()) {
                        QRegularExpressionMatch match2 = i.next();

                        QString col = match2.captured(1);
                        QString R_0 = match2.captured(2);
                        QString R_1 = match2.captured(3);
                        QString G_0 = match2.captured(4);
                        QString G_1 = match2.captured(5);
                        QString B_0 = match2.captured(6);
                        QString B_1 = match2.captured(7);

                        int icol = col.toLong();
                        int iR_0 = R_0.toInt();
                        int iR_1 = R_1.toInt();
                        int iG_0 = G_0.toInt();
                        int iG_1 = G_1.toInt();
                        int iB_0 = B_0.toInt();
                        int iB_1 = B_1.toInt();

                        float red = (float)iR_0;
                        float green = (float)iG_0;
                        float blue = (float)iB_0;

                        if (R_0 == "")
                        {
                            iR_0 = 0;
                            R_0 = "0";
                        }

                        if (G_0 == "")
                        {
                            iG_0 = 0;
                            G_0 = "0";
                        }
                            if (B_0 == "")
                            {
                                iB_0 = 0;
                                B_0 = "0";
                            }


                        float r_step = 0;
                        if ( (R_1 != "") && (R_0 != R_1) )
                        {
                                // red channel is a gradient
                                // calculate step
                            if (iR_1 > iR_0)
                                r_step = (iR_1 - iR_0 + 1) / (float)icol;
                            else
                                r_step = (iR_1 - iR_0 - 1) / (float)icol;

                        }

                        float g_step = 0;
                        if ( (G_1 != "") && (G_0 != G_1) )
                        {
                                // green channel is a gradient
                                // calculate step
                            if (iG_1 > iG_0)
                                g_step = (iG_1 - iG_0 + 1) / (float)icol;
                            else
                                g_step = (iG_1 - iG_0 - 1) / (float)icol;
                        }

                        float b_step = 0;
                        if ( (B_1 != "") && (B_0 != B_1) )
                        {
                                // blue channel is a gradient
                                // calculate step
                            if (iB_1 > iB_0)
                                b_step = (iB_1 - iB_0 + 1) / (float)icol;
                            else
                                b_step = (iB_1 - iB_0 - 1) / (float)icol;
                        }




                       // if (r_step || g_step || b_step)
                        {
                            // a gradient
                            for (int i = 0; i < icol ; i++ )
                            {
                                precomputedHistogram.m_r[static_cast<int>(red+0.5)]+=rows;
                                precomputedHistogram.m_g[static_cast<int>(green+0.5)]+=rows;
                                precomputedHistogram.m_b[static_cast<int>(blue+0.5)]+=rows;

                                red+=r_step;
                                green+=g_step;
                                blue+=b_step;
                            }
                        }
                }
                oTestInfo.precomputedHistogram = precomputedHistogram;
                out.push_back(oTestInfo);
            }
        }
    }
}

// TODO: This didn't fare well. The histograms generated by photoshop are too different than the ones from QImage::bits
void TestcaseExtractor::obtainIntegratedTestCasesPrecomputedType(QVector<TestExpectation>& out)
{
    QDir dirTestRoot(DIR_TEST);
    QDir dirTestIntegratedTest02(dirTestRoot.absolutePath() + DIR_TEST_INTEGRATEDTESTCASES_PRECOMPUTED);

    QFileInfoList dirTestList = dirTestIntegratedTest02.entryInfoList();
    for (auto oFileInfo : dirTestList)
    {

        QString folder = oFileInfo.absoluteFilePath();
        // . .. 1 2 3 4 5
        QDir subdir(oFileInfo.filePath());
        QFileInfoList dirSubTestList = subdir.entryInfoList();

        for (auto oFileInfo2 : dirSubTestList)
        {
            // 1 2 3 4 5
            QString filename = oFileInfo2.fileName();


            if ( oFileInfo2.fileName().endsWith(".jpg") ||
                 oFileInfo2.fileName().endsWith(".tif") ||
                 oFileInfo2.fileName().endsWith(".gif") ||
                 oFileInfo2.fileName().endsWith(".bmp") ||
                 oFileInfo2.fileName().endsWith(".png"))
            {

                //std::cout << "Reading image: " << oFileInfo2.fileName().toStdString() << std::endl;

                TestExpectation oPreComputedType;
                oPreComputedType.imageFilePath = oFileInfo2.absoluteFilePath();

                QFile csvFile(oFileInfo2.absolutePath()+"/Histogram.csv");
                if (csvFile.open(QIODevice::ReadOnly))
                {

                    Histogram precomputedHistogram;
                    QTextStream in(&csvFile);
                    while (!in.atEnd())
                    {
                        QString line = in.readLine();
                        QRegularExpression regex("^(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)$");
                        QRegularExpressionMatch match = regex.match(line);

                        if( match.hasMatch())
                        {
                            int level = match.captured(1).toInt();
                            int r = match.captured(3).toInt();
                            int g = match.captured(4).toInt();
                            int b = match.captured(5).toInt();

                            precomputedHistogram.m_r[level] = r;
                            precomputedHistogram.m_g[level] = g;
                            precomputedHistogram.m_b[level] = b;
                        }
                    }

                    oPreComputedType.precomputedHistogram = precomputedHistogram;
                    out.push_back(oPreComputedType);

                }
            }
        }
    }
}

void TestcaseExtractor::obtainTestCases(QVector<TestExpectation>& out1, QVector<TestExpectation>& out2)
{
    obtainIntegratedTestCasesPatternType(out1);


    // TODO: This didn't fare well. The histograms generated by photoshop are too different than the ones from QImage::bits
    obtainIntegratedTestCasesPrecomputedType(out2);
}
