#include "max2.h"
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#define LEN 10001
#define NUMOFLOOP 1000	
using namespace std;

void max2ItTest();
void max2It2Test();
void max2ReTest();

int A[LEN];

int main(){
	ifstream in("testData.txt");
	char oneline[256];
	string str;
	int ix = 0;
	while(!in.eof()){
		in.getline(oneline, 100);
		str = string(oneline);
		if(str.size()) A[ix++] = stoi(str);
	}
																 
	max2ItTest();
	max2It2Test();
	max2ReTest();
}

void max2ReTest(){
	int one, two;
	clock_t begin, end;

	begin = clock();
	for(int ix = 0; ix <NUMOFLOOP; ++ix)
		max2Re(A, 0, LEN, one, two);
	end = clock();

	assert(one == 9999);
	assert(two == 9999);
	cout << "max2Re test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}

void max2ItTest(){
	int one, two;
	clock_t begin, end;

	begin = clock();
	for(int ix = 0; ix != NUMOFLOOP; ++ix)
		max2It(A, 0, LEN, one, two);
	end = clock();

	assert(one == 9999);
	assert(two == 9999);
	printf("max2It test passed.\n");
	cout << "Running time: " << end - begin << endl;
}

void max2It2Test(){
	int one, two;
	clock_t begin, end;

	begin = clock();
	for(int ix = 0; ix != NUMOFLOOP; ++ix)
		max2It2(A, 0, LEN, one, two);
	end = clock();

	assert(one == 9999);
	assert(two == 9999);
	cout << "max2It2 test passed." << endl;
	cout << "Running time: " << end - begin << endl;
}

