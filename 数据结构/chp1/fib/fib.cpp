#include <iostream>
#include <stdio.h>
#include <time.h>
using namespace std;

typedef long long __int64;

__int64 fib_re1(int n);
__int64 fib_re2(int n);
__int64 fib_it(int n);

int main(int argc, char** argv){
    int n = 48;

    printf("iterative version: --------------------------\n");
    for( int ix = 0; ix <= n; ix++){
        printf("#%d: %lld\n", ix, fib_it(ix));
    }

    printf("recursive version two: ----------------------\n");
    for(int ix = 0; ix <= n; ix++){
        printf("#%d: %lld\n", ix, fib_re2(ix));
    }

    printf("recursive version :--------------------------\n");
    for( int ix = 0; ix <= n; ix++){
        printf("#%d: %lld\n", ix, fib_re1(ix));
    }
    return 0;
}

__int64 fib_re1(int n){
    if( n == 0 || n == 1 ) return 1;
    return fib_re1(n - 1) + fib_re1(n - 2);
}

__int64 fib_re2(int n, __int64 &prev){
    __int64 prevPrev;
    if( n == 0){
        prev = 0;
        return 1;
    }
    prev = fib_re2(n - 1, prevPrev);
    return prev + prevPrev;
}

__int64 fib_re2(int n){
    __int64 prev;
    return fib_re2(n, prev);
}

__int64 fib_it(int n){
    __int64 prev = 1, prevPrev = 0;
    __int64 res = 0;
    for(int ix = 0; ix <= n; ++ix){
        res = prevPrev + prev;
        prevPrev = prev;
        prev = res;
    }
    return res;
}

