include config.mk

.PHONY: all install clean
.SUFFIXES: .md .html

all: edit save
clean:
	rm -f edit save *.o

install: edit save
	cp edit ${CGIBIN}
	cp save ${CGIBIN}
	cp style.css ${HTDOCS}
	cp edit.css ${HTDOCS}
	cp commonmark.js ${HTDOCS}
	cp edit.html ${HTDOCS}
	cp page.html ${HTDOCS}
	cp data/Makefile ${HTDOCS}/Makefile

util.o: util.c util.h
	$(CC) -c $(CFLAGS) -o $@ util.c

edit.o: edit.c
	$(CC) -c $(CFLAGS) -I../cgiparse -o $@ edit.c

save.o: save.c
	$(CC) -c $(CFLAGS) -I../cgiparse -o $@ save.c

edit: edit.o util.o
	gcc -static -o $@ edit.o util.o -L../cgiparse -lcgi

save: save.o util.o
	gcc -static -o $@ save.o util.o -L../cgiparse -lcgi
