#include "skiplist.h"
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

void basic_test();
void insert_delete_test();

int main(){
	cout << "begin tests ..." << endl;

	basic_test();
	insert_delete_test();
	
	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void basic_test(){
	SkipList<int, int> slist;
	assert(slist.size() == 0);
	assert(slist.level() == 1);
	slist.print();

	assert(slist.remove(0) == false);

	cout << "basic_test passed." << endl;
}

void insert_delete_test(){
	SkipList<int, int> slist;
	//int src[15] = { 5, 9, 0, 2, 3, 6, 1, 7, 4, 8, 3, 8, 4, 7, 7 };
	int src[15] = { 7 };
	srand((unsigned int)time(0));
	for (int ix = 0; ix != 15; ++ix) {
		slist.put(src[ix], src[ix]);
		cout << "This is the " << ix + 1 << "th insert, the level is " << slist.level() << endl;
		slist.print();
	}

	//search test
	for (int ix = 0; ix != 15; ++ix) {
		int v = slist.get(src[ix]);
		assert(v == src[ix]);
	}

	cout << endl;

	for(int ix = 0; ix != 15; ++ix){
		slist.remove(src[ix]);
		cout << "This is the " << ix + 1 << "th delete, the level is " << slist.level() << endl;
		slist.print();
	}

	cout << "insert_delete test passed." << endl;
}
