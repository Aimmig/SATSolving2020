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

void search() {
    // search for satisfiable solution until found or forever
    // print the solution if found
}


int main(int argc, char** argv) {
    printf("c This is <your-name>'s local search satisfiability solver\n");
    printf("c USAGE: ./<your-name> <cnf-formula-in-dimacs-format>\n");
    if (!loadSatProblem(argv[1])) {
        printf("ERROR: problem not loaded\n");
        return 1;
    }
    search();
    // if your program gets to this line, it solved the problem.
    printf("s SATISFIABLE\nv ");
}
