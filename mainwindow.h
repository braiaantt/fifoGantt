#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include "process.h"

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

    QStringList categories;
    QVector<QColor> colors;
    int x;
    int sliderValue;
    QLineSeries* currentLineSeries;
    QLineSeries* lineSeriesInputOutputOne;
    QLineSeries* lineSeriesInputOutputTwo;
    QVector<std::shared_ptr<Process>> processes;
    QVector<std::shared_ptr<Process>> readyQueue;
    QVector<std::shared_ptr<Process>> inputOutputOne;
    QVector<std::shared_ptr<Process>> inputOutputTwo;
    QVector<std::shared_ptr<Process>> arrivalProcesses;

    void updateCoords(int x, int y);
    void createLineSeries();
    void verticalLineSeries(int lastY);
    void markProcessKilled();

    void initCoreGraphic();
    void initInputOutputGraphic();
    void initAxis(int &axisXCount);
    void initLegends();

    void processing();
    void checkArrivalProcesses();
    void checkArrivalInputOutputProcesses(QVector<std::shared_ptr<Process>> &, int);
    void sortArrivalProcesses();
    void createProcesses();
    void paintIoChart();
    void paintCoreChart();
    void createIoChartLineSeries(int);

};
#endif // MAINWINDOW_H
