#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * pi_p.c - Version paralela del calculo de pi usando Pthreads.
 * Estrategia: data parallelism. Cada hilo calcula una suma parcial local
 * sobre un sub-rango del bucle y retorna el resultado a main mediante
 * pthread_join. No se usan mutex dentro del bucle.
 */

typedef struct {
    long long start;
    long long end;
    double h;
} ThreadArgs;

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

static int ParsePositiveInt(const char *text, const char *name) {
    long long value = ParsePositiveLongLong(text, name);
    if (value > 1000000LL) {
        fprintf(stderr, "Error: %s demasiado grande.\n", name);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

/*
 * ThreadCalc(arg): funcion ejecutada por cada hilo.
 * Cada hilo calcula la suma parcial de f(x) para el rango [start, end).
 * Retorna un puntero dinamico a double para que main lo reciba con join.
 */
static void *ThreadCalc(void *arg) {
    ThreadArgs *data = (ThreadArgs *)arg;
    double local_sum = 0.0;

    for (long long i = data->start; i < data->end; i += 1) {
        const double x = data->h * ((double)i + 0.5);
        local_sum += f(x);
    }

    double *partial = (double *)malloc(sizeof(double));
    if (partial == NULL) {
        perror("malloc partial");
        pthread_exit(NULL);
    }
    *partial = local_sum;
    return partial;
}

/*
 * CalcPi(n, num_threads): calcula pi en paralelo.
 * Divide las n iteraciones entre num_threads hilos. Si n no es divisible
 * por num_threads, las primeras particiones reciben una iteracion adicional.
 */
double CalcPi(long long n, int num_threads) {
    if (num_threads <= 0) {
        fprintf(stderr, "Error: num_threads debe ser positivo.\n");
        exit(EXIT_FAILURE);
    }

    if ((long long)num_threads > n) {
        num_threads = (int)n;
    }

    pthread_t *threads = (pthread_t *)malloc((size_t)num_threads * sizeof(pthread_t));
    ThreadArgs *args = (ThreadArgs *)malloc((size_t)num_threads * sizeof(ThreadArgs));

    if (threads == NULL || args == NULL) {
        perror("malloc threads/args");
        free(threads);
        free(args);
        exit(EXIT_FAILURE);
    }

    const double h = 1.0 / (double)n;
    const long long base_chunk = n / (long long)num_threads;
    const long long remainder = n % (long long)num_threads;

    long long current = 0;
    for (int t = 0; t < num_threads; t += 1) {
        const long long extra = (t < remainder) ? 1 : 0;
        const long long chunk = base_chunk + extra;

        args[t].start = current;
        args[t].end = current + chunk;
        args[t].h = h;
        current += chunk;

        const int rc = pthread_create(&threads[t], NULL, ThreadCalc, &args[t]);
        if (rc != 0) {
            fprintf(stderr, "Error en pthread_create para hilo %d: %s\n", t, strerror(rc));
            free(threads);
            free(args);
            exit(EXIT_FAILURE);
        }
    }

    double total_sum = 0.0;
    for (int t = 0; t < num_threads; t += 1) {
        void *ret = NULL;
        const int rc = pthread_join(threads[t], &ret);
        if (rc != 0) {
            fprintf(stderr, "Error en pthread_join para hilo %d: %s\n", t, strerror(rc));
            free(threads);
            free(args);
            exit(EXIT_FAILURE);
        }

        if (ret == NULL) {
            fprintf(stderr, "Error: el hilo %d no retorno una suma parcial valida.\n", t);
            free(threads);
            free(args);
            exit(EXIT_FAILURE);
        }

        double *partial = (double *)ret;
        total_sum += *partial;
        free(partial);
    }

    free(threads);
    free(args);
    return h * total_sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <n> <num_hilos>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 2000000000 4\n", argv[0]);
        return EXIT_FAILURE;
    }

    const long long n = ParsePositiveLongLong(argv[1], "n");
    const int num_threads = ParsePositiveInt(argv[2], "num_hilos");

    const double start = GetTime();
    const double pi = CalcPi(n, num_threads);
    const double end = GetTime();
    const double elapsed = end - start;

    printf("program=pi_p n=%lld threads=%d pi=%.15f time=%.9f\n", n, num_threads, pi, elapsed);
    return EXIT_SUCCESS;
}
