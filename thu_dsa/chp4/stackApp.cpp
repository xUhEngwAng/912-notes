#include "../chp2/Fib.h"
#include "stackApp.h"
#include <stdlib.h>
#include <string>

/*-----------build-in functions----------*/
int readNumber(char* &expr);
int fac(int n);
inline char orderBetween(char optr1, char optr2);
int findOptr(char optr);
double cal(char optr, double opnd);
double cal(double opnd1, char optr, double opnd2);
void append(char* &des, char c);

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
	char currParen;
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

/*
 * @brief : to compute the value of an infix expression
 * @args  : infixExpr
 * @return: return the value in double
 * @others: only consider valid infix expression
 */
double evaluate(char* infixExpr){
	Stack<double> opnd;
	Stack<char>   optr;
	double opnd1, opnd2;
	optr.push('\0');
	while(!optr.empty()){
		if (isdigit(*infixExpr)) opnd.push(readNumber(infixExpr));
		else{
			if (*infixExpr == ' ' || *infixExpr == '\t') { // skip white spaces
				++infixExpr;
				continue;
			}

			switch(orderBetween(optr.top(), *infixExpr)){
			case '>':
				if (optr.top() == '!') opnd.push(cal(optr.pop(), opnd.pop()));
				else{
					opnd2 = opnd.pop();
					opnd1 = opnd.pop();
					opnd.push(cal(opnd1, optr.pop(), opnd2));
				}
				break;

			case '<':
				optr.push(*infixExpr++);
				break;

			case '=':
				infixExpr++;
				optr.pop();
				break;
			}
		}
	}
	return opnd.pop();
}

/*
 * @brief : to convert an infix expression to a postfix one
 * @args  : infixExpr
 * @return: return the converted postfix expression
 * @others: only consider valid infix expression
 */
char* toPostfix(char* infixExpr){
	char* postfixExpr = "";
	Stack<char> optr;
	optr.push('\0');
	while(!optr.empty()){
		if (isdigit(*infixExpr)) append(postfixExpr, *infixExpr++);
		else{
			if (*infixExpr == ' ' || *infixExpr == '\t') { // skip white spaces
				++infixExpr;
				continue;
			}
			switch(orderBetween(optr.top(), *infixExpr)){
			case '>':
				append(postfixExpr, optr.pop());
				break;

			case '<':
				optr.push(*infixExpr++);
				break;

			case '=':
				optr.pop();
				++infixExpr;
				break;
			}
		}
	}
	return postfixExpr;
}

/*-----------build-in functions----------*/
int readNumber(char* &expr){
	int res = 0;
	while(isdigit(*expr)){
		res = res * 10 + *expr - '0';
		expr++;
	}
	return res;
}

inline char orderBetween(char optr1, char optr2){
	return pri[findOptr(optr1)][findOptr(optr2)];
}

int findOptr(char optr){
	if (optr == '+')  return 0;
	if (optr == '-')  return 1;
	if (optr == '*')  return 2;
	if (optr == '/')  return 3;
	if (optr == '^')  return 4;
	if (optr == '!')  return 5;
	if (optr == '(')  return 6;
	if (optr == ')')  return 7;
	if (optr == '\0') return 8;
}

int fac(int n){
	int res = 1;
	for (; n != 0; res *= n, n--);
	return res;
}

double cal(char optr, double opnd){
	double res = 0.0;
	switch(optr){
	case '!':
		res = fac(opnd);
		break;
	default:
		break;
	}
	return res;
}

double cal(double opnd1, char optr, double opnd2) {
	double res = 0.0;
	switch (optr){
	case '+':
		res = opnd1 + opnd2;
		break;

	case '-':
		res = opnd1 - opnd2;
		break;

	case '*':
		res = opnd1 * opnd2;
		break;

	case '/':
		res = opnd1 / opnd2;
		break;

	case '^':
		res = pow(opnd1, opnd2);
		break;

	default:
		break;
	}
	return res;
}

/*
 * @brief : to append a char to an array of char, even if the array is already full
 * @args  : char* des, char c
 * @return: void
 * @others: the array can automatically expand itself when needed
 */
void append(char* &des, char c){
	int n = strlen(des);
	char* tmp = des;
	des = new char[n + 2];
	sprintf(des, "%s%c\0", tmp, c);
}
