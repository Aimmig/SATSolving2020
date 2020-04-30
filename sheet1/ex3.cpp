extern "C" {
    #include "ipasir.h"
}

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <vector>


int upperBound(int n, int k){
	return (n-1)/(k-1);
}


int addappartClauses(void* solver, int n, int k, int r){
	int res = 0;
	int maxInternal = n-(k-1)*r;
	std::vector<int> variables;
	for (int val = 1; val<=maxInternal; val++){
		for (int v = 0; v < k; v++){
			variables.push_back(val+v*r);
		}
		res+=2;
		for (auto l : variables){
			ipasir_add(solver, l);
		}
		ipasir_add(solver,0);
	
		for (auto l : variables){
			ipasir_add(solver, -l);
		}
		ipasir_add(solver,0);
		
		variables.clear();
	}
	return res;
}

void loadFormula(void* solver, int n, int k, int outVariables[]) {
	int maxR = upperBound(n,k);
	for (int r=1; r<=maxR; r++){
		outVariables[1] += addappartClauses(solver,n,k,r);
	}
	outVariables[0]=n;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 3) {
		puts("c USAGE: ./example <n> <k>");
		return 0;
	}

	void *solver = ipasir_init();
	int params[2] = {0,0};
	loadFormula(solver, atoi(argv[1]), atoi(argv[2]), params);

	//ipasir_assume(solver,1);
	int satRes = ipasir_solve(solver);
	
	std::cout << "c Number of variables: " << params[0] << std::endl;
	std::cout << "c Number of clauses: " << params[1] << std::endl;
	
	if (satRes == 20) {
		std::cout << "c The input formula is unsatisfiable" << std::endl;
	}
	
	else if (satRes == 10) {
		std::cout << "c The input formula is satisfiable" << std::endl;
		std::cout << "v ";
		for (int var = 1; var <= params[0]; var++) {
			int value = ipasir_val(solver, var);
			std::cout << value << " ";
		}
	}
	return 0;
}
