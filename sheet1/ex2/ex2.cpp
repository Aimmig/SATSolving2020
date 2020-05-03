extern "C" {
    #include "ipasir.h"
}

#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


/*
 * Encodes that the cell at row,col has given val in sudoku
 */
int encode(int n, int row, int col, int val){
	return 1+n*(row)+col+(val)*n*n;
}

/*
 * Get the column of a given literal
 */
int decode_col(int n, int val){
	int res =val%n;
	return res > 0 ? res : n;
}

/*
 * Get the row of a given literal
 */
int decode_row(int n, int val){
	return ((((val-1)%(n*n))-decode_col(n, val)+1)/n) + 1;
}

/*
 * Get the sudoku value for a given literal
 */
int decode_val(int n, int val){
	return 1+(((val-1)-decode_col(n,val)+1)/n)/n;
}

/*
 *Encodes variables for a blocEncodes variables for a block
 */
int encode_block(int u, int j, int b, int i, int k,int n, int s){
	return 1+u+(j)*n+b*k+n*n*(i)+s*n*k;
}

/*
 *Add all variables that correspond to the sudoku block constraint
 *e.g in each k*k block there has to be each number
 */
void addBlockClauses(void* solver, int num, int k){
	int n =k*k;
	int var = 0;
	for (int s=0;s<k;s++){
		for (int b=0;b<k;b++){
			for(int j=0; j<k;j++){
				for (int u=0; u<k; u++){
					var = encode_block(u,j,b,num,k,n,s);
					//std::cout<<var<<" ";
					ipasir_add(solver,var);
				}
			}
			ipasir_add(solver,0);
			//std::cout<<std::endl;
		}
	}
	//std::cout<<"----"<<std::endl;
}

/*
 *Add conflicting clauses to sovler
 *e.g. on each field there can only be one number
 */
void addConflictingClauses(void* solver, int k){
	int n=k*k;
	int l1 = 0;
	int l2 = 0;
	for (int row=0; row<n; row++){
		for (int col=0; col<n; col++){
			for (int n1=0; n1<n; n1++){
				for (int n2=0; n2<n1; n2++){
					l1 = -encode(n,row,col,n1);
					l2 = -encode(n,row,col,n2);
					//std::cout<<l1<<" "<<l2<<std::endl;
					ipasir_add(solver,l1);
					ipasir_add(solver,l2);
					ipasir_add(solver,0);
				}
			}
		}
	}
}

/*
 *Add all the sudoku rules
 */
void addRules(void* solver, int k){
	int n=k*k;
	for (int i=0; i<n;i++){
		for (int r=0; r<n; r++){
			//clause for rows
			for (int c=0; c<n; c++){
				int var = encode(n,r,c,i);
				//std::cout<<var<<" ";
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
			//std::cout<<std::endl;
			//clauses for colums
			for (int c=0; c<n; c++){
				int var = encode(n,c,r,i);
				//std::cout<<var<<" ";
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
			//std::cout<<std::endl;
		}
		//std::cout<<"----"<<std::endl;
		//clauess for blocks
		addBlockClauses(solver, i,k);
	}
	//no 2 numbers on same field
	addConflictingClauses(solver,k);
}

/*
 * Assume that at row,col the sudoku value is the given val
 */
void assumeCell(void* solver, int n, int row, int col, int val){
	int var = encode(n, row, col, val);
	ipasir_assume(solver, var);
}

/*
 * Read the graph from file & encode the coloring problem
 * returns vector with all edges
 */
int betterFile(void* solver, const char* filename){
	std::ifstream infile(filename);
	std::string line;
	std::getline(infile,line);
	int k = std::stoi(line);
	int n = k*k;
	int val = 0;
	
	addRules(solver,k);
	std::cout<<"c Solving Sudoku of size "<<k<<std::endl;
	
	for (int row =0; row<n; row++){
		std::getline(infile,line);
		std::istringstream iss(line);
		std::string part;
		for (int col=0; col<n; col++){
			iss >> part;
			val = std::stoi(part);
			if (val){
				val--;
				//std::cout<<"Assuming cell"<<"at "<<row+1<<" "<<col+1<<"with "<<val+1<<std::endl;
				assumeCell(solver, n, row, col, val);
			}
		}
		//std::cout<<std::endl;
	}
	return k;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 2) {
		std::cout<<"c USAGE: ./example <SSudoku-file>"<<std::endl;
		return 0;
	}

	void *solver = ipasir_init();
	
	int k = betterFile(solver, argv[1]);
	int n = k*k;
	int maxVar = (int)std::pow(n,3);
	int satRes = ipasir_solve(solver);

	
	if (satRes == 20) {
		std::cout<<"c Invalid Sudoku, not solvable"<<std::endl;
	}
	
	if (satRes == 10) {
		int solution[n][n];
		int value,col,row,res = 0;
		std::cout<<"c Solution for Sudoku found"<<std::endl;
		for (int var = 1; var <= maxVar; var+=1) {
			value = ipasir_val(solver, var);
			if (value > 0){
				row = decode_row(n,value)-1;
				col = decode_col(n,value)-1;
				res = decode_val(n,value);
				solution[row][col] = res;
			}
		}
		for (int row = 0; row < n ; row++){
			std::cout<<"c ";
			for (int col = 0; col < n ; col++){
				std::cout<<solution[row][col]<<" ";
			}
			std::cout<<std::endl;
		}
	}
	ipasir_release(solver);
	return 0;
}
