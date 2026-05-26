#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

N="${1:-2000000000}"
CORES="${2:-$(nproc 2>/dev/null || echo 4)}"
MAX_THREADS=$((2 * CORES))

make all
mkdir -p results
CSV="results/pi_benchmark.csv"
echo "threads,tp_seconds,speedup,efficiency" > "$CSV"

echo "Midiendo Ts con n=$N"
SERIAL_OUT=$(./bin/pi_s "$N")
echo "$SERIAL_OUT"
TS=$(echo "$SERIAL_OUT" | sed -n 's/.*time=\([0-9.]*\).*/\1/p')

echo "threads,tp_seconds,speedup,efficiency" > "$CSV"
THREADS=1
while [ "$THREADS" -le "$MAX_THREADS" ]; do
    echo "Midiendo Tp con $THREADS hilos"
    OUT=$(./bin/pi_p "$N" "$THREADS")
    echo "$OUT"
    TP=$(echo "$OUT" | sed -n 's/.*time=\([0-9.]*\).*/\1/p')
    SPEEDUP=$(awk -v ts="$TS" -v tp="$TP" 'BEGIN { printf "%.9f", ts/tp }')
    EFF=$(awk -v sp="$SPEEDUP" -v th="$THREADS" 'BEGIN { printf "%.9f", sp/th }')
    echo "$THREADS,$TP,$SPEEDUP,$EFF" >> "$CSV"
    THREADS=$((THREADS * 2))
done

echo "Resultados guardados en $CSV"
