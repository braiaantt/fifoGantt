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

    chart = nullptr;
    chartInputOutput = nullptr;
    currentLineSeries = nullptr;
    lineSeriesInputOutputOne = nullptr;
    lineSeriesInputOutputTwo = nullptr;

    colors = {Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::darkRed};

    x = 0;
    sliderValue = 14;

}

MainWindow::~MainWindow()
{
    delete ui; 
    delete chart;
    delete chartInputOutput;
}

void MainWindow::keyPressEvent(QKeyEvent* event){

    if(event->key() == Qt::Key_Right) {
        processing();
    }

    QMainWindow::keyPressEvent(event);

}

void MainWindow::processing(){

    processManager.checkArrivalProcesses(x);

    int processesEnded = processManager.checkArrivalInputOutputProcesses();

    if(processesEnded == 1){
        lineSeriesInputOutputOne = nullptr;
    } else if(processesEnded == 2){
        lineSeriesInputOutputTwo = nullptr;
    } else if(processesEnded == 3){
        lineSeriesInputOutputOne = nullptr;
        lineSeriesInputOutputTwo = nullptr;
    }
    paintIoChart();


    processManager.sortArrivalProcesses();

    if(processManager.readyQueueIsEmpty()){
        qDebug()<<"ReadyQueue vacía!";
        return;
    }

    std::shared_ptr<Process> currentProcess = processManager.getCurrentProcess();

    qDebug()<<"Trabajando proceso: "<<currentProcess->getName();

    if(currentProcess->getCpuTime() == 0){

        qDebug()<<"Tiempo cpu terminado";
        updateCoords(x,currentProcess->getAxisY());

        if(currentProcess->getIoTime() != 0){

            int ioChannel;
            int lastY = currentProcess->getAxisY();
            currentProcess->updateCpuTime();

            if(currentProcess->getIoChannel() == 1){
                qDebug()<<"Asignado a io 1";
                ioChannel = 1;
            } else {
                qDebug()<<"Asignado a io 2";
                ioChannel = 2;
            }

            processManager.moveProcessFromReadyQueueToIo(ioChannel);

            currentProcess = processManager.getCurrentProcess();
            qDebug()<<"Nuevo proceso: "<<currentProcess->getName();
            verticalLineSeries(lastY);

            currentProcess->substractTime();

        } else if(currentProcess->getCpuTime() > 0){

            qDebug()<<"Proceso "<<currentProcess->getName()<<" movido al final";

            int lastY = currentProcess->getAxisY();
            currentProcess->updateCpuTime();
            processManager.moveProcessToEnd();
            currentProcess = processManager.getCurrentProcess();
            verticalLineSeries(lastY);

            qDebug()<<"Nuevo proceso: "<<currentProcess->getName();

        } else {

            qDebug()<<"Proceso muerto";
            markProcessKilled();
            processManager.killProcess();//readyQueue.removeAt(0);
            if(!processManager.readyQueueIsEmpty()){

                int lastY = currentProcess->getAxisY();
                currentProcess = processManager.getCurrentProcess();
                qDebug()<<"Nuevo proceso: "<<currentProcess->getName();
                verticalLineSeries(lastY);
                currentProcess->substractTime();

            } else {
                qDebug()<<"No hay más procesos!";
            }

        }

    } else {
        currentProcess->substractTime();
    }

    paintCoreChart();
    paintIoChart();

    if(x >= sliderValue){
        ui->graphicsView->horizontalScrollBar()->setValue(ui->graphicsView->horizontalScrollBar()->value() + 50);
        ui->graphicsView_2->horizontalScrollBar()->setValue(ui->graphicsView_2->horizontalScrollBar()->value() + 50);
    }

    x++;

}

void MainWindow::paintCoreChart(){

    if(currentLineSeries == nullptr){

        createLineSeries();

    }

    if(!processManager.readyQueueIsEmpty()){
        updateCoords(x, processManager.getCurrentProcess()->getAxisY());
    }

}

void MainWindow::paintIoChart(){

    if(!processManager.inputOutputOneIsEmpty()){
        if(lineSeriesInputOutputOne == nullptr) createIoChartLineSeries(1);
        lineSeriesInputOutputOne->append(x, 1);
    }

    if(!processManager.inputOutputTwoIsEmpty()){
        if(lineSeriesInputOutputTwo == nullptr) createIoChartLineSeries(0);
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
        lineSeries->setColor(colors[processManager.getCurrentIoOneProcess()->getAxisY()]);
    } else {
        lineSeriesInputOutputTwo = lineSeries;
        lineSeries->setColor(colors[processManager.getCurrentIoTwoProcess()->getAxisY()]);
    }

    auto markers = chartInputOutput->legend()->markers(lineSeries);
    if (!markers.isEmpty()) {
        markers.first()->setVisible(false);
    }

}

void MainWindow::initAxis(){

    int axisXCount = calcAxisCount(); //valor calculado por el resultado de la suma de los tiempos de nucleo

    axisX = new QValueAxis();
    axisY = new QBarCategoryAxis();

    //Configuracion eje x
    axisX->setRange(0, axisXCount);
    axisX->setTickCount(axisXCount+1);
    axisX->setLabelFormat("%.0f");
    axisX->setTitleText("Tiempo");
    axisX->setGridLineVisible(false);

    // Configuracion eje y
    QStringList processesNames = processManager.getProcessesNames();
    axisY->append(processesNames);
    axisY->setGridLineVisible(false);

}

void MainWindow::initCoreGraphic(){

    chart = new QChart();
    QChartView *chartView = new QChartView(chart);
    QGraphicsScene *scene = new QGraphicsScene(chart);

    chart->setAnimationOptions(QChart::SeriesAnimations);

    initAxis();

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(axisX->tickCount() * 50, 300);
    scene->addWidget(chartView);
    ui->graphicsView->setScene(scene);
    chartView->setGeometry(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    ui->graphicsView->horizontalScrollBar()->setValue(0);

}

int MainWindow::calcAxisCount(){

    QTableWidget *table = ui->tableWidgetProcessesData;

    int totalCpuTime = 0;

    for(int i = 0; i<table->rowCount(); i++){

        int firstCpuTime = table->item(i,2)->text().toInt();
        int secondCpuTime = table->item(i,5)->text().toInt();
        totalCpuTime += firstCpuTime + secondCpuTime;

    }

    return totalCpuTime;

}

void MainWindow::initLegends(){

    QStringList processesNames = processManager.getProcessesNames();

    for(int i = 0; i<processesNames.size(); i++){
        QLineSeries *legendSeries = new QLineSeries(chart);
        chart->addSeries(legendSeries);
        legendSeries->setName(processesNames[i]);
        legendSeries->setColor(colors[i]);
    }

}

void MainWindow::verticalLineSeries(int lastY){

    createLineSeries();
    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setColor(Qt::gray);
    currentLineSeries->setPen(pen);

    updateCoords(x,lastY);
    updateCoords(x,processManager.getCurrentProcess()->getAxisY());

    currentLineSeries = nullptr;

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

    if(!processManager.readyQueueIsEmpty()){
        lineSeries->setColor(colors[processManager.getCurrentProcess()->getAxisY()]);
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

    if(!processManager.readyQueueIsEmpty()){
        point->setColor(colors[processManager.getCurrentProcess()->getAxisY()]);
    }

    auto markers = chart->legend()->markers(point);
    if (!markers.isEmpty()) {
        markers.first()->setVisible(false); // Oculta el marcador en la leyenda
    }

    point->append(x,processManager.getCurrentProcess()->getAxisY());

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
    ui->graphicsView_2->horizontalScrollBar()->setValue(0);
}

void MainWindow::on_pushButtonInitCharts_clicked()
{
    QTableWidget* table = ui->tableWidgetProcessesData;
    QString name;
    int arrivalTime, firstCpuTime, secondCpuTime,inputOutputOne, inputOutputTwo, axisY;

    for(int i = 0; i<table->rowCount(); i++){

        name = table->item(i, 0)->text();
        arrivalTime = table->item(i,1)->text().toInt();
        firstCpuTime = table->item(i,2)->text().toInt();
        inputOutputOne = table->item(i,3)->text().toInt();
        inputOutputTwo = table->item(i,4)->text().toInt();
        secondCpuTime = table->item(i,5)->text().toInt();

        if(inputOutputOne != 0){
            processManager.addProcess(name,arrivalTime,firstCpuTime,secondCpuTime,inputOutputOne,1,i);
        } else {
            processManager.addProcess(name,arrivalTime,firstCpuTime,secondCpuTime,inputOutputTwo,0,i);
        }

    }

    initCoreGraphic();
    initLegends();
    initInputOutputGraphic();

}

