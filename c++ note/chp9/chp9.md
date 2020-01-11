Conclusions on Sequential Containers
====================================

## 目录
+ 综述
+ 线性容器基本操作
	- 构造函数
	- 拷贝/赋值/交换
	- 插入
	- 删除
	- 访问
	- 容量
	- 遍历
+ string的额外操作
+ 容器适配器
	- 栈
	- 队列
	- 优先队列

## 综述

这篇文章讨论c++标准库(stl)中的线性容器(`Sequential Container`)，它们包括`vector`，`deque`，`list`，`forward_list`，`array`以及`string`。每个容器提供的成员函数之间具有一些共性，同时也会提供一些特殊的成员函数，理解它们的关键在于了解每个容器背后的基本原理（这应该是数据结构的基础知识），以及相关成员函数的设计思想。

六个线性容器又可以分为两个阵营，向量、双端队列、 string 和 array是第一个阵营，它们在物理上都是连续存储的；列表和单向列表是第二个阵营，它们是采用动态的存储策略。存储策略的不同会导致不同容器的静态操作与动态操作具有不同的性能，因此提供的成员函数也有区别，例如 vector 并不提供在容器的前端插入或者删除元素的成员函数`push_front`和`pop_front`，因为这将需要`O(n)`的时间复杂度。在后面还将详细地阐述这一点。

此外，六个容器中还有两朵奇葩，即 array 和单向列表。 array 是一个容量固定的容器，而所有其他容器都是可以动态扩容的，这决定了 array 的许多成员函数都与其他容器不同，例如不提供插入或者删除元素的接口；单向列表的设计思想是要保证最优的性能，因此单向列表削减了一些成员变量，比如单向列表不提供`size`接口，因为`O(1)`的`size`接口需要维护一个额外的 size 变量，在每次插入或者删除时都需要对该变量进行修改，这会在一定程度上降低性能。此外，由于单向列表结构的特殊性，它提供的插入和删除结构都与其他容器不同，这也会在后面详细阐述。

在实际应用中，对字符串的处理是相当常见的，为了使这部分工作变得简单，string 提供了大量额外的成员函数，本文将对其中比较常用的进行介绍。

最后，本文介绍了容器适配器的概念，并且就其中最常见的三个，即栈，队列，优先队列进行说明。

## 线性容器基本操作

任何一个线性容器都包含一些共有的基本操作，包括初始化，拷贝/赋值/交换，插入元素，删除元素，访问元素，遍历整个容器，以及获得容器的容量信息。下面将分别进行阐述。

### 初始化

线性容器提供了多种构造函数：

+ 默认构造函数。
+ 拷贝构造函数。从另一个**同类型**的容器进行拷贝。这里需要注意的是，对于 array ，它的容量也是它的类型，因此 array 只能从相同元素类型并且容量也相同的 array 进行拷贝构造。

```cpp
Container<type> c1;
Container<type> c2(c1);    //c2 is a copy of c1

//for array type
array<int, 6> arr1;        //arr1 has 6 elements with value 0
array<int, 6> arr2 = arr1; //arr2 is a copy of arr1
array<int, 8> arr3 = arr2; //error, different size
```

+ 指定容量的构造函数，该构造函数还可以指定每个元素的初始值。需要注意的是这个一个 `explicit` 的构造函数。显然 array 不提供该构造函数，否则就会有两个容量信息了。

```cpp
vector<int> ivec1(10);     //ivec1 has 10 elements valued 0
vector<int> ivec2 = 10;    //error, explicit constructor
vector<int> ive3(10, 5);   //ivec3 has 10 elements valued 5

forward_list<float> flist(5);
deque<double> ddeque(20, 3.14);
```

+ 初始化列表(`initializer list`)。需要注意，对于 array ，列表中的元素个数不可以多于 array 的容量，当列表元素少于 array 的容量时，其余的元素被默认初始化，这其实和内置类型的数组是一致的。

```cpp
array<int, 5> arr1 = {1, 2, 3, 4, 5};
array<int, 5> arr2 = {1, 2, 3, 4, 5, 6};     //error
array<int, 5> arr3 = {1};                    //other elements are default initialized to 0

forward_list<float> f_list = {1.2, 3.4, 5.6};//f_list has size 3
deque<double> ddeque = {5.0, 6.9};           //ddeque has size 2
```

+ 通过两个**其他容器**的迭代器指定拷贝的范围。这种方法使得可以从其他类型的容器拷贝。array 是不支持这种方式的，因为指定的范围难以保证与 array 的容量一致。

```cpp
list<int> ilist = {1, 2, 3, 4, 5};
vector<int> ivec(ilist.begin(), ilist.end());   //valid, ivec = {1, 2, 3, 4, 5}
deque<int> ideque(ivec.begin() + 1, ivec.end());//valid, ideque = {2, 3, 4, 5}
```

### 拷贝/赋值/交换

stl中提供了三种拷贝/赋值的方式：

+ 拷贝运算符(copy assignment operator)。和构造函数一样，这种方式也只能支持同类型容器之间的拷贝（或者赋值）。

```cpp
array<int, 10> arr1 = {42};
array<int, 10> arr2 = {1, 2};
array<int, 11> arr3;
arr2 = arr1;                //valid, replace elements in arr2
arr3 = arr2;                //error, different size

vector<int> ivec1 = {1, 2, 3, 4, 5};
vector<int> ivec2(10, 1);
ivec2 = ivec1;              //valid
```

+ `assign`成员函数。`assign`成员函数有多种重载形式，它们的参数其实就类似于对应的构造函数的参数，比如说可以通过两个迭代器来指定拷贝的范围，同样这实现了在不同类型容器之间的拷贝；可以传入一个初始化列表，也可以传入一个正整数代表 size，以及这些元素的值。显然，array 并不提供`assign`成员函数，但是有一个类似的`fill`函数，可以对所有元素赋值。

```cpp
vector<int> ivec = {1, 2, 3, 4, 5};
list<int> ilist1(10, 1), ilist2, ilist3;
ilist1.assign(ivec.begin(), ivec.end());  //ilist1 now has 5 elements
ilsit2.assign({1, 2, 3, 4, 5});           //so does ilist2
ilist3.assign(5, 1);                      //and ilist3

array<int, 5> arr = {1};
arr.assign(ivec.begin(), ivec.end());     //error
arr.assign(5, 2);                         //error
arr.assign({1, 2, 3, 4, 5});              //error
arr.fill(5);                              //arr has 5 elements valued 5
```

+ 通过一个初始化列表进行拷贝。对于 array 而言，初始化列表的长度不能超过 array 的容量，当列表元素少于容量时，其他元素被默认初始化。这与构造函数也是一致的。

```cpp
vector<int> ivec(5, 1);
ivec = {2, 3, 4, 5};                     //ivec has 4 elements

array<int, 5> arr;
arr = {1, 2, 3 ,4, 5};                   //valid
arr = {1, 2, 3, 4, 5, 6};                //error, to many elements
```

此外，stl 还提供了**交换**两个**同类型**容器的方法`swap`。对于除了 array 以外的其他容器，`swap`并不进行元素的拷贝，赋值或者移动等，而是简单地交换两个容器的指针而已，因此可以保证`O(1)`的时间复杂度。然而，交换两个 array 却要做实质的元素移动操作，因此交换 array 的时间复杂度为`O(n)`。

```cpp
vector<string> svec1(10);
vector<string> svec2(24);
swap(svec1, svec2);        //swap two vectors in constant time
svec1.swap(svec2);         //same as the above statement

array<int, 5> arr1 = {1}, arr2 = {2};
array<int, 6> arr3;
swap(arr1, arr2);          //O(n) swap
swap(arr1, arr3);          //error: different size
```

### 插入与删除元素


