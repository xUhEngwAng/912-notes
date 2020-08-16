#ifndef BTNODE_H_
#define BTNODE_H_

#include "../chp2/Vector.h"

#define BTNodePosi(T) BTNode<T>*

template <typename T>
class BTNode{
public:
	Vector<T> keys;
	Vector<BTNodePosi(T)> children;
	BTNodePosi(T) parent;

	BTNode() { children.push_back(nullptr); }
	BTNode(T const &key, BTNodePosi(T) left = nullptr, BTNodePosi(T) right = nullptr){ 
		keys.push_back(key);
		children.push_back(left);
		children.push_back(right);
	}
};

#endif
