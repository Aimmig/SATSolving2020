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
  SudokuSolver(std::ifstream& infile);
  int solve();
  void printSolution();
  void getSolution();
  int n,k,maxVar;
  std::vector<int> solution;
private:
  void* solver;
  int encode(int row, int col, int val);
  int decode_col(int val);
  int decode_row(int val);
  int decode_val(int val);
  int getAdditionalVar(int i, int j);
  int encode_block(int inner_row, int inner_col, int outer_row, int outer_col, int num);
  void addBlockClauses(int num);
  void addConflictingClauses();
  void addRules();
  void sequentialAMO(std::vector<int> vars, int offset);
  void atMostOneEncoding(std::vector<int> vars);
  void assumeCell(int row, int col, int val);
};

int SudokuSolver::solve(){
	return ipasir_solve(solver);
}

void SudokuSolver::getSolution(){
	std::vector<int> solution(n*n,-1); 

	int value,col,row,res = 0;
	
	for (int var = 1; var <= maxVar; var+=1) {
		value = ipasir_val(solver, var);
		if (value > 0){
			//std::cout<<value<<std::endl;
			row = decode_row(value)-1;
			col = decode_col(value)-1;
			res = decode_val(value);
			//std::cout<<"["<<row<<","<<col<<"]="<<value<<std::endl;
			solution[row+n*col] = res;
			//std::cout<<solution[row+n*col]<<" ";
		}
	}
	this->solution = solution;
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
int SudokuSolver::encode_block(int inner_row, int inner_col, int outer_row, int outer_col, int num){
	return 1 + inner_row + inner_col*n + outer_row*k+n*n*(num) + outer_col*n*k;
}

/*
 * CARDINALITY CONSTRAINT
 * *Add conflicting clauses to sovler
 *e.g. on each field there can only be one number
 */
void SudokuSolver::addConflictingClauses(){
	for (int row=0; row<n; row++){
		for (int col=0; col<n; col++){
			std::vector<int> conflicting;
			int var;
			for (int offset=0; offset<n; offset++){
				var = encode(row,col,offset);
				//std::cout<<var<<" ";
				conflicting.push_back(var);
			}
			//std::cout<<std::endl;
			sequentialAMO(conflicting,row+n*col);
		}
	}

}

int SudokuSolver::getAdditionalVar(int i,int j){
	return maxVar+i+(n-1)*j;
}

void SudokuSolver::sequentialAMO(std::vector<int> set, int offset){

	//std::cout<<-set[0]<<" "<<getAdditionalVar(1,offset)<<std::endl;
	ipasir_add(solver,-set[0]);
	ipasir_add(solver, getAdditionalVar(1,offset));
	ipasir_add(solver,0);
	
	for (int i=1; i< n-1;i++){
		ipasir_add(solver, -set[i]);
		ipasir_add(solver, getAdditionalVar(i+1,offset));
		ipasir_add(solver,0);
		
		ipasir_add(solver,-getAdditionalVar(i,offset));	
		ipasir_add(solver, getAdditionalVar(i+1,offset));
		ipasir_add(solver,0);
		
		ipasir_add(solver, -set[i]);
		ipasir_add(solver, -getAdditionalVar(i,offset));
		ipasir_add(solver,0);
	}
	//std::cout<<std::endl;
	//std::cout<<-set[n-1]<<" "<<getAdditionalVar(n-1,offset)<<std::endl;
	ipasir_add(solver,-set[n-1]);
	ipasir_add(solver, -getAdditionalVar(n-1,offset));
	ipasir_add(solver,0);

}

/*
 *Add all the sudoku rules
 */
void SudokuSolver::addRules(){
	for (int i=0; i<n;i++){
		for (int r=0; r<n; r++){
			for (int c=0; c<n; c++){
				int var = encode(r,c,i);
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
			//std::cout<<std::endl;
			///std::cout<<"----"<<std::endl;
			//clauses for colums
			for (int c=0; c<n; c++){
				int var = encode(c,r,i);
				ipasir_add(solver,var);
			}
			ipasir_add(solver,0);
		}
	       	for (int outer_col=0;outer_col<k;outer_col++){
        	       	for (int outer_row=0;outer_row<k;outer_row++){
				int var = 0;
				for(int inner_col=0; inner_col<k;inner_col++){
                        	       	for (int inner_row=0; inner_row<k; inner_row++){
                                	       	var = encode_block(inner_row,inner_col,outer_row,outer_col,i);
                                       		//std::cout<<var<<" ";
                                       		ipasir_add(solver,var);
                               		}
                       		}
                       		ipasir_add(solver,0);
                       		//std::cout<<std::endl;
				//std::cout<<"----"<<std::endl;
               		}
       		}

		
	}

	//TO-DO insert cardinality constraint optimization here
	//no 2 numbers on same field
	addConflictingClauses();
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
SudokuSolver::SudokuSolver(std::ifstream& infile){
	std::string line;
	std::getline(infile,line);
	int k = std::stoi(line);
	int n = k*k;
	int val = 0;
	this->n = n;
	this->k = k;
	this->maxVar = (int)pow(n,3);
	this->solver = ipasir_init();
	
	addRules();
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


void SudokuSolver::printSolution(){
	for (int row = 0; row < n ; row++){
		//std::cout<<"c ";
		for (int col = 0; col < n ; col++){
			std::cout<<solution[row+n*col]<<" ";
		}
		std::cout<<std::endl;
	}

}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 2) {
		std::cout<<"c USAGE: ./ex2 <Sudoku-file>"<<std::endl;
		return 0;
	}
	std::ifstream infile(argv[1]);
	if (!infile.is_open()){
		std::cout<<"Error while opening file "<<argv[1]<<std::endl;
		return -1;
	}
	SudokuSolver* s = new SudokuSolver(infile);
	std::cout<<"c Sudoku loaded from file "<<argv[1]<<std::endl;
	int satRes = s->solve();
	if (satRes == 20){
		std::cout<<"c Invalid Sudoku, not solvable"<<std::endl;
	}
	if (satRes == 10){
		std::cout<<"c Solution for Sudoku found"<<std::endl;
		s->getSolution();
	 	s->printSolution();	
	}
	return 0;

}
