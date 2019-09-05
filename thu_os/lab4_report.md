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

+ 基于markdown格式来完成，以文本方式为主
+ 填写各个基本练习中要求完成的报告内容
+ 完成实验后，请分析`ucore lab`中提供的参考答案，并请在实验报告中说明你的实现与参考答案的区别
+ 列出你认为本实验中重要的知识点，以及与对应的OS原理中的知识点，并简要说明你对二者的含义，关系，差异等方面的理解（也可能出现实验中的知识点没有对应的原理知识点）
+ 列出你认为OS原理中很重要，但在实验中没有对应上的知识点

## lab4 整体实验流程概述

lab4 相对于前面的实验，就是增加了一个内核线程，即`kern/process`文件夹里面的内容。此外其他的文件也有一些细微的修改，比如增加了一些函数供后续调用等，这种变化目前可以忽略啊。

在增加的`kern/process`中，主要就是完成了三件事情，即建立第零个内核线程，建立第一个内核线程，以及将CPU的控制权从第零个线程转交给第一个线程，这也分别对应了本实验的三个练习。将在下面分别详细阐述。

## 建立第零个内核线程

在原理课中有讲到，在实际的操作系统中，例如在`linux`中，当一个用户进程希望创建一个子进程时，可以通过`fork`系统调用，由操作系统代劳为其建立一个子进程。在`fork`系统调用中，主要进行的操作是，将当前的进程拷贝到新创建的进程中，即将占用的各类资源，一些状态信息，复制到新创建的进程中，然后再在后续的操作中，更新其地址空间，更新子进程要执行的代码等。

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
+ parent：它这里是设置为`NULL`，然后推迟到后面的`do_fork`中再设置为当前进程`current`。我觉得直接设置为`current`应该也没问题吧。
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

这只是我的版本，基本和老师的标准答案差不多，只是我的实现不优雅而已，比如说老师的代码里面对`context`的设置只有一句

```c
memset(&(proc->context), 0, sizeof(struct context));
```

就完成了，我就比较蠢了。当然了，我不蠢谁蠢啊。

### `tf`和`context`的作用

这俩的作用在第零号内核线程还看不出来啊，其实主要是为了完成线程的切换的，只有合理设置了这两个字段，才能使新建立的线程被正确调度运行；以及可以在被其他进程打断以后，可以正确恢复现场。具体的还是在练习3里面会详细说明。

## 建立第一个内核线程

在有了第零个内核线程后，我们就可以通过该线程，来建立新的内核线程了。创建内核线程在函数`kern/process/proc.c::kernel_thread`中实现：

```c
// kernel_thread - create a kernel thread using "fn" function
// NOTE: the contents of temp trapframe tf will be copied to 
//       proc->tf in do_fork-->copy_thread function
int
kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags) {
    struct trapframe tf;
    memset(&tf, 0, sizeof(struct trapframe));
    tf.tf_cs = KERNEL_CS;
    tf.tf_ds = tf.tf_es = tf.tf_ss = KERNEL_DS;
    tf.tf_regs.reg_ebx = (uint32_t)fn;
    tf.tf_regs.reg_edx = (uint32_t)arg;
    tf.tf_eip = (uint32_t)kernel_thread_entry;
    return do_fork(clone_flags | CLONE_VM, 0, &tf);
}
```

可以看到，`kernel_thread`函数采用了局部变量`tf`来放置保存内核线程的临时中断帧，并把中断帧的指针传递给`do_fork`函数，而`do_fork`函数会调用`copy_thread`函数来在新创建的进程内核栈上专门给进程的中断帧分配一块空间。以下对`tf`的字段设置的值进行一定的解释：

+ 由于这里是创建内核线程，该内核线程中断恢复后仍然是在内核中的运行，因此将`tf.tf_cs`设置为`KERNEL_CS`，`tf.tf_df`设置为`KERNEL_DS`，表示它在内核空间中运行。
+ `tf.tf_eip`表示该内核线程中断恢复后，应该执行的下一条语句。由于它这不还没开始执行吗，因此这里设置为它要执行的第一条语句，即`kern/process/entry.S`中定义的全局函数`kernel_thread_entry`。
+ 转到`kernel_thread_entry`，我们就可以看懂`tf.tf_regs.reg_ebx(edx)`为什么要这样设置了：

```asm
kernel_thread_entry:        # void kernel_thread(void)

    pushl %edx              # push arg
    call *%ebx              # call fn

    pushl %eax              # save the return value of fn(arg)
    call do_exit            # call do_exit to terminate current thread
```

可以看到，在`kernal_thread_entry`中，我们的第一个内核线程是调用`*%ebx`，即`fn`函数，该函数作为`kernel_thread`的参数传入；该`fn`函数的参数，即刚刚被压栈的`%edx`，就是这里作为`kernel_thread`的参数传入的`arg`。至此，在`kernel_thread`的上半部分，就已经设置好了第一个内核线程的中断恢复点，不过此时这些信息还保存在局部变量`tf`中，需要在后续的`copy_thread`函数中，才能将相关信息拷贝到该进程自己的`proc_struct.tf`中。

创建第一个内核线程的接下来的操作都在`do_fork`中完成。

### 为新创建的内核线程分配资源（需要编码）

创建一个内核线程需要分配和设置好很多资源。`kernel_thread`函数通过调用`do_fork`函数完成具体内核线程的创建工作。`do_fork`函数会调用`alloc_proc`函数来分配并初始化一个进程控制块，但`alloc_proc`只是找到了一小块内存用以记录进程的必要信息，并没有实际分配这些资源。`ucore`一般通过`do_fork`实际创建新的内核线程。`do_fork`的作用是，创建当前内核线程的一个副本，它们的执行上下文、代码、数据都一样，但是存储位置不同。在这个过程中，需要给新内核线程分配资源，并且复制原进程的状态。你需要完成在`kern/process/proc.c`中的`do_fork`函数中的处理过程。它的大致执行步骤包括：

+ 调用`alloc_proc`，首先获得一块用户信息块。
+ 为进程分配一个内核栈。
+ 复制原进程的内存管理信息到新进程（但内核线程不必做此事）
+ 复制原进程上下文到新进程
+ 将新进程添加到进程列表
+ 唤醒新进程
+ 返回新进程号

请在实验报告中简要说明你的设计实现过程。请回答如下问题：

+ 请说明ucore是否做到给每个新fork的线程一个唯一的id？请说明你的分析和理由。

> `do_fork`函数的实现

在前面的`alloc_proc`函数中，只是建立的一个新的线程控制块（TCB），并且对其中的字段做了一些预初始化操作，并没有实际地为新建立的线程分配资源。相关的操作留到`do_fork`函数中完成。所以`do_fork`函数的主要功能就是为新建立的线程分配资源，并且设置相关的标识与状态信息。按理说，`do_fork`函数创建的新线程应该是当前线程`current`的一个拷贝，它们应该具有相同的数据，代码，执行上下文等，只有pid标志符不同而已，但是事实好像并不是这样，我将慢慢分析。

`do_fork`函数就是进行资源的分配，这些资源包括

+ 为新的线程控制块分配内存空间，调用`alloc_proc`来完成。
+ 为该线程分配内核地址空间，来作为它的内核堆栈。这个步骤可以通过调用`setup_kstack`来完成。
+ 为新的线程分配虚拟用户地址空间，这个步骤可以通过`copy_mm`来完成。`copy_mm`可以选择新的线程是共享父线程的地址空间（`CLONE_VM`），还是将父线程的地址空间拷贝到一个新的虚拟用户地址空间。当然，由于这里还只有内核线程，它们都没有用户地址空间，这里还不需要关注这个问题。

在分配这些内存空间的时候，都有可能因为内存空间不足而失败，因此应该对失败的情况进行检测，一旦发生失败，即将已经分配的资源归还给操作系统。

此外，需要设置新线程的状态标志，如pid，并且将state转变为就绪态`PROC_RUNNABLE`，然后将新的线程添加到线程队列中，以供操作系统统一管理。至此，已经将`do_fork`的具体实现阐述清楚，代码如下：

```c
int
do_fork(uint32_t clone_flags, uintptr_t stack, struct trapframe *tf) {
    int ret = -E_NO_FREE_PROC;
    struct proc_struct *proc;
    if (nr_process >= MAX_PROCESS) {
        goto fork_out;
    }
    ret = -E_NO_MEM;
	proc = alloc_proc();
    setup_kstack(proc);
    proc->pid = get_pid();
    copy_mm(clone_flags, proc);
    copy_thread(proc, stack, tf);
    hash_proc(proc);
    list_add(&proc_list, &(proc->list_link));
    wakeup_proc(proc);
    ++nr_process;
    ret = proc->pid;
fork_out:
    return ret;

bad_fork_cleanup_kstack:
    put_kstack(proc);
bad_fork_cleanup_proc:
    kfree(proc);
    goto fork_out;
}
```

我这里的代码也是不对的，首先是我忘了设置`proc->parent = current`，还好测试用例里面没有相关的测试......此外，就是没有做失败情况的判断以及资源的回收。这是非常致命的问题啊，操作系统越用内存空间越少，需要引起重视。此外，还有一个问题是参考答案中的某些操作，是取消了中断的，即是不允许被中断的。这个我还没有什么研究啊，不知道为什么要这样进行。

所以，最终把参考答案也贴一下：

```c
int
do_fork(uint32_t clone_flags, uintptr_t stack, struct trapframe *tf) {
    int ret = -E_NO_FREE_PROC;
    struct proc_struct *proc;
    if (nr_process >= MAX_PROCESS) {
        goto fork_out;
    }
    ret = -E_NO_MEM;
    if ((proc = alloc_proc()) == NULL) {
        goto fork_out;
    }

    proc->parent = current;

    if (setup_kstack(proc) != 0) {
        goto bad_fork_cleanup_proc;
    }
    if (copy_mm(clone_flags, proc) != 0) {
        goto bad_fork_cleanup_kstack;
    }
    copy_thread(proc, stack, tf);

    bool intr_flag;
    local_intr_save(intr_flag);
    {
        proc->pid = get_pid();
        hash_proc(proc);
        list_add(&proc_list, &(proc->list_link));
        nr_process ++;
    }
    local_intr_restore(intr_flag);

    wakeup_proc(proc);

    ret = proc->pid;
fork_out:
    return ret;

bad_fork_cleanup_kstack:
    put_kstack(proc);
bad_fork_cleanup_proc:
    kfree(proc);
    goto fork_out;
}
```

> 每个新`fork`的线程是否获得唯一的id？

的确是的，新创建的线程，是通过`get_pid`函数，来生成一个新的pid的。并且由于`MAX_PID > MAX_PROCESS`，即可用的pid数量大于可以存在的进程数，每个线程都可以获得一个唯一的pid。具体的可以查看`get_pid`函数的具体实现。

## 练习3：阅读代码，理解`proc_run`函数和它调用的函数如何完成进程切换的。（无编码工作）

请在实验报告中简要说明你对`proc_run`函数的分析。并回答如下问题：

+ 在本实验的执行过程中，创建且运行了几个内核线程？
+ 语句`local_intr_save(intr_flag);....local_intr_restore(intr_flag);`在这里有何作用?请说明理由

### 线程切换过程分析

第一个内核线程创建好了以后，第零个内核线程就可以通过调用调度函数`schedule`来将CPU的控制权交给第一个内核线程了。接下来，就是慢慢分析这个过程。

> 调度函数`schedule`

`schedule`的主要功能，就是根据预先设置好的进程调度算法（这里是先入先出的`FIFO`算法），来从就绪队列中选择一个进程，并且调用`proc_run`函数使该进程开始运行。

> `proc_run`函数

`proc_run`函数的功能是可以使一个线程开始在CPU上运行。为了让CPU开始运行一个新的线程，需要做哪些工作呢？

首先，运行一个新的线程，肯定需要更新`ss:eip`寄存器，使其指向新的线程要执行的下一条语句，对于还未投入运行的线程，即指向它的第一条语句；对于执行了一段时间被调度出CPU的线程，就是指向它上一次执行的中断点。此外，其他的寄存器也要做相应的更新，更新为该线程上一次执行被中断时的情形，即恢复它的执行上下文。所有这些信息都保存在`proc_struct.context`里面。

除了一些寄存器的信息以外，运行一个新的线程，还需要切换到该线程的用户空间堆栈中，为此需要更新`cr3`寄存器的值为该线程的`proc_struct.cr3`。除了用户空间，该线程应该还具有一个虚拟内核空间，用来保存中断时所需要的信息，线程的切换还需要加载该线程的虚拟内核空间，`ts.esp0`即使保存这个信息的寄存器。

最后，将`current`赋值为被切换的进程，标志进程切换完成。至此，我们已经分析出`proc_run`的主要实现步骤，具体的代码如下：

```c
// proc_run - make process "proc" running on cpu
// NOTE: before call switch_to, should load  base addr of "proc"'s new PDT
void
proc_run(struct proc_struct *proc) {
    if (proc != current) {
        bool intr_flag;
        struct proc_struct *prev = current, *next = proc;
        local_intr_save(intr_flag);
        {
            current = proc;
            load_esp0(next->kstack + KSTACKSIZE);
            lcr3(next->cr3);
            switch_to(&(prev->context), &(next->context));
        }
        local_intr_restore(intr_flag);
    }
}
```

由于在做线程切换时，不允许被调度的，否则会产生进程的嵌套调度，这里的操作需要首先关闭中断。可以看到，其中的语句就是上面分析的那些，其中的上下文切换在`switch_to`中实现。

> `switch_to`的实现

`switch_to`是完成了进程上下文的切换。由于我们说上下文的切换主要就是更新一些寄存器里的值，都是比较底层的操作，因此`switch_to`用汇编代码来实现。具体的代码如下：

```asm
switch_to:                      # switch_to(from, to)

    # save from's registers
    movl 4(%esp), %eax          # eax points to from
    popl 0(%eax)                # save eip !popl
    movl %esp, 4(%eax)
    movl %ebx, 8(%eax)
    movl %ecx, 12(%eax)
    movl %edx, 16(%eax)
    movl %esi, 20(%eax)
    movl %edi, 24(%eax)
    movl %ebp, 28(%eax)

    # restore to's registers
    movl 4(%esp), %eax          # not 8(%esp): popped return address already
                                # eax now points to to
    movl 28(%eax), %ebp
    movl 24(%eax), %edi
    movl 20(%eax), %esi
    movl 16(%eax), %edx
    movl 12(%eax), %ecx
    movl 8(%eax), %ebx
    movl 4(%eax), %esp

    pushl 0(%eax)               # push eip

    ret
```

`switch_to`包含两部分的内容，即保存被切换的线程的上下文，以及将新的线程的上下文加载到寄存器中。它们两个的上下文信息保存在`4(%esp)`和`8(%esp)`中，因为之前是函数调用`switch_to(&(prev->context), &(next->context))`的参数，在`eip`之前被压入到栈中。还需要注意的是，这里压入栈的并不是整个`context`结构体，而是两个`context`结构体的地址，因此后面都是利用这个地址做内存访问。

还需要注意的点是，在将`context.eip`压入栈中后`pushl 0(%eax)`，`switch_to`调用了`ret`。这其实是手动模拟了函数调用返回，因为在执行`ret`命令时，硬件会将刚刚压栈的`0(%eax)`弹栈到`eip`中，随后就跳转到`cs:eip`指示的位置去执行。那么问题来了，第一个内核线程的`context.eip`是被设置为了多少呢？

这个可以在`copy_thread`中找到答案，其代码如下：

```c
static void
copy_thread(struct proc_struct *proc, uintptr_t esp, struct trapframe *tf) {
    proc->tf = (struct trapframe *)(proc->kstack + KSTACKSIZE) - 1;
    *(proc->tf) = *tf;
    proc->tf->tf_regs.reg_eax = 0;
    proc->tf->tf_esp = esp;
    proc->tf->tf_eflags |= FL_IF;

    proc->context.eip = (uintptr_t)forkret;
    proc->context.esp = (uintptr_t)(proc->tf);
}
```

可以看到，在`copy_thread`中，首先是将当前线程的中断帧`tf`设置在了其内核堆栈的顶部，并且将此前设置的`tf`拷贝到了线程自己的中断帧中。此外，第一个内核线程的`context.eip`是被设置为了`forkret`，`forkret`转而调用了`forkrets`，这个函数可以在`kern/trap/trapentry.S`中找到。

> `forkrets`

在`forkrets`中的操作非常简单，代码如下：

```asm
forkrets:
    # set stack to this new process's trapframe
    movl 4(%esp), %esp
    jmp __trapret
```

即将栈顶指针指向我们已经设置好了的`proc_struct.tf`，然后跳转到`__trapret`，这里的思想和前面手动模拟函数调用返回一样，只不过这里是手动模拟了中断返回。此后，在`__trapret`中，就可以通过中断返回，将程序的控制权转交到中断帧中所指示的`ss:eip`，并且将堆栈切换到中断帧设置好了的用户堆栈（如果是用户线程的话）。这样，中断返回后，执行的语句就是`tf`中设置好了的`kernel_thread_entry`了，在前面我们已经对它进行了分析。至此，已经完成了CPU控制权的切换。

> 线程切换总结

线程的切换，主要有依赖`proc_struct.context`和`proc_struct.tf`，即上下文信息和中断帧信息。首先通过上下文信息，跳转到了中断返回函数，在其中通过模拟中断返回机制，使程序的控制权最终转交到中断帧中所指示的位置。值得注意的是，这是线程第一个投入运行时的情况，在经过调度后重新恢复运行时，只需要上下文信息`context`就足够了，因为在此前的被切换的过程中，`context`已经保存了恢复的地址和状态信息了。
