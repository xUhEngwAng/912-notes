#include "kmp.h"
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

void test_make_next(){
	char* str = "chine chinchilla";
	int exp[] = { -1, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 0, 0 };
	int* result = makeNext(str);
	for (int ix = 0; ix != strlen(str); ++ix)
		assert(exp[ix] == result[ix]);
}

void test_match(){
	char* text = "chine chinchilla";
	char* p1   = "chine";
	char* p2   = "chinc";
	char* p3   = "chinec";
	assert(match(text, p1) == 0);
	assert(match(text, p2) == 6);
	assert(match(text, p3) == 16);
}

int main(){
	cout << "begin tests..." << endl;

	test_make_next();
	test_match();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}
