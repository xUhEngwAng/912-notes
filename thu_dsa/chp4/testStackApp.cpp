#include "../chp2/Fib.h"
#include "stackApp.h"
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

void test_convert();
void test_paren();

int main(){
	cout << "Running tests......" << endl;
	test_convert();
	test_paren();

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
