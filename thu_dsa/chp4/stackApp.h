#ifndef STACKAPP_H_
#define STACKAPP_H_

#include "Stack.h"

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

#endif
