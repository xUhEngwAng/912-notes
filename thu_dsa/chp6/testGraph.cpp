#include "GraphMatrix.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;

void test_bfs();
void test_dfs();

int main(){
	cout << "Running tests." << endl;

	test_bfs();
	test_dfs();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_bfs(){
	GraphMatrix<char, int> G;
	graph_demo(G);
	G.bfs(0);
	assert(G.dtime(0) == 1);
	assert(G.dtime(1) == 3);
	assert(G.dtime(2) == 4);
	assert(G.dtime(3) == 5);
	assert(G.dtime(4) == 7);
	assert(G.dtime(5) == 8);
	assert(G.dtime(6) == 9);
	assert(G.dtime(7) == 2);
	assert(G.dtime(8) == 6);
	assert(G.dtime(9) == 10);
}

void test_dfs(){
	GraphMatrix<char, int> G;
	graph_demo(G);
	G.dfs(0);
	assert(G.dtime(0) == 1);
	assert(G.dtime(1) == 4);
	assert(G.dtime(2) == 7);
	assert(G.dtime(3) == 9);
	assert(G.dtime(4) == 13);
	assert(G.dtime(5) == 15);
	assert(G.dtime(6) == 17);
	assert(G.dtime(7) == 2);
	assert(G.dtime(8) == 10);
	assert(G.dtime(9) == 19);

	assert(G.ftime(0) = 6);
	assert(G.ftime(1) = 5);
	assert(G.ftime(2) = 8);
	assert(G.ftime(3) = 12);
	assert(G.ftime(4) = 14);
	assert(G.ftime(5) = 16);
	assert(G.ftime(6) = 18);
	assert(G.ftime(7) = 3);
	assert(G.ftime(8) = 11);
	assert(G.ftime(9) = 20);
}
