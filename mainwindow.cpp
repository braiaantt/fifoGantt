#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QScrollBar"
#include "QTimer"
#include "QLegendMarker"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    x = 0;
    y = 0;
    sliderValue = 14;

    chart = new QChart();
    chartView = new QChartView(chart);
    scene = new QGraphicsScene();
    axisX = new QValueAxis();
    axisY = new QBarCategoryAxis();
    colors = {Qt::red, Qt::blue, Qt::green, Qt::yellow};

    chart->setTitle("Puto el que lee");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Configura el eje X
    axisX->setRange(0, 25);
    axisX->setTickCount(26);
    axisX->setLabelFormat("%.0f"); // Formato de las etiquetas
    axisX->setTitleText("Tiempo (s)");
    axisX->setGridLineVisible(false);
    chart->addAxis(axisX, Qt::AlignBottom);

    // Configura el eje Y (con categorías)
    QStringList categories = {"A", "B", "C", "D"};
    axisY->append(categories);
    axisY->setGridLineVisible(false);
    chart->addAxis(axisY, Qt::AlignLeft);

    for(int i = 0; i<categories.size(); i++){
        QLineSeries *legendSeries = new QLineSeries();
        chart->addSeries(legendSeries);
        legendSeries->setName(categories[i]);
        legendSeries->setColor(colors[i]);
    }

    createLineSeries();

    chart->setMinimumWidth(axisX->tickCount()*50);

    // Configura la vista y la escena
    chartView->setRenderHint(QPainter::Antialiasing);
    scene->addWidget(chartView);
    ui->graphicsView->setScene(scene);
    chartView->setGeometry(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    setNewLineSeries(x,y);

    QTimer::singleShot(0, this,[this](){
        ui->graphicsView->horizontalScrollBar()->setValue(0);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event){

    if (event->key() == Qt::Key_Up) {
        y++;
        if(y >= 0 && y < 4){
            createLineSeries();
        } else {
            qDebug()<<"Value Y not in range: "<<y;
        }
    }
    else if (event->key() == Qt::Key_Down) {
        y--;
        if(y > 0){

            if(y >= 0 && y < 4){
                createLineSeries();
            } else {
                qDebug()<<"Value Y not in range: "<<y;
            }
        }
    }
    else if (event->key() == Qt::Key_Left) {
        if(x>0){
            x--;
        }
    }
    else if (event->key() == Qt::Key_Right) {
        x++;
        if(x == sliderValue){
            ui->graphicsView->horizontalScrollBar()->setValue(ui->graphicsView->horizontalScrollBar()->value() + 50);
            sliderValue++;
        }
    }

    setNewLineSeries(x,y);

    // Llama al comportamiento por defecto
    QMainWindow::keyPressEvent(event);

}

void MainWindow::setNewLineSeries(int &x, int &y){

    currentLineSeries->append(x,y);

}

void MainWindow::createLineSeries(){

    QLineSeries* lineSeries = new QLineSeries();
    chart->addSeries(lineSeries);
    lineSeries->attachAxis(axisY);
    lineSeries->attachAxis(axisX);
    series << lineSeries;
    currentLineSeries = lineSeries;

    if(y >= 0 && y < 4){
        lineSeries->setColor(colors[y]);
    }

    auto markers = chart->legend()->markers(lineSeries);
    if (!markers.isEmpty()) {
        markers.first()->setVisible(false); // Oculta el marcador en la leyenda
    }

}

QLineSeries* MainWindow::getCurrentLineSeries(int &yValue){
    return series[yValue];
}
