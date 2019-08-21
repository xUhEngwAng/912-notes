#include "kmp.h"
#include <iostream>

int* makeNext(char* str){
	int* next;
	int len, i = 0, j = -1;
	for (len = 0; str[len] != '\0'; ++len);
	next = new int[len];
	next[0] = -1;
	while(i < len - 1){
		if(j < 0 || str[i] == str[j]) next[++i] = ++j;
		else						  j = next[j];
	}
	return next;
}

int match(char* text, char* pattern){
	int* next = makeNext(pattern);
	int i = 0, j = 0, m = strlen(text), n = strlen(pattern);
	while(i < m && j < n){
		if(j < 0 || text[i] == pattern[j]){
			++i;
			++j;
		}
		else j = next[j];
	}
	return i - j;
}
