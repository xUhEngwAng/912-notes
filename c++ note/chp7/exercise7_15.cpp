// test construtors
#include "exercise7_15.h"
#include <cassert>

using std::cout;
using std::cin;
using std::endl;

int main(){
	//test constructorss
	Person person1;
	// here we see non-builtin type(such as string) would not have undefined value
	assert(person1.getAddr() == string()); 
	assert(person1.getName() == string());

	Person person2("shine", "shanghai");
	// and string can directly be compare with literal character string
	assert(person2.getName() == "shine");
	assert(person2.getAddr() == string("shanghai"));

	Person person3(cin); // cin >> shine >> shanghai;
	assert(person2.getName() == "shine");
	assert(person2.getAddr() == string("shanghai"));

	cout << "All tests passed." << endl;
	system("pause");
	return 0;
}
