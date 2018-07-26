include config.mk

.PHONY: all install clean
.SUFFIXES: .md .html

all: kedit
clean:
	rm -f edit save *.o

install: all
	cp kedit ${CGIBIN}
	#cp save ${CGIBIN}
	cp style.css ${HTDOCS}
	cp edit.css ${HTDOCS}
	cp commonmark.js ${HTDOCS}
	mkdir -p ${ASSETS}
	cp edit.html ${ASSETS}
	cp page.html ${ASSETS}
	cp data/Makefile ${ASSETS}

util.o: util.c util.h
	$(CC) -c $(CFLAGS) -o $@ util.c

edit.o: edit.c
	$(CC) -c $(CFLAGS) -I../cgiparse -o $@ edit.c

kedit.o: kedit.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ kedit.c

ksave.o: ksave.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ ksave.c

save.o: save.c
	$(CC) -c $(CFLAGS) -I../cgiparse -o $@ save.c

edit: edit.o util.o
	gcc -static -o $@ edit.o util.o -L../cgiparse -lcgi

kedit: kedit.o
	gcc -static -o $@ kedit.o -L/usr/local/lib -lkcgihtml -lkcgi -lz

ksave: ksave.o
	gcc -static -o $@ ksave.o -L/usr/local/lib -lkcgihtml -lkcgi -lz

save: save.o util.o
	gcc -static -o $@ save.o util.o -L../cgiparse -lcgi
