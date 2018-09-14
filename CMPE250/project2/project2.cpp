/*
Student Name: Baran Kılıç
Student Number: 2014400123
Project Number: 2
Operating System: Xubuntu 14.04
Compile Status:
Program Status:
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Cpu {
    double frequency;
    bool idle;
    double activeTime;
    Cpu(double frequency_){
        frequency = frequency_;
        idle = true;
        activeTime = 0;
    }
};

struct OutputUnit {
    double quantum;
    bool idle;
    double activeTime;
    OutputUnit(double quantum_){
        quantum = quantum_;
        idle = true;
        activeTime = 0;
    }
};

struct Event {
    int id;
    int type; //4 type: 0-cpuArrival, 1-cpuExit, 2-outArrival, 3-outExit
    int unitIndex; //index of the unit this event will be in
    double executionTime; //time that this event will be processed
    double waitTime; //time passed in queues as total
    double arrivalTime; //system arrival time
    double cpuWork;
    double remainingOutWork;
    Event(int id_,int type_,int unitIndex_,double executionTime_,double cpuWork_, double outWork){
        id = id_;
        type = type_;
        unitIndex = unitIndex_;
        executionTime = executionTime_;
        waitTime = 0;
        arrivalTime = executionTime_;
        cpuWork = cpuWork_;
        remainingOutWork = outWork;
    }
};

//less than comparison (but remember priority queue is a max heap)
// if return a<b then priority queue is [b a]
struct comparatorEvent {
    bool operator() (Event e1, Event e2){
        if(e1.executionTime == e2.executionTime){
            if(e1.type == e2.type){
                return e1.unitIndex > e2.unitIndex;
            }else{
                return e1.type < e2.type;
            }
        }else{
            return e1.executionTime > e2.executionTime;
        }
    }
};

struct comparatorCpu {
    bool operator() (Event e1, Event e2){
        return e1.cpuWork > e2.cpuWork;
    }
};

//returns the smallest index of the unit that is idle from a vector of unit
//if there is none return -1
template<typename T>
int indexOfIdleUnit(const vector<T> &vectorOfUnits){
    for(int i=0;i<vectorOfUnits.size();++i){
        if(vectorOfUnits[i].idle){
            return i;
        }
    }
    return -1;
}

template<typename T>
int indexOfUnitWithMaxActiveTime(const vector<T> &vectorOfUnits){
    int index = 0;
    double maxActiveTime = 0;
    for(int i=0;i<vectorOfUnits.size();++i){
        if(vectorOfUnits[i].activeTime > maxActiveTime){
            index = i;
            maxActiveTime = vectorOfUnits[i].activeTime;
        }
    }
    return index;
}

int main(int argc, char* argv[]){

    //Reading input file--------------
    ifstream inputFile(argv[1]);

    int numOfCpu;
    inputFile >> numOfCpu;

    vector<Cpu> cpuVector;
    for (int i = 0; i < numOfCpu; ++i) {
        double frequency;
        inputFile >> frequency;
        cpuVector.push_back(Cpu(frequency));
    }

    int numOfOutputUnit;
    inputFile >> numOfOutputUnit;

    vector<OutputUnit> outVector;
    for (int i = 0; i < numOfOutputUnit; ++i) {
        double quantum;
        inputFile >> quantum;
        outVector.push_back(OutputUnit(quantum));
    }

    int numOfTasks;
    inputFile >> numOfTasks;
    priority_queue<Event,vector<Event>, comparatorEvent> events;

    for(int i = 0; i < numOfTasks; ++i){
        double arrivalTime,cpuWork,outWork;
        inputFile >> arrivalTime >> cpuWork >> outWork;
        Event e(i,0,numOfCpu,arrivalTime,cpuWork,outWork);//0 is cpuArrival
        events.push(e);
    }

    //Processing events----------------
    double time = 0;
    priority_queue<Event,vector<Event>, comparatorCpu> cpuPriorityQueue;
    queue<Event> outQueue;

    //statistics
    int maxSizeOfCpuPQ = 0;
    int maxSizeOfOutQueue = 0;
    double totalWaitTime = 0;
    double maxWaitTime = 0;
    double totalSystemTime = 0;

    while(!events.empty()){
        Event current = events.top();
        events.pop();
        time = current.executionTime;

        if(current.type == 3){//outExit
            int unitIndex = current.unitIndex;
            if(current.remainingOutWork>0){
                if(outQueue.empty()){
                    double timeSpendInOutput = min(current.remainingOutWork,outVector[unitIndex].quantum);
                    outVector[unitIndex].activeTime += timeSpendInOutput;

                    current.executionTime = time + timeSpendInOutput;
                    current.remainingOutWork -= timeSpendInOutput;
                    events.push(current);
                }else{
                    outVector[unitIndex].idle = true;

                    current.type = 2;//outArrival
                    events.push(current);
                    //cout << time << "\n\t process#" << current.id << " is done at OI_unit#" << unitIndex << endl;

                    Event waitingEvent = outQueue.front();
                    outQueue.pop();
                    waitingEvent.waitTime += (time - waitingEvent.executionTime);
                    waitingEvent.executionTime = time;
                    waitingEvent.unitIndex = -1;
                    events.push(waitingEvent);
                    //cout << time << "\n\t process#" << waitingEvent.id;
                    //cout << " goes to OI_unit#" << unitIndex << " after waiting " << waitingEvent.waitTime << endl;
                    //cout <<"\tOI_unit departure at " << waitingEvent.executionTime << endl;
                }
            }else{
                outVector[unitIndex].idle = true;

                totalWaitTime += current.waitTime;
                if(current.waitTime>maxWaitTime){
                    maxWaitTime = current.waitTime;
                }
                totalSystemTime = totalSystemTime + (time - current.arrivalTime);

                //cout << time << "\n\t process#" << current.id << " is done at OI_unit#" << unitIndex << endl;

                if(!outQueue.empty()){
                    Event waitingEvent = outQueue.front();
                    outQueue.pop();
                    waitingEvent.waitTime += (time - waitingEvent.executionTime);
                    waitingEvent.executionTime = time;
                    waitingEvent.unitIndex = -1;
                    events.push(waitingEvent);
                    //cout << time << "\n\t process#" << waitingEvent.id;
                    //cout << " goes to OI_unit#" << unitIndex << " after waiting " << waitingEvent.waitTime << endl;
                    //cout <<"\tOI_unit departure at " << waitingEvent.executionTime << endl;
                }
            }
        }else if(current.type == 2){//outArrival
            int idleIndex = indexOfIdleUnit(outVector);
            if(idleIndex<0){ //no idle output unit
                outQueue.push(current);
                if(outQueue.size()>maxSizeOfOutQueue){
                    maxSizeOfOutQueue = outQueue.size();
                }
                //cout << time << "\n\tprocess#" << current.id << " goes second priority queue" <<endl;
            }else{ //there is idle output unit
                outVector[idleIndex].idle = false;
                double timeSpendInOutput = min(current.remainingOutWork,outVector[idleIndex].quantum);
                outVector[idleIndex].activeTime += timeSpendInOutput;

                current.type = 3;//outExit
                current.executionTime = time + timeSpendInOutput;
                current.unitIndex = idleIndex;
                current.remainingOutWork -= timeSpendInOutput;
                events.push(current);
                //cout << time << "\n\tprocess#" << current.id << " goes to OI_unit#" << idleIndex;
                //cout << " after waiting " << current.waitTime << endl;
                //cout <<"\tOI_unit departure at " << current.executionTime << endl;
            }
        }else if(current.type == 1){//cpuExit
            int unitIndex = current.unitIndex;
            cpuVector[unitIndex].idle =true;

            current.type = 2;//outArrival
            events.push(current);
            //cout << time << "\n\tprocess#" << current.id << " is done at CPU#" << unitIndex << endl;

            if(!cpuPriorityQueue.empty()){
                Event waitingEvent = cpuPriorityQueue.top();
                cpuPriorityQueue.pop();
                waitingEvent.waitTime += (time - waitingEvent.executionTime);
                waitingEvent.executionTime = time;
                waitingEvent.unitIndex = -1;//to give this event priority
                events.push(waitingEvent);
            }
        }else if(current.type == 0){//cpuArrival
            int idleIndex = indexOfIdleUnit(cpuVector);
            if(idleIndex<0){ //no idle cpu
                cpuPriorityQueue.push(current);
                if(cpuPriorityQueue.size()>maxSizeOfCpuPQ){
                    maxSizeOfCpuPQ = cpuPriorityQueue.size();
                }
                //cout << time << "\n\tprocess#" << current.id << " goes first priority queue" <<endl;
            }else{ //there is idle cpu
                cpuVector[idleIndex].idle = false;
                double timeSpendInCpu = current.cpuWork/cpuVector[idleIndex].frequency;
                cpuVector[idleIndex].activeTime += timeSpendInCpu;

                current.type = 1;//cpuExit
                current.executionTime = time + timeSpendInCpu;
                current.unitIndex = idleIndex;
                events.push(current);
                //cout << time << "\n\tprocess#" << current.id << " goes to CPU#" << idleIndex << endl;
                //cout <<"\tcpu departure at " << current.executionTime << endl;
            }
        }
    }

    //Writing output file------------
    ofstream outFile(argv[2]);
    outFile << maxSizeOfCpuPQ << "\n";
    outFile << maxSizeOfOutQueue << "\n";
    outFile << (indexOfUnitWithMaxActiveTime(cpuVector)+1) << "\n";//since id's start with 1
    outFile << (indexOfUnitWithMaxActiveTime(outVector)+1) << "\n";
    outFile << fixed << (totalWaitTime/numOfTasks) << "\n"; //average wait time
    outFile << fixed << maxWaitTime << "\n";
    outFile << fixed <<(totalSystemTime/numOfTasks) << "\n"; //average system time

    return 0;
}