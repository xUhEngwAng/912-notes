哲学家就餐问题及其实现
====================

## 哲学家就餐问题描述

哲学家就餐问题是指，有五个哲学家围坐一桌，每两个哲学家之间都有一只叉子，一共有五只叉子。每个哲学家都只有两个动作，即思考和就餐，哲学家思考的时候不需要任何的资源，但只有同时拿起他左右的两只叉子，才能开始进餐。进餐完毕后将叉子放归原位。这个问题在于，应该如何保证哲学家们的动作有序进行，如不会出现有人永远拿不到叉子的情况。

## 一些初步的尝试

### 第一次尝试

对哲学家就餐问题进行分析，可以发现，相邻的两个哲学家对他们中间的叉子应该是互斥访问的。为此，为每个叉子设置一个信号量，每个哲学家在进餐之间，首先需要获得其左右两个叉子，即分别对其左右的两只叉子做一次`P`操作；进餐完毕后，将两只叉子放归原位，即分别对其左右两只叉子做一次`V`操作。实现的伪代码如下：

```c
Semaphore forks[5];
for(fork : forks)
	fork.sem = 1;				//initialize to 1

#define LEFT  (i - 1 + 5) % 5
#define RIGHT (i + 1) % 5

//for philosopher i
void philosopher(){
	while(true){
		thinking();
		forks[LEFT].P();
		forks[RIGHT].V();
		eating();
		forks[LEFT].V();
		forks[RIGHT].V();
	}
}
```

容易看出，这种实现方法是有问题的，倘若所有五个哲学家同时想要进餐，有一种情况是他们都分别拿起了他们左边的叉子，这样所有哲学家想要获得右边的叉子时都会失败而进入阻塞状态，并且这种阻塞将一直进行下去，因为没有哲学家会主动释放已经获得的叉子，即出现了死锁。

### 第二次尝试

为了对上面的尝试做出改进，可以注意到，在任意时刻只可能有两个哲学家在同时进餐，因此可以做一些额外的限制，使得只能有两个哲学家同时请求获得叉子，为此可以再设置一个初值为2的信号量`mutex`，实现的伪代码如下：

```c
Semaphore forks[5];
for(fork : forks)
	fork.sem = 1;
Semaphore mutex(2);

//for philosopher i
void philosopher(){
	thinking();
	//I'm hungry
	mutex.P();
	forks[LEFT].P();
	forks[RIGHT].P();
	eating();
	forks[LEFT].V();
	forks[RIGHT].V();
	mutex.V();
}
```

由于任意时刻只能有两个哲学家在请求叉子，因此至少会有一个哲学家可以同时获得他左右的两只叉子然后开始进餐，第一次尝试中死锁的情况在这里不会再发生。但是如果两个请求叉子的哲学家是相邻的，他们中必有一个会进入阻塞状态，此时另外两只叉子还是空闲的，却不能有哲学家进入临界区获得叉子了，也就是说这种策略会导致资源的浪费。

### 第三次尝试

再次对第一次尝试进行分析，可以发现，第一次尝试之所以会失败，是因为所有哲学家都请求了同一侧的叉子，导致出现了循环等待的情况。为了解决这个问题，可以让不同的哲学家请求不同侧的叉子，比如奇数号的哲学家优先请求左侧叉子，而偶数号的哲学家优先请求右侧叉子。这种策略的伪代码如下：

```c
Semaphore forks[5];
for(fork : forks)
	fork.sem = 1;				//initialize to 1

#define LEFT  (i - 1 + 5) % 5
#define RIGHT (i + 1) % 5

//for philosopher i
void philosopher(){
	while(true){
		thinking();
		if(i % 2 == 1){
			forks[LEFT].P();
			forks[RIGHT].V();
		}else{
			forks[RIGHT].P();
			forks[LEFT].P();
		}
		eating();
		forks[LEFT].V();
		forks[RIGHT].V();
	}
}
```

在这种策略下就可以有效避免死锁了，并且还可以实现多人同时就餐，不会有第二次尝试中资源的浪费。

## 利用`AND`信号量实现

`AND`信号量我感觉更多是一种思想吧，即在对进程请求的多个资源进行分配时，首先检查这些资源是否都是空闲的，如果的确都是空闲的，则将资源全部分配给该进程，否则一个资源也不分配。很明显，这里检查资源是否空闲过程应该是原子操作才行。我感觉`AND`信号量具有多种实现的方法啊，比如可以对资源分配的过程加一个互斥访问锁，如下面的代码指示的那样：

```c
Semaphore mutex(1);

void philosopher(){
	while(true){
		thinking();
		//I'm hungry
		mutex.P();
		forks[LEFT].P();
		forks[RIGHT].P();
		mutex.V();
		eating();
		forks[LEFT].V();
		forks[RIGHT].V();
	}
}
```

这里由于把对所有资源的分配组织成了一个原子操作，因此也不会出现第一种尝试中的死锁现象。

另一种方法是把哲学家左右的两只叉子抽象为一个资源，即为每个哲学家设置一个信号量。哲学家请求叉子时，首先检查他相邻的两个哲学家是否在就餐，只有相邻两个哲学家都没有获得叉子的时候才为他分配同时分配两只叉子。应该注意到，检查相邻哲学家是否在就餐，其实就是在检查是否左右两只叉子是否都是空闲状态，因此该检查的动作应该被封装为原子操作。为了检查哲学家的状态，为每个哲学家设置三个状态，即思考`THINKING`，饥饿`HUNGRY`，和就餐`EATING`。实现的伪代码如下：

```c
Semaphore phis[5]; 			//one semaphore for each philosopher
Semaphore mutex(1);
int state[5];
for(phi : phis)
	phi.sem = 0;			//not available in the beginning

void check(i){
	if(state[i] == HUNGRY && (state[LEFT] != EATING && state[RIGHT] != EATING)){
		state[i] = EATING;
		phis[i].V();	
	}
}

//for philosopher i
void philosopher(){
	while(true){
		thinking();
		//I'm hungry
		state[i] = HUNGRY
		mutex.P();
		check(i);
		mutex.V();
		phis[i].P();
		eating();
		state[i] = THINKING;
		mutex.P();
		check(LEFT);
		check(RIGHT);
		mutex.V();
	}
}
```

在[lab7_report](lab7_report.md)中还叙述了如何通过条件变量和管程，利用`AND`信号量机制来实现哲学家就餐问题。
