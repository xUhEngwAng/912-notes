Lab 3 Report
============

## 实验目的

+ 了解虚拟内存的Page Fault异常处理实现
+ 了解页替换算法在操作系统中的实现

## 实验内容

本次实验是在实验二的基础上，借助于页表机制和实验一中涉及的中断异常处理机制，完成Page Fault异常处理和FIFO页替换算法的实现，结合磁盘提供的缓存空间，从而能够支持虚存管理，提供一个比实际物理内存空间“更大”的虚拟内存空间给系统使用。这个实验与实际操作系统中的实现比较起来要简单，不过需要了解实验一和实验二的具体实现。实际操作系统系统中的虚拟内存管理设计与实现是相当复杂的，涉及到与进程管理系统、文件系统等的交叉访问。如果大家有余力，可以尝试完成扩展练习，实现extended　clock页替换算法。

## 练习

对实验报告的要求：

+ 基于markdown格式来完成，以文本方式为主
+ 填写各个基本练习中要求完成的报告内容
+ 完成实验后，请分析ucore_lab中提供的参考答案，并请在实验报告中说明你的实现与参考答案的区别
+ 列出你认为本实验中重要的知识点，以及与对应的OS原理中的知识点，并简要说明你对二者的含义，关系，差异等方面的理解（也可能出现实验中的知识点没有对应的原理知识点）
+ 列出你认为OS原理中很重要，但在实验中没有对应上的知识点

## 新增加的文件概述

lab3相对于前面的lab2，增加了虚拟内存的管理，为此lab3中增加了一些关键文件。首先对这些文件的功能做一个简单的说明，后面还会有更详细的分析。

为了进行对虚拟内存的管理，需要有相关数据结构的支持，其中的两个关键数据结构`struct mm_struct`， `struct vma_struct`以及相关的函数定义在`mm/vmm.c(h)`当中。通过这两个结构体，`ucore`模拟了一个用户进程的虚拟地址空间，并对其进行相关的测试工作。

在`mm/swap.c(h)`中，定义了页面置换算法的框架`struct smap_manager`，以及为了完成页面换入换出所需要的一些必要函数，还有相关的测试代码。

在`mm/swap_fifo.c(h)`中，利用`struct swap_manager`框架实现了先进先出置换算法`FIFO`（其实没有实现，需要自己编程实现）。

此外，为了将页面换入换出到外存上，需要支持对外存的读写操作，这些函数定义在了`driver/ide.c`当中。

## 虚拟内存管理

lab3中需要完成对虚拟内存的管理工作。上面已经说到，虚拟内存的管理是通过两个结构体`mm_struct`以及`vmm_struct`来实现的，下面主要阐述这两个结构体如何实现对虚拟内存的管理。

### `vma_struct`

`vma_struct`是用于描述一个进程的连续虚拟内存空间的，如果一个进程有若干个相互之间不连续的虚拟内存空间，则每个虚拟内存空间都需要维护一个`vma_struct`结构体。`vma_struct`的结构如下：

```c
// the virtual continuous memory area(vma), [vm_start, vm_end), 
// addr belong to a vma means  vma.vm_start<= addr <vma.vm_end 
struct vma_struct {
    struct mm_struct *vm_mm; // the set of vma using the same PDT 
    uintptr_t vm_start;      // start addr of vma      
    uintptr_t vm_end;        // end addr of vma, not include the vm_end itself
    uint32_t vm_flags;       // flags of vma
    list_entry_t list_link;  // linear list link which sorted by start addr of vma
};
```

可以看到，其中的`vm_start`以及`vm_end`分别描述该连续虚拟内存空间的起始虚拟地址和结束虚拟地址，一个进程的各个`vma_struct`之间通过`vma_struct::list_link`连接起来，构成了整个进程的虚拟内存空间。

### `mm_struct`

`mm_struct`可以被认为是进程控制块(PCB, Process Control Block)之类的结构，一个`mm_struct`代表了一个进程。`mm_struct`的结构如下：

```c
// the control struct for a set of vma using the same PDT
struct mm_struct {
    list_entry_t mmap_list;        // linear list link which sorted by start addr of vma
    struct vma_struct *mmap_cache; // current accessed vma, used for speed purpose
    pde_t *pgdir;                  // the PDT of these vma
    int map_count;                 // the count of these vma
    void *sm_priv;                 // the private data for swap manager
};
```

可以看到，`mm_struct`维护了一个页目录表`mm_struct::pgdir`，它是当前进程的页目录表，将当前进程的虚拟地址映射到物理地址上。此外，`mm_struct::mmap_list`其实是`vma_struct`的链表的头节点，通过这个`mmap_list`可以将当前进程的各个虚拟地址空间连接起来，并且用`map_count`来指示这些虚拟地址空间的数量。

此外，需要注意的是，`mm_struct`还维护了一个`mmap_cache`变量，这其实保存的是当前进程上一次访问的连续虚拟地址空间。根据局部性原理，该进程接下来极有可能再次访问该虚拟地址空间，从而可以在下次访问它时，可以直接获得对应的`vma_struct`，而不需要遍历`vma_struct`链表来找到它。

通过对虚拟地址空间的管理，操作系统可以完成一系列的工作，例如检查当前进程所要访问的虚拟地址是否合法。检查的方法是在该进程的虚拟地址空间查找要访问的虚拟地址，如果的确在其中，则说明该地址是一个合法的访问地址，否则就会触发页访问异常。相关的代码如下：

```c
// find_vma - find a vma  (vma->vm_start <= addr <= vma_vm_end)
struct vma_struct *
find_vma(struct mm_struct *mm, uintptr_t addr) {
    struct vma_struct *vma = NULL;
    if (mm != NULL) {
        vma = mm->mmap_cache;
        if (!(vma != NULL && vma->vm_start <= addr && vma->vm_end > addr)) {
                bool found = 0;
                list_entry_t *list = &(mm->mmap_list), *le = list;
                while ((le = list_next(le)) != list) {
                    vma = le2vma(le, list_link);
                    if (vma->vm_start<=addr && addr < vma->vm_end) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    vma = NULL;
                }
        }
        if (vma != NULL) {
            mm->mmap_cache = vma;
        }
    }
    return vma;
}
```

查找的方法就是从`mm_struct::mmap_cache`开始遍历整个`vma_struct`链表，直到找到一个对应的虚拟地址空间，或者始终没有找到。

## 练习一：给未被映射的地址映射上物理页（需要编程）

完成`do_pgfault`（`mm/vmm.c`）函数，给未被映射的地址映射上物理页。设置访问权限的时候需要参考页面所在`VMA` 的权限，同时需要注意映射物理页时需要操作内存控制结构所指定的页表，而不是内核的页表。注意：在`LAB2 EXERCISE 1`处填写代码。执行

```
make qemu
```

后，如果通过check_pgfault函数的测试后，会有“check_pgfault() succeeded!”的输出，表示练习1基本正确。

请在实验报告中简要说明你的设计实现过程。请回答如下问题：

+ 请描述页目录项（Pag Director Entry）和页表（Page Table Entry）中组成部分对ucore实现页替换算法的潜在用处。
+ 如果ucore的缺页服务例程在执行过程中访问内存，出现了页访问异常，请问硬件要做哪些事情？


### 页访问异常的处理

产生页访问异常的原因有很多，比如

+ 目标页帧不存在，即页表项全为零，表示没有给虚拟页分配物理页帧。
+ 相应的物理页帧不在内存当中，而是在磁盘的`swap`分区。此时页表项的`PTE_P`标志位为零。
+ 访问权限出错，比如试图写只读的页，或者用户进程试图访问内核的地址空间。

所有这些情况都会产生页访问异常(Page Fault, PF)，因此页访问异常的处理函数首先需要解决的问题，就是如何来分辨当前页访问异常产生的原因，从而根据不同的情况采取不同的解决方法。

> 如何辨别不同原因产生的页访问异常？

在发生页访问异常时，CPU会将产生异常的线性地址存储在`CR2`寄存器中，并且把页访问异常类型的值（简称页访问异常错误码，errorCode）保存在中断栈中。因此，可以通过这两个信息来追溯页访问异常的原因。

lab3中采用的方法是将这两个信息接合起来考虑，首先根据错误码来定位某些原因引起的页访问异常，再利用`CR2`寄存器中的线性地址，通过手动查当前进程的页目录表，来进一步对页访问异常进行定位。页访问异常错误的结构如下所示：

![page_fault_errorcode](images/page_fault_errorcode.png)

我们这里只用到了第零位和第一位，毕竟目前还没有用户态，第二位的内核访问/用户访问也无从说起。根据错误码来对页访问异常进行辨别的代码如下所示：

```c
int
do_pgfault(struct mm_struct *mm, uint32_t error_code, uintptr_t addr) {
    int ret = -E_INVAL;
    //try to find a vma which include addr
    struct vma_struct *vma = find_vma(mm, addr);

    pgfault_num++;
    //If the addr is in the range of a mm's vma?
    if (vma == NULL || vma->vm_start > addr) {
        cprintf("not valid addr %x, and  can not find it in vma\n", addr);
        goto failed;
    }
    //check the error_code
    switch (error_code & 3) {
    default:
            /* error code flag : default is 3 ( W/R=1, P=1): write, present */
    case 2: /* error code flag : (W/R=1, P=0): write, not present */
        if (!(vma->vm_flags & VM_WRITE)) {
            cprintf("do_pgfault failed: error code flag = write AND not present, but the addr's vma cannot write\n");
            goto failed;
        }
        break;
    case 1: /* error code flag : (W/R=0, P=1): read, present */
        cprintf("do_pgfault failed: error code flag = read AND present\n");
        goto failed;
    case 0: /* error code flag : (W/R=0, P=0): read, not present */
        if (!(vma->vm_flags & (VM_READ | VM_EXEC))) {
            cprintf("do_pgfault failed: error code flag = read AND not present, but the addr's vma cannot read or exec\n");
            goto failed;
        }
    }
    // other operations
    //......
```

可以看到，根据异常错误码，主要是对一些权限信息进行检查。例如当前进程是否在试图写一个不可写的页面，或者被访问的页面是否是不可读也是不可执行的，如果的确出现了权限的越界，操作系统应该终止这些操作的进行，并且直接返回错误信息。而如果所有权限信息都是正确的，发生页访问异常的原因只是当前页面不存在(`P = 0`)，则需要进行后续的操作，以判断是进行内存分配，还是页面置换。此时，就需要结合存储在`CR2`寄存器中的线性地址来进行进一步的判断了。

前面已经提到过，具体的方法就是查询当前进程的页目录表(`mm_struct::pgdir`)，通过得到的页表项来进行判断--如果页表项全为零，则表示不存在当前虚拟地址的映射关系，此时应该给当前的虚拟页分配一个新的页面；而如果页表项不为零，只是`PTE_P`为零，则表示要访问的页不在内存中，需要进行页面置换，这部分将在练习二中讨论。

### 给未被映射的地址映射上物理页

如果访问的地址尚未被映射到物理页，则在页访问异常的处理函数`do_pgfault`中，需要给该也分配一个新的物理页帧，并且将分配的物理页的地址以及相关的权限信息填入到页表项中，这和我们前面的内容是一致的。

但是这里需要考虑特殊的情况，假如当前内存中已经没有空闲的页面了，又应该如何操作呢？在lab2中我们是直接返回错误码，然后就退出了，但是这在引入了虚拟内存地址的现在显然是不合适的。因此，此时是需要调用页面置换算法，从当前进程的页面中，选择一个置换到外存中，随后再进行存储空间的分配。然后这一系列的操作，老师都已经封装成的一个函数，见下面的代码：

```c
	
	uint32_t perm = PTE_U;
    if (vma->vm_flags & VM_WRITE) {
        perm |= PTE_W;
    }
    addr = ROUNDDOWN(addr, PGSIZE);
    ret = -E_NO_MEM;
    pte_t *ptep=NULL;

    /*LAB3 EXERCISE 1: YOUR CODE*/
    ptep = get_pte(mm->pgdir, addr, 1);                   //(1) try to find a pte, if pte's PT(Page Table) isn't existed, then create a PT.
    if (*ptep == 0) {
        pgdir_alloc_page(mm->pgdir, addr, perm | PTE_P);  //(2) if the phy addr isn't exist, then alloc a page & map the phy addr with logical addr
    }
```

这里的`pgdir_alloc_page`函数，已经实现了上面我所描述的功能。在它的内部，是调用了`alloc_pages`函数，其实现如下：

```c
//alloc_pages - call pmm->alloc_pages to allocate a continuous n*PAGESIZE memory 
struct Page *
alloc_pages(size_t n) {
    struct Page *page=NULL;
    bool intr_flag;
    
    while (1)
    {
         local_intr_save(intr_flag);
         {
              page = pmm_manager->alloc_pages(n);
         }
         local_intr_restore(intr_flag);

         if (page != NULL || n > 1 || swap_init_ok == 0) break;
         
         extern struct mm_struct *check_mm_struct;
         swap_out(check_mm_struct, n, 0);
    }
    return page;
}
```

可以看到，在`alloc_pages`函数内部，已经包含了页面置换的功能。

我的exercise1的实现，和老师的大体相同吧，但是我的肯定还是有问题的，老师的代码主要是多了很多异常的检查工作，比我的代码健壮多了，见下：

```c
	// try to find a pte, if pte's PT(Page Table) isn't existed, then create a PT.
    // (notice the 3th parameter '1')
    if ((ptep = get_pte(mm->pgdir, addr, 1)) == NULL) {
        cprintf("get_pte in do_pgfault failed\n");
        goto failed;
    }
    
    if (*ptep == 0) { // if the phy addr isn't exist, then alloc a page & map the phy addr with logical addr
        if (pgdir_alloc_page(mm->pgdir, addr, perm) == NULL) {
            cprintf("pgdir_alloc_page in do_pgfault failed\n");
            goto failed;
        }
    }
```
