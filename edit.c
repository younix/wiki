#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cgi.h>

#define PAGE_CONTENT "__PAGE_CONTENT__"
#define PAGE_PATH    "__PAGE_PATH__"

int
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

int
main(void)
{
	FILE *fh;
	const char *path;
	char *tag;
	char file[PATH_MAX];
	char buf[BUFSIZ];

	if ((path = cgigetvalue("path")) == NULL)
		goto err;

	if (strstr(path, "..") != NULL)
		goto err;

	snprintf(file, sizeof file, "/htdocs/%s.md", path);

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

	return EXIT_SUCCESS;

 err:
	printf("error: %s\n", strerror(errno));
	return EXIT_FAILURE;
}
