#ifndef COMPLETEBINARYHEAP_H_
#define COMPLETEBINARYHEAP_H_

#include "../chp2/Vector.h"
#include "PriorityQueue.h"

#define INHEAP(i) ((i >= 0) && (i < getSize()))
#define PARENT(i) ((i-1) >> 1)
#define LCHILD(i) ((i << 1) + 1)
#define RCHILD(i) ((i << 1) + 2)
#define HASPARENT(i) INHEAP(PARENT(i))
#define HASLCHILD(i) INHEAP(LCHILD(i))
#define HASRCHILD(i) INHEAP(RCHILD(i))
#define LASTINTERNAL PARENT(size() - 1)

template <typename K, typename V>
class CBHeap: public Vector<entry<K, V>>, public PriorityQueue<K, V>{
protected:
	//internal methods
	void percolate_down(int pos);
	void percolate_up(int pos);
	int proper_parent(int pos);

public:
	//constructor
	CBHeap() = default;
	CBHeap(entry<K, V>* src, int n);

	int  size() { return getSize(); }
	bool empty(){ return size() == 0; }
	void insert(entry<K, V> e);
	void insert(K key, V value) { insert(entry<K, V>(key, value)); }
	entry<K, V> getMax() { return _elem[0]; }
	entry<K, V> delMax();

};

//internal methods

template <typename K, typename V>
int CBHeap<K, V>::proper_parent(int pos){
	if (!HASLCHILD(pos)) return pos;
	if (!HASRCHILD(pos)) return get(pos) >= get(LCHILD(pos)) ? pos : LCHILD(pos);
	int proper = get(pos) >= get(LCHILD(pos)) ? pos : LCHILD(pos);
	return get(proper) >= get(RCHILD(pos)) ? proper : RCHILD(pos);
}

template <typename K, typename V>
void CBHeap<K, V>::percolate_down(int pos){
	entry<K, V> tmp = get(pos);
	for(int p = proper_parent(pos); p != pos; p = proper_parent(pos)){
		get(pos) = get(p);
		pos = p;
		get(pos) = tmp;
	}
}

template <typename K, typename V>
void CBHeap<K, V>::percolate_up(int pos){
	entry<K, V> tmp = get(pos);
	while(HASPARENT(pos)){
		if(get(PARENT(pos)) >= tmp) break;

		get(pos) = get(PARENT(pos));
		pos = PARENT(pos);
	}
	get(pos) = tmp;
}

//public methods

template <typename K, typename V>
CBHeap<K, V>::CBHeap(entry<K, V>* src, int n){
	copyfrom(src, 0, n);
	for (int ix = LASTINTERNAL; ix >= 0; --ix)
		percolate_down(ix);
}

template <typename K, typename V>
void CBHeap<K, V>::insert(entry<K, V> e){
	push_back(e);
	percolate_up(getSize() - 1);
}

template <typename K, typename V>
entry<K, V> CBHeap<K, V>::delMax(){
	entry<K, V> max = get(0);
	_elem[0] = _elem[_size-- - 1];
	percolate_down(0);
	return max;
}

#endif
