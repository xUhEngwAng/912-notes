#include "../chp2/Vector.h"
#include <iostream>
#include <cassert>
#include <random>
#include <ctime>

#define SEDGEWICK
#define SCALE     65536
#define LOOP      16

using namespace std;

Vector<int> shellSeq;
time_t test_shellsort();

void init_shell_seq(int n){
#ifdef INSERTSORT
	shellSeq.push_back(1);
#else
#ifdef SHELLSEQ
	shellSeq.push_back(1);
	int ix = 0;
	while ((shellSeq[ix] << 1) < n) shellSeq.push_back(shellSeq[ix++] << 1);
#else
#ifdef PAPERNOV
	int ix = 0, curr = 2;
	while (curr - 1 < n) {
		shellSeq.push_back(curr - 1);
		curr = curr << 1;
	}
#else //SEDGEWICK
	int power2i = 1, power4i = 1;                //stand for 2^k and 4^k
	int power2j = 4, power4j = 16;
	int seqi = 1, seqj = 5;
	while(seqi < n || seqj < n){
		if (seqj < seqi) {
			shellSeq.push_back(seqj);
			power2j = power2j << 1;
			power4j = power4j << 2;
			seqj = power4j - 3 * power2j + 1;
		}
		else if (seqi < seqj) {
			shellSeq.push_back(seqi);
			power2i = power2i << 1;
			power4i = power4i << 2;
			seqi = 9 * power4i - 9 * power2i + 1;
		}
		else{
			shellSeq.push_back(seqi);
			power2i = power2i << 1;
			power4i = power4i << 2;
			seqi = 9 * power4i - 9 * power2i + 1;
			power2j = power2j << 1;
			power4j = power4j << 2;
			seqj = power4j - 3 * power2j + 1;
		}
	}
#endif
#endif
#endif
}

void shellSort(int* src, int n){
	//init_shell_seq(n);
	int step, pos, curr;
	for(int ix = shellSeq.getSize() - 1; ix >= 0; --ix){
		step = shellSeq[ix];
		for(int jx = step; jx != n; ++jx){
			pos = jx - step;
			curr = src[jx];
			while (pos >= 0 && curr < src[pos]){
				//swap src[jx] and src[pos]
				src[pos + step] = src[pos];
				pos -= step;
			}
			src[pos + step] = curr;
		}
	}
}

int main(){
	srand((int)time(NULL));
	time_t time = 0;
	cout << "begin tests ..." << endl;

	for(int ix = 0; ix < LOOP; ++ix)
		time += test_shellsort();
	cout << "test_shellsort passed, running time: " << time / LOOP << endl;

	cout << "all tests passed." << endl;
	system("pause");
	return 0;
}

time_t test_shellsort(){
	int *src = new int[SCALE];
	time_t begin, end;
	for (int ix = 0; ix != SCALE; ++ix)
		src[ix] = rand();
	
	init_shell_seq(SCALE);
	begin = clock();
	shellSort(src, SCALE);
	end = clock();

	for (int ix = 1; ix != SCALE; ++ix)
		assert(src[ix - 1] <= src[ix]);

	return end - begin;
}
