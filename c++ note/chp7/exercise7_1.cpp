#include <iostream>
#include <string>
using namespace std;

struct Sales_data{
	string bookNo;
	unsigned units_sold = 0;
	double revenue = 0.0;

	string isbn() const { return bookNo; }
	void combine(Sales_data const &data){
		units_sold += data.units_sold;
		revenue    += data.revenue;
	}
};

Sales_data add(Sales_data const &one, Sales_data const &two){
	Sales_data res;
	res.bookNo     = one.bookNo;
	res.units_sold = one.units_sold + two.units_sold;
	res.revenue    = one.revenue + two.revenue;
	return res;
}

istream& read(istream &is, Sales_data &data){
	is >> data.bookNo;
	is >> data.units_sold;
	is >> data.revenue;
	return is;
}

ostream& print(ostream &os, Sales_data const &data){
	os << "book number: "   << data.bookNo << endl;
	os << "units sold: "    << data.units_sold << endl;
	os << "total revenue: " << data.revenue << endl;
	return os;
}

int main(){
	Sales_data total;							// variable to hold the running sum
	if (read(cin, total)) {						// read the first transaction
		Sales_data trans;						// variable to hold data for the next transaction
		while (read(cin, trans)) {				// read the remaining transactions
			if (total.isbn() == trans.isbn())	// check the isbns
				total.combine(trans);			// update the running total
			else {
				print(cout, total) << endl;		// print the results
				total = trans;					// process the next book
			}
		}
		print(cout, total) << endl;				// print the last transaction
	}
	else {										// there was no input
		cerr << "No data?!" << endl;			// notify the user
	}

	system("pause");
	return 0;
}

