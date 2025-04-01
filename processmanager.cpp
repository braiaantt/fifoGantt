#include "processmanager.h"
#include "QDebug"

ProcessManager::ProcessManager()
{

}

void ProcessManager::addProcess(QString name, int arrivalTime, int firstCpuTime, int secondCpuTime, int ioTime, int ioChannel, int axisY){

    processes.append(std::make_shared<Process>(name, arrivalTime, firstCpuTime,
                                               secondCpuTime, ioTime, ioChannel,axisY));

}

void ProcessManager::checkArrivalProcesses(int x){

    for(auto &process : processes){
        if(process->getArrivalTime() == x){

            arrivalProcesses.append(process);
            qDebug()<<"Proceso"<< process->getName() <<"añadido a arrivalProcesses";

        }
    }

}

int ProcessManager::checkArrivalInputOutputProcesses(){

    int res = 0;

    bool processEndedListOne = checkIoList(inputOutputOne);
    bool processEndedListTwo = checkIoList(inputOutputTwo);

    if(processEndedListOne && processEndedListTwo){
        res = 3;
    } else if(processEndedListOne){
        res = 1;
    } else if(processEndedListTwo){
        res = 2;
    }

    return res;
}

bool ProcessManager::checkIoList(QVector<std::shared_ptr<Process>> &ioList){

    if(!ioList.empty()){

        std::shared_ptr<Process> currentProcess = ioList[0];

        qDebug()<<"Proceso: "<<currentProcess->getName();
        qDebug()<<"ioTime: "<<currentProcess->getIoTime();
        ioList[0]->substractIoTime();
        qDebug()<<"ioTimeDsp: "<<currentProcess->getIoTime();

        if(currentProcess->getIoTime() == 0){

            qDebug()<<"Proceso "<<currentProcess->getName()<<" añadido a arrivalProcesses";
            arrivalProcesses.append(currentProcess);

            return true;

        }

    } else {
        qDebug()<<"Lista vacía";
    }

    return false;

}

void ProcessManager::sortArrivalProcesses(){


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

bool ProcessManager::readyQueueIsEmpty(){

    return readyQueue.empty();

}

bool ProcessManager::inputOutputOneIsEmpty(){
    return inputOutputOne.empty();
}

bool ProcessManager::inputOutputTwoIsEmpty(){
    return inputOutputTwo.empty();
}

void ProcessManager::moveProcessFromReadyQueueToIo(int ioChannel){

    if(ioChannel == 1){
        inputOutputOne.append(readyQueue[0]);
    } else {
        inputOutputTwo.append(readyQueue[0]);
    }
    readyQueue.removeAt(0);

}

void ProcessManager::moveProcessToEnd(){

    readyQueue.move(0, readyQueue.size() - 1);

}

void ProcessManager::removeProcessFromIo(int ioChannel){

    if(ioChannel == 1){
        inputOutputOne.removeAt(0);
    } else {
        inputOutputTwo.removeAt(0);
    }

}

void ProcessManager::killProcess(){

    readyQueue.removeAt(0);

}

//getters

QStringList ProcessManager::getProcessesNames(){

    QStringList processesNames;

    for(auto &process : processes){
        QString name = process->getName();
        processesNames << name;
    }

    return processesNames;

}

std::shared_ptr<Process> ProcessManager::getCurrentProcess(){
    return readyQueue[0];
}

std::shared_ptr<Process> ProcessManager::getCurrentIoOneProcess(){
    return inputOutputOne[0];
}

std::shared_ptr<Process> ProcessManager::getCurrentIoTwoProcess(){
    return inputOutputTwo[0];
}
