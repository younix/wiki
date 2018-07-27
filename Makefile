include config.mk

.PHONY: all install clean
.SUFFIXES: .md .html

all: kedit ksave
clean:
	rm -f kedit ksave edit save *.o

install: all
	cp kedit ${CGIBIN}
	cp ksave ${CGIBIN}
	cp style.css ${HTDOCS}
	cp edit.css ${HTDOCS}
	cp commonmark.js ${HTDOCS}
	mkdir -p ${ASSETS}
	cp edit.html ${ASSETS}
	cp page.html ${ASSETS}
	cp data/Makefile ${ASSETS}

util.o: util.c util.h
	$(CC) -c $(CFLAGS) -o $@ util.c

kedit.o: kedit.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ kedit.c

ksave.o: ksave.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ ksave.c

kedit: kedit.o util.o
	$(CC) -static -o $@ kedit.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz

ksave: ksave.o util.o
	$(CC) -static -o $@ ksave.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz
