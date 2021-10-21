#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cpumodel.h"
#include "cpuusagewidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    CpuModel* m_cpuModel;
    CpuUsageWidget* m_cpuUsageWidget;
};

#endif // MAINWINDOW_H
