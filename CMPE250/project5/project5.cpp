/*
Student Name: Baran Kılıç
Student Number: 2014400123
Project Number: 5
Operating System: Xubuntu 14.04
Compile Status: Compiles
Program Status: Works correctly
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <queue>
#include <utility>

using namespace std;

class MyBinaryHeap{//min int heap with key change function
public:
    MyBinaryHeap(int capacity) : openSet(capacity,-1){
        pq.push_back(make_pair(0,0));//to leave index 0 empty
    }

    bool empty(){
        return currentSize == 0;
    }

    void push(pair<int,int> x){
        pq.push_back(x);
        currentSize++;
        openSet[x.second] = currentSize;
        percolateUp(currentSize);
    }

    void pop(){
        openSet[pq[1].second] = -1;
        swap(pq[1],pq[currentSize]);
        pq.pop_back();
        currentSize--;
        percolateDown(1);
    }

    void changeKey(int newValue, int index){
        int pqIndex = openSet[index];
        if(pq[pqIndex].first > newValue){//key decrease
            pq[pqIndex].first = newValue;
            percolateUp(pqIndex);
        }else{//key increase
            pq[pqIndex].first = newValue;
            percolateDown(pqIndex);
        }
    }

    pair<int,int> top(){
        return pq[1];
    };

    bool inSet(int index){
        return openSet[index] != -1;
    }


private:
    int currentSize = 0;
    vector<pair<int,int>> pq;
    vector<int> openSet;

    void percolateDown(int hole){
        int child;
        for( ; hole*2 <= currentSize; hole = child){
            child = hole*2;
            if(child != currentSize && pq[child+1].first < pq[child].first){
                child++;
            }
            if(pq[child].first < pq[hole].first){
                swap(openSet[pq[hole].second], openSet[pq[child].second]);
                swap(pq[hole],pq[child]);
            }else{
                break;
            }
        }
    }

    void percolateUp(int hole){
        for( ; hole > 1 && pq[hole].first < pq[hole/2].first; hole/=2 ){
            swap(openSet[pq[hole].second], openSet[pq[hole/2].second]);
            swap(pq[hole], pq[hole/2]);
        }
    }
};

//source: https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
int aStarSearch(int start, int goal, unsigned int numVertex, const vector<int> &distToGoal, const vector<vector<pair<int,int>>> &graph){
    vector<bool> closedSet(numVertex,false);// The set of nodes already evaluated
    MyBinaryHeap pq(numVertex);

    // For each node, the cost of getting from the start node to that node
    vector<int> gScore(numVertex,numeric_limits<int>::max());
    gScore[start] = 0;

    // For each node, the total cost of getting from the start node to the goal
    // by passing by that node. That value is partly known, partly heuristic.
    vector<int> fScore(numVertex,numeric_limits<int>::max());
    // For the first node, that value is completely heuristic.
    fScore[start] = distToGoal[start];

    // Initially, only the start node is known
    pq.push(make_pair(fScore[start],start));//first:fScore, second:vertexNum

    while(!pq.empty()){//while openSet is not empty
        int current = pq.top().second;//the node in openSet having the lowest fScore[] value
        if(current == goal){
            return gScore[current];
        }

        pq.pop();//openSet remove current
        closedSet[current] = true;//closedSet add current
        for(int i=0;i<graph[current].size();i++){//for each neighbor of current
            int neighbour = graph[current][i].first;
            if(closedSet[neighbour] == true){
                continue;// Ignore the neighbor which is already evaluated.
            }

            // The distance from start to a neighbor
            int tentativeGScore = gScore[current] + graph[current][i].second;
            if(!pq.inSet(neighbour)){//(neighbor not in openSet) Discover a new node
                gScore[neighbour] = tentativeGScore;
                fScore[neighbour] = gScore[neighbour] + distToGoal[neighbour];

                pq.push(make_pair(fScore[neighbour],neighbour));
            }else if(tentativeGScore >= gScore[neighbour]){
                continue;// This is not a better path.
            }else{
                // This path is the best until now. Record it!
                gScore[neighbour] = tentativeGScore;
                fScore[neighbour] = gScore[neighbour] + distToGoal[neighbour];
                pq.changeKey(fScore[neighbour],neighbour);
            }
        }
    }
    //return failure
}

int main(int argc, char *argv[]){

    ifstream inputFile(argv[1]);
    unsigned int numVertex,numRoad;
    inputFile >> numVertex >> numRoad;
    vector<int> distToGoal(numVertex);
    for(int i=0;i<numVertex;i++){
        inputFile >> distToGoal[i];
    }
    int n1,n2,weight;
    vector<vector<pair<int,int>>> graph(numVertex);
    for(int i=0;i<numRoad;i++){
        inputFile >> n1 >> n2 >> weight;
        graph[n1].push_back(make_pair(n2,weight));
        graph[n2].push_back(make_pair(n1,weight));
    }
    int startNode,stopNode;
    inputFile >> startNode >> stopNode;
    inputFile.close();

    int shortestPath = aStarSearch(startNode,stopNode,numVertex,distToGoal,graph);

    ofstream outFile(argv[2]);
    outFile << shortestPath;

    return 0;
}