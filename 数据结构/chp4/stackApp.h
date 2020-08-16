#ifndef STACKAPP_H_
#define STACKAPP_H_

#include "Stack.h"

#define NOPTR 9

static char digits[] = { '0','1','2','3','4','5','6','7','8','9',
                         'a','b','c','d','e','f','g','h','i','j',
                         'k','l','m','n','o','p','q','r','s','t',
                         'u','v','w','x','y','z' };
/*
 * @brief : convert a decimal digit n to any base(2 <= base <= 36)
 * @args  : 
 * @return: return the result in the form of char*
 */
char* convert(__int64 n, int base);

/*
 * @brief : to judge if an expression of parens matches
 * @args  : exp
 * @return: return true if matches
 */
bool paren(const char exp[]);

//pri[stack top optr][curr optr]
const char pri[NOPTR][NOPTR] = {
	/*          +    -    *    /    ^    !    (    )    \0 */
	/*  +  */  '>', '>', '<', '<', '<', '<', '<', '>', '>',
	/*  -  */  '>', '>', '<', '<', '<', '<', '<', '>', '>',
	/*  *  */  '>', '>', '>', '>', '<', '<', '<', '>', '>',
	/*  /  */  '>', '>', '>', '>', '<', '<', '<', '>', '>',
	/*  ^  */  '>', '>', '>', '>', '>', '<', '<', '>', '>',
	/*  !  */  '>', '>', '>', '>', '>', '>', '<', '>', '>',
	/*  (  */  '<', '<', '<', '<', '<', '<', '<', '=', ' ',
	/*  )  */  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	/*  \0 */  '<' ,'<', '<', '<', '<', '<', '<', ' ', '=' };

/*
 * @brief : to compute the value of an infix expression
 * @args  : infixExpr
 * @return: return the value in double
 * @others: only consider valid infix expression 
 */
double evaluate(char* infixExpr);

/*
 * @brief : to convert an infix expression to a postfix one
 * @args  : infixExpr
 * @return: return the converted postfix expression
 * @others: only consider valid infix expression
 */
char* toPostfix(char* infixExpr);

/*
 * @brief : to generate a very long expression in the pattern 1+0*1^(1+0*1^(1+0*1^(......)
 * @args  : number of brackets in the expression
 * @return: the long expression
 */
char* generateLongExpr(int n);

#endif
