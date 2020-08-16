#include <string>
#include <vector>
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
using namespace std;

int lcslenIt(string one, string two, int len1, int len2);
int lcslenRe(string one, string two, int len1, int len2);
string lcsIt(string one, string two, int len1, int len2);
vector<string> lcsRe(string one, string two, int len1, int len2);


