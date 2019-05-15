Conclusion on Master Theorem
============================

## What is _Master Theorem_

In the analysis of algorithms, `Master Theorem` provides a simple way to compute the time complexity(using big O notation) for `divide-and-conquer` recurrences.

We all know that for `divide-and-conquer` algorithms, there are two way to analysis their time complexity, i.e. _recursion track_ and _recurrence equation_. Master Theorem is a way to compute such time complexity using both the two way: it tries to solve the _recurrence equation_ by means of _recusion track_.

## Introduction

First we'll make some abstractions. Lets' consider an algorithm implemented in the form of a recursion. Generally, we can assume that to solve a problem of scale `n`, we can divide it into `a` subproblems, whose scale would be `n/b`, with $f(n)$ being the time to create the subproblems and combine their results in the above procedure.

The runtime of subck an algorithm on an input of size 'n', usually denoted $T(n)$, can be expressed by the recurrence relation

$$
T(n) = aT(\frac{n}{b}) + f(n)
$$

## Generic solution

First we'll assume $f(n)$ being in the form of $f(n) = n^d$. Generally, this is indeed the most common form of $f(n)$.In this way, the equation should look like this:

$$
T(n) = aT(\frac{n}{b}) + n^d
$$

We'll try to solve this equation. And the essence of this is actually _recursion track_.

In the method of _recursion track_, the computation of each layer of recursion can form a computation tree, with the sum of time cost in each layer being the total time cost of that layer.

+ For the 0th(highest) layer, the time cost is $n^d$.
+ For the 1st layer, the time cost is $a * (\frac{n}{b})^d = n^d * \frac{a}{b^d}$
+ For the 2nd layer, the time cost is $a^2 * (\frac{n}{b^2})^d = n^d * (\frac{a}{b^d})^2$
+ For the 3rd layer, the time cost is $a^3 * (\frac{n}{b^3})^d = n^d * (\frac{a}{b^d})^3$
+ ...
+ For the kth(the last) layer, the time cost is $a^k * (\frac{n}{b^2})^d = n^d * (\frac{a}{b^d})^k$, where $k = log_{b}^{n}$

This way, we can compute T(n):

$$
T(n) = n^d + n^d * \frac{a}{b^d} + n^d * (\frac{a}{b^d})^2 + ... + n^d * (\frac{a}{b^d})^k
$$

Then simplify T(n) so that T(n) can be:

$$
T(n) = n^d(1 + \frac{a}{b^d} + (\frac{a}{b^d})^2 + ... + (\frac{a}{b^d})^k)
$$

And this is a _geometric progression_, so let's assume $q = \frac{a}{b^d}$, then T(n) can be:

$$
T(n) = n^d(1 + q + q^2 + ... + q^k)
$$

## Conlusion

> Case 1: q = 1

$T(n) = k * n^d = log_{b}^{n} * n^d = O(logn · n^d) $

> Case 2: q < 1

Then $T(n)$ is a convergent geometric progression. So $T(n)$ can be expressed as:

$$
T(n) = O(n^d)
$$

> Case 3: q > 1

$$
T(n) = n^d(1 + q + q^2 + ... + q^k) = n^d * \frac{1-q^{k+1}}{1-q} = O(n^d * q^k = O(a^{log_{b}^{n}})
$$

To further simplify this, we need to apply the following equation:

$$
a^{log_{b}^{n}} = n^{log_{b}^{a}}
$$

So $T(n) = O(n^{log_{b}^{a}})$


