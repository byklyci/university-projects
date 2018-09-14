/*
Student Name: Baran Kılıç
Student Number: 2014400123
Project Number: 4
Operating System: Xubuntu 14.04
Compile Status: Compiles
Program Status: Runs
Notes: Anything you want to say about your code that will be helpful in the grading process.

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

using namespace std;

bool depthFirstSearch(int source,int from,int to,int minEdge,int vertex,vector<bool> visited,vector<vector<vector<int>>> &graph,set<int> &attackedDistrict,vector<int> &minEdges){
    visited[vertex] = true;
    if(source!=vertex && attackedDistrict.find(vertex) != attackedDistrict.end()){//this vertex is a attacked vertex
        minEdges.push_back(minEdge);
        for(int i=0;i<graph[from].size();i++){//delete this min edge from graph
            if(graph[from][i][0]==to){
                graph[from].erase(graph[from].begin()+i);
                break;
            }
        }
        for(int i=0;i<graph[to].size();i++){
            if(graph[to][i][0]==from){
                graph[to].erase(graph[to].begin()+i);
                break;
            }
        }
        //cout << "attacked vertex " << vertex << " source " << source << " from " << from << " to " << to << " min edge: " << minEdge << endl;
        return true;
    }
    for(int i=0;i<graph[vertex].size();i++){
        int destination = graph[vertex][i][0];//adjacent vertex
        if(!visited[destination]){
            if(graph[vertex][i][1]<minEdge){
                //minEdge = graph[vertex][i][1];    from = vertex;  to = destination;
                if(depthFirstSearch(source,vertex,destination,graph[vertex][i][1],destination,visited,graph,attackedDistrict,minEdges)){
                    return true;
                }
            }else{
                if(depthFirstSearch(source,from,to,minEdge,destination,visited,graph,attackedDistrict,minEdges)){
                    return true;
                }
            }
        }
    }
    return false;
}

int main(int argc, char *argv[]){
    int numOfVertex,numOfAttackedDistrict;
    ifstream inputFile(argv[1]);
    inputFile >> numOfVertex >> numOfAttackedDistrict;
    vector<vector<vector<int>>> graph(numOfVertex);
    int n1,n2,weight;
    for(int i=1; i<numOfVertex ;i++){
        inputFile >> n1 >> n2 >> weight;
        vector<int> edge;
        edge.push_back(n2);
        edge.push_back(weight);
        graph[n1].push_back(edge);
        edge[0] = n1;
        graph[n2].push_back(edge);
    }
    set<int> attackedDistrict;
    for(int i=0;i<numOfAttackedDistrict;i++){
        inputFile >> n1;
        attackedDistrict.insert(n1);
    }
    inputFile.close();

    vector<bool> visited(numOfVertex,false);
    set<int>::iterator itStartingVertex = attackedDistrict.begin();
    vector<int> minEdges;

    while(minEdges.size()<numOfAttackedDistrict-1){
        while(depthFirstSearch(*(itStartingVertex),-1,-1,numeric_limits<int>::max(),*(itStartingVertex),visited,graph,attackedDistrict,minEdges)){

        }
        itStartingVertex++;
    }

    long long int minTimeToCutCommunication = 0;
    for(int i=0;i<minEdges.size();i++){
        minTimeToCutCommunication += minEdges[i];
    }

    ofstream outFile(argv[2]);
    outFile << minTimeToCutCommunication;

    return 0;
}