#include <iostream>
#include <cassert>
#include "lcs.h"
#include <time.h>
#define NUMOFLOOP 1
using namespace std;

void test_lcsIt();
void test_lcsRe();
void test_lcslenIt();
void test_lcslenRe();

int main(){
	test_lcslenIt();
	test_lcslenRe();
	test_lcsRe();
	test_lcsIt();
	system("pause");
}

void test_lcsRe(){
	clock_t begin, end;

	begin = clock();
	//vector<string> lcs = lcsRe(string("educational"), string("advantage"), 11, 9);
	vector<string> lcs = lcsRe(string("immaculate"), string("computer"), 10, 8);
	end = clock();
	assert(!lcs.empty());
	assert(lcs[0].length() == 4);
	for (string entry : lcs)
		cout << entry << " ";
	cout << "\nRecursive lcs test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}

void test_lcsIt() {
	clock_t begin, end;

	begin = clock();
	//string lcs = lcsIt(string("educational"), string("advantage"), 11, 9);
	string lcs = lcsIt(string("immaculate"), string("computer"), 10, 8);
	end = clock();
	assert(!lcs.empty());
	assert(lcs.length() == 4);
	cout << lcs << endl;
	cout << "Recursive lcs test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}


void test_lcslenIt(){
	clock_t begin, end;
	int len;

	begin = clock();
	for(int ix = 0; ix != NUMOFLOOP; ++ix)
		len = lcslenIt(string("educational"), string("advantage"), 11, 9);
	end = clock();
	assert(len == 4);
	assert(lcslenIt(string("didactical"), string("advantage"), 10, 9) == 4);
	cout << "Iterative lcs test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}

void test_lcslenRe(){
	clock_t begin, end;
	int len;

	begin = clock();
	for(int ix = 0; ix != NUMOFLOOP; ++ix)
		len = lcslenRe(string("educational"), string("advantage"), 11, 9);
	end = clock();
	assert(len == 4);
	assert(lcslenRe(string("didactical"), string("advantage"), 10, 9) == 4);
	cout << "Recursive lcs test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}
