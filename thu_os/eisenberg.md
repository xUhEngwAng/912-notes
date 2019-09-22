多进程互斥问题的软件实现方法
==========================

在前面[进程管理（3）：同步互斥](chp17.md)中，已经就两个进程情况下互斥问题的软件实现方法进行了讨论，最终介绍了两种算法，即`Peterson`算法和`Dekkers`算法，这两种算法的基本思路是相同的，但是`Dekkers`算法更容易推广到多进程的情况。因此，以下我们将尝试以`Dekkers`算法为基础，实现多进程的互斥访问。

## 一些简单尝试

首先还是先来回顾一下`Dekkers`算法，它的代码描述如下：

```c
flag[i] = true;
while(flag[j] == true){ 
    if(turn != i){ 
       flag[i] = false 
       while(turn != i); 
       flag[i] = true 
    }  
} 
critical_section
turn = j
flag[i] = false;
```

在两个进程的情况下，该算法是通过设置一个`turn`标志位来表示当前应该由哪个进程进入临界区，每个进程都拥有一个`flag[i]`标志位，表示当前进程是否想要进入临界区。

因此，可以简明的将`Dekkers`算法推广到多进程的情况下，即为每一个进程都设置一个`flag`标志位来表示该进程是否想要进入临界区，继续沿用`turn`标志位表示获得了进入临界区权限的进程。在进入临界区之前，每个进程都需要首先判断所有其他进程的`flag`标志，保证没有其他进程想要进入临界区时，当前进程才进入。在进程退出临界区时，将`turn`标志设置为当前进程的下一个进程。这样，就形成了下面的代码：

```c
//code for process pid
flag[pid] = true;
while(1){
	for(idx = pid; idx != pid; idx = (idx + 1) % n)
		if(flag[idx] == true) break;
	if(idx = pid) break;
	if(turn != pid){
		flag[pid] = false;
		while(turn != pid);
		flag[pid] = true;
	}
}
critical_section
turn = (pid + 1) % n;
flag[pid] = false;
```

和双进程的`Dekkers`算法比较，该版本的主要区别只是在`while`循环处，需要判断所有其他进程的标志位，一旦有一个进程的标志位为`true`，就会进入循环内部。

乍一看这个算法好像是挺好的，但它还具有致命性的缺陷。考虑一种情况，同时有多个进程请求进入临界区，但是当前`turn`所指向的那个进程并不想要进入临界区。那些想要进入临界区的进程都会因为互相判断到对方而进入循环体，由于它们都不拥有`turn`，它们都将陷入等待`turn`的循环，但是拥有`turn`的进程却并不进入临界区，因此也无法释放`turn`标志。可见，这种实现方法不满足`空闲则入`原则。

## 另一种尝试

然后看了看向勇老师的网课，他说所有可能进入临界区的进程是排成了一个环。想要进入临界区的进程只需要等待介于`turn`和自己之间的进程。进程退出临界区时，将`turn`设置为下一个请求进入的进程。整体的流程如下图所示：

![nproc_mutex](nproc_mutex.png)

因此，我就根据这个简单的描述修改了上面第一种尝试的代码，形成了下面的代码：

```c
//code for process pid
flag[pid] = true;
while(1){
	for(idx = turn; idx != pid; idx = (idx + 1) % n)
		if(flag[idx] = true) break;
	if(idx == pid) break;
	if(turn != pid){
		flag[pid] = false;
		while(turn != pid);
		flag[pid] = true;
	}
}
critical_section
for(idx = pid + 1; idx != pid; idx = (idx + 1) % n)
	if(flag[idx] == true) break;
turn = idx;
flag[pid] = false;
```

看起来这种实现方法似乎规避了第一次尝试的不足，因为总存在第一个离`turn`最近的请求进程，它只需要等待`turn`进程即可，不可能出现多个想要进入临界区的进程互相等待的情况，并且如果`turn`进程并没有请求进入临界区，离`turn`最近的进程可以直接通过循环进入临界区，满足了`空闲则入`原则。

但是通过深入的分析，我们可以构造出一种特殊情况。当前拥有`turn`的进程也没有请求进入临界区，距离`turn`为1的进程没有请求临界区，距离`turn`为3的进程发出了请求，因此它进入`for`循环判断在`turn`和它之间的进程是否还有发出请求的。然后这个循环还没有结束，刚刚判断完了距离`turn`为1的进程，就发生了进程的调度。此时，距离`turn`为1的进程也发出了进入临界区的请求，这样，这两个进程就同时进入临界区了，就违背了`忙则等待`原则。

## `eisenberg`算法

`eisenberg`算法是在上面算法的基础上的又一次改进，它的伪代码如下：

```c
repeat {
	/* announce that we need the resource */
	flags[i] := WAITING;

	/* scan processes from the one with the turn up to ourselves. */
	/* repeat if necessary until the scan finds all processes idle */
CYCLE1:
	index := turn;
	while (index != i) {
		if (flags[index] != IDLE) index := turn;
		else index := (index+1) mod n;
	}

	/* now tentatively claim the resource */
	flags[i] := ACTIVE;
CYCLE2:
	/* find the first active process besides ourselves, if any */
	index := 0;
	while ((index < n) && ((index = i) || (flags[index] != ACTIVE))) {
		index := index+1;
	}

	/* if there were no other active processes, AND if we have the turn
	   or else whoever has it is idle, then proceed.  Otherwise, repeat
	   the whole sequence. */
        } until ((index >= n) && ((turn = i) || (flags[turn] = IDLE)));

        /* Start of CRITICAL SECTION */

	/* claim the turn and proceed */
	turn := i;

        /* Critical Section Code of the Process */

        /* End of CRITICAL SECTION */

        /* find a process which is not IDLE */
	/* (if there are no others, we will find ourselves) */

	index := (turn+1) mod n;
	while (flags[index] = IDLE) {
		index := (index+1) mod n;
	}

	/* give the turn to someone that needs it, or keep it */
	turn := index;

	/* we're finished now */
	flags[i] := IDLE;

    /* REMAINDER Section */
```

可以看得到，这个算法也太复杂了，下面直接给出对该算法的分析。

在`eisenberg`算法中，进程被分成了三个状态，分别为`IDLE`, `WAITING`, `ACTIVE`。`IDLE`表示进程没有请求进入临界区，`WAITING`表示发出了请求，在等待进入临界区，`ACTIVE`表示可能进入了临界区，也可能还没有。进入区主要是分成两个部分，分别是`CYCLE1`和`CYCLE2`。

在`CYCLE1`中的操作与我们第二次尝试的`for`循环一致，即判断从`turn`到当前进程之间是否存在请求进程，若存在，则不断循环`CYCLE1`，直至不存在这样的进程，将当前进程标记为`ACTIVE`。可以看出，所有标记为`ACTIVE`的进程，在循环中都判断自己是距离`turn`最近的请求进程，但是由于进程的调度，这样的进程仍然有多个，而这正是导致我们第二次尝试失败的原因。

在`CYCLE2`中，对所有`ACTIVE`的进程做进一步的判断，主要操作就是判断除了当前进程以外，是否还存在其他`ACTIVE`的进程。需要注意的是，这里是对所有其他进程做判断，而不仅限于判断从`turn`到当前进程。

进入临界区的条件是，当前进程是唯一的`ACTIVE`进程，并且当前进程获得了`turn`，或者获得`turn`的进程处于`IDLE`状态。否则，退回到`CYCLE1`重新进行上面的判断。

首先证明，`eisenberg`算法是满足`忙则等待`原则的，这是通过`CYCLE2`来实现的。在第一次尝试中已经说明过，对所有进程做遍历并且保证当前进程是唯一的请求进程或者`ACTIVE`进程是一种更强的互斥条件，它甚至会导致`空闲则入`原则的不满足。

下面说明一定会有进程可以进入临界区，即`空闲则入`原则是满足的。设一次循环中，所有`ACTIVE`状态的进程中，距离`turn`最近的进程为`pid_min`，距离`turn`最远的进程为`pid_max`。在`CYCLE2`中，由于同时存在多个`ACTIVE`进程，所有这些进程都会重新进入`WAITING`状态并且退回到`CYCLE1`。第二次循环，根据`CYCLE1`的性质，这些进程中只有`pid_min`可以通过`CYCLE1`进入`ACTIVE`状态，此时进入`ACTIVE`状态的进程至多只有`pid_min - turn`个。这样，在每次循环以后，可以进入`ACTIVE`状态的进程数量是递减的，最终只会有一个进程可以通过`CYCLE1`进入`ACTIVE`状态，并且获得临界区的访问权限。