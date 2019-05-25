#include "stackApp.h"
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

void test_convert();
void test_paren();
void test_evaluate();
void test_toPostfix();

int stackApp_test_main(){
	cout << "Running tests......" << endl;
	test_convert();
	test_paren();
	test_evaluate();
	test_toPostfix();
	
	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_convert(){
	assert(strcmp(convert(1314, 16), "522") == 0);
	assert(strcmp(convert(520, 2), "1000001000") == 0);
	assert(strcmp(convert(520, 16), "208") == 0);
}

void test_paren(){
	assert(paren("(())") == true);
	assert(paren("()()") == true);
	assert(paren("[(])") == false);
	assert(paren("{[()]{[({})]}}") == true);
}

void test_evaluate(){
	assert(evaluate("2*5+(3+4-2*7)/2") == 6.5);
	assert(evaluate("(0!+ 1) * 2 ^ (3!+ 4) - (5!- 67 - (8 + 9))") == 2012);
}

void test_toPostfix(){
	assert(strcmp(toPostfix("(0! + 1) ^ (2 * 3! + 4 - 5)"),"0!1+23!*4+5-^") == 0);
}
