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
    std::shared_ptr<Process> execProcess;

    bool checkIoList(QVector<std::shared_ptr<Process>> &);
    void updateExecProcess();

public:
    ProcessManager();

    void addProcess(QString name, int arrivalTime, int firstCpuTime, int secondCpuTime, int ioTime, int ioChannel, int axisY);
    QStringList checkArrivalProcesses(int x);
    int checkArrivalInputOutputProcesses();
    QStringList sortArrivalProcesses();
    void setArrivalProcessesOnReadyQueue();
    bool readyQueueIsEmpty();
    bool inputOutputOneIsEmpty();
    bool inputOutputTwoIsEmpty();
    void moveProcessFromExecToIo(int ioChannel);
    void moveProcessToEnd();
    void removeProcessFromIo(int ioChannel);
    void killProcess();
    void reset();
    void deleteProcesses();

    //getters
    QStringList getProcessesNames();
    std::shared_ptr<Process> getExecProcess();
    std::shared_ptr<Process> getCurrentIoOneProcess();
    std::shared_ptr<Process> getCurrentIoTwoProcess();
    QStringList getReadyQueueProcessesNames();

};

#endif // PROCESSMANAGER_H
