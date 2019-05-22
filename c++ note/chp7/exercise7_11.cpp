//test constructors
#include "exercise7_11.h"
#include <cassert>

using std::cin;
using std::cout;
using std::cerr;

void test_constructor();

int main(){
	test_constructor();

	system("pause");
	return 0;
}

void test_constructor(){
	//test default constructor
	Sales_data data1;
	assert(data1.units_sold == 0);
	assert(data1.revenue == 0.0);

	//test constructor2
	Sales_data data2(cin); // cin >> 123456789 2 20.5
	assert(data2.bookNo == "123456789");
	assert(data2.revenue == 41.0);
	assert(data2.units_sold == 2);

	//test constructor3
	Sales_data data3("123456789");
	assert(data3.bookNo == "123456789");
	assert(data3.revenue == 0.0);
	assert(data3.units_sold == 0);

	//test constructor4
	Sales_data data4("123456789", 2, 20.5);
	assert(data4.bookNo == "123456789");
	assert(data4.revenue == 41.0);
	assert(data4.units_sold == 2);

	cout << "All tests passed." << endl;
}
