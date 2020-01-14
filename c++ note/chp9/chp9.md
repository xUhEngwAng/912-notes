Conclusions on Sequential Containers
====================================

## 目录
+ 综述
+ 线性容器基本操作
	- 构造函数
	- 拷贝/赋值/交换
	- 插入与删除
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

此外，六个容器中还有两朵奇葩，即 array 和单向列表。 array 是一个容量固定的容器，而所有其他容器都是可以动态扩容的，这决定了 array 的许多成员函数都与其他容器不同，例如不提供插入或者删除元素的接口；单向列表的设计思想是要保证最优的性能，因此单向列表削减了一些成员变量，比如单向列表不提供`size`接口，因为`O(1)`的`size`接口需要维护一个额外的 size 变量，在每次插入或者删除时都需要对该变量进行修改，这会在一定程度上降低性能。此外，由于单向列表结构的特殊性，它提供的插入和删除接口都与其他容器不同，这也会在后面详细阐述。

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

除 array 外的其他容器都提供了若干插入与删除元素的成员函数，而 array 由于它的定长特性，并不允许元素的插入与删除。除了单向列表以外，这些插入和删除的成员函数都有相同的接口形式，因而更加便于记忆与把握；而单向列表由于它结构上的特殊性，提供了独特的一套插入删除接口，也会在后面涉及到。

> `push_back`与`pop_back`

`push_back`与`pop_back`的设计思想是提供一个高效地在容器后端插入或者删除元素的接口，它的时间复杂度仅为`O(1)`。显然，单向列表并不提供这一接口，因为在单向列表后端插入或删除元素需要`O(n)`的时间复杂度。另外四个容器，即向量，双端队列以及 string以及列表都提供这两个成员函数。 

需要注意的是，`push_back`与`pop_back`都没有返回值。

> `push_front`与`pop_front`

与`push_back`和`pop_back`类似，`push_front`与`pop_front`提供在`O(1)`时间内在容器的**前端**插入或者删除元素的接口。对于向量与 string 而言，在前端插入或者删除元素涉及到其他所有元素的移动，时间复杂度为`O(n)`，因而它们并不提供这一接口。双向队列，列表与单向列表都提供这两个成员函数。

这两个函数也没有返回值。

> `insert`与`erase`

`insert`与`erase`是更加一般地插入或者删除元素的接口，可以在任意位置进行插入或者删除。

对于`insert`而言，有若干接口形式，所有这些接口的第一个参数都是一个迭代器指示的位置信息，表示元素应该被插入的位置。需要注意的是，元素实际上是被插入到该迭代器的前面一个位置，这样当传入的参数是`c.begin()`的时候就可以将新的元素插入到容器的最前端，传入的参数时`c.end()`的时候，将新的元素插入到容器的最后端。因此，`insert`函数，实际上是`insert_before`接口。

+ `insert(p, t)`：将新的元素`t`插入到位置`p`的前面。
+ `insert(p, begin, end)`：将**另一容器**从迭代器`begin`到`end`的元素插入到位置`p`的前面。
+ `insert(p, n, t)`：将n个相同的元素`t`插入到位置`p`的前面。
+ `insert(p, il)`：将初始化列表`il`中的元素插入到位置`p`的前面。

需要注意的是，`insert`返回指向第一个被插入元素的迭代器，如果没有元素被插入的话（比如`begin = end`，则返回第一个参数。利用`insert`的返回值可以实现反复将新的元素插入到容器的最前端

```cpp
list<string> slist;
auto it = slist.begin();
while(cin >> word){
	it = slist.insert(it, word);
}
```

上面的代码与下面这个版本完全等效

```cpp
list<string> slist;
while(cin >> word)
	slist.push_front(word);
```

对于`erase`而言，也提供了两种接口：

+ `erase(p)`：删除迭代器`p`指向的元素。
+ `erase(begin, end)`：删除迭代器`begin`到`end`范围内的所有元素。

两种接口都返回指向被删除的最后一个元素的后一个位置的迭代器。除此以外，stl 还提供了`clear`成员函数，可以将所有元素清除，其效果等价于

```cpp
c.clear();//is equivalent to 
c.erase(c.begin(), c.end());
```

> `emplace`家族

除了`insert`家族以外，`emplace`家族也可以实现将元素插入到容器中的功能。与`insert`不同，`emplace`的作用并非将传入的元素拷贝到容器中，而是**构造**一个新的元素然后再插入到容器中，因此它的参数并非容器的`element_type`，而是`element_type`的构造函数的参数。`emplace`成员函数首先调用元素类型的构造函数，然后将新构造的元素插入到容器中。

与`insert`相同，`emplace`家族也提供了相似的三种成员函数：

+ `emplace_back`：将新构造的元素插入到容器的尾部，显然单向列表并不提供该接口。
+ `emplace_front`：将新构造的元素插入到容器的首部，显然向量和 string 不提供该接口。
+ `emplace`：将新构造的元素插入到容器中的任何位置，与`insert`一样，该接口实际上是`emplace_before`。

`emplace`家族的参数与`insert`家族是类似的，只不过`emplace`并不支持同时插入多个元素，它们的返回值也是一样的，即指向被插入的（第一个）元素的迭代器。

> 单向列表的插入与删除

此前所提及的`insert`，`erase`以及`emplace`接口，都不被单向列表实现。这是由单向列表结构的特殊性决定的。

考虑一种情况，单向列表使用`insert(p, t)`插入元素`t`，为了将元素插入到迭代器`p`的前面，不得不从头遍历单向列表，找到迭代器`p`前面一个位置，然后才能调整指针完成实质的插入操作，该算法的时间复杂度为`O(n)`，而列表这种动态结构的插入与删除理论上是只需要`O(1)`的时间的，该性能显然不能令人满意。对于元素的删除也有同样的问题，因为将迭代器指示的元素释放后，还需要调整它前后的指针，为了找到被删除元素的前驱，也需要耗费`O(n)`的时间。

容易看到，上面操作非常低效，实质上是因为找到某个元素的前驱需要耗费大量的时间。因此单向列表定义了特殊的插入与删除成员函数，规避了这种无意义的查找，即`insert_after`，`erase_after`以及`emplace_after`。

对于`insert_after`而言，由于在插入到迭代器指示位置的后面，迭代器本身就是被插入元素的前驱，在`O(1)`时间内就能实现指针的修改。对于`erase_after`也是同样的道理，被删除元素的前驱就是传入的参数，因此可以在`O(1)`时间内完成指针的修改。

单向列表提供的插入与删除接口，与`insert`，`erase`完全相同：

+ `insert_after(p, t)`
+ `insert_after(p, begin, end)`
+ `insert_after(p, n, t)`
+ `insert_after(p, it)`

所有这些接口都返回指向被插入的**最后**一个元素的迭代器，需要注意`insert`是返回指向被插入的第一个元素的迭代器。如果没有元素被插入的话，仍然是返回`p`。

`erase_after`的两个接口

+ `erase_after(p)`
+ `erase_after(begin, end)`：将从`begin`之后的第一个元素，到（不包括）`end`之间的元素删除。

它们的返回值仍然是指向被删除元素的后一个元素的迭代器。

在这几个接口的语义下，会产生一些新的问题，比如如果想要插入到单向列表的最前面，应该怎么实现呢？因此单向列表还定义了特殊的迭代器`forward_list::before_begin`以及`forward_list::cbefore_begin`，利用这两个迭代器就可以插入或者删除最前面的元素了。

### 元素访问

对线性容器中的元素访问有四种不同的方式：

+ `c.front()`：返回容器中第一个元素的**引用**；
+ `c.back()`：返回容器中最后一个元素的**引用**；
+ `c[n]`：返回容器中第 n 个元素的引用；
+ `c.at(n)`：返回容器中第 n 个元素的引用。

并非所有六个容器都支持这些方式，对这一点的理解还是要基于容器本身的结构。比如单向列表并不支持 `c.back()` 这种方式，因为需要 `O(n)` 的时间才能访问到最后一个元素；同理，不支持随机访问的结构，即单向列表与列表，都不支持 `c[n]`以及`c.at(n)` 这两个接口，因为这两个接口都是基于容器内元素的物理连续性，从而可以利用偏移量 n 快速计算出元素的地址。

### 容量

在前面已经提到过，除了 forward_list 以外，所有其他线性容器都支持复杂度为 `O(1)` 的 `size` 接口，返回当前容器的元素个数。此外，string 还有一个 `length` 成员函数，它的作用与 `size` 是完全相同的。

此外，所有线性容器都提供了 `resize(n)` 接口，使得容器含有 n 个元素。如果容器原先的元素个数大于 n，则将后面的元素删除；而如果原先的元素个数小于 n，新添加的元素被默认初始化。此外，还可以通过 `resize(n, t)` 使得新添加的元素被初始化为 `t`。

对于 vector 和 string 而言，需要更加精细的容量控制。这是因为 forward_list 与 list 都是采用非连续的存储策略，插入元素时只需要分配一个元素的存储空间即可；而 vector 和 string 中元素个数一旦超过了为其预先分配的空间，则不得不重新在内存中为它开辟一个更大的空间，并且将元素依次拷贝过去，整个过程的时间复杂度为`O(n)`。

因此，vector 和 string 需要同时管理 size 和 capacity 信息。两者都可以利用`c.reserve(n)`成员函数，预先在内存中开辟大小为 n （个元素）的空间，执行后有 `size = 0, capacity = n * sizeof(element_type)`，这样在插入 n 个元素前，就都不会进行自动扩容了。

需要注意的是，deque 是没有这样的问题的，因为它的底层结构并非完全连续的，而是由若干连续内存区段组成的，这些段之间并非连续。因此，deque 自动扩容时，只需分配一个区段的存储空间即可，并不会进行元素的拷贝，开销相对较小。


### 遍历

遍历则相对简单。所有的线性容器（乃至关联容器）都提供了一组相同的接口：

+ `c.begin()`, `c.end()`。
+ `c.cbegin()`, `c.cend()`。
+ `c.rbegin()`, `c.rend()`。用于反向遍历容器的场合，显然 forward_list 并不提供该接口。
+ `c.crbegin()`, `c.crend()`。返回常量反向迭代器。

## string 的额外操作

stl 为 string 提供了若干额外的操作接口，在相当程度上简化了 string 的操作。

+ substr(begin, len)
+ extra constructor
	- string str = "c style string";
	- string str
+ find, rfind, find_first_of, find_last_of, find_first_not_of, find_last_not_of
	- find(pattern, pos)
+ append, replace
+ convert
	- to_string
	- stod(double)
	- stoi(integer)
+ isdigit, isalpha, isalnum, iscontrl
+ tolower(c), toupper(c)

## 容器适配器

栈和队列默认是使用 deque 实现的，而优先队列默认是使用 vector 来实现。三个容器适配器都支持的操作：

+ push()
+ emplace()
+ pop()
+ size()
+ empty

获取适配器中顶端（末端）元素

+ stack.top()
+ queue.front(), queue.back()
+ priority_queue.top(), priority_queue.front()

实际上，`algorithm` 头文件中提供了一组与堆相关的函数，我感觉比 priority_queue 灵活很多:

+ `make_heap(begin, end, cmp)`: 对一个 vector 进行建堆。
+ `pop_heap(begin, end, cmp)`： 将堆顶元素与最后一个元素交换，然后对新的堆顶做下滤操作。堆的容量会减少1，但是 vector 的容量不变，需要实质地调用 c.pop_back() 才能删除原先的堆顶。
+ `push_heap(begin, end, cmp)`：调用 c.push_back() 后调用该函数，对插入到尾部的元素进行上滤操作。
+ `sort_heap(begin, end, cmp)`：堆排序，默认是升序。
