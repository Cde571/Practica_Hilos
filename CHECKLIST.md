# Checklist de cumplimiento - Práctica 4 API de Hilos

Autor: Cristian Echeverry  
Trabajo: Individual

| Requisito | Estado | Evidencia en el repositorio |
|---|---:|---|
| Crear `pi.c` serial | OK | `src/pi.c` |
| Duplicar y paralelizar como `pi_p.c` | OK | `src/pi_p.c` |
| `pi_p.c` recibe número de hilos por línea de comandos | OK | `./bin/pi_p <n> <num_hilos>` |
| Dividir el rango 0 a n-1 entre T hilos | OK | Función `CalcPi` en `src/pi_p.c` |
| Evitar mutex dentro del bucle principal | OK | Cada hilo usa `local_sum` y retorna suma parcial |
| Usar `pthread_create` | OK | `src/pi_p.c`, `src/fibonacci.c` |
| Usar `pthread_join` | OK | Recolección de resultados parciales y sincronización de Fibonacci |
| Medir tiempo de `CalcPi` | OK | `GetTime()` en `pi.c` y `pi_p.c` |
| Crear `fibonacci.c` | OK | `src/fibonacci.c` |
| Main asigna arreglo compartido con `malloc` | OK | `src/fibonacci.c` |
| Main pasa arreglo y N al hilo trabajador | OK | Estructura `FibArgs` |
| Hilo trabajador llena el arreglo | OK | `GenerateFibonacci` |
| Main espera con `pthread_join` antes de imprimir | OK | `src/fibonacci.c` |
| Notebook `analisis.ipynb` | OK | `notebooks/analisis.ipynb` |
| Tabla Speedup/Eficiencia | OK | Se genera automáticamente en el notebook |
| Gráfico Speedup | OK | Se genera automáticamente en el notebook |
| README con informe | OK | `README.md` |
| Documentación de funciones | OK | `README.md` y comentarios en código |
| Problemas y soluciones | OK | `README.md` |
| Pruebas realizadas | OK | `README.md` y `scripts/run_tests.sh` |
| Manifiesto de transparencia | OK | `README.md` |
| Cinco conclusiones | OK | `README.md` |
| Códigos en carpeta aparte | OK | `src/` |
| Video de 10 minutos | PENDIENTE | Crear video y reemplazar enlace en `README.md` |
| Número de documento y correo | PENDIENTE | Reemplazar campos en `README.md` |
