#include "cpumodel.h"
#include <QDebug>
#include <QFile>
#include <QDir>

CpuModel::CpuModel(QObject *parent) :
    QObject(parent),
    m_cpuInfo(CpuModel::readCpuInfo()),
    m_cpuStat(CpuModel::readCpuStat()),
    m_subCpuStats(CpuModel::readSubCpuStats())

{

}

QString CpuModel::name()
{
    if (m_cpuInfo.length() == 0)
        return "";

    return m_cpuInfo[0]["model name"];
}

QStringList CpuModel::cpuFiles()
{
    static QStringList cpuList;

    if (cpuList.length() > 0)
    {
        return cpuList;
    }

    QDir cpuDir{CpuModel::cpuDirPath};
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

QList<uint> CpuModel::freq(const char* path)
{
    QList<uint> curFreqList;
    QStringList cpus = CpuModel::cpuFiles();
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

QList<uint> CpuModel::curFreq()
{
    return CpuModel::freq(CpuModel::curFreqFilePath);
}

QList<uint> CpuModel::minFreq()
{
    return CpuModel::freq(CpuModel::minFreqFilePath);
}

QList<uint> CpuModel::maxFreq()
{
    return CpuModel::freq(CpuModel::maxFreqFilePath);
}

double CpuModel::usageRatio()
{
    auto stat = CpuModel::readCpuStat();
    auto ratio = CpuModel::ratio(m_cpuStat, stat);
    m_cpuStat = stat;
    return ratio;
}

QList<double> CpuModel::subUsageRatio()
{
    QList<double> ratio;
    auto subCpuStats = CpuModel::readSubCpuStats();

    for(int i = 0; i < subCpuStats.length(); i++)
    {
        ratio.push_back(CpuModel::ratio(m_subCpuStats[i], subCpuStats[i]));
    }

    m_subCpuStats = subCpuStats;
    return ratio;
}

QString CpuModel::readableFreq(uint freq)
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

QList<QMap<QString, QString>> CpuModel::readCpuInfo()
{
    QList<QMap<QString, QString>> info;
    QFile infoFile{CpuModel::cpuInfoFilePath};
    if (!infoFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "failed to open " << CpuModel::cpuInfoFilePath;
        return info;
    }

    int index = -1;
    while (true)
    {
        QString line = infoFile.readLine();
        if (line.isEmpty() == 0)
            break;

        if (line.trimmed().isEmpty())
            continue;

        QStringList keyValue = line.split(":");
        if (keyValue.length() < 2)
        {
            qWarning() << "cpu info data is not key:value";
            continue;
        }

        QString key = keyValue[0].trimmed();
        QString value = keyValue[1].trimmed();

        if (key == "processor")
        {
            info.push_back(QMap<QString, QString>{});
            index++;
            continue;
        }

        if (index < 0)
        {
            qWarning() << "cpu info index < 0";
            continue;
        }

        info[index][key] = value;
    }

    return info;
}

QList<uint> CpuModel::readCpuStat(uint core)
{
    QList<uint> stat;
    QFile statFile{CpuModel::cpuStatFilePath};
    if (!statFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "failed to open " << CpuModel::cpuInfoFilePath;
        return stat;
    }

    for(uint i = 0; i < core; i++)
    {
        statFile.readLine();
    }

    QString line = statFile.readLine();
    QStringList times = line.split(" ", QString::SkipEmptyParts);
    for (int i = 1; i < times.length(); i++)
    {
        stat.push_back(times[i].toUInt());
    }

    return stat;
}

QList<QList<uint>> CpuModel::readSubCpuStats()
{
    QList<QList<uint>> stats;
    auto cpus = CpuModel::cpuFiles();
    for (uint i = 0; static_cast<int>(i) < cpus.length(); i++)
    {
        stats.push_back(CpuModel::readCpuStat(i+1));
    }

    return stats;
}

double CpuModel::ratio(const QList<uint>& s0, const QList<uint>& s1)
{
    // $cat /proc/stat | grep cpu
    //     #   user    nice   system idle   iowait  irq  softirq
    //    cpu  1350454 219    506756 8805863 42072  0    69204 0 0 0
    //    cpu0 317896   46    133366 3901838 20417  0    24688 0 0 0
    //    cpu1 343872   61    123997 1635209 7425   0    20978 0 0 0
    //    cpu2 339104   68    124649 1639959 6962   0    15075 0 0 0
    //    cpu3 349581   44    124743 1628855 7266   0    8462 0 0 0
    if (s0.length() < 4 || s1.length() < 4)
        return 0.0;

    double sum0 = s0[0] + s0[2] + s0[3];
    double usage0 = s0[0] + s0[2];

    double sum1 = s1[0] + s1[2] + s1[3];
    double usage1 = s1[0] + s1[2];

    return (usage1 - usage0) / (sum1 - sum0);
}
