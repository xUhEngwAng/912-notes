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
	void copyfrom(Vector<T> const &V, int lo, int hi);
	void expand();
	//void shrink();

public:
	//constructors
	Vector();
	Vector(int capacity);
	Vector(T* const A, int n) { copyfrom(A, 0, n); }
	Vector(T* const A, int lo, int hi) { copyfrom(A, lo, hi); }
	Vector(Vector<T> const &V) { copyfrom(V._elem, 0, V._size); }
	Vector(Vector<T> const &V, int lo, int hi) { copyfrom(V._elem, lo, hi); }

	//deconstructor
	~Vector() { delete[]_elem; }

	//read-only interfaces
	void print(void);
	int find(T const &elem);
	int find(T const &elem, int lo, int hi);
	int getSize() { return _size; }
	int getCapacity() { return _capacity; }
	bool empty(){ return _size == 0;}

	//writable interfaces
	T& get(int index) { return _elem[index]; }
	void replace(int index, T const &elem) { _elem[index] = elem; }
	T& operator[](int index) { return get(index); }
	Vector<T>& operator=(Vector<T> const &V);
	void push_back(T const &elem);
	void insert(int pos, T const &elem);
	T pop_back(void) { return 	_elem[--_size]; }
	T pop(int index);
	T pop(int lo, int hi);//return the last element popped
	int remove(T const &elem);//remove first element matched, return index, -1 if not found
};

//protected methods
template <typename T>
void Vector<T>::copyfrom(T* const A, int lo, int hi) {
	_capacity = (hi - lo) << 1;
	_size = 0;
	_elem = new T[_capacity];
	while (lo != hi) _elem[_size++] = A[lo++];
}

template <typename T>
void Vector<T>::copyfrom(Vector<T> const &V, int lo, int hi) {
	copyfrom(V._elem, lo, hi);
}

template<typename T>
void Vector<T>::expand(){
	if (_size < _capacity) return;
	//else double capacity
	T* tmp = _elem;
	delete[]_elem;
	copyfrom(tmp, 0, _size);
}

//constructors
template<typename T>
Vector<T>::Vector() {
	_capacity = DEFAULT_CAPACITY;
	_size = 0;
	_elem = new T[_capacity];
}

template<typename T>
Vector<T>::Vector(int capacity) {
	_capacity = capacity;
	_size = 0;
	_elem = new T[_capacity];
}

//read-only interfaces
template<typename T>
void Vector<T>::print() {
	//pass
}

template<typename T>
int Vector<T>::find(T const &elem){
	int pos = _size;
	while (--pos >= 0 && _elem[pos] != elem);
	return pos;
}

template<typename T>
int Vector<T>::find(T const &elem, int lo, int hi){
	int pos = hi;
	while (--pos >= lo && _elem[pos] != elem);
	return pos;
}


// writable interfaces
template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T> const &V) {
	if (_elem) delete[]_elem;
	copyfrom(V, 0, V._size);
	return *this;
}
template<typename T>
void Vector<T>::push_back(T const &elem){
	expand();
	_elem[_size++] = elem;
}

//insert before pos
template<typename T>
void Vector<T>::insert(int pos, T const &elem){
	expand();
	int ix;
	for (ix = _size; ix != pos; --ix)
		_elem[ix] = _elem[ix - 1];
	++_size;
	_elem[ix] = elem;
}

template<typename T>
T Vector<T>::pop(int index){
	T res = _elem[index];
	int ix;
	for (ix = index; ix != _size - 1; ++ix)
		_elem[ix] = _elem[ix + 1];
	--_size;
	return res;
}

template<typename T>
T Vector<T>::pop(int lo, int hi){
	T res = _elem[hi - 1];
	while (hi < _size) _elem[lo++] = _elem[hi++];
	_size -= hi - lo;
	return res;
}

template<typename T>
int Vector<T>::remove(T const &elem){
	int pos = _size;
	while (--pos >= 0 && _elem[pos] != elem);
	if(pos != -1) pop(pos);
	return pos;
}

#endif
