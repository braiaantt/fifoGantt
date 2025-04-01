#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include "processmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    QChart *chart;
    QChart *chartInputOutput;
    QValueAxis *axisX;
    QBarCategoryAxis *axisY;
    QValueAxis *axisXInputOutput;
    QBarCategoryAxis *axisYInputOutput;
    ProcessManager processManager;

    QVector<QColor> colors;
    int x;
    int sliderValue;
    QLineSeries* currentLineSeries;
    QLineSeries* lineSeriesInputOutputOne;
    QLineSeries* lineSeriesInputOutputTwo;

    void updateCoords(int x, int y);
    void createLineSeries();
    void verticalLineSeries(int lastY);
    void markProcessKilled();

    void initCoreGraphic();
    void initInputOutputGraphic();
    void initAxis(int &axisXCount);
    void initLegends();

    void processing();
    void paintIoChart();
    void paintCoreChart();
    void createIoChartLineSeries(int);

};
#endif // MAINWINDOW_H
