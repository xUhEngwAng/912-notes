#ifndef LISTNODE_H_
#define LISTNODE_H_

#define ListNodePosi(T) ListNode<T>* 

template <typename T>
class ListNode{
public:
	ListNodePosi(T) prev;
	ListNodePosi(T) succ;
	T               val;

	//constructor
	ListNode();
	ListNode(T val, ListNodePosi(T) const &p = nullptr, ListNodePosi(T) const &s = nullptr);

	//external interfaces
	ListNodePosi(T) insertAsPred(T const &val);
	ListNodePosi(T) insertAsSucc(T const &val);
};

/*----------implementation of class ListNode----------*/

//constructor
template <typename T>
ListNode<T>::ListNode(){
	prev = nullptr;
	succ = nullptr;
}

template <typename T>
ListNode<T>::ListNode( T v, ListNodePosi(T) const &p = nullptr, ListNodePosi(T) const &s = nullptr ){
	val  = v;
	prev = p;
	succ = s;
}

//external interfaces
template <typename T>
ListNodePosi(T) ListNode<T>::insertAsPred(T const &val){
	ListNodePosi(T) newNode = new ListNode<T>(val, prev, this);
	prev->succ = newNode;
	prev = newNode;
	return newNode;
}

template <typename T>
ListNodePosi(T) ListNode<T>::insertAsSucc(T const &val){
	ListNodePosi(T) newNode = new ListNode(val, this, succ);
	succ->prev = newNode;
	succ = newNode;
	return newNode;
}

#endif
