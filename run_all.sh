#! /bin/bash

SEQUITUR="sequitur/c++/sequitur"

if ! [[ -f $SEQUITUR ]]; then
    echo "sequitur executable was not found!"
    exit 0
fi

g++ -std=c++11 analyze_opportunity.cpp -O3 -o analyze_opportunity.o
g++ -std=c++11 analyze_stream_length.cpp -O3 -o analyze_stream_length.o

for trace in $(ls Traces/)
do
	input_file="Traces/$trace"
    trace_name=${trace%.*}
	echo "Trace: $trace_name"
	./$SEQUITUR -m 5000 -p -d < $input_file > "$trace_name-grammar.txt"
    echo "+++++++++++++++++++++++++++++++++++++++++++++++++++"
	./analyze_opportunity.o "$trace_name-grammar.txt" $(wc -l < $input_file)
    echo "+++++++++++++++++++++++++++++++++++++++++++++++++++"
    ./analyze_stream_length.o "$trace_name-grammar.txt"
    echo "---------------------------------------------------"
    echo "---------------------------------------------------"
done

rm *-grammar.txt*
rm analyze_opportunity.o analyze_stream_length.o

