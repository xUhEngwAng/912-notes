Lab4 Report
===========

## 实验目的

+ 了解内核线程创建/执行的管理过程
+ 了解内核线程的切换和基本调度过程

## 实验内容

实验2/3完成了物理和虚拟内存管理，这给创建内核线程（内核线程是一种特殊的进程）打下了提供内存管理的基础。当一个程序加载到内存中运行时，首先通过`ucore OS`的内存管理子系统分配合适的空间，然后就需要考虑如何分时使用CPU来“并发”执行多个程序，让每个运行的程序（这里用线程或进程表示）“感到”它们各自拥有“自己”的CPU。

本次实验将首先接触的是内核线程的管理。内核线程是一种特殊的进程，内核线程与用户进程的区别有两个：

+ 内核线程只运行在内核态
+ 用户进程会在在用户态和内核态交替运行
+ 所有内核线程共用ucore内核内存空间，不需为每个内核线程维护单独的内存空间
+ 而用户进程需要维护各自的用户内存空间

## 练习

对实验报告的要求：

+基于markdown格式来完成，以文本方式为主
+ 填写各个基本练习中要求完成的报告内容
+ 完成实验后，请分析`ucore lab`中提供的参考答案，并请在实验报告中说明你的实现与参考答案的区别
+ 列出你认为本实验中重要的知识点，以及与对应的OS原理中的知识点，并简要说明你对二者的含义，关系，差异等方面的理解（也可能出现实验中的知识点没有对应的原理知识点）
+ 列出你认为OS原理中很重要，但在实验中没有对应上的知识点

## lab4 整体实验流程概述

lab4 相对于前面的实验，就是增加了一个内核线程，即`kern/process`文件夹里面的内容。此外其他的文件也有一些细微的修改，比如增加了一些函数供后续调用等，这种变化目前可以忽略啊。

在增加的`kern/process`中，主要就是完成了三件事情，即建立第零个内核线程，建立第一个内核线程，以及将CPU的控制权从第零个线程转交给第一个线程，这也分别对应了本实验的三个练习。将在下面分别详细阐述。

## 建立第零个内核线程

在原理课中有讲到，在实际的操作系统中，例如在`linux`中，当一个用户进程希望创建一个子进程时，可以通过`fork`系统调用，由操作系统代劳为其建立一个子进程。在`fork`系统调用中，主要进行的操作是，将当前的进程拷贝到新创建的进程中，即占用的各类资源，一些状态信息，复制到新创建的进程中，然后再在后续的操作中，更新其地址空间，更新子进程要执行的代码等。

但是这些操作都不适用于第零号内核线程，因为上面进程的创建，是在已经有了一个进程的基础上进行的，对于第零号进程则不然。我们这里讨论的，就是如何实现这种由零到一的转变。

操作系统通过进程控制块（PCB, Process Control Block），或者称为线程控制块（TCB, Thread Control Block）来管理进程。第零号内核线程的创建，其实就是手动构造它的TCB，将它的各个属性设置为合适的值，使它可以正常运行。

此外还需要明确，第零号内核线程究竟是干什么的？实际上，它什么也不干，即所谓的`idleproc`，它的主要作用就是在操作系统没有其它进程可以运行的时候，让操作系统运行这个线程。而它的主函数体如下：

```c
// cpu_idle - at the end of kern_init, the first kernel thread idleproc will do below works
void
cpu_idle(void) {
    while (1) {
        if (current->need_resched) {
            schedule();
        }
    }
}
```

可以看到，它的工作就是不断地调用调度函数（`schedule`），以试图找到一个处于就绪队列的进程，将其放到CPU运行。

> 构造第零号内核线程

构造第零号内核线程就是手动构造它的TCB，让它成为一个合法的线程。那么，应该如何构造呢？具体步骤如下：

+ 首先为它的TCB分配一个内存空间，这可以通过`alloc_proc`函数实现。
+ 接下来就可以为它的TCB的各字段赋值了，主要包括
	- pid：需要给它设置唯一的进程id，这里设置为零，以表现出它的特殊性
	- state:标记它的状态为就绪态(`PROC_RUNNABLE`);
	- kstack:设置内核堆栈，关于什么是内核堆栈，会在后面提到。这里直接设置它的内核堆栈为`UCORE`建立的内核堆栈`bootstack`。
	- need_resched:是否需要被调度。当然要被调度啦！它就是个无所事事的`idle`线程啊。
	- name:给它个名字，就叫`idle`吧。
	- cr3：在`alloc_proc`中进行了设置，将其赋值为`boot_cr3`，即内核虚拟地址空间的页表，这是因为内核线程和操作系统共享内核地址空间。
+ 此外，还有一些字段是没有设置的，比如
	- parent:父进程，你说它有没有父进程？
	- mm:用户虚拟地址空间。它是内核线程啊，没有用户地址空间。
	- 一些状态信息，如`tf`，`flags`，`context`：它作为第一个运行的线程可以不用设置的，因为它是第一个运行的啊，后面在它被打断的时候，会通过中断机制设置这些信息。而别的线程就不行。会在后面说线程切换的时候详细说明。

上面就说清楚了应该如何构造第一个内核线程，具体的代码如下：

```c
void
proc_init(void) {
    int i;

    list_init(&proc_list);
    for (i = 0; i < HASH_LIST_SIZE; i ++) {
        list_init(hash_list + i);
    }

    if ((idleproc = alloc_proc()) == NULL) {
        panic("cannot alloc idleproc.\n");
    }

    idleproc->pid = 0;
    idleproc->state = PROC_RUNNABLE;
    idleproc->kstack = (uintptr_t)bootstack;
    idleproc->need_resched = 1;
    set_proc_name(idleproc, "idle");
    nr_process ++;

    current = idleproc;
......
```

在前面还进行了进程列表的初始化(`proc_list`)，该列表用来保存所有进程，用于进程调度算法的。一旦运行了`current = idleproc`，第零个内核线程就投入运行了，后续代码的执行，后续的系统初始化工作，都可以看做它的运行过程。

## 练习1：分配并初始化一个进程控制块（需要编码）

`alloc_proc`函数（位于`kern/process/proc.c`中）负责分配并返回一个新的`struct proc_struct`结构，用于存储新建立的内核线程的管理信息。`ucore`需要对这个结构进行最基本的初始化，你需要完成这个初始化过程。

> 【提示】在alloc_proc函数的实现中，需要初始化的proc_struct结构中的成员变量至少包括：state/pid/runs/kstack/need_resched/parent/mm/context/tf/cr3/flags/name。

请在实验报告中简要说明你的设计实现过程。请回答如下问题：

+ 请说明`proc_struct`中`struct context context和struct trapframe *tf`成员变量含义和在本实验中的作用是什么？（提示通过看代码和编程调试可以判断出来）

### `alloc_proc`函数的实现

在进程的三状态模型中，我们说进程的状态除了就绪，运行，等待（挂起）以外，还有两个特殊的状态，即创建和退出。进程的创建态就涉及到这里的`alloc_proc`函数，因此，在`alloc_proc`函数中，除了为了新的进程控制块申请一块空闲的内存空间外，还要为其中的字段进行一些初步的设置，来表示该进程还未完成创建，处于创建态，此时还不可以被调度，亦不可以被杀死。

进程控制块中各个字段的初始化如下：

+ state: 处于创建态，这里用`PROC_UNINIT`表示，即“尚未初始化完成”。
+ pid：因为此时还不是一个合法的进程，所以给它的pid赋负值，来表示这种不合法。这里简单设置为-1。
+ runs: 运行时间，为零。
+ kstack: 内核堆栈。这个进程还刚在初始化啊，相关资源还没有分配，内核堆栈也还没有建立好，设置为空指针`NULL = 0`。此外的资源信息都没有，即`mm = NULL`。
+ need_resched: 不可调度。
+ parent：设置为当前进程`current`。
+ context和tf和flags：是用于恢复进程现场的信息，这还没有现场啊，都设置为零。
+ cr3：由于是内核线程，和操作系统共用页表，设置为`boot_cr3`。

大体上就是这样，具体的代码如下：

```c
alloc_proc(void) {
    struct proc_struct *proc = kmalloc(sizeof(struct proc_struct));
    if (proc != NULL) {
    proc->state  = PROC_UNINIT;
	proc->pid    = -1;
	proc->runs   = 0;
	proc->kstack = 0;
	proc->cr3    = boot_cr3;
	proc->flags  = 0;
	proc->parent = NULL;
	proc->mm     = NULL;
	proc->tf     = NULL;
	set_proc_name(proc, "undefined");
	proc->need_resched = 0;
	proc->context.eip = 0;
	proc->context.esp = 0;
	proc->context.ebx = 0;
	proc->context.ecx = 0;
	proc->context.edx = 0;
	proc->context.esi = 0;
	proc->context.edi = 0;
	proc->context.ebp = 0;
    }
    return proc;
}
```

这只是我的版本，其中是存在问题的，比如我没有设置`parent`信息，上面也说了应该设置为`current`。不过其他的都差不多，只是我的实现不优雅而已，比如说老师的代码里面对`context`的设置只有一句`memset(&(proc->context), 0, sizeof(struct context));`就完成了，我就比较蠢了。
