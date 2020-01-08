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

构造函数的执行顺序，是首先执行初始化列表，继而执行构造函数体内的赋值语句。需要注意的是，即使初始化列表为空，编译器仍然会首先对各个成员变量进行默认的初始化操作，因此相对于第一种版本，第二种版本不仅没有省下执行初始化列表的时间，还增加了函数体内的变量赋值的开销。如果成员变量是一个较大的类类型的话，两者的时间开销差别还是蛮大的。

因此，应该尽量使用初始化列表来对成员变量进行初始化操作，这样可以获得更优的性能。实际上，在一些情况下，必须使用初始化列表。

对于某一些类型的变量，比如常量变量(const)，或者（左值）引用变量，必须在声明的时候同时对它们初始化。如果一个类含有这些成员变量，则必须使用初始化列表对它们进行初始化。此外，如果一个成员变量是没有默认构造函数的类类型，也必须使用初始化列表对它进行初始化。从这里应该可以看出，尽管在多数情况下都可以忽略，但是初始化和赋值其实是两种不同的操作。

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

## 拷贝控制

拷贝控制(copy control)是指一个类控制它的对象在拷贝，移动，赋值，析构操作下所进行的活动。其中包含五个函数（操作），即拷贝构造函数(copy constructor)，拷贝赋值运算符(copy assignment operator)，移动构造函数(move constructor)，移动赋值运算符(move assignment operator)以及析构函数(destructor)。下面将对它们分别进行讨论。

### 拷贝构造函数

拷贝构造函数是一类特殊的构造函数，它的参数为同类对象的一个引用，该引用通常是一个常量(const)引用，尽管非常量也是可以的。

```cpp
class Mystring{
public:
	string str;

	//default constructor
	Mystring() = default;
	//copy constructor
	Mystring(const Mystring& right);
	//converting constructor
	Mystring(int num);
}
```

顾名思义，拷贝构造函数的作用，就是在对象被拷贝初始化(copy initialization)的时候被调用，它规定了新的对象应该如何从被拷贝对象进行复制：

```cpp
Mystring str1;         //default initialization
Mystring str2 = str1;  //copy initialization, call copy constructor
```

如果不自己定义拷贝构造函数，那么编译器会自动添加一个默认拷贝构造函数(synthesized copy constructor)，它的函数体是，将类的所有可拷贝成员，逐个复制到新的对象当中，即浅拷贝(shallow copy)。

因此，如果一个类的成员变量包含指针类型，默认拷贝构造函数就可以导致隐患的错误——将一个对象的指针简单拷贝到另一个对象，两个指针指向同一个变量，如果一个对象释放了该指针，则另一个对象也将不再能访问该变量。在这种情况下，就有必要手动定义拷贝构造函数，为目标指针分配一个新的存储空间：

```cpp
class HasPtr{
private:
	std::string *ps;
	int          i;
public:
	HasPtr() = default;
	//copy constructor
	HasPtr(const HasPtr& hp): ps(new string(*hp.ps)), i(hp.i){}
}
```

在上面的例子中，就为目标对象的指针重新分配了存储空间，即深拷贝(deep copy)，从而避免了默认拷贝构造函数可能存在的问题。需要注意的是，这里直接访问了另一个对象`hp`的私有成员变量，这是因为同一类的对象互为友元(friend)，因此可以相互访问对象的私有变量。

下面以一个例子，来说明拷贝初始化过程中发生的操作：

```cpp
std::string str = "Study hard tomorrow.";
```

这个例子在前面转换构造函数的部分已经提过了，它会首先调用转换构造函数，将`const char*`类型的`"Study hard tomorrow"`转换为`std::string`类型，继而再调用拷贝构造函数，拷贝初始化`str`。

这里的两次构造函数调用看起来多少有些冗余，实际上，在上面这种情况下，一些编译器可能会省略掉(omit)拷贝构造函数的调用，将上面的拷贝初始化优化为直接初始化：

```cpp
std::string str("Study hard tomorrow.");
```

此时就只会调用一次转换构造函数了。

实际上，除了拷贝初始化以外，拷贝构造函数还在多个场合会被隐式调用：

+ 函数调用的参数为对象的非引用类型(non-reference type)。此时为了将实参转化为形参传入函数内部，编译器会自动调用拷贝构造函数构造一个新的对象（形参）。
+ 函数调用的返回值为对象的非引用类型。此时并不能简单返回函数体内的局部变量，因为函数执行完毕后所有局部变量都将被回收，因此编译器会自动调用拷贝构造函数复制出一个新的对象作为返回结果。
+ 初始化数组等复合类型时，将被拷贝的数组的每个对象逐个复制到新的数组当中，此时对拷贝的每个对象编译器都会自动调用拷贝构造函数。
+ std的容器进行拷贝初始化的时候，对复制的每一个对象都自动调用拷贝构造函数。

由于拷贝构造函数有这么多隐式调用的场合，所以绝对不可以用`explicit`关键字去修饰拷贝构造函数。

现在也可以说明为什么拷贝构造函数的参数必须是同类对象的一个`引用`了。如果拷贝构造函数的参数是非引用类型，在进行拷贝构造时，对于传入的非引用类型的实参（即被拷贝的对象），需要调用拷贝构造函数构造出一个新的形参，这个过程就将不断持续下去，无法终止。

### 拷贝赋值运算符

顾名思义，拷贝赋值运算符在对象被赋值时被调用。和拷贝构造函数一样，如果用户没有定义拷贝赋值运算符，编译器将添加默认的拷贝赋值运算符(synthesized copy assignment operator)，并且该默认的运算符也是进行浅拷贝。

因此，如果类含有类型为指针的成员变量，则往往需要自己定义拷贝复制运算符。这里需要注意的是，拷贝赋值运算符的参数通常也是同类对象的常量引用`const reference type`，尽管不是引用类型也是可以的。在这种情况下，进行赋值操作时，编译器会首先调用拷贝构造函数构造出新的形参，随后再执行拷贝赋值运算符内的操作。

由于c++内置类型的赋值操作，其返回值都是左侧操作数的引用。为了保持与内置类型的一致性，自己定义的拷贝赋值运算符也应该返回左侧操作数的引用。

```cpp
HasPtr& HasPtr::operator=(const HasPtr& hp){
	delete ps;               //free ps first
	ps = new string(*hp.ps);
	i  = hp.i;
	return *this;            //return a reference to left-hand operand
}
```

可以看到，在拷贝赋值运算符中，应该首先释放掉原有指针的空间，防止内存泄露。

### 析构函数以及三者之间的关系

关于析构函数我觉得我还是挺熟悉的，这里就不做详细的说明了。

和上面两个函数一样，如果用户没有自己定义析构函数，编译器将自动为类添加一个默认析构函数(synthesized destructor)，它的函数体是空的，如下：

```cpp
HasPtr{
private:
	...
public:
	//other member functions

	~HasPtr(){}		//default destructor
}
```

但这并不是说默认析构函数什么工作都没有进行。实际上，析构函数体并不直接释放对象的成员变量，对象的成员变量是在执行完析构函数体之后隐式被释放的。因此析构函数体主要进行的应该是指针成员变量的释放，释放动态分配的内存空间。

下面重点讲解拷贝构造函数，拷贝赋值运算符以及析构函数三者的关系(the rule of three)。

通常说来，我们并不需要手动定义全部的三个函数，而是视情况定义部分函数就可以了。这里存在一些经验规则：

> 定义了析构函数的类往往需要同时定义拷贝构造函数和拷贝赋值运算符。

仍然以上面的`HasPtr`类为例，假设它只定义了析构函数：

```cpp
HasPtr{
private:
	string *ps;
	int     i;
public:
	~HasPtr(){
		delete ps;
	}
	//other member functions
}
```

析构函数体内释放`ps`指向的内存空间，随后成员变量`ps`和`i`被隐式回收。考虑一种情况，存在一个外部函数`f`，它的函数体如下所示：

```cpp
HasPtr f(HasPtr hp){
	HasPtr ret = hp;
	return ret;
}
```

如果传入一个`HasPtr`类型的对象，在实参形参转换时会第一次调用拷贝构造函数，浅拷贝出形参`hp`；在`ret = hp`处调用拷贝赋值运算符，浅拷贝出`ret`；函数返回时，会第二次调用拷贝构造函数，随后两个局部变量`hp`和`ret`都会被销毁。需要注意的是，这里所有对象，他们的成员变量`ps`都指向同一个内存区域，因此在销毁过程中，该内存区域会被释放两次，这是一个内存错误。

此外，这样一次调用后，原来的实参指向的`string`将失效，因为该区域已经被释放了。从这个例子可以看出，如果定义了析构函数，往往还需要手动定义拷贝构造函数和拷贝赋值运算符。

> 拷贝构造函数和拷贝赋值运算符往往需要同时被定义，或者同时不被定义。

换言之，如果定义了一个，往往还需要定义另一个。但是定义了拷贝构造函数和拷贝赋值运算符的类并不一定需要定义析构函数。

作为总结，理解这里三个函数的关键，在于把握它们各自的调用时机，即在什么语句中，参数的形式是什么样的时候会被调用，这样就可以避免不少潜在的错误。
