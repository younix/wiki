CC=cc
CFLAGS=-std=c99 -pedantic -Wall -Wextra

.PHONY: all install clean
.SUFFIXES: .md .html

all: edit save
clean:
	rm -f edit save

install: edit
	cp edit /var/www/cgi-bin/
	cp save /var/www/cgi-bin/
	cp edit.html /var/www/htdocs/
	cp style.css /var/www/htdocs/
	cp marked.js /var/www/htdocs/
	cp head.html /var/www/wiki/
	cp foot.html /var/www/wiki/

edit.o: edit.c
	$(CC) -c $(CFLAGS) -I../cgiparse -o $@ edit.c

edit: edit.o
	gcc -static -o $@ edit.o -L../cgiparse -lcgi

save.o: save.c
	$(CC) -c $(CFLAGS) -I../cgiparse -o $@ save.c

save: save.o
	gcc -static -o $@ save.o -L../cgiparse -lcgi
