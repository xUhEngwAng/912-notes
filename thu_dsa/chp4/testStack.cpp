#include "Stack.h"
#include "time.h"
#include <iostream>
#include <cassert>

#define SIZE 1000

void test_basic();
void test_capacity();

int test_stack_main(){
	test_basic();
	test_capacity();

	system("pause");
	return 0;
}

void test_basic(){
	Stack<int> stack;
	stack.push(2);
	stack.push(7);
	assert(stack.getSize() == 2);
	assert(stack.top() == 7);
	assert(stack.pop() == 7);
	assert(stack.getSize() == 1);
	assert(stack.top() == 2);
	assert(stack.pop() == 2);
	assert(stack.getSize() == 0);
}

void test_capacity(){
	Stack<int> stack;
	srand((unsigned)time(NULL));
	for (int ix = 0; ix != SIZE; ++ix)
		stack.push(rand());
	assert(stack.getSize() == 1000);
}
