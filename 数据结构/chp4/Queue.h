#ifndef QUEUE_H_
#define QUEUE_H_

#include "../chp3/List.h"

template <typename T>
class Queue: public List<T>{
public:
	void enqueue(T const &elem) { push_back(elem); }
	T dequeue() { return  pop_front()->val; }
	T front() { return first()->val; }
	T rear() { return last()->val; }
};

#endif
