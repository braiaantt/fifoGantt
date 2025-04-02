#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QScrollBar"
#include "QTimer"
#include "QLegendMarker"
#include "QScatterSeries"
#include <QMessageBox>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    chart = nullptr;
    chartInputOutput = nullptr;
    chartView = nullptr;
    chartViewInputOutput = nullptr;
    scene = nullptr;
    sceneInputOutput = nullptr;
    currentLineSeries = nullptr;
    lineSeriesInputOutputOne = nullptr;
    lineSeriesInputOutputTwo = nullptr;
    axisY = nullptr;
    axisX = nullptr;
    axisXInputOutput = nullptr;
    axisYInputOutput = nullptr;
    initButtonClicked = false;

    colors = {Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::darkRed};
    ui->tableWidgetProcessesData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->tableWidgetProcessesData, &QTableWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

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

void MainWindow::showContextMenu(const QPoint &point){

    QTableWidgetItem *item = ui->tableWidgetProcessesData->itemAt(point);
    if(!item) return;

    ui->tableWidgetProcessesData->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetProcessesData->selectRow(item->row());

    QMenu menu(this);
    QAction *deleteAction = menu.addAction("Eliminar proceso");

    QAction *selectedAction = menu.exec(ui->tableWidgetProcessesData->viewport()->mapToGlobal(point));

    if(deleteAction == selectedAction){
        int row = item->row();
        ui->tableWidgetProcessesData->removeRow(row);
    }

    ui->tableWidgetProcessesData->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidgetProcessesData->clearSelection();

}

void MainWindow::processing(){

    QStringList logs;

    ui->lineEditCurrentTimePos->setText(QString::number(x));
    qDebug()<<"";
    qDebug()<<"";
    qDebug()<<"";
    qDebug()<<x;


    QStringList arrivalProcessesLogs = processManager.checkArrivalProcesses(x);
    logs << arrivalProcessesLogs;

    int processesEnded = processManager.checkArrivalInputOutputProcesses();

    paintIoChart();
    if(processesEnded == 1){
        QString logIoOne = "LLegada de " + ui->listWidgetInputOutputOne->item(0)->text() + " a readyQueue desde I/O₁ en tiempo "+QString::number(x);
        logs<< logIoOne;
        lineSeriesInputOutputOne = nullptr;
        processManager.removeProcessFromIo(1);
        delete ui->listWidgetInputOutputOne->takeItem(0);
    } else if(processesEnded == 2){
        QString logIoTwo = "Llegada de " + ui->listWidgetInputOutputTwo->item(0)->text() + " a readyQueue desde I/O₂ en tiempo "+QString::number(x);
        logs << logIoTwo;
        lineSeriesInputOutputTwo = nullptr;
        processManager.removeProcessFromIo(2);
        delete ui->listWidgetInputOutputTwo->takeItem(0);
    } else if(processesEnded == 3){
        QString logIoOne = "LLegada de " + ui->listWidgetInputOutputOne->item(0)->text() + " a readyQueue desde I/O₁ en tiempo "+QString::number(x);
        QString logIoTwo = "LLegada de " + ui->listWidgetInputOutputTwo->item(0)->text() + " a readyQueue desde I/O₂ en tiempo "+QString::number(x);
        logs << logIoOne;
        logs << logIoTwo;
        lineSeriesInputOutputOne = nullptr;
        lineSeriesInputOutputTwo = nullptr;
        processManager.removeProcessFromIo(1);
        processManager.removeProcessFromIo(2);
        delete ui->listWidgetInputOutputOne->takeItem(0);
        delete ui->listWidgetInputOutputTwo->takeItem(0);
    }

    QStringList arrivalIoProcessesLogs = processManager.sortArrivalProcesses();
    logs << arrivalIoProcessesLogs;

    processManager.setArrivalProcessesOnReadyQueue();
    ui->listWidgetReadyQueue->clear();
    ui->listWidgetReadyQueue->addItems(processManager.getReadyQueueProcessesNames());

    std::shared_ptr<Process> execProcess = processManager.getExecProcess();

    if(!execProcess){
        qDebug()<<"Sin procesos a ejecutar!";
        return;
    }

    ui->lineEditCurrentProcess->setText(execProcess->getName());

    if(execProcess->getCpuTime() == 0){


        logs.append("Tiempo cpu proceso "+execProcess->getName()+" terminado");
        updateCoords(x,execProcess->getAxisY());

        if(execProcess->getIoTime() != 0){

            int ioChannel;
            int lastY = execProcess->getAxisY();
            execProcess->updateCpuTime();

            if(execProcess->getIoChannel() == 1){
                logs.append("Proceso " + execProcess->getName() + " asignado a I/O₁");

                ui->listWidgetInputOutputOne->addItem(execProcess->getName());
                ioChannel = 1;
            } else {
                logs.append("Proceso " + execProcess->getName() + " asignado a I/O₂");
                ui->listWidgetInputOutputTwo->addItem(execProcess->getName());
                ioChannel = 2;
            }

            processManager.moveProcessFromExecToIo(ioChannel);

            execProcess = processManager.getExecProcess();
            logs.append("Nuevo execProcess: "+execProcess->getName());
            ui->lineEditCurrentProcess->setText(execProcess->getName());
            delete ui->listWidgetReadyQueue->takeItem(0);
            verticalLineSeries(lastY);

            execProcess->substractTime();

        } else if(execProcess->getCpuTime() > 0){

            logs.append("Proceso " + execProcess->getName() + " movido al final de la readyQueue");

            ui->listWidgetReadyQueue->addItem(ui->listWidgetReadyQueue->takeItem(0));

            int lastY = execProcess->getAxisY();
            execProcess->updateCpuTime();
            processManager.moveProcessToEnd();
            execProcess = processManager.getExecProcess();
            ui->lineEditCurrentProcess->setText(execProcess->getName());
            delete ui->listWidgetReadyQueue->takeItem(0);
            verticalLineSeries(lastY);

            logs.append("Nuevo execProcess: "+execProcess->getName());

        } else {

            logs.append("Proceso " + execProcess->getName() + " terminado");
            markProcessKilled();
            processManager.killProcess();
            ui->lineEditCurrentProcess->setText("");
            delete ui->listWidgetReadyQueue->takeItem(0);

            if(processManager.getExecProcess() != nullptr){

                int lastY = execProcess->getAxisY();
                execProcess = processManager.getExecProcess();
                logs.append("Nuevo execProcess: "+execProcess->getName());
                ui->lineEditCurrentProcess->setText(execProcess->getName());
                verticalLineSeries(lastY);
                execProcess->substractTime();

            } else {
                qDebug()<<"No hay más procesos!";
            }

        }

    } else {
        execProcess->substractTime();
    }

    paintCoreChart();
    paintIoChart();
    if(!logs.empty()) logs << "";
    ui->listWidgetLogs->addItems(logs);
    ui->listWidgetLogs->scrollToBottom();

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

    if(processManager.getExecProcess() != nullptr){
        updateCoords(x, processManager.getExecProcess()->getAxisY());
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

    axisX = new QValueAxis(chart);
    axisY = new QBarCategoryAxis(chart);

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

    chartView = new QChartView();
    chart = new QChart();
    scene = new QGraphicsScene();

    chart->setAnimationOptions(QChart::SeriesAnimations);

    initAxis();

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    chartView->setChart(chart);
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

    return totalCpuTime + 10;

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
    updateCoords(x,processManager.getExecProcess()->getAxisY());

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

    if(processManager.getExecProcess() != nullptr){
        lineSeries->setColor(colors[processManager.getExecProcess()->getAxisY()]);
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

    if(processManager.getExecProcess() != nullptr){
        point->setColor(colors[processManager.getExecProcess()->getAxisY()]);
        point->append(x,processManager.getExecProcess()->getAxisY());
    }

    auto markers = chart->legend()->markers(point);
    if (!markers.isEmpty()) {
        markers.first()->setVisible(false); // Oculta el marcador en la leyenda
    }

}

void MainWindow::initInputOutputGraphic(){

    chartInputOutput = new QChart();
    chartViewInputOutput = new QChartView();
    sceneInputOutput = new QGraphicsScene();

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

    chartViewInputOutput->setChart(chartInputOutput);
    chartViewInputOutput->setMinimumSize(axisX->tickCount() * 50, 230);
    chartViewInputOutput->setRenderHint(QPainter::Antialiasing);
    sceneInputOutput->addWidget(chartViewInputOutput);
    ui->graphicsView_2->setScene(sceneInputOutput);
    chartViewInputOutput->setGeometry(0, 0, ui->graphicsView_2->width(), ui->graphicsView_2->height());
    ui->graphicsView_2->horizontalScrollBar()->setValue(0);

}

void MainWindow::on_pushButtonInitCharts_clicked()
{

    if(initButtonClicked){
        ui->pushButtonRestartCharts->click();
        processManager.deleteProcesses();
    } else {
        initButtonClicked = true;
    }

    if(!checkInputsTableWidget()){

        QMessageBox::warning(this, "Error", "En la tabla solo se permiten valores de tipo 'int' (Entero)");
        return;
    }

    if(!checkIoInputs()){

        QMessageBox::warning(this, "Error", "Solo puede haber un valor de I/O.");
        return;
    }

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

bool MainWindow::checkInputsTableWidget(){

    QTableWidget* table = ui->tableWidgetProcessesData;

    for(int i = 0; i<table->rowCount(); i++){

        for(int j = 1; j<table->columnCount(); j++){

            QTableWidgetItem *item = table->item(i,j);
            bool isInt;
            item->text().toInt(&isInt);
            if(!isInt) return false;

        }

    }

    return true;

}

bool MainWindow::checkIoInputs(){

    QTableWidget *table = ui->tableWidgetProcessesData;

    for(int i = 0; i<table->rowCount(); i++){

        QTableWidgetItem* ioOne = table->item(i,3);
        QTableWidgetItem* ioTwo = table->item(i,4);

        if(ioOne->text().toInt() > 0 && ioTwo->text().toInt() > 0){
            return false;
        }

    }

    return true;

}

void MainWindow::on_pushButtonRestartCharts_clicked()
{

    delete chart;
    delete chartInputOutput;
    delete chartView;
    delete chartViewInputOutput;
    delete scene;
    delete sceneInputOutput;

    chart = nullptr;
    chartInputOutput = nullptr;
    chartView = nullptr;
    chartViewInputOutput = nullptr;
    scene = nullptr;
    sceneInputOutput = nullptr;

    currentLineSeries = nullptr;
    lineSeriesInputOutputOne = nullptr;
    lineSeriesInputOutputTwo = nullptr;

    axisX = nullptr;
    axisY = nullptr;
    axisXInputOutput = nullptr;
    axisYInputOutput = nullptr;

    initCoreGraphic();

    initLegends();

    initInputOutputGraphic();

    x = 0;
    ui->graphicsView->horizontalScrollBar()->setValue(0);
    ui->graphicsView_2->horizontalScrollBar()->setValue(0);
    processManager.reset();

    ui->listWidgetInputOutputOne->clear();
    ui->listWidgetInputOutputTwo->clear();
    ui->listWidgetLogs->clear();
    ui->listWidgetReadyQueue->clear();

    ui->pushButtonRestartCharts->clearFocus();

    ui->lineEditCurrentProcess->clear();
    ui->lineEditCurrentTimePos->clear();

}


void MainWindow::on_pushButtonAddProcess_clicked()
{

    QTableWidget* table = ui->tableWidgetProcessesData;
    int rowCount = table->rowCount();
    table->insertRow(rowCount);

    for(int column = 0; column< table->columnCount(); column++){
        QTableWidgetItem *item = new QTableWidgetItem();
        table->setItem(rowCount, column, item);
    }

}
