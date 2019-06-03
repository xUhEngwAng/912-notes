#ifndef GRAPH_H_
#define GRAPH_H_

#include "../chp2/Vector.h"
#include "../chp4/Stack.h"

typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus;
typedef enum { UNDETERMINED, TREE, BACKWARD, FORWARD, CROSS } Etype;

//abstract class Graph
template <typename Tv> template <typename Te>
class Graph{
private:
	void reset();
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
	virtual void insertEdge(Te const&, int, int, int) = 0;
	virtual Te   removeEdge(int, int) = 0;
	virtual Etype& type(int, int) = 0;
	virtual Te& edge(int, int) = 0;
	virtual int& weight(int, int) = 0;

	//Graph related algorithms
	void bfs(int);
	void dfs(int);
	void bcc(int);
	void prim(int);
	void dijkstra(int);
	Stack<Tv>* tSort(int);
	template <typename PU> void pfd(int, PU);
};

#endif