#ifndef VECTOR_H_
#define VECTOR_H_

#include "Fib.h"
#include <iostream>

#define DEFAULT_CAPACITY 3
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))

using std::cout;
using std::endl;

template<typename T>
class Vector{
protected:
	T*  _elem;
	int _size;
	int _capacity;

	//protected methods
	void copyfrom(T* const A, int lo, int hi);
	void copyfrom(Vector<T> const &V, int lo, int hi);
	void expand();
	void swap(T &one, T &two);
	void merge(int lo, int mid, int hi);
	int bubble(int lo, int hi);
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
	int search(T const &elem, int lo, int hi);
	int fib_search(T const &elem, int lo, int hi);
	int binary_search(T const &elem, int lo, int hi);
	int interpolation_search(T const &elem, int lo, int hi);
	int getSize() { return _size; }
	int getCapacity() { return _capacity; }
	int unordered() { return unordered(0, _size); }
	int unordered(int lo, int hi);
	bool empty(){ return _size == 0;}

	//writable interfaces
	T& get(int index) { return _elem[index]; }
	void replace(int index, T const &elem) { _elem[index] = elem; }
	T& operator[](int index) { return get(index); }
	Vector<T>& operator=(Vector<T> const &V);
	void push_back(T const &elem);
	void insert(int pos, T const &elem);
	T pop_back(void) { return _elem[--_size]; }
	T pop(int index);
	T pop(int lo, int hi);//return the last element popped
	int remove(T const &elem);//remove first element matched, return index, -1 if not found
	int unique(void); //return the number of elements deleted
	int uniquify(void);//unique method for sorted vector
	void map(void(*visit)(T&));
	template <typename VST> void map(VST& visit);

	//sorting algorithms
	void bubbleSort() { bubbleSort(0, _size); }
	void bubbleSort(int lo, int hi);
	void mergeSort() { mergeSort(0, _size); }
	void mergeSort(int lo, int hi);
};

//protected methods
template <typename T>
void Vector<T>::copyfrom(T* const A, int lo, int hi) {
	_capacity = MAX((hi - lo) << 1, DEFAULT_CAPACITY);
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
	copyfrom(tmp, 0, _size);
	delete[]tmp;
}

template <typename T>
void Vector<T>::swap(T &one, T &two){
	T temp;
	temp = one;
	one = two;
	two = temp;
}

template <typename T>
void Vector<T>::merge(int lo, int mid, int hi){
	int leftLen = mid - lo;
	T* tmp = new T[leftLen];
	for (int ix = 0; ix != leftLen; ++ix) tmp[ix] = _elem[ix + lo];

	for (int pos = lo, ix = 0, jx = mid; ix != leftLen;) {
		if (hi <= jx || tmp[ix] <= _elem[jx]) _elem[pos++] = tmp[ix++];
		if (jx < hi && _elem[jx] < tmp[ix])  _elem[pos++] = _elem[jx++];
	}
}

template <typename T>
int Vector<T>::bubble(int lo, int hi){
	int lastSwap = 0;
	while(++lo != hi){
		if(_elem[lo] < _elem[lo - 1]){
			swap(_elem[lo], _elem[lo - 1]);
			lastSwap = lo;
		}
	}
	return lastSwap;
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

template <typename T>
int Vector<T>::search(T const &elem, int lo, int hi){
	int mid;
	while (lo < hi) {
		mid = (lo + hi) >> 1;
		if (elem < _elem[mid]) hi = mid;
		else if (_elem[mid] < elem) lo = mid + 1;
		else return mid;
	}
	return lo - 1;
}
template <typename T>
int Vector<T>:: fib_search(T const &elem, int lo, int hi){
	Fib fib(hi - lo);
	int mid;
	while(lo < hi){
		while (hi - lo <= fib.get()) fib.prev();
		mid = fib.get() + lo;
		if (elem < _elem[mid]) hi = mid;
		else if (_elem[mid] < elem) lo = mid + 1;
		else return mid;
	}
	return lo - 1;
}

template <typename T>
int Vector<T>::binary_search(T const &elem, int lo, int hi){
	int mid;
	while(lo < hi){
		mid = (lo + hi) >> 1;
		elem < _elem[mid] ? hi = mid : lo = mid + 1;
	}
	return lo - 1;
}

template <typename T>
int Vector<T>::interpolation_search(T const &elem, int lo, int hi){
	int mid;
	while(lo < hi){
		mid = lo + (elem - _elem[lo])*(hi - lo) / (_elem[hi] - _elem[lo]);
		if (mid < lo || mid >= hi) break;

		if (elem < _elem[mid]) hi = mid;
		else if (_elem[mid] < elem) lo = mid + 1;
		else return mid;
	}
	return lo - 1;
}

template <typename T>
int Vector<T>::unordered(int lo, int hi){
	int reversed = 0;
	while(++lo != hi){
		if (_elem[lo] < _elem[lo - 1]) ++reversed;
	}
	return reversed;
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

template <typename T>
int Vector<T>::unique(){
	int count = 0;
	for(int pos = 1; pos != _size;){
		if (find(_elem[pos], 0, pos) == -1) ++pos;
		else{
			pop(pos);
			++count;
		}
	}
	return count;
}

template <typename T>
int Vector<T>::uniquify(){
	int i = 0, j = 0;
	while (++j != _size)
		if (_elem[i] != _elem[j]) _elem[++i] = _elem[j];
	_size = i + 1;
	return j - i - 1;
}

template <typename T>
void Vector<T>::map(void(*visit)(T&)) {
	for (int ix = 0; ix != _size; ++ix) visit(_elem[ix]);
}

template <typename T> template <typename VST>
void Vector<T>::map(VST& visit) {
	for (int ix = 0; ix != _size; ++ix) visit(_elem[ix]);
}

//sorting algorithms
template <typename T>
void Vector<T>::bubbleSort(int lo, int hi){
	while (lo < (hi = bubble(lo, hi)));
}

template <typename T>
void Vector<T>::mergeSort(int lo, int hi){
	if (hi - lo < 2) return;

	int mid = (lo + hi) >> 1;
	mergeSort(lo, mid);
	mergeSort(mid, hi);
	merge(lo, mid, hi);
}

#endif
