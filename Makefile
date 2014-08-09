mdfiles!=ls md/*.md

HEADER=template/header.html
FOOTER=template/footer.html

MD!=find md -name '*.md'
HTML=${MD:.md=.html}

all: ${HTML}
clean:
	rm -f ${HTML}

.SUFFIXES: .md .html
.md.html:
	@echo "$< -> $@"
	@cat ${HEADER} 		 > $@
	@create_nav $< 		 >> $@
	@markdown_py -o html5 $< >> $@
	@cat ${FOOTER}		 >> $@
