mdfiles!=ls md/*.md

HEADER=template/header.html
FOOTER=template/footer.html

MD!=ls md/*.md
HTML=${MD:.md=.html}

all: ${HTML}
clean:
	rm -f ${HTML}

.SUFFIXES: .md .html
.md.html:
	@echo "$< -> $@"
	@cat ${HEADER} > $@
	@smu $< >> $@
	@cat ${FOOTER} >> $@
