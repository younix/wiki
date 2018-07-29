MD != find ../htdocs -name '*.md'
HTML = ${MD:S/md$/html/}

.PHONY: all clean
all: ${HTML}
clean:
	rm -rf ${HTML}

.SUFFIXES: .md .html
.md.html:
	cmark $< > $@
