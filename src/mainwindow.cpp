#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_cpuWidget(new CpuWidget(this))
{

}

MainWindow::~MainWindow()
{

}
