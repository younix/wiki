#include <sys/mman.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

extern struct templ templates[];

struct templ templates[] = {
	{"_?EDIT_TIME?_"   , "a"},
	{"_?EDIT_USER?_"   , "b"},
	{"_?PAGE_PATH?_"   , "c"},
	{"_?PAGE_CONTENT?_", "d"},
	{NULL, NULL},
};

#if 0
static int
write_file(char *file, FILE *fh, int fd)
{
	char *content;
	struct stat s;

	if ((fd = open(file, O_RDONLY)) == -1)
		return -1;

	if (fstat(fd, &s) == -1)
		return -1;

	content = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (content == MAP_FAILED)
		return -1;

	if (fh == NULL) {
		if (write(fd, content, s.st_size) == -1)
			return -1;
	} else {
		if (fwrite(content, s.st_size, 1, stdout) != s.st_size)
			return -1;
	}

	if (close(fd) == -1)
		return -1;

	return 0;
}
#endif

int
load_template(const char *file, int out,
    int (*content)(const char *file, int out))
{
	FILE *fh;
	char buf[BUFSIZ];

	if ((fh = fopen(file, "r")) == NULL)
		goto err;

	while (fgets(buf, sizeof buf, fh) != NULL) {
		char *tag = NULL;
		char *line = buf;

//	puts("blub!!!");

 again:
		/* if buf does not contains a tag, just write the whole buf */
		if ((tag = strstr(line, "_?")) == NULL) {
			if (write(out, line, strlen(line)) == -1)
				err(EXIT_FAILURE, "write");
			continue;
		}

		/* find the tag */
		for (unsigned int i = 0; i < TMPL_MAX; i++) {
			if (strncmp(tag, templates[i].key,
			    strlen(templates[i].key)) != 0)
				continue;

			*tag = '\0';
			if (write(out, line, strlen(line)) == -1)
				err(EXIT_FAILURE, "write");

			if (i == TMPL_CONTENT) {	/* write file content */
				if (content(templates[i].value, out) == -1)
					errx(EXIT_FAILURE, "content");
			} else {			/* write templ. value */
				if (write(out, templates[i].value,
				    strlen(templates[i].value)) == -1)
					errx(EXIT_FAILURE, "write");
			}

			line = tag + strlen(templates[i].key);
			goto again;
		}
		errx(EXIT_FAILURE, "unknown template key");
		return -1;	/* unknown template key */
	}

	if (fclose(fh) == EOF)
		goto err;

	return 0;

 err:
	printf("error: %s\n", strerror(errno));
	return -1;
}
