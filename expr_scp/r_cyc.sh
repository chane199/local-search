#!/bin/bash

MAX=100000000

for((i=100; i<=20; i=i+1)) do {
	./msp ../scp/scpcyc09.txt 0 $i $MAX >>cyc09.txt;
}
done

for((i=10; i<=20; i=i+1)) do {
	./msp ../scp/scpcyc10.txt 1792 $i $MAX >>cyc10.txt;
}
done

for((i=10; i<=20; i=i+1)) do {
	./msp ../scp/scpcyc11.txt 0 $i $MAX >>cyc11.txt;
}
done
