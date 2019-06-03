#ifndef GRAPHMATRIX_H_
#define GRAPHMATRIX_H_

#include "Graph.h"
#include "Edge.h"
#include "Vertex.h"
#include "../chp2/Vector.h"

template <typename Tv> template <typename Te>
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
	int& dtime(int vertexId) { return V[vertexId].dtime; }
	int& ftime(int vertexId) { return V[vertexId].ftime; }
	int& parent(int vertexId) { return V[vertexId].parent; }
	int& priority(int vertexId) { return V[vertexId].priority; }
	VStatus& status(int vertexId) { return V[vertexId].status; }

	//directed edge methods
	bool exists(int src, int tail) { return E[src][tail] != nullptr; }
	void insertEdge(Te const &data, int weight, int src, int tail);
	Te   removeEdge(int src, int tail);
	Etype& type(int src, int tail) { return E[src][tail]->type; }
	Te&    edge(int src, int tail) { return E[src][tail]->data; }
	int&   weight(int src, int tail) { return E[src][tail]->weight; }
};

/*---------------Implementations---------------*/

//constructor
template <typename Tv> template <typename Te>
GraphMatrix<Tv, Te>::GraphMatrix(){
	num_of_vertices = 0;
	num_of_edges    = 0;

	V = Vector<Vertex<Tv>>();
	E = Vector<Vector<Edge<Te>>>();
}

//implementations of abstract Graph methods

//vertex methods
template <typename Tv> template <typename Te>
int GraphMatrix<Tv, Te>::insertVertex(Tv const & data){
	V.push_back(Vertex<Tv>(data));
	for (int ix = 0; ix != num_of_vertices; ++ix) E[ix].push_back(nullptr);
	E.push_back(Vector<Edge<Te>*>(nullptr, ++num_of_vertices));
	return num_of_vertices - 1;
}

template <typename Tv> template <typename Te>
Te GraphMatrix<Tv, Te>::removeVertex(int vertexId){
	for (int ix = 0; ix != num_of_vertices; ++ix) {
		//remove edges
		if(exists(ix, vertexId) removeEdge(ix, vertexId);
		if(exists(vertexId, ix) removeEdge(vertexId, ix);
		E[ix].pop(vertexId);
	}
	E.pop(vertexId);
	--num_of_vertices;
	return V.pop(vertexId).data;
}

//edge methods
template <typename Tv> template <typename Te>
void GraphMatrix<Tv, Te>::insertEdge(Te const &data, int weight, int src, int tail){
	if (src < 0 || src >= num_of_vertices || tail < 0 || tail >= num_of_vertices) return;
	if (exists(src, tail)) return;

	E[src][tail] = new Edge<Te>(data, weight);

	++num_of_edges;
	V[tail].inDegree++;
	V[src].outDegree--;
}

template <typename Tv> template <typename Te>
Te GraphMatrix<Tv, Te>::removeEdge(int src, int tail){
	Te edgeBak = edge(src, tail);
	delete edge(src, tail);
	edge(src, tail) = nullptr;

	--num_of_edges;
	V[tail].inDegree--;
	V[src].outDegree--;
	return edgeBak;
}

#endif
