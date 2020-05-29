#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>

using namespace std;

int width, height,maxVar, numHelperVars, num_units;
int currentMaxHelper;

//encode row,col = val
int encode(int row, int col, int val){
    return (val-1)*width*height + col + width*row +1;
}

int getNumVars(){
    return maxVar + width*height*(width*height-1);
}

int getNumClauses(){
   return num_units + width*height*(1+2+3*(width*height-2))+width*height*(width*height-1);
}

//get all neighbours indexes for row,col
std::vector<tuple<int,int>> getNeighbours(int row, int col){
   int startW = (col - 1 < 0) ? col : col-1;
   int startH = (row - 1 < 0) ? row : row-1;
   int endW   = (col + 1 > width) ? col : col+1;
   int endH   = (row + 1 > height) ? row : row+1;

   vector<tuple<int,int>> neighbours;
   // See how many are alive
   for (int rowNum=startH; rowNum<=endH; rowNum++) {
       for (int colNum=startW; colNum<=endW; colNum++) {
           if (colNum == col && rowNum == row){
               continue;
           }
           neighbours.push_back(tuple<int,int>(rowNum,colNum));
       }
   }
   return neighbours;
}

//for sequential AMO constraint
int getAdditionalVar(int j){
  return currentMaxHelper + j;
}

void printHeader(std::vector<int> unit_clauses){
     printf("p cnf %d %d\n", getNumVars(),getNumClauses());
     for (auto var: unit_clauses){
         printf("%d 0\n", var);
     }
}

//sequential one hot encoding
void sequentialAMO(std::vector<int> set){
     for(int j=0; j<width*height;j++){
         printf("%d ",set[j]);
     }
     printf("0\n");
     
     printf("%d %d %d\n", -set[0],getAdditionalVar(1),0);

     for (int i=1; i< numHelperVars;i++){
         printf("%d %d %d\n", -set[i],getAdditionalVar(i+1),0);
         printf("%d %d %d\n", -getAdditionalVar(i),getAdditionalVar(i+1),0);
         printf("%d %d %d\n", -set[i],-getAdditionalVar(i),0);
     } 
     
     printf("%d %d %d\n", -set[numHelperVars],-getAdditionalVar(numHelperVars),0);
     currentMaxHelper +=numHelperVars;
}

void encodeRules(){
    std::vector<int> set(width*height,0);
    for (int i=0; i< height; i++){
       for (int j=0; j< width; j++){
           for (int val=1;val<=width*height;val++){
              set[val-1] = encode(i,j,val);
           }
           sequentialAMO(set);
           auto ne = getNeighbours(i,j);
           int row, col;
           for (int val=1; val<width*height;val++){
               printf("%d ",-encode(i,j,val));
               //printf("%d:%d ",i,j);
               for (auto t: ne){
                  row = get<0>(t);
                  col = get<1>(t);
                  printf("%d ",encode(row,col,val+1));
                  //printf("%d:%d ",row,col);
                  //printf("%d ,%d, %d, %d;",encode(row,col,val+1), row, col ,val+1);
               }
               printf("0\n");
           }
       }
    }
}



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
        file.close();
        auto v = split(line,':');
        auto sizes = split(v[0], ',');
        width= stoi(sizes[0]);
        height = stoi(sizes[1]);
        maxVar = width*height*width*height;
        currentMaxHelper = maxVar;
        numHelperVars = width*height -1;
        auto lines = split(v[1],';');
        std::vector<int> units;
        for (int i=0;i<height;i++){
           auto row = split(lines[i],',');
           for (int j=0; j<width; j++){
              if (row[j].compare("0") != 0){
                  int val =  stoi(row[j]);
                  int var = encode(i,j,val);
                  units.push_back(var);
              }
           }
        }
        num_units = units.size();
        printHeader(units);
        return true;
    }
    return false;
}

int main(int argc, char** argv) {
    printf("c This is <your-name>'s local search satisfiability solver\n");
    printf("c USAGE: ./<your-name> <cnf-formula-in-dimacs-format> <seed> <MAX_TRIES> <MAX_FLIPS>\n");
    if (argc != 2){
       printf("Incorrect number of arguments specified");
       return 1;
    }
    loadSatProblem(argv[1]);
    encodeRules();
}
