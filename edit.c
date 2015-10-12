#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cgi.h>

#include "util.h"

extern struct templ templates[];

static int
new_put_file(const char *file, int out)
{
	FILE *in;
	size_t n;
	char buf[BUFSIZ];

	if ((in = fopen(file, "r")) == NULL) {
		if (errno == ENOENT)
			return 0;
		return -1;
	}

	while ((n = fread(buf, sizeof *buf, BUFSIZ, in)) > 0)
		if (write(out, buf, n) == -1)
			return -1;

	if (fclose(in) == EOF)
		return -1;

	return 0;
}

int
main(void)
{
	const char *path;
	char file[PATH_MAX];

	if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1)
		printf("dup2: %s\n", strerror(errno));

	if ((path = cgigetvalue("path")) == NULL)
		errx(EXIT_FAILURE, "path not found");

	if (strstr(path, "..") != NULL)
		errx(EXIT_FAILURE, "ilegal path");

	snprintf(file, sizeof file, "/htdocs/%s.md", path);

	templates[TMPL_PATH].value = path;
	templates[TMPL_CONTENT].value = file;

	htmlheader();
	fflush(stdout);

	if (load_template("/wiki/edit.html", STDOUT_FILENO, new_put_file)
	    == -1)
		errx(EXIT_FAILURE, "unable to load template");

	return EXIT_SUCCESS;
}
