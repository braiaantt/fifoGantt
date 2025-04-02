#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

class Process
{
private:

    QString name;
    int arrivalTime;
    int cpuTime;
    int firstCpuTime;
    int secondCpuTime;
    int ioTime;
    int auxIoTime;
    int ioChannel;
    int axisY;

    bool timeFinished;

    void checkTimeFinished(int &);

public:
    Process();
    Process(QString name, int arrivalTime, int firstCpuTime, int secondCpuTime, int ioTime, int ioChannel, int axisY);

    void substractTime();
    void substractIoTime();
    void updateCpuTime();

    //Setters
    void setTimeFinished(bool);
    void resetAuxIoTime();
    void resetCpuTime();

    //Getters
    QString getName();
    int getArrivalTime();
    int getCpuTime();
    int getFirstCpuTime();
    int getSecondCpuTime();
    int getIoTime();
    int getIoChannel();
    int getAxisY();
    bool getTimeFinished();

};

#endif // PROCESS_H
