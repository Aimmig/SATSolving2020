extern "C" {
    #include "ipasir.h"
}

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class GraphColorer{
public:
  GraphColorer(const char* filename, int numColors);
  int solve();
  int tryMoreColors();
  void printSolution();
  std::vector<int> getSolution();
  int color;
private:
  int numVertices, minColor;
  std::vector<std::pair<int,int>> edgelist;
  void* solver;
  //int getAdditionalVar(int i,int j);
  //void sequentialAMO(std::vector<int> set, int offset);
  int getActivationLiteral(int vertex, int color);
  int getNumVariables();
  int encode_pos(int color, int vertex);
  int encode_neg(int color, int vertex);
  void enforceColor(int color);
  int decode_vertex(int var);
  int decode_col(int var);
  void addClauses(int numColors);
  void addMoreVertexClauses(int color);
  void addConflictingClauses(int numColors);
};

int GraphColorer::solve(){
	enforceColor(color);
	return ipasir_solve(solver);
}

int GraphColorer::tryMoreColors(){
	addMoreVertexClauses(color);
	color++;
	addClauses(color);

	for (int col = minColor; col <color; col++){
		for (int v=0; v<numVertices;v++){
			int activationLit = getActivationLiteral(v, col);
			//std::cout<<"Setting Activation literal"<<activationLit<<std::endl;
			ipasir_assume(solver,activationLit);
		}
	}
	return this->solve();
}

/*
 * Encode an artifical activation literal
 */
int GraphColorer::getActivationLiteral(int vertex, int color){

	return 2*(vertex)+1+2*numVertices*color;
}

/*
 * Maximum number of (acutal) variables
 */
int GraphColorer::getNumVariables(){
	return 2*numVertices*color;
}

/*
int GraphColorer::getAdditionalVar(int i,int j){
	return getNumVariables()+i+j;
}*/

/*
 * returns a variable coresponding to a color and vertex
 */
int GraphColorer::encode_pos(int color, int vertex){
	return 2*(1+vertex+color*numVertices);
}

int GraphColorer::encode_neg(int color, int vertex){
	return -encode_pos(color,vertex)+2;
}

/*
 * The at most one constraint for the colors per vertex isn't
 * actually needed to solve the problem. Multiple colors per vertex
 * are actually ok, as long as they dont conflict.
 * This allows to decrease the number of clauses
 *
void GraphColorer::sequentialAMO(std::vector<int> set, int activationLit){
	for (uint i=0; i<set.size(); i++){
		for (uint j=0; j<i; j++){
			ipasir_add(solver,-set[i]);
			ipasir_add(solver,-set[j]);
			ipasir_add(solver,activationLit);
			//std::cout<<-set[i]<<" "<<-set[j]<<std::endl;
			ipasir_add(solver,0);
		}
	}

	//std::cout<<-set[0]<<" "<<getAdditionalVar(1,offset)<<std::endl;
        

	int bla = 1000000;
	int offset = activationLit;
	ipasir_add(solver,-set[0]);
	std::cout<<getAdditionalVar(1,bla)<<std::endl;
	ipasir_add(solver, getAdditionalVar(1,bla));
	ipasir_add(solver,offset);	
	ipasir_add(solver,0);
	int n = set.size();
	for (int i=1; i< n-1;i++){
		ipasir_add(solver, -set[i]);
		ipasir_add(solver, getAdditionalVar(i+1,bla));
		ipasir_add(solver,offset);
		ipasir_add(solver,0);

		ipasir_add(solver,-getAdditionalVar(i,bla));
		ipasir_add(solver, getAdditionalVar(i+1,bla));
		ipasir_add(solver,offset);
		ipasir_add(solver,0);

		ipasir_add(solver, -set[i]);
		ipasir_add(solver, -getAdditionalVar(i,bla));
		ipasir_add(solver,offset);
		ipasir_add(solver,0);
	}
	//std::cout<<std::endl;
	//std::cout<<-set[n-1]<<" "<<getAdditionalVar(n-1,offset)<<std::endl;
	ipasir_add(solver,-set[n-1]);
	ipasir_add(solver, -getAdditionalVar(n-1,bla));
	ipasir_add(solver,offset);
	ipasir_add(solver,0);
}*/


/*
 * Enforces the number of colors by setting all activation literals acordingly
 */
void GraphColorer::enforceColor(int color){
	int activationLit = 0;
	for (int v=0; v<numVertices;v++){
		activationLit = getActivationLiteral(v, color);
		//std::cout<<"Setting Activation literal -"<<activationLit<<std::endl;	
		ipasir_assume(solver,-activationLit);
	}
}

/*
 * Decodes a variable into the vertex number
 */
int GraphColorer::decode_vertex(int var){
	return 1+((var/2)-1)%numVertices;
}

/*
 * Decodes a variable into the corresponding color
 */
int GraphColorer::decode_col(int var){
	return 1+((var/2)-decode_vertex(var))/numVertices;
}

/*
 * Add clauses such that each vertex gets a color:
 * u_1,u_2,...,u_c,u_a for each vertex u
 * where u_a is an additional activation literal
 */
void GraphColorer::addClauses(int numColors){
	int var = 0;
	int activationLit = 0;
	for (int v=0; v<numVertices;v++){
		std::vector<int> set;
		//std::cout<<"c ";
		for (int col=0; col<numColors; col++){
			var = encode_pos(col,v);
			ipasir_add(solver,var);
			set.push_back(var);
			//std::cout<< var << " ";
		}
		activationLit = getActivationLiteral(v,numColors);
		ipasir_add(solver,activationLit);
		ipasir_add(solver,0);
		//std::cout << std::endl;
		//sequentialAMO(set,activationLit);
		//std::cout<<activationLit;
		
	}
	//std::cout<<"c--------"<<std::endl;
}

/*
 * Add Clause such for e=(u,v) u & v have different colors:
 * -u_c,-v_c
 */
void GraphColorer::addMoreVertexClauses(int color){
	for (auto e : edgelist){
		int var1 = encode_neg(color,e.first);
		ipasir_add(solver,var1);
		//std::cout<<"c "<<var1<< " ";
		int var2 = encode_neg(color,e.second);
		ipasir_add(solver,var2);
		//std::cout<<var1<<" "<<var2<<std::endl;
		ipasir_add(solver,0);
	}
}

/*
 * Add all the conflicting clauses for all edges
 */
void GraphColorer::addConflictingClauses(int numColors){
	for (int k=0; k<numColors; k++){
		addMoreVertexClauses(k);
	}
	//std::cout<<"c------"<<std::endl;
}

/*
 * Read the graph from file & encode the coloring problem
 * returns vector with all edges
 */
GraphColorer::GraphColorer(const char* filename, int numColors){
	void *solver = ipasir_init();

	std::ifstream infile(filename);
	std::string line;
	this->solver = solver;
	this->color = numColors;
	this->minColor = numColors;
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
	addConflictingClauses(numColors);
	addClauses(numColors);
	//std::cout<<"c Graph has " << numVertices<<" vertices"<<std::endl;
	//std::cout<<"c Graph has " << numEdges <<" edes"<<std::endl;
}

void GraphColorer::printSolution(){
	std::cout << "c The graph is colorable with " <<this->color<<" colors"<< std::endl;
	std::cout << "c The following is a valid coloring, where each line contains the color of the corresponding vertices"<< std::endl;
	std::cout << "c The colors of vertices are printed from 1 up to n"<<std::endl;
	int vertex = 0;
	int color = 0;
	int variables = this->getNumVariables();
	int value = 0;

	std::vector<int> solution(numVertices,0);
	//only request values for even variables (odd are activations literals)
	for (int var = 2; var <= variables; var+=2) {
		value = ipasir_val(this->solver, var);
		if (value >= 0){
			vertex = this->decode_vertex(value);
			color = this->decode_col(value);
			solution[vertex-1] = color;
		}
	}
	for (int i=0; i<numVertices; i++){
		//std::cout<<i+1<<" "<<solution[i]<<std::endl;
		std::cout<<solution[i]<<std::endl;
	}
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 3) {
		puts("c USAGE: ./example <dimacs.cnf> <minColorToTry>");
		return 0;
	}

	int minColor = atoi(argv[2]);
	
	GraphColorer* g = new GraphColorer(argv[1], minColor);
	
	int satRes = g->solve();
	
	while (satRes == 20) {
		//std::cout<<"c--------"<<std::endl;
		std::cout<<"c The graph is not colorable with "<<g->color<<" colors" << std::endl;
		std::cout<<"c Now trying with "<<g->color+1<<" colors"<<std::endl;
		//std::cout<<"c--------"<<std::endl;
		satRes = g->tryMoreColors();
	}	
	if (satRes == 10) {
		g->printSolution();
	}
	return 0;
}
