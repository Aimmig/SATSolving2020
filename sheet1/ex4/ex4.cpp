/*
*/

extern "C" {
    #include "ipasir.h"
}
#include <vector>
#include <tuple>
#include <math.h>
#include <iostream>

std::vector<std::tuple<int, int, int>> generateTripplets(int n){
	std::vector<std::tuple<int,int,int>> tripplets;
	int r,x,y,z = -1;
	for(int s=1;s<n;s++){
		for(int t=1; t<n; t++){
			r= (int) sqrt(2*s*t);
			if (r*r == 2*s*t){
				z = r+s+t;
				if (z>n) break;
				x = r+s;
				y = r+t;
				//if(!std::find(tripplets.begin(), tripplets.end(),
				//	     std::tuple<int,int,int>(y,x,z) != tripplets.end())){
					tripplets.push_back(std::tuple<int,int,int>(x,y,z));
					//std::cout<<x<<" "<<y<<" "<<z<<std::endl;
				//}
			}

		}
	}
	return tripplets;
}

void buildCNF(void* solver, std::vector<std::tuple<int,int,int>> tripplets){
	for (auto t : tripplets){
		ipasir_add(solver, std::get<0>(t));
		ipasir_add(solver, std::get<1>(t));
		ipasir_add(solver, std::get<2>(t));
		ipasir_add(solver, 0);
		ipasir_add(solver, -std::get<0>(t));
		ipasir_add(solver, -std::get<1>(t));
		ipasir_add(solver, -std::get<2>(t));
		ipasir_add(solver, 0);
	}
}

int sign(int x) {
    return (x > 0) - (x < 0);
}

#include <iostream>
#include <vector>

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;
	std::cout << "c This programm generates a pythagorean for 1..n" << std::endl;
	if (argc != 2) {
		puts("c USAGE: ./ex4 <n>");
		return 0;
	}
	int n = atoi(argv[1]);

	auto t = generateTripplets(n);
	void *solver = ipasir_init();
	buildCNF(solver, t);

	int satRes = ipasir_solve(solver);
	
	//std::cout << "c Number of variables: " << params[0] << std::endl;
	//std::cout << "c Number of clauses: " << params[1] << std::endl;
	
	if (satRes == 20) {
		std::cout << "c The input formula is unsatisfiable" << std::endl;
	}
	
	else if (satRes == 10) {
		std::cout << "c The input formula is satisfiable e.g:" << std::endl;
		for (int var = 1; var <= n; var++) {
			int variable_val = ipasir_val(solver, var);
			std::cout<<abs(variable_val)<<" "<<sign(variable_val)<<std::endl;
		}
		std::cout << std::endl;
	}
	return 0;
}
