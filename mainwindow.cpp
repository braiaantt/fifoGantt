#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QScrollBar"
#include "QTimer"
#include "QLegendMarker"
#include "QScatterSeries"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    colors = {Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::darkRed};

    createProcesses();
    x = 0;
    sliderValue = 14;

    initCoreGraphic();
    initLegends();
    createLineSeries();

    initInputOutputGraphic();
    createIoChartLineSeries(1);
    createIoChartLineSeries(0);

    QTimer::singleShot(0, this,[this](){
        ui->graphicsView->horizontalScrollBar()->setValue(0);
        ui->graphicsView_2->horizontalScrollBar()->setValue(0);
    });

}

MainWindow::~MainWindow()
{
    delete ui; 
    delete chart;
    delete chartInputOutput;
}

void MainWindow::processing(){

    qDebug()<<"";
    qDebug()<<"";
    qDebug()<<"X: "<<x;
    qDebug()<<"";

    checkArrivalProcesses();

    qDebug()<<"---------inputOutputOne---------";
    checkArrivalInputOutputProcesses(inputOutputOne, 1);
    qDebug()<<"--------------------------------";
    qDebug()<<"";
    qDebug()<<"---------inputOutputTwo---------";
    checkArrivalInputOutputProcesses(inputOutputTwo, 0);
    qDebug()<<"--------------------------------";
    qDebug()<<"";

    sortArrivalProcesses();

    if(readyQueue.empty()){
        qDebug()<<"ReadyQueue vacía!";
        return;
    }

    qDebug()<<"------ReadyQueue------";
    for(auto &process : readyQueue){
        qDebug()<<process->getName();
    }
    qDebug()<<"----------------------";
    qDebug()<<"";

    std::shared_ptr<Process> &currentProcess = readyQueue[0];

    qDebug()<<"Trabajando proceso: "<<currentProcess->getName();

    updateCoords(x,currentProcess->getAxisY());

    if(currentProcess->getCpuTime() == 0){

        qDebug()<<"Tiempo cpu terminado";
        updateCoords(x,currentProcess->getAxisY());
        verticalLineSeries();

        if(currentProcess->getIoTime() != 0){

            currentProcess->updateCpuTime();

            if(currentProcess->getIoChannel() == 1){
                qDebug()<<"Asignado a io 1";
                inputOutputOne.append(currentProcess);
                lineSeriesInputOutputOne->setColor(colors[inputOutputOne[0]->getAxisY()]);
            } else {
                qDebug()<<"Asignado a io 2";
                inputOutputTwo.append(currentProcess);
                lineSeriesInputOutputTwo->setColor(colors[inputOutputTwo[0]->getAxisY()]);
            }

            readyQueue.removeOne(currentProcess);
            currentProcess = readyQueue[0];
            qDebug()<<"Nuevo proceso: "<<currentProcess->getName();
            updateCoords(x,currentProcess->getAxisY());
            createLineSeries();
            updateCoords(x,currentProcess->getAxisY());
            currentProcess->substractTime();

        } else if(currentProcess->getIoTime() > 0){

            qDebug()<<"Proceso "<<currentProcess->getName()<<" movido al final";

            currentProcess->updateCpuTime();
            readyQueue.removeOne(currentProcess);
            readyQueue.append(currentProcess);
            updateCoords(x,currentProcess->getAxisY());
            createLineSeries();
            updateCoords(x,currentProcess->getAxisY());
            qDebug()<<"Nuevo proceso: "<<readyQueue[0]->getName();

        } else {

            qDebug()<<"Proceso muerto";
            markProcessKilled();
            readyQueue.removeAt(0);
            if(!readyQueue.empty()){
                currentProcess = readyQueue[0];
                qDebug()<<"Nuevo proceso: "<<currentProcess->getName();
                updateCoords(x,currentProcess->getAxisY());
                createLineSeries();
                updateCoords(x,currentProcess->getAxisY());
                currentProcess->substractTime();
            } else {
                qDebug()<<"No hay más procesos!";
            }

        }

    } else {
        currentProcess->substractTime();
    }

    paintIoChart();

    if(x >= sliderValue){
        ui->graphicsView->horizontalScrollBar()->setValue(ui->graphicsView->horizontalScrollBar()->value() + 50);
        ui->graphicsView_2->horizontalScrollBar()->setValue(ui->graphicsView_2->horizontalScrollBar()->value() + 50);
    }

    x++;

}

void MainWindow::paintIoChart(){

    if(!inputOutputOne.empty()){
        lineSeriesInputOutputOne->append(x, 1);
    }

    if(!inputOutputTwo.empty()){
        lineSeriesInputOutputTwo->append(x, 0);
    }

}

void MainWindow::createIoChartLineSeries(int ioChannel){

    QLineSeries* lineSeries = new QLineSeries(chartInputOutput);
    chartInputOutput->addSeries(lineSeries);
    lineSeries->attachAxis(axisYInputOutput);
    lineSeries->attachAxis(axisXInputOutput);

    if(ioChannel == 1){
        lineSeriesInputOutputOne = lineSeries;
    } else {
        lineSeriesInputOutputTwo = lineSeries;
    }

    auto markers = chartInputOutput->legend()->markers(lineSeries);
    if (!markers.isEmpty()) {
        markers.first()->setVisible(false);
    }

}

void MainWindow::checkArrivalProcesses(){

    for(auto &process : processes){
        if(process->getArrivalTime() == x){

            arrivalProcesses.append(process);
            qDebug()<<"Proceso"<< process->getName() <<"añadido a arrivalProcesses";

        }
    }

}

void MainWindow::checkArrivalInputOutputProcesses(QVector<std::shared_ptr<Process>> &ioList, int ioChannel){

    if(!ioList.empty()){

        std::shared_ptr<Process> currentProcess = ioList[0];

        qDebug()<<"Proceso: "<<currentProcess->getName();
        qDebug()<<"ioTime: "<<currentProcess->getIoTime();
        ioList[0]->substractIoTime();
        qDebug()<<"ioTimeDsp: "<<currentProcess->getIoTime();

        if(currentProcess->getIoTime() == 0){

            paintIoChart();
            qDebug()<<"Proceso "<<currentProcess->getName()<<" añadido a arrivalProcesses";
            arrivalProcesses.append(currentProcess);
            ioList.removeOne(currentProcess);

            if(ioChannel == 1){
                createIoChartLineSeries(1);
            } else {
                createIoChartLineSeries(0);
            }

        }

    } else {
        qDebug()<<"Lista vacía";
    }

}

void MainWindow::sortArrivalProcesses(){

    if(arrivalProcesses.empty()) return;

    qDebug()<<"------ArrivalProcesses antes------";
    for(auto &process : arrivalProcesses){
        qDebug()<<process->getName();
    }
    qDebug()<<"-----------------------------------";

    if(arrivalProcesses.size() == 1){

        readyQueue.append(arrivalProcesses);

    } else if(arrivalProcesses.size() > 1){

        int size = arrivalProcesses.size();
        for (int i = 0; i < size - 1; ++i) {
            for (int j = 0; j < size - i - 1; ++j) {
                if (arrivalProcesses[j]->getArrivalTime() > arrivalProcesses[j+1]->getArrivalTime()) {
                    arrivalProcesses.swapItemsAt(j, j+1);
                }
            }
        }

        readyQueue.append(arrivalProcesses);
    }

    qDebug()<<"------ArrivalProcesses despues------";
    for(auto &process : arrivalProcesses){
        qDebug()<<process->getName();
    }
    qDebug()<<"-----------------------------------";
    arrivalProcesses.clear();

}

void MainWindow::keyPressEvent(QKeyEvent* event){

    if(event->key() == Qt::Key_Right) {
        processing();
    }

    QMainWindow::keyPressEvent(event);

}

void MainWindow::initAxis(int &axisXCount){

    axisX = new QValueAxis();
    axisY = new QBarCategoryAxis();

    //Configuracion eje x
    axisX->setRange(0, axisXCount);
    axisX->setTickCount(axisXCount+1);
    axisX->setLabelFormat("%.0f");
    axisX->setTitleText("Tiempo");
    axisX->setGridLineVisible(false);

    // Configuracion eje y
    categories = {"A", "B", "C", "D", "E"};
    axisY->append(categories);
    axisY->setGridLineVisible(false);

}

void MainWindow::initCoreGraphic(){
    int axisXCount = 30; //valor calculado por el resultado de la suma de los tiempos de nucleo

    chart = new QChart();
    QChartView *chartView = new QChartView(chart);
    QGraphicsScene *scene = new QGraphicsScene(chart);

    chart->setAnimationOptions(QChart::SeriesAnimations);

    initAxis(axisXCount);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    //chart->setMinimumWidth(axisX->tickCount()*50);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(axisX->tickCount() * 50, 300);
    scene->addWidget(chartView);
    ui->graphicsView->setScene(scene);
    chartView->setGeometry(0, 0, ui->graphicsView->width(), ui->graphicsView->height());

}

void MainWindow::initLegends(){

    for(int i = 0; i<categories.size(); i++){
        QLineSeries *legendSeries = new QLineSeries(chart);
        chart->addSeries(legendSeries);
        legendSeries->setName(categories[i]);
        legendSeries->setColor(colors[i]);
    }

}

void MainWindow::verticalLineSeries(){

    createLineSeries();
    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setColor(Qt::gray);
    currentLineSeries->setPen(pen);

    updateCoords(x,readyQueue[0]->getAxisY());

}

void MainWindow::updateCoords(int x, int y){

    currentLineSeries->append(x,y);

}

void MainWindow::createLineSeries(){

    QLineSeries* lineSeries = new QLineSeries(chart);
    chart->addSeries(lineSeries);
    lineSeries->attachAxis(axisY);
    lineSeries->attachAxis(axisX);
    currentLineSeries = lineSeries;

    if(!readyQueue.empty()){
        lineSeries->setColor(colors[readyQueue[0]->getAxisY()]);
    }

    auto markers = chart->legend()->markers(lineSeries);
    if (!markers.isEmpty()) {
        markers.first()->setVisible(false);
    }

}

void MainWindow::markProcessKilled(){

    QLineSeries* point= new QLineSeries(chart);
    chart->addSeries(point);
    point->attachAxis(axisY);
    point->attachAxis(axisX);
    point->setPointsVisible(true);

    if(!readyQueue.empty()){
        point->setColor(colors[readyQueue[0]->getAxisY()]);
    }

    auto markers = chart->legend()->markers(point);
    if (!markers.isEmpty()) {
        markers.first()->setVisible(false); // Oculta el marcador en la leyenda
    }

    point->append(x,readyQueue[0]->getAxisY());

}

void MainWindow::initInputOutputGraphic(){

    chartInputOutput = new QChart();
    QChartView *chartView = new QChartView(chartInputOutput);
    QGraphicsScene *scene = new QGraphicsScene(chartInputOutput);
    axisYInputOutput = new QBarCategoryAxis(chartInputOutput);
    chartInputOutput->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categoriesInputOutput = {"I/O 1","I/O 2"};

    axisYInputOutput->append(categoriesInputOutput);
    axisYInputOutput->setGridLineVisible(false);

    axisXInputOutput = new QValueAxis(chartInputOutput);

    //Configuracion eje x
    axisXInputOutput->setRange(0, axisX->max());
    axisXInputOutput->setTickCount(axisX->max()+1);
    axisXInputOutput->setLabelFormat("%.0f");
    axisXInputOutput->setTitleText("Tiempo");
    axisXInputOutput->setGridLineVisible(false);

    chartInputOutput->addAxis(axisXInputOutput, Qt::AlignBottom);
    chartInputOutput->addAxis(axisYInputOutput, Qt::AlignLeft);

    chartView->setMinimumSize(axisX->tickCount() * 50, 230);
    chartView->setRenderHint(QPainter::Antialiasing);
    scene->addWidget(chartView);
    ui->graphicsView_2->setScene(scene);
    chartView->setGeometry(0, 0, ui->graphicsView_2->width(), ui->graphicsView_2->height());
}

void MainWindow::createProcesses(){


    processes.append(std::make_shared<Process>(
        "A",  // name
        3,    // arrivalTime
        5,    // firstCpuTime
        2,    // secondCpuTime
        2,    // ioTime
        1,    // ioChannel
        0     // axisY
        ));

    // Proceso B
    processes.append(std::make_shared<Process>(
        "B", 0, 2, 4, 2, 0, 1
        ));

    // Proceso C
    processes.append(std::make_shared<Process>(
        "C", 1, 4, 3, 8, 1, 2
        ));

    // Proceso D
    processes.append(std::make_shared<Process>(
        "D", 4, 2, -1, 0, -1, 3
        ));

}
