# Parameter-wrangling

Analysis of SSE &amp; demonstration to show the ill-effect of function parameters wrangling/re-arrangement.

The result of this experiment is based upon the Platform ABI and artchitecture , the following are the requirement for this experiment:

## Requirments 

* AMD64 ABI, which can be found at [here](https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-1.0.pdf)
* Linux , 64 bit

## Analysis & Insights

The code in Double-Int.c is never supossed to define the function IntPowerOfDouble — because there is no function IntPowerOfDouble in reality. It's a function pointer that points to DoubleToTheInt, but with parameters that infer that it requires its integer arguments to come before its double argument.

**Output Of Double-Int.c**
```
rohan@hackerspace:-$ clang-3.5 -lm Double-Int.c  
rohan@hackerspace:-$ ./a.out  
(0.99)^100: 0.366032  
(0.99)^100: 0.366032  
```

Now try changing all the int arguments to float — you'll see that FloatPowerOfDouble does something even stranger. That is,

**Output Of Double-Float.c**
```
rohan@hackerspace:-$ clang-3.5 -lm Double-Float.c  
rohan@hackerspace:-$ ./a.out  
(0.99)^100: 0.366032  
(0.99)^100: 0.000000  
```
Now I'm using gef or GDB Enhanced Features for debugging the Double-Int.c, so I did  disassembly on the main:  
```
gef➤  disas main  
Dump of assembler code for function main:  
   0x0000000000400680 <+0>:	push   rbp  
   0x0000000000400681 <+1>:	mov    rbp,rsp  
   0x0000000000400684 <+4>:	sub    rsp,0x10  
   0x0000000000400688 <+8>:	movsd  xmm0,QWORD PTR ds:0x400778  
   0x0000000000400691 <+17>:	mov    edi,0x64  
   0x0000000000400696 <+22>:	movabs rax,0x400650  
   0x00000000004006a0 <+32>:	mov    QWORD PTR [rbp-0x8],rax  
   0x00000000004006a4 <+36>:	call   0x400650 <DoubleToTheInt>  
   0x00000000004006a9 <+41>:	movabs rdi,0x400780  
   0x00000000004006b3 <+51>:	mov    al,0x1  
   0x00000000004006b5 <+53>:	call   0x400510 <printf@plt>  
   0x00000000004006ba <+58>:	mov    edi,0x64  
   0x00000000004006bf <+63>:	movsd  xmm0,QWORD PTR ds:0x400778  
   0x00000000004006c8 <+72>:	mov    DWORD PTR [rbp-0xc],eax  
   0x00000000004006cb <+75>:	call   QWORD PTR [rbp-0x8]  
   0x00000000004006ce <+78>:	movabs rdi,0x400780  
   0x00000000004006d8 <+88>:	mov    al,0x1  
   0x00000000004006da <+90>:	call   0x400510 <printf@plt>  
   0x00000000004006df <+95>:	mov    ecx,0x0  
   0x00000000004006e4 <+100>:	mov    DWORD PTR [rbp-0x10],eax  
   0x00000000004006e7 <+103>:	mov    eax,ecx  
   0x00000000004006e9 <+105>:	add    rsp,0x10  
   0x00000000004006ed <+109>:	pop    rbp  
   0x00000000004006ee <+110>:	ret      
End of assembler dump.  
```
The function which has been called by main routine in  cases modified order & original order can be identified by this line:    
``` 0x00000000004006a4 <+36>: call   0x400650 <DoubleToTheInt> ```   --> Original Order
``` 0x00000000004006cb <+75>:	call   QWORD PTR [rbp-0x8]```   --> Modified Order
which is:
```
double  DoubleToTheInt(double base, int power) {  
    return pow(base, power);  
}  
```
Now beacuse we have defined function which has reordered the argument of ```DoubleToInt(double,int)``` which is defined as pointer to ```DoubleToInt(double,int)``` function:

```
 double (*IntPowerOfDouble)(int, double) =
        (double (*)(int, double))&DoubleToTheInt;
```
Now we have to see that how these modified arguments get into the registers as per AMD64 ABI:

Now examine the critical sections about registers invovled with functions:  

```0x0000000000400688 <+8>:	movsd  xmm0,QWORD PTR ds:0x400778```    --> Original Order ( DoubleToInt (double,int) )  
```0x00000000004006bf <+63>:	movsd  xmm0,QWORD PTR ds:0x400778 ```   --> Modified Order ( IntPowerOfDouble (int, double))  
```0x0000000000400691 <+17>:	mov    edi,0x64  ``` --> Original Order ( DoubleToInt (double,int) )   
```0x00000000004006ba <+58>:	mov    edi,0x64  ``` --> Modiifed Order ( IntPowerOfDouble (int, double))  

MOVSD reprsents moving the  Scalar Double-Precision floating-Point Value in registers. Details can be found [here](http://www.felixcloutier.com/x86/MOVSD.html).
```
gef➤  x 0x400778  
0x400778: 0x7ae147ae  
```
Now we set the breakpoint at ``` 0x00000000004006a4 <+36>: call   0x400650 <DoubleToTheInt> ```
```
gef➤  b *0x00000000004006a4  
Breakpoint 1 at 0x4006a4  
gef➤  r  
Starting program: /home/rohan/a.out     
Breakpoint 1, 0x00000000004006a4 in main ()  

──────────────────────────────────────────────────────[ registers ]────  
$rax   : 0x0000000000400650  →  <DoubleToTheInt+0> push rbp  
$rbx   : 0x0000000000000000  
$rcx   : 0x0000000000000000  
$rdx   : 0x00007fffffffdd68  →  0x00007fffffffe137  →  "LC_PAPER=en_IN"  
$rsp   : 0x00007fffffffdc60  →  0x00007fffffffdd50  →  0x0000000000000001  
$rbp   : 0x00007fffffffdc70  →  0x0000000000000000  
$rsi   : 0x00007fffffffdd58  →  0x00007fffffffe125  →  "/home/rohan/a.out"  
$rdi   : 0x0000000000000064  
$rip   : 0x00000000004006a4  →  <main+36> call 0x400650 <DoubleToTheInt>  
$r8    : 0x00007ffff7acee80  →  0x0000000000000000  
$r9    : 0x00007ffff7dea600  →  <_dl_fini+0> push rbp  
$r10   : 0x00007fffffffdb00  →  0x0000000000000000  
$r11   : 0x00007ffff772ce50  →  <__libc_start_main+0> push r14  
$r12   : 0x0000000000400550  →  <_start+0> xor ebp, ebp  
$r13   : 0x00007fffffffdd50  →  0x0000000000000001  
$r14   : 0x0000000000000000  
$r15   : 0x0000000000000000  
$eflags: [carry PARITY adjust zero sign trap INTERRUPT direction overflow resume virtualx86 identification]  
$es: 0x0000  $cs: 0x0033  $ss: 0x002b  $fs: 0x0000  $gs: 0x0000  $ds: 0x0000    
```
Also we set another break point at ``0x00000000004006cb <+75>:	call   QWORD PTR [rbp-0x8] ```
```
gef➤  b *0x00000000004006cb
Breakpoint 3 at 0x4006cb
gef➤  c
Continuing.
(0.99)^100: 0.366032 

Breakpoint 3, 0x00000000004006cb in main ()

──────────────────────────────────────────────────[ registers ]────
$rax   : 0x0000000000000016
$rbx   : 0x0000000000000000
$rcx   : 0x0000000000000014
$rdx   : 0x00007ffff7acf9e0  →  0x0000000000000000
$rsp   : 0x00007fffffffdc60  →  0x00000016ffffdd50
$rbp   : 0x00007fffffffdc70  →  0x0000000000000000
$rsi   : 0x000000007fffffeb
$rdi   : 0x0000000000000064
$rip   : 0x00000000004006cb  →  <main+75> call QWORD PTR [rbp-0x8]
$r8    : 0x0000000000000008
$r9    : 0x0000000000000008
$r10   : 0x00007ffff7acc6a0  →  0x0000000000000000
$r11   : 0x0000000000000246
$r12   : 0x0000000000400550  →  <_start+0> xor ebp, ebp
$r13   : 0x00007fffffffdd50  →  0x0000000000000001
$r14   : 0x0000000000000000
$r15   : 0x0000000000000000
$eflags: [carry parity adjust zero sign trap INTERRUPT direction overflow resume virtualx86 identification]
$es: 0x0000  $cs: 0x0033  $ss: 0x002b  $fs: 0x0000  $gs: 0x0000  $ds: 0x0000  

```
## SIMD Analysis:  

In 64 bit mode, fisrt 4 parameter are transferred in register RAX, RCX, RDX, R8, R9 in case of integer arguments

In case of Linux the program works like this:
* Floating-point arguments are placed, in order, into SSE registers, labeled XMM0, XMM1, etc.
* Integer and pointer arguments are placed, in order, into general registers, labeled RDX, RCX, etc.

Now from above this is clear that the arguments of the function are in ```xmm0``` and ```edi```. The "0.99" or ```0x7ae147ae ``` is passed to ```xmm0``` beacuse its a scalar double precision number and "100" or ```0x64``` is passed to ```edi```. 

Even after we reorder the argument for the function DoubleToInt, we exactly got the same result as we would have got without reordering. The elaborated explaination can be inferred from below:

Contents of SSE registers before the parameter enters function: 

|  XMM0 |  XMM1|   YMM0|YMM1|
|---|---|---|---|
|  0.99 | N/A  |  N/A | N/A|

Content of source index & destination index register before the parameter enters to function:

|  RDI | RSI |
|---|---|
| 0x64 or 100 | N/A |

As described above the state of the registers is same in using the function 
