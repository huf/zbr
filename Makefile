default: zbr

zbr: zbr.c frames.h
	gcc -g -Wall -o zbr zbr.c -lncurses

frames.h: mkframes.pl zebra.txt frames.h.tpl
	perl -pe 's!TITS_GO_HERE!qx(./mkframes.pl zebra.txt)!e' frames.h.tpl > frames.h

clean:
	rm -f frames.h zbr

.PHONY: clean

