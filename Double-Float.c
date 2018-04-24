#include <math.h>
#include <stdio.h>

double  DoubleToTheFloat(double base, float power) {
    return pow(base, power);
}

int main() {
    double (*FloatPowerOfDouble)(float, double) =
        (double (*)(float, double))&DoubleToTheFloat;

    printf("(0.99)^100: %lf \n", DoubleToTheFloat(0.99, 100));   // OK
    printf("(0.99)^100: %lf \n", FloatPowerOfDouble(100, 0.99)); // Uh-oh...
}

