#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * fibonacci.c - Generador de la secuencia de Fibonacci usando un hilo trabajador.
 * Se sigue la definicion dada en el enunciado:
 * F(-2)=0, F(-1)=1, F(n)=F(n-1)+F(n-2), para n >= 0.
 * Por tanto, F(0)=1, F(1)=2, F(2)=3, ...
 */

typedef struct {
    unsigned long long *values;
    size_t n;
} FibArgs;

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

/*
 * GenerateFibonacci(arg): funcion del hilo trabajador.
 * Recibe un puntero a FibArgs, calcula n valores y los escribe en el
 * arreglo compartido asignado por main.
 */
static void *GenerateFibonacci(void *arg) {
    FibArgs *data = (FibArgs *)arg;
    unsigned long long prev2 = 0ULL; /* F(-2) */
    unsigned long long prev1 = 1ULL; /* F(-1) */

    for (size_t i = 0; i < data->n; i += 1) {
        const unsigned long long current = prev1 + prev2;
        data->values[i] = current;
        prev2 = prev1;
        prev1 = current;
    }

    return NULL;
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
        fprintf(stderr, "Ejemplo: %s 10\n", argv[0]);
        return EXIT_FAILURE;
    }

    const size_t n = ParsePositiveSize(argv[1], "N");
    const int quiet = (argc == 3 && strcmp(argv[2], "--quiet") == 0);

    unsigned long long *values = (unsigned long long *)malloc(n * sizeof(unsigned long long));
    if (values == NULL) {
        perror("malloc values");
        return EXIT_FAILURE;
    }

    FibArgs args;
    args.values = values;
    args.n = n;

    pthread_t worker;
    const int rc_create = pthread_create(&worker, NULL, GenerateFibonacci, &args);
    if (rc_create != 0) {
        fprintf(stderr, "Error en pthread_create: %s\n", strerror(rc_create));
        free(values);
        return EXIT_FAILURE;
    }

    const int rc_join = pthread_join(worker, NULL);
    if (rc_join != 0) {
        fprintf(stderr, "Error en pthread_join: %s\n", strerror(rc_join));
        free(values);
        return EXIT_FAILURE;
    }

    if (!quiet) {
        PrintSequence(values, n);
    } else {
        printf("program=fibonacci n=%zu status=ok\n", n);
    }

    free(values);
    return EXIT_SUCCESS;
}
