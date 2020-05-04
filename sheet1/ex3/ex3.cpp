/*
 Calculate number of variables and clauses for
 checking wether W(2,k) > n

 Using the encoding of the lecture we have:
 - for each integer 1,...n a variable x_i
 - some clauses to exclude all possibility of k equally spaced

 So trivally we have just n variables

 Calculating the clauses is a bit tricky
 Dividing the clauses in subset S_r, such that each subset
 exludes just the possibility spaced r appart

 1,2,.......,k	   1,3,..,1+r*k
 2,3,......,k+1    ....
 .............	   ....
 n-(k-1),....,n	   n-(k-1)*2,...,n    n-(k-1)*r
*/


extern "C" {
    #include "ipasir.h"
}

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <vector>

/* The upper for equally spacing k from n */
int upperBound(int n, int k){
	return (n-1)/(k-1);
}

/*
 * Add all clauses that exclude the equally spaced items
 * with distant r
 */
int addappartClauses(void* solver, int n, int k, int r){
	int res = 0;
	//biggest starting index of variables for clauses
	int maxInternal = n-(k-1)*r;
	std::vector<int> variables;
	for (int val = 1; val<=maxInternal; val++){
		for (int v = 0; v < k; v++){
			variables.push_back(val+v*r);
		}
		//add these claues positive & negative
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

/*
 * Adds all clauses for encoding the problem
 */
void loadFormula(void* solver, int n, int k, int outVariables[]) {
	int maxR = upperBound(n,k);
	for (int r=1; r<=maxR; r++){
		outVariables[1] += addappartClauses(solver,n,k,r);
	}
	outVariables[0]=n;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;
	std::cout << "c This programm checks if W(2,k) > n" << std::endl;
	if (argc != 3) {
		puts("c USAGE: ./example <n> <k>");
		return 0;
	}

	void *solver = ipasir_init();
	int params[2] = {0,0};
	int n = atoi(argv[1]);
	int k = atoi(argv[2]);
	loadFormula(solver, n, k, params);

	int satRes = ipasir_solve(solver);
	
	std::cout << "c Number of variables: " << params[0] << std::endl;
	std::cout << "c Number of clauses: " << params[1] << std::endl;
	
	if (satRes == 20) {
		std::cout << "c The input formula is unsatisfiable" << std::endl;
		std::cout << "c This means W(2," << k << ") > " << n << " is false" << std::endl;
	}
	
	else if (satRes == 10) {
		std::cout << "c The input formula is satisfiable" << std::endl;
		std::cout << "v ";
		for (int var = 1; var <= params[0]; var++) {
			int value = ipasir_val(solver, var);
			std::cout << value << " ";
		}
		std::cout << std::endl;
		std::cout << "c This means W(2," << k << ") > " << n << " is true" << std::endl;
	}
	return 0;
}
