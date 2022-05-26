#! /bin/bash

## Compile sequitur
SEQUITUR_DIR="./sequitur"
SEQUITUR_BIN="./sequitur/sequitur"
cd "$SEQUITUR_DIR"
make -j &>/dev/null
if [[ ! -f sequitur ]]; then
    echo "Sequitur executable was not found!"
    exit 0
fi
cd ..

make -j &>/dev/null
ANALYZE_BIN="measure_opp.out"

for TRACE in $(ls Traces/); do
    INPUT_FILE="Traces/$TRACE"
    TRACE_NAME="${TRACE%.*}"
    GRAMMAR_FILE="$TRACE_NAME.gr"

    echo "Trace: $TRACE_NAME"
    ./"$SEQUITUR_BIN" -m 5000 -p -d < "$INPUT_FILE" > "$GRAMMAR_FILE" 2>/dev/null
    ./"$ANALYZE_BIN" "$GRAMMAR_FILE" "$(wc -l < $INPUT_FILE)"

    rm "$GRAMMAR_FILE"
    echo "==================================="
done

make clean &>/dev/null
