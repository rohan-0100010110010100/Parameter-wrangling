# Parameter-wrangling

Analysis of SIMD &amp; demonstration to show the ill-effect of function parameters wrangling/re-arrangement.

The code in Double-Int.c is never supossed to define the function IntPowerOfDouble — because there is no function IntPowerOfDouble in reality. It's a function pointer that points to DoubleToTheInt, but with parameters that infer that it requires its integer arguments to come before its double argument.

## Output Of Double-Int.c

rohan@hackerspace:~$ clang-3.5 -lm punny.c \s
rohan@hackerspace:~$ ./a.out\s
(0.99)^100: 0.366032 \s
(0.99)^100: 0.366032 \s


