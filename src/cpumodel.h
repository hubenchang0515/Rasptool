#ifndef CPUWIDGET_H
#define CPUWIDGET_H

#include <QWidget>
#include <QMap>

class CpuModel : public QObject
{
    Q_OBJECT
public:
    explicit CpuModel(QObject *parent = nullptr);

    static QStringList cpuFiles();

    QString name();
    QList<uint> curFreq();
    QList<uint> minFreq();
    QList<uint> maxFreq();
    double usageRatio();
    QList<double> subUsageRatio();

signals:

public slots:

private:
    static constexpr const char* cpuDirPath = "/sys/devices/system/cpu";
    static constexpr const char* curFreqFilePath = "cpufreq/scaling_cur_freq";
    static constexpr const char* minFreqFilePath = "cpufreq/scaling_min_freq";
    static constexpr const char* maxFreqFilePath = "cpufreq/scaling_max_freq";
    static constexpr const char* curModeFilePath = "cpufreq/scaling_governor";
    static constexpr const char* supModesFilePath = "cpufreq/scaling_available_governors";

    static constexpr const char* cpuStatFilePath = "/proc/stat";
    static constexpr const char* cpuInfoFilePath = "/proc/cpuinfo";

    static QList<uint> freq(const char* path);
    static QString readableFreq(uint freq);
    static QList<QMap<QString, QString>> readCpuInfo();
    static QList<uint> readCpuStat(uint core=0);
    static QList<QList<uint>> readSubCpuStats();
    static double ratio(const QList<uint>& s0, const QList<uint>& s1);

    QList<QMap<QString, QString>> m_cpuInfo;
    QList<uint> m_cpuStat;
    QList<QList<uint>> m_subCpuStats;


};

#endif // CPUWIDGET_H
