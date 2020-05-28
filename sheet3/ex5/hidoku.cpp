#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

//encode row,col = val
int encode(int row, int col, int val){ return 0;}

//get all neighbours if row,col =val
//std::vector<int> getNeighbours(int row, int col, int val){}

/*
//for sequential AMO constraint
int getAdditionalVar(int i,int j){
   return maxVar+i+(n-1)*j;
}

//sequential one hot encoding
void sequentialAMO(std::vector<int> set, int offset){
     //std::vector<int> e0(2, 0);
     -set[0];
     getAdditionalVar(1,offset);
     0

     for (int i=1; i< n-1;i++){
         ipasir_add(solver, -set[i]);
         ipasir_add(solver, getAdditionalVar(i+1,offset));
         ipasir_add(solver,0);
     
         ipasir_add(solver,-getAdditionalVar(i,offset));
         pasir_add(solver, getAdditionalVar(i+1,offset));
         ipasir_add(solver,0);
     
         ipasir_add(solver, -set[i]);
         ipasir_add(solver, -getAdditionalVar(i,offset));
         ipasir_add(solver,0);
     }
     
     ipasir_add(solver,-set[n-1]);
     ipasir_add(solver, -getAdditionalVar(n-1,offset));
     ipasir_add(solver,0);
}
*/

vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

// Used for parsing input CNF
bool loadSatProblem(const char* filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        string line;
        getline(file, line);
        auto v = split(line,':');
        auto lines = split(v[1],';');
        for (int i=0; i<lines.size();i++){
           cout<<lines[i]<<endl;
        }
    file.close();
    }
    //split line at :
    //first half: row,col

    //split second part at ;
    //each part of that is one line
    //split at , to get the numbers
    return true;
}

int main(int argc, char** argv) {
    printf("c This is <your-name>'s local search satisfiability solver\n");
    printf("c USAGE: ./<your-name> <cnf-formula-in-dimacs-format> <seed> <MAX_TRIES> <MAX_FLIPS>\n");
    if (argc != 2){
       printf("Incorrect number of arguments specified");
       return 1;
    }
    loadSatProblem(argv[1]);
}
