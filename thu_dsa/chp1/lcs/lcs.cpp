#include "lcs.h"

int lcsRe(string one, string two){
	int len;
	if(one.empty() || two.empty()) return 0;

	int len1 = one.size(), len2 = two.size();
	if(one[len1 - 1] == two[len2 - 1]){
		len = lcsRe(one.substr(0, len1 - 1), two.substr(0, len2 - 1)) + 1;
		return len;
	}
	len = MAX(lcsRe(one.substr(0, len1 - 1), two), lcsRe(one, two.substr(0, len2 - 1)));
	return len;
}

int lcsIt(string one, string two){
	if(one.empty() || two.empty()) return 0;
	
	int len1 = one.size(), len2 = two.size();
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

