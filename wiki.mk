MD != find md -name '../htdocs/*.md'
HTML = ${MD:S/md$/html/}

.PHONY: all clean
all: ${HTML}
clean:
	rm -rf html ${HTML}

.SUFFIXES: .md .html
.md.html:
	./nav.sh $< > $@
	smu -n $< > $@
