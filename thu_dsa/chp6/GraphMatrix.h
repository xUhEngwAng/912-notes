#ifndef GRAPHMATRIX_H_
#define GRAPHMATRIX_H_

#include "Graph.h"
#include "Edge.h"
#include "Vertex.h"
#include "../chp2/Vector.h"

template <typename Tv, typename Te> 
class GraphMatrix: public Graph<Tv, Te>{
private:
	Vector<Vertex<Tv>> V;
	Vector<Vector<Edge<Te>*>> E;

public:
	//constructor
	GraphMatrix();

	//deconstructor
	~GraphMatrix();

	//implementations of abstract Graph methods

	//vertex methods
	int insertVertex(Tv const& data);			//return the id number of the new vertex
	Tv  removeVertex(int vertexId);
	Tv& vertex(int vertexId) { return V[vertexId].data; }
	int inDegree(int vertexId) { return V[vertexId].inDegree; }
	int outDegree(int vertexId) { return V[vertexId].outDegree; }
	int firstNeighbor(int vertexId);
	int nextNeighbor(int vertexId, int curr);
	int& dtime(int vertexId) { return V[vertexId].dTime; }
	int& ftime(int vertexId) { return V[vertexId].fTime; }
	int& parent(int vertexId) { return V[vertexId].parent; }
	int& priority(int vertexId) { return V[vertexId].priority; }
	VStatus& status(int vertexId) { return V[vertexId].status; }

	//directed edge methods
	bool exists(int src, int tail) { return E[src][tail] != nullptr; }
	void insertEdge(Te const &data, int src, int tail) { insertEdge(data, 1, src, tail); }
	void insertEdge(Te const &data, double weight, int src, int tail);
	Te   removeEdge(int src, int tail);
	Etype&  type(int src, int tail) { return E[src][tail]->type; }
	Te&     edge(int src, int tail) { return E[src][tail]->data; }
	double& weight(int src, int tail) { return E[src][tail]->weight; }
};

//application interfaces
void graph_demo(GraphMatrix<char, int> &G);

/*---------------Implementations---------------*/

//constructor
template <typename Tv, typename Te>
GraphMatrix<Tv, Te>::GraphMatrix(){
	num_of_vertices = 0;
	num_of_edges    = 0;
}

//deconstructor
template <typename Tv, typename Te>
GraphMatrix<Tv, Te>::~GraphMatrix(){
	for (int ix = 0; ix != num_of_vertices; ++ix)
		for (int jx = 0; jx != num_of_vertices; ++jx)
			if (exists(ix, jx)) delete E[ix][jx];
}

//implementations of abstract Graph methods

//vertex methods
template <typename Tv, typename Te>
int GraphMatrix<Tv, Te>::insertVertex(Tv const & data){
	V.push_back(Vertex<Tv>(data));
	for (int ix = 0; ix != num_of_vertices; ++ix) E[ix].push_back(nullptr);
	E.push_back(Vector<Edge<Te>*>(++num_of_vertices, nullptr));
	return num_of_vertices - 1;
}

template <typename Tv, typename Te>
Tv GraphMatrix<Tv, Te>::removeVertex(int vertexId){
	for (int ix = 0; ix != num_of_vertices; ++ix)
		if (exists(vertexId, ix)) removeEdge(vertexId, ix);

	for (int ix = 0; ix != num_of_vertices; ++ix) {
		if(exists(ix, vertexId))  removeEdge(ix, vertexId);
		E[ix].pop(vertexId);
	}
	E.pop(vertexId);
	num_of_vertices--;
	return V.pop(vertexId).data;
}

template <typename Tv, typename Te>
int GraphMatrix<Tv, Te>::firstNeighbor(int vertexId) {
	return nextNeighbor(vertexId, num_of_vertices);
}

template <typename Tv, typename Te>
int GraphMatrix<Tv, Te>::nextNeighbor(int vertexId, int curr) {
	while(0 <= --curr){
		if (exists(vertexId, curr)) break;
	}
	return curr;
}

//edge methods
template <typename Tv, typename Te>
void GraphMatrix<Tv, Te>::insertEdge(Te const &data, double weight, int src, int tail){
	if (src < 0 || src >= num_of_vertices || tail < 0 || tail >= num_of_vertices) return;
	if (exists(src, tail)) return;

	E[src][tail] = new Edge<Te>(data, weight);

	++num_of_edges;
	V[tail].inDegree++;
	V[src].outDegree++;
}

template <typename Tv, typename Te>
Te GraphMatrix<Tv, Te>::removeEdge(int src, int tail){
	Te edgeBak = edge(src, tail);
	delete E[src][tail];
	E[src][tail] = nullptr;

	--num_of_edges;
	V[tail].inDegree--;
	V[src].outDegree--;
	return edgeBak;
}

//application interfaces
void graph_demo(GraphMatrix<char, int> &G) {
	G = GraphMatrix<char, int>();
	char *alphabet = "abcdefghij";
	for (int ix = 0; ix != 10; ++ix)
		G.insertVertex(alphabet[ix]);
	G.insertEdge(1, 0, 1);
	G.insertEdge(1, 0, 7);
	G.insertEdge(1, 4, 2);
	G.insertEdge(1, 5, 4);
	G.insertEdge(1, 5, 3);
	G.insertEdge(1, 6, 7);
	G.insertEdge(1, 9, 6);
	G.insertEdge(1, 3, 8);
}

#endif
