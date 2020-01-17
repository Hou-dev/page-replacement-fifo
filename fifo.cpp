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
                    pageFaults++;
                    if(s.find(nobandQ.front()) == s.end()){
                    s.insert(nobandQ.front());
                    cout<<"Put Page "<< nobandQ.front() <<" into the memory."<<endl;
                    fifoQ.push(nobandQ.front());
                    nobandQ.pop();
                }
                }
                //if the buffer is not full but number does not already belong in buffer we add it
                if(s.find(number) == s.end()){
                    pageFaults++;
                    s.insert(number);
                    cout<<"Put Page "<< number <<" into the memory."<<endl;
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

int main(int argc, char *argv[]){
    string a;
    string b;
    string c;
    a = argv[1];
    b = argv[2];
    c = argv[3];
    readFile(a,b,c);
    pagedFIFO();
    return 0;
}