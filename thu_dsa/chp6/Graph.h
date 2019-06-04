#ifndef GRAPH_H_
#define GRAPH_H_

#include "../chp2/Vector.h"
#include "../chp4/Stack.h"
#include "../chp4/Queue.h"

typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus;
typedef enum { UNDETERMINED, TREE, BACKWARD, FORWARD, CROSS } Etype;

//abstract class Graph
template <typename Tv, typename Te>
class Graph{
private:
	void reset();										//reset all information of all vertices and edges
	void BFS(int, int&);
	void DFS(int, int&);
	void BCC(int, int&, Stack<int>&);
	bool TSort(int, int&, Stack<Tv>*);
	template <typename PU> void PFS(int, PU);

public:
	int num_of_vertices;
	int num_of_edges;

	//vertex methods
	virtual int insertVertex(Tv const&) = 0;			//return the id number of the new vertex
	virtual Tv  removeVertex(int) = 0;
	virtual Tv& vertex(int) = 0;
	virtual int inDegree(int) = 0;
	virtual int outDegree(int) = 0;
	virtual int firstNeighbor(int) = 0;
	virtual int nextNeighbor(int, int) = 0;
	virtual int& dtime(int) = 0;
	virtual int& ftime(int) = 0;
	virtual int& parent(int) = 0;
	virtual int& priority(int) = 0;
	virtual VStatus& status(int) = 0;

	//directed egde methods
	virtual bool exists(int, int) = 0;
	virtual void insertEdge(Te const&, double, int, int) = 0;
	virtual Te   removeEdge(int, int) = 0;
	virtual Etype& type(int, int) = 0;
	virtual Te& edge(int, int) = 0;
	virtual double& weight(int, int) = 0;

	//Graph related algorithms
	void bfs(int);
	void dfs(int);
	void bcc(int);
	void prim(int);
	void dijkstra(int);
	Stack<Tv>* tSort(int);
	template <typename PU> void pfd(int, PU);
};

//private methods
template <typename Tv, typename Te>
void Graph<Tv, Te>::reset(){//reset all information of all vertices and edges
	for(int ix = 0; ix != num_of_vertices; ++ix){
		status(ix)   = UNDISCOVERED;
		priority(ix) = 0;
		dtime(ix)    = -1;
		ftime(ix)    = -1;
		parent(ix)   = -1;
		for (int jx = 0; jx != num_of_vertices; ++jx)
			if(exists(ix, jx)) type(ix, jx) = UNDETERMINED;
	}
}

template <typename Tv, typename Te>
void Graph<Tv, Te>::BFS(int x, int &clock){
	Queue<int> Q;
	int neighbor;
	Q.enqueue(x);
	status(x) = DISCOVERED;
	while(!Q.empty()){
		x = Q.dequeue();
		dtime(x) = ++clock;
		for (neighbor = firstNeighbor(x); neighbor != -1; neighbor = nextNeighbor(x, neighbor)){
			if (status(neighbor) == UNDISCOVERED) {
				status(neighbor) = DISCOVERED;
				type(x, neighbor) = TREE;
				parent(neighbor) = x;
				Q.enqueue(neighbor);
			}
			else type(x, neighbor) = CROSS;
		}
		status(x) = VISITED;
	}
}

template <typename Tv, typename Te>
void Graph<Tv, Te>::DFS(int x, int& clock){
	dtime(x) = ++clock;
	status(x) = DISCOVERED;
	for(int neighbor = firstNeighbor(x); neighbor != -1; neighbor = nextNeighbor(x, neighbor)){
		switch(status(neighbor)){
		case UNDISCOVERED:
			type(x, neighbor) = TREE;
			parent(neighbor) = x;
			DFS(neighbor, clock);
			break;

		case DISCOVERED:
			type(x, neighbor) = BACKWARD;
			break;

		case VISITED:
		default:
			if (dtime(x) < dtime(neighbor)) type(x, neighbor) = FORWARD;
			else                            type(x, neighbor) = CROSS;
			break;
		}
	}
	ftime(x) = ++clock;
	status(x) = VISITED;
}

//Graph related algorithms

template <typename Tv, typename Te>
void Graph<Tv, Te>::bfs(int start){
	reset();
	int x = start;
	int clock = 0;
	do {
		if (status(x) == UNDISCOVERED)
			BFS(x, clock);
	} while ((x = ++x % num_of_vertices) != start);
}

template <typename Tv, typename Te>
void Graph<Tv, Te>::dfs(int start){
	reset();
	int clock = 0;
	int x = start;
	do{
		if (status(x) == UNDISCOVERED)
			DFS(x, clock);
	} while ((x = ++x % num_of_vertices) != start);
}

#endif