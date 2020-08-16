知识总结：AVL 树
=================

## 平衡树

> 为什么需要平衡树？BST哪里不好吗？

在BST中我们说到，BST是神似`Vector`，因为它的搜索策略非常类似于`Vector`的二分查找。但是我们同时也提到，BST的查找并不是真正意义上的二分查找，这取决于BST的结构。在BST退化为`List`的情况下，它的查找也退化成为`List`的逐个位置遍历查找，需要$O(n)$的时间。这也是BST查找算法的最坏情况。

实际上，考虑平均情况下的BST高度，在由n个互异的词条<随机生成>的BST中，BST的平均高度为$\Theta(logn)$；但如果只考虑不同的BST的拓扑结构的话，即在<随机组成>的情况下，不同的BST的数量满足Catalan数，即$T(n) = Catalan(n)$，此时BST的平均高度为$\Theta(\sqrt{n})$（关于这两个的证明可以看邓公习题集）。

针对这两种不同的统计口径，我们更倾向于采用第二种。这是因为BST的构造过程中，理想的随机并不常见，往往是按单调甚至线性的次序插入；其次，如果采用BST的删除算法，总是删除`succ()`结点的话，BST总是会有向左倾的趋势。因此在实际中，高度很高的BST并不少见，采用后一种结论更加符合实际。

通过上面的分析，我们可以看到，BST算法的性能很大程度取决于它的结构。而在最坏情况下，BST的搜索算法需要$O(n)$的时间。即使在平均的情况下，BST的算法也需要$O(\sqrt{n})$的时间，这个性能不太令人满意。

> 如何对BST进行改进，使之有一个更好的性能？

上面说BST的性能很大程度取决它的结构，因此，如果我们能控制每次操作以后，BST都保持相当的平衡，那么BST的性能就可以得到很大的改进。

能对BST进行结构调整的基础在于BST的<歧义性>，这个是不言自明的，上面还指出所有这些异构的BST的数量满足Catalan数。所有这些BST可以称为等价BST，它们之间都满足<上下可变，左右不乱>的性质。这样就存在调整的可能性，使将一棵不太平衡的BST调整成为一棵平衡的BST。可以证明，经过至多$O(n)$次旋转调整，任何两棵BST均可相互转化（可查看邓公习题集）。

首先想到的必然是完全二叉树，完全二叉树具有最理想的平衡，因为完全二叉树的树高严格等于$[logn]$，我们也称之为理想平衡。但是，如果我们比较任意一棵BST和与之相对应的完全二叉树，就会发现他们的结构相差甚远，要将这样一棵BST转化成完全二叉树，可能需要大量的工作量，往往会需要$O(n)$的时间，这个代价未免太大。

完全二叉树的理想平衡条件可能过于苛刻，因此我们可以稍微放松平衡的标准，也就是所谓的<适度平衡>。所谓适度平衡，是指树的高度也许达不到$[logn]$的理想标准，但是也渐进地不超过$O(logn)$，这样的话，维护这样的适度平衡条件，相对来说就比较容易，一般只需要$O(logn)$的时间。这样的二叉搜索树，就称为平衡二叉搜索树(BBST)。

对于任何一棵BBST，都应该关注它的平衡条件，以及失衡后的调整算法。

## AVL的基本概念

> 什么是AVL?为什么AVL是平衡树？

AVL数是引入了一个平衡因子(Balance Factor)的概念，所谓平衡因子，即左右子树高度之差：
$$
BalanceFactor(v) = height(v.leftChild) - height(v.rightChild)
$$
而AVL的定义是，任何结点的平衡因子的绝对值不超过1。

很明显，这里的平衡因子也只是一个局部的条件，为什么可以得到AVL就是一棵平衡树呢？

对于树高为h的AVL树，我们说它的结点数不少于$fib(h + 3) - 1$，其中$fib(1) = 1, fib(2) = 1$。从而包含n个结点的AVL树其高度不超过$O(logn)$，从而AVL树是一棵适度平衡的BST。其证明如下：

+ 首先对于树高为1的平凡的情况，$n = 1, h = 0$，$fib(h + 3) - 1 = 2 - 1 = 1$，满足上述结论。
+ 对于任意树高h，结点最少的情况下，其左右子树树高分别为$h - 1, h - 2$，从而结点数$T(h) \ge T(h - 1) + T(h - 2) + 1 \ge fib(h + 2) + fib(h + 1) - 1 = fib(h + 3) - 1$。故得证。

由于Fibonacci数呈现一个指数级数，所以反过来，任意含n个结点的AVL树，其树高不高于$O(logn)$。

## AVL的失衡调整算法

AVL的查找可以直接沿用BST的搜索算法。但是在AVL的插入和删除过程中，极有可能会破坏某些结点的AVL平衡条件，因此需要重写AVL的插入和删除算法，使之可以在失衡后重新恢复平衡。

### AVL 插入

> AVL插入情况的分析。

考虑在一次插入后，某个结点发生了失衡，那么本次插入一定是插入在该结点更高的那棵子树上。这样，该结点的树高会增加1，从而使得该结点的祖先结点也有可能因此而发生失衡。实际上，一次插入至多会导致$O(logn)$个结点失衡，并且它们全是被插入结点的祖先。

再考虑第一个失衡的结点，由于是插入到它的较高子树上导致其失衡，因此第一个失衡的结点至少是被插入结点的祖父结点。

经过一次旋转调整后，可以使第一个失衡的结点恢复平衡，同时它的祖先也全部恢复平衡，全树重新平衡。这是因为对第一个失衡结点的旋转调整，将使其树高恢复到插入以前的树高(可以对照旋转的四种情况证明)，从而所有祖先结点也就相应的恢复了平衡。

既然每次插入调整后树高都保持不变，那么AVL的树高如何增长呢？

这是因为插入后有可能平衡性不变，但是高度发生改变。

> AVL插入算法。

根据上面的分析，可以得到AVL插入的算法。简单说来，还是首先调用BST的插入算法，将新的结点插入为叶结点。由于第一个失衡的结点至少是被插入结点的祖父结点，因此可以从它开始沿着路径向上移动，判断当前的结点是否失衡。如果失衡，则调用一次调整算法后，全树就已经恢复了平衡，因此可以直接退出。如果没有失衡，则更新树的高度。整个代码如下：

```cpp
template <typename T>
BinNodePosi(T) AVL<T>::insert(T const &key){
	BinNodePosi(T) &x = search(key);
	if(x) return x;									//key already exists
	//else
	x = new BinNode(key, __hot);
	for(auto v = __hot; v; v = v->parent){
		if(AVLBalanced(v)) updateHeight(v);
		else{
			BinNodePosi(T) p = v->parent;
			BinNodePosi(T) r = rotateAt(higherChild(higherChild(v)));
			if(p) (v == p->leftChild? p->leftChild: p->rightChild) = r;
			else  __root = v;
			break;
		}
	}
	++__size;
	return x;
}
```

### AVL 删除

> AVL删除情况的分析。

删除至多只会导致一个结点失衡，这个结点可以是被删除结点的父结点。这是因为若在删除一个结点x后导致了某个结点v的失衡，则x必是v较低那棵子树上的结点，因此尽管v发生了失衡，但是v的高度不会发生变化，从而v的祖先结点的高度也不会发生变化，故至多只会有一个结点失衡。并且该失衡的结点可以是被删除结点的父结点，这也是不言而喻的。

对第一个失衡的结点进行一次旋转调整，有可能会导致该结点的祖先结点（不一定是父结点）失衡。这是因为删除后的一次旋转调整，可能会导致调整后的子树根节点高度不变，但也有可能高度降低1，因此之后的祖先结点也可能接着发生失衡。并且这种失衡至多会发生$O(logn)$次。所以至多需要$O(logn)$次调整。

与AVL的插入相比较，插入结点的过程就仿佛是一个好孩子，而删除结点的过程仿佛是一个坏孩子。这是因为，尽管一次插入可能会导致多至$O(logn)$个结点发生失衡，但是只需要改正第一次错误，后面的错误就不会再犯；而尽管一次删除操作只会有一个结点失衡，将该失衡结点完成调整后，又有可能导致后续同样的错误，所谓知错不改，所以是个坏孩子。

删除后有可能某一子树平衡性不变，但是高度降低。因此必须完全遍历至根节点，没有中途退出循环的途径。因为无法确定上层祖先是否会失衡。

> AVL删除算法。

根据上面的讨论，可以得到AVL的删除算法。

简单说来，还是首先调用BST的删除，删除掉位于树底部的结点，随后从被删除结点的父亲开始向上移动，判断沿途结点是否失衡。若失衡，则调用失衡调整算法，使之重新恢复平衡。若没有失衡，则更新树的高度。两种情况都需要继续向上移动，直到根节点。整个代码如下：

```cpp
template <typename T>
bool AVL<T>::remove(T const &key){
	BinNOdePosi(T) &x = search(key);
	if(!x) return false; 							//key doesn't exist
	//else
	removeAt(x, __hot);
	for(BinNodePosi(T) v = __hot; v; v = v->parent){
		if(AVLBALANCED(v)) updateHeight(v);
		else{
			BinNodePosi(T) p = v->parent;
			BinNodePosi(T) r = rotateAt(higherChild(higherChild(v)));
			if(p)
				(v == p->leftChild? p->leftChild: p->rightChild) = r;
			else __root = r;
			v = r;
		}
	}
	--__size;
	return true;
}
```

## 3+4重构

我们知道，对于AVL树的调整策略，可以分为四种情况，分别是`zigzig`, `zigzag`, `zagzig`和`zagzag`。但是在上面的代码中，所有的调整步骤全都是调用了同一个`rotateAt()`函数，这是因为邓公统一了这四种平衡调整策略，把它们四种都归入了所谓<3+4重构>的框架中。

> 什么是3+4重构？

其实，AVL重平衡的四种情况，全都是祖孙三代三个结点和它们的四棵子树之间的重构。简单说来，四种情况全都是选取祖孙三代三个结点中，中序遍历序列居中的那个结点作为新的树根，另外两个分别作为左子结点和右子结点，四棵子树相应的作为它们的子树。这个是满足之前提到过的<上下可变，左右不乱>的性质的。

因此可以省去繁杂的分情况讨论，而直接将它们四种情况全部归入这一个算法，即3+4重构。具体的代码如下：

```cpp
template <typename K, typename V>
BinNodePosi(T) BST<K, V>::connect34(BinNodePosi(T) x,  BinNodePosi(T) y,  BinNodePosi(T) z,
									BinNodePosi(T) T0, BinNodePosi(T) T1, BinNodePosi(T) T2, BinNodePosi(T) T3){
	x->leftChild = T0;
	x->rightChild = T1;
	if (T0) T0->parent = x;
	if (T1) T1->parent = x;
	updateHeight(x);

	z->leftChild = T2;
	z->rightChild = T3;
	if (T2) T2->parent = z;
	if (T3) T3->parent = z;
	updateHeight(z);

	x->parent = y;
	z->parent = y;
	y->leftChild = x;
	y->rightChild = z;
	updateHeight(y);
	return y;
}

template <typename K, typename V>
BinNodePosi(T) BST<K, V>::rotateAt(BinNodePosi(T) x){
	BinNodePosi(T) p = x->parent;
	BinNodePosi(T) g = p->parent;
	if(p == g->leftChild){
		if (x == p->leftChild) {
			p->parent = g->parent;
			return connect34(x, p, g, x->leftChild, x->rightChild, p->rightChild, g->rightChild);
		}
		else {
			x->parent = g->parent;
			return connect34(p, x, g, p->leftChild, x->leftChild, x->rightChild, g->rightChild);
		}
	}else{
		if (x == p->leftChild) {
			x->parent = g->parent;
			return connect34(g, x, p, g->leftChild, x->leftChild, x->rightChild, p->rightChild);
		}
		else {
			p->parent = g->parent;
			return connect34(g, p, x, g->leftChild, p->leftChild, x->leftChild, x->rightChild);
		}
	}
}
```

可以看到，`rotateAt`函数只是在内部对这四种情况做一个鉴别，随后就根据每种情况，以相应的参数调用3+4重构算法，从而完成了AVL的重平衡。

可以证明，经过3+4重构后得到的子树，仍然是满足AVL平衡条件的。关于这个，可以对四种情况依次做一个分析。
