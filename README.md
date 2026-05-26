# Práctica 4 - API de Hilos POSIX

**Curso:** Sistemas Operativos  
**Tema:** API de Hilos / Pthreads  
**Autor:** Cristian Echeverry  
**Modalidad:** Trabajo individual  
**Correo:** [COMPLETAR CORREO INSTITUCIONAL]  
**Documento:** [COMPLETAR DOCUMENTO]  
**Video de sustentación:** [PEGAR ENLACE DEL VIDEO DE 10 MINUTOS]

---

## 1. Descripción general

Este repositorio contiene la solución de la práctica de laboratorio sobre la API de hilos POSIX. La solución desarrolla dos partes principales:

1. Paralelización del cálculo de π usando Pthreads.
2. Generación de la secuencia de Fibonacci mediante un hilo trabajador.

El objetivo principal es aplicar `pthread_create`, `pthread_join` y transferencia de argumentos a hilos, además de medir el impacto del paralelismo con métricas como **Speedup** y **Eficiencia**.

---

## 2. Estructura del repositorio

```text
.
├── README.md
├── CHECKLIST.md
├── Makefile
├── src/
│   ├── pi.c
│   ├── pi_p.c
│   ├── fibonacci.c
│   └── fibonacci_serial.c
├── notebooks/
│   └── analisis.ipynb
├── scripts/
│   ├── run_tests.sh
│   └── run_benchmarks.sh
├── report/
│   ├── informe_practica4.tex
│   └── informe_practica4.pdf
└── results/
```

---

## 3. Compilación

La práctica solicita los siguientes comandos base:

```bash
gcc -o pi_s src/pi.c -lm
gcc -o pi_p src/pi_p.c -lpthread -lm
gcc -o fibonacci src/fibonacci.c -lpthread
```

También se incluye un `Makefile` para compilar todo de forma más cómoda:

```bash
make all
```

Esto genera los ejecutables en la carpeta `bin/`:

```text
bin/pi_s
bin/pi_p
bin/fibonacci
bin/fibonacci_serial
```

---

## 4. Ejecución

### 4.1 Cálculo serial de π

```bash
./bin/pi_s 2000000000
```

Salida esperada con formato:

```text
program=pi_s n=2000000000 pi=3.141592653589... time=...
```

### 4.2 Cálculo paralelo de π

```bash
./bin/pi_p 2000000000 4
```

Donde:

- `2000000000` es el número de rectángulos `n`.
- `4` es el número de hilos.

Salida esperada con formato:

```text
program=pi_p n=2000000000 threads=4 pi=3.141592653589... time=...
```

### 4.3 Fibonacci con un hilo trabajador

```bash
./bin/fibonacci 10
```

Como el enunciado define `F(-2)=0`, `F(-1)=1` y `F(n)=F(n-1)+F(n-2)`, la salida para `N=10` es:

```text
1 2 3 5 8 13 21 34 55 89
```

---

## 5. Documentación de funciones desarrolladas

### 5.1 `src/pi.c`

| Función | Descripción |
|---|---|
| `f(double x)` | Evalúa la función matemática `4/(1+x²)` usada en la integral. |
| `GetTime(void)` | Retorna el tiempo actual usando `clock_gettime(CLOCK_MONOTONIC)`. |
| `ParsePositiveLongLong(...)` | Valida que el argumento de línea de comandos sea un entero positivo. |
| `CalcPi(long long n)` | Calcula π de forma serial usando la regla del punto medio. |
| `main(...)` | Lee `n`, mide el tiempo exclusivo de `CalcPi` e imprime resultado y tiempo. |

### 5.2 `src/pi_p.c`

| Función / estructura | Descripción |
|---|---|
| `ThreadArgs` | Estructura con `start`, `end` y `h` para indicar el rango de iteraciones de cada hilo. |
| `ThreadCalc(void *arg)` | Función ejecutada por cada hilo. Calcula una suma parcial local y retorna un `double*`. |
| `CalcPi(long long n, int num_threads)` | Divide el rango total de iteraciones entre hilos, crea los hilos, recolecta las sumas parciales con `pthread_join` y calcula π. |
| `main(...)` | Lee `n` y `num_threads`, mide el tiempo de `CalcPi` e imprime resultado y tiempo. |

### 5.3 `src/fibonacci.c`

| Función / estructura | Descripción |
|---|---|
| `FibArgs` | Estructura que permite pasar al hilo trabajador el puntero al arreglo compartido y el valor `N`. |
| `GenerateFibonacci(void *arg)` | Calcula los `N` valores de Fibonacci y los almacena en el arreglo compartido. |
| `PrintSequence(...)` | Imprime la secuencia almacenada en el arreglo. |
| `main(...)` | Reserva memoria con `malloc`, crea el hilo trabajador, espera con `pthread_join` y luego imprime los resultados. |

### 5.4 `src/fibonacci_serial.c`

Esta versión se agregó para cumplir el análisis solicitado en el notebook: comparar el cálculo de Fibonacci con y sin hilos para valores grandes de `N`.

---

## 6. Estrategia de paralelización de π

La paralelización se hizo con **data parallelism**. El bucle original:

```c
for (i = 0; i < n; i++) {
    fX = fH * ((double)i + 0.5);
    fSum += f(fX);
}
```

se divide entre `T` hilos. Cada hilo recibe un subrango `[start, end)` y calcula una suma parcial en una variable local. Esta decisión evita usar `mutex` dentro del bucle principal, porque un mutex en cada iteración introduciría alta contención y perjudicaría el rendimiento.

Cuando todos los hilos terminan, `main` usa `pthread_join` para recibir cada suma parcial, las acumula y multiplica por `fH` para obtener la aproximación final de π.

---

## 7. Sincronización

La sincronización se realiza con `pthread_join`.

En `pi_p.c`, `pthread_join` permite que el hilo principal espere la finalización de cada hilo trabajador y reciba su suma parcial.

En `fibonacci.c`, `pthread_join` evita que `main` imprima el arreglo antes de que el hilo trabajador termine de calcular la secuencia.

---

## 8. Pruebas realizadas

Se incluye el script:

```bash
./scripts/run_tests.sh
```

Este script compila y ejecuta pruebas cortas:

1. `pi_s` con `n=1000000`.
2. `pi_p` con `n=1000000` y 1 hilo.
3. `pi_p` con `n=1000000` y 2 hilos.
4. `fibonacci` con `N=10`.
5. `fibonacci` con `N=15`.

Para ejecutar el benchmark completo de π con `n=2000000000`:

```bash
./scripts/run_benchmarks.sh 2000000000
```

El notebook `notebooks/analisis.ipynb` también compila, ejecuta, construye la tabla de resultados y genera el gráfico de Speedup.

---

## 9. Notebook de análisis

El notebook se encuentra en:

```text
notebooks/analisis.ipynb
```

Contenido del notebook:

1. Compilación de los programas.
2. Medición de `Ts` usando `./pi_s 2000000000`.
3. Medición de `Tp(N)` usando `./pi_p 2000000000 N`, con `N = 1, 2, 4, 8, ...` hasta `2 × núcleos CPU`.
4. Tabla con `Tp`, `Speedup = Ts/Tp` y `Eficiencia = Speedup/N`.
5. Gráfico de Speedup.
6. Salida de `./fibonacci 15`.
7. Prueba de Fibonacci sin hilos con `N > 100000`.
8. Análisis de transferencia de datos y uso de `pthread_join`.

---

## 10. Problemas presentados y soluciones

| Problema | Solución aplicada |
|---|---|
| Posible contención si todos los hilos actualizan una variable global compartida. | Se usaron sumas parciales locales y retorno por `pthread_join`, evitando mutex dentro del bucle. |
| División no exacta de `n` entre el número de hilos. | Se distribuyó el residuo entre los primeros hilos para cubrir todas las iteraciones sin repetir ni omitir. |
| Medición imprecisa del tiempo. | Se usó `clock_gettime(CLOCK_MONOTONIC)` y se midió solo alrededor de `CalcPi`. |
| Riesgo de imprimir Fibonacci antes de que el hilo termine. | `main` espera al trabajador mediante `pthread_join`. |
| Crecimiento de Fibonacci para `N` grande produce overflow en `unsigned long long`. | Se documenta que la prueba para `N > 100000` mide desempeño y sincronización, no exactitud matemática de valores grandes. |

---

## 11. Manifiesto de transparencia sobre uso de IA generativa

Se utilizó IA generativa como apoyo para:

1. Organizar la estructura del repositorio.
2. Generar una versión inicial del código en C.
3. Redactar documentación técnica del README.
4. Crear el notebook de análisis y el informe en LaTeX.
5. Revisar el cumplimiento punto por punto de la rúbrica del enunciado.

La validación final, compilación, ejecución de pruebas, revisión del código, grabación del video y entrega en Moodle deben ser realizadas por el estudiante.

---

## 12. Guion sugerido del video de sustentación

Duración aproximada: 10 minutos.

1. **Presentación del repositorio** (1 min): mostrar carpetas `src/`, `notebooks/`, `scripts/` y `report/`.
2. **Parte 1 - pi_p.c** (3 min): explicar cómo se divide el rango de iteraciones, cómo se crean los hilos y cómo se recolectan sumas parciales.
3. **Parte 2 - fibonacci.c** (2 min): explicar el arreglo compartido, la estructura `FibArgs` y el uso de `pthread_join`.
4. **Demostración** (2 min): ejecutar `pi_s`, `pi_p` con 1 hilo, `pi_p` con varios hilos y `fibonacci`.
5. **Análisis** (2 min): mostrar tabla y gráfico de Speedup en el notebook, explicar overhead, speedup máximo y eficiencia.

---

## 13. Conclusiones

1. La paralelización del cálculo de π permite dividir un trabajo computacionalmente intensivo entre varios hilos, reduciendo el tiempo de ejecución cuando existe suficiente capacidad de CPU.
2. El uso de sumas parciales locales evita la contención que aparecería si varios hilos actualizaran una misma variable compartida dentro del bucle.
3. `pthread_join` cumple un papel fundamental porque sincroniza el hilo principal con los hilos trabajadores y permite recolectar resultados de forma segura.
4. El Speedup no crece indefinidamente con el número de hilos, debido al overhead de creación, sincronización, planificación y límites físicos del procesador.
5. La eficiencia tiende a disminuir cuando aumenta el número de hilos, especialmente cuando se supera el número de núcleos físicos disponibles.
6. En el problema de Fibonacci, el hilo trabajador demuestra el paso de argumentos y el uso de memoria compartida, aunque el cálculo secuencial de la serie no ofrece paralelismo real con un único hilo.

---

## 14. Referencias

- Remzi H. Arpaci-Dusseau y Andrea C. Arpaci-Dusseau. *Operating Systems: Three Easy Pieces*.
- Capítulo 26: Threads Intro.
- Capítulo 27: Thread API.
- Diapositivas de apoyo: Interlude: Thread API.
