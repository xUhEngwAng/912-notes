#include "GraphMatrix.h"

//application interfaces
void graph_demo(GraphMatrix<char, int> &G) {
	G = GraphMatrix<char, int>();
	char *alphabet = "abcdefghijklmnopqrstuvwxyz";
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
