#include "../chp2/Fib.h"
#include "stackApp.h"

/*
* @brief : convert a decimal digit n to any base(2 <= base <= 36)
* @args  :
* @return: return the result in the form of char*
*/
char* convert(__int64 n, int base){
	Stack<char> charStack;
	while(n != 0){
		charStack.push(digits[n % base]);
		n /= base;
	}
	char* res = new char[charStack.getSize() + 1];
	int pos = 0;
	while (!charStack.empty()) res[pos++] = charStack.pop();
	res[pos] = '\0';
	return res;
}

/*
* @brief : to judge if an expression of matches
* @args  : exp
* @return: return true if matches
*/
bool paren(const char exp[]){
	Stack<char> parenStack;
	char currParen, popped;
	for(int ix = 0; (currParen = exp[ix]) != '\0'; ++ix){
		switch(currParen){
		case '(':
		case '[':
		case '{':
			parenStack.push(currParen);
			break;

		case ')':
			if (parenStack.empty()) return false;
			if (parenStack.pop() != '(') return false;
			break;

		case ']':
			if (parenStack.empty()) return false;
			if (parenStack.pop() != '[') return false;
			break;

		case '}':
			if (parenStack.empty()) return false;
			if (parenStack.pop() != '{') return false;
			break;
		}
	}
	return parenStack.empty()? true : false;
}
