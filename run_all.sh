#! /bin/bash

SEQUITUR_DIR="./sequitur"
SEQUITUR_BIN="$SEQUITUR_DIR/sequitur"
ANALYZE_BIN="analyze_seq.out"
TRACE_DIR="Traces"

cd "$SEQUITUR_DIR"
make -j &>/dev/null
cd - &>/dev/null
if [[ ! -f "$SEQUITUR_BIN" ]]; then
    echo "$SEQUITUR_BIN was not found!"
    exit 1
fi

make -j &>/dev/null
if [[ ! -f "$ANALYZE_BIN" ]]; then
    echo "$ANALYZE_BIN was not found!"
    exit 1
fi

if [[ ! -d "$TRACE_DIR" ]]; then
    echo "$TRACE_DIR was not found!"
    exit 1
fi

for TRACE in $TRACE_DIR/*; do
    TRACE_NAME="${TRACE%.*}"
    GRAMMAR_FILE="$TRACE_NAME.gr"

    echo "Trace: $TRACE_NAME"
    ./"$SEQUITUR_BIN" -m 5000 -p -d < "$TRACE" > "$GRAMMAR_FILE" 2>/dev/null
    ./"$ANALYZE_BIN" "$GRAMMAR_FILE" $(wc -l < "$TRACE")

    rm "$GRAMMAR_FILE"
    echo "==================================="
done

make clean &>/dev/null
