#include "cpumodel.h"
#include <QDebug>
#include <QFile>
#include <QDir>

CpuWidget::CpuWidget(QWidget *parent) : QObject(parent)
{
    qDebug() << cpus() << minFreq() << curFreq() << maxFreq();
}


QStringList CpuWidget::cpus()
{
    static QStringList cpuList;

    if (cpuList.length() > 0)
    {
        return cpuList;
    }

    QDir cpuDir{CpuWidget::cpuDirPath};
    QString presentPath = cpuDir.absoluteFilePath("present");
    QFile presentFile{presentPath};

    if (!presentFile.open(QIODevice::ReadOnly))
    {
        return cpuList;
    }

    QString data = presentFile.readAll();
    QStringList range = data.split("-");

    if (range.length() != 2)
    {
        return cpuList;
    }

    uint min = range[0].toUInt();
    uint max = range[1].toUInt();

    for (uint i = min; i <= max; i++)
    {
        cpuList.push_back(cpuDir.absoluteFilePath(QString::asprintf("cpu%u", i)));
    }
    return cpuList;
}

QList<uint> CpuWidget::freq(const char* path)
{
    QList<uint> curFreqList;
    QStringList cpus = CpuWidget::cpus();
    for (auto& cpu : cpus)
    {
        QFileInfo cpuFreqFileInfo(cpu, path);
        QFile cupFreqFile(cpuFreqFileInfo.absoluteFilePath());
        if (!cupFreqFile.open(QIODevice::ReadOnly))
        {
            curFreqList.push_back(0);
            continue;
        }

        QString data = cupFreqFile.readAll();
        curFreqList.push_back(data.toUInt());
    }

    return curFreqList;
}

QList<uint> CpuWidget::curFreq()
{
    return CpuWidget::freq(CpuWidget::curFreqFilePath);
}

QList<uint> CpuWidget::minFreq()
{
    return CpuWidget::freq(CpuWidget::minFreqFilePath);
}

QList<uint> CpuWidget::maxFreq()
{
    return CpuWidget::freq(CpuWidget::maxFreqFilePath);
}

QString CpuWidget::readableFreq(uint freq)
{
    if (freq > 1000000)
    {
        return QString::asprintf("%.2f GHz", static_cast<double>(freq) / 1000000.0);
    }

    if (freq > 1000)
    {
        return QString::asprintf("%.2f MHz", static_cast<double>(freq) / 1000.0);
    }

    return QString::asprintf("%u KHz", freq);
}
