#ifndef CPUWIDGET_H
#define CPUWIDGET_H

#include <QWidget>
class CpuWidget : public QObject
{
    Q_OBJECT
public:
    explicit CpuWidget(QWidget *parent = nullptr);

    static QStringList cpus();
    static QList<uint> freq(const char* path);
    static QList<uint> curFreq();
    static QList<uint> minFreq();
    static QList<uint> maxFreq();
    static QString readableFreq(uint freq);

signals:

public slots:

private:
    static constexpr const char* cpuDirPath = "/sys/devices/system/cpu";
    static constexpr const char* curFreqFilePath = "cpufreq/scaling_cur_freq";
    static constexpr const char* minFreqFilePath = "cpufreq/scaling_min_freq";
    static constexpr const char* maxFreqFilePath = "cpufreq/scaling_max_freq";
    static constexpr const char* curModeFilePath = "cpufreq/scaling_governor";
    static constexpr const char* supModesFilePath = "cpufreq/scaling_available_governors";
};

#endif // CPUWIDGET_H
