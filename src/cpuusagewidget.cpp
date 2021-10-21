#include "cpuusagewidget.h"
#include <QLayout>
#include <QDebug>

CpuUsageWidget::CpuUsageWidget(QWidget *parent, int core) :
    QWidget(parent),
    m_view(new QtCharts::QChartView(this)),
    m_chart(new QtCharts::QChart)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    this->setLayout(layout);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setChart(m_chart);
    m_chart->legend()->hide();

    for (int i = 0; i < core; i++)
    {
        auto series = new QtCharts::QSplineSeries(this);
        series->setPointsVisible(true);
        m_seriesList.push_back(series);
        m_chart->addSeries(series);

        QQueue<double> data;
        for (int i = 0; i <= CpuUsageWidget::xCount; i++)
            data.enqueue(0);
        m_data.push_back(data);
    }

    m_chart->createDefaultAxes();
    auto axisX = dynamic_cast<QtCharts::QValueAxis*>(m_chart->axisX());
    auto axisY = dynamic_cast<QtCharts::QValueAxis*>(m_chart->axisY());

    if (axisX != nullptr && axisY != nullptr)
    {
        axisX->setRange(0, CpuUsageWidget::xCount);
        axisY->setRange(0.0, 1.0);
        axisX->setGridLineVisible(true);
        axisY->setGridLineVisible(true);
        axisX->setLabelFormat(" ");
        axisY->setLabelFormat(" ");
    }
}

QSize CpuUsageWidget::sizeHint() const
{
    return QSize{320, 240};
}

void CpuUsageWidget::append(double usage, QList<double> subUsages)
{
    m_chart->setTitle(QString::asprintf("CPU(%.2f%%)", usage * 100));

    for (int i = 0; i < subUsages.length(); i++)
    {
        m_data[i].dequeue();
        m_data[i].enqueue(subUsages[i]);
        m_seriesList[i]->clear();
        for (int x = 0; x <= CpuUsageWidget::xCount; x++)
        {
            m_seriesList[i]->append(x, m_data[i][x]);
        }
    }
}
