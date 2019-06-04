#include "GraphMatrix.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;

void test_constructor();
void test_vertex_insert();
void test_vertex_delete();
void test_edge_insert();

int GraphMatrix_test_main(){
	cout << "Running tests" << endl;

	test_constructor();
	test_vertex_insert();
	test_edge_insert();
	test_vertex_delete();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_constructor(){
	GraphMatrix<int, int> G;
	assert(G.num_of_edges == 0);
	assert(G.num_of_vertices == 0);
}

void test_vertex_insert(){
	GraphMatrix<char, int> G;
	G.insertVertex('a');
	assert(G.num_of_vertices == 1);
	assert(G.num_of_edges == 0);
	assert(G.exists(0, 0) == false);
	assert(G.vertex(0) == 'a');
	assert(G.inDegree(0) == 0);
	assert(G.outDegree(0) == 0);
	assert(G.priority(0) == 0);
	assert(G.dtime(0) == -1);
	assert(G.ftime(0) == -1);
	assert(G.parent(0) == -1);

	G.insertVertex('b');
	assert(G.num_of_vertices == 2);
	assert(G.num_of_edges == 0);
	assert(G.exists(0, 1) == false);
	assert(G.exists(1, 1) == false);
	assert(G.vertex(1) == 'b');
}

void test_edge_insert(){
	GraphMatrix<char, int> G;
	graph_demo(G);

	assert(G.num_of_edges == 8);
	assert(G.edge(0, 1) == 1);
	assert(G.weight(0, 1) == 1.);
	assert(G.type(0, 1) == UNDETERMINED);
	assert(G.exists(0, 7));
	assert(G.exists(4, 2));
	assert(G.exists(5, 4));
	assert(G.exists(5, 3));
	assert(G.exists(6, 7));
	assert(G.exists(9, 6));
	assert(G.exists(3, 8));

	assert(!G.exists(1, 0));
	assert(!G.exists(7, 0));
	assert(!G.exists(2, 4));
	assert(!G.exists(4, 5));
	assert(!G.exists(3, 5));
	assert(!G.exists(7, 6));
	assert(!G.exists(6, 9));
	assert(!G.exists(8, 3));

	assert(G.inDegree(0) == 0);
	assert(G.inDegree(1) == 1);
	assert(G.inDegree(2) == 1);
	assert(G.inDegree(3) == 1);
	assert(G.inDegree(4) == 1);
	assert(G.inDegree(5) == 0);
	assert(G.inDegree(6) == 1);
	assert(G.inDegree(7) == 2);
	assert(G.inDegree(8) == 1);
	assert(G.inDegree(9) == 0);

	assert(G.outDegree(0) == 2);
	assert(G.outDegree(1) == 0);
	assert(G.outDegree(2) == 0);
	assert(G.outDegree(3) == 1);
	assert(G.outDegree(4) == 1);
	assert(G.outDegree(5) == 2);
	assert(G.outDegree(6) == 1);
	assert(G.outDegree(7) == 0);
	assert(G.outDegree(8) == 0);
	assert(G.outDegree(9) == 1);
}

void test_vertex_delete(){
	GraphMatrix<char, int> G;
	graph_demo(G);

	G.removeVertex(0);
	assert(G.num_of_vertices == 9);
	assert(G.num_of_edges == 6);
	assert(G.inDegree(0) == 0);
	assert(G.inDegree(1) == 1);
	assert(G.inDegree(2) == 1);
	assert(G.inDegree(3) == 1);
	assert(G.inDegree(4) == 0);
	assert(G.inDegree(5) == 1);
	assert(G.inDegree(6) == 1);
	assert(G.inDegree(7) == 1);
	assert(G.inDegree(8) == 0);

	assert(G.outDegree(0) == 0);
	assert(G.outDegree(1) == 0);
	assert(G.outDegree(2) == 1);
	assert(G.outDegree(3) == 1);
	assert(G.outDegree(4) == 2);
	assert(G.outDegree(5) == 1);
	assert(G.outDegree(6) == 0);
	assert(G.outDegree(7) == 0);
	assert(G.outDegree(8) == 1);
	
	G.removeVertex(5);
	assert(G.num_of_vertices == 8);
	assert(G.num_of_edges == 4);
	assert(G.inDegree(0) == 0);
	assert(G.inDegree(1) == 1);
	assert(G.inDegree(2) == 1);
	assert(G.inDegree(3) == 1);
	assert(G.inDegree(4) == 0);
	assert(G.inDegree(5) == 0);
	assert(G.inDegree(6) == 1);
	assert(G.inDegree(7) == 0);

	assert(G.outDegree(0) == 0);
	assert(G.outDegree(1) == 0);
	assert(G.outDegree(2) == 1);
	assert(G.outDegree(3) == 1);
	assert(G.outDegree(4) == 2);
	assert(G.outDegree(5) == 0);
	assert(G.outDegree(6) == 0);
	assert(G.outDegree(7) == 0);
}
