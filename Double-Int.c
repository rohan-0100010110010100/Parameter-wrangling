
#include <math.h>
#include <stdio.h>

double  DoubleToTheInt(double base, int power) {
    return pow(base, power);
}

int main() {
    // cast to a function pointer with arguments reversed
    double (*IntPowerOfDouble)(int, double) =
        (double (*)(int, double))&DoubleToTheInt;

    printf("(0.99)^100: %lf \n", DoubleToTheInt(0.99, 100));
    printf("(0.99)^100: %lf \n", IntPowerOfDouble(100, 0.99));
}
