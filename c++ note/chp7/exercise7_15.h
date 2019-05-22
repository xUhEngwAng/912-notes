#ifndef EXERCISE7_15_H_
#define EXERCISE7_15_H_

#include <string>
#include <iostream>
using std::string;
using std::istream;
using std::ostream;

struct Person;

istream& read(istream &is, Person &person);
ostream& print(ostream &os, Person const &person);

struct Person {
	string name;
	string address;

	//construtors
	Person() = default;
	Person(istream& is) { read(is, *this); }
	Person(string name, string address) : name(name), address(address) {};

	string getName() const { return name; }
	string getAddr() const { return address; }
};

istream& read(istream &is, Person &person) {
	is >> person.name >> person.address;
	return is;
}

ostream& print(ostream &os, Person const &person) {
	os << person.name << ' ' << person.address;
	return os;
}

#endif
