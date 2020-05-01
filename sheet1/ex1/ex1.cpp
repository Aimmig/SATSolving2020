extern "C" {
    #include "ipasir.h"
}

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <vector>

/*
 * returns a variable coresponding to a color and vertex
 */
int encode(int color, int vertex, int numVertices){
	return 1+vertex+color*numVertices;
}

/*
 * Decodes a variable int the vertex number
 */
int decode_vertex(int var, int numVertices){
	int res = abs(var)%numVertices;
	return res > 0 ? res : numVertices;
}

/*
 * Decodes a variable into the corresponding color
 */
int decode_col(int var, int numColors, int numVertices){
	return ((abs(var)-decode_vertex(var, numVertices))%numColors) +1 ; 
}

/*
 * parses jst a single number
 */
int parseNumber(FILE* f, int c){
	int res = c - '0';
	c = fgetc(f);
	while(isdigit(c)){
		res = res*10 + (c-'0');
		c = fgetc(f);
	}
	return res;
}

void addClauses(void* solver, int numVertices, int numColors){
	int var = 0;
	for (int v=0; v<numVertices;v++){
		for (int col=0; col<numColors; col++){
			var = encode(col,v,numVertices);
			ipasir_add(solver,var);
			std::cout << var << " ";
		}
		std::cout << std::endl;
		ipasir_add(solver,0);
	}
	std::cout<<"--------"<<std::endl;
}

void addConflictingClauses(void* solver, int numColors, int numVertices, int v1, int v2){
	std::cout<<v1 <<" "<<v2 <<std::endl;
	for (int k=0; k<numColors; k++){
		int var1 = -encode(k,v1,numVertices)+1;
		ipasir_add(solver,var1);
		std::cout<<var1<< " ";
		int var2 = -encode(k,v2,numVertices)+1;
		ipasir_add(solver,var2);
		std::cout<<var2<< " ";
		ipasir_add(solver,0);
		std::cout<<std::endl;
	}
	std::cout<<"------"<<std::endl;
}

bool betterFile(void* solver, const char* filename, int numColors, int* outVariables){
	std::ifstream infile(filename);
	std::string line;
	int numVertices;
	int numEdges;
  	std::vector<int> edge;	
	while(std::getline(infile,line)){
		std::vector<std::string> splitted;
		std::istringstream iss(line);
		std::string part;
		while(iss >> part){
			splitted.push_back(part);
		}
		if (splitted[0].compare(std::string("p")) == 0){
			numVertices = std::stoi(splitted[2]);
			numEdges = std::stoi(splitted[3]);
		}
		if (splitted[0].compare(std::string("e")) == 0){
			//std::cout<<"--Filling vector---"<<std::endl;	
			int v1 = std::stoi(splitted[1]);
			//std::cout<<v1 << " ";
			int v2 = std::stoi(splitted[2]);
			//std::cout<<v2;
			edge.push_back(v1);
			edge.push_back(v2);
			std::cout<<std::endl;
			/*for (auto const& value : edge){
    				std::cout << value << std::endl;
			}
			std::cout<<"------"<<std::endl;
			*/
		}
		if (!edge.empty()){
			addConflictingClauses(solver, numColors, numVertices, edge[0], edge[1]);
			edge.clear();
		}
		splitted.clear();
	}
	addClauses(solver,numVertices, numColors);
	std::cout<<"c vertices" << numVertices <<std::endl;
	std::cout<<"c edges " << numEdges <<std::endl;
	*outVariables = numVertices*numColors;
	return true;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 2) {
		puts("c USAGE: ./example <dimacs.cnf>");
		return 0;
	}

	void *solver = ipasir_init();
	int variables = 0;
	int colors = 3;
	//bool loaded = loadFormula(solver, argv[1], colors, &variables);
	bool loaded = betterFile(solver, argv[1], colors, &variables);
	if (!loaded) {
		std::cout << "c The input formula " << argv[1] << " could not be loaded." << std::endl;
		return 0;
	}
	else {
		std::cout << "c Loaded, solving" << std::endl;
	}

	// ipasir_assume(solver, 1);

	ipasir_add(solver, 1);
	ipasir_add(solver, 0);

	int satRes;// = ipasir_solve(solver);

	if (satRes == 20) {
		std::cout << "c The input formula is unsatisfiable" << std::endl;
	}
	
	else if (satRes == 10) {
		std::cout << "c The input formula is satisfiable" << std::endl;
		//std::cout << "v ";

		int count = 1;
		int numVertices = variables/colors;
		//while (satRes == 10) {
		//	std::vector<int> clause;
			for (int var = 1; var <= variables; var++) {
				int value = ipasir_val(solver, var);
				//if (value > 0){
					std::cout << value << ": " <<decode_vertex(value,numVertices) << " " << decode_col(value,colors,numVertices)<<std::endl;
				//}
				//clause.push_back(-value);
			}
			std::cout << std::endl;

			/*for (int lit : clause) {
				ipasir_add(solver, lit);
			}
			ipasir_add(solver, 0);
			*/
			// ipasir_assume(solver, 1);

			//satRes = ipasir_solve(solver);
			//clause.clear();
			//count++;
			//std::cout << "Found " << count << " solutions" << std::endl;
		//}
	}
	return 0;
}
