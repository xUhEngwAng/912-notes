#include "List.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;

void test_constructor1();
void test_constructor2();
void test_push();
void test_pop();
void test_find();
void test_deduplicate();
void test_uniquify();

int main(){
	cout << "Running tests......" << endl;
	test_constructor1();
	test_constructor2();
	test_push();
	test_pop();
	test_find();
	test_deduplicate();
	test_uniquify();

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}

void test_constructor1(){
	List<double> l;
	assert(l.getSize() == 0);
	assert(l.empty());
}

void test_constructor2(){
	List<int> l1;
	int i[] = { 5,6,8,9,7,3,4,1,2,0 };
	for (int ix = 0; ix != 10; ix++)
		l1.push_back(i[ix]);

	List<int> l2(l1);
	for (int ix = 0; ix != 10; ix++)
		assert(l2[ix] == i[ix]);

	List<int> l4(l1.first(), 5);
	for (int ix = 0; ix != 5; ix++)
		assert(l4[ix] == i[ix]);
	assert(l4.getSize() == 5);

	List<int> l5(l1.last(), 1);
	assert(l5.getSize() == 1);
	assert(l5.first()->val == 0);

	List<int> l3(l1, 3, 6);
	assert(l3.getSize() == 6);
	for (int ix = 0; ix != 6; ix++)
		assert(l3[ix] == i[3+ix]);
}

void test_push(){
	List<double> l;
	double d[] = { 1.2, 3.4, 5.6, 7.8, 9.0 };
	for (int ix = 0; ix != 5; ix++)
		l.push_back(d[ix]);
	for (int ix = 0; ix != 5; ix++)
		assert(l[ix] == d[ix]);

	List<int> l1;
	int i[] = { 5,6,8,9,7,3,4,1,2,0 };
	for (int ix = 0; ix != 10; ix++)
		l1.push_front(i[ix]);
	for (int ix = 0; ix != 10; ix++)
		assert(l1[ix] == i[9-ix]);

	//test insert_before
	assert(l1.insert_before(0, 12)->val == 12);
	assert(l1.getSize() == 11);
	assert(l1.insert_before(11, 14)->val == 14);
	assert(l1.getSize() == 12);
}

void test_pop(){
	List<int> l1;
	int i[] = { 5,6,8,9,7,3,4,1,2,0 };
	for (int ix = 0; ix != 10; ix++)
		l1.push_back(i[ix]);

	assert(l1.pop_back()->val == 0);
	assert(l1.pop_back()->val == 2);
	assert(l1.pop_front()->val == 5);
	assert(l1.pop_front()->val == 6);
	assert(l1.getSize() == 6);

	assert(l1.pop(3)->val == 3);
	assert(l1.pop(5) == nullptr);
	ListNodePosi(int) p = l1.first();
	assert(l1.pop(p)->val == 8);
}

void test_find(){
	List<int> l1;
	int i[] = { 5,6,8,9,7,3,4,1,2,0 };
	for (int ix = 0; ix != 10; ix++)
		l1.push_back(i[ix]);

	assert(l1.find(8)->val == 8);
	assert(l1.find(7)->val == 7);
	assert(l1.find(12) == nullptr);

	assert(l1.find(7, 5, l1.last())->val == 7);
	assert(l1.find(7, 4, l1.last()) == nullptr);
}

void test_deduplicate(){
	List<int> l1;
	int i[] = { 5,6,8,9,7,3,4,1,2,0};
	for (int ix = 0; ix != 10; ix++) {
		l1.push_front(i[ix]);
		l1.push_back(i[ix]);
	}

	assert(l1.getSize() == 20);
	assert(l1.deduplicate() == 10);
	assert(l1.getSize() == 10);
	for (int ix = 0; ix != 10; ix++)
		assert(l1[ix] == i[ix]);
}

void test_uniquify(){
	List<int> l1;
	int i[] = {5,8,12,14,15,19,20,22,29,36};
	for (int ix = 0; ix != 11; ix++) {
		l1.push_back(i[ix]);
		l1.push_back(i[ix]);
	}
	
	assert(l1.getSize() == 22);
	assert(l1.uniquify() == 11);
	for (int ix = 0; ix != 11; ix++)
		assert(l1[ix] == i[ix]);
}
