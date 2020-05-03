extern "C" {
    #include "ipasir.h"
}

#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class SudokuSolver{

public:
  SudokuSolver(const char* filename);
  int solve();
  std::vector<int> getSolution();
  int n,k;
private:
  void* solver;
  int encode(int row, int col, int val);
  int decode_col(int val);
  int decode_row(int val);
  int decode_val(int val);
  int encode_block(int u, int j, int b, int i, int k,int s);
  void addBlockClauses(int num, int k);
  void addConflictingClauses(int k);
  void addRules(int k);
  void assumeCell(int row, int col, int val);
  int betterFile(const char* filename);
};

int SudokuSolver::solve(){
	return ipasir_solve(solver);
}

std::vector<int> SudokuSolver::getSolution(){
	std::vector<int> solution(n*n,0); 

	//std::vector<int> solution;
	int value,col,row,res = 0;
	int maxVar = (int)pow(n,3);
	
	for (int var = 1; var <= maxVar; var+=1) {
		//solution.push_back(0);
		value = ipasir_val(solver, var);
		//std::cout<<value<<std::endl;
		if (value > 0){
			row = decode_row(value)-1;
			col = decode_col(value)-1;
			res = decode_val(value);
			//solution[row][col] = res;
			solution[row+n*col] = res;
		}
	}
	return solution;
}

/*
 * Encodes that the cell at row,col has given val in sudoku
 */
int SudokuSolver::encode(int row, int col, int val){
	return 1+n*(row)+col+(val)*n*n;
}

/*
 * Get the column of a given literal
 */
int SudokuSolver::decode_col(int val){
	int res =val%n;
	return res > 0 ? res : n;
}

/*
 * Get the row of a given literal
 */
int SudokuSolver::decode_row(int val){
	return ((((val-1)%(n*n))-decode_col(val)+1)/n) + 1;
}

/*
 * Get the sudoku value for a given literal
 */
int SudokuSolver::decode_val(int val){
	return 1+(((val-1)-decode_col(val)+1)/n)/n;
}

/*
 *Encodes variables for a blocEncodes variables for a block
 */
int SudokuSolver::encode_block(int u, int j, int b, int i, int k,int s){
	return 1+u+(j)*n+b*k+n*n*(i)+s*n*k;
}

/*
 *Add all variables that correspond to the sudoku block constraint
 *e.g in each k*k block there has to be each number
 */
void SudokuSolver::addBlockClauses(int num, int k){
	int var = 0;
	for (int s=0;s<k;s++){
		for (int b=0;b<k;b++){
			for(int j=0; j<k;j++){
				for (int u=0; u<k; u++){
					var = encode_block(u,j,b,num,k,s);
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
void SudokuSolver::addConflictingClauses(int k){
	int l1 = 0;
	int l2 = 0;
	for (int row=0; row<n; row++){
		for (int col=0; col<n; col++){
			for (int n1=0; n1<n; n1++){
				for (int n2=0; n2<n1; n2++){
					l1 = -encode(row,col,n1);
					l2 = -encode(row,col,n2);
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
void SudokuSolver::addRules(int k){
	for (int i=0; i<n;i++){
		for (int r=0; r<n; r++){
			//clause for rows
			for (int c=0; c<n; c++){
				int var = encode(r,c,i);
				//std::cout<<var<<" ";
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
			//std::cout<<std::endl;
			//clauses for colums
			for (int c=0; c<n; c++){
				int var = encode(c,r,i);
				//std::cout<<var<<" ";
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
			//std::cout<<std::endl;
		}
		//std::cout<<"----"<<std::endl;
		//clauess for blocks
		addBlockClauses(i,k);
	}
	//no 2 numbers on same field
	addConflictingClauses(k);
}

/*
 * Assume that at row,col the sudoku value is the given val
 */
void SudokuSolver::assumeCell(int row, int col, int val){
	int var = encode(row, col, val);
	ipasir_assume(solver, var);
}

/*
 * Read the graph from file & encode the coloring problem
 * returns vector with all edges
 */
SudokuSolver::SudokuSolver(const char* filename){
	std::ifstream infile(filename);
	std::string line;
	std::getline(infile,line);
	int k = std::stoi(line);
	int n = k*k;
	int val = 0;
	this->n = n;
	this->solver = ipasir_init();
	
	addRules(k);
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
				assumeCell(row, col, val);
			}
		}
		//std::cout<<std::endl;
	}
}


void printSolution(int n, std::vector<int> s){
	for (int row = 0; row < n ; row++){
		//std::cout<<"c ";
		for (int col = 0; col < n ; col++){
			std::cout<<s[row+n*col]<<" ";
		}
		std::cout<<std::endl;
	}

}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 2) {
		std::cout<<"c USAGE: ./example <SSudoku-file>"<<std::endl;
		return 0;
	}
	
	SudokuSolver* s = new SudokuSolver(argv[1]);
	int satRes = s->solve();

	if (satRes == 20){
		std::cout<<"c Invalid Sudoku, not solvable"<<std::endl;
	}
	if (satRes == 10){
		std::cout<<"c Solution for Sudoku found"<<std::endl;
		auto solution = s->getSolution();
	 	printSolution(s->n,solution);	
	}
	return 0;

}
