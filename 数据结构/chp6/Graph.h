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
	void BFS(int, int&);								//Breadth First Search
	void DFS(int, int&);								//Depth First Search
	void BCC(int, int&, Stack<int>&);					//Biconnected Component
	bool TSort(int, int&, Stack<Tv>*);					//Topological Sort
	template <typename PU> void PFS(int, PU);			//Priority First Search

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
	template <typename PU> void pfs(int, PU);
};

//private methods
template <typename Tv, typename Te>
void Graph<Tv, Te>::reset(){//reset all information of all vertices and edges
	for(int ix = 0; ix != num_of_vertices; ++ix){
		status(ix)   = UNDISCOVERED;
		priority(ix) = INT_MAX;
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

#define hca(x) ftime(x)
template <typename Tv, typename Te>
void Graph<Tv, Te>::BCC(int x, int &clock, Stack<int> &S) {
	status(x) = DISCOVERED;
	S.push(x);
	hca(x) = dtime(x) = ++clock;
	for (int v = firstNeighbor(x); v != -1; v = nextNeighbor(x, v)) {
		switch (status(v)) {
		case UNDISCOVERED:
			type(x, v) = TREE;
			BCC(v, clock, S);
			if (hca(v) < dtime(x)) hca(x) = MIN(hca(v), hca(x));
			else {//a biconnected component found
				while (S.pop() != x);
				S.push(x);
			}
			break;

		case DISCOVERED:
			type(x, v) = BACKWARD;
			if (parent(x) != v) hca(x) = MIN(hca(x), dtime(v));
			break;

		case VISITED:
		default:
			type(x, v) = dtime(x) < dtime(v) ? FORWARD : CROSS;
			break;
		}
	}
}
#undef hca

template <typename Tv, typename Te>
bool Graph<Tv, Te>::TSort(int x, int &clock, Stack<Tv> *S){
	status(x) = DISCOVERED;
	dtime(x)  = ++clock;
	for(int v = firstNeighbor(x); v != -1; v = nextNeighbor(x, v)){
		switch(status(v)){
		case UNDISCOVERED:
			status(v) = DISCOVERED;
			type(x, v) = TREE;
			if (!TSort(v, clock, S)) return false;
			break;

		case DISCOVERED:
			type(x, v) = BACKWARD;
			return false;

		case VISITED:
		default:
			type(x, v) = dtime(x) > dtime(v)? FORWARD: CROSS;
			break;
		}
	}
	S->push(x);
	return true;
}

template <typename Tv, typename Te>
template <typename PU>
void Graph<Tv, Te>::PFS(int x, PU PrioUpdater){
	priority(x) = 0;
	status(x)   = VISITED;
	parent(x)   = -1;
	while(1){
		for (int v = firstNeighbor(x); v != -1; v = nextNeighbor(x, v))
			PrioUpdater(this, x, v);
		//find current highest priority
		for (int shortest = INT_MAX, v = 0; v != num_of_vertices; ++v)
			if (status(v) == UNDISCOVERED && priority(v) < shortest) {
				shortest = priority(v);
				x        = v;
			}
		if (status(x) == VISITED) break;
		status(x) == VISITED;
		type(parent(x), x) = TREE;
	}
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

template <typename Tv, typename Te>
void Graph<Tv, Te>::bcc(int start){
	reset();
	int clock = 0;
	int curr  = start;
	Stack<int> S;
	do{
		if (status(curr) == UNDISCOVERED) {
			BCC(curr, clock, S);
			S.pop();	//remove the last element in the Stack, which is the start point of current connected components
		}
	} while ((curr = ++curr % num_of_vertices) £¡ = start);
}

//Prim Algorithm
template <typename Tv, typename Te>
class PrimPU{
public:
	void operator()(Graph<Tv, Te> *G, int parent, int v){
		if(G->status(v) == UNDISCOVERED){
			if (G->weight(parent, v) < G->priority(v)) {
				G->priority = G->weight(parent, v);
				G->parent(v) = parent;
			}
		}
	}
};

template <typename Tv, typename Te>
void Graph<Tv, Te>::prim(int x){
	pfs(x, PrimPU<Tv, Te>());
}

//Dijkstra Algorithm

template <typename Tv, typename Te>
class DijkstraPU{
public:
	void operator()(Graph<Tv, Te> *G, int parent, int v){
		if(G->status(v) == UNDISCOVERED){
			if(G->priority(parent) + G->weight(parent, v) < G->priority(v)){
				G->priority(v) = G->priority(parent) + G->weight(parent, v);
				G->parent(v) = parent;
			}
		}
	}
};

template <typename Tv, typename Te>
void Graph<Tv, Te>::dijkstra(int x){
	pfs(x, DijkstraPU<Tv, Te>());
}

template <typename Tv, typename Te>
Stack<Tv>* Graph<Tv, Te>::tSort(int start){
	reset();
	Stack<Tv> *S;
	int clock = 0;
	int curr  = start;
	do{
		if(status(curr) == UNDISCOVERED && !TSort(curr, clock, S)){
			while (!S->empty()) S->pop();
			break;
		}
	} while ((curr = ++curr % num_of_vertices) != start);
	return S;
}

template <typename Tv, typename Te>
template <typename PU> 
void Graph<Tv, Te>::pfs(int start, PU PrioUpdater){
	reset();
	int curr = start;
	do{
		if (status(curr) == UNDISCOVERED)
			PFS(curr, PrioUpdater);
	} while ((curr = ++curr % num_of_vertices) != start);
}

#endif
