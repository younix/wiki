#include <sys/stat.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cgi.h>

#include "util.h"

extern struct templ templates[];

#define PATH_CHARSET	"abcdefghijklmnopqrstuvwxyz"	\
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"	\
			"1234567890" "_/"

static int
convert(const char *mark_file, int fd)
{
	int status;

	switch (fork()) {
	case 0:
		dup2(fd, STDOUT_FILENO); /* redirecting stdout to html file */
		execl("/bin/cmark", "/bin/cmark", mark_file, NULL);
		exit(EXIT_FAILURE);
	case -1:
		err(EXIT_FAILURE, "fork");
	}

	wait(&status);
	if (status != 0)
		errx(EXIT_FAILURE, "mark to html conversion failed");

	return 0;
}

int
main(void)
{
	const char *path;
	const char *content;
	char mark_file[PATH_MAX];
	char html_file[PATH_MAX];
	int fd;

	if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1)
		printf("dup2: %s\n", strerror(errno));

	if ((path = cgigetvalue("path")) == NULL)
		errx(EXIT_FAILURE, "cgigetvalue(\"path\"): NULL");

	if ((content = cgigetvalue("content")) == NULL)
		errx(EXIT_FAILURE, "cgigetvalue(\"content\"): NULL");

	/* check correctnes fo path */
	if (strspn(path, PATH_CHARSET) != strlen(path))
		errx(EXIT_FAILURE, "path contains illegal character");

	snprintf(mark_file, sizeof mark_file, "/htdocs/%s.md", path);
	snprintf(html_file, sizeof html_file, "/htdocs/%s.html", path);

	templates[TMPL_PATH].value = path;
	templates[TMPL_CONTENT].value = mark_file;

	/* safe markdown code to source file */
	if ((fd = open(mark_file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR)) == -1)
		err(EXIT_FAILURE, "open");

	if (write(fd, content, strlen(content)) == -1)
		err(EXIT_FAILURE, "write");

	if (close(fd) == -1)
		err(EXIT_FAILURE, "close");

	/* create html file */
	if ((fd = open(html_file, O_WRONLY|O_TRUNC|O_CREAT,
	    S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1)
		err(EXIT_FAILURE, "open");

	if (load_template("/wiki/page.html", fd, convert) == -1)
		errx(EXIT_FAILURE, "unable to load template");

	if (close(fd) == -1)
		return EXIT_FAILURE;

	printf("Refresh: 0; url=/%s.html\n", path);
	printf("Location: /%s.html\n\n", path);

	return EXIT_SUCCESS;
}
