/*
 * Generates pythagorean tripplet coloring for numbers 1....n
 *
 * Therefore all pythagorean tripplets up to n are needed
 * With these we can construct a CNF, that describes the problem:
 *
 * (x_a v x_b v x_c) (-x_a v -x_b v -x_c) for all a,b,c with a^2+b^2=c^2
 *
 * So we have at most n variables (one for each number <=n)
 * (probably it'll be some less because some numbers won't occur in any
 * pythagorean tripple
 *
 * For the number of clauses we have 2 clauses per tripplet, so:
 * 2*F(n) where F(n) is the number of all possible pythagorean tripplets <=n
 *
*/

extern "C" {
    #include "ipasir.h"
}
#include <set>
#include <tuple>
#include <math.h>
#include <iostream>

/*
 * Generate all pythagorean tripplets up to n
 */
std::set<std::tuple<int, int, int>> generateTripplets(int n){
	std::set<std::tuple<int,int,int>> tripplets;
	int r,x,y,z = -1;
	//int num = 0;
	for(int s=1;s<n;s++){
		for(int t=1; t<n; t++){
			r= (int) sqrt(2*s*t);
			if (r*r == 2*s*t){
				z = r+s+t;
				if (z>n) break;
				x = r+s;
				y = r+t;
				bool is_in = tripplets.find(std::tuple<int,int,int>(y,x,z)) != tripplets.end();
				if (!is_in){
					//num++;
					//std::cout<<x<<" "<<y<<" "<<z<<std::endl;
					tripplets.insert(std::tuple<int,int,int>(x,y,z));
				}
			}

		}
	}
	//std::cout<<num<<std::endl;
	return tripplets;
}


/*
 * Construct CNF as explained above
 */
void buildCNF(void* solver, std::set<std::tuple<int,int,int>> tripplets){
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

int getColor(int x) {
	if (x >= 1)
	    return 1;
	else return 0;
}

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
	
	if (satRes == 20) {
		std::cout << "c The input formula is unsatisfiable" << std::endl;
	}
	
	else if (satRes == 10) {
		std::cout << "c The input formula is satisfiable e.g:" << std::endl;
		for (int var = 1; var <= n; var++) {
			int variable_val = ipasir_val(solver, var);
			std::cout<<abs(variable_val)<<" "<<getColor(variable_val)<<std::endl;
		}
		std::cout << std::endl;
	}
	return 0;
}
