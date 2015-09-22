//
//  main.cpp
//  algorithm_proj2
//
//  Created by Xianshu Li on 5/8/15.
//  Copyright (c) 2015 Xianshu Li. All rights reserved.
//

#include <iostream>
#include <list>
#include <vector>
#include <stack>
#include <limits>
#include <fstream>
#include <string>
#include <stack>
#include <algorithm>
using namespace std;
enum Color{ White, Gray, Black};
struct Process {
    int processID;
    int execution_time;
    int start;
    int finish;
    vector<int> depend_list;
    vector<int> adj_list;
    Color color;
    int d;
    int f;
};
struct CompareStart{
    bool operator()(const Process& left, const Process& right) {
        return left.start < right.start;
    }
};
struct CompareFinish{
    bool operator()(const Process& left, const Process& right) {
        return left.finish < right.finish;
    }
};

struct Graph {
    int vertex_number;
    vector<Process> process_vec;
};

void DFS_Visit(Graph& g, Process& u, int& time, bool& has_cycle, stack<Process>& Stack) {
    ++time;
    u.d = time;
    u.color = Gray;
    for (auto i : u.adj_list) {
        if (g.process_vec[i - 1].color == White) {
            DFS_Visit(g, g.process_vec[i - 1], time, has_cycle, Stack);
        } else if (g.process_vec[i - 1].color == Gray) {
            has_cycle = true;
        }
    }
    u.color = Black;
    ++time;
    u.f = time;
    Stack.push(u);
}
bool DFS(Graph& g, stack<Process>& Stack) {
    bool has_cycle = false;
    for (auto &i : g.process_vec) {
        i.color = White;
    }
    int time = 0;
    for (auto &u : g.process_vec) {
        if (u.color == White) {
            DFS_Visit(g, u, time, has_cycle, Stack);
        }
    }
    return has_cycle;
}
void ConstructGraph(Graph& g) {
    for (auto i : g.process_vec) {
        for (auto j : i.depend_list) {
            g.process_vec[j - 1].adj_list.push_back(i.processID);
        }
    }
    for (auto &i : g.process_vec) {
        i.start = 0;
        i.finish = i.execution_time;
    }
}
bool LongestPath(Graph& g) {
    stack<Process> Stack;
    bool flag = DFS(g, Stack);
    while (!Stack.empty()) {
        auto u = Stack.top();
        Stack.pop();
        for (auto i : u.adj_list) {
            if(g.process_vec[i - 1].start < g.process_vec[u.processID - 1].start
               + g.process_vec[u.processID - 1].execution_time) {
                g.process_vec[i - 1].start = g.process_vec[u.processID - 1].start
                + g.process_vec[u.processID - 1].execution_time;
            }
        }
    }
    for (auto i = g.process_vec.begin(); i < g.process_vec.end(); ++i) {
        i->finish = i->start + i->execution_time;
    }
    return flag;
}
int main(int argc, const char * argv[]) {
    ifstream fin1(argv[1]);
    int process_number;
    fin1 >> process_number;
    Process p;
    Graph g;
    string s;
    int number;
    vector<Process> process_vec;
    char garbage;
    while (fin1 >> p.processID >> p.execution_time) {
        vector<int> depend_list;
        fin1 >> garbage;
        while (fin1 >> number) {
            char delimiter;
            depend_list.push_back(number);
            fin1 >> delimiter;
            if (delimiter == '}') {
                fin1.unget();
                break;
            }
        }
        if (fin1.fail()) {
            fin1.clear();
        }
        fin1 >> garbage;
        p.depend_list = depend_list;
        process_vec.push_back(p);
        g.process_vec = process_vec;
    }
    ConstructGraph(g);
    bool has_cycle = LongestPath(g);
    if (!has_cycle) {
        sort(g.process_vec.begin(), g.process_vec.end(), CompareStart());
        for (auto i : g.process_vec) {
            cout << "ID:" << i.processID << " StartTime:" << i.start << endl;
        }
        auto max_iterator = max_element(g.process_vec.begin(), g.process_vec.end(), CompareFinish());
        cout << "TN is " << max_iterator->finish << endl;
    } else {
        cout << "There is no feasible solution for given input." << endl;
    }
    return 0;
}

