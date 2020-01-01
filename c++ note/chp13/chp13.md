Constructor and Copy Control
============================

## 目录

+ 构造函数
	- 初始化列表
	- 委派构造函数
	- 转换构造函数
+ 拷贝控制
	- 拷贝构造函数
	- 拷贝赋值运算符
	- 析构函数以及三者之间的联系

## 构造函数

### 初始化列表

初始化列表(Constructor Initializer List)其实是我比较常用的，但是我之前的理解有很多误区，这里对它进行一些深入的讨论。

初始化列表的功能与在构造函数体内直接赋值是没有任何区别的，比如说下面用初始化列表的代码：

```cpp
Sales_data::Sales_data(const string &s, unsigned int cnt, double price): bookNo(s), units_sold(cnt), revenue(cnt * price){}
```

与

```cpp
Sales_data::Sales_data(const string &s, unsigned int cnt, double price){
	bookNo = s;
	units_sold = cnt;
	revenue = price * cnt;
}
```

在功能上没有任何区别。我之前也是认为两者完全等价，甚至还更加偏向于第二种写法，因为感觉格式要好看一点。实际上，两者在性能上是不相同的。

构造函数的执行顺序，是首先执行初始化列表，继而执行构造函数体内的赋值语句。需要注意的是，即使初始化列表为空，编译器仍然会首先对各个成员变量进行默认的初始化操作，因此相对于第一种版本，第二种版本不仅没有剩下执行初始化列表的时间，还增加了函数体内的变量赋值的开销。如果成员变量是一个较大的类类型的话，两者的时间开销差别还是蛮大的。

因此，应该尽量使用初始化列表来对成员变量进行初始化操作，这样可以获得更优的性能。实际上，在一些情况下，必须使用初始化列表。

对于某一些类型的变量，比如常量变量(const)，或者（左值）引用变量，只能进行初始化操作而不能进行赋值操作。如果一个类含有这些成员变量，则必须使用初始化列表对它们进行初始化。此外，如果一个成员变量是没有默认构造函数的类类型，也必须使用初始化列表对它进行初始化。从这里应该可以看出，尽管在多数情况下都可以忽略，但是初始化和赋值其实是两种不同的操作。

> 初始化列表的执行顺序问题。

初始化列表的执行顺序并非是按列表中变量的先后次序，而是按照变量在类定义的次序。因此，下面的代码是存在问题的：

```cpp
class X {
private:
	int i;
	int j;
public:
	// undefined: i is initialized before j
	X(int val): j(val), i(j) { }
};
```

其中，初始化列表中`i`首先被初始化为`j`，然后再对`j`进行初始化，但是对`i`进行初始化时`j`还是未定义的，就有可能导致后续的错误。

基于上面的讨论，初始化列表的次序最好与成员变量的定义次序相同，并且最好不要用一个成员变量去初始化另一个成员变量，这样就可以避免可能的错误。比如说下面的代码就不存在初始化列表的执行次序问题了：

```cpp
X(int val): j(val), i(val){}
```

### 委派构造函数

在我写代码的时候，也经常存在这样的需要，即一个构造函数是另一个更加详细的构造函数的子集，或者部分操作。这个时候我就不想全部重新写一遍，看起来也不好看，最好的办法就是在后者中调用前者，这就是委派构造函数(delegating constructor)——这个名字应该是指将一部分工作委派给另一个构造函数完成。

委派构造函数的语法也很简单，只需要在`:`后面，本来初始化列表的位置调用被委派的构造函数即可。被委派的构造函数(delegated constructor)的初始化列表和函数体都会相继被执行，之后再执行委派构造函数(delegating constructor)的剩余部分。下面的代码就是委派构造函数的一个例子：

```cpp
class Sales_data {
public:
	// nondelegating constructor initializes members from corresponding arguments
	Sales_data(std::string s, unsigned cnt, double price): bookNo(s), units_sold(cnt), revenue(cnt*price) {}

	// remaining constructors all delegate to another constructor
	Sales_data(): Sales_data("", 0, 0) {}
	Sales_data(std::string s): Sales_data(s, 0,0) {}
	Sales_data(std::istream &is): Sales_data(){ read(is, *this); }
};
```

### 转化构造函数

转化构造函数是一类特殊的构造函数，它只含有一个参数，参数的类型可以是类本身（此时就是拷贝构造函数了，将在后面提到），也可以是其他类型。它的特殊性在于，转化构造函数定义了隐式的类型转化方法，即从参数的类型转化到当前的类类型。

比如说我可以定义一个`Mystring`类型，它基本沿用`std::string`的基本方法，但是增加一个构造函数，将输入的整型转化为对应的字符串，如下：

```cpp
class Mystring{
public:
	string str;

	Mystring() = default;
	//converting constructor
	Mystring(int num): str(to_string(num)){}
}
```

这样，在任何期望一个`Mystring`类型变量的位置，都可以传入一个`int`类型，编译器会自动调用上面定义的转换构造函数，从该`int`类型构造出一个`Mystring`类对象。

例如存在一个外部函数，接收一个`Mystring`类型的对象作为参数，打印出其中的字符串信息：

```cpp
void print(Mystring mystr){
	cout << mystr.str << endl;
}

print(2020);//this call is perfectly legal
```

完全可以对该函数传入一个整型的变量，此时编译器会自动调用转换构造函数，从该整型变量构造出一个`Mystring`类型变量，作为函数的参数。实际上，在`Mystring`对象初始化的时候，还可以采用下面的方式：

```cpp
Mystring mystr = 2020;//copy initialization
```

这种初始化方式称为拷贝初始化(copy initialization)，与直接初始化存在一些区别。在上面的语句中，编译器也会调用转化构造函数，因此该语句与

```cpp
Mystring mystr(2020);//direct initialization
```

这样的直接初始化完全等效。与上面类似，甚至可以直接使用`int`型变量对`Mystring`对象进行赋值。

从上面也可以看出，转化构造函数是存在一些隐患的，比如说`Mystring str = 2020;`这样的语句看起来多少让人看起来有一些迷惑。为了避免这种歧义性，可以手动避免转化构造函数的隐式类型转化，只需要在转化构造函数的声明前面添加`explicit`关键字即可。

```cpp
class Mystring{
public:
	string str;
	explicit Mystring(int num): str(to_string(num)){}
	//other functions
```

这样，就可以避免隐式类型转化了。如果需要将`int`转化为`Mystring`类型，就必须显式调用(explicit)转化构造函数才行。实际上，之前我已经多次遇到转化构造函数了，只是我当时并不知道而已。

例如`std::string`存在一个将`const char*`转化为`string`的构造函数，因此存在下面的`string`初始化与函数调用语句：

```cpp
string str = "Study hard tomorrow.";  //legal
int num    = stoi("2020");            //legal, implicit convert "2020" to string.
```

同理，`vector`类中存在一个构造函数，可以指定`vector`的初始大小，传入的参数是一个`int`变量。但是并不可以这样初始化`vector`:

```cpp
vector<int> iVec = 2020;              //illegal, this constructor is explicit
vector<int> iVec(2020);               //legal, explicit initialization
```

用一个整型变量给`vector`赋值，这看起来也太怪了，存在不少的歧义性，因此这个构造函数被标记为`explicit`了。

需要注意的是，`explicit`关键字只能出现在类构造函数声明的位置，如果该构造函数在类声明体外部被定义，则不能再次添加`explicit`关键字了。

```cpp
//error: explicit allowed only on a constructor declaration in a class header
explicit Mystring::Mystring(int num){
	...
}
```
