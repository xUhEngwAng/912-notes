#include <list>
#include <deque>

//Exercise 9.2
std::list<std::deque<int> > List;

//Exercise 9.4
using std::vector;
bool find(vector<int>::iterator front, vector<int>::iterator behind, int target){
	for(;front<behind;front++){
		if(*front == target) return TRUE;
	}
	return FALSE;
}

//Exercise 9.5
using std::vector;
vector<int>::iterator find(vector<int>::iterator front, vector<int>::iterator behind, int target){
	for(;front<behind;front++){
		if(*front == target) return front;
	}
	return behind;
}

//Exercise 9.6
while(iter1 < iter2)/* ... */  --> while(iter1 != iter2)/* ... */ 
_list DO NOT support `<` operator_

//Exercise 9.18
#include <iostream>
#include <string>
#include <deque>
using namespace std;
int main(){
	deque<string> strdq;
	string currStr;
	while(cin >> currStr){
		strdq.push_back(currStr);
	}
	deque<string>::iterator dqIt;
	for(dqIt = strdq.begin(); dqIt != strdq.end(); dqIt++){
		cout << *dqIt;
	}
	return 0;
}

//Exercise 9.19
#include <iostream>
#include <string>
#include <list>
using namespace std;
int main() {
	list<string> strdq;
	string currStr;
	while (cin >> currStr) {
		strdq.push_back(currStr);
	}
	list<string>::const_iterator dqIt;
	for (dqIt = strdq.cbegin(); dqIt != strdq.cend(); dqIt++) {
		cout << *dqIt << endl;
	}

	system("pause");
	return 0;
}

//Exercise 9.20
#include <iostream>
#include <string>
#include <list>
#include <deque>
using namespace std;
int main(){
	list<int> iList = {8,9,5,3,1,2,6,4,7,0};
	deque<int> evendq, odddq;
	for(auto listIt = iList.cbegin(); listIt != iList.cend(); listIt++){
		//if odd
		if(*listIt % 2) odddq.push_back(*listIt);
		else            evendq.push_back(*listIt);
	}

	for(auto dqIt = odddq.cbegin(); dqIt != odddq.cend(); cout << *dqIt << ' ', dqIt++);
	cout << endl;
	for(auto dqIt = evendq.cbegin(); dqIt != evnedq.cend(); cout << *dqIt << ' ', dqIt++);
	cout << endl;
	
	return 0;
}

//Exercise 9.24
#include <stdexcept>
#include <vector>
#include <iostream>
using namespace std;
int main(){
	vector<int> emptyVector;
	try {
		//cout << *emptyVector.begin() << endl;
		//cout << emptyVector.front() << endl;
		//cout << emptyVector[0] << endl;
		cout << emptyVector.at(0) << endl;
	}catch(out_of_range err){
		cout << err.what() << endl;
		cout << "Press to exit" << endl;
		system("pause");
	}
	return 0;
}

//Exercise 9.26
#include <vector>
#include <list>
#include <iostream>
using namespace std;
int	main(){
	int ia[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 55, 89 };
	vector<int> ivec;
	list<int>   ilst;
	for(int i = 0; i != 11; i++){
		ivec.push_back(ia[i]);
		ilst.push_back(ia[i]);
	}
	for(auto vIt = ivec.begin(); vIt != ivec.end();){
		if (*vIt % 2 == 0) vIt = ivec.erase(vIt); //even
		else               vIt += 1;
	}
	for(auto lIt = ilst.begin(); lIt != ilst.end();){
		if (*lIt % 2) lIt = ilst.erase(lIt);      // odd
		else          lIt++;
	}
	cout << "result vector:" << endl;
	for (auto It = ivec.begin(); It != ivec.end(); It++)
		cout << *It << ' ';
	cout << "\nresult list" << endl;
	for (auto It = ilst.begin(); It != ilst.end(); It++)
		cout << *It << ' ';
	cout << endl;
	system("pause");
	return 0;
}

//Exercise 9.27
#include <vector>
#include <forward_list>
#include <iostream>
using namespace std;
int	main(){
	forward_list<int>   ilst = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 55, 89 };
	auto lIt = ilst.begin();
	auto prevlIt = ilst.before_begin();
	for(; lIt != ilst.end();){
		if (*lIt % 2) lIt = ilst.erase_after(prevlIt);      // odd
		else{
			lIt++, prevlIt++;
		}
	}
	cout << "\nresult list" << endl;
	for (auto It = ilst.begin(); It != ilst.end(); It++)
		cout << *It << ' ';
	cout << endl;
	system("pause");
	return 0;
}


//Exercise 9.28
#include <forward_list>
#include <iostream>
#include <string>
using namespace std;
forward_list<string> insertStr(forward_list<string> src, string pos, string destStr);
int main(){
	forward_list<string> strLst = { "This", "is", "not", "TRUE", "happiness", "!" };
	auto resultLst = insertStr(strLst, string("is"), string("true"));
	for (auto It = resultLst.cbegin(); It != resultLst.cend(); It++)
		cout << *It << ' ';
	cout << endl;
	system("pause");
	return 0;
}

forward_list<string> insertStr(forward_list<string> src, string pos, string destStr){
	auto It = src.begin();
	auto prev = src.before_begin();
	while(It != src.end()){
		if (*It == pos) {
			src.insert_after(It, destStr);
			break;
		}
		++prev;
		++It;
	}
	if (It == src.end())  src.insert_after(prev, destStr);
	return src;
}

//Exercise 9.43
#include <string>
#include <iostream>
using namespace std;
string strReplace(string s, string oldVal, string newVal);
string::iterator find(string &s, string dest, string::iterator start);
int main(){
	string src("thothothohelloworld"), oldVal("hell"), newVal("world");
	src = strReplace(src, oldVal, newVal);
	cout << src << endl;
	system("pause");
	return 0;
}
string strReplace(string s, string oldVal, string newVal){
	auto pos = s.begin();
	while (pos != s.end()) {
		pos = find(s, oldVal, pos);
		if (pos == s.end()) break;
		pos = s.erase(pos, oldVal.size() + pos);
		pos = s.insert(pos, newVal.begin(), newVal.end());
		pos += newVal.size();
	}
	return s;
}
string::iterator find(string &s, string dest, string::iterator start){
	string::iterator pos = start, local, tmp;
	for(; pos <= s.end() - dest.size(); pos++){
		tmp   = dest.begin();
		local = pos;
		while(*tmp == *local){
			tmp++, local++;
			if (tmp == dest.end()) return pos;
		}
	}
	return s.end();
}


// Exercise 9.44
#include <string>
#include <iostream>
using namespace std;
string strReplace(string s, string oldVal, string newVal);
int main(){
	string src("thothotho"), oldVal("tho"), newVal("though");
	src = strReplace(src, oldVal, newVal);
	cout << src << endl;
	system("pause");
	return 0;
}
string strReplace(string s, string oldVal, string newVal){
	string::size_type pos = 0;
	while (pos != s.size()) {
		pos = s.find(oldVal, pos);
		if (pos == string::npos) break;
		s.replace(pos, oldVal.size(), newVal);
		pos += newVal.size();
	}
	return s;
}

//Exercise 9.45
#include <iostream>
#include <string>
using namespace std;
string completeName(string name, string prefix, string suffix);
int main(){
	string name, prefix, suffix;
	cin >> name >> prefix >> suffix;
	string fullname = completeNam(name, prefix, suffix);
	cout << fullname << endl;
	system("pause");
	return 0;
}
string completeName(string name, string prefix, string suffix){
	name.insert(name.begin(), prefix.begin(), prefix.end());
	name.append(suffix);
	return name;
}

//Exercise 9.46: using index and length, and insert function only
string completeName(string name, string prefix, string suffix){
	name.insert(0, prefix);
	name.insert(name.size(), suffix);
	return name;
}

//Exercise 9.47
#include <iostream>
#include <string>
using namespace std;
int main(){
	string s("ab2c3d7R4E6");
	string numbers("0123456789");
	string::size_type pos = 0;
	while((pos = s.find_first_of(numbers, pos)) != s.npos){
		cout << s[pos] << ' ';
		pos++;
	}
	cout << endl;
	pos = 0;
	while ((pos = s.find_first_not_of(numbers, pos)) != s.npos) {
		cout << s[pos] << ' ';
		pos++;
	}
	cout << endl;
	system("pause");
	return 0;
}

//Exercise 9.49
#include <iostream>
#include <vector>
#include <string>
using namespace std;
int main(){
	string ascender("bdfhjklt"), descender("gjpqy");
	string invalid(ascender.append(descender));
	vector<string> text;
	string word;
	while(cin >> word){
		text.push_back(word);
	}
	string::size_type maxlen = 0;
	vector<string>::const_iterator pos;
	for(auto It = text.cbegin();It != text.cend(); It++){
		if((*It).find_first_of(invalid) == (*It).npos){
			if ((*It).size() > maxlen) {
				maxlen = (*It).size();
				pos = It;
			}
		}
	}
	cout << *pos << endl;
	system("pause");
	return 0;
}

//Exercise 9.50.a
#include <iostream>
#include <string>
#include <vector>
using namespace std;
int main() {
	int N;
	cin >> N;
	vector<string> s(N);
	for (int ix = 0; ix != N; ix++)
		cin >> s[ix];
	int sum = 0;
	for (auto It = s.cbegin(); It != s.cend(); It++) {
		sum += stoi(*It);
	}
	cout << sum << endl;
	system("pause");
	return 0;
}

//Exercise 9.50.b
#include <iostream>
#include <string>
#include <vector>
using namespace std;
int main() {
	int N;
	cin >> N;
	vector<string> s(N);
	for (int ix = 0; ix != N; ix++)
		cin >> s[ix];
	double sum = 0;
	for (auto It = s.cbegin(); It != s.cend(); It++) {
		sum += stod(*It);
	}
	cout << sum << endl;
	system("pause");
	return 0;
}

//Exercise 9.51
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#define NUM_OF_MONTH 12 
using namespace std;
class date{
private:
	unsigned year;
	unsigned month;
	unsigned day;
	const char* months[12]{"January", "February", "March", "April", "May", "June",
						   "July", "August", "September", "October", "November", "December" };
	const char* alphabet{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" };
public:
	date(string strDate);
	date();
	unsigned get_year(){
		return year;
	}
	//get_month();get_day();
	void print_date(int format = 0);
};

//default constructor
date::date(){
	year  = 1998;
	month = 1;
	day   = 1;
}

//constructor taking a string
date::date(string strDate){
	string strMonth, strYear, strDay;
	if (strDate.find(' ') != strDate.npos) {
		strMonth = strDate.substr(0, strDate.find(' '));
		int ix;
		try {
			for (ix = 0; ix != NUM_OF_MONTH; ix++) {
				if (!strMonth.compare(months[ix]) || !strMonth.compare(string(months[ix], 3))) {
					month = ix + 1;
					break;
				}
			}
			if (ix == NUM_OF_MONTH) throw runtime_error("input error: invalid month");
			strYear = strDate.substr(strDate.size() - 4);
			if (strYear.find_first_of(alphabet) != strYear.npos) throw runtime_error("input error: invalid year");
			else year = stoi(strYear);
			strDay = strDate.substr(strDate.find(' ') + 1, strDate.size() - 6);
			if (strDay.find_first_of(alphabet) != strDay.npos) throw runtime_error("input error: invalid day");
			else day = stoi(strDay);
		}
		catch(runtime_error err){
			cout << err.what() << endl;
			cout << "continue or not(y/n): ";
			char c;
			cin >> c;
			if (c == 'n' || c == 'N') exit(0);
		}
	}else{//format 1/1/1998
		try {
			strMonth = strDate.substr(0, strDate.find('/'));
			if (strMonth.find_first_of(alphabet) != strMonth.npos) throw runtime_error("input error: invalid month");
			else month = stoi(strMonth);
			strYear = strDate.substr(strDate.size() - 4);
			if (strYear.find_first_of(alphabet) != strYear.npos) throw runtime_error("input error: invalid year");
			else year = stoi(strYear);
			strDay = strDate.substr(strDate.find('/') + 1, strDate.size() - 5);
			if (strDay.find_first_of(alphabet) != strDay.npos) throw runtime_error("input error: invalid day");
			else day = stoi(strDay);
		}
		catch(runtime_error err){
			cout << err.what() << endl;
			cout << "continue or not(y/n): ";
			char c;
			cin >> c;
			if (c == 'n' || c == 'N') exit(0);
		}
	}
}

//print current date with various formats
void date::print_date(int format){
	switch(format){
	default:
	case 0:
		cout << months[month-1] << ' ' << day << ", " << year << endl;;
		break;
	case 1:
		cout << string(months[month-1], 3) << ' ' << day << ", " << year << endl;
		break;
	case 2:
		cout << month << '/' << day << '/' << year << endl;
		break;
	}
}

//Exercise 9.52
#include <iostream>
#include <string>
#include <stack>
using namespace std;
int main(){
	string expr{ "This is (Mooophy(awesome)((((wooooooooo))))) and (ocxs) over" };
	const char repl = '#';
	stack<char> charStack;
	int seen = 0;
	for(auto It = expr.cbegin(); It != expr.cend(); It++){
		charStack.push(*It);
		if (*It == '(') seen += 1;
		if(*It == ')' && seen > 0){
			seen -= 1;
			while (charStack.top() != '(') charStack.pop();
			charStack.pop();
			charStack.push(repl);
		}
	}
	//output the result
	string output;
	for (; !charStack.empty(); charStack.pop())
		output.insert(output.begin(), charStack.top());
	cout << output << endl;
	system("pause");
	return 0;
}
