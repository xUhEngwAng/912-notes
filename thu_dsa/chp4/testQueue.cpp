#include "Queue.h"
#include <time.h>
#include <cassert>
#include <iostream>
using std::cout;
using std::endl;

void test_constructor();
void test_enqueue();
void test_dequeue();

int main(){
	clock_t begin = clock();
	cout << "Running tests." << endl;

	test_constructor();
	test_enqueue();
	test_dequeue();

	clock_t end = clock();
	cout << "All tests passed." << "Running time: " << end - begin << "ticks." << endl;

	system("pause");
	return 0;
}

void test_constructor(){
	Queue<int> q1;
	Queue<char> q2;
	Queue<double> q3;
	Queue < Queue<int>> q4;
	assert(q1.getSize() == 0);
	assert(q2.getSize() == 0);
	assert(q3.getSize() == 0);
	assert(q4.getSize() == 0);
}

void test_enqueue(){
	Queue<int> q;
	q.enqueue(2);
	assert(q.getSize() == 1);
	q.enqueue(5);
	assert(q.getSize() == 2);
	assert(q.front() == 2);
	assert(q.rear() == 5);
}

void test_dequeue(){
	Queue<int> q;
	q.enqueue(2);
	q.enqueue(5);
	q.enqueue(7);
	assert(q.getSize() == 3);
	assert(q.front() == 2);
	assert(q.rear() == 7);

	assert(q.dequeue() == 2);
	assert(q.dequeue() == 5);
	assert(q.dequeue() == 7);
	assert(q.getSize() == 0);
}
