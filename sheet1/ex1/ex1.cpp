extern "C" {
    #include "ipasir.h"
}

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class GraphColorer{
public:
  GraphColorer(void* solver, const char* filename, int numColors);
  int solve();
  std::vector<int> getSolution();
  int numVertices;
  std::vector<std::pair<int,int>> edgelist;
//private:
  void* solver;
  int getActivationLiteral(int vertex, int color, int numVertices);
  int getNumVariables(int numVertices, int numColors);
  int encode_pos(int color, int vertex, int numVertices);
  int encode_neg(int color, int vertex, int numVertices);
  void enforceColor(void* solver, int color, int numVertices);
  int decode_vertex(int var, int numVertices);
  int decode_col(int var, int numVertices);
  void addClauses(void* solver, int numVertices, int numColors);
  void addMoreVertexClauses(void* solver, int color, int numVertices, std::vector<std::pair<int,int>> edgeList);
  void addConflictingClauses(void* solver, int numColors, int numVertices, std::vector<std::pair<int,int>> edgelist);
  //std::vector<std::pair<int,int>> betterFile(void* solver, const char* filename, int numColors, int* outVariables);

};

int GraphColorer::solve(){
	return ipasir_solve(solver);
}

/*
 * Encode an artifical activation literal
 */
int GraphColorer::getActivationLiteral(int vertex, int color, int numVertices){

	return 2*(vertex)+1+2*numVertices*color;
}

/*
 * Maximum number of (acutal) variables
 */
int GraphColorer::getNumVariables(int numVertices, int numColors){
	return 2*numVertices*numColors;
}

/*
 * returns a variable coresponding to a color and vertex
 */
int GraphColorer::encode_pos(int color, int vertex, int numVertices){
	return 2*(1+vertex+color*numVertices);
}

int GraphColorer::encode_neg(int color, int vertex, int numVertices){
	return -encode_pos(color,vertex,numVertices)+2;
}

/*
 * Enforces the number of colors by setting all activation literals acordingly
 */
void GraphColorer::enforceColor(void* solver, int color, int numVertices){
	int activationLit = 0;
	for (int v=0; v<numVertices;v++){
		activationLit = getActivationLiteral(v, color,numVertices);
		//std::cout<<"Setting Activation literal -"<<activationLit<<std::endl;	
		ipasir_assume(solver,-activationLit);
	}
}

/*
 * Decodes a variable into the vertex number
 */
int GraphColorer::decode_vertex(int var, int numVertices){
	return 1+((var/2)-1)%numVertices;
}

/*
 * Decodes a variable into the corresponding color
 */
int GraphColorer::decode_col(int var, int numVertices){
	return 1+((var/2)-decode_vertex(var,numVertices))/numVertices;
}

/*
 * Add clauses such that each vertex gets a color:
 * u_1,u_2,...,u_c,u_a for each vertex u
 * where u_a is an additional activation literal
 */
void GraphColorer::addClauses(void* solver, int numVertices, int numColors){
	int var = 0;
	int activationLit = 0;
	for (int v=0; v<numVertices;v++){
		//std::cout<<"c ";
		for (int col=0; col<numColors; col++){
			var = encode_pos(col,v,numVertices);
			ipasir_add(solver,var);
			//std::cout<< var << " ";
		}
		activationLit = getActivationLiteral(v,numColors,numVertices);
		ipasir_add(solver,activationLit);
		//std::cout<<activationLit;
		//std::cout << std::endl;
		ipasir_add(solver,0);
		
	}
	//std::cout<<"c--------"<<std::endl;
}

/*
 * Add Clause such for e=(u,v) u & v have different colors:
 * -u_c,-v_c
 */
void GraphColorer::addMoreVertexClauses(void* solver, int color, int numVertices, std::vector<std::pair<int,int>> edgeList){
	for (auto e : edgeList){
		int var1 = encode_neg(color,e.first,numVertices);
		ipasir_add(solver,var1);
		//std::cout<<"c "<<var1<< " ";
		int var2 = encode_neg(color,e.second,numVertices);
		ipasir_add(solver,var2);
		//std::cout<<var2<< " ";
		ipasir_add(solver,0);
		//std::cout<<std::endl;
	}
}

/*
 * Add all the conflicting clauses for all edges
 */
void GraphColorer::addConflictingClauses(void* solver, int numColors, int numVertices, std::vector<std::pair<int,int>> edgelist){
	for (int k=0; k<numColors; k++){
		addMoreVertexClauses(solver,k,numVertices,edgelist);
	}
	//std::cout<<"c------"<<std::endl;
}

/*
 * Read the graph from file & encode the coloring problem
 * returns vector with all edges
 */
GraphColorer::GraphColorer(void* solver, const char* filename, int numColors){
	std::ifstream infile(filename);
	std::string line;
	this->solver = solver;
	while(std::getline(infile,line)){
		std::vector<std::string> splitted;
		std::istringstream iss(line);
		std::string part;
		while(iss >> part){
			splitted.push_back(part);
		}
		if (splitted[0].compare(std::string("p")) == 0){
			this->numVertices = std::stoi(splitted[2]);
			//numEdges = std::stoi(splitted[3]);
		}
		if (splitted[0].compare(std::string("e")) == 0){
			std::pair<int,int> edge;
			int v1 = std::stoi(splitted[1]);
			int v2 = std::stoi(splitted[2]);
			edge.first = v1;
			edge.second = v2;
			edgelist.push_back(edge);
		}
		splitted.clear();
	}
	addConflictingClauses(solver, numColors, numVertices, edgelist);
	addClauses(solver,numVertices, numColors);
	//std::cout<<"c Graph has " << numVertices<<" vertices"<<std::endl;
	//std::cout<<"c Graph has " << numEdges <<" edes"<<std::endl;
	//*outVariables = numVertices;
	//return res;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 3) {
		puts("c USAGE: ./example <dimacs.cnf> <minColorToTry>");
		return 0;
	}

	void *solver = ipasir_init();
	int minColor = atoi(argv[2]);
	int colors = minColor;
	//int numVertices = 0;
	
	GraphColorer* g = new GraphColorer(solver, argv[1], minColor);
	//std::cout<<"c Trying to color graph with "<< colors <<" Colors"<<std::endl;
	//std::vector<std::pair<int,int>> edgelist = betterFile(solver, argv[1], colors, &numVertices);
	
	g->enforceColor(solver,colors,g->numVertices);
	
	int satRes = g->solve();
	
	std::cout<<satRes<<" ";
	while (satRes == 20) {
		//std::cout<<"c--------"<<std::endl;
		std::cout<<"c The graph is not colorable with "<<colors<<" colors" << std::endl;
		std::cout<<"c Now trying with "<<colors+1<<" colors"<<std::endl;
		//std::cout<<"c--------"<<std::endl;
		g->addMoreVertexClauses(solver, colors, g->numVertices, g->edgelist);
		colors++;
		g->addClauses(solver,g->numVertices,colors);

		for (int col = minColor; col <colors; col++){
			for (int v=0; v<g->numVertices;v++){
				int activationLit = g->getActivationLiteral(v, col, g->numVertices);
				//std::cout<<"Setting Activation literal"<<activationLit<<std::endl;
				ipasir_assume(solver,activationLit);
			}
		}
		g->enforceColor(solver,colors,g->numVertices);
		satRes = ipasir_solve(solver);
	}
	
	if (satRes == 10) {
		std::cout << "c The graph is colorable with " <<colors<<" colors"<< std::endl;
		std::cout << "c The output format is: <node> <color>"<< std::endl;
		int vertex = 0;
		int color = 0;
		int variables = g->getNumVariables(g->numVertices,colors);
		int value = 0;
		
		//only request values for even variables (odd are activations literals)
		for (int var = 2; var <= variables; var+=2) {
			value = ipasir_val(solver, var);
			if (value > 0){
				vertex = g->decode_vertex(value,g->numVertices);
				color = g->decode_col(value,g->numVertices);
				std::cout <<"v "<< vertex <<" "<< color<<std::endl;
			}
		}
	}
	return 0;
}
