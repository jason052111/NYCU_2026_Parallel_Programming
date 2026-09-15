#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long toss_darts(long long number_of_tosses) {
    long long number_in_circle = 0;

    for (long long toss = 0; toss < number_of_tosses; toss++) {
        double x = (double)rand() / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand() / RAND_MAX * 2.0 - 1.0;

        if (x * x + y * y <= 1.0) {
            number_in_circle++;
        }
    }

    return number_in_circle;
}

int main() {
    srand(time(NULL));

    long long number_of_tosses = 10000000;
    long long number_in_circle = toss_darts(number_of_tosses);

    double pi_estimate = 4.0 * number_in_circle / number_of_tosses;

    printf("%f\n", pi_estimate);

    return 0;
}