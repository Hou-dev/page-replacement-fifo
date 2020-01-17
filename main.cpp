#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <list>
#include <algorithm>
using namespace std;

//variable declearations
string algorithm; //Algo type
int memorySize; //Size of memory  use for Algo
string fileName; //Name of file
vector<string>inputCommands; //vector containing all command read for input file

void readFile(string algo,string memsize,string fname){
    //variables for reading
    algorithm = algo;
    memorySize = atoi(memsize.c_str());
    fileName = fname;
    string inpStr;
    ifstream inpfile;
    inpfile.open(fname);
    //open file and push the commands into vectors
    while(!inpfile.eof()){
        getline(inpfile,inpStr,' ');
        inputCommands.push_back(inpStr);
    }
}

void pagedFIFO(){
    cout<<"Running FIFO"<<endl;
    cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    int delays; //delay counter
    int pageFaults; //page fault counter
    int pageReference; //page reference counter
    queue<int>fifoQ; //queue for fifo
    queue<int>nobandQ; //queue for full fifo queue
    set<int> s;// this set make sure there aren't any duplicates in the buffer
    //reading file and parsing inputs
    for(int i = 0; i < inputCommands.size();i++){
        int number;
        string waitsignal;
        //parse inputs into two varibles
        number = atoi(inputCommands[i].substr(0,1).c_str());
        waitsignal = inputCommands[i].substr(2,2);
        cout<<number<<" : "<<waitsignal<<": ";
        if(waitsignal == "a"){
            //if bandwith is available and there is space in buffer, add number
            if(fifoQ.size() < memorySize){
                if(nobandQ.size() > 0){
                    if(s.find(nobandQ.front()) == s.end()){
                    s.insert(nobandQ.front());
                    cout<<"Put Page "<< nobandQ.front() <<" into the memory."<<endl;
                    pageFaults++;
                    fifoQ.push(nobandQ.front());
                    nobandQ.pop();
                }
                }
                //if the buffer is not full but number does not already belong in buffer we add it
                if(s.find(number) == s.end()){
                    s.insert(number);
                    cout<<"Put Page "<< number <<" into the memory."<<endl;
                    pageFaults++;
                    fifoQ.push(number);
                }
                //if the number is in the buffer we continue
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                }
                
            }
            //if the buffer is full 
            else if(fifoQ.size() == memorySize){
                pageReference++;
                if(s.find(number) == s.end() ){
                    int var = fifoQ.front();
                    //remove from fifo
                    cout<<"Remove Page "<<var<< " from the memory."<<endl;
                    fifoQ.pop();
                    s.erase(var);
                    s.insert(number);
                    cout<<"Put Page "<< number <<" into the memory"<<endl;
                    //put in to fifo
                    fifoQ.push(number);
                    pageFaults++;
                }
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                }
            }      
        }
        else if (waitsignal == "n"){
            //there is no bandwidth so we add to queue or continue if number if already in buffer
            cout<<"Tries to put Page "<< number<< " into the memory, but air bandwidth is not available."<<endl;
            if(s.find(number) == s.end() ){
                //push reference to a fifo queue to be reference later
                nobandQ.push(number);
                delays++;
            }
            else{
                cout<<"Found Page "<<number<<" in memory, so not queued."<<endl;
            }
        }
        //cout<<inputCommands[i].substr(2,2)<<endl;
    }
    cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
    cout<<"Report:"<<endl;
    cout<<"The total number of page references: "<<pageReference<<endl;
    cout<<"The number of page faults: "<<pageFaults<<endl;
    cout<<"The number of delayed page replacements due to unavailable air bandwidth: "<<delays<<endl;
}

void pagedLRU(){
    cout<<"Running LRU"<<endl;
    cout<<"==============================================================================="<<endl;
    int delays; //delay counter
    int pageFaults; //page fault counter
    int pageReference; //counter for page refernce.
    list<int>LRU; //queue for lru
    queue<int>nobandQ; //queue for full fifo queue
    set<int> s; //this set makes sure there isnt any duplicates in the buffer
    //reading file and parsing inputs
    for(int i = 0; i < inputCommands.size();i++){
        int number;
        string waitsignal;
        //parse inputs into two varibles
        number = atoi(inputCommands[i].substr(0,1).c_str());
        waitsignal = inputCommands[i].substr(2,2);
        cout<<number<<" : "<<waitsignal<<": ";
        if(waitsignal == "a"){
            //if there is banwidth available we add to buffer
            if(LRU.size() < memorySize){
                if(nobandQ.size() > 0){
                    if(s.find(nobandQ.front()) == s.end() ){
                        s.insert(nobandQ.front());
                        cout<<"Put Page "<< nobandQ.front() <<" into the memory."<<endl;
                        pageFaults++;
                        LRU.push_front(nobandQ.front());
                        nobandQ.pop();
                    }
                }
                //if there is space in buffer and the number isnt in buffer yet we add it
                else if (s.find(number) == s.end()){
                    s.insert(number);
                    cout<<"Put Page "<< number <<" into the memory."<<endl;
                    pageFaults++;
                    LRU.push_back(number);
                }
                //this portion is used to reorder LRU
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                    list<int>::iterator it = find(LRU.begin(), LRU.end(), number);
                    LRU.erase(it);
                    LRU.push_back(number);
                }
            }
            //if there is no space in buffer we remove the LRU
            else if(LRU.size() == memorySize){
                pageReference++;
                //adds the new number to set and performs LRU
                if(s.find(number) == s.end() ){
                    int var = LRU.front();
                    cout<<"Remove Page "<<var<< " from the memory."<<endl;
                    LRU.pop_front();
                    s.erase(var);
                    s.insert(number);
                    cout<<"Put Page "<< number <<" into the memory"<<endl;
                    LRU.push_back(number);
                    pageFaults++;
                }
                //continue if number is in the buffer
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                    list<int>::iterator it = find(LRU.begin(), LRU.end(), number);
                    LRU.erase(it);
                    LRU.push_front(number);
                }

            }
        }
        //no bandwidth if available so we add to fifo queue or continue if found in buffer
        else if (waitsignal == "n"){
            cout<<"Tries to put Page "<< number<< " into the memory, but air bandwidth is not available."<<endl;
            if(s.find(number) == s.end() ){
                nobandQ.push(number);
                delays++;
            }
            //continue if number is already in buffer
            else{
                cout<<"Found Page "<<number<<" in memory, so not queued."<<endl;
            }
        }

    }
    cout<<"Report:"<<endl;
    cout<<"The total number of page references: "<<pageReference<<endl;
    cout<<"The number of page faults: "<<pageFaults<<endl;
    cout<<"The number of delayed page replacements due to unavailable air bandwidth: "<<delays<<endl;
    cout<<"==============================================================================="<<endl;
}

//predictor is used to look ahead and remove the necessary number
int predict(vector<string>&cmd,vector<int>&fr, int number, int index){
    int far = 0;
    int loc = index;
    for(int j = 0; j<fr.size();j++){
        for(int i = index; i<cmd.size();i++){
            if(fr[j] != atoi(cmd[i].substr(0,1).c_str())){
                if( i+1 == cmd.size()){
                    far = j;
                }
            } 
        }
    }
    return far;
} 

void pagedOPT(){
    cout<<"Running OPT"<<endl;
    cout<<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<endl;
    int delays; //delay counter
    int pageFaults; //page fault counter
    int pageReference; //page reference counter
    vector<int>OPT; //queue for OPT
    queue<int>nobandQ; //queue for no banwidth
    set<int> s; //set to check if number is in buffer
    //reading file and parsing inputs
    for(int i = 0; i < inputCommands.size();i++){
        int number;
        string waitsignal;
        //parse inputs into two varibles
        number = atoi(inputCommands[i].substr(0,1).c_str());
        waitsignal = inputCommands[i].substr(2,2);
        cout<<number<<" : "<<waitsignal<<": ";
        if(waitsignal == "a"){
            if(OPT.size() < memorySize){
                //if there is space in buffer we add it in
                if(nobandQ.size() > 0){
                    if(s.find(nobandQ.front()) == s.end() ){
                        s.insert(nobandQ.front());
                        cout<<"Put Page "<< nobandQ.front() <<" into the memory."<<endl;
                        pageFaults++;
                        OPT.push_back(nobandQ.front());
                        nobandQ.pop();
                    }
                }
                //if there is space, we add it if number is not there in the buffer
                else if (s.find(number) == s.end()){
                    s.insert(number);
                    cout<<"Put Page "<< number <<" into the memory."<<endl;
                    pageFaults++;
                    OPT.push_back(number);
                }
                //contnue if there is number is already there
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                }
            }
            //if the buffer is full, we perform OPT removal
            else if(OPT.size() == memorySize){
                pageReference++;
                if(s.find(number) == s.end() ){
                   //predict is return the location of the replacement
                   int j = predict(inputCommands,OPT,number,i);
                   //cout<<"J: "<<j<<endl;
                   cout<<"Remove Page "<<OPT[j]<< " from the memory."<<endl;
                   s.erase(OPT[j]);
                   OPT[j] = number;
                   s.insert(number);
                   //this will put the new page into memory
                   cout<<"Put Page "<< number <<" into the memory"<<endl;
                   pageFaults++;
                /*int var = OPT.front();
                                        cout<<"Remove Page "<<var<< " from the memory."<<endl;
                                        OPT.pop_front();
                                        s.erase(var);
                                        s.insert(number);
                                        cout<<"Put Page "<< number <<" into the memory"<<endl;
                                        OPT.push_back(number);
                                        pageFaults++;*/
                }
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                }
            }
        }
        else if (waitsignal == "n"){
            //there is no bandwidth
            cout<<"Tries to put Page "<< number<< " into the memory, but air bandwidth is not available."<<endl;
            if(s.find(number) == s.end() ){
                //adds to fifo queue
                nobandQ.push(number);
                delays++;
            }
            else{
                //does not add to queue if found
                cout<<"Found Page "<<number<<" in memory, so not queued."<<endl;
            }
            
        }

    }
    cout<<"Report:"<<endl;
    cout<<"The total number of page references: "<<pageReference<<endl;
    cout<<"The number of page faults: "<<pageFaults<<endl;
    cout<<"The number of delayed page replacements due to unavailable air bandwidth: "<<delays<<endl;
    cout<<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<endl;
}

int main(int argc, char *argv[]){
    string a;
    string b;
    string c;
    a = argv[1];
    b = argv[2];
    c = argv[3];
    readFile(a,b,c);
    if(a == "LRU"){
        pagedLRU();
    }
    else if(a == "OPT"){
        pagedOPT();
    }
    else if (a == "FIFO"){
        pagedFIFO();
    }
    
    return 0;
}