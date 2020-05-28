#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;

// Data structure for representing a clause
// Feel free to modify it
struct Clause {
    // Number of literals in the clause
    int numLits;
    // An array containing the literals
    int* lits;
};


int MAX_TRIES;
int MAX_FLIPS;
// Number of variables
int numVariables;
// Number of clauses
int numClauses;
// An array containing all the clauses
Clause* clauses;

// Print a clause (for Debugging purposes)
void printClause(const Clause& cls) {
    for (int li = 0; li < cls.numLits; li++) {
        printf("%d ", cls.lits[li]);
    }
    printf("0 \n");
}

// Used for parsing input CNF
int readNextNumber(FILE* f, int c) {
    while (!isdigit(c)) {
        c = fgetc(f);
    }
    int num = 0;
    while (isdigit(c)) {
        num = num*10 + (c-'0');
        c = fgetc(f);
    }
    return num;
}

// Used for parsing input CNF
void readLine(FILE* f) {
    int c = fgetc(f);
    while(c != '\n') {
        c = fgetc(f);
    }
}

// Used for parsing input CNF
bool loadSatProblem(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return false;
    }
    int c = 0;
    bool neg = false;
    int clauseInd = 0;
    vector<int> tmpClause;
    while (c != EOF) {
        c = fgetc(f);

        // comment line
        if (c == 'c') {
            readLine(f);
            continue;
        }
        // problem lines
        if (c == 'p') {
            numVariables = readNextNumber(f, 0);
            numClauses = readNextNumber(f, c);
            clauses = new Clause[numClauses];
            continue;
        }
        // whitespace
        if (isspace(c)) {
            continue;
        }
        // negative
        if (c == '-') {
            neg = true;
            continue;
        }

        // number
        if (isdigit(c)) {
            int num = readNextNumber(f, c);
            if (neg) {
                num *= -1;
            }
            neg = false;
            if (num == 0) {
                clauses[clauseInd].numLits = tmpClause.size();
                clauses[clauseInd].lits = new int[tmpClause.size()];
                for (size_t i = 0; i < tmpClause.size(); i++)
                    clauses[clauseInd].lits[i] = tmpClause[i];
                tmpClause.clear();
                clauseInd++;
            } else {
                tmpClause.push_back(num);
            }
        }
    }
    fclose(f);
    return true;
}
void initRandomAssignment(int* assignment){
     int random_bit = -1;
     for(int i=0; i<numVariables;i++){
         random_bit = rand() % 2;
         if (random_bit == 0){
             assignment[i] = -(i+1);
         }
         else{
             assignment[i] = i+1;
         }
     }
}

bool checkClause(int* assignment, const Clause& cls){
     for (int i=0; i<cls.numLits;i++){
         //printf("assig[i]:%d  cls[i]%d\n",assignment[abs(cls.lits[i])-1], cls.lits[i]);
         if(assignment[abs(cls.lits[i])-1] == cls.lits[i]) return true;
     }
     return false;
}

int getFlipLiteral(){
     return rand() % (numVariables)+1;
}

bool search() {
     int *assignment = new int[numVariables];
     for (int i=1; i<= MAX_TRIES; i++){
         initRandomAssignment(assignment);
         for (int j=1; j<=MAX_FLIPS; j++){
             bool sat = true;
             for (int c=0; c< numClauses; c++){
                  sat = sat && checkClause(assignment,clauses[c]);
             }
             if (sat){
                  printf("s SATISFIABLE\n");
                  printf("v ");
                  for (int u=0; u<numVariables; u++){
                      printf("%d ",assignment[u]);
                  }
                  printf("0\n");
                  return true;
             }
             else{
                 int l = getFlipLiteral();
                 //printf("now flipping literal%d\n", l);
                 assignment[l] *= -1;
             }
         }
         //printf("------------\n");
     }
     return false;
}

int main(int argc, char** argv) {
    printf("c This is <your-name>'s local search satisfiability solver\n");
    printf("c USAGE: ./<your-name> <cnf-formula-in-dimacs-format> <seed> <MAX_TRIES> <MAX_FLIPS>\n");
    if (argc != 5){
       printf("Incorrect number of arguments specified");
       return 1;
    }
    if (!loadSatProblem(argv[1])) {
        printf("ERROR: problem not loaded\n");
        return 1;
    }
    MAX_TRIES = atoi(argv[3]);
    MAX_FLIPS = atoi(argv[4]);
    srand (atoi(argv[2]));
    if(!search()){
       printf("c No solution found in %d tries & %d flips\n", MAX_TRIES, MAX_FLIPS);
       printf("s UNKNOWN");
    }
}
