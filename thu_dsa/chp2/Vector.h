#ifndef VECTOR_H_
#define VECTOR_H_

#include <iostream>
#define DEFAULT_CAPACITY 3

#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))

using std::cout;
using std::endl;

template<typename T>
class Vector{
protected:
	T* _elem;
	int _size;
	int _capacity;

	//protected methods
	void copyfrom(T* const A, int lo, int hi);
	void copyfrom(Vector<T>& const V, int lo, int hi);
	void expand();
	void shrink();

public:
	//constructors
	Vector();
	Vector(int capacity);
	Vector(T* const A, int n);
	Vector(T* const A, int lo, int hi);
	Vector(Vectort<T>& const V);
	Vector(Vector<T>& const V, int lo, int hi);

	//deconstructor
	~Vector();

	//read-only interfaces
	void print(void);
	int find(T& const elem);
	int find(T& const elem, int lo, int hi);
	int getSize() { return _size; }
	int getCapacity(){return _capacity}

	//writable interfaces
	T& get(int index);
	T& put(int index, T& elem);
	T& operator[](int index) const;
	int push_back(T& const elem);
	int insert(T& const elem);
	T pop_back(void);
	T pop(int index);
	T pop(int hi, int lo);//return the last element popped
	int remove(T& const elem);
};

#endif
