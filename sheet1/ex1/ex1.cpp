extern "C" {
    #include "ipasir.h"
}

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

/*
 * returns a variable coresponding to a color and vertex
 */
int encode_pos(int color, int vertex, int numVertices){
	return 1+vertex+color*numVertices;
}

int encode_neg(int color, int vertex, int numVertices){
	return -encode_pos(color,vertex,numVertices)+1;
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
	std::cout<<"c "<<var <<" "<<numColors<<" "<<numVertices<<std::endl;
	return 1+(abs(var)-decode_vertex(var, numVertices))/numVertices;
}

void addClauses(void* solver, int numVertices, int numColors){
	int var = 0;
	for (int v=0; v<numVertices;v++){
		std::cout<<"c ";
		for (int col=0; col<numColors; col++){
			var = encode_pos(col,v,numVertices);
			ipasir_add(solver,var);
			std::cout<< var << " ";
		}
		std::cout << std::endl;
		ipasir_add(solver,0);
	}
	std::cout<<"c--------"<<std::endl;
}

void addOneVertexClauses(void* solver, int color, int numVertices, int v1, int v2){
	int var1 = encode_neg(color,v1,numVertices);
	ipasir_add(solver,var1);
	std::cout<<"c "<<var1<< " ";
	int var2 = encode_neg(color,v2,numVertices);
	ipasir_add(solver,var2);
	std::cout<<var2<< " ";
	ipasir_add(solver,0);
	std::cout<<std::endl;
}

int addMoreVertexClauses(void* solver, int color, int numVertices, std::vector<std::pair<int,int>> edgeList){
	for (auto e : edgeList){
		addOneVertexClauses(solver, color, numVertices, e.first, e.second);
	}
	return color+1;
}

void addConflictingClauses(void* solver, int numColors, int numVertices, std::vector<std::pair<int,int>> edgelist){
	for (int k=0; k<numColors; k++){
		addMoreVertexClauses(solver,k,numVertices,edgelist);
	}
	std::cout<<"c------"<<std::endl;
}

std::vector<std::pair<int,int>> betterFile(void* solver, const char* filename, int numColors, int* outVariables){
	std::ifstream infile(filename);
	std::string line;
	int numVertices = 0;
	int numEdges = 0;
	std::vector<std::pair<int,int>> res;
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
			std::pair<int,int> edge;
			int v1 = std::stoi(splitted[1]);
			int v2 = std::stoi(splitted[2]);
			edge.first = v1;
			edge.second = v2;
			res.push_back(edge);
		}
		splitted.clear();
	}
	addConflictingClauses(solver, numColors, numVertices, res);
	addClauses(solver,numVertices, numColors);
	std::cout<<"c Graph has " << numVertices<<" vertices"<<std::endl;
	std::cout<<"c Graph has " << numEdges <<" edes"<<std::endl;
	*outVariables = numVertices*numColors;
	return res;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 3) {
		puts("c USAGE: ./example <dimacs.cnf> <numColors>");
		return 0;
	}

	void *solver = ipasir_init();
	int variables = 0;
	int colors = atoi(argv[2]);
	std::cout<<"c Trying to color graph with "<< colors <<" Colors"<<std::endl;
	std::vector<std::pair<int,int>> edgelist = betterFile(solver, argv[1], colors, &variables);
	/*if (!loaded) {
		std::cout << "c The input formula " << argv[1] << " could not be loaded." << std::endl;
		return 0;
	}
	else {
		std::cout << "c Loaded, solving" << std::endl;
	}*/
	int numVertices = variables/colors;
	//colors = addMoreVertexClauses(solver,colors,numVertices,1,2);
	//colors = addMoreVertexClauses(solver,colors,numVertices,1,2);


	//ipasir_assume(solver, 1);
	//ipasir_add(solver, 1);
	//ipasir_add(solver, 0);

	int satRes = ipasir_solve(solver);

	if (satRes == 20) {
		std::cout<<"c--------"<<std::endl;
		std::cout << "c The input formula is unsatisfiable for "<<colors<<" colors" << std::endl;
		//colors++;
		std::cout << "c Now trying with "<<colors+1<<" colors"<<std::endl;
		std::cout<<"c--------"<<std::endl;
		addMoreVertexClauses(solver, colors, numVertices, edgelist);
	}
	
	if (satRes == 10) {
		std::cout << "c The input formula is satisfiable" << std::endl;
		//std::cout << "v ";

		//int count = 1;
		//while (satRes == 10) {
		//	std::vector<int> clause;
			int vertex = 0;
			int color = 0;
			for (int var = 1; var <= variables; var++) {
				int value = ipasir_val(solver, var);
				if (value > 0){
					vertex = decode_vertex(value,numVertices);
					color = decode_col(value,colors,numVertices);
					std::cout <<"c Variable "<<value<<" encodes:"<<std::endl;
					std::cout <<"v Node " << vertex <<" gets color "<< color<<std::endl;
				}
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
