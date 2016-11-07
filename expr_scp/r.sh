#!/bin/bash
best4=(38 37 38 38 38 37 38 37 38 38);
best5=(34 34 34 34 34 34 34 34 34 34);
best6=(21 20 21 20 21);
besta=(39 38 38 37 38);
bestb=(22 22 22 22 22);
bestc=(43 43 43 43 43);
bestd=(24 24 24 25 25);
beste=(5 5 5 5 5);
bestnre=(16 17 17 17 16);
bestnrf=(10 10 10 10 10);
bestnrg=(61 61 62 62 62);
bestnrh=(34 34 34 34 34);

MAX_IT=20000000;
for(( i=1; i<=10; i=i+1 )) do {
        echo $i-------->>4.txt
	for(( j=10; j<=20; j=j+1)) do {
		./msp ~/Desktop/scp/scp4$i.txt ${best4[$i-1]} $j $MAX_IT >>4.txt;
	}
	done
	echo >> 4.txt
}
done

for(( i=1; i<=10; i=i+1 )) do {
echo $i-------->>5.txt;
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scp5$i.txt ${best5[$i-1]} $j $MAX_IT>>5.txt;
}
done
echo >>5.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i---------->>6.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scp6$i.txt ${best6[$i-1]} $j $MAX_IT>>6.txt;
}
done
echo >>6.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i----------->>a.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpa$i.txt ${besta[$i-1]} $j $MAX_IT>>a.txt;
}
done
echo >>a.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i----------->>b.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpb$i.txt ${bestb[$i-1]} $j $MAX_IT>>b.txt;
}
done
echo >>b.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i------------>>c.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpc$i.txt ${bestc[$i-1]} $j $MAX_IT>>c.txt;
}
done
echo >>c.txt
}
done


for(( i=1; i<=5; i=i+1 )) do {
echo $i----------->>d.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpd$i.txt ${bestd[$i-1]} $j $MAX_IT>>d.txt;
}
done
echo >>d.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i----------->>nre.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpnre$i.txt ${bestnre[$i-1]} $j $MAX_IT>>nre.txt;
}
done
echo >>nre.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i------------>>nrf.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpnrf$i.txt ${bestnrf[$i-1]} $j $MAX_IT>>nrf.txt;
}
done
echo >>nrf.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i------------>>nrg.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpnrg$i.txt ${bestnrg[$i-1]} $j $MAX_IT>>nrg.txt;
}
done
echo >>nrg.txt
}
done

for(( i=1; i<=5; i=i+1 )) do {
echo $i------------->>nrh.txt
for(( j=10; j<=20; j=j+1)) do {
	./msp ~/Desktop/scp/scpnrh$i.txt ${bestnrh[$i-1]} $j $MAX_IT>>nrh.txt;
}
done
echo >>nrh.txt
}
done
