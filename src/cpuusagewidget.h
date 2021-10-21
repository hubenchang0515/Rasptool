#ifndef CPUUSAGEWIDGET_H
#define CPUUSAGEWIDGET_H

#include <QWidget>
#include <QQueue>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>

class CpuUsageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CpuUsageWidget(QWidget *parent = nullptr, int core=1);
    virtual QSize sizeHint() const override;
    void append(double usage, QList<double> subUsages);

signals:

public slots:

private:
    static constexpr const int xCount = 60;

    QtCharts::QChartView* m_view;
    QtCharts::QChart* m_chart;
    QList<QtCharts::QLineSeries*> m_seriesList;
    QList<QQueue<double>> m_data;
};

#endif // CPUUSAGEWIDGET_H
