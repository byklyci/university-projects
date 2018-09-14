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

// class that implements semaphores with no busy waiting
class Semaphore{
public:
    int id; // id of the semaphore.  e.g. 1 for waitS_1
    int value; // value of the semaphore
    deque<Process> waitQueue; // processes that wait this semaphore
    string output; // this string stores output of print queue function. I will write it to a file in the end

    explicit Semaphore(int _id) : value(1), id(_id){} // initialize value to 1 and id to given id

    // waitS for semaphore. locks the semaphore.
    // processQueue: ready queue
    // time: time of the simulation at calling this waitS function
    // returns 0 if semaphore is not available. returns 1 otherwise
    int waitS(deque<Process> &processQueue, int time){
        value--;
        // if semaphore is not available the the process from ready queue and
        // place it in the wait queue of the semaphore
        if(value<0){
            Process p = processQueue.front();
            processQueue.pop_front();
            waitQueue.push_back(p);
            printQueue(time);
            return 0;
        }else{
            return 1;
        }
    }

    // signS for semaphore. makes the semaphore available for another process
    // processQueue: ready queue
    // time: time of the simulation at calling this waitS function
    // returns true if a process that waits this semaphore and this process has
    // been waked up. false otherwise.
    bool signS(deque<Process> &processQueue, int time){
        value++;
        // if there are any process that waits the semaphore wake it up by taking it from wait queue
        // and placing it to the ready queue
        if(value<=0){
            Process p = waitQueue.front();
            waitQueue.pop_front();
            processQueue.push_back(p);
            printQueue(time);
            return true;
        }
        return false;
    }

    // time: simulation time of this print queue function
    // prints current status of the wait queue to the string named "output"
    void printQueue(int time){
        output += to_string(time) + "::HEAD-";
        if(waitQueue.empty()){
            output += "-";
        }else{
            for (auto &iter : waitQueue) {
                output += iter.name + "-";
            }
        }
        output += "TAIL\n";
    }

    // prints the content of the output string (contains the waiting queue information) to
    // a file if this string is not empty (if it is empty this means that this semaphore is not used)
    void print(){
        if(output.length() > 0){
            ofstream outFile("output_" + to_string(id)+".txt");
            outFile << output;
        }
    }
};

class Printer{
public:
    int id; // id of printer. used for naming output
    deque<Process> waitQueue; // wait queue of the printer
    deque<int> execTimeQueue; // execution time of the process at this printer
    int instrStartTime; // time when a process has started executing at this printer
    string output; // this string stores output of print queue function. I will write it to a file in the end

    // at the beginning id is initialized with _id and instrStartTime with -1
    explicit Printer(int _id) : id(_id), instrStartTime(-1){}

    // the process at the beginning of the process queue is taken and placed to the wait queue of the printer
    // processQueue: ready queue
    // execTime: the time that this process should execute at this printer
    // currentTime: time of calling of this function
    void addProcess(deque<Process> &processQueue, int execTime,int currentTime){
        Process p = processQueue.front();
        waitQueue.push_back(p);
        printQueue(currentTime);
        processQueue.pop_front();
        execTimeQueue.push_back(execTime);
        if(instrStartTime == -1){
            instrStartTime = currentTime;
        }
    }

    // returns true if the process at this queue finished executing. false,otherwise
    // currentTime: time of calling of this function
    bool isProcessExecuted(int currentTime){
        if(instrStartTime == -1){
            return false;
        }else{
            return instrStartTime+execTimeQueue.front() <= currentTime;
        }
    }

    // the process at the beginning of the wait queue is taken end placed at the end of the ready queue
    // processQueue: ready queue
    // currentTime: time of calling of this function
    void removeProcess(deque<Process> &processQueue,int currentTime){
        Process p = waitQueue.front();
        waitQueue.pop_front();
        printQueue(currentTime);
        execTimeQueue.pop_front();
        processQueue.push_back(p);
        if(waitQueue.empty()){
            instrStartTime = -1;
        }else{
            instrStartTime = currentTime;
        }
    }

    // time: simulation time of this print queue function
    // prints current status of the wait queue to the string named "output"
    void printQueue(int time){
        output += to_string(time) + "::HEAD-";
        if(waitQueue.empty()){
            output += "-";
        }else{
            for (auto &iter : waitQueue) {
                output += iter.name + "-";
            }
        }
        output += "TAIL\n";
    }

    // prints the content of the output string (contains the waiting queue information) to
    // a file if this string is not empty
    void print(){
        if(output.length() > 0){
            ofstream outFile("output_" + to_string(id)+".txt");
            outFile << output;
        }
    }
};

class HardDrive{
public:
    int id; // id of printer. used for naming output
    deque<Process> waitQueue; // wait queue of the hard drive
    deque<int> execTimeQueue; // execution time of the process at this hard drive
    int instrStartTime; // time when a process has started executing at this hard drive
    vector<int> cache; // cache for hard drive
    int lru; // index of cache that is least recently used
    string output; // this string stores output of print queue function. I will write it to a file in the end

    explicit HardDrive(int _id) :
            id(_id),
            instrStartTime(-1),
            cache{-1,-1}, //cache is initially empty
            lru(1) {}

    // checks whether the block is in cache. returns true if it is in cache.
    // it also updates the least recently used cache information
    bool isInCache(int blockNo){
        if(blockNo == cache[0]){
            lru = 1;
            return true;
        } else if(blockNo == cache[1]){
            lru = 0;
            return true;
        } else{
            return false;
        }
    }

    // updates the cache and least recently used cache information
    void updateCache(int blockNo){
        cache[lru] = blockNo;
        if(lru == 0){
            lru = 1;
        }else if(lru == 1){
            lru = 0;
        }
    }

    // returns true if block is in cache. else it takes the process from ready queue and place it to the
    // end of wait queue of the hard drive
    // blockNo: block number of the data to be read
    // execTime: the time that will pass when reading the block if it is not in cache
    // currentTime: time of calling of this function
    bool read(int blockNo,deque<Process> &processQueue, int execTime,int currentTime){
        if(isInCache(blockNo)){
            return true;
        }else{
            updateCache(blockNo);
            addProcess(processQueue,execTime,currentTime);
            return false;
        }
    }

    // the process at the beginning of the process queue is taken and placed to the wait queue of the printer
    // processQueue: ready queue
    // execTime: the time that this process should execute at this printer
    // currentTime: time of calling of this function
    void addProcess(deque<Process> &processQueue, int execTime,int currentTime){
        Process p = processQueue.front();
        waitQueue.push_back(p);
        printQueue(currentTime);
        processQueue.pop_front();
        execTimeQueue.push_back(execTime);
        if(instrStartTime == -1){
            instrStartTime = currentTime;
        }
    }

    // returns true if the process at this queue finished executing. false,otherwise
    // currentTime: time of calling of this function
    bool isProcessExecuted(int currentTime){
        if(instrStartTime == -1){
            return false;
        }else{
            return instrStartTime+execTimeQueue.front() <= currentTime;
        }
    }

    // the process at the beginning of the wait queue is taken end placed at the end of the ready queue
    // processQueue: ready queue
    // currentTime: time of calling of this function
    void removeProcess(deque<Process> &processQueue,int currentTime){
        Process p = waitQueue.front();
        waitQueue.pop_front();
        printQueue(currentTime);
        execTimeQueue.pop_front();
        processQueue.push_back(p);
        if(waitQueue.empty()){
            instrStartTime = -1;
        }else{
            instrStartTime = currentTime;
        }
    }

    // time: simulation time of this print queue function
    // prints current status of the wait queue to the string named "output"
    void printQueue(int time){
        output += to_string(time) + "::HEAD-";
        if(waitQueue.empty()){
            output += "-";
        }else{
            for (auto &iter : waitQueue) {
                output += iter.name + "-";
            }
        }
        output += "TAIL\n";
    }

    // prints the content of the output string (contains the waiting queue information) to
    // a file if this string is not empty
    void print(){
        if(output.length() > 0){
            ofstream outFile("output_" + to_string(id)+".txt");
            outFile << output;
        }
    }
};

// class that implements round robin scheduling algorithm
class Scheduler{
public:
    deque<Process> processQueue; // ready queue
    int timeSlot = 100; // quantum for round robin

    CodeMap codeMap; // a structure to access code information easily

    vector<Semaphore> semaphoreList; // semaphore list to access semaphores by their ids

    vector<Printer> printerList; // printer list to access printers by their ids

    HardDrive hd; // hard drive

    Scheduler() : hd(12) // initializes hard drive
    {
        // initializes the semaphoreList. there are 10 semaphores
        for(int i=0;i<10;i++){
            Semaphore s(i);
            semaphoreList.push_back(s);
        }
        // initializes the printerList. there are 2 printers
        for(int i=10;i<12;i++){
            Printer pr(i);
            printerList.push_back(pr);
        }
    };

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
            //processQueue.pop_front();
            string codeName = p.code; // code file name of the process
            int currentLine = p.nextLine; // next line to execute
            string instrName; // name of the instruction to execute
            int execTime; // execution time of the instruction
            // it is set to true if this process executes waitS and semaphore is not available and
            // thus this process is placed into the wait queue
            // or process is placed in the wait queue of printer
            bool placedInWaitQueue = false;

            // if process has remaining time slot continue execution
            while(remainingTime>0){
                instrName = codeMap.getInstruction(codeName,currentLine);
                execTime = codeMap.getExecutionTime(codeName,currentLine);

                currentLine++;

                if(instrName.compare(0,5,"waitS")==0){//if instrName starts with waitS
                    // save the current state in case the semaphore is not available
                    processQueue.front().nextLine = currentLine;
                    int semaphoreNo = stoi(instrName.substr(6,1)); // the semaphore this process uses
                    // result of the waitS
                    int semaphoreValue = semaphoreList[semaphoreNo].waitS(processQueue,time+elapsedTime);
                    if(semaphoreValue == 0){ // if semaphore is not available
                        placedInWaitQueue = true;
                        // since this process is in the semaphore wait queue, break this instruction execution loop
                        break;
                    }
                }else if(instrName.compare(0,5,"signS")==0){ // if instrName starts with signS
                    int semaphoreNo = stoi(instrName.substr(6,1)); // the semaphore this process uses
                    // whether a process in the wait queue is waked up
                    bool processWakeup = semaphoreList[semaphoreNo].signS(processQueue,time+elapsedTime);
                    // if a process in the wait queue is waked up, ready
                    // queue is updated therefore print the ready queue
                    if(processWakeup){
                        printQueue(time+elapsedTime,outFile);
                    }
                }else if(instrName.compare(0,5,"dispM")==0){ // if instrName starts with dispM
                    int printerNo = stoi(instrName.substr(6,1)); // the printer id this process uses
                    processQueue.front().nextLine = currentLine; // save the current state of the process
                    // sends the process to wait queue
                    printerList[printerNo].addProcess(processQueue,execTime,time+elapsedTime);
                    placedInWaitQueue = true;
                    break;
                } else if(instrName.compare(0,5,"readM")==0){ // if instrName starts with readM
                    int blockNo = stoi(instrName.substr(6)); // the block number we want to read
                    processQueue.front().nextLine = currentLine; // save the current state of the process
                    // sends the process to wait queue if it is not in cache
                    bool isInCache = hd.read(blockNo,processQueue,execTime,time+elapsedTime);
                    if(isInCache){
                        // if block is in cache the execution time is zero
                        // since I update the remainingTime and elapsedTime later to make the execution time zero
                        // I added the reverse instructions that will make the change in remainingTime and elapsedTime
                        // in total zero
                        remainingTime += execTime;
                        elapsedTime -= execTime;
                    }else{
                        placedInWaitQueue = true;
                        break;
                    }
                }

                remainingTime -= execTime; // update the remaining time
                elapsedTime += execTime; // update the elapsed time

                // checks whether the processes of the printers and hard drive has finished
                for(int i=0;i<2;i++){
                    if(printerList[i].isProcessExecuted(time+elapsedTime)){
                        printerList[i].removeProcess(processQueue,time+elapsedTime);
                    }
                }

                if(hd.isProcessExecuted(time+elapsedTime)){
                    hd.removeProcess(processQueue,time+elapsedTime);
                }

                // if executed instruction is the last one (exit) break the loop
                // since there are no more instructions to execute
                if(instrName == "exit"){
                    break;
                }
            }


            // check whether a new process arrived during the execution of the process
            // "There will be always a process in the ready queue according to test definition files."
            // So no need to check process arrival from another place
            checkArrival(time,elapsedTime,processDef);

            // if the process is pushed to the wait queue of a semaphore or printer it is not in the
            // ready queue already so do not do anything. if not:
            // take the the current process from the head of the queue and place it to the end of it
            if(!placedInWaitQueue){
                processQueue.front().nextLine = currentLine;
                Process pr = processQueue.front();
                processQueue.pop_front();
                processQueue.push_back(pr);
            }

            // if the last instruction is exit pop (process is finished) pop the process from the queue
            if(instrName == "exit"){
                processQueue.pop_back();
            }

            printQueue(time+elapsedTime,outFile);

        }
        // else{}
        // I do not consider the case where queue is empty because of the announcement:
        // "There will be always a process in the ready queue according to test definition files.
        // So you don't have to worry about what to do in "idle CPU" state."

        return elapsedTime;
    }

    // adds the processes to the ready queue that arrived during the executing of a process
    // time: beginning of the execution
    // elapsedTime: time spent executing the process
    // processDef: process definition contains the processes that will arrive
    void checkArrival(int time,int elapsedTime, ProcessDefinition &processDef){
        int nextArrival = processDef.nextArrivalTime();
        // add the processes to the queue that arrived between time and time + execution time
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

    // prints wait queues
    void printWaitQueues(){
        for(int i=0;i<10;i++){
            semaphoreList[i].print();
        }
        for(int i=0;i<2;i++){
            printerList[i].print();
        }
        hd.print();
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

        ProcessDefinition processDef(definitionFilename); // an object that contains process definitions
        Scheduler scheduler; // an object for round robin scheduler

        ofstream outFile(outputFilename);

        // skip the time until the first arriving process
        time = processDef.nextArrivalTime();
        // add the first process to the queue
        scheduler.addProcess(processDef.getProcess());

        scheduler.printQueue(time,outFile);

        // execute process until there is not any arriving process and scheduler do not have any process to execute
        while(processDef.hasProcess() || scheduler.hasProcess()){
            // execute a process and increment time by execution time
            time += scheduler.executeProcess(time, outFile, processDef);
        }

        scheduler.printWaitQueues(); // prints the wait queues
    }
};

int main(int argc, char *argv[]){
    Simulator simulation;
    simulation.start();
    return 0;
}
