#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

make all

echo "\n[Prueba 1] pi serial con n=1000000"
./bin/pi_s 1000000

echo "\n[Prueba 2] pi paralelo con n=1000000 y 1 hilo"
./bin/pi_p 1000000 1

echo "\n[Prueba 3] pi paralelo con n=1000000 y 2 hilos"
./bin/pi_p 1000000 2

echo "\n[Prueba 4] Fibonacci con N=10"
./bin/fibonacci 10

echo "\n[Prueba 5] Fibonacci con N=15"
./bin/fibonacci 15
