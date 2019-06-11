#ifndef BINNODE_H_
#define BINNODE_H_

#include "../chp4/Stack.h"
#include "../chp4/Queue.h"

#define BinNodePosi(T) BinNode<T>*

template <typename T>
class BinNode{
protected:
	template <typename VST> 
	void visitAlongLeftBranch(VST &visit, Stack<BinNodePosi(T)> &S);
	void goAlongLeftBranch(Stack<BinNodePosi(T)> &S);
	void goToHLVFL(Stack<BinNodePosi(T)> &S);							//Highest Leaf Visible From Left

public:
	BinNodePosi(T) parent;
	BinNodePosi(T) leftChild;
	BinNodePosi(T) rightChild;
	T              data;
	int            height;
	int            color;
	int            npl;

	//constructor
	BinNode() : parent(nullptr), leftChild(nullptr), rightChild(nullptr), height(0), color(0), npl(0) {};
	BinNode(T const &val) : parent(nullptr), leftChild(nullptr), rightChild(nullptr), data(val), height(0), color(0), npl(0) {};
	BinNode(T const &val, BinNodePosi(T) p): parent(p), leftChild(nullptr), rightChild(nullptr), data(val), height(0), color(0), npl(0) {};

	int size() const;									//compute the size of the tree rooted at current node
	BinNodePosi(T) insertAsLC(T const &val);			//always assume that this.leftChild  == nullptr
	BinNodePosi(T) insertAsRC(T const &val);			//always assume that this.rightChild == nullptr
	BinNodePosi(T) succ();								//return the direct successor of current node

	//tree traversal
	template <typename VST> void preOrder_Re(VST &visit);
	template <typename VST> void preOrder_It1(VST &visit);
	template <typename VST> void preOrder_It2(VST &visit);
	template <typename VST> void inOrder_Re(VST &visit);
	template <typename VST> void inOrder_It1(VST &visit);
	template <typename VST> void inOrder_It2(VST &visit);
	template <typename VST> void postOrder_Re(VST &visit);
	template <typename VST> void postOrder_It(VST &visit);
	template <typename VST> void levelOrder(VST &visit);
};

//protected methods

template <typename T> template <typename VST>
void BinNode<T>::visitAlongLeftBranch(VST &visit, Stack<BinNodePosi(T)> &S){
	BinNodePosi(T) curr = this;
	while(curr){
		visit(curr->data);
		S.push(curr->rightChild);
		curr = curr->leftChild;
	}
}

template <typename T>
void BinNode<T>::goAlongLeftBranch(Stack<BinNodePosi(T)> &S){
	BinNodePosi(T) curr = this;
	while (curr) {
		S.push(curr);
		curr = curr->leftChild;
	}
}

template <typename T>
void BinNode<T>::goToHLVFL(Stack<BinNodePosi(T)> &S){
	BinNodePosi(T) curr;
	while(curr = S.top()){
		if(curr->leftChild){
			if (curr->rightChild) S.push(curr->rightChild);
			S.push(curr->leftChild);
		}
		else S.push(curr->rightChild);
	}
	S.pop();
}

//public interfaces

template <typename T>
int BinNode<T>::size() const {
	int leftSize = this->leftChild ? this->leftChild->size() : 0;
	int rightSize = this -> rightChild ? this->rightChild->size() : 0;
	return leftSize + rightSize + 1;
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsLC(T const &val){
	BinNodePosi(T) currNode = new BinNode<T>(val);
	currNode->parent = this;
	this->leftChild  = currNode;

	return currNode;
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsRC(T const &val){
	BinNodePosi(T) currNode = new BinNode<T>(val);
	currNode->parent = this;
	this->rightChild = currNode;

	return currNode;
}

template <typename T>
BinNodePosi(T) BinNode<T>::succ(){
	BinNodePosi(T) succ;
	if(rightChild){
		succ = rightChild;
		while (succ->leftChild) succ = succ->leftChild;
	}else{
		succ = this;
		while(succ->parent && succ == succ->parent->rightChild) succ = succ->parent;
		succ = succ->parent;
	}
	return succ;
}

//tree traversal

template <typename T> template <typename VST>
void BinNode<T>::preOrder_Re(VST &visit){
	visit(this->data);
	if(this->leftChild)  this->leftChild->preOrder_Re(visit);
	if(this->rightChild) this->rightChild->preOrder_Re(visit);
}

template <typename T> template <typename VST>
void BinNode<T>::preOrder_It1(VST &visit) {
	Stack<BinNodePosi(T)> S;
	BinNodePosi(T) curr = this;
	S.push(curr);
	while(!S.empty()){
		curr = S.pop();
		visit(curr->data);
		if(curr->rightChild) S.push(curr->rightChild);
		if(curr->leftChild)  S.push(curr->leftChild);
	}
}

template <typename T> template <typename VST>
void BinNode<T>::preOrder_It2(VST &visit){
	Stack<BinNodePosi(T)> S;
	BinNodePosi(T) curr = this;
	while(true){
		if (curr) curr->visitAlongLeftBranch(visit, S);
		if (S.empty()) break;
		curr = S.pop();
	}
}

template <typename T> template <typename VST> 
void BinNode<T>::inOrder_Re(VST &visit){
	if (this->leftChild) this->leftChild->inOrder_Re(visit);
	visit(this->data);
	if (this->rightChild) this->rightChild->inOrder_Re(visit);
}

template <typename T> template <typename VST>
void BinNode<T>::inOrder_It1(VST &visit){
	Stack<BinNodePosi(T)> S;
	BinNodePosi(T) curr = this;
	while(true){
		curr->goAlongLeftBranch(S);
		if (S.empty()) break;

		curr = S.pop();
		visit(curr->data);
		curr = curr->rightChild;
	}
}

template <typename T> template <typename VST>
void BinNode<T>::inOrder_It2(VST &visit) {
	BinNodePosi(T) curr = this;
	bool backTrack = false;
	while(curr){
		if (!backTrack) {
			while (curr->leftChild) curr = curr->leftChild;
			backTrack = true;
		}
		else{
			visit(curr->data);
			if (curr->rightChild) {
				curr = curr->rightChild;
				backTrack = false;
			}
			else{
				while (curr->parent && curr == curr->parent->rightChild) curr = curr->parent;
				curr = curr->parent;
			}
		}
	}
}

template <typename T> template <typename VST>
void BinNode<T>::postOrder_Re(VST &visit){
	if (this->leftChild)  this->leftChild->postOrder_Re(visit);
	if (this->rightChild) this->rightChild->postOrder_Re(visit);
	visit(this->data);
}

template <typename T> template <typename VST>
void BinNode<T>::postOrder_It(VST &visit){
	Stack<BinNodePosi(T)> S;
	BinNodePosi(T) curr = this;
	S.push(curr);
	while(!S.empty()){
		if(S.top() != curr->parent) goToHLVFL(S);
		curr = S.pop();
		visit(curr->data);
	}
}

template <typename T> template <typename VST>
void BinNode<T>::levelOrder(VST &visit){
	Queue<BinNodePosi(T)> q;
	BinNodePosi(T) curr = this;
	q.enqueue(curr);
	while(!q.empty()){
		curr = q.dequeue();
		visit(curr->data);
		if (curr->leftChild)  q.enqueue(curr->leftChild);
		if (curr->rightChild) q.enqueue(curr->rightChild);
	}
}

#endif
