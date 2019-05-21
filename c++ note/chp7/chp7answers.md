Solutions to Exercises in Chapter Seven
=======================================

> Exercise 7.1: Write a version of the transaction-processing program from §1.6 (p. 24) using the Sales_data class you defined for the exercises in §2.6.1 (p. 72).

[exercise7_1.cpp](exercise7_1.cpp)

> Exercise 7.2: Add the combine and isbn members to the Sales_data class you wrote for the exercises in § 2.6.2 (p. 76).

[exercise7_2.h](exercise7_2.h)

> Exercise 7.3: Revise your transaction-processing program from § 7.1.1 (p.256) to use these members.

[exercsie7_3.cpp](exercise7_3.cpp)

> Exercise 7.4: Write a class named Person that represents the name and address of a person. Use a string to hold each of these elements. Subsequent exercises will incrementally add features to this class.

```cpp
#ifndef EXERCISE7_4_H_
#define EXERCISE7_4_H_

#include <string>
using std::string;

struct Person{
	string name;
	string address;
};

#endif
```

> Exercise 7.5: Provide operations in your Person class to return the name and address. Should these functions be const? Explain your choice.

```cpp
#ifndef EXERCISE7_4_H_
#define EXERCISE7_4_H_

#include <string>
using std::string;

struct Person{
	string name;
	string address;
	string getName() const { return name; }
	string getAddr() const { return address; }
};

#endif
```

These two functions should be `const`. Because these two functions do not modify the object's members, and `const` objects of this class should be able to call these two functions.

> Exercise 7.6: Define your own versions of the add, read, and print functions.

It seems I have done this back in Exercise 7.1.

> Exercise 7.7: Rewrite the transaction-processing program you wrote for the exercises in § 7.1.2 (p. 260) to use these new functions.

It seems I have done this back in Exercise 7.1.

> Exercise 7.8: Why does `read` define its Sales_data parameter as a plain reference and `print` define its parameter as a reference to const?

Because in `read`, the `Sales_data` object will be modified. While in `print`, this function do not modify the object. 

> Exercise 7.9: Add operations to `read` and `print` Person objects to the code you wrote for the exercises in § 7.1.2 (p. 260).

```
istream& read(istream &is, Person &person){
	is >> person.name >> person.address;
	return is;
}

ostream& print(ostream &os, Person const &person){
	os << person.name << ' ' << person.address;
	return os;
}
```

> Exercise 7.10: What does the condition in the following if statement do? 
`if (read(read(cin, data1), data2))`

Take in two objects(data1 and data2) at the same time, and step into the `if` block only when both objects are valid.
