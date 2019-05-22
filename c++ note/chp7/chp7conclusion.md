Conlusions on Chapter Seven
===========================

## 7.1 Defining Abstract Data Type

> 什么是抽象数据类型？

主要可以参考邓公的网课和教材，基本概念和邓公讲的是一致的。抽象数据类型的关键是两个概念，即封装和数据抽象。为此，需要弄清楚`interface`和`implementation`之间的区别与联系。

### 7.1.2 Defining Member Functions

> 首先思考一个问题，当我们调用类的成员函数的时候，如`cppstring Sales_data::isbn() const { return bookNo;}`成员函数是怎么找到当前对象的`bookNo`成员变量的呢？

`this`指针。成员函数实际上是有一个隐性的变量，即`this`。当调用成员函数的时候，`this`指针会被初始化为指向被调用对象的地址。比如说，当调用

```cpp
total.isbn();
``` 

的时候，编译器将`this`指针设置为`total`的地址。这就好像`python`一样，所以这个成员函数的调用可以写成

```cpp
Sales_data::isbn(&total);
```

这样，利用`this`指针，就可以随机索引当前对象的任何一个成员变量了。

> 可以看到，上面的`isbn`成员函数有一个修饰符`const`，这个`const`修饰符有什么作用？

这个可以有两个理解方法。

+ 从语言的功能层面，一个`const`的对象只能调用它的标记为`const`的成员函数。这是很自然的，因为编译器会认为没有`const`标记的成员函数会修改对象的成员变量。所以，对于只读的成员函数，尽量都标记为`const`,以便其`const`的对象也可以调用这些函数。

+ 从变量类型的方面。对于一般的对象，如`Sales_data total`这种，`this`指针的类型为`Sales_data* const`，即`this`指针是`const pointer to non-const object`，在这种情况下，`this`指针是不能指向`const`的对象的。为了指定`this`指针的类型为`const Sales_data* cosnt`，即`const pointer to const object`，就需要在函数的参数列表和具体实现之间加上`const`修饰符。实际上，这也是`const`对象只能调用`const`成员函数在语言层面的原因。

- class scope and defining a member function outside the class 
- a function to return `this` object

### 7.1.3 defining nonmember class-related function
- read and print function. io classes

### 7.1.4 constructors
- constructors cannot be `const`
- synthesized default constructor and in-class intializers
- classes with members of built-in or compound types should not rely on the synthesized default constructor
	+ so we must define the default constructor for three reasons
- =default
- constructor initializer list
- Defining a Constructor outside the Class Body

### 7.1.5. Copy, Assignment, and Destruction
- Synthesized Versions
- cases then Synthesized Versions cannot be used
	+ classes that allocate resources that reside outside the class objects themselves
	+ vertor and string members
