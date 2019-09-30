Lab6 Report
===========

## 实验目的

+ 理解操作系统的调度管理机制
+ 熟悉`ucore`的系统调度器框架，以及缺省的`Round-Robin`调度算法
+ 基于调度器框架实现一个`Stride Scheduling`调度算法来替换缺省的调度算法

## 实验内容

实验五完成了用户进程的管理，可在用户态运行多个进程。但到目前为止，采用的调度策略是很简单的`FIFO`调度策略。本次实验，主要是熟悉`ucore`的系统调度器框架，以及基于此框架的`Round-Robin（RR）` 调度算法。然后参考`RR`调度算法的实现，完成`Stride Scheduling`调度算法。

## 练习

对实验报告的要求：

+ 基于markdown格式来完成，以文本方式为主
+ 填写各个基本练习中要求完成的报告内容
+ 完成实验后，请分析`ucore lab`中提供的参考答案，并请在实验报告中说明你的实现与参考答案的区别
+ 列出你认为本实验中重要的知识点，以及与对应的OS原理中的知识点，并简要说明你对二者的含义，关系，差异等方面的理解（也可能出现实验中的知识点没有对应的原理知识点）
+ 列出你认为OS原理中很重要，但在实验中没有对应上的知识点

## lab6概述

首先简单叙述一下lab6都完成了或者需要完成哪些工作。

lab6最重要的工作，就是实现了进程调度的框架类，并且基于这个框架，更新了有关进程调度的一些函数的实现，比如`schedule`，还有`wake_proc`等。

此外，使用这个进程调度框架，可以实现原理课中讲过的各种进程调度算法，在lab6中是实现了（要求实现）`RR`调度算法以及`Stride`调度算法。

## 进程调度框架的实现

为了实现一个一般的进程调度框架，我们需要考虑不同的进程调度算法所具有的共性，将这些共性抽象出来形成代码，就成为了我们这里的进程调度框架。

对于任意一个进程调度算法，在进行调度`schedule`时，都需要进行两方面的工作，即从就绪队列中选择一个新的进程投入运行，并且将当前进程（如果仍然就绪的话）重新加入就绪队列中。因此，这里就涉及到两个基本的操作，即从就绪队列中选取一个进程(`pick`)，以及将一个进程加入到就绪队列中(`enqueue`)。实际上，这里还隐含的一个操作，即将被选取的进程从就绪队列中删除，即`dequeue`操作。

上面的讨论只是关于具体的进程调度的，现在再来考虑以下其他和进程调度有关的情形。

+ 在`fork`一个新的进程时，在`do_fork`函数中需要将新创建的进程加入就绪队列中，可以调用上面提到的`enqueue`操作；
+ 当进程等待资源或者等待子进程退出(`wait`)时，需要将自己的状态标记为等待态(`WAITING`)，并且调用`schedule`函数，在`schedule`函数中将完成其余实质性的工作；
+ 当进程退出时(`exit`)，需要将自己的状态标记为僵尸态(`ZOMBIE`)，随后调用`schedule`函数，完成剩余的工作；
+ 当进程因为获得等待的资源而被唤醒时(`wakeup_proc`)，应该将该进程的状态标记为就绪态(`RUNNING`)，并且将它放入就绪队列中，可以调用`enqueue`来完成这个操作。

可以看到，上面抽象出来的三个基本操作，几乎已经可以满足进程调度中可能出现的所有情况了，因此只要把这些操作封装起来，就基本实现了我们这里的进程调度框架。如下面的代码所示：

```c
struct sched_class {
    // the name of sched_class
    const char *name;
    // Init the run queue
    void (*init)(struct run_queue *rq);
    // put the proc into runqueue, and this function must be called with rq_lock
    void (*enqueue)(struct run_queue *rq, struct proc_struct *proc);
    // get the proc out runqueue, and this function must be called with rq_lock
    void (*dequeue)(struct run_queue *rq, struct proc_struct *proc);
    // choose the next runnable task
    struct proc_struct *(*pick_next)(struct run_queue *rq);
}
```

这里实现的进程调度类和上面我们描述的几乎是对应的，只是添加了`name`字段来保存当前使用的进程调度算法的名字，以及`init`函数来对相关数据结构进行初始化。

但是这里实现的进程调度类的确已经可以满足所有的进程调度算法了吗？考虑时间片轮转算法，我们会为每个进程分配一个固定的时间片，一旦这个时间片用完，就对当前的进程进行调度。这里的问题是，如何在进程执行的过程中更新这个时间片呢？

显然，这个工作只能由操作系统来完成，因为和进程有关的控制信息是保存在内核的，并且让用户进程本身来管理这些控制信息也比较危险。一种解决方案是由时钟定期地产生中断，在时钟中断的处理函数对进程的时间片进行更新。

事实上，不只是`RR`算法，其他进程调度算法也需要在运行过程中动态地更新一些参数。例如最高响应比优先算法`HRRN`，也需要在执行中动态更新每个进程的等待时间，来计算每个进程的响应比。因此，我们这里的进程调度框架还需要一个参数更新的函数，来在运行时动态地更新调度算法的参数。更新后的进程调度框架类见下面的代码：

```c
struct sched_class {
	......
	// dealer of the time-tick
	void (*proc_tick)(struct run_queue *rq, struct proc_struct *proc);
}
```

## 练习1: 使用 Round Robin 调度算法（不需要编码）

完成练习0后，建议大家比较一下（可用`kdiff3`等文件比较软件）个人完成的lab5和练习0完成后的刚修改的lab6之间的区别，分析了解lab6采用`RR`调度算法后的执行过程。执行`make grade`，大部分测试用例应该通过。但执行`priority.c`应该过不去。

请在实验报告中完成：

+ 请理解并分析`sched_class`中各个函数指针的用法，并接合`Round Robin` 调度算法描ucore的调度执行过程
+ 请在实验报告中简要说明如何设计实现”多级反馈队列调度算法“，给出概要设计，鼓励给出详细设计

### `sched_class`中各个函数指针的用法

基于进程调度框架`sched_class`，我们可以实现各种不同的进程调度算法，所需要做的只是定义一个新的`struct sched_class`类型的变量，作为当前调度算法的调度器，并且实现其中定义的各个函数指针就可以了。

那么这里产生了一个问题，在实现了某一个进程调度算法的调度器`struct sched_class`以后，怎么让操作系统使用这个调度器进行进程的调度呢？事实上，在`kern/schedule/sched.c(h)`中进行了相应的实现。

具体的实现方法就是定义了一个全局的进程调度器`struct sched_class *sched_class`，首先是基于这个调度器的函数指针实现了进程调度相关的函数，如下面的代码所示：

```c
static struct sched_class *sched_class;

static struct run_queue *rq;

static inline void
sched_class_enqueue(struct proc_struct *proc) {
    if (proc != idleproc) {
        sched_class->enqueue(rq, proc);
    }
}

static inline void
sched_class_dequeue(struct proc_struct *proc) {
    sched_class->dequeue(rq, proc);
}

static inline struct proc_struct *
sched_class_pick_next(void) {
    return sched_class->pick_next(rq);
}

static void
sched_class_proc_tick(struct proc_struct *proc) {
    if (proc != idleproc) {
        sched_class->proc_tick(rq, proc);
    }
    else {
        proc->need_resched = 1;
    }
}
```

可以看到，这些与调度相关的函数本质上都是调用了`sched_class`中的同名函数而已。基于这些基本函数，可以实现进程调度函数`schedule`，进程唤醒函数`wakeup_proc`：

```c
void
wakeup_proc(struct proc_struct *proc) {
    assert(proc->state != PROC_ZOMBIE);
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        if (proc->state != PROC_RUNNABLE) {
            proc->state = PROC_RUNNABLE;
            proc->wait_state = 0;
            if (proc != current) {
                sched_class_enqueue(proc);
            }
        }
        else {
            warn("wakeup runnable process.\n");
        }
    }
    local_intr_restore(intr_flag);
}

void
schedule(void) {
    bool intr_flag;
    struct proc_struct *next;
    local_intr_save(intr_flag);
    {
        current->need_resched = 0;
        if (current->state == PROC_RUNNABLE) {
            sched_class_enqueue(current);
        }
        if ((next = sched_class_pick_next()) != NULL) {
            sched_class_dequeue(next);
        }
        if (next == NULL) {
            next = idleproc;
        }
        next->runs ++;
        if (next != current) {
            proc_run(next);
        }
    }
    local_intr_restore(intr_flag);
}
```

为了实际运行某个调度算法的调度器，只需要使全局的进程调度器`sched_class`指针指向这个算法的调度器，这样，在操作系统进行调度的时候，实际上就是使用该调度算法的策略来进行调度了。

```c
sched_class = &default_sched_class;
```

### `RR`调度算法的实现

基于这个进程调度框架，为了实现`RR`调度算法，只需要实现其中定义的各个函数就可以了。下面将分析`RR`调度算法应该如何基于这个进程调度框架来实现。

我们知道，`RR`调度算法采用的进程选择策略和先来先服务`FCFS`算法一致，即在选择一个进程进入运行时，只需要选择就绪队列的首进程；当一个新的进程加入就绪队列时，只需要将它加入到就绪队列的末尾就可以了。很明显，为了实现`RR`调度算法，需要管理这样一个按照进程进入就绪状态的先后次序排序的就绪队列，这就是定义在`kern/schedule/sched.h`中的`run_queue`结构体：

```c
struct run_queue {
    list_entry_t run_list;
    unsigned int proc_num;
    int max_time_slice;
};
```

这里是采用一个双向链表`list_entry_t`来作为就绪队列的底层结构，除此以外，`run_queue`结构体中还管理了处在就绪队列中的进程数量`proc_num`，以及当前就绪队列所能分到的最大时间片长度`max_time_slice`。

这样，我们就可以基于进程调度框架实现`RR`调度算法了：

```c
static void
RR_init(struct run_queue *rq) {
    list_init(&(rq->run_list));
    rq->proc_num = 0;
}

static void
RR_enqueue(struct run_queue *rq, struct proc_struct *proc) {
    assert(list_empty(&(proc->run_link)));
    list_add_before(&(rq->run_list), &(proc->run_link));
    if (proc->time_slice == 0 || proc->time_slice > rq->max_time_slice) {
        proc->time_slice = rq->max_time_slice;
    }
    proc->rq = rq;
    rq->proc_num ++;
}

static void
RR_dequeue(struct run_queue *rq, struct proc_struct *proc) {
    assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
    list_del_init(&(proc->run_link));
    rq->proc_num --;
}

static struct proc_struct *
RR_pick_next(struct run_queue *rq) {
    list_entry_t *le = list_next(&(rq->run_list));
    if (le != &(rq->run_list)) {
        return le2proc(le, run_link);
    }
    return NULL;
}
```

可以看到，`RR`调度算法的实现和我们上面的分析几乎是一致的，只有一些细节上的修改。除此以外，还需要实现`RR`调度算法的参数更新函数`proc_tick`，根据`RR`调度算法的策略，应该是在每次时钟中断时，将当前进程的时间片大小减一，当当前进程用完它的时间片时，就设置进程的需要调度标志`need_resched`，等待后续处理对它进行调度。实现的`proc_tick`函数如下：

```c
static void
RR_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
    if (proc->time_slice > 0) {
        proc->time_slice --;
    }
    if (proc->time_slice == 0) {
        proc->need_resched = 1;
    }
}
```

基本实现了`RR`调度算法以后，就可以定义它的调度器，并且使全局进程调度器指针`sched_class`指向这个调度器：

```c
struct sched_class default_sched_class = {
    .name = "RR_scheduler",
    .init = RR_init,
    .enqueue = RR_enqueue,
    .dequeue = RR_dequeue,
    .pick_next = RR_pick_next,
    .proc_tick = RR_proc_tick,
};

void
sched_init(void) {
    list_init(&timer_list);

    sched_class = &default_sched_class;

    rq = &__rq;
    rq->max_time_slice = MAX_TIME_SLICE;
    sched_class->init(rq);

    cprintf("sched class: %s\n", sched_class->name);
}
```

### `RR`算法调度执行过程分析

在[进程管理（2）：进程调度策略](chp15.md)中分析过，进程调度的时机主要分为两类，一类是进程主动让出CPU控制权，另一类是当前运行的进程被其他进程抢占。但无论是第一类还是第二类，对于用户进程而言都是在中断中进行的。

若采用`RR`调度算法，当一个进程的时间片用完时，其他进程就会对当前进程进行抢占。这里的抢占与进程调度是借助中断机制实现的，下面就具体分析这个实现过程。

当操作系统产生一次时钟中断时，硬件会把当前进程的中断帧保存，并且进入内核处理中断。它会进入`kern/trap/trap.c::trap`函数，在其中会调用`trap_dispatch`对中断进行分发和处理，这里详细的过程可以参考[lab1实验报告](lab1_report.md)；最终，程序会跳转到时钟中断的中断处理函数执行，在这里，它会调用`sched_class_proc_tick`函数来对当前进程的时间片进行更新，倘若当前进程的时间片恰好用光，则会修改它的需要调度标志`need_resched`。`trap`函数的具体实现如下：

```c
void
trap(struct trapframe *tf) {
    // dispatch based on what type of trap occurred
    // used for previous projects
    if (current == NULL) {
        trap_dispatch(tf);
    }
    else {
        // keep a trapframe chain in stack
        struct trapframe *otf = current->tf;
        current->tf = tf;
    
        bool in_kernel = trap_in_kernel(tf);
    
        trap_dispatch(tf);
    
        current->tf = otf;
        if (!in_kernel) {
            if (current->flags & PF_EXITING) {
                do_exit(-E_KILLED);
            }
            if (current->need_resched) {
                schedule();
            }
        }
    }
}
```

可以看到，从中断处理函数返回以后，在`trap`中会对当前进程是否是内核线程做一次判断，倘若是内核线程，则直接恢复中断现场。如果当前进程是一个用户进程，则会进一步判断它是否需要调度，如果的确需要调度，则会调用`schedule`函数来完成调度。这里对内核线程的判断，表示对内核线程是不可抢占的。

需要注意的是，此时是在中断里面进行调度，当调用`schedule`函数时，会在`proc_run`函数中完成内核堆栈的切换以及上下文信息的切换。这样，当`switch_to`返回时，会返回到新运行进程的`trap`函数，上次该进程正是在此处完成了进程的切换的，通过`__trapret`，该进程可以恢复它上次被切换时进入中断而保存的中断帧，从而成功跳转到用户程序中被中断的位置接着执行。进程切换过程的示意图所下图所示：

![process_switch](images/process_switch.png)

实际上，不只是时间片用完时的进程抢占，其他用户进程切换的情形也和这里是类似的，例如一个用户进程需要等待某一个资源，此时它需要调用`wait`系统调用进入中断，在实际的服务函数`do_wait`中，会调用`schedule`来完成进程切换。该进程再次被调度时，仍然是通过`iret`从中断处理中返回。唯一的不同点在于此时是通过`do_wait`中的`schedule`实现调度的，而前者是通过`trap`中的`schedule`实现调度的。

### 多级反馈队列调度算法的设计

为了实现`MLFQ`算法，显然首先是需要在底层提供多个队列，每个队列具有不同的优先级，相应的也具有不同大小的时间片长度。

```c
#define NUM_RUN_QUEUE 5
static struct run_queue rq[NUM_RUN_QUEUE];

static void
MLFQ_init(struct run_queue *rq){
	for(int ix = 0; ix != NUM_RUN_QUEUE; ++ix){
		list_init(rq[ix].run_list);
		rq[ix].proc_num = 0;
		rq[ix].max_time_slice = MAX_TIME_SLICE * (1 << ix);
	}
}
```

由于进程会在不同的队列之间切换，需要保存当前进程已经执行的次数，实际上，这个信息已经保存在了`proc_struct->runs`变量中了。当进程首次被创建时，将它加入优先级最高的队列；以后，如果进程的时间片用完，则让它进入优先级低一级的就绪队列，而如果只是因为等待资源而阻塞的话，则在唤醒后还是加入同级队列。基于此，可以给出`enqueue`函数的实现：

```c
static void
MLFQ_enqueue(struct run_queue *rq, struct proc_struct *proc){
	int runs = proc->runs;
	if(runs > 4) runs = 4;

	if(proc->time_slice == 0) 
		proc->time_slice = rq[runs].max_time_slice;

	else{//return to the original queue
		proc->runs--;
		runs = proc->runs > 4?proc->runs: 4;
	}
	list_add_before(&(rq[runs].run_list), &(proc->run_link));
	proc->rq = &rq[runs];
	rq[runs].proc_num++;
}
```

在选择一个进程进行调度时，即按照优先级遍历各个就绪队列，将首个就绪进程取出即可。为了将该进程从就绪队列中删除，直接调用`list_del`函数就可以了。这两个函数的实现如下：

```c
static void 
MLFQ_dequeue(struct run_queue *rq, struct proc_struct *proc){
	proc->rq->proc_num--;
	list_del_init(&(proc->run_link));
}

static struct proc_struct *
MLFQ_pick_next(struct run_queue *rq){
	for(int ix = 0; ix != NUM_RUN_QUEUE; ++ix){
		if(rq[ix].proc_num != 0){
			list_entry_t *le = list_next(&(rq[ix].run_list));
			assert(le != &(rq[ix].run_list));
			return le2proc(le, run_link);
		}
	}
	return NULL;
}
```

最后，时间片长度更新的函数和`RR`调度算法应该是一致的，这样，就完成了`MLFQ_sched_class`调度器的构造。

```c
static void
MLFQ_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
    if (proc->time_slice > 0) {
        proc->time_slice --;
    }
    if (proc->time_slice == 0) {
        proc->need_resched = 1;
    }
}

struct sched_class MLFQ_sched_class = {
    .name = "MLFQ_scheduler",
    .init = MLFQ_init,
    .enqueue = MLFQ_enqueue,
    .dequeue = MLFQ_dequeue,
    .pick_next = MLFQ_pick_next,
    .proc_tick = MLFQ_proc_tick,
};
```

非常惭愧的是，由于时间有限，这里只是写出了初步实现的代码，因为懒得写测试用例，所以根本没有测试过，连语法都有错误也是可能的......以后我有机会再说这个问题。

## 练习2: 实现`Stride Scheduling`调度算法（需要编码）

首先需要换掉`RR`调度器的实现，即用`default_sched_stride_c`覆盖`default_sched.c`。然后根据此文件和后续文档对`Stride`调度器的相关描述，完成`Stride`调度算法的实现。

执行：make grade。如果所显示的应用程序检测都输出ok，则基本正确。如果只是priority.c过不去，可执行 make run-priority 命令来单独调试它。大致执行结果可看附录。（ 使用的是 qemu-1.0.1 ）。

请在实验报告中简要说明你的设计实现过程。

### `Stride Scheduling`算法描述

采用`RR`调度算法时，各个进程之间是公平的，它们都占有几乎相同的CPU时间。然而在实际的系统中，进程之间是有优先级之分的，高优先级的进程应该比低优先级的进程拥有更多的机会得到CPU的使用权。基于抢占式的优先级调度算法固然是一种解决方案，但是这种调度策略会产生饥饿现象。

我们这里是希望每个进程得到的CPU时间资源和它们的优先级成正比关系，`Stride`调度算法就是基于这种想法产生的。

为了实现`Stride`调度算法，首先每个进程都需要有一个优先级`priority`，根据`Stride`算法的思路，每个进程的执行时间是与这个优先级成正比的，为此，`Stride`算法中定义了两个变量，步长`stride`和总的步数`pass`。每个进程的步长`stride`是与它的优先级成反比的，可以定义一个大的常数`BIG_STRIDE`，这样

```c
stride = BIG_STRIDE / priority;
```

在需要进行进程调度时，总是选择当前总的步数`pass`最小的一个进程，并且让它“行走”一个步长`stride`的距离，即

```c
pass += stride;
```

这样，对于高优先级的进程，其步长`stride`较小，相应的`pass`增加得更慢，就有更多的机会得到CPU的使用权。而低优先级的进程，由于步长`stride`很大，`pass`在每次调度后都有一个很大的增量，它获得CPU使用权的机会就较少。当一个进程得到CPU的使用权后，`Stride`算法就类似于`RR`算法了——每个进程都只能运行一个固定的时间片，就需要再次进行调度。

可以证明（虽然我不会），`Stride`调度算法对进程的调度次数正比于其优先级，并且整个算法具有确定性，即在不考虑计时器事件的情况下，整个调度机制都是可预知和重现的。

### `Stride`算法的实现

同前面一样，只需要实现进程调度框架中的那几个函数，就可以实现`Stride`调度算法，以下将分别对各个函数进行分析。

首先需要注意到，`Stride`算法的选择策略是选择`pass`值最小的进程。为了迅速找到这样一个进程，我们需要合理选择底层的数据结构，这里是采用斜堆(`skew heap`)来实现，可以保证在平均情况下插入和删除操作的时间复杂度不超过`O(logn)`。因此，这里的底层就需要对`struct run_queue`结构体做一个拓展，使它具有一个斜堆的指针：

```c
struct run_queue {
    list_entry_t run_list;
    unsigned int proc_num;
    int max_time_slice;
    // For LAB6 ONLY
    skew_heap_entry_t *lab6_run_pool;
};
```

同理，进程控制块`struct proc_struct`中也要做相应的拓展。这样，在进程的`enqueue`和`dequeue`操作中，都需要维护这样的斜堆的结构。下面给出`enqueue`，`pick`和`dequeue`函数的具体实现：

```c
static void
stride_enqueue(struct run_queue *rq, struct proc_struct *proc) {
      rq->lab6_run_pool = skew_heap_insert(rq->lab6_run_pool, &(proc->lab6_run_pool), proc_stride_comp_f);
      if (proc->time_slice == 0 || proc->time_slice > rq->max_time_slice) {
        proc->time_slice = rq->max_time_slice;
      }
      proc->rq = rq;
      rq->proc_num++;
}

static void
stride_dequeue(struct run_queue *rq, struct proc_struct *proc) {
      rq->lab6_run_pool = skew_heap_remove(rq->lab6_run_pool, &(proc->lab6_run_pool), proc_stride_comp_f);
      rq->proc_num--;
}

static struct proc_struct *
stride_pick_next(struct run_queue *rq) {
      if (rq->lab6_run_pool == NULL) return NULL;
      struct proc_struct *p = le2proc(rq->lab6_run_pool, lab6_run_pool);
      if(p->lab6_priority == 0){//for kernel thread
        p->lab6_pass += BIG_STRIDE;
      }
      else
        p->lab6_pass += BIG_STRIDE / p->lab6_priority;

      return p;
}
```

但是这里还存在一个问题，即`pass`的值是一直增加的，而计算机的位数是有限的，通常只是采用32位整数来保存这个值，这样`pass`的值迟早会溢出，此时再对`pass`进行比较来选择下一个调度的进程就会产生错误的结果。因此，需要合理设计这里的比较函数`proc_stride_comp_f`，将这种情况考虑在内。

容易证明，在任何一次进行调度时，就绪队列中的任意两个进程都满足

$$
| pass_1 - pass_2 | \le stride\_max
$$

其中，`stride_max`表示所有进程中的最大步进值。这样，可以进一步

$$
passMax - passMin \le BIG\_STRIDE
$$

因此一种解决方案是，对两个`pass`之差的结果进行分析，如果它是介于`-BIG_STRIDE`到`BIG_STRIDE`之间的，则认为是一个合理值，返回正常的结果。若这个差值在上述区间之外，则认为其中一个`pass`已经发生了溢出，因此取相反的结果，对应的比较函数的实现如下：

```c
static int
proc_stride_comp_f(void *a, void *b)
{
      struct proc_struct *p = le2proc(a, lab6_run_pool);
      struct proc_struct *q = le2proc(b, lab6_run_pool);
      int32_t c = p->lab6_stride - q->lab6_stride;
      if (c > 0 && c <= BIG_STRIDE)  return 1;
      if(c < -BIG_STRIDE) return 1;
      if (c == 0) return 0;
      return -1;
}
```

但是按照老师的意思，这种方法不够优雅，还要多做几次判断，代码可读性......这谁读得懂啊。按照老师的意思，比较函数的实现就是正常的实现方法：

```c
static int
proc_stride_comp_f(void *a, void *b)
{
      struct proc_struct *p = le2proc(a, lab6_run_pool);
      struct proc_struct *q = le2proc(b, lab6_run_pool);
      int32_t c = p->lab6_stride - q->lab6_stride;
      if (c > 0)  return 1;
      if (c == 0) return 0;
      return -1;
}
```

在这样的情况下，就需要正确选择`BIG_STRIDE`的值，使得两个无符号32位整数之差，在`pass_1`的确大于`pass_2`时（指真值，而不是溢出之后的值），`pass_1 - pass_2`为正的有符号整数；在`pass_1`小于`pass_2`时，为负的有符号整数。

经过我的深思熟虑（我也说不清中间的逻辑是什么，这里比较复杂啊，设计比较底层的知识），可以验证，当`BIG_STRIDE = ~(1 << 31)`时，正好满足这个条件，此时就可以使用第二个版本那个简明的比较函数了。
