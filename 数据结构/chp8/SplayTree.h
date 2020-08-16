#ifndef SPLAYTREE_H_
#define SPLAYTREE_H_

#include "../chp7/BST.h"

#define T entry<K, V>
template <typename K, typename V>
class SplayTree: public BST<K, V>{
protected:
	BinNodePosi(T) splay(BinNodePosi(T) x);
	BinNodePosi(T) zig(BinNodePosi(T) p);						//right rotation
	BinNodePosi(T) zag(BinNodePosi(T) p);						//left  rotation
	BinNodePosi(T) zigzig(BinNodePosi(T) g);
	BinNodePosi(T) zagzag(BinNodePosi(T) g);

public:
	BinNodePosi(T)& search(K const &key);
	BinNodePosi(T)  insert(K const &key, V const &value);
	bool            remove(K const &key);
};

//implementations

//protected methods

template <typename K, typename V>
BinNodePosi(T) SplayTree<K, V>::splay(BinNodePosi(T) x){
	BinNodePosi(T) p;
	BinNodePosi(T) g;
	BinNodePosi(T) hot;
	while(x != __root){
		p = x->parent, g = p->parent;
		if (!g) {
			hot = p->parent;
			x = (x == p->leftChild ? zig(p) : zag(p));
			x->parent = hot;
			if (hot) (p == hot->leftChild)? hot->leftChild : hot->rightChild = x;
		}
		else {
			if (p == g->leftChild) {
				if (x == p->leftChild) {							//zig-zig
					hot       = g->parent;
					x         = zigzig(g);
					x->parent = hot;
					if (hot) (g == hot->leftChild ? hot->leftChild : hot->rightChild) = x;
				}
				else {												//zig-zag
					hot       = g->parent;
					x         = connect34(p, x, g, p->leftChild, x->leftChild, x->rightChild, g->rightChild);
					x->parent = hot;
					if (hot) g == hot->leftChild ? hot->leftChild : hot->rightChild = x;
				}
			}
			else {
				if (x == p->leftChild) {							//zag-zig
					hot       = g->parent;
					x         = connect34(g, x, p, g->leftChild, x->leftChild, x->rightChild, p->rightChild);
					x->parent = hot;
					if (hot) g == hot->leftChild ? hot->leftChild : hot->rightChild = x;
				}
				else {												//zag-zag
					hot       = g->parent;
					x         = zagzag(g);
					x->parent = hot;
					if (hot) g == hot->leftChild ? hot->leftChild : hot->rightChild = x;
				}
			}
		}
		if (!x->parent) __root = x;
	}
	return x;
}

template <typename K, typename V>
BinNodePosi(T) SplayTree<K, V>::zig(BinNodePosi(T) p){
	BinNodePosi(T) x = p->leftChild;
	p->leftChild = x->rightChild;
	if (x->rightChild) x->rightChild->parent = p;

	x->rightChild = p;
	p->parent = x;

	updateHeight(p);
	updateHeight(x);
	return x;
}

template <typename K, typename V>
BinNodePosi(T) SplayTree<K, V>::zag(BinNodePosi(T) p) {
	BinNodePosi(T) x = p->rightChild;
	p->rightChild = x->leftChild;
	if (x->leftChild) x->leftChild->parent = p;

	x->leftChild = p;
	p->parent = x;

	updateHeight(p);
	updateHeight(x);
	return x;
}

template <typename K, typename V>
BinNodePosi(T) SplayTree<K, V>::zigzig(BinNodePosi(T) g) {
	BinNodePosi(T) p = g->leftChild;
	BinNodePosi(T) x = p->leftChild;

	p->leftChild = x->rightChild;
	if (x->rightChild) x->rightChild->parent = p;

	x->rightChild = p;
	p->parent = x;

	g->leftChild = p->rightChild;
	if (p->rightChild) p->rightChild->parent = g;

	p->rightChild = g;
	g->parent = p;

	updateHeight(g);
	updateHeight(p);
	updateHeight(x);

	return x;
}

template <typename K, typename V>
BinNodePosi(T) SplayTree<K, V>::zagzag(BinNodePosi(T) g) {
	BinNodePosi(T) p = g->rightChild;
	BinNodePosi(T) x = p->rightChild;

	p->rightChild = x->leftChild;
	if (x->leftChild) x->leftChild->parent = p;

	x->leftChild = p;
	p->parent = x;

	g->rightChild = p->leftChild;
	if (p->leftChild) p->leftChild->parent = g;

	p->leftChild = g;
	g->parent = p;

	updateHeight(g);
	updateHeight(p);
	updateHeight(x);

	return x;
}

//public interfaces

template <typename K, typename V>
BinNodePosi(T)& SplayTree<K, V>::search(K const &key){
	BinNodePosi(T) x = searchIn(__root, key, __hot = nullptr);
	if (x) splay(x);
	else if(__hot) splay(__hot);
	return __root;
}

template <typename K, typename V>
BinNodePosi(T) SplayTree<K, V>::insert(K const &key, V const &value) {
	BinNodePosi(T) x = search(key);
	if (x && x->data.key == key) return x;
	//else
	__root = new BinNode<T>(entry<K, V>(key, value));
	++__size;
	if (!x) return __root;

	if(key < x->data.key){
		__root->leftChild  = x->leftChild;
		__root->rightChild = x;
		x->leftChild       = nullptr;
		x->parent          = __root;
	}else{
		__root->leftChild  = x;
		__root->rightChild = x->rightChild;
		x->rightChild      = nullptr;
		x->parent          = __root;
	}
	updateHeight(x);
	updateHeight(__root);
	return __root;
}

template <typename K, typename V>
bool SplayTree<K, V>::remove(K const &key){
	BinNodePosi(T) x = search(key);
	if (!x || x->data.key != key) return false;
	//else
	if(!__root->leftChild){
		__root = __root->rightChild;
		if(__root) __root->parent = nullptr;
		search(key);
	}
	else if(!__root->rightChild){
		__root = __root->leftChild;
		__root->parent = nullptr;
		search(key);
	}
	else {
		__root = x->rightChild;
		__root->parent = nullptr;
		search(key);						//move x's succ to root, and __root has no left child(for succ has the smallest key
		__root->leftChild = x->leftChild;
		x->leftChild->parent = __root;
		updateHeight(__root);
	}
	--__size;
	delete x;
	return true;
}

#endif
