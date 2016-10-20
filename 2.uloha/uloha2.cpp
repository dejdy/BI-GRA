#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <list>
#include <stdint.h>

using namespace std;
#endif /* __PROGTEST__ */

#include <sstream>
#define MAX 1000000

class CGraph
{
  public:
  CGraph() {};
  ~CGraph();
  bool bfs();
  bool dfs(int u);
  int compute();
  void initialize();

  int m, n;
  map<int, vector<int> > graph;
  int * distance, * pair_U, * pair_V;

};

void CGraph::initialize()
{
    distance = new int[m+1];
    pair_U = new int[m+1];
    pair_V = new int[n+1];

    for(int i = 0; i <= m; i++)
    {
        pair_U[i] = 0;
    }

    for(int i = 0; i <= n; i++)
    {
        pair_V[i] = 0;
    }
}

CGraph::~CGraph()
{
    delete [] distance;
    delete [] pair_U;
    delete [] pair_V;
}
/* Hopcroft algoritmus podle https://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm */

int CGraph::compute()
{
    int retVal = 0;
    while(bfs() == true)
    {
        for(int i = 1; i <= m; i++)
        {
            if(pair_U[i] == 0 && dfs(i)) retVal++;
        }
    }
    return retVal;
}

bool CGraph::bfs()
{
    queue<int> fronta;
    for(int i = 1; i<=m; i++)
    {
        if(pair_U[i] == 0)
        {
            fronta.push(i);
            distance[i] = 0;
        }
        else
        {
            distance[i] = MAX;
        }
    }

    distance[0] = MAX;

    while(!fronta.empty())
    {
        int cur = fronta.front();
        fronta.pop();
        if(distance[cur] < distance[0])
        {
            for(auto it = graph[cur].begin(); it != graph[cur].end(); it++)
            {
                int v = *it;
                if(distance[pair_V[v]] == MAX)
                {
                    distance[pair_V[v]] = distance[cur] + 1;
                    fronta.push(pair_V[v]);
                }
            }
        }
    }

    if(distance[0] != MAX) return true;
    return false;
}

bool CGraph::dfs(int u)
{
    if(u != 0)
    {
        for(auto it = graph[u].begin(); it != graph[u].end(); it++)
        {
            int v = *it;

            if(distance[pair_V[v]] == distance[u]+1)
            {
                if(dfs(pair_V[v]))
                {
                    pair_V[v] = u;
                    pair_U[u] = v;
                    return true;
                }
            }
        }

        distance[u] = MAX;
        return false;
    }
    return true;
}

void parseInput(const char * inFile, CGraph & g)
{
    ifstream infile(inFile);
    string line;
    int iteration = 0;
    while (getline(infile, line))
    {
        stringstream ss(line);
        if(iteration == 0) // reading first line
        {
            ss >> g.m >> g.n;
        }
        else
        {
            int num;
            while(ss >> num) // nacitani cisel
            {
                g.graph[iteration].push_back(num);
            }
        }
        iteration++;
    }
}

void printGraph(map<int, vector<int> > & graph)
{
    for(auto it = graph.begin(); it!=graph.end(); it++)
    {
        cout << it->first << " -> ";
        for(auto it2 = it->second.begin(); it2!=it->second.end(); it2++)
            cout << *it2 << " ";
        cout << endl;
    }
}

void printOutput(int res)
{
    cout << res << endl;
}

void printOutput(const char * outFile, int res)
{
    ofstream outfile(outFile);
    outfile << res << endl;
    outfile.close();
}

void rozvrh( const char * inFile, const char * outFile )
{
    CGraph g;
    parseInput(inFile, g);
    //printGraph(g.graph);
    g.initialize();
    printOutput(outFile, g.compute());
    //printOutput(g.compute());
}

#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
    rozvrh("test1.txt", "omg");
    return 0;
}
#endif /* __PROGTEST__ */
