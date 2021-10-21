#include "mainwindow.h"
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_cpuModel(new CpuModel(this)),
    m_cpuUsageWidget(new CpuUsageWidget(this, CpuModel::cpuFiles().length()))
{
    setCentralWidget(m_cpuUsageWidget);

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, [this](){
        m_cpuUsageWidget->append(m_cpuModel->usageRatio(), m_cpuModel->subUsageRatio());
    });
    timer->start();
}

MainWindow::~MainWindow()
{

}
