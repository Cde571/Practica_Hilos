#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * fibonacci_serial.c - Version sin hilos usada para comparar tiempos.
 * Usa la misma definicion de fibonacci.c.
 */

static double GetTime(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

static size_t ParsePositiveSize(const char *text, const char *name) {
    char *end = NULL;
    errno = 0;
    unsigned long long value = strtoull(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || value == 0) {
        fprintf(stderr, "Error: %s debe ser un entero positivo. Valor recibido: '%s'\n", name, text);
        exit(EXIT_FAILURE);
    }
    return (size_t)value;
}

static void CalcFibonacci(unsigned long long *values, size_t n) {
    unsigned long long prev2 = 0ULL; /* F(-2) */
    unsigned long long prev1 = 1ULL; /* F(-1) */

    for (size_t i = 0; i < n; i += 1) {
        const unsigned long long current = prev1 + prev2;
        values[i] = current;
        prev2 = prev1;
        prev1 = current;
    }
}

static void PrintSequence(const unsigned long long *values, size_t n) {
    for (size_t i = 0; i < n; i += 1) {
        printf("%llu", values[i]);
        if (i + 1 < n) {
            printf(" ");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Uso: %s <N> [--quiet]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const size_t n = ParsePositiveSize(argv[1], "N");
    const int quiet = (argc == 3 && strcmp(argv[2], "--quiet") == 0);

    unsigned long long *values = (unsigned long long *)malloc(n * sizeof(unsigned long long));
    if (values == NULL) {
        perror("malloc values");
        return EXIT_FAILURE;
    }

    const double start = GetTime();
    CalcFibonacci(values, n);
    const double end = GetTime();

    if (!quiet) {
        PrintSequence(values, n);
    }
    printf("program=fibonacci_serial n=%zu time=%.9f\n", n, end - start);

    free(values);
    return EXIT_SUCCESS;
}
