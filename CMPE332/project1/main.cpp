#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <deque>
#include <map>

using namespace std;

class Process{
public:
    string name; // name of the process (e.g. P1)
    string code; // filename of the code that this process will execute
    int arrival; // arrival time of the process
    int nextLine; // next line of the code to execute

    Process(string &name,string &code, int arrival){
        this->name = name;
        this->code = code;
        this->arrival = arrival;
        this->nextLine = 0;
    }
};

class ProcessDefinition{
private:
    queue<Process> arrivalQueue; // queue that contains the processes that will arrive in the future
public:
    // creates the arrivalQueue from given process definition file
    explicit ProcessDefinition(const string &filename){
        string name;
        string code;
        int arrival;

        ifstream inputFile(filename);
        while(inputFile >> name >> code >> arrival){
            arrivalQueue.push(Process(name,code,arrival));
        }
    }

    // returns the process that arrives next
    Process getProcess(){
        Process p = arrivalQueue.front();
        arrivalQueue.pop();
        return p;
    }

    // returns the arrival time of the next arriving process
    int nextArrivalTime(){
        return arrivalQueue.front().arrival;
    }

    // returns true if there are any process that will arrive in the future
    bool hasProcess(){
        return !arrivalQueue.empty();
    }
};

class Code{
public:
    vector<string> instruction; // instuctions of the code , lines can be accessed by index
    vector<int> executionTime; // execution time of the instruction

    // creates Code class from the code file
    explicit Code(const string &filename){
        string instr;
        int execTime;
        ifstream inputFile(filename);
        while(inputFile >> instr >> execTime){
            instruction.push_back(instr);
            executionTime.push_back(execTime);
        }
    }

    Code() = default;
};

// a class in order to access the codes from code filenames
class CodeMap{
private:
    map<string,Code> nameToCodeMap; // maps code filename to Code
public:
    // creates 4 code objects from code files and add them to the map
    CodeMap(){
        for(int i=1;i<5;i++){
            string filename = to_string(i) + ".code.txt";
            Code myCode(filename);
            nameToCodeMap[filename] = myCode;
        }
    }

    // name: filename of the code
    // line: line of the instruction
    // returns the instruction of a code
    string getInstruction(string &name, int line){
        return nameToCodeMap[name].instruction[line];
    }

    // returns the exectution time of an instruction
    int getExecutionTime(string &name, int line){
        return nameToCodeMap[name].executionTime[line];
    }

};

// class that implements round robin scheduling algorithm
class Scheduler{
public:
    deque<Process> processQueue; // ready queue
    int timeSlot = 100; // quantum for round robin

    CodeMap codeMap;

    // pops a process from ready queue execute its instruction and push it back to the queue if necessary
    // time: time at the beginning of this execution
    // outFile: file stream for output to print the queue
    // processDef: contains the process definition. necessary to check whether if new processes during this execution
    // returns the elapsed time for executing the instructions in the time slot
    int executeProcess(int time,ofstream &outFile, ProcessDefinition &processDef){
        int elapsedTime = 0; // time that elapses during instruction execution
        int remainingTime = timeSlot; // remaining time slot for this process

        if(!processQueue.empty()){
            Process p = processQueue.front(); // process to execute
            processQueue.pop_front();
            string codeName = p.code; // code file name of the process
            int currentLine = p.nextLine; // next line to execute
            string instrName; // name of the instruction to execute
            int execTime; // execution time of the instruction

            // if process has remaining time slot continue execution
            while(remainingTime>0){
                instrName = codeMap.getInstruction(codeName,currentLine);
                execTime = codeMap.getExecutionTime(codeName,currentLine);

                remainingTime -= execTime;
                elapsedTime += execTime;

                // if executed instruction is the last one (exit) break the loop
                if(instrName == "exit"){
                    break;
                }

                currentLine++;
            }

            // check whether a new process arrived during the execution of the process
            // "There will be always a process in the ready queue according to test definition files."
            // So no need to check process arrival from another place
            checkArrival(time,elapsedTime,processDef);

            // if the process does not end push it back to the queue
            if(instrName != "exit"){
                p.nextLine = currentLine;
                processQueue.push_back(p);
            }
        }
        // else{}
        // I do not consider the case where queue is empty because of the announcement:
        // "There will be always a process in the ready queue according to test definition files.
        // So you don't have to worry about what to do in "idle CPU" state."

        printQueue(time+elapsedTime,outFile);

        return elapsedTime;
    }

    // adds the processes that arrived during the executing of a process
    // time: beginning of the execution
    // elapsedTime: time spent executing the process
    // processDef: process definition contains the processes that will arrive
    void checkArrival(int time,int elapsedTime, ProcessDefinition &processDef){
        int nextArrival = processDef.nextArrivalTime();
        while(nextArrival>= time && nextArrival <= time + elapsedTime){
            addProcess(processDef.getProcess());
            nextArrival = processDef.nextArrivalTime();
        }
    }

    // outputs the current state of the process queue to a file
    // time: current time
    // outFile: file stream to output
    void printQueue(int time,ofstream &outFile){
        outFile << time << "::HEAD-";
        if(processQueue.empty()){
            outFile << "-";
        }else{
            for (auto &iter : processQueue) {
                outFile << iter.name << "-";
            }
        }
        outFile << "TAIL" << endl;
    }

    // adds a process to the process queue
    void addProcess(const Process &p){
        processQueue.push_back(p);
    }

    // returns true if process queue not empty
    bool hasProcess(){
        return !processQueue.empty();
    }
};

// a class to simulate the round robin scheduler
// it simulates the execution by jumping from one time to another time
class Simulator{
public:
    int time=0;

    string definitionFilename = "definition.txt";
    string outputFilename = "output.txt";

    void start(){
        time = 0;

        ProcessDefinition processDef(definitionFilename);
        Scheduler scheduler;

        ofstream outFile(outputFilename);

        // skip the time until the first arriving process
        time = processDef.nextArrivalTime();
        // add the first process to the queue
        scheduler.addProcess(processDef.getProcess());

        scheduler.printQueue(time,outFile);

        // execute process until there is not any arriving process and scheduler do not have any process to execute
        while(processDef.hasProcess() || scheduler.hasProcess()){
            time += scheduler.executeProcess(time, outFile, processDef);
        }
    }
};

int main(int argc, char *argv[]){
    Simulator simulation;
    simulation.start();
    return 0;
}
