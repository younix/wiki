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

#if 0
static int
put_file(FILE *out, char *file)
{
	FILE *in;
	size_t n;
	char buf[BUFSIZ];

	if (out == NULL)
		return -1;

	if ((in = fopen(file, "r")) == NULL) {
		if (errno == ENOENT)
			return 0;
		return -1;
	}

	while ((n = fread(buf, sizeof *buf, BUFSIZ, in)) > 0)
		if (fwrite(buf, sizeof *buf, n, out) < n)
			return -1;

	if (fclose(in) == EOF)
		return -1;

	return 0;
}
#endif

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
//	FILE *fh;
	const char *path;
//	char *tag;
	char file[PATH_MAX];
//	char buf[BUFSIZ];

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

#if 0
	if ((fh = fopen("/htdocs/edit.html", "r")) == NULL)
		goto err;

	htmlheader();

	while (fgets(buf, sizeof buf, fh) != NULL) {
		if ((tag = strstr(buf, PAGE_CONTENT)) != NULL) {
			*tag = '\0';
			fputs(buf, stdout);

			if (put_file(stdout, file) == -1)
				goto err;

			tag += sizeof(PAGE_CONTENT) - 1;
			fputs(tag, stdout);

			continue;
		}

		if ((tag = strstr(buf, PAGE_PATH)) != NULL) {
			*tag = '\0';
			fputs(buf, stdout);

			fputs(path, stdout);

			tag += sizeof(PAGE_PATH) - 1;
			fputs(tag, stdout);

			continue;
		}

		fputs(buf, stdout);
	}

	if (fclose(fh) == EOF)
		goto err;
#endif

	return EXIT_SUCCESS;
}
