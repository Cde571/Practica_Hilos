#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * pi.c - Version serial del calculo de pi por integracion numerica.
 * Practica 4 - API de Hilos.
 */

static double f(double x) {
    return 4.0 / (1.0 + x * x);
}

static double GetTime(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

static long long ParsePositiveLongLong(const char *text, const char *name) {
    char *end = NULL;
    errno = 0;
    long long value = strtoll(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || value <= 0) {
        fprintf(stderr, "Error: %s debe ser un entero positivo. Valor recibido: '%s'\n", name, text);
        exit(EXIT_FAILURE);
    }
    return value;
}

/*
 * CalcPi(n): aproxima pi usando la regla del punto medio con n rectangulos.
 * Parametro:
 *   n: numero total de subintervalos.
 * Retorna:
 *   aproximacion de pi.
 */
double CalcPi(long long n) {
    const double fH = 1.0 / (double)n;
    double fSum = 0.0;

    for (long long i = 0; i < n; i += 1) {
        const double fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }

    return fH * fSum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <n>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 2000000000\n", argv[0]);
        return EXIT_FAILURE;
    }

    const long long n = ParsePositiveLongLong(argv[1], "n");

    const double start = GetTime();
    const double pi = CalcPi(n);
    const double end = GetTime();
    const double elapsed = end - start;

    printf("program=pi_s n=%lld pi=%.15f time=%.9f\n", n, pi, elapsed);
    return EXIT_SUCCESS;
}
