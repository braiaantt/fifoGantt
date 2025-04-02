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

private slots:
    void on_pushButtonInitCharts_clicked();

    void on_pushButtonRestartCharts_clicked();

    void on_pushButtonAddProcess_clicked();

private:
    Ui::MainWindow *ui;
    QChart* chart;
    QChart * chartInputOutput;
    QChartView* chartView;
    QChartView* chartViewInputOutput;
    QGraphicsScene* scene;
    QGraphicsScene* sceneInputOutput;
    QValueAxis *axisX;
    QBarCategoryAxis *axisY;
    QValueAxis *axisXInputOutput;
    QBarCategoryAxis *axisYInputOutput;
    ProcessManager processManager;
    bool initButtonClicked;

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
    void initAxis();
    void initLegends();
    int calcAxisCount();

    void processing();
    void paintIoChart();
    void paintCoreChart();
    void createIoChartLineSeries(int);

    bool checkInputsTableWidget();
    bool checkIoInputs();

};
#endif // MAINWINDOW_H
