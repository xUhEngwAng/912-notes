Lab5 Report
===========

## 实验目的

+ 了解第一个用户进程创建过程
+ 了解系统调用框架的实现机制
+ 了解ucore如何实现系统调用`sys_fork/sys_exec/sys_exit/sys_wait`来进行进程管理

## 实验内容

实验4完成了内核线程，但到目前为止，所有的运行都在内核态执行。实验5将创建用户进程，让用户进程在用户态执行，且在需要`ucore`支持时，可通过系统调用来让`ucore`提供服务。为此需要构造出第一个用户进程，并通过系统调用`sys_fork/sys_exec/sys_exit/sys_wait`来支持运行不同的应用程序，完成对用户进程的执行过程的基本管理。相关原理介绍可看附录B。

## 练习

对实验报告的要求：

+ 基于markdown格式来完成，以文本方式为主
+ 填写各个基本练习中要求完成的报告内容
+ 完成实验后，请分析`ucore lab`中提供的参考答案，并请在实验报告中说明你的实现与参考答案的区别
+ 列出你认为本实验中重要的知识点，以及与对应的OS原理中的知识点，并简要说明你对二者的含义，关系，差异等方面的理解（也可能出现实验中的知识点没有对应的原理知识点）
+ 列出你认为OS原理中很重要，但在实验中没有对应上的知识点

## 实验执行流程概述

首先简单叙述一下lab5整个实验的执行流程，以及它与lab4之间的区别。

一开始的执行流程都和lab4一样，同样的初始化工作，在`proc_init`中创建了第零个内核线程`idleproc`，以及在`idleproc`线程中创建了第一个内核线程`init_main`。最后，`idleproc`通过调用`cpu_idle`函数将CPU的控制权切换到`init_main`。

切换到`init_main`后，lab4中只是简单地打印了一些内容，然后就结束退出了。但是在lab5中，`init_main`的工作就没有这么简单了。它又调用了`kernel_thread`创建了第二个内核线程`user_main`，并且通过调用调度函数将控制权切换到`user_main`。

在`user_main`中，进行的主要工作是将这个内核线程打造成一个用户进程。为此，它调用了`do_execve`函数，其中首先将当前进程的内存资源清空，然后在`load_icode`函数中将要执行的用户程序加载到了当前进程的内存空间中，并且进行了其他一些资源分配工作。最后通过中断帧`tf`的设置，实现了CPU从内核空间向用户空间的切换。

下面将就上面的这些步骤，给出具体的分析。

## 用户进程的加载与执行

`user_main`内核线程中，首先是通过宏调用了`kernel_execve`函数，其函数体如下：

```c
static int
kernel_execve(const char *name, unsigned char *binary, size_t size) {
    int ret, len = strlen(name);
    asm volatile (
        "int %1;"
        : "=a" (ret)
        : "i" (T_SYSCALL), "0" (SYS_exec), "d" (name), "c" (len), "b" (binary), "D" (size)
        : "memory");
    return ret;
}
```

在该函数中，是通过系统调用`INT 80`调用了`do_execve`函数，关于系统调用的实现将在后面具体分析。`do_execve`函数的主要功能，就是将一个用户程序的`elf`格式文件加载到当前进程的内存空间中，并且让该用户程序在用户空间运行起来，整个过程非常类似`bootloader`加载ucore内核到内存中的情形。实际上，这也是原理课中讲过的，`linux`系统中，新建一个用户进程的常见方法，即首先调用`fork`系统调用，在`fork`系统调用中，会新建一个进程，并且把当前进程的内存空间，代码，状态等信息拷贝到新建的进程中，这样父进程和子进程几乎一模一样。随后，用户为了让新进程执行指定的代码，需要调用`exec`系统调用，来将指定的程序加载到新创建的进程中。

> `do_execve`为了实现其功能，需要进行的操作？

`do_execve`为了将指定的程序加载到刚刚`fork`产生的新进程中，首先肯定是需要释放现有的内存空间，因为现有的内存空间都是复制当前进程的父进程的，对于该进程后续的执行没有任何作用，因此首先需要将这部分不需要的资源归还给操作系统，这部分操作对应了`do_execve`函数的前半部分，具体的代码如下：

```c
int
do_execve(const char *name, size_t len, unsigned char *binary, size_t size) {
......
if (mm != NULL) {
        lcr3(boot_cr3);
        if (mm_count_dec(mm) == 0) {
            exit_mmap(mm);
            put_pgdir(mm);
            mm_destroy(mm);
        }
        current->mm = NULL;
    }
    int ret;
    if ((ret = load_icode(binary, size)) != 0) {
        goto execve_exit;
    }
......
```

其中的`exit_mmap`，`put_pgdir`，`mm_destroy`依次对应了释放进程的用户地址空间，释放进程的页表占用的空间，释放进程的`mm_struct`占用的空间。需要注意的是这里还有一个`mm_count_dec`函数，是将`mm_struct.mm_count`递减，该字段用来表示使用该`mm_struct`的进程数量，因为多个进程可以共用同一个页表以及虚拟地址空间。

清空了内存空间以后，就可以要求操作系统分配新的内存空间，以将当前进程要执行的程序加载到新的内存空间中。这部分的操作类似于`bootloader`中将内核加载到内存中的过程，主要原理就是利用`elf`文件的头部，将程序的各个段依次读出，并且拷贝到内存空间中，对应了`load_icode`大部分的操作，因为太长，并且涉及到比较底层的知识，这里就不贴代码了，可以自己去查看一下。

在将要执行的用户代码加载到它的用户内存空间后，就可以让它运行起来了。由于是用户进程，在此之前还需要为它分配它的用户栈，代码如下：

```c
//(4) build user stack memory
vm_flags = VM_READ | VM_WRITE | VM_STACK;
if ((ret = mm_map(mm, USTACKTOP - USTACKSIZE, USTACKSIZE, vm_flags, NULL)) != 0) {
    goto bad_cleanup_mmap;
}
assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-PGSIZE , PTE_USER) != NULL);
assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-2*PGSIZE , PTE_USER) != NULL);
assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-3*PGSIZE , PTE_USER) != NULL);
assert(pgdir_alloc_page(mm->pgdir, USTACKTOP-4*PGSIZE , PTE_USER) != NULL);
```

可以看到，这里的用户栈的栈底是`USTACKTOP`，它实际上就是用户虚拟内存空间的顶部`USERTOP`，用户栈的大小是`USTACKSZIE`，为256个页面大小，并且预先为该进程分配了4个物理页帧作为它的用户栈。之前我还以为是所有用户进程一起使用这256页大小的用户栈空间，后来才想明白每个用户进程的栈都是从`USTACKTOP`起始，大小为256个页面大小。因为这里使用的都是虚拟地址啊，并且引入了页面置换机制，所以尽管不同的用户进程的用户栈拥有相同的虚拟地址，它们却会被映射到不同的物理地址上。这样一来，我就明白为什么会有栈溢出这么一回事儿了，因为物理内存空间是有限的，多道程序系统又会同时运行许多应用程序，如果每个应用程序都占用过大的栈空间，CPU的并行性或者效率就会降低，所以需要为用户栈的大小设置一个上限，一旦超过了这个上限，比如建立了一个大数组，当然就会出现`stack overflow`了。

此后，就需要正确设置中断帧`tf`，利用中断返回机制让用户程序进入运行了。

###  加载应用程序并执行（需要编码）

`do_execv`函数调用`load_icode`（位于`kern/process/proc.c`中）来加载并解析一个处于内存中的`ELF`执行文件格式的应用程序，建立相应的用户内存空间来放置应用程序的代码段、数据段等，且要设置好`proc_struct`结构中的成员变量`trapframe`中的内容，确保在执行此进程后，能够从应用程序设定的起始执行地址开始执行。需设置正确的`trapframe`内容。

请在实验报告中简要说明你的设计实现过程。

请在实验报告中描述当创建一个用户态进程并加载了应用程序后，CPU是如何让这个应用程序最终在用户态执行起来的。即这个用户态进程被`ucore`选择占用CPU执行（`RUNNING`态）到具体执行应用程序第一条指令的整个经过

> 中断帧的设置

不知道还有没有人记得，一开始我就描述了`user_main`内核线程是通过系统调用`INT 80`，来调用`do_execve`函数的，它为什么非要通过系统调用呢？要知道这可是一个内核线程啊，它可以直接调用`do_execve`的。因此，它通过系统调用的目的，其实就是为了最后这一步内核线程向用户进程的切换。

`user_main`是通过系统调用来调用`do_execve`函数的，因此它当前的中断帧其实就是在中断机制中，由硬件和操作系统所设置的，设置这个中断帧的目的是在中断返回时，可以正确返回到应用进程被中断的那一条指令。所以，我们只需要修改这里的中断帧，就可以在`iret`时，不要回到`user_main`函数中，而是跳转到前面我们已经加载到内存中的用户程序中，这样就可以实现用户应用程序被操作系统执行。

这样，我们这里的主要工作，就是要正确设置中断帧，让它表示用户程序的第一条指令。其中包括：

+ 段寄存器的设置：由于是用户进程，应该正确设置段选择子中的`CPL`了，简单说来就是让这里的`CS = USER_CS`，`DS = SS = ES = USER_DS`。
+ `ip`的设置：`cs:ip`应该指向用户进程的第一条指令，即`ip = elf->e_entry`。
+ `esp`的设置：只有在涉及到特权级切换时，才会由硬件压栈的用户栈栈顶指针`esp`，此时应该指向刚设置的用户栈的栈顶，即`esp = USTACKTOP`。
+ 状态字`flags`的设置：由于刚开始执行，也没有什么保存的状态，就设置为可以被中断`FL_IF`就可以了。

完全这样的设置后，只需要执行到中断返回时的`iret`，就可以实现用户应用程序的执行了。具体的代码如下：

```c
	//(6) setup trapframe for user environment
	struct trapframe *tf = current->tf;
	memset(tf, 0, sizeof(struct trapframe));
	tf->tf_cs = USER_CS;
	tf->tf_ds = USER_DS;
	tf->tf_es = USER_DS;
	tf->tf_ss = USER_DS;
	tf->tf_esp = USTACKTOP;
	tf->tf_eip = elf->e_entry;
	tf->tf_eflags = FL_IF;
	ret = 0;
out:
	return ret;
```

至此，lab5的基本执行流程就已经分析完了，后面就其中的一些函数与功能的具体实现来进行分析。

## 练习2: 父进程复制自己的内存空间给子进程（需要编码）

创建子进程的函数`do_fork`在执行中将拷贝当前进程（即父进程）的用户内存地址空间中的合法内容到新进程中（子进程），完成内存资源的复制。具体是通过`copy_range`函数（位于`kern/mm/pmm.c`中）实现的，请补充`copy_range`的实现，确保能够正确执行。

请在实验报告中简要说明如何设计实现”Copy on Write 机制“，给出概要设计，鼓励给出详细设计。

> Copy-on-write（简称COW）的基本概念是指如果有多个使用者对一个资源A（比如内存块）进行读操作，则每个使用者只需获得一个指向同一个资源A的指针，就可以该资源了。若某使用者需要对这个资源A进行写操作，系统会对该资源进行拷贝操作，从而使得该“写操作”使用者获得一个该资源A的“私有”拷贝—资源B，可对资源B进行写操作。该“写操作”使用者对资源B的改变对于其他的使用者而言是不可见的，因为其他使用者看到的还是资源A。

### 用户内存空间的复制

在`do_fork`中，需要完成将父进程的用户内存空间复制到子进程中，它通过调用`mm_copy`来完成这个工作。在`mm_copy`中，存在两个模式，一种是父子进程共享(share)内存空间，在这种情况下，只需要将父进程的`mm`赋值给子进程的`mm`就可以了；另一种是拷贝内存空间(duplicate)，也就是我们这里的情况，是通过调用`dup_mmap`函数来实现的。

为了实现内存空间的拷贝，需要做哪些工作呢？内存空间的拷贝包括虚拟内存空间拷贝和物理内存空间拷贝。实际上，完全可以不用拷贝虚拟内存空间，但是既然`do_fork`的语义是`fork`，似乎还是保持父子进程几乎完全相同比较好；此外，反正虚拟内存空间都是每个进程所独有的，与其他进程没有任何关系，其实怎么设置都不是很有关系，既然如此不如直接拷贝还更加方便。

所以第一步的操作就是虚拟内存空间的拷贝。为此，只需要将父进程的`mm`中的各个`vma_struct`，依次拷贝到子进程新建的`mm`中，这部分内容在`dup_mmap`函数中的前半部分实现。具体的代码如下：

```c
int
dup_mmap(struct mm_struct *to, struct mm_struct *from) {
    assert(to != NULL && from != NULL);
    list_entry_t *list = &(from->mmap_list), *le = list;
    while ((le = list_prev(le)) != list) {
        struct vma_struct *vma, *nvma;
        vma = le2vma(le, list_link);
        nvma = vma_create(vma->vm_start, vma->vm_end, vma->vm_flags);
        if (nvma == NULL) {
            return -E_NO_MEM;
        }

        insert_vma_struct(to, nvma);

        bool share = 0;
        if (copy_range(to->pgdir, from->pgdir, vma->vm_start, vma->vm_end, share) != 0) {
            return -E_NO_MEM;
        }
    }
    return 0;
}
```

可以看到，在`dup_mmap`中，就是遍历父进程`mm`中的各个`vma_struct`，将它们逐个拷贝给子进程。随后调用`copy_range`函数来实现物理内存空间的拷贝。

为了实现物理内存空间的拷贝，只需要使用`get_pte`函数遍历父进程的每一个物理页帧，每有一个这样的物理页帧，就为子进程分配一个新的内存空间，并且将该父进程页帧中的内容通过`memcpy`拷贝到子进程新分配的空间中。最后，在将子进程新分配的页帧的起始地址填入到子进程的页表中，这样就完成了一个物理页帧的拷贝。具体的代码如下：

```c
int
copy_range(pde_t *to, pde_t *from, uintptr_t start, uintptr_t end, bool share) {
    assert(start % PGSIZE == 0 && end % PGSIZE == 0);
    assert(USER_ACCESS(start, end));
    // copy content by page unit.
    do {
        //call get_pte to find process A's pte according to the addr start
        pte_t *ptep = get_pte(from, start, 0), *nptep;
        if (ptep == NULL) {
            start = ROUNDDOWN(start + PTSIZE, PTSIZE);
            continue ;
        }
        //call get_pte to find process B's pte according to the addr start. If pte is NULL, just alloc a PT
        if (*ptep & PTE_P) {
            if ((nptep = get_pte(to, start, 1)) == NULL) {
                return -E_NO_MEM;
            }
        uint32_t perm = (*ptep & PTE_USER);
        //get page from ptep
        struct Page *page = pte2page(*ptep);
        // alloc a page for process B
        struct Page *npage=alloc_page();
        assert(page!=NULL);
        assert(npage!=NULL);
        int ret=0;
        void* src_kvaddr = page2kva(page);
        void* dst_kvaddr = page2kva(npage);
        memcpy(dst_kvaddr, src_kvaddr, PGSIZE);
        page_insert(to, npage, start, perm);
        assert(ret == 0);
        }
        start += PGSIZE;
    } while (start != 0 && start < end);
    return 0;
}
```

### `COW`机制实现

简单的版本的话，就是在`do_fork`函数中，只是让子进程拷贝父进程的地址空间。一旦父进程或者子进程试图写它们共享的空间，可以通过`Page Fault`机制，为该页新分配一个空间，并且修改对应进程的页表。这样，被修改的页就只有修改页的那个进程可以看到，对于其他进程修改都是不可见的。当然，要是具体实现的话，需要考虑许多问题，情况比较复杂，我以后再好好专研吧。
