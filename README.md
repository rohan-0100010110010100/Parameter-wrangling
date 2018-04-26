# Parameter-wrangling

Analysis of SSE &amp; demonstration to show the ill-effect of function parameters wrangling/re-arrangement.

The result of this experiment is based upon the Platform ABI and artchitecture , the following are the requirement for this experiment:

# Requirments 

* AMD64 ABI, which can be found at [here](https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-1.0.pdf)
* Linux , 64 bit

# Analysis & Insights

The code in Double-Int.c is never supossed to define the function IntPowerOfDouble — because there is no function IntPowerOfDouble in reality. It's a function pointer that points to DoubleToTheInt, but with parameters that infer that it requires its integer arguments to come before its double argument.

**Output Of Double-Int.c**

rohan@hackerspace:-$ clang-3.5 -lm Double-Int.c  
rohan@hackerspace:-$ ./a.out  
(0.99)^100: 0.366032  
(0.99)^100: 0.366032  


Now try changing all the int arguments to float — you'll see that FloatPowerOfDouble does something even stranger. That is,

**Output Of Double-Float.c**

rohan@hackerspace:-$ clang-3.5 -lm Double-Float.c  
rohan@hackerspace:-$ ./a.out  
(0.99)^100: 0.366032  
(0.99)^100: 0.000000  
