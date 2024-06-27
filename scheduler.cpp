#include <bits/stdc++.h>
using namespace std;

string ALGORITHMS[6] = {"", "First Come First Serve", "Round Robin", "Shortest Job First", "Shortest Remaining Time First", "Highest Respnse Ratio Next"};
string operation;
int last_instant=0, process_count;
vector<pair<char, int>> algorithms;
struct Process{
    string name;
    int arrivalTime;
    int executionTime;

};
struct ProcessResponse {
    string name;
    double responseRatio;
    int executionTime;
};
vector<Process> processes;
vector<vector<string>>timeline;
unordered_map<string,int>processToIndex;
vector<int>finishTime;
vector<int>turnaroundtime;

bool compareProcesses(const Process& a, const Process& b) {
    return a.arrivalTime < b.arrivalTime;
}

bool sortByexecutiontime(const Process &a, const Process &b) {
    return a.executionTime < b.executionTime;
}

bool sortByArrivalTime(const Process &a, const Process &b) {
    return a.arrivalTime < b.arrivalTime;
}

bool sortByResponseRatio(const ProcessResponse &a, const ProcessResponse &b) {
    return a.responseRatio > b.responseRatio;
}



void clear_timeline()
{
    for(int i=0; i<last_instant; i++)
        for(int j=0; j<process_count; j++)
            timeline[i][j] = ' ';
}


void idleprocess(){
    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = processes[i].arrivalTime;
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i]!= processes[i].name)
                timeline[k][i] = '-';
        }
    }
}

void FCFS()
{
    int time = processes[0].arrivalTime;
    for (int i = 0; i < process_count; i++)
    {
        int PID = i;
        int arrivalTime = processes[i].arrivalTime;
        int executiontime = processes[i].executionTime;

        finishTime[PID] = (time + executiontime);
        turnaroundtime[PID] = (finishTime[PID] - arrivalTime);
        
        for (int j = time; j < finishTime[PID]; j++)
            timeline[j][PID] = processes[i].name;
        for (int j = arrivalTime; j < time; j++)
            timeline[j][PID] = '-';
        time += executiontime;
    }
}

void RoundRobin(int Quantum){
    queue<pair<int,int>>processqueue;//pair of process index and execution time
    int currentprocessindex=0;
    if(processes[currentprocessindex].arrivalTime==0){
        processqueue.push({currentprocessindex,processes[currentprocessindex].executionTime});
        currentprocessindex++;
    }
    int currentQuantum = Quantum;
    for(int time =0;time<last_instant;time++){
        if(!processqueue.empty()){
            int PID = processqueue.front().first;
            processqueue.front().second = processqueue.front().second-1;
            int remainingexecutiontime = processqueue.front().second;
            currentQuantum--;
            timeline[time][PID]=processes[PID].name;
            while(currentprocessindex<process_count && processes[currentprocessindex].arrivalTime==time+1){
                processqueue.push({currentprocessindex,processes[currentprocessindex].executionTime});
                currentprocessindex++;
            }

            if(currentQuantum==0 && remainingexecutiontime==0){
                processqueue.pop();
                finishTime[PID]=time+1;
                turnaroundtime[PID] = (finishTime[PID] - processes[PID].arrivalTime);
                
                currentQuantum=Quantum;
                
            }else if(currentQuantum==0 && remainingexecutiontime!=0){
                processqueue.pop();
                processqueue.push({PID,remainingexecutiontime});
                currentQuantum=Quantum;
            }else if(currentQuantum!=0 && remainingexecutiontime==0){
                processqueue.pop();
                finishTime[PID]=time+1;
                turnaroundtime[PID] = (finishTime[PID] - processes[PID].arrivalTime);
            
                
                currentQuantum=Quantum;
            }
        }
        while(currentprocessindex<process_count && processes[currentprocessindex].arrivalTime==time+1){
            processqueue.push({currentprocessindex,processes[currentprocessindex].executionTime});
            currentprocessindex++;
        }
    }
    idleprocess();
}

void SJF(){
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // pair of execution time and index
    int currentindex = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(currentindex<process_count && processes[currentindex].arrivalTime <= i){
            pq.push({processes[currentindex].executionTime, currentindex});
            currentindex++;
        }
        if (!pq.empty())
        {
            int PID = pq.top().second;
            pq.pop();
            int arrivalTime = processes[PID].arrivalTime;
            int executiontime = processes[PID].executionTime;
            

            
            for (int j = arrivalTime; j < i; j++)
                timeline[j][PID] = '-';

            
            for (int j = i; j < i + executiontime; j++)
                timeline[j][PID] = processes[PID].name;

            finishTime[PID] = (i + executiontime);
            turnaroundtime[PID] = (finishTime[PID] - arrivalTime);
            
            i += executiontime - 1;
        }
    }
}

void shortestremtimefirst(){
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j<process_count && processes[j].arrivalTime == i){
            pq.push({processes[j].executionTime, j});
            j++;
        }
        if (!pq.empty())
        {
            int PID = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            timeline[i][PID] = processes[PID].name;

            if (remainingTime == 1) // process finished
            {
                finishTime[PID] = i + 1;
                turnaroundtime[PID] = (finishTime[PID] - processes[PID].arrivalTime);
                
            }
            else
            {
                pq.push({remainingTime - 1, PID});
            }
        }
    }
    idleprocess();
}

void HRRN(){

    
    vector<ProcessResponse> readyqueue;
    int j=0;
    for (int current_instant = 0; current_instant < last_instant; current_instant++)
    {
        while(j<process_count && processes[j].arrivalTime<=current_instant){
            readyqueue.push_back({processes[j].name, 1.0, 0});
            j++;
        }
        // response ratio for every process
        for (auto &proc : readyqueue)
        {
            int PID = processToIndex[proc.name];
            int waiting_time = current_instant - processes[PID].arrivalTime;
            int execution_time = processes[PID].executionTime;
            proc.responseRatio = (waiting_time + execution_time)*1.0 / execution_time;
        }

        // Sort present processes by highest to lowest response ratio
        sort(readyqueue.begin(),readyqueue.end(), sortByResponseRatio);

        if (!readyqueue.empty())
        {
            int PID = processToIndex[readyqueue[0].name];
            while(current_instant<last_instant && readyqueue[0].executionTime != processes[PID].executionTime){
                timeline[current_instant][PID]=processes[PID].name;
                current_instant++;
                readyqueue[0].executionTime++;
            }
            current_instant--;
            readyqueue.erase(readyqueue.begin());
            finishTime[PID] = current_instant + 1;
            turnaroundtime[PID] = finishTime[PID] - processes[PID].arrivalTime;
            
        }
    }
    idleprocess();
}

void algoname(int algorithm_index)
{
    int algorithm_id = algorithms[algorithm_index].first - '0';
    if(algorithm_id==2)
        cout << ALGORITHMS[algorithm_id] <<" Quantum-"<<algorithms[algorithm_index].second <<endl;
    else
        cout << ALGORITHMS[algorithm_id] << endl;
}

void printProcesses()
{
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        cout << "|  " << processes[i].name << "  ";
    cout << "\n";
}
void printArrivalTime()
{
    cout << "Arrival time    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",processes[i].arrivalTime);
    cout<<"\n";
}
void printexecutiontime()
{
    cout << "burst time      ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",processes[i].executionTime);
    cout<<"\n";
}
void printFinishTime()
{
    cout << "Finish time     ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",finishTime[i]);
    cout<<"\n";
}
void printTurnAroundTime()
{
    cout << "Turnaround time ";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("|%3d  ",turnaroundtime[i]);
        sum += turnaroundtime[i];
    }
    
}


void printStats(int algorithm_index)
{
    algoname(algorithm_index);
    printProcesses();
    printArrivalTime();
    printexecutiontime();
    printFinishTime();
    printTurnAroundTime();
}

void printTimeline(int algorithm_index)
{
    cout <<"\n";
    for (int i = 0; i < process_count; i++)
    {
        // cout << processes[i].name << "     |";
        for (int j = 0; j < last_instant; j++)
        {
            cout << timeline[j][i]<<"|";
        }
        cout << " \n";
    }
}

void execute_algorithm(char algorithm_id, int quantum)
{
    switch (algorithm_id)
    {
    case '1':
        cout<<"First Come First Serve  ";
        FCFS();
        break;
    case '2':
        cout<<"Round Robin Quantum-"<<quantum<<"  ";
        RoundRobin(quantum);
        break;
    case '3':
        cout<<"Shortest Job First   ";
        SJF();
        break;
    case '4':
        cout<<"Shortest Remaining Time First   ";
        shortestremtimefirst();
        break;
    case '5':
        cout<<"Highest Response Ratio Next  ";
        HRRN();
        break;
    
    default:
        break;
    }
}

int main()
{
    string algorithm_chunk;
    cin >> algorithm_chunk>> process_count;
    size_t start = 0;
    size_t end = 0;

    while ((end = algorithm_chunk.find(',', start)) != string::npos) {
        string segment = algorithm_chunk.substr(start, end - start);
        size_t dash_pos = segment.find('-');
        
        char algorithm_id = segment[0];
        int quantum = (dash_pos != string::npos) ? stoi(segment.substr(dash_pos + 1)) : -1;
        
        algorithms.push_back({algorithm_id, quantum});
        start = end + 1;
    }
    
    // Handle the last segment (or only segment if there are no commas)
    string segment = algorithm_chunk.substr(start);
    size_t dash_pos = segment.find('-');
    
    char algorithm_id = segment[0];
    int quantum = (dash_pos != string::npos) ? stoi(segment.substr(dash_pos + 1)) : -1;
    
    algorithms.push_back({algorithm_id, quantum});
    for (int i = 0; i < process_count; ++i) {
        string process_chunk;
        cin >> process_chunk;

        size_t pos1 = process_chunk.find(',');
        size_t pos2 = process_chunk.find(',', pos1 + 1);

        string process_name = process_chunk.substr(0, pos1);
        int process_arrival_time = stoi(process_chunk.substr(pos1 + 1, pos2 - pos1 - 1));
        int process_execution_time = stoi(process_chunk.substr(pos2 + 1));

        processes.push_back({process_name, process_arrival_time, process_execution_time});
        processToIndex[process_name] = i;
    }
    sort(processes.begin(), processes.end(), compareProcesses);
    for (int i = 0; i < process_count; ++i) {
        processToIndex[processes[i].name] = i;
    }
    finishTime.resize(process_count);
    turnaroundtime.resize(process_count);
    for(int i=0;i<process_count;i++){
        last_instant=last_instant+processes[i].executionTime;
    }
    timeline.resize(last_instant);
    for(int i=0; i<last_instant; i++)
        for(int j=0; j<process_count; j++)
            timeline[i].push_back(" ");
    for (int idx = 0; idx < (int)algorithms.size(); idx++)
    {
        clear_timeline();
        execute_algorithm(algorithms[idx].first, algorithms[idx].second);
        printTimeline(idx);
        printStats(idx);
        cout << "\n";
    }
    return 0;
}