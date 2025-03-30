#include "process.h"

Process::Process()
{

}

Process::Process(QString name, int arrivalTime, int firstCpuTime, int secondCpuTime, int ioTime, int ioChannel, int axisY)
    : name(name),
    arrivalTime(arrivalTime),
    firstCpuTime(firstCpuTime),
    secondCpuTime(secondCpuTime),
    ioTime(ioTime),
    ioChannel(ioChannel),
    axisY(axisY)
{

    timeFinished = false;
    cpuTime = firstCpuTime;

}

void Process::substractTime(){

    cpuTime--;

}

void Process::substractIoTime(){

    ioTime--;

}

void Process::checkTimeFinished(int &time){

    if(time == 0){
        timeFinished = true;
    }

}

void Process::updateCpuTime(){

    if(secondCpuTime > 0){
        cpuTime = secondCpuTime;
    }

}

//Setters

void Process::setTimeFinished(bool _timeFinished){
    timeFinished = _timeFinished;
}

//Getters

QString Process::getName(){
    return name;
}

int Process::getArrivalTime(){
    return arrivalTime;
}

int Process::getCpuTime(){
    return cpuTime;
}

int Process::getFirstCpuTime(){
    return firstCpuTime;
}

int Process::getSecondCpuTime(){
    return secondCpuTime;
}

int Process::getIoTime(){
    return ioTime;
}

int Process::getIoChannel(){
    return ioChannel;
}

int Process::getAxisY(){
    return axisY;
}

bool Process::getTimeFinished(){
    return timeFinished;
}
