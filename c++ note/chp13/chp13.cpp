#include <iostream>
#include <list>
#include <string>
#include <forward_list>
#include <vector>
#include <array>

using namespace std;

class HasPtr {
public:
	explicit HasPtr(const std::string &s = std::string()) :
		ps(new std::string(s)), i(0) {}

	//copy constructor
	HasPtr(const HasPtr& ptr);

	//copy assignment operator
	HasPtr& operator=(const HasPtr& ptr);

	//destructor
	~HasPtr(){
		delete ps;
	}
private:
	std::string *ps;
	int          i;
};


HasPtr::HasPtr(const HasPtr& ptr){
	ps = new string(*ptr.ps);
	i = ptr.i;
}

HasPtr& HasPtr::operator=(const HasPtr& ptr){
	delete ps;
	ps = new string(*ptr.ps);
	i  = ptr.i;
	return *this;
}

HasPtr f(HasPtr fp){
	HasPtr ret = fp;
	return ret;
}

int main(){
	HasPtr ptr1("2020");
	HasPtr ptr2("2019");
	HasPtr ptr3 = ptr2;
	forward_list<int> li({ 4, 5, 6 });
	array<int, 5> arr = { 1 };
	arr = { 1, 2, 3, 4};
	arr.assign(1);
	arr.fill(4);
	for (auto e : arr)
		cout << e << " ";
	cout << endl;

	for (int e : li)
		cout << e << " ";
	ptr2 = ptr1;
	ptr3 = ptr1;
	f(ptr1);
	system("pause");
	return 0;
}
