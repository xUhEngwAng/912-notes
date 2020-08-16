#include <iostream>
#include <cassert>
#include <random>
#include <ctime>

#define PARTITION3
#define SCALE          ((int)2048)

using namespace std;

int partition1(int *src, int n, int pivot);
int partition2(int *src, int n, int pivot);
int partition3(int *src, int n, int pivot);
time_t test_partition();
time_t test_worst_case();

int partition(int *src, int n, int pivot){
#ifdef PARTITION1
	return partition1(src, n, pivot);
#else
#ifdef PARTITION2
	return partition2(src, n, pivot);
#else
	return partition3(src, n, pivot);
#endif
#endif
}

int partition1(int *src, int n, int pivot){
	assert(n > 1 && pivot == src[0]);
	int pos = 0, tail = n, head = 0;
	while (1) {
		while (--tail > pos && src[tail] >= pivot);	
		if (tail == pos) break;
		src[pos] = src[tail];
		pos = tail;

		while (++head < pos && src[head] <= pivot);
		if (head == pos) break;
		src[pos] = src[head];
		pos = head;
	}
	src[pos] = pivot;
	return pos;
}

int partition2(int *src, int n, int pivot){
	assert(n > 1 && pivot == src[0]);
	int pos = 0, tail = n, head = 0;
	while (1) {
		while (--tail > pos && src[tail] > pivot);
		if (tail == pos) break;
		src[pos] = src[tail];
		pos = tail;

		while (++head < pos && src[head] < pivot);
		if (head == pos) break;
		src[pos] = src[head];
		pos = head;
	}
	src[pos] = pivot;
	return pos;
}

int partition3(int *src, int n, int pivot){
	assert(n > 1 && pivot == src[0]);
	int mid = 1, current = 0, temp;
	while(++current != n){
		if(src[current] < pivot){
			//swap
			temp = src[current];
			src[current] = src[mid];
			src[mid] = temp;
			++mid;
		}
	}
	src[0] = src[mid - 1];
	src[mid - 1] = pivot;
	return mid - 1;
}

void quickSort(int *src, int n){
	if (n == 0 || n == 1) return;

	//init a pivot
	int randpos = rand() % n;
	int pivot = src[randpos];
	src[randpos] = src[0];
	src[0] = pivot;

	int pos = partition(src, n, pivot);
	quickSort(src, pos);
	quickSort(src + pos + 1, n - pos - 1);
}

int main(){
	srand((int)time(NULL));
	clock_t time;
	cout << "begin tests..." << endl;

	time = test_partition();
	cout << "test partition passed, running time: " << time << endl;

	time = test_worst_case();
	cout << "test_worst_case passed, running time:" << time << endl;

	cout << "all tests passed" << endl;
	system("pause");
	return 0;
}

time_t test_partition(){
	time_t start, end;
	//generate data
	int *src = new int[SCALE];
	for (int ix = 0; ix != SCALE; ++ix)
		src[ix] = rand();

	start = clock();
	quickSort(src, SCALE);
	end = clock();

	for (int ix = 0; ix != SCALE - 1; ++ix) {
		assert(src[ix] <= src[ix + 1]);
	}
	return end - start;
}

//this func may cause stack overflow if SCALE if too BIG
time_t test_worst_case(){
	time_t start, end;
	int *src = new int[SCALE];
	for (int ix = 0; ix != SCALE; ++ix)
		src[ix] = 0;

	start = clock();
	quickSort(src, SCALE);
	end = clock();

	for (int ix = 0; ix != SCALE - 1; ++ix) {
		assert(src[ix] <= src[ix + 1]);
	}
	return end - start;
}
