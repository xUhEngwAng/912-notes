#include <iostream>
#include <cassert>
#include "lcs.h"
#include <time.h>
#define NUMOFLOOP 10
using namespace std;

void test_lcsIt();
void test_lcsRe();

int main(){
	test_lcsIt();
	test_lcsRe();
}

void test_lcsIt(){
	clock_t begin, end;
	int len;

	begin = clock();
	for(int ix = 0; ix != NUMOFLOOP; ++ix)
		len = lcsIt(string("educational"), string("advantage"));
	end = clock();
	assert(len == 4);
	assert(lcsIt(string("didactical"), string("advantage")) == 4);
	cout << "Iterative lcs test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}

void test_lcsRe(){
	clock_t begin, end;
	int len;

	begin = clock();
	for(int ix = 0; ix != NUMOFLOOP; ++ix)
		len = lcsRe(string("educational"), string("advantage"));
	end = clock();
	assert(len == 4);
	assert(lcsRe(string("didactical"), string("advantager")) == 4);
	cout << "Recursive lcs test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}

