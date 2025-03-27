#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>

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
    QChartView *chartView;
    QGraphicsScene *scene;
    QValueAxis *axisX;
    QBarCategoryAxis *axisY;
    QStringList categories;
    QVector<QColor> colors;
    int x;
    int y;
    int sliderValue;
    bool verticalLineSeriesActive;
    QVector<QLineSeries*> series;
    QLineSeries* currentLineSeries;

    void setNewLineSeries(int x, int y);
    void createLineSeries();
    void createYLineSeries();
    void horizontalLineSeries();
    void verticalLineSeries();
    QLineSeries* getCurrentLineSeries(int& yValue);

};
#endif // MAINWINDOW_H
