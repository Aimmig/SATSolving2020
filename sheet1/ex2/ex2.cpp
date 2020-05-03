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
	return (1+n*(row-1)+(col-1))+(val-1)*n*n;
}

/*
 *
 */
void addRules(void* solver, int k){
	int n=k*k;
	//TO-DO anpassen für jede Zahl
	//for (int i=1; i<=n;i++){
		int i=1;
		for (int r=1; r<=n; r++){
			for (int c=1; c<=n; c++){
				std::cout<<encode(n,r,c,i)<<" ";
			}
			std::cout<<std::endl;
			for (int c=1; c<=n; c++){
				std::cout<<encode(n,c,r,i)<<" ";
			}
			std::cout<<std::endl;
		}
		//TO-DO: add condition for each block
		for (int b=0;b<k;b++){
			for(int j=1; j<=k;j++){
				for (int u=1; u<=k; u++){
					std::cout<<u+(j-1)*n+b*k<<" ";
				}
			}
			std::cout<<std::endl;
		}
	//}
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
	/*
		while(iss >> part){
			splitted.push_back(part);
		}
		if (splitted[0].compare(std::string("p")) == 0){
			numVertices = std::stoi(splitted[2]);
			//numEdges = std::stoi(splitted[3]);
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
	*outVariables = numVertices;
	*/
	return 0;
}

int main(int argc, char **argv) {
	std::cout << "c Using the incremental SAT solver " << ipasir_signature() << std::endl;

	if (argc != 2) {
		puts("c USAGE: ./example <file.sudo>");
		return 0;
	}

	void *solver = ipasir_init();
	int numVertices = 0;
	//int edgelist = 
	betterFile(solver, argv[1], &numVertices);
	
	/*if (!loaded) {
		std::cout << "c The input formula " << argv[1] << " could not be loaded." << std::endl;
		return 0;
	}
	else {
		std::cout << "c Loaded, solving" << std::endl;
	}*/
	/*
	enforceColor(solver,colors,numVertices);

	int satRes = ipasir_solve(solver);

	
	while (satRes == 20) {
		//std::cout<<"c--------"<<std::endl;
		std::cout<<"c The graph is not colorable with "<<colors<<" colors" << std::endl;
		std::cout<<"c Now trying with "<<colors+1<<" colors"<<std::endl;
		//std::cout<<"c--------"<<std::endl;
		addMoreVertexClauses(solver, colors, numVertices, edgelist);
		colors++;
		addClauses(solver,numVertices,colors);

		for (int col = minColor; col <colors; col++){
			for (int v=0; v<numVertices;v++){
				int activationLit = getActivationLiteral(v, col,numVertices);
				//std::cout<<"Setting Activation literal"<<activationLit<<std::endl;
				ipasir_assume(solver,activationLit);
			}
		}
		enforceColor(solver,colors,numVertices);
		satRes = ipasir_solve(solver);
	}
	
	if (satRes == 10) {
		std::cout << "c The graph is colorable with " <<colors<<" colors"<< std::endl;
		std::cout << "c The output format is: <node> <color>"<< std::endl;
		int vertex = 0;
		int color = 0;
		int variables = getNumVariables(numVertices,colors);
		int value = 0;
		
		//only request values for even variables (odd are activations literals)
		for (int var = 2; var <= variables; var+=2) {
			value = ipasir_val(solver, var);
			if (value > 0){
				vertex = decode_vertex(value,numVertices);
				color = decode_col(value,numVertices);
				std::cout <<"v "<< vertex <<" "<< color<<std::endl;
			}
		}
	}*/
	return 0;
}
