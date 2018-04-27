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
The function which has been called by main routine can be identified by this line:    
``` 0x00000000004006a4 <+36>: call   0x400650 <DoubleToTheInt> ```
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
```
0x0000000000400688 <+8>:	movsd  xmm0,QWORD PTR ds:0x400778  
```
MOVSD reprsents moving the  Scalar Double-Precision floating-Point Value details can be found [here](http://www.felixcloutier.com/x86/MOVSD.html).
```
gef➤  x 0x400778  
0x400778: 0x7ae147ae  
```
