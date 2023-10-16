include config.mk

.PHONY: all install clean
.SUFFIXES: .md .html

all: edit.cgi save.cgi
clean:
	rm -f *.o *.cgi

install: all
	install -o www -g www -m ugo=rx -d ${HTDOCS}
	install -o www -g www -m ugo=rx edit.cgi ${HTDOCS}
	install -o www -g www -m ugo=rx save.cgi ${HTDOCS}
	install -o www -g www -m ugo=r style.css ${HTDOCS}
	install -o www -g www -m ugo=r edit.css ${HTDOCS}
	install -o www -g www -m ugo=r commonmark.js ${HTDOCS}
	mkdir -p ${ASSETS}
	install -o www -g www -m ugo=r edit.html ${ASSETS}
	install -o www -g www -m ugo=r page.html ${ASSETS}
	install -o www -g www -m ugo=r wiki.mk ${ASSETS}
	install -o www -g www -m ugo=r md2html.sh ${ASSETS}

util.o: util.c util.h
	$(CC) -c $(CFLAGS) -o $@ util.c

edit.o: edit.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ edit.c

save.o: save.c
	$(CC) -c $(CFLAGS) -I/usr/local/include -o $@ save.c

temp: temp.c
	$(CC) -o $@ temp.c $(CFLAGS) -I/usr/local/include -L/usr/local/lib -lkcgi -lz

edit.cgi: edit.o util.o
	$(CC) -static -o $@ edit.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz

save.cgi: save.o util.o
	$(CC) -static -o $@ save.o util.o -L/usr/local/lib -lkcgihtml -lkcgi -lz -lcmark
