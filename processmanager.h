#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QVector>
#include "process.h"

class ProcessManager
{

private:
    QVector<std::shared_ptr<Process>> processes;
    QVector<std::shared_ptr<Process>> readyQueue;
    QVector<std::shared_ptr<Process>> inputOutputOne;
    QVector<std::shared_ptr<Process>> inputOutputTwo;
    QVector<std::shared_ptr<Process>> arrivalProcesses;

    bool checkIoList(QVector<std::shared_ptr<Process>> &);

public:
    ProcessManager();

    void addProcess(QString name, int arrivalTime, int firstCpuTime, int secondCpuTime, int ioTime, int ioChannel, int axisY);
    void checkArrivalProcesses(int x);
    int checkArrivalInputOutputProcesses();
    void sortArrivalProcesses();
    bool readyQueueIsEmpty();
    bool inputOutputOneIsEmpty();
    bool inputOutputTwoIsEmpty();
    void moveProcessFromReadyQueueToIo(int ioChannel);
    void moveProcessToEnd();
    void killProcess();

    //getters
    QStringList getProcessesNames();
    std::shared_ptr<Process> getCurrentProcess();
    std::shared_ptr<Process> getCurrentIoOneProcess();
    std::shared_ptr<Process> getCurrentIoTwoProcess();

};

#endif // PROCESSMANAGER_H
