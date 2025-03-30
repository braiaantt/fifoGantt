#include "processmanager.h"

ProcessManager::ProcessManager()
{

}

void ProcessManager::removeProcess(){

    readyQueue.removeAt(0);

}

void ProcessManager::moveProcessTo(ProcessManager& processManager){

    processManager.addProcessToReadyQueue(currentProcess);
    removeProcess();

    //updateProcess
    if(readyQueue.size() > 0){
        currentProcess = readyQueue[0];
    }

}

void ProcessManager::addProcessToReadyQueue(Process process){

    readyQueue << process;

}


