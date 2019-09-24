#ifndef QUADLIST_H_
#define QUADLIST_H_

#define QuadNodePosi(T) QuadNode<T>*
#define MAXINT (int)(0x7fffffff)
#define MININT (int)(0x80000000)

template <typename T>
class QuadNode{
public:
	T entry;
	QuadNodePosi(T) prev;
	QuadNodePosi(T) next;
	QuadNodePosi(T) above;
	QuadNodePosi(T) below;

	QuadNode() = default;
	QuadNode(T entry) :entry(entry) {}
};

template <typename T>
class QuadList{
private:
	QuadNodePosi(T) __head;
	QuadNodePosi(T) __tail;
	int             __size;

public:
	//constructor
	QuadList();

	//external interfaces

	//read-only interfaces
	int  size()  const { return __size; }
	bool empty() const { return size() == 0; }
	QuadNodePosi(T) first() const { return __head->next;}
	QuadNodePosi(T) last()  const { return __tail->prev;}
	QuadNodePosi(T) prev(QuadNodePosi(T) p)  const { return p->prev; }
	QuadNodePosi(T) next(QuadNodePosi(T) p)  const { return p->next; }

	//writable interfaces
	void sethead(T entry) { __head->entry = entry; }
	void settail(T entry) { __tail->entry = entry; }
	void connectTo(QuadList<T>* down);
	void insert_before_above(QuadNodePosi(T) p, T const& entry, QuadNodePosi(T) base = nullptr);
	void insert_after_above(QuadNodePosi(T)p, T const & entry, QuadNodePosi(T) base = nullptr);
	void remove(QuadNodePosi(T) p);
};

//constructor
template <typename T>
QuadList<T>::QuadList(){
	__head = new QuadNode<T>();
	__tail = new QuadNode<T>();
	__size = 0;
	__head->next = __tail;
	__head->prev = __tail;
	__tail->next = __head;
	__tail->prev = __head;
}

//writable interfaces

template <typename T>
void QuadList<T>::connectTo(QuadList<T>* down){
	//assert(down != nullptr);
	__head->below = down->__head;
	__tail->below = down->__tail;
	down->__head->above = __head;
	down->__tail->above = __tail;
}

template <typename T>
void QuadList<T>::insert_before_above(QuadNodePosi(T) p, T const & entry, QuadNodePosi(T) base){
	QuadNodePosi(T) newNode = new QuadNode<T>(entry);
	newNode->next = p;
	newNode->prev = p->prev;
	p->prev = newNode;
	newNode->prev->next = newNode;
	if(base) base->above = newNode;
	newNode->below = base;
	++__size;
}

template <typename T>
void QuadList<T>::insert_after_above(QuadNodePosi(T) p, T const & entry, QuadNodePosi(T) base) {
	insert_before_above(p->prev, entry, base);
}

template <typename T>
void QuadList<T>::remove(QuadNodePosi(T) p){
	//assert(p != head && p != tail);
	p->prev->next = p->next;
	p->next->prev = p->prev;
	--__size;
}

#endif
