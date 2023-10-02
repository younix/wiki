include config.mk

.PHONY: all install clean
.SUFFIXES: .md .html

all: edit.cgi save.cgi env.cgi
clean:
	rm -f edit save *.o

install: all
	cp edit.cgi ${HTDOCS}
	cp save.cgi ${HTDOCS}
	cp env.cgi ${HTDOCS}
	cp style.css ${HTDOCS}
	cp edit.css ${HTDOCS}
	cp commonmark.js ${HTDOCS}
	mkdir -p ${ASSETS}
	cp edit.html ${ASSETS}
	cp wiki.mk ${ASSETS}
	cp md2html.sh ${ASSETS}

util.o: util.c util.h
	$(CC) -c $(CFLAGS) -o $@ util.c

edit.o: edit.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ edit.c

save.o: save.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ save.c

env.o: env.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ env.c -g

edit.cgi: edit.o util.o
	$(CC) -static -o $@ edit.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz

save.cgi: save.o util.o
	$(CC) -static -o $@ save.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz

env.cgi: env.o
	$(CC) -static -o $@ env.o -L/usr/local/lib -lkcgihtml -lkcgi -lz -g
