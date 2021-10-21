#include "cpuusagewidget.h"
#include <QLayout>
#include <QDebug>

CpuUsageWidget::CpuUsageWidget(QWidget *parent, int core) :
    QWidget(parent),
    m_view(new QtCharts::QChartView(this)),
    m_chart(new QtCharts::QChart),
    m_axisX(new QtCharts::QValueAxis(this)),
    m_axisY(new QtCharts::QValueAxis(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    this->setLayout(layout);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setChart(m_chart);
    m_chart->legend()->hide();

    m_axisX->setRange(0, CpuUsageWidget::xCount);
    m_axisY->setRange(0.0, 1.0);
    m_axisX->setGridLineVisible(true);
    m_axisY->setGridLineVisible(true);

    for (int i = 0; i < core; i++)
    {
        auto series = new QtCharts::QSplineSeries(this);
        m_chart->addSeries(series);
        m_chart->setAxisX(m_axisX, series);
        m_chart->setAxisY(m_axisY, series);
        m_seriesList.push_back(series);

        QQueue<double> data;
        for (uint i = 0; i <= CpuUsageWidget::xCount; i++)
            data.enqueue(0);
        m_data.push_back(data);
    }
}

QSize CpuUsageWidget::sizeHint() const
{
    return QSize{600, 400};
}

void CpuUsageWidget::append(double usage, QList<double> subUsages)
{
    m_chart->setTitle(QString::asprintf("CPU(%.2f%%)", usage * 100));

    for (int i = 0; i < subUsages.length(); i++)
    {
        m_data[i].dequeue();
        m_data[i].enqueue(subUsages[i]);
        m_seriesList[i]->clear();
        for (int x = 0; static_cast<uint>(x) <= CpuUsageWidget::xCount; x++)
        {
            m_seriesList[i]->append(x, m_data[i][x]);
        }
    }
}
