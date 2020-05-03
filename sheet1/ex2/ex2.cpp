extern "C" {
    #include "ipasir.h"
}

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


/*
 *
 */
int encode(int n, int row, int col, int val){
	return (1+n*(row)+(col))+(val)*n*n;
}

int encode_block(int u, int j, int b, int i, int k,int n, int s){
	return 1+u+(j)*n+b*k+n*n*(i)+s*n*k;
}

void addBlockClauses(void* solver, int num, int k){
	int n =k*k;
	int var = 0;
	for (int s=0;s<k;s++){
		for (int b=0;b<k;b++){
			for(int j=0; j<k;j++){
				for (int u=0; u<k; u++){
					var = encode_block(u,j,b,num,k,n,s);
					std::cout<<var<<" ";
					ipasir_add(solver,var);
				}
			}
			ipasir_add(solver,0);
			std::cout<<std::endl;
		}
	}
	std::cout<<"----"<<std::endl;
}

/*
 *
 */
void addRules(void* solver, int k){
	int n=k*k;
	for (int i=0; i<n;i++){
		for (int r=0; r<n; r++){
			//clause for rows
			for (int c=0; c<n; c++){
				int var = encode(n,r,c,i);
				std::cout<<var<<" ";
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
			std::cout<<std::endl;
			//clauses for colums
			for (int c=0; c<n; c++){
				int var = encode(n,c,r,i);
				std::cout<<var<<" ";
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
			std::cout<<std::endl;
		}
		std::cout<<"----"<<std::endl;
		addBlockClauses(solver, i,k);
	}
}

/*
 *
 */
void assumeCell(void* solver, int n, int row, int col, int val){
	int var = encode(n, row, col, val);
	ipasir_assume(solver, var);
}

/*
 * Read the graph from file & encode the coloring problem
 * returns vector with all edges
 */
int betterFile(void* solver, const char* filename, int* outVariables){
	std::ifstream infile(filename);
	std::string line;
	std::getline(infile,line);
	int k = std::stoi(line);
	int n = k*k;
	int val = 0;
	
	addRules(solver,k);
	std::cout<<"Solving sudok of size "<<k<<std::endl;
	
	for (int row =1; row<=n; row++){
		std::getline(infile,line);
		std::istringstream iss(line);
		std::string part;
		for (int col=1; col<=n; col++){
			iss >> part;
			val = std::stoi(part);
			if (val){
				std::cout<<val<<" ";
				assumeCell(solver, k, row, col, val);
			}
		}
		std::cout<<std::endl;
	}
	return 0;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 2) {
		puts("c USAGE: ./example <file.sudo>");
		return 0;
	}

	void *solver = ipasir_init();
	int k = 0;
	betterFile(solver, argv[1], &k);
	
	/*if (!loaded) {
		std::cout << "c The input formula " << argv[1] << " could not be loaded." << std::endl;
		return 0;
	}
	else {
		std::cout << "c Loaded, solving" << std::endl;
	}*/

	std::cout<<"Now solving sudo"<<std::endl;
	int satRes = ipasir_solve(solver);

	
	if (satRes == 20) {
		std::cout<<"Invalid sudo, not solvable"<<std::endl;
	}
	
	if (satRes == 10) {
		std::cout<<"Solutin for sudoku found"<<std::endl;
		//only request values for even variables (odd are activations literals)
		/*for (int var = 2; var <= variables; var+=2) {
			value = ipasir_val(solver, var);
			if (value > 0){
				vertex = decode_vertex(value,numVertices);
				color = decode_col(value,numVertices);
				std::cout <<"v "<< vertex <<" "<< color<<std::endl;
			}
		}*/
	}
	return 0;
}
