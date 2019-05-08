#include "Vector.h"

//protected methods
template <typename T>
void Vector::copyfrom(T* const A, int lo, int hi){
	int ix = 0;
	while (lo != hi) _elem[ix++] = A[lo++];
}

template <typename T>
void Vector::copyfrom(Vector<T>& const V, int lo, int hi){
	copyfrom(V._elem, lo, hi);
}

//constructors
template<typename T>
Vector::Vector() {
	_capacity = DEFAULT_CAPACITY;
	_size = 0;
	_elem = new T[_capacity];
}

template<typename T>
Vector::Vector(int capacity) {
	_capacity = capacity;
	_size = 0;
	_elem = new T[_capacity];
}

template<typename T>
Vector::Vector(T* const A, int lo, int hi){
	_capacity = MAX(DEFAULT_CAPACITY, (hi - lo) >> 1);
	_size = hi - lo;
	copyfrom(A, lo, hi);
}

template<typename T>
Vector(Vectort<T>& const V){
	_size = V.getSize();
	_capacity = _size >> 1;
	copyfrom(V._elem, 0, _size);
}

template<typename T>
Vector::Vector(Vector<T>& const V, int lo, int hi){
	_capacity = MAX(DEFAULT_CAPACITY, (hi - lo) >> 1);
	_size = hi - lo;
	copyfrom(V, lo, hi);
}

//read-only interfaces
template<typename T>
Vector::print(){
	
}
