/*
Student Name: Baran Kılıç
Student Number: 2014400123
Project Number: 3
Operating System: Xubuntu 14.04
Compile Status:
Program Status:
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

struct Event {
    int node;
    double finishTime;
    Event(int node_,double finishTime_){
        node = node_;
        finishTime= finishTime_;
    }
};

struct comparatorEvent {
    bool operator() (Event e1,Event e2){
        return e1.finishTime > e2.finishTime;
    }
};

int main(int argc, char *argv[]){

    int n,m;
    ifstream inputFile(argv[1]);
    inputFile >> n >> m;

    vector<double> processes;
    for(int i=0;i<n;i++){
        double value;
        inputFile >> value;
        processes.push_back(value);
    }

    int indegree[n];
    for(int i=0;i<n;i++){
        indegree[i]=0;
    }
    vector<vector<int>> graph(n);
    int from,to;
    for(int i=0; i<m;i++){
        inputFile >> from >> to;
        indegree[to]++;
        graph[from].push_back(to);
    }
    inputFile.close();

    priority_queue<Event, vector<Event>, comparatorEvent> events;
    for(int i=0; i<n; i++){
       if(indegree[i]==0){
           Event e(i,processes[i]);
           events.push(e);
       }
    }

    double time = 0;
    int count = 0;
    while(!events.empty()){
        Event current = events.top();
        events.pop();
        time = current.finishTime;
        int node = current.node;
        count++;

        for(int i=0; i<graph[node].size(); i++){
            int index = graph[node][i];
            indegree[index]--;
            if(indegree[index]==0){
                Event e(index,time+processes[index]);
                events.push(e);
            }
        }
    }

    if(count<n){
    	time = -1.0;
    }

    ofstream outFile(argv[2]);
    outFile << fixed << time;

    return 0;
}