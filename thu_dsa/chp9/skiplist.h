#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#include "dict.h"
#include "quadlist.h"
#include "../chp3/List.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using std::cerr;
using std::endl;

template <typename K, typename V>
class entry{
public:
	K key;
	V value;

	entry() = default;
	entry(K key, V value) : key(key), value(value) {}

	operator==(entry const &e){
		return key == e.key;
	}
};

template <typename K, typename V>
class SkipList: Dict<K, V>, List<QuadList<entry<K, V>>*>{
protected:
	QuadNode<entry<K, V>>* skipSearch(K key);

public:
	//constructor
	SkipList();

	int  size()  const { return first()->size(); }
	int  level() const { return getSize(); }

	V    get(K key);
	bool put(K key, V value);
	bool remove(K key);
};

//constructor
template <typename K, typename V>
SkipList<K, V>::SkipList(){
	QuadList<entry<K, V>>* qlist = new QuadList<entry<K, V>>();
	push_back(qlist);
}

//internal methods

template <typename K, typename V>
QuadNode<entry<K, V>>* SkipList<K, V>::skipSearch(K key) {
	while (1) {
		QuadNode<entry<K, V>>* qnode = last()->first();					//the first quadlist node of the last quadlist
		while (qnode->entry.key < key) qnode = qnode->next;
		if (qnode->entry.key == key) return qnode;
		//else
		qnode = qnode->prev;
		if (qnode->below == nullptr) return qnode;
		//else
		qnode = qnode->below;
	}
}

//external interfaces

template <typename K, typename V>
V SkipList<K, V>::get(K key){
	QuadNode<entry<K, V>>* res;
	res = skipSearch(key);
	if(res->entry.key != key){
		cerr << "key:" << key << "is not in SkipList!" << endl;
	}
	return res->entry.value;
}

template <typename K, typename V>
bool SkipList<K, V>::put(K key, V value){
	QuadNode<entry<K, V>>* qnode = skipSearch(key);
	QuadNode<entry<K, V>>* belowNode;
	ListNode<QuadList<entry<K, V>>*>* qlist = first();

	while (qnode->below != nullptr) qnode = qnode->below;
	if (qnode->entry.key != key) qnode = qnode->next;
	qlist->val->insert_before_above(qnode, entry<K, V>(key, value));

	srand((int)time(0));
	while(rand() & 1){
		belowNode = qnode->prev;
		qlist = next(qlist);
		if(qlist->val->empty()){
			push_back(new QuadList<entry<K, V>>());
		}
		while (qnode->above == nullptr) qnode = qnode->next;
		qnode = qnode->above;
		qlist->val->insert_before_above(qnode, entry<K, V>(key, value), belowNode);
	}
	return true;
}

template <typename K, typename V>
bool SkipList<K, V>::remove(K key){
	QuadNode<entry<K, V>>* qnode = skipSearch(key);
	QuadNode<entry<K, V>>* tempNode;
	ListNode<QuadList<entry<K, V>>*>* qlist = first();
	if (qnode->entry.key != key) return false;
	//else
	while (qnode->below) qnode = qnode->below;
	
	while(qnode){
		tempNode = qnode;
		qlist->val->remove(qnode);
		qnode = qnode->above;
		qlist = qlist->val->next();
		delete tempNode;
	}
	return true;
}

#endif
