#ifndef EXERCISE7_4_H_
#define EXERCISE7_4_H_

#include <string>
#include <iostream>
using std::string;
using std::istream;
using std::ostream;

struct Person{
	string name;
	string address;
	string getName() const { return name; }
	string getAddr() const { return address; }
};

istream& read(istream &is, Person &person){
	is >> person.name >> person.address;
	return is;
}

ostream& print(ostream &os, Person const &person){
	os << person.name << ' ' << person.address;
	return os;
}

#endif
