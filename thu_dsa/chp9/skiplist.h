#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#include "dict.h"
#include "quadlist.h"
#include "../chp3/List.h"
#include <iostream>
#include <iomanip>
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

	bool operator==(entry const &e){
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

	int  size()  const { return first()->val->size(); }
	int  level() const { return getSize(); }

	V    get(K key);
	bool put(K key, V value);
	bool remove(K key);
	void print() const;
};

//constructor
template <typename K, typename V>
SkipList<K, V>::SkipList(){
	QuadList<entry<K, V>>* qlist = new QuadList<entry<K, V>>();
	qlist->sethead(entry<K, V>(MININT, 0));
	qlist->settail(entry<K, V>(MAXINT, 0));
	push_back(qlist);
}

//internal methods

template <typename K, typename V>
QuadNode<entry<K, V>>* SkipList<K, V>::skipSearch(K key) {
	QuadNode<entry<K, V>>* qnode = last()->val->first();			//the first quadlist node of the last quadlist
	while (1) {	
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
	ListNode<QuadList<entry<K, V>>*> *qlist = first();
	QuadList<entry<K, V>> *newlist;

	while (qnode->below != nullptr) qnode = qnode->below;
	if (qnode->entry.key != key) qnode = qnode->next;
	qlist->val->insert_before_above(qnode, entry<K, V>(key, value));
	if (next(qlist) == nullptr) {
		newlist = new QuadList<entry<K, V>>();
		newlist->sethead(entry<K, V>(MININT, 0));
		newlist->settail(entry<K, V>(MAXINT, 0));
		newlist->connectTo(qlist->val);
		push_back(newlist);
	}

	while(rand() % 2 == 0){
		belowNode = qnode->prev;
		qlist = next(qlist);
		while (qnode->above == nullptr) qnode = qnode->next;
		qnode = qnode->above;
		qlist->val->insert_before_above(qnode, entry<K, V>(key, value), belowNode);
		if (next(qlist) == nullptr) {
			newlist = new QuadList<entry<K, V>>();
			newlist->sethead(entry<K, V>(MININT, 0));
			newlist->settail(entry<K, V>(MAXINT, 0));
			newlist->connectTo(qlist->val);
			push_back(newlist);
		}
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
		qlist = next(qlist);
		delete tempNode;
	}

	//remove level if empty
	qlist = prev(last());
	while(qlist && qlist->val->empty()){
		pop_back();
		qlist = prev(qlist);
	}

	return true;
}

template <typename K, typename V>
void SkipList<K, V>::print() const{
	QuadList<entry<K, V>>* qlist = first()->val;
	QuadNode<entry<K, V>>* qnode;
	QuadNode<entry<K, V>>* tempNode;
	int* currLevel = new int[size()], idx = 0;
	for(qnode = qlist->first(); qnode->entry.key != MAXINT; qnode = qnode->next){
		currLevel[idx] = 0;
		tempNode = qnode;
		while(tempNode){
			currLevel[idx]++;
			tempNode = tempNode->above;
		}
		++idx;
	}

	for (int jx = level(); jx != 0; --jx) {
		idx = 0;
		for (qnode = qlist->first(); qnode->entry.key != MAXINT; qnode = qnode->next){
			if (currLevel[idx++] >= jx) cout << qnode->entry.key << '\t';
			else                        cout << " " << '\t';
		}
		cout << endl;
	}
}

#endif
