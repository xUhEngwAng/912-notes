#include "lcs.h"
#include <algorithm>

int lcslenRe(string one, string two, int len1, int len2){
	int len;
	if(len1 == 0 || len2 == 0) return 0;

	if(one[len1 - 1] == two[len2 - 1]){
		len = lcslenRe(one.substr(0, len1 - 1), two.substr(0, len2 - 1), len1 - 1, len2 - 1) + 1;
		return len;
	}
	int lenone = lcslenRe(one.substr(0, len1 - 1), two, len1 - 1, len2);
	int lentwo = lcslenRe(one, two.substr(0, len2 - 1), len1, len2 - 1);
	return lenone > lentwo ? lenone : lentwo;
}

int lcslenIt(string one, string two, int len1, int len2){ 
	if(one.empty() || two.empty()) return 0;
	
	vector<vector<int>> lens(len1 + 1, vector<int>(len2 + 1, 0));
	for(int ix = 0; ix != len1; ++ix){
		for(int jx = 0; jx != len2; ++jx){
			if(one[ix] == two[jx])
				lens[ix + 1][jx + 1] = lens[ix][jx] + 1;
			else
				lens[ix+1][jx+1] = MAX(lens[ix][jx+1], lens[ix+1][jx]);
		}
	}
	return lens[len1][len2];
}

vector<string> lcsRe(string one, string two, int len1, int len2){
	vector<string> lcs;
	if (len1 == 0 || len2 == 0) return lcs;

	vector<string> lcs1, lcs2;
	if(one[len1 - 1] == two[len2 - 1]){
		lcs = lcsRe(one.substr(0, len1 - 1), two.substr(0, len2 - 1), len1 - 1, len2 - 1);
		if (lcs.empty()) lcs.push_back(one.substr(len1 - 1, 1));
		else
			for (auto It = lcs.begin(); It != lcs.end(); ++It)
				It->append(1, one[len1 - 1]);
		return lcs;
	}
	lcs1 = lcsRe(one.substr(0, len1 - 1), two, len1 - 1, len2);
	lcs2 = lcsRe(one, two.substr(0, len2 - 1), len1, len2 - 1);
	if (lcs1.empty()) lcs = lcs2;
	else if (lcs2.empty()) lcs = lcs1;
	else{
		if (lcs1[0].length() < lcs2[0].length()) lcs = lcs2;
		else if (lcs1[0].length() > lcs2[0].length()) lcs = lcs1;
		else {
			lcs = lcs1;
			for (string entry : lcs2)
				if(find(lcs.cbegin(), lcs.cend(), entry) == lcs.cend())
					lcs.push_back(entry);
		}
	}
	return lcs;
}

enum direction { LEFT, UPPER, DIAGON, UNINIT };

class State{
public:
	int len;
	direction dir;
	State(){
		len = 0;
		dir = UNINIT;
	}
};

string lcsIt(string one, string two, int len1, int len2){
	string lcs;
	if (len1 == 0 || len2 == 0) return lcs;

	vector<vector<State>> states(len1 + 1, vector<State>(len2 + 1));
	for(int i = 0; i != len1; ++i){
		for(int j = 0; j != len2; ++j){
			if(one[i] == two[j]){
				states[i + 1][j + 1].len = states[i][j].len + 1;
				states[i + 1][j + 1].dir = DIAGON;
			}
			else{
				if(states[i][j + 1].len < states[i + 1][j].len){
					states[i + 1][j + 1].len = states[i + 1][j].len;
					states[i + 1][j + 1].dir = LEFT;
				}else{
					states[i + 1][j + 1].len = states[i][j + 1].len;
					states[i + 1][j + 1].dir = UPPER;
				}
			}
		}
	}

	lcs.resize(states[len1][len2].len);
	int pos = lcs.size();
	for(int i = len1, j = len2; i > 0 && j > 0; ){
		switch(states[i][j].dir){
		case DIAGON:
			lcs[--pos] = one[i - 1];
			--i, --j;
			break;
		case UPPER:
			--i;
			break;
		case LEFT:
			--j;
			break;
		default:
			exit(-1);
		}
	}
	return lcs;
}
