# Parameter-wrangling

Analysis of SIMD &amp; demonstration to show the ill-effect of function parameters wrangling/re-arrangement.

The code in Double-Int.c is never supossed to define the function IntPowerOfDouble — because there is no function IntPowerOfDouble in reality. It's a function pointer that points to DoubleToTheInt, but with parameters that infer that it requires its integer arguments to come before its double argument.

## Output Of Double-Int.c

(0.99)^100: 0.366032 
(0.99)^100: 0.366032 


