#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QVector>
#include "process.h"

class ProcessManager
{

private:
    QVector<Process> readyQueue;
    Process currentProcess;
    int time;

    void removeProcess();

public:
    ProcessManager();

    void moveProcessTo(ProcessManager &);
    void addProcessToReadyQueue(Process);

    //Getters
    int getTime();

};

#endif // PROCESSMANAGER_H
