#ifndef EXERCISE7_2_H_
#define EXERCISE7_2_H_

#include <iostream>
#include <string>
using namespace std;

struct Sales_data {
	string bookNo;
	unsigned units_sold = 0;
	double revenue = 0.0;

	string isbn() const { return bookNo; }
	double avg_price() const { return revenue / units_sold; }
	Sales_data& combine(Sales_data const &data);		//behave same as the `+=` operator, thus return a left-value
};

Sales_data& Sales_data::combine(Sales_data const &data){
	units_sold += data.units_sold;
	revenue    += data.revenue;
	return *this;
}

Sales_data add(Sales_data const &one, Sales_data const &two) {
	Sales_data res = one;
	return res.combine(two);
}

istream& read(istream &is, Sales_data &data) {
	is >> data.bookNo;
	is >> data.units_sold;
	is >> data.revenue;
	return is;
}

ostream& print(ostream &os, Sales_data const data) {
	os << "book number: " << data.bookNo << endl;
	os << "units sold: " << data.units_sold << endl;
	os << "total revenue: " << data.revenue << endl; 
	os << "averge price: " << data.avg_price();    // no endl at the end
	return os;
}

#endif
