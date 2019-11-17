Lab 1 Report
============

## 实验目的

操作系统是一个软件，也需要通过某种机制加载并运行它。在这里我们将通过另外一个更加简单的软件-bootloader来完成这些工作。为此，我们需要完成一个能够切换到x86的保护模式并显示字符的bootloader，为启动操作系统ucore做准备。lab1提供了一个非常小的bootloader和ucore OS，整个bootloader执行代码小于512个字节，这样才能放到硬盘的主引导扇区中。通过分析和实现这个bootloader和ucore OS，读者可以了解到：

+ 计算机原理

	- CPU的编址与寻址: 基于分段机制的内存管理
	- CPU的中断机制
	- 外设：串口/并口/CGA，时钟，硬盘
	- Bootloader软件

+ 编译运行bootloader的过程
	- 调试bootloader的方法
	- PC启动bootloader的过程
	- ELF执行文件的格式和加载
	- 外设访问：读硬盘，在CGA上显示字符串
	- ucore OS软件

+ 编译运行ucore OS的过程
	- ucore OS的启动过程
	- 调试ucore OS的方法
	- 函数调用关系：在汇编级了解函数调用栈的结构和处理过程
	- 中断管理：与软件相关的中断处理
	- 外设管理：时钟

## 实验内容

lab1中包含一个bootloader和一个OS。这个bootloader可以切换到X86保护模式，能够读磁盘并加载ELF执行文件格式，并显示字符。而这lab1中的OS只是一个可以处理时钟中断和显示字符的幼儿园级别OS。

## 练习

为了实现lab1的目标，lab1提供了6个基本练习和1个扩展练习，要求完成实验报告。

对实验报告的要求：

+ 基于markdown格式来完成，以文本方式为主。
+ 填写各个基本练习中要求完成的报告内容
+ 完成实验后，请分析ucore_lab中提供的参考答案，并请在实验报告中说明你的实现与参考答案的区别
+ 列出你认为本实验中重要的知识点，以及与对应的OS原理中的知识点，并简要说明你对二者的含义，关系，差异等方面的理解（也可能出现实验中的知识点没有对应的原理知识点）
+ 列出你认为OS原理中很重要，但在实验中没有对应上的知识点

## 练习1：理解通过make生成执行文件的过程。

列出本实验各练习中对应的OS原理的知识点，并说明本实验中的实现部分如何对应和体现了原理中的基本概念和关键知识点。

在此练习中，大家需要通过静态分析代码来了解：

+ 操作系统镜像文件ucore.img是如何一步一步生成的？(需要比较详细地解释Makefile中每一条相关命令和命令参数的含义，以及说明命令导致的结果)
+ 一个被系统认为是符合规范的硬盘主引导扇区的特征是什么？

### 分析ucore.img的生成过程

通过`make V=`命令，可以看到ucore.img生成过程中各个命令的执行次序。整个生成过程大致可以分为三个阶段，现简述如下：

+ step1 生成操作系统kernel文件

	- 首先是对于kernel的各个依赖项分别进行编译。这里只是举了几个例子，实际上通过这种方式，生成了许多内核需要用到的`.o`文件,如`vectors.o`, `pmm.o`, `stdio.o`, `clock.o`...

	```
	cc kern/init/init.c
	gcc -Ikern/init/ -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc  \-fno-stack-protector -Ilibs/ -Ikern/debug/ -Ikern/driver/ -Ikern/trap/ -Ikern/mm/ -c kern/init/init.c -o obj/kern/init/init.o
	cc kern/driver/console.c
	gcc -Ikern/driver/ -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc  -fno-stack-protector -Ilibs/ -Ikern/debug/ -Ikern/driver/ -Ikern/trap/ -Ikern/mm/ -c kern/driver/console.c -o obj/kern/driver/console.o
	......
	```

	- 将编译生成的这些`.o`文件，利用链接指令`ld`，生成操作系统内核可执行文件`kernel`。

	```
	ld bin/kernel
	ld -m    elf_i386 -nostdlib -T tools/kernel.ld -o bin/kernel  obj/kern/init/init.o \
	obj/kern/libs/readline.o obj/kern/libs/stdio.o obj/kern/debug/kdebug.o \
	obj/kern/debug/kmonitor.o obj/kern/debug/panic.o obj/kern/driver/clock.o \
	obj/kern/driver/console.o obj/kern/driver/intr.o obj/kern/driver/picirq.o \
	obj/kern/trap/trap.o obj/kern/trap/trapentry.o obj/kern/trap/vectors.o \
	obj/kern/mm/pmm.o  obj/libs/printfmt.o obj/libs/string.o
	```

+ step2 生成bootloader可执行文件`bootblock`

	- 同样，也是先对几个依赖项进行编译，包括`bootasm.S`, `bootmain.c`
	
	```
	cc boot/bootasm.S
	gcc -Iboot/ -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc  -fno-stack-protector -Ilibs/ -Os -nostdinc -c boot/bootasm.S -o obj/boot/bootasm.o
	cc boot/bootmain.c
	gcc -Iboot/ -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc  -fno-stack-protector -Ilibs/ -Os -nostdinc -c boot/bootmain.c -o obj/boot/bootmain.o
	```

	- 将编译生成的`bootasm.o`和`bootmain.o`链接生成bootloader可执行文件。这里可以注意到，在链接指令`ld`中，指定了该文件读到内存中的地址`0x7C00`，即bootloader起始地址。并且，该bootloader文件大小为512字节，正好为一个磁盘扇区的大小，这也与上课讲的理论知识相符合。

	```
	ld bin/bootblock
	ld -m    elf_i386 -nostdlib -N -e start -Ttext 0x7C00 obj/boot/bootasm.o obj/boot/bootmain.o -o obj/bootblock.o
	'obj/bootblock.out' size: 488 bytes
	build 512 bytes boot sector: 'bin/bootblock' success!
	```

	> 但是这里有一个问题，就是`bootblock.o`是怎么生成`bootclock.out`以及`bootclock`的，并且为什么它们的大小从488B变到了512B。
	
	`sign.c`在其中起了作用，将488B的的`bootclock.out`读到了512B的`bin/bootclock`(后面将继续讨论)

+ step 3 将上面两步生成的kernel 以及 bootblock 连接生成虚拟磁盘镜像文件ucore.img,使用`dd`命令(convert and copy a file)

	```
	dd if=/dev/zero of=bin/ucore.img count=10000
	10000+0 records in
	10000+0 records out
	5120000 bytes (5.1 MB) copied, 0.0530962 s, 96.4 MB/s
	dd if=bin/bootblock of=bin/ucore.img conv=notrunc
	1+0 records in
	1+0 records out
	512 bytes (512 B) copied, 0.000104995 s, 4.9 MB/s
	dd if=bin/kernel of=bin/ucore.img seek=1 conv=notrunc
	146+1 records in
	146+1 records out
	74923 bytes (75 kB) copied, 0.000316792 s, 237 MB/s
	```

可以看到，最终`ucore.img`由三部分组成：
+ 第一部分为`/dev/zero`，不清楚是什么文件，是一个`character special file`
+ 第二部分为bootloader
+ 第三部分为操作系统内核kernel

### 符合规范的硬盘主引导扇区的特征

关于此项，老师给的提示是查看`tools/sign.c`文件。其主要功能是读取一个文件(第一个参数)到内存中，该文件大小小于510B，然后在其尾部添加主引导记录的结束标志`0x55AA`,并将512B(文件本身有效部分可能没有512B)写回到磁盘上的目标文件中(第二个参数)。

由此可以总结出符合规范的硬盘主引导扇区的特征：
+ 文件大小为512B，即一个硬盘扇区的大小。(当然也有可能其中有效文件根本没有这么大，但是BIOS还是读入一整个磁盘扇区的512B)
+ 文件尾部为`0x55AA`。这里文件尾部是指文件的第511字节和第512字节，而不管文件实际的有效大小有多大。

## 练习2：使用qemu执行并调试lab1中的软件。

为了熟悉使用qemu和gdb进行的调试工作，我们进行如下的小练习：

+ 从CPU加电后执行的第一条指令开始，单步跟踪BIOS的执行。
+ 在初始化位置`0x7c00`设置实地址断点,测试断点正常。
+ 从`0x7c00`开始跟踪代码运行,将单步跟踪反汇编得到的代码与`bootasm.S`和`bootblock.asm`进行比较。
+ 自己找一个bootloader或内核中的代码位置，设置断点并进行测试。

### 跟踪BIOS运行

老师说在命令行运行`make debug`就可以跟踪BIOS的运行了，然而实际操作的时候并不是这样。

```
$ make debug
The target architecture is assumed to be i8086
0x0000fff0 in ?? ()
Breakpoint 1 at 0x7d00: file boot/bootmain.c, line 88.

Breakpoint 1, bootmain () at boot/bootmain.c:88
```

可以看到，程序并没有在BIOS第一条指令中断，而是直接运行到了`bootmain.c`的`bootmain()`。为了解决这个问题，可以查看`Makefile`里面关于`debug`命令的描述

```
debug: $(UCOREIMG)
        $(V)$(QEMU) -S -s -parallel stdio -hda $< -serial null &
        $(V)sleep 2
        $(V)$(TERMINAL)  -e "cgdb -q -x tools/gdbinit"
```

这里说，运行gdb的时候，是去读`tools/gdbinit`文件，因此去看看这个文件

```
1 file obj/bootblock.o
2 set architecture i8086
3 target remote :1234
4 break bootmain
5 continue
```

问题出现了，这里定义是在`bootmain`这个函数这里设置到断点，然后它居然就`continue`了，所以程序运行就直接来到了`bootmain`。解决方案也很简单，我选择使用`make lab1-mon`指令，正常运行时断点在bootloader的第一条指令(0x7c00)处，我把它的`continue`给删掉就可以了。删掉之后`tools/lab1init`长这样：

```
1 set architecture i8086
2 target remote :1234
```

现在，运行`make lab1-mon`就可以跟踪BIOS运行了。

```
The target architecture is assumed to be i8086
0x0000fff0 in ?? ()
(gdb) x /2i $pc
=> 0xfff0:	add    %al,(%bx,%si)
   0xfff2:	add    %al,(%bx,%si)
(gdb) x $cs
   0xf000:	add    %al,(%bx,%si)
(gdb) 
```

在上面的指令中，我查看了当前的`ip`寄存器以及`cs`寄存器，从而可以得出当前运行的指令在`0xffff0`,这就是BIOS第一条指令存储的位置，也是CPU上电后执行的第一条指令。现在去查看这条指令

```
(gdb) x /2i 0xffff0
   0xffff0:	ljmp   $0xf000,$0xe05b
   0xffff5:	xor    %dh,0x322f
```

这是一个长跳转指令，会跳转到`0xfe05b`这个位置，然后继续执行。

### 跟踪Bootloader的执行

在原理里面有讲到，BIOS加载Bootloader到内存中的`0x7c00`位置，并且跳转到这个位置执行。为了跟踪Bootloader执行，可以在`0x7c00`处设置一个断点。

```
(gdb) b *0x7c00
Breakpoint 1 at 0x7c00
```

要注意的是这里设置断点要使用`*`标志符，表示是对内存中某个地址设置断点。

之后，就可以查看Bootloader的汇编代码了：

```
(gdb) c
Continuing.

Breakpoint 1, 0x00007c00 in ?? ()
(gdb) x /10i $pc
=> 0x7c00:	cli    
   0x7c01:	cld    
   0x7c02:	xor    %ax,%ax
   0x7c04:	mov    %ax,%ds
   0x7c06:	mov    %ax,%es
   0x7c08:	mov    %ax,%ss
   0x7c0a:	in     $0x64,%al
   0x7c0c:	test   $0x2,%al
   0x7c0e:	jne    0x7c0a
   0x7c10:	mov    $0xd1,%al
```

和`bootasm.S`以及`bootblock.asm`比对的话，代码都是一样的。(当然是一样的，执行的就是它的代码...

## 练习3：分析bootloader进入保护模式的过程

BIOS将通过读取硬盘主引导扇区到内存，并转跳到对应内存中的位置执行bootloader。请分析bootloader是如何完成从实模式进入保护模式的。

提示：需要阅读小节“保护模式和分段机制”和lab1/boot/bootasm.S源码，了解如何从实模式切换到保护模式，需要了解：

+ 为何开启A20，以及如何开启A20
+ 如何初始化GDT表
+ 如何使能和进入保护模式

### bootloader进入保护模式的过程

其实在题干上面已经说清楚了，Bootloader从实模式进入保护模式，主要需要进行三方面的工作：

+ 开启A20地址线。
+ 初始化段表(GDT表)。这是因为实模式和保护模式的寻址方式不同，在保护模式中使用段表来进行寻址，因此在进入保护模式之前应该首先设置好段表(GDT)以及段表寄存器(GDTR)。
+ 设置系统寄存器CR0，以使能保护模式位，进入保护模式。
+ 进入保护模式后，寻址方式发生了改变。当前的CS:IP已经不能寻址到下一条本应执行的指令，因此需要一条长跳转指令，开始保护模式的运行。

下面将针对各条具体进行说明。

### A20地址线

> A20地址线有什么作用？

A20地址线的控制，简单说来，就是可以使能A20地址线。在A20地址线禁止的情况下，内存地址的第20位(对于32位地址，是从0~31位)恒为零。这样，系统就只能访问奇数兆的空间，即0--1M, 2--3M...所以在保护模式下，必须要打开A20地址线

> 为什么会出现A20地址线的控制？

这是具有一定的历史原因，是为了intel x86产品向下兼容而出现的。

说的是本来8086的芯片本来只有20位的地址线，只能访问1M的内存。但是实际上，在cs = 0xffff, ip = 0xffff时，cs:ip = 0x10ffef。这就超过了1M的内存访问空间。8086的芯片在这种情况下会出现“回卷”，即不会出现异常，而是会从零地址0x0重新定位。

但是在后来的芯片中，地址线的位数增加了，比如这时有了24位地址线。但是为了向下兼容，芯片提供了实模式，使之也可以工作在8086的条件下。但是由于地址线多了，寻址超过1M内存时，系统不再会回卷了，而是会实际去访问那部分不能访问的空间(想来应该是硬件电路设计的问题)，这就造成了向下不兼容。

为了实现向下兼容的回卷，IBM给整个系统加上了硬件逻辑，这样就出现了A20 Gate。在A20 Gate禁止的情况下，访问超过1M内存的空间时，由于第20位地址始终为零，就又出现了回卷。

> 如何控制A20地址线？

这就完全是硬件方面的知识了。需要查看相关控制器的手册。这些控制器一般都是以外设的形式与CPU连接的，所以主要是通过与外设通信的`in`和`out`指令，来控制这些外设的功能。

对于A20地址线来说，控制它的外设即`8042键盘控制器`(叫是叫这个名字，但是好像和键盘没有任何联系)

`8042键盘控制器`的IO端口是0x60～0x6f, 实际上使用的只有0x60和0x64两个端口。其中，0x60端口是数据端口，命令的参数以及返回值都是通过这个读写端口获得， 0x64端口是命令端口，命令是通过写这个端口来控制`8042键盘控制器`。除此以外，8042还有3个内部端口：Input Port、Outport Port和Test Port。它们的一些具体操作如下：

+ 读Output Port：向64h发送0d0h命令，然后从60h读取Output Port的内容
+ 写Output Port：向64h发送0d1h命令，然后向60h写入Output Port的数据
+ 禁止键盘操作命令：向64h发送0adh
+ 打开键盘操作命令：向64h发送0aeh

A20 Gate被定义在Output Port的bit 1上。所以理论上讲，我们只要操作8042芯片的输出端口（64h）的bit 1，就可以控制A20 Gate，但实际上，当你准备向8042的输入缓冲区里写数据时，可能里面还有其它数据没有处理，所以，我们要首先禁止键盘操作，同时等待数据缓冲区中没有数据以后，才能真正地去操作8042打开或者关闭A20 Gate。打开A20 Gate的具体步骤大致如下：

+ 等待8042 Input buffer为空
+ 发送Write 8042 Output Port （P2）命令到8042 Input buffer
+ 等待8042 Input buffer为空
+ 将8042 Output Port（P2）得到字节的第2位置1，然后写入8042 Input buffer

> 具体操作代码

查看bootasm.S可以找到使能A20地址线的代码：

```
seta20.1:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.1

    movb $0xd1, %al                                 # 0xd1 -> port 0x64
    outb %al, $0x64                                 # 0xd1 means: write data to 8042's P2 port

seta20.2:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.2

    movb $0xdf, %al                                 # 0xdf -> port 0x60
    outb %al, $0x60                                 # 0xdf = 11011111, means set P2's A20 bit(the 1 bit) to 1
```

其中需要做一些说明：

+ `inb $0x64, %al`指令读0x64端口。之前说0x64端口是命令端口，但是读这个端口可以获得关于8042的一些状态信息，因此可以判断该控制器是否就绪。如果没有就绪的话，就持续读该端口，直到控制器就绪。
+ `movb $0xdf, %al;outb %al, $0x60`直接将11011111b写到了8042控制器，对于我来说感觉有点莽撞。或许先读Output Port，然后再只修改A20位的值为1，再写回到控制器是不是会更稳妥一点？

### 初始化段表

上面提到了为什么要首先初始化段表?这是由于保护模式和实模式的寻址方式不同，下面将具体讨论。

> 保护模式和实模式的寻址方式有什么不同？

实模式就比较简单，直接cs:ip，然后cs左移4为就完事儿了，非常简单。

保护模式就不一样了，因为地址空间已经变成了32位，而段寄存器的长度还是16位，直接用段寄存器的值来索引内存的某一个段感觉有点寒碜啊。这时候，段寄存器的值已经不是某一个地址了，而是用来索引段表中某一项的段选择子(segment selector)

我们知道，段表的结构是一个线性表，里面每一项包括8字节，其中32位指示该段在内存中的起始地址。段表中至多可以有8192(2^13)项，也就是允许同时存在8192个段。此时，CS寄存器中的值只是对段表这个线性表的一个索引，表示当前选择的是段表中的第几个段。

由于至多有8192个段，因此可以只用CS中的高13位，就足以指定段表中的任意一个段。还有低3位是用来表示一些控制信息：

+ 第2位，表指示位(TI, Table Indicator)，用来指定应该访问哪一个描述符表。0表示全局描述符表(GDT), 1表示局部描述符表(LGT)。
+ 第0--1位，请求特权级(RPL, Requested Privilege Level),制定当前段的优先级，是用于保护机制的。

> 如何找到段表的位置?

段表作为一个特殊的数据结构，本身并不属于任意一个段，而是相当于一个独立的段。通过全局描述符表寄存器(GDTR)来标志其位置。GDTR是一个48位寄存器，其中前32位标志了段表的位置，后16位为limit，指示了段表的大小(段表的最后一个字节)，其值通常是$8·N-1$，其中N为段表中的段描述符的数量。8是因为段表中一个段描述符为8个字节，64bits。

通过上面的讨论，现在可以总结出保护模式下内存访问的步骤：

+ 读取CS寄存器，获得要访问的段描述符在段表中的偏移量
+ 读取GDTR寄存器，验证访问的偏移没有越界，并得到段表在内存中的地址
+ 找到段表，并访问目标段描述符，获得目标段的内存地址。 
+ 将段描述符中的段大小(20位)与IP寄存器中的偏移量进行比较，确保没有越界
+ 一些访问权限的检验
+ 将段的内存地址与IP中的偏移量相加，获得要访问的物理内存地址。

> 初始化段表的代码

在bootasm.S中可以找到初始化段表的代码：

```
lgdt gdtdesc

gdt:
    SEG_NULLASM                                     # null seg
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)           # code seg for bootloader and kernel
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel

gdtdesc:
    .word 0x17                                      # sizeof(gdt) - 1
    .long gdt                                       # address gdt

```

其中，`SEG_NULLASM`以及`SET_ASM`在asm.h中定义

```
#define SEG_NULLASM                                             \
    .word 0, 0;                                                 \
    .byte 0, 0, 0, 0

#define SEG_ASM(type,base,lim)                                  \
    .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);          \
    .byte (((base) >> 16) & 0xff), (0x90 | (type)),             \
        (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)


/* Application segment type bits */
#define STA_X       0x8     // Executable segment
#define STA_E       0x4     // Expand down (non-executable segments)
#define STA_C       0x4     // Conforming code segment (executable only)
#define STA_W       0x2     // Writeable (non-executable segments)
#define STA_R       0x2     // Readable (executable segments)
#define STA_A       0x1     // Accessed
```

在上面的代码中，
+ `gdt`段定义了段表中的三个段描述符，即空描述符(第一个总是空描述符)， 代码段描述符，数据段描述符。这里注意到，代码段和数据段是重合的，都是从0地址开始。然后也和当前CS寄存器的值相同，确保进入保护模式后程序可以正常跳转。
+ `gdtdesc`段定义了GDTR寄存器中应该有的值，其高32位是段表的起始地址(gdt)，低16位为段表的limit(之前我以为它错了，怎么可能是17？！然后我刚刚发现它是16进制的，所以应该是23，即3·8-1)
+ `ldgt gdtdesc`将`gdtdesc`的值加载到GDTR中，这样，段表就初始化完毕了。

### 设置系统寄存器CR0

系统寄存器CR0的第0位PE(Protection Enable)用于使能保护模式。设置为1时就可以开启保护模式。这样，就是简单的寄存器操作了。

```
.set CR0_PE_ON,		0x1 		# protected mode enable flag

movl %cr0, %eax
orl $CR0_PE_ON, %eax
movl %eax, %cr0
```

太简单了，就不多说了。

### 执行长跳转指令

> 为什么需要长跳转指令？

改变寻址方式后，使用之前的CS和IP已经不能正确访问到下一条要执行的指令了。就比如说，当前`cs = 0X0`, 使用这个段选择子只能选择到段表中的第一项，即`SEG_NULLASM`，而这时一个无效项。要正确访问到段表中的代码段描述符，应该将`cs`设置为段表中第一项的偏移量，即0x0008,这是因为段选择子的低三位为控制位，第二位为0表示访问GDT,低两位为零表示最高特权级。

但是存在一个问题，`cs`寄存器是不能直接设置的。它只能通过程序跳转指令，如`CALL`, `RET`, `INT`, `LJMP`指令来改变。因此，这里需要一个长跳转指令，来改变cs寄存器中的值，使之可以正确访问下一条要执行的指令。

```
.set PROT_MODE_CSEG,        0x8                     # kernel code segment selector
.set PROT_MODE_DSEG,        0x10                    # kernel data segment selector
.set CR0_PE_ON,             0x1                     # protected mode enable flag

.code16
	......

	# Jump to next instruction, but in 32-bit code segment.
	# Switches processor into 32-bit mode.
	ljmp $PROT_MODE_CSEG, $protcseg

.code32                                             # Assemble for 32-bit mode
protcseg:
    # Set up the protected-mode data segment registers
```

其中，`$PROT_MODE_CSEG`就是段表中代码段的偏移，即0x8, `$protcseg`则为下一条指令的偏移地址。这样，Bootloader才算完全实现了从实模式进入到保护模式。那么，这条长跳转指令本身是如何被找到的呢？我的想法是由于CPU的流水线机制，导致在进入保护模式前这条指令被预取了。


## 练习4：分析bootloader加载ELF格式的OS的过程。

通过阅读bootmain.c，了解bootloader如何加载ELF文件。通过分析源代码和通过qemu来运行并调试bootloader&OS

+ bootloader如何读取硬盘扇区的？
+ bootloader是如何加载ELF格式的OS？

提示：可阅读“硬盘访问概述”，“ELF执行文件格式概述”这两小节。

### Bootloader读取硬盘扇区

Bootloader的主要功能就是要读取硬盘扇区上的操作系统，把它加载到内存中，然后把控制权转交给操作系统。因此这里就涉及到程序要直接读取硬盘扇区。

我们知道，硬盘设备是作为外设与CPU连接的，因此读取硬盘扇区本质上也是通过I/O操作，发送命令并且读取数据，以将硬盘上的数据读取到内存中，这与上面A20的开启本质上是一样的。

> 硬盘扇区的硬件格式

一般主板有2个IDE(Integrated Drive Electronics)通道，每个通道可以接2个IDE硬盘。访问第一个硬盘的扇区可设置IO地址寄存器0x1f0-0x1f7来实现。

读写硬盘根据寻址空间大小的不同，可以分为多个模式，如
+ chs方式 ：小于8G (8064MB)
+ LBA28方式：小于137GB
+ LBA48方式：小于144,000,000 GB

其中LBA28模式表示28位的逻辑区块地址(Logical Block Address)，每一个扇区为512字节，这样28位代表了256M个扇区，所以可以存储128GB的数据。我们这里就是使用LBA28的方式来读取硬盘。

LBA28模式具有多个寄存器：

+ data寄存器，位于端口号0x1F0。可以用来读取或写入数据
+ features寄存器，位于端口号0x1F1。用于读取时的错误信息，写入时的额外参数
+ sector count寄存器，位于0x1F2。用于指定读取或写入的扇区数
+ LBA low寄存器，位于0x1F3。用于指定lba地址的低8位
+ LBA mid寄存器，位于0x1F4。用于指定lba地址的中8位
+ LBA high寄存器	，位于0x1F5。用于指定lba地址的高8位
+ device寄存器，位于端口号0x1F6。用于指定lba地址的前4位（占用device寄存器的低4位），主盘值为0（占用device寄存器的第5位），第6位值为1，第7位LBA模式为1，CHS模式为0，第8位值为1
+ command寄存器，位于端口号啊0x1F7。用于读取，写入的命令，返回磁盘状态。其中0x20命令表示读取扇区，0x30命令表示写入扇区

> 读取硬盘扇区的方法

简单说来，可以分为下面几步

+ 等待磁盘准备好

读取command寄存器，判断硬盘状态是否就绪

```
/* waitdisk - wait for disk ready */
static void
waitdisk(void) {
    while ((inb(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}
```

+ 发出读取扇区的命令

将要读取的硬盘扇区的信息先写入前面那些寄存器，然后通过command寄存器发出读取的命令。

```
    outb(0x1F2, 1);                         // count = 1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors
}
```

+ 再次等待磁盘准备好

再次调用`waitdist`函数。

+ 把磁盘扇区数据读到指定内存

从data寄存器读取请求的数据。

```
    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
```

要注意的是这里的`insl(0x1F0, dst, SECTSIZE / 4)`，为什么`SECSIZE`要除以4呢？这是因为`insl`指令是读入一个长字符(l for long)序列,读入的单位是4B，第三个参数是制定要读入的4字节的数量，所以应该除以4，以保证读入512字节，即一个扇区的大小。

上面给出的程序是读取一个硬盘扇区的函数，通过恰当的封装，可以实现通过从0开始的字节数来读取恰当硬盘扇区的函数(bootmain.c中正是这样做的)。但由于其基本原理还是那样，在这里不多做叙述。

### Bootloader加载ELF格式的OS

在上面的讨论中，已经可以做到将OS读入到内存当中，之后还剩下的问题是Bootloader需要将控制权转交给操作系统，为此就需要知道OS应该从哪条指令执行，然后跳转到那条指令。简单说来，就是要能解析OS的文件格式，而这里是elf格式。

> 什么是elf格式?

elf，<del>其实就是精灵。</del>即Executable and Linkable Format，为可执行和可链接格式。如它的名字所指示的，elf格式文件包括三种主要类型：

+ 可执行文件(executable file)。显然，我们这里的OS就是可执行文件。
+ 可重定位文件(relocatable file)
+ 共享目标文件(shared object file)

这里我们只分析可执行文件类型。

elf可执行文件由几个部分组成：

+ ELF头部。ELF header在文件开始处描述了整个文件的组织。比如文件的适用机器，文件的大小，版本，程序头的入口以及程序头的数量，还有整个可执行文件的入口地址。

```
struct elfhdr {
  uint magic;  // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint version;
  uint entry;  // 程序入口的虚拟地址
  uint phoff;  // program header 表的位置偏移
  uint shoff;
  uint flags;
  ushort ehsize;
  ushort phentsize;
  ushort phnum; //program header表中的入口数目
  ushort shentsize;
  ushort shnum;
  ushort shstrndx;
};
```

其中，通过判断magic字段，可以确定这是否为一个合法的ELF文件。若是合法的ELF文件，则可以通过程序头的偏移量(`phoff`)以及程序头的数量(`phnum`),来读到程序头，从而可以获得程序运行的更加具体的信息。

+ 程序头表。描述与程序执行直接相关的目标文件结构信息，用来在文件中定位各个段的映像，同时包含其他一些用来为程序创建进程映像所必需的信息。

```
struct proghdr {
  uint type;   // 段类型
  uint offset;  // 段相对文件头的偏移值
  uint va;     // 段的第一个字节将被放到内存中的虚拟地址
  uint pa;
  uint filesz;
  uint memsz;  // 段在内存映像中占用的字节数
  uint flags;
  uint align;
};
```
可以看到，其中描述了程序段相对于文件头开始位置`offset`，以及被加载到内存当中的地址`va`。利用这些信息，可以把相应的程序加载到指定的位置，从而使得后续控制权可以正常地转交给os。

+ 节
+ 节头表

> 加载elf格式的os并且转交控制权的过程

可以查看`bootmain.c`的代码。可以看到，Bootloader加载elf格式的os可以分为几个步骤：

+ 将os从硬盘读入到内存中,并判断是否合法

```
readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);
if (ELFHDR->e_magic != ELF_MAGIC) {
	goto bad;
}
```

+ 通过elf文件头，获得该os的基本信息，如程序头的入口。并且将可执行代码从程序头标志的位置读入到内存中被程序头指定的地址。

```
ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
eph = ph + ELFHDR->e_phnum;
for (; ph < eph; ph ++) {
    readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
}
```

+ 跳转到elf文件标志的起始地址开始执行os的指令。至此，CPU的控制权转交给了os。

```
((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();
```

## 练习5：实现函数调用堆栈跟踪函数 （需要编程）

我们需要在lab1中完成`kdebug.c`中函数`print_stackframe`的实现，可以通过函数`print_stackframe`来跟踪函数调用堆栈中记录的返回地址。在如果能够正确实现此函数，可在lab1中执行 “make qemu”后，在qemu模拟器中得到类似如下的输出：

```
……
ebp:0x00007b28 eip:0x00100992 args:0x00010094 0x00010094 0x00007b58 0x00100096
    kern/debug/kdebug.c:305: print_stackframe+22
ebp:0x00007b38 eip:0x00100c79 args:0x00000000 0x00000000 0x00000000 0x00007ba8
    kern/debug/kmonitor.c:125: mon_backtrace+10
ebp:0x00007b58 eip:0x00100096 args:0x00000000 0x00007b80 0xffff0000 0x00007b84
    kern/init/init.c:48: grade_backtrace2+33
ebp:0x00007b78 eip:0x001000bf args:0x00000000 0xffff0000 0x00007ba4 0x00000029
    kern/init/init.c:53: grade_backtrace1+38
ebp:0x00007b98 eip:0x001000dd args:0x00000000 0x00100000 0xffff0000 0x0000001d
    kern/init/init.c:58: grade_backtrace0+23
ebp:0x00007bb8 eip:0x00100102 args:0x0010353c 0x00103520 0x00001308 0x00000000
    kern/init/init.c:63: grade_backtrace+34
ebp:0x00007be8 eip:0x00100059 args:0x00000000 0x00000000 0x00000000 0x00007c53
    kern/init/init.c:28: kern_init+88
ebp:0x00007bf8 eip:0x00007d73 args:0xc031fcfa 0xc08ed88e 0x64e4d08e 0xfa7502a8
<unknow>: -- 0x00007d72 –
……
```

请完成实验，看看输出是否与上述显示大致一致，并解释最后一行各个数值的含义。

提示：可阅读小节“函数堆栈”，了解编译器如何建立函数调用关系的。在完成lab1编译后，查看lab1/obj/bootblock.asm，了解bootloader源码与机器码的语句和地址等的对应关系；查看lab1/obj/kernel.asm，了解 ucore OS源码与机器码的语句和地址等的对应关系。

要求完成函数kern/debug/kdebug.c::print_stackframe的实现，提交改进后源代码包（可以编译执行），并在实验报告中简要说明实现过程，并写出对上述问题的回答。

> 基本原理：为什么可以实现函数调用堆栈的跟踪？

在函数的嵌套调用过程中，硬件会保存一些寄存器信息，以便于函数调用完成后返回被调用函数的下一条语句。这些被保存的信息，一般会包括一些会被改变的的通用寄存器，下一条执行的指令地址(IP)。之前我的理解就仅限于此，因为这两个信息已经足以使函数返回到之前被打断的位置。在这种情况下是不能实现函数调用堆栈的跟踪的。

但是实际系统在保存这两个信息以外，在进行函数调用时，还会将当前的`ebp`寄存器压栈，并且将`esp`寄存器的值赋给`ebp`。这可以在任何函数调用的汇编语句中看到：

```asm
int
kern_init(void) {
	100000:       55                      push   %ebp
	100001:       89 e5                   mov    %esp,%ebp
```

这相当于建立了一个当前被调用函数的堆栈，因为将`ebp`更新为`esp`，意味着堆栈段清空，而以后该函数用到的一些局部变量，将全部存储在以新的`ebp`开始的堆栈里面。

但是这里最重要的是，由于在更新`ebp`的语句之前，将原来的`ebp`压入了堆栈。这意味着当前的`esp`(也是新的`ebp`)正好指向上一个函数堆栈的起始位置。这样，通过`ebp`，就可以将函数嵌套调用路径上的各个函数全部串接起来。这也就是跟踪函数调用堆栈的原理。实际上，各个编译器报错时的函数调用关系就是这样产生的。

> 具体`print_stackframe`函数的实现。

可以看到，题目中不仅要求要打印函数调用堆栈的`ebp`，还需要`eip`，函数的参数，以及函数所在的文件以及函数等信息。要获取这些信息，就需要了解函数调用时各个寄存器的入栈次序。

在调用某个函数时：

+ 首先要由原函数将被调用函数的参数压入栈中，从而正确实现函数的调用。
+ `call`语句的执行，将使得`eip`被硬件压入栈中。
+ 在被调用函数体内，会由编译器自动加上`push %ebp; mov %esp, %ebp`的操作
+ 这以后，是当前函数的局部栈，存放当前函数要用到的局部变量等。

因此可以看到，以`ebp`为基准，可以轻松地获得`eip`和函数的参数等信息，如`eip`的信息，就存储在`ebp + 4`的位置，可以使用内嵌汇编来获得。但是需要注意的是，这里的`eip`并不是与`ebp`配套的`eip`，而是被调用函数的返回地址。要获取当前函数的`eip`信息，可以采用同样的思想，即调用一个读取`eip`的函数，在该函数中读取到的`eip`就是我们期望获得的`eip`了。

```c
static __noinline uint32_t
read_eip(void) {
    uint32_t eip;
    asm volatile("movl 4(%%ebp), %0" : "=r" (eip));
    return eip;
}
```

注意到该函数的修饰符为`__noinline`，因为就是需要真正的函数调用，才能由硬件压入当前的`eip`，从而可被读取出。

但除此以外，还需要获得某一个函数的其他信息，即其所在的文件，以及在当前文件中的位置。此外还有函数参数的个数。这些信息，是存储在所谓的`stab table`里面。

`stab`是一个c代码调试信息的格式，当使用gcc编译c代码时，使用`-g`指令，会使编译器添加额外的调试信息，包括行号，变量的类型以及范围，以及函数的参数个数等。而这些信息就存储在`stab table`里面。而读取`stab`格式好像比较繁琐，所以老师已经提供了相应的api，所以直接调用就可以了。

这样，`print_stackframe`的代码如下：

```c
void
print_stackframe(void) {
     /* LAB1 YOUR CODE : STEP 1 */
     /* (1) call read_ebp() to get the value of ebp. the type is (uint32_t);
      * (2) call read_eip() to get the value of eip. the type is (uint32_t);
      * (3) from 0 .. STACKFRAME_DEPTH
      *    (3.1) printf value of ebp, eip
      *    (3.2) (uint32_t)calling arguments [0..4] = the contents in address (unit32_t)ebp +2 [0..4]
      *    (3.3) cprintf("\n");
      *    (3.4) call print_debuginfo(eip-1) to print the C calling function name and line number, etc.
      *    (3.5) popup a calling stackframe
      *           NOTICE: the calling funciton's return addr eip  = ss:[ebp+4]
      *                   the calling funciton's ebp = ss:[ebp]
      */
	struct eipdebuginfo info;
	uint32_t curr_arg, offset, ix, count, eip, ebp;
	uint8_t  flag = 1;
	eip = read_eip();
	ebp = read_ebp();
	for(ix = 0; ix != STACKFRAME_DEPTH && flag;ix++){
		cprintf("ebp:0x%08x eip:0x%08x ", ebp, eip);
		
		if(debuginfo_eip(eip, &info) != 0)
			 flag = 0;
		for(count = 0; count != info.eip_fn_narg; count++){
			offset = 8 + 4 * count;
			asm volatile("addl %1, %0":"+r"(offset): "r"(ebp)); 
			asm volatile("movl (%1), %0" : "=r"(curr_arg): "r"(offset));
			cprintf(" 0x%08x", curr_arg);
		}
		cprintf("\n");
		
		print_debuginfo(eip);
		
		asm volatile("movl 4(%1), %0":"=r"(eip):"r"(ebp));
		asm volatile("movl (%1), %0":"=r"(ebp):"r"(ebp));
	}	
}
```

其中，函数的一些信息，如行号，文件名之类的，直接通过函数`debuginfo_eip`获得。

关于这个函数，在完成的过程中还是遇到了不少问题的，总结如下：

+ gcc内联汇编不够熟悉。一开始总是在内联汇编中直接指定寄存器，如`movl %%eax, %%ebx`，然而这样根本不能编译通过。这是因为可能这些寄存器已经在用了，是不能随便改变的，所以还是使用内联汇编的扩展语法比较稳妥。
+ 一开始追踪`ebp`的调用时，是将当前的`ebp`寄存器中的值读出，然后将这个值作为地址，利用地址所指的数据去更新`ebp`，即

```
asm volatile("movl %%ebp, %0":"=r"(ebp))
asm volatile("movl (%0), %%ebp": "r"(ebp))
```

这个思路我本来觉得挺好的，但是在更新`ebp`后我之前定义的局部变量就都失效了，就是说具有一个未定义的值。因此，也印证了各个变量的确是存储在当前函数的局部堆栈里面，一旦修改了`ebp`，就索引不到这些变量了。
+ 还应该注意到最后两句内联汇编的次序。之前也说过，`ebp + 4`的值并不是与当前`ebp`对应的`eip`，而是上层函数的返回地址。因此，需要先读取这个`eip`，再更新`ebp`，才能使两者对应起来。
+ 感觉题目给的示例输出有问题啊，似乎是直接指定了函数的参数固定输出四个。而我的代码是根据`debuginfo`给出的函数参数个数信息，逐个进行打印的。
+ 关于最后最后一行的数值，可以追溯到`bootasm.S`中对`esp`和`ebp`的初始化工作：

```asm
    # Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
    movl $0x0, %ebp
    movl $start, %esp
    call bootmain
```

可以看到，初始的`esp`被设置为了`bootloaded`的起始地址，亦即`0x7c00`。最后进行了一次函数调用`call bootmain`，首先是硬件将`eip`压栈，此时`esp = 0x7bfb`，随后是编译器添加的将`ebp`压栈，然后令`ebp = esp`，此时`ebp = esp = 0x7bf8`，这也就是函数调用堆栈中的第一个`ebp`，因此对函数调用过程的追踪终止于此，可以看到`ebp`的值和这里的分析是一致的。

## 练习6：完善中断初始化和处理 （需要编程）

请完成编码工作和回答如下问题：

+ 中断描述符表（也可简称为保护模式下的中断向量表）中一个表项占多少字节？其中哪几位代表中断处理代码的入口？
+ 请编程完善kern/trap/trap.c中对中断向量表进行初始化的函数idt_init。在idt_init函数中，依次对所有中断入口进行初始化。使用mmu.h中的SETGATE宏，填充idt数组内容。每个中断的入口由tools/vectors.c生成，使用trap.c中声明的vectors数组即可。
+ 请编程完善trap.c中的中断处理函数trap，在对时钟中断进行处理的部分填写trap函数中处理时钟中断的部分，使操作系统每遇到100次时钟中断后，调用print_ticks子程序，向屏幕上打印一行文字”100 ticks”。

要求完成问题2和问题3 提出的相关函数实现，提交改进后的源代码包（可以编译执行），并在实验报告中简要说明实现过程，并写出对问题1的回答。完成这问题2和3要求的部分代码后，运行整个系统，可以看到大约每1秒会输出一次”100 ticks”，而按下的键也会在屏幕上显示。

### 中段描述符表

> 中断描述符表的结构？

和全局描述符表一样，中断描述符表的每一个表项占8个字节，其每一个表项叫做门描述符。和段描述符不一样，它没有段大小(20位)字段，因为可以通过查段表获得。

为了要能描述中断服务程序的入口地址，门描述符显然应该具有

+ 段选择子，2个字节。这样，查到段选择子后，显然还需要查段表，才能获得入口程序的物理地址。
+ 偏移量，4个字节

除此以外，还需要一些控制字，来描述优先级，中断类型(硬中断，异常，陷入)等信息。其具体结构如下：

```c
/* Gate descriptors for interrupts and traps */
struct gatedesc {
    unsigned gd_off_15_0 : 16;        // low 16 bits of offset in segment
    unsigned gd_ss : 16;            // segment selector
    unsigned gd_args : 5;            // # args, 0 for interrupt/trap gates
    unsigned gd_rsv1 : 3;            // reserved(should be zero I guess)
    unsigned gd_type : 4;            // type(STS_{TG,IG32,TG32})
    unsigned gd_s : 1;                // must be 0 (system)
    unsigned gd_dpl : 2;            // descriptor(meaning new) privilege level
    unsigned gd_p : 1;                // Present
    unsigned gd_off_31_16 : 16;        // high bits of offset in segment
};
```

### 初始化中断描述符表(`init_idt`)

> 既然中断描述符表存放了中断服务程序的入口地址，那么只需要将这些入口地址按照中断描述符的格式填入就可以了。但是有一个问题，中断描述符表最多有256个表项，所有这些中断号全都已经定义了中断服务程序了吗？

答案是对，也不对。

首先来看中断的执行过程。这和我原来想象中的不太一样。按照我的想象，通过硬件判断出中断号后以后，通过查idt表，就可以获得服务程序的入口地址，进而转到中断服务程序去执行。


<del>
但这个想法其实不太能经得起推敲。因为256个中断号，其中前32个(0~31)是系统保留的，用于异常和不可屏蔽中断，其它的都是可以留给用户定义的。如果这些用户定义的中断服务程序更换了地址，那么为了使它们生效，还需要重新加载中段描述符表，这种操作我反正是没有听说过。</del>

<del>因此，比较好的方法就是固定各个中断服务程序的入口地址。实际上也的确是这么做的，这些入口地址也称为中断向量。但是这又会带来一个问题，应该给每个中断服务程序预留多少空间呢，因为每个中断服务程序理论上都是可以很大的，固定了入口地址带来了不便性。</del>

<del>实际的方法差不多是上面两者的折中，即中断向量里面进行的操作是固定的，无非是将一些信息压栈。然后所有中断向量都会跳转到同一个函数，在这个函数中再根据先前保存的各个信息(尤其是中断号)，跳转到对应的实际的中断处理程序，这些处理程序可以是用户定义的，它们可以存在于内存的各个位置，就和普通的程序无异。
</del>

上面写的我感觉有点不太对啊。中断服务程序肯定是预先就定义好的，只有操作系统的设计者才可以改变，因此通过idt表直接获得中断服务程序的入口地址的方案是完全可行的。但是问题在于，所有的中断都需要进行一些共同的操作，比如各个寄存器压栈，由用户栈到内核栈的切换等，如果由idt表直接跳转到中断服务程序的话，所有这些操作都需要在不同的中断服务程序里面出现，代码复用性差，而且也占用更多的内存空间。因此，一个方案就是idt表的所有项都跳转到同一个地址，来进行相同的中断准备工作，然后再进行实质上的中断服务操作。

这样，初始化`idt`表的工作，其实就是将各个中断服务程序的入口地址逐个填入过程，问题就变得很简单了：

```c
static struct pseudodesc idt_pd = {
    sizeof(idt) - 1, (uintptr_t)idt
};

/* idt_init - initialize IDT to each of the entry points in kern/trap/vectors.S */
void
idt_init(void) {
     /* LAB1 YOUR CODE : STEP 2 */
     /* (1) Where are the entry addrs of each Interrupt Service Routine (ISR)?
      *     All ISR's entry addrs are stored in __vectors. where is uintptr_t __vectors[] ?
      *     __vectors[] is in kern/trap/vector.S which is produced by tools/vector.c
      *     (try "make" command in lab1, then you will find vector.S in kern/trap DIR)
      *     You can use  "extern uintptr_t __vectors[];" to define this extern variable which will be used later.
      * (2) Now you should setup the entries of ISR in Interrupt Description Table (IDT).
      *     Can you see idt[256] in this file? Yes, it's IDT! you can use SETGATE macro to setup each item of IDT
      * (3) After setup the contents of IDT, you will let CPU know where is the IDT by using 'lidt' instruction.
      *     You don't know the meaning of this instruction? just google it! and check the libs/x86.h to know more.
      *     Notice: the argument of lidt is idt_pd. try to find it!
      */
	extern uintptr_t __vectors[];
	
	uint16_t cs;
	asm volatile("movw %%cs, %0":"=r"(cs));
	uint32_t ix;
	for(ix = 0; ix < 256; ++ix)
		SETGATE(idt[ix], 0, cs, __vectors[ix], 0);
	
	lidt(&idt_pd);
}
```

其中，`__vectors`数组中存放了各个中断向量的入口地址，但这个地址只是一个偏移量，还需要结合当前的代码段选择子`cs`，就可以初始化`idt`了。和段表一样，最后需要告诉操作系统这个中断描述符表在内存中的位置，即将其信息写入`idtr`寄存器中。

> 中断执行全过程。

只初始化`idt`表也太简单了，再来探索下中断执行的一些细节部分。

我们已经知道，中断的执行是通过以下几步：

+ 通过硬件的操作，找出中断号，然后交给相应的中断向量（涉及到查`idt`表）
+ 中断向量里面将中断号和错误信息压入堆栈，然后跳转到`__alltraps`代码段(因为是使用`jmp`指令，所以不是函数)
+ 在`__alltraps`继续把各个寄存器压栈，包括四个段寄存器(`ds`, `es`, `fs`, `gs`)，所有的通用寄存器(`edi`, `esi`, `ebp`, `esp`, `ebx`, `edx`, `ecx`, `eax`)。足可见中断是一件大事，要把所有寄存器都准备好。
+ `__alltraps`调用`trap`函数
+ 在`trap`函数中，调用`trap_dispatch`函数，转到具体的中断服务例程

需要注意的是，`trap_dispatch`函数接受一个指向`trapframe`结构体的指针。这个指针是通过将`esp`压栈实现的，也就是说该指针就是`esp`，而对应的结构体，就是被压入栈中的数据。`trapframe`结构体如下所示：

```c
/* registers as pushed by pushal */
struct pushregs {
    uint32_t reg_edi;
    uint32_t reg_esi;
    uint32_t reg_ebp;
    uint32_t reg_oesp;            /* Useless */
    uint32_t reg_ebx;
    uint32_t reg_edx;
    uint32_t reg_ecx;
    uint32_t reg_eax;
};

struct trapframe {
    struct pushregs tf_regs;
    uint16_t tf_gs;
    uint16_t tf_padding0;
    uint16_t tf_fs;
    uint16_t tf_padding1;
    uint16_t tf_es;
    uint16_t tf_padding2;
    uint16_t tf_ds;
    uint16_t tf_padding3;
    uint32_t tf_trapno;
    /* below here defined by x86 hardware */
    uint32_t tf_err;
    uintptr_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding4;
    uint32_t tf_eflags;
    /* below here only when crossing rings, such as from user to kernel */
    uintptr_t tf_esp;
    uint16_t tf_ss;
    uint16_t tf_padding5;
} __attribute__((packed));
```

可见，`trapframe`的低字节就是在`__alltraps`中被压入栈中的那些寄存器(因为栈是从高字节向低字节生长的)，其中的`tf_err`, `tf_trapno`是在中断向量`vectors`中被压入栈的，而`tf_eip`, `tf_cs`, `tf_eflags`都是硬件压入栈中的(所以这也是中断和函数调用的区别)。并且我们还可以看到，如果该中断涉及特权级的转换(从用户态转换到内核态)，硬件还会压入`tf_esp`以及`tf_ss`，以便中断返回时从内核堆栈切换回用户堆栈。

### 时钟中断

这个练习的代码太简单了，不想多说，就直接贴程序吧：

```c
/* trap_dispatch - dispatch based on what type of trap occurred */
extern int ticks_count;
static void
trap_dispatch(struct trapframe *tf) {
    char c;

    switch (tf->tf_trapno) {
    case IRQ_OFFSET + IRQ_TIMER:
        /* LAB1 YOUR CODE : STEP 3 */
        /* handle the timer interrupt */
        /* (1) After a timer interrupt, you should record this event using a global variable (increase it), such as ticks in kern/driver/clock.c
         * (2) Every TICK_NUM cycle, you can print some info using a funciton, such as print_ticks().
         * (3) Too Simple? Yes, I think so!
         */
		
		if(++ticks_count == TICK_NUM){
			ticks_count = 0;
			print_ticks();
		}
        break;
    case IRQ_OFFSET + IRQ_COM1:
        c = cons_getc();
        cprintf("serial [%03d] %c\n", c, c);
        break;
    case IRQ_OFFSET + IRQ_KBD:
        c = cons_getc();
        cprintf("kbd [%03d] %c\n", c, c);
        break;
    //LAB1 CHALLENGE 1 : YOUR CODE you should modify below codes.
    case T_SWITCH_TOU:
    case T_SWITCH_TOK:
        panic("T_SWITCH_** ??\n");
        break;
    case IRQ_OFFSET + IRQ_IDE1:
    case IRQ_OFFSET + IRQ_IDE2:
        /* do nothing */
        break;
    default:
        // in kernel, it must be a mistake
        if ((tf->tf_cs & 3) == 0) {
            print_trapframe(tf);
            panic("unexpected trap in kernel.\n");
        }
    }
}
```

可以看到，在`trap_dispatch`里面才是进行实质的中断处理，主要是判断当前中断的中断号(存储在`trapframe`结构体中)，我觉得应该可以用其他信息，来进行中断服务例程。

关于这个程序，唯一想说的还是那个全局变量`ticks_count`，我将它定义在了`init.c`里面，但是总感觉不够优雅，我也不知道应该怎么优雅...结果后来发现老师已经定义好了一个`ticks`全局变量。
