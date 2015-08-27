//
//  main.cpp
//  algorithm_proj3
//
//  Created by Xianshu Li on 5/10/15.
//  Copyright (c) 2015 Xianshu Li. All rights reserved.
//

#include <iostream>
#include <iostream>
#include <list>
#include <vector>
#include <stack>
#include <limits>
#include <fstream>
#include <string>
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
    int depend_weight;
    int processorID;
};

struct Processor {
    int processorID;
    bool busy;
    vector<Process> scheduled_process = vector<Process>{};
};
struct CompareWeight{
    bool operator()(const Process& left, const Process& right) {
        if (left.depend_weight > right.depend_weight) {
            return true;
        } else if (left.depend_weight == right.depend_weight) {
            if (left.execution_time >= right.execution_time) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
};

struct CompareID{
    bool operator()(const Process& left, const Process& right) {
        return left.processID < right.processID;
    }
};
struct CompareStart{
    bool operator()(const Process& left, const Process& right) {
        return left.start < right.start;
    }
};

class Graph {
public:
    bool baseline= false;
    int timestamp = 0;
    int time;
    int vertex_number;
    vector<Process> process_vec;
    bool has_cycle = false;
    vector<Process> candidate = vector<Process>{};
    vector<Process> unscheduled_process = vector<Process>{};
    vector<Processor> processor_vec = vector<Processor>{};
    vector<Process> in_processor = vector<Process>{};
public:
    void FindCandidate(void) {
        for (auto i = unscheduled_process.begin(); i < unscheduled_process.end(); ++i) {
            if (i->depend_list.size() == 0) {
                candidate.push_back(*i);
            }
        }
        for (auto &i : candidate) {
            for (auto j = unscheduled_process.begin(); j < unscheduled_process.end(); ++j) {
                if (i.processID == j->processID) {
                    Find_Path(*j);
                    i.depend_weight = j->depend_weight;
                }
            }
        }
        for (auto i : candidate) {
            for (auto j = unscheduled_process.begin(); j < unscheduled_process.end(); ++j) {
                if (j->processID == i.processID) {
                    unscheduled_process.erase(j);
                    --j;
                }
            }
        }
        if (baseline) {
            sort(candidate.begin(), candidate.end(), CompareID());
        } else {
            sort(candidate.begin(), candidate.end(), CompareWeight());
        }
//        for (auto i : candidate) {
//            cout << "ID:" << i.processID << " " << i.depend_weight  << endl;
//        }
    }
    void Find_Path_Visit(Process& u) {
        u.color = Gray;
        for (auto i : u.adj_list) {
            if (unscheduled_process[i - 1].color == White) {
                Find_Path_Visit(unscheduled_process[i -1]);
            }
        }
        u.color = Black;
    }
    void Find_Path(Process& u) {
        for (auto &i : unscheduled_process) {
            i.color = White;
        }
        u.depend_weight = 0;
        Find_Path_Visit(u);
        for (auto i : unscheduled_process) {
            if (i.color == Black) {
                u.depend_weight += i.execution_time;
            }
        }
    }
    void DFS_Visit(Process& u) {
        ++time;
        u.d = time;
        u.color = Gray;
        for (auto i : u.adj_list) {
            if (process_vec[i - 1].color == White) {
                DFS_Visit(process_vec[i - 1]);
            } else if (process_vec[i - 1].color == Gray) {
                has_cycle = true;
            }
        }
        u.color = Black;
        ++time;
        u.f = time;
    }
    void DFS(void) {
        for (auto &i : process_vec) {
            i.color = White;
        }
        time = 0;
        for (auto &u : process_vec) {
            if (u.color == White) {
                DFS_Visit(u);
            }
        }
    }
    void PrintProcess() {
        vector<Process> temp = process_vec;
        sort(temp.begin(), temp.end(), CompareStart());
        for (auto i : temp) {
            cout << "ID:" << i.processID << " Start:" << i.start << " Finish:" << i.finish << " Processor ID:" << i.processorID << endl;
        }
    }
    void ConstructGraph(void) {
        for (auto i : process_vec) {
            for (auto j : i.depend_list) {
                process_vec[j - 1].adj_list.push_back(i.processID);
            }
        }
        for (auto &i : process_vec) {
            i.start = 0;
            i.finish = i.execution_time;
//            i.depend_weight = i.execution_time;
        }
        unscheduled_process = process_vec;
        for (int i = 0; i < 3; ++i) {
            Processor p;
            p.processorID = i;
            p.busy = false;
            processor_vec.push_back(p);
        }
    }
    void ShowDependList(void) {
        for (auto i : process_vec) {
            cout << "ID:" << i.processID << " ";
            for (auto j : i.depend_list) {
                cout << j << " ";
            }
            cout << endl;
        }
    }
    void schedule() {
        timestamp = 0;
        while (unscheduled_process.size() != 0 || in_processor.size() != 0) {
            for (auto i = in_processor.begin(); i < in_processor.end(); ++i) {
                if (timestamp == i->start + i->execution_time) {//this process has finished
                    processor_vec[i->processorID].busy = false;
                    //delete this process from dependlist
                    //scan every process's dependlist in unscheduled_vec and delete the element equals to i->processID
                    for (auto it = unscheduled_process.begin(); it < unscheduled_process.end(); ++ it) {
                        for (auto it2 = it->depend_list.begin(); it2 < it->depend_list.end(); ++it2) {
                            if (*it2 == i->processID) {
                                it->depend_list.erase(it2);
                                --it2;
                            }
                        }
                    }
//                    delete the element in the adj_list, actually don't need it since won't have the candidate process in adj_list
                    for (auto it = unscheduled_process.begin(); it < unscheduled_process.end(); ++ it) {
                        for (auto it2 = it->adj_list.begin(); it2 < it->adj_list.end(); ++it2) {
                            if (*it2 == i->processID) {
                                it->adj_list.erase(it2);
                                --it2;
                            }
                        }
                    }
                    in_processor.erase(i);
                    --i;
//                    for (auto j = in_processor.begin(); j < in_processor.end(); ++j) {
//                        //remove from  in_processor
//                        if (j->processID == i->processID) {
//                            in_processor.erase(j);
//                            --i;
//                        }
//                    }
                }
            }
            FindCandidate();
            for (auto &i : processor_vec) {
                if (i.busy == false && candidate.size() != 0) {
                    process_vec[candidate[0].processID - 1].start = timestamp;
                    process_vec[candidate[0].processID - 1].finish = timestamp + process_vec[candidate[0].processID - 1].execution_time;
                    process_vec[candidate[0].processID - 1].processorID = i.processorID;
                    process_vec[candidate[0].processID - 1].depend_weight = candidate[0].depend_weight;
                    in_processor.push_back(process_vec[candidate[0].processID - 1]);
                    i.scheduled_process.push_back(process_vec[candidate[0].processID - 1]);
                    candidate.erase(candidate.begin());
                    i.busy = true;
                }

            }
            ++timestamp;
        }
        --timestamp;
    }
};
int main(int argc, const char * argv[]) {
    ifstream fin1(argv[1]);
    int process_number;
    fin1 >> process_number;
    Process p;
    Graph g;
    string s;
    //    g.matrix = vector<vector<int>>(process_number + 1, vector<int>(process_number + 1));
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
    g.ConstructGraph();
//    g.ShowDependList();
    Graph g2 = g;
    g2.baseline = true;
    g.DFS();
    if (g.has_cycle) {
        cout << "There is no feasible solution.\n";
    } else {
        g.schedule();
        g2.schedule();
        cout << "The T3 of my algorithm is:" << g.timestamp << endl;
        cout << "The T3B is:" << g2.timestamp << endl;
        cout << "The start time of each process in my algorithm is:" << endl;
        g.PrintProcess();
    }
    return 0;
}

