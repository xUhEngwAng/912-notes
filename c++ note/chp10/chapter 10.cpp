//Exercise 10.2
#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#define NUM 10
using namespace std;
int main(){
	list<string> strList;
	string tmp;
	for (int ix = 0; ix != NUM; ix++) {
		cin >> tmp;
		strList.push_back(tmp);
	}
	string val;
	cin >> val;
	cout << count(strList.begin(), strList.end(), val) << endl;
	system("pause");
	return 0;
}

//Exercise 10.3
#include <vector>
#include <iostream>
#include <numeric>
using namespace std;
int main(){
	vector<int> intVec{ 5,4,3,1,2 };
	cout << accumulate(intVec.cbegin(), intVec.cend(), 0) << endl;
	system("pause");
	return 0;
}

//Exercise 10.6
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#define NUM 10
using namespace std;
int main(){
	vector<int> intVec;
	fill_n(back_inserter(intVec), 10, 0);
	for (auto It = intVec.cbegin(); It != intVec.cend(); It++)
		cout << *It << ' ';
	cout << endl;
	system("pause");
	return 0;
}

//Exercise 10.9
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
void print(vector<string> strVec);
int main(){
	vector<string> strVec;
	string word;
	while (cin >> word)
		strVec.push_back(word);
	print(strVec);
	sort(strVec.begin(), strVec.end());
	print(strVec);
	auto unique_end = unique(strVec.begin(), strVec.end());
	print(strVec);
	strVec.erase(unique_end, strVec.end());
	print(strVec);

	system("pause");
	return 0;
}

void print(vector<string> strVec){
	if(!strVec.empty()){
		for (auto It = strVec.cbegin(); It != strVec.cend(); It++)
			cout << *It << ' ';
	}
	cout << endl;
}

//Exercise 10.13
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;
void print(vector<string> strVec);
bool have_five_char(string word){
	return word.size() >= 5;
}
int main(){
	vector<string> words;
	string word;
	while (cin >> word)
		words.push_back(word);
	auto end = partition(words.begin(), words.end(), have_five_char);
	print(words);
	words.erase(end, words.end());
	print(words);
	system("pause");
	return 0;
}
void print(vector<string> strVec) {
	if (!strVec.empty()) {
		for (auto It = strVec.cbegin(); It != strVec.cend(); It++)
			cout << *It << ' ';
	}
	cout << endl;
}

//Exercise 10.14
auto func = [](int a, int b){return a+b;};

//Exercise 10.15
int a = 42;
auto func = [a](int b){return a+b;};

//Exercise 10.20
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#define  LENGTH 6
using namespace std;
int main(){
	vector<string> strVec;
	string word;
	while (cin >> word)
		strVec.push_back(word);
	auto count = count_if(strVec.cbegin(), strVec.cend(), [](string str) {return str.size() > LENGTH; });
	cout << count << endl;
	system("pause");
	return 0;
}

//Exercise 10.21
auto func = [arg]() mutable ->bool{
	if(arg == 0) return True;
	//else
	while(arg-- != 0);
	return False;
}