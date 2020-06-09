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

int width, height, I,T,S,L,O, numHelperVars;
int maxVar, currentMaxHelper, numParts;

//encode row,col = val
int encode(int row, int col, int val){
    return (val-1)*width*height + col + width*row +1;
}

/*int decodeCol(int var, int sizes){
    return (var-1)% (sizes);
}

int decodeRow(int var, int sizes){
    return ((var-1 - decodeCol(var,sizes)) % (sizes*sizes))/sizes;
}

int decodeVal(int var, int sizes){
    return 1+ (((var-1 - decodeCol(var,sizes))/sizes) - decodeRow(var,sizes))/sizes;
}*/

int getNumVars(){
    return maxVar+ width*height*(numParts-1);
}

int getNumClauses(){
   return width*height*(2+3*(numParts-2)); 
}

//get all neighbours indexes for row,col
/*std::vector<tuple<int,int>> getNeighbours(int row, int col){
   int startW = (col - 1 < 0) ? col : col-1;
   int startH = (row - 1 < 0) ? row : row-1;
   int endW   = (col + 1 >= width) ? col : col+1;
   int endH   = (row + 1 >= height) ? row : row+1;
   
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
   //printf("------\n");
   return neighbours;
}*/

//for sequential AMO constraint
int getAdditionalVar(int j){
  return currentMaxHelper + j;
}

void printHeader(){
     printf("p cnf %d %d\n", getNumVars(),getNumClauses());
}

//sequential one hot encoding
void sequentialAMO(std::vector<int> set){
     printf("%d %d %d\n", -set[0],getAdditionalVar(1),0);
     
     for (int i=1; i< numHelperVars;i++){
         printf("%d %d %d\n", -set[i],getAdditionalVar(i+1),0);
         printf("%d %d %d\n", -getAdditionalVar(i),getAdditionalVar(i+1),0);
         printf("%d %d %d\n", -set[i],-getAdditionalVar(i),0);
     }
     
     printf("%d %d %d\n", -set[numHelperVars],-getAdditionalVar(numHelperVars),0);
     currentMaxHelper +=numHelperVars;
}

/*void encodeRules(){
    std::vector<int> set(width*height,0);
    for (int i=0; i< height; i++){
       for (int j=0; j< width; j++){
           for (int val=1;val<=width*height;val++){
              set[val-1] = encode(i,j,val);
           }
           sequentialAMO(set);
           //printf("%d--- %d\n",i,j);
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
    for (int val=1;val<=width*height;val++){
       for (int i=0; i< height; i++){
            for (int j=0; j< width; j++){
                printf("%d ", encode(i,j,val));
            }
        }
        printf("0\n");
    }
}*/

/*vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}*/

/*
void encodeO(int row, int col, int val){
     printf("%d ", -encode(row,col,val));
     for (r=0;r<row;r++){
         
     }
}*/

void encodeI(int row, int col, int val){
     if (col+3 >= height){
         printf("%d 0\n", -encode(row,col,val));
     }
     else{
         for (int c=0; c<height; c++){
              for (int r=0; r<width; r++){
                 if(r!=row){
                    printf("%d %d 0\n",  -encode(row,col,val), -encode(r,c,val));
                 }
                 else{
                    if(c>col && c<=col+3){
                         printf("%d %d 0\n",  -encode(row,col,val), encode(r,c,val));
                    }
                    else{
                      if(c!=col){
                         printf("%d %d 0\n",  -encode(row,col,val), -encode(r,c,val));
                      }
                    }
                 }
              }
          }
     }
}

/*
void decode(char* filename, int size){
    std::ifstream file(filename);
    if (file.is_open()) {
       string line;
       while (getline(file, line)){
           if (line[0] == 'v'){
              auto vars = split(line,' ');
              int row,col,val;
              int solution[size][size];
              for (int i=1;i<=size*size*size*size;i++){
                 int var = stoi(vars[i]);
                 if (var > 0){
                    row = decodeRow(var,size);
                    col = decodeCol(var,size);
                    val = decodeVal(var,size);
                    solution[row][col] = val;
                    }
               }
               string res = "sol:";
               for (int u=0; u<size;u++){
                  for(int v=0; v<size;v++){
                     res += to_string(solution[u][v]);
                     res += ",";
                  }
                  res.pop_back();
                  res += ";";
               }
               printf(res.c_str());
               return;
           }
       }
    }
}*/

// Used for parsing input CNF
bool loadSatProblem() {
    vector<int> set(width*height,0);
    for (int r=0; r<width; r++){
       for(int c=0; c<height; c++){
           for(int t=0; t<numParts; t++){
                set[t]=encode(r,c,t);
           }
           //sequentialAMO(set);
           int currentPart = 0;
           for (int p=currentPart; p<I;p++){
               encodeI(r,c,p);
           }
       }
    }
    /*
    currentPart +=I;
    printf("T:\n");
    for (int p=currentPart; p<I+T;p++){
         printf("%d\n",p);
    }
    currentPart +=T;
    printf("S:\n");
    for (int p=currentPart; p<I+T+S;p++){
         printf("%d\n",p);
    }
    currentPart +=S;
    printf("L:\n");
    for (int p=currentPart; p<I+T+S+L;p++){
         printf("%d\n",p);
    }
    currentPart +=L;
    printf("O:\n");
    for (int p=currentPart; p<I+T+S+L+O;p++){
         printf("%d\n",p);
    }*/
    return false;
}

int main(int argc, char** argv) {
    if (argc == 8){
       height = atoi(argv[1]);
       width = atoi(argv[2]);
       I = atoi(argv[3]);
       T = atoi(argv[4]);
       S = atoi(argv[5]);
       L = atoi(argv[6]);
       O = atoi(argv[7]);
       numParts = I+T+S+L+O;
       printf("%d\n",numParts);
       maxVar = height*width*numParts;
       currentMaxHelper = maxVar;
       numHelperVars = numParts-1;
       printHeader();
       loadSatProblem();
   }
   return -1;
}
