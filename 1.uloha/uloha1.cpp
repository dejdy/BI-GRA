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

int globalIndex;

class Node
{
  public:
    Node(string);
    string name;
    int index;
    int lowlink;
};

Node::Node(string n)
{
    name = n;
    index = -1;
    lowlink = -1;
}

struct Term
{
    string strTerm;
    vector<string> cells;
    list<string> operands;
    list<string> st;
};

void tarjanAlgorithm(map<string, Node*> & tarjanMap, map<string, struct Term *> & graphMap, Node * node, list<vector<Node*> > & scc, list<Node*> & s);

void loadFile(const char * inFile, vector<string> & fileLines)
{
    ifstream infile(inFile);
    string line;
    while (getline(infile, line))
    {
        fileLines.push_back(line);
    }
}

void deleteTerms(map<string, struct Term *> & graphMap)
{
    for(auto it = graphMap.begin(); it!=graphMap.end(); it++)
        delete it->second;
}

void deleteNodes(list<vector<Node*> > & l)
{
    for(auto it = l.begin(); it!=l.end(); it++)
    {
        for(unsigned int i = 0; i < it->size(); i++)
            delete it->at(i);
    }
}

void findCells(struct Term * term)
{
    for(unsigned int i = 0; i < term->strTerm.size(); i++)
    {
        if((term->strTerm[i] == ' ' || term->strTerm[i] == '+' || term->strTerm[i] == '*' || term->strTerm[i] == '-' || term->strTerm[i] == '/') && (term->strTerm.find(':')!=string::npos))
        {
            string s = "";
            if(term->strTerm[i] != ' ')
            {
                s.push_back(term->strTerm[i]);
                term->operands.push_front(s);
            }
            continue;
        }
        string str = "";

        while(i<term->strTerm.size() && term->strTerm[i]!=' ')
        {
            str += term->strTerm[i];
            i++;
        }
        term->cells.push_back(str);
        term->operands.push_front(str);
    }
}

void parseInput(vector<string> & fileLines, map<string, struct Term *> & graphMap, map<int, string> & orderMap, set<string> & nodes)
{
    int m, n;
    sscanf(fileLines[0].c_str(), "%dx%d", &m, &n); // Loading first line
    char tempLine [10000];
    char tempName [10000];
    for(unsigned int i=1; i<fileLines.size(); i++)
    {
        struct Term * term = new Term();
        sscanf(fileLines[i].c_str(), "%[1234567890:] = %[1234567890+-*/: ]", tempName, tempLine);
        string str(tempLine);
        string strName(tempName);
        term->strTerm = str;
        findCells(term);
        nodes.insert(strName);
        graphMap[strName] = term;
        orderMap[i-1] = strName;
    }
}

void printMap(map<string, struct Term *> & graphMap, map<int, string> & orderMap)
{
    for(unsigned int i = 0; i < graphMap.size(); i++)
    {
        cout << "Key: " << orderMap[i] << " value: " << graphMap[orderMap[i]]->strTerm << endl;
        cout << "Terms: ";
        for(unsigned int i = 0; i < graphMap[orderMap[i]]->cells.size(); i++)
            cout << graphMap[orderMap[i]]->cells[i] << " ";
        cout << endl << endl;
    }
}

/*
    Tarjanuv algoritmus podle https://algoritmy.net/article/1515/Tarjanuv-algoritmus
*/
list<vector<Node*> > executeTarjan(map<string, struct Term *> & graphMap)
{
    list<Node *> s; //stack
    list<vector<Node*> > scc;
    map<string, Node*> tarjanMap;

    for(auto it=graphMap.begin(); it!=graphMap.end(); it++)
    {
        Node * newNode = new Node(it->first);
        tarjanMap[it->first] = newNode;
    }

    for(auto it2=tarjanMap.begin(); it2!=tarjanMap.end(); it2++)
    {
        if(it2->second->index == -1) tarjanAlgorithm(tarjanMap, graphMap, it2->second, scc, s);
    }

    return scc;
}

/*
    Tarjanuv algoritmus podle https://algoritmy.net/article/1515/Tarjanuv-algoritmus
*/
void tarjanAlgorithm(map<string, Node*> & tarjanMap, map<string, struct Term *> & graphMap, Node * node, list<vector<Node*> > & scc, list<Node*> & s)
{
    node->index = globalIndex;
    node->lowlink = globalIndex;
    globalIndex++;

    s.push_back(node);

    for(unsigned int i = 0; i<graphMap[node->name]->cells.size(); i++)    //Pro kazdeho potomka node
    {
        if(graphMap[node->name]->cells[i].find(":")!=string::npos && tarjanMap[graphMap[node->name]->cells[i]]->index == -1) // pokud je to uzel a ma index -1
        {
            tarjanAlgorithm(tarjanMap, graphMap, tarjanMap[graphMap[node->name]->cells[i]], scc, s);
            node->lowlink = min(node->lowlink, tarjanMap[graphMap[node->name]->cells[i]]->lowlink); //uprav lowlink otce
        }
        else if (graphMap[node->name]->cells[i].find(":")!=string::npos && find(s.begin(), s.end(), tarjanMap[graphMap[node->name]->cells[i]])!=s.end()) //pokud komponenta nebyla jiz uzavrena)
        {
            node->lowlink = min(node->lowlink, tarjanMap[graphMap[node->name]->cells[i]]->index);
        }
    }

    if (node->lowlink == node->index)
    {
        Node * n;
        vector<Node*> component;
        do
        {
            n = s.back();
            s.pop_back();
            component.push_back(n);
        }
        while(n!=node);

        scc.push_back(component);
    }

}

void printComponents(list<vector<Node*> > comp)
{
    for(auto it = comp.begin(); it!=comp.end(); it++)
    {
        for(unsigned int i = 0; i < it->size(); i++)
        {
            cout << it->at(i)->name << " ";
        }
        cout << endl;
    }
}

void mapOutputCycles(map<string, string> & output, list<vector<Node*> > & comp, set<string> & nodes, set<string> & ret, set<string> & cycles)
{
    for(auto it = comp.begin(); it!=comp.end(); it++)
    {
        if(it->size()>1) // Pokud je v komponente vice uzlu nez jeden, namapujeme vystup #cycle
        {
            for(unsigned int i = 0; i < it->size(); i++)
            {
                output[it->at(i)->name] = "#cycle";
                cycles.insert(it->at(i)->name);
                ret.insert(it->at(i)->name);
                nodes.erase(it->at(i)->name);
            }
        }
    }
}

void mapOutputInvalid(map<string, string> & output, set<string> & nodes, map<string, struct Term *> & graphMap, set<string> & cycleNodes)
{
    set<string> tempNodes;
    for(auto it = nodes.begin(); it!=nodes.end(); ++it)
    {
        tempNodes.insert(*it);
    }

    for(auto it = tempNodes.begin(); it!=tempNodes.end(); ++it) // Pro vsechny node, ktere nejsou #cycle
    {
        for(unsigned int i = 0; i<graphMap[*it]->cells.size(); i++) // pokud je nektery z nich v nejakem cyklu
        {
            if(find(cycleNodes.begin(), cycleNodes.end(), graphMap[*it]->cells[i])!=cycleNodes.end())
            {
                output[*it] = "#invalid";
                nodes.erase(*it);
            }
        }
    }
}

void printOutput(map<string, string> & output, map<int, string> & orderMap)
{
    for(auto it = orderMap.begin(); it!=orderMap.end(); it++)
    {
        cout << it->second << " = " << output[it->second] << endl;
    }
}

void printOutput(map<string, string> & output, map<int, string> & orderMap, const char * path)
{
    ofstream outfile(path);
    for(auto it = orderMap.begin(); it!=orderMap.end(); it++)
    {
        outfile << it->second << " = " << output[it->second] << endl;
    }
    outfile.close();
}

bool isNum(string s)
{
    if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-'))) return false;

    char * p ;
    strtol(s.c_str(), &p, 10);

    return (*p == 0);
}

string evaluateNode(string node, map<string, struct Term *> & graphMap, set<string> & cycles)
{
    if(cycles.find(node) != cycles.end()) return "#invalid";
    if(node.find('#')!=string::npos) return "#invalid";
    if(isNum(node)) return node;
    struct Term * t = graphMap[node];
    if(t->cells.size()==1 && t->cells[0].find(':')==string::npos) // pokud je v termu jen cislo
        return t->cells[0];
    if(t->cells.size()==1) return evaluateNode(t->cells[0], graphMap, cycles);



    for (auto it = graphMap[node]->operands.begin(); it != graphMap[node]->operands.end(); it++)
    {
        if(it->find('#')!=string::npos) return "#invalid";
        if(it->find(':')!=string::npos || isNum(*it)) // if is cell or num
        {
            graphMap[node]->st.push_back(*it);
        }
        if(*it == "+")
        {
            string x = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(x.find('#')!=string::npos) return "#invalid";
            string y = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(y.find('#')!=string::npos) return "#invalid";
            string res1 = evaluateNode(x, graphMap, cycles);
            string res2 = evaluateNode(y, graphMap, cycles);
            if(res1.find('#')!=string::npos || res2.find('#')!=string::npos) return "#invalid";
            int res3 = stoi(res1) + stoi(res2);
            graphMap[node]->st.push_back(to_string(res3));
        }
        if(*it == "*")
        {
            string x = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(x.find('#')!=string::npos) return "#invalid";
            string y = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(y.find('#')!=string::npos) return "#invalid";
            string res1 = evaluateNode(x, graphMap, cycles);
            string res2 = evaluateNode(y, graphMap, cycles);
            if(res1.find('#')!=string::npos || res2.find('#')!=string::npos) return "#invalid";
            int res3 = stoi(res1) * stoi(res2);
            graphMap[node]->st.push_back(to_string(res3));
        }
        if(*it == "-")
        {
            string x = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(x.find('#')!=string::npos) return "#invalid";
            string y = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(y.find('#')!=string::npos) return "#invalid";
            string res1 = evaluateNode(x, graphMap, cycles);
            string res2 = evaluateNode(y, graphMap, cycles);
            if(res1.find('#')!=string::npos || res2.find('#')!=string::npos) return "#invalid";
            int res3 = stoi(res1) - stoi(res2);
            graphMap[node]->st.push_back(to_string(res3));
        }
        if(*it == "/")
        {
            string x = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(x.find('#')!=string::npos) return "#invalid";
            string y = graphMap[node]->st.back();
            graphMap[node]->st.pop_back();
            if(y.find('#')!=string::npos) return "#invalid";
            string res1 = evaluateNode(x, graphMap, cycles);
            string res2 = evaluateNode(y, graphMap, cycles);
            if(res1.find('#')!=string::npos || res2.find('#')!=string::npos) return "#invalid";
            if(stoi(res2) == 0)
            {
                return "#zero";
            }
            int finalRes;
            if((stoi(res1)<0 && stoi(res2)>0) || (stoi(res1)>0 && stoi(res2)<0))
            {
                float res3 = (float)stoi(res1) / (float)stoi(res2);
                res3 = res3 - 1.0f;
                finalRes = (int) res3;
            }
            else
            {
                int res3 = stoi(res1) / stoi(res2);
                finalRes = res3;
            }
            graphMap[node]->st.push_back(to_string(finalRes));

            //int res3 = stoi(res1) / stoi(res2);
            //graphMap[node]->st.push_back(to_string(res3));
        }
    }
    return graphMap[node]->st.back();
}

void evaluate(map<string, struct Term *> & graphMap, map<string, string> & outputMap, set<string> & nodes, set<string> & cycles)
{
    for(auto it = nodes.begin(); it!=nodes.end(); it++)
    {
        string res = evaluateNode(*it, graphMap, cycles);
        outputMap[*it] = res;
    }
}

void finalCheck(map<string, struct Term *> & graphMap, map<string, string> & outputMap)
{
    for(auto it = outputMap.begin(); it!= outputMap.end(); it++)
    {
        if(it->second == "#zero") // tak musime iterovat po jeho operandech a koukam, jestli jsou vsechny validni
        {
            struct Term * t = graphMap[it->first];
            for(unsigned int i = 0; i < t->cells.size(); i++)
            {
                if(t->cells[i].find(':')==string::npos) continue; // pokud obsahuje jen cislo

                if( outputMap[t->cells[i]].find('#')!=string::npos)
                {
                    outputMap[it->first] = "#invalid";
                }
            }
        }
    }
}

void audit( const char * inFile, const char * outFile )
{
    globalIndex = 0;
    vector<string> fileLines;
    map<string, struct Term *> graphMap;
    map<int, string> orderMap;
    map<string, string> outputMap;
    set<string> nodes;
    set<string> cycleNodes;
    set<string> cycles;

    loadFile(inFile, fileLines);
    parseInput(fileLines, graphMap, orderMap, nodes);
    list<vector<Node*> > components = executeTarjan(graphMap);
    mapOutputCycles(outputMap, components, nodes, cycleNodes, cycles);
    mapOutputInvalid(outputMap, nodes, graphMap, cycleNodes);
    evaluate(graphMap, outputMap, nodes, cycles);
    finalCheck(graphMap, outputMap);

    //printOutput(outputMap, orderMap);
    printOutput(outputMap, orderMap, outFile);

    deleteTerms(graphMap);
    deleteNodes(components);
}


#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
    audit("test3.txt", "hm3.txt");
    cout << endl;
    audit("test2.txt", "hm2.txt");
    cout << endl;
    audit("test1.txt", "hm1.txt");
    return 0;
}
#endif /* __PROGTEST__ */
