#include "max2.h"

inline void swap(int &one, int&two){
    int tmp = one;
    one = two;
    two = tmp;
}

/*
 * compute max two elements in A[lo, hi)
 * iterative versions
 */
void max2It(int* A, int lo, int hi, int &one, int &two){
    if(hi - lo < 2) return;
    one = MAX(A[lo], A[lo + 1]);
    two = MIN(A[lo], A[lo + 1]);
    for(int ix = lo + 2; ix != hi; ++ix){
        if(A[ix] > two){
            two = A[ix];
            if(two > one) swap(one, two);
        }
    }
}

void max2It2(int* A, int lo, int hi, int &one, int &two){
	if(hi - lo < 2) return;

	int maxIndex = lo;
	one = A[lo];
	for(int ix = lo + 1; ix != hi; ++ix)
		if(A[ix] > one) {one = A[ix];maxIndex = ix;}
	
	two = 0;
	for(int ix = lo; ix != maxIndex; ++ix)
		if(A[ix] > two) two = A[ix];
	
	int tmp = 0;
	for(int ix = maxIndex + 1; ix != hi; ++ix)
		if(A[ix] > tmp) tmp = A[ix];
	
	two = MAX(two, tmp);
}

void max2Re(int* A, int lo, int hi, int &one, int &two){
    if(hi - lo == 2){
		if(A[lo] > A[lo + 1]){one = A[lo], two = A[lo + 1];return;}	
		else {one = A[lo + 1], two = A[lo]; return;}
	}
    if(hi - lo == 3){
        one = A[lo], two = A[lo + 1];
		if(one < two) swap(one, two);
		if(A[lo + 2] > two){
			two = A[lo + 2];
			if(one < two) swap(one, two);
		}
        return;
    }
    int mid = (lo + hi) >> 1;
    int oneL, twoL, oneR, twoR;
    max2Re(A, lo, mid, oneL, twoL);
    max2Re(A, mid, hi, oneR, twoR);
    if(oneL > oneR){
		one = oneL;
		two = MAX(oneR, twoL);
	}
	else{
		one = oneR;
		two = MAX(oneL, twoR);
	}
}

