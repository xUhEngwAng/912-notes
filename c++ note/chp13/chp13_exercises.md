CPP Primer Chp13 Exercises
==========================

> Exercise 13.1: What is a copy constuctor? When is it used?

拷贝构造函数(copy constructor)是在用拷贝的方式初始化对象的时候，被调用的构造函数，它的形参必须是`const T&`类型，其中`T`为当前类的类名。它会在三种情形下被使用：

+ 拷贝初始化(copy initialization)。比如说

```cpp
myClass object1();//default construction
myClass object2 = object1;//copy initialization
```

+ 函数的形参是一个对象而非对象的引用。此时需要调用拷贝构造函数，从实参拷贝构造一个局部的形参传递到函数体的内部。
+ 函数的返回值是一个对象而非对象的引用。由于函数内部的变量具有临时性，函数返回后就会被销毁，因此需要拷贝构造一个新的对象传递给函数的调用者。

> Exercise 13.2: Explain why the following declaration is illegal:

```cpp
Sales_data::Sales_data(Sales_data rhs);
```

拷贝构造函数的参数必须是对对象的引用。这是因为拷贝构造函数的一个作用，就是在函数的参数为对象的非引用类型时，将实参拷贝构造到形参。如果如上面的声明，对于传入的`rhs`的实参，由于并非是引用类型，需要调用拷贝构造函数将其转化为形参，这个过程将一直持续下去，无法终止。

> Exercise 13.4: Assuming Point is a class type with a public copy constructor, identify each use of the copy constructor in this program fragment:

```cpp
Point global;
Point foo_bar(Point arg)
{
	Point local = arg, *heap = new Point(global);
	*heap = local;
	Point pa[4] = { local, *heap };
	return *heap;
}
```

0. `arg`形参并非引用类型，参数传递对应了第一次调用。
1. `Point local = arg`。拷贝初始化(copy initialization)时调用拷贝构造函数。
2. `heap = new Point(global)`。显式调用拷贝构造函数。
3. `Point pa[4] = { local, *heap };`这里有两次调用。
4. `return *heap;`返回值并非引用类型，需要调用拷贝构造函数。

> Given the following sketch of a class, write a copy constructor that copies all the members. Your constructor should dynamically allocate a new string and copy the object to which ps points, rather than copying ps itself.

```cpp
class HasPtr{
public:
	HasPtr(const std::string &s = std::string()):
		ps(new std::string(s)), i(0){}
private:
	std::string *ps;
	int          i;
};
```

```cpp
//copy constructor
HasPtr::HasPtr(const HasPtr& ptr){
	ps = new string(*ptr.ps);
	i  = ptr.i;
}
```

> Exercise 13.6: What is a copy-assignment operator? When is this operator used? What does the synthesized copy-assignment operator do? When is it synthesized?

拷贝赋值运算符是一个函数，用于将形参对象的值拷贝到当前对象中；当需要对对象进行赋值时，应该使用拷贝复制运算符；默认的拷贝赋值运算符将传入对象的所有非静态(nonstatic)成员拷贝目标对象当中，可以看出这是一个浅拷贝(shallow copy)，当成员变量有指针类型时就会出现问题；如果用户没有定义自己的拷贝赋值运算符，编译器就会生成默认的拷贝赋值运算符。

> Exercise 13.8: Write the assignment operator for the HasPtr class from exercise 13.5 in. As with the copy constructor, your
assignment operator should copy the object to which ps points.

```cpp
HasPtr& HasPtr::operator=(const HasPtr& ptr){
	delete ps;
	ps = new string(*ptr.ps)
	i  = ptr.i;
	return *this;
}
```
