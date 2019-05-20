#ifndef STACK_H_
#define STACK_H_

#include "../chp2/Vector.h"

template <typename T>
class Stack: public Vector<T>{
public:
	//stack basic operations
	void push(T const &e) { push_back(e); }
	T    pop() { return pop_back(); }
	T&   top() { return get(getSize() - 1); }
};

#endif
