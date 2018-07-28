include config.mk

.PHONY: all install clean
.SUFFIXES: .md .html

all: edit save
clean:
	rm -f edit save *.o

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

edit.o: edit.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ edit.c

save.o: save.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ save.c

edit: edit.o util.o
	$(CC) -static -o $@ edit.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz

save: save.o util.o
	$(CC) -static -o $@ save.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz
