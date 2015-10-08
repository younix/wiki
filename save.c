#include <sys/wait.h>

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cgi.h>

static int
append_file(char *in_file, int out)
{
	int in;
	ssize_t n;
	char buf[BUFSIZ];

	if ((in = open(in_file, O_RDONLY)) == -1)
		return -1;

	while ((n = read(in, buf, sizeof buf)) <= 0)
		if (write(out, buf, n) < n)
			goto err;

	if (n == -1)
		goto err;

	if (close(in) == -1)
		return -1;

	return 0;
 err:
	close(in);
	return -1;
}

static int
convert(char *mark_file, char *html_file)
{
	int fd;
	int status;

	if ((fd = open(html_file, O_WRONLY)) == -1)
		return -1;

	if (append_file("/wiki/head.html", fd) == -1)
		return -1;

	switch (fork()) {
	case 0:
		dup2(fd, STDOUT_FILENO);
		execl("/bin/cmark", mark_file, NULL);
	case -1: return -1;
	default:
		wait(&status);
		if (status != 0)
			return -1;
	}

	lseek(fd, 0, SEEK_END);
	if (append_file("/wiki/foot.html", fd) == -1)
		return -1;

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

	htmlheader();

	printf("cgigetvalue\n");

	if ((path = cgigetvalue("path")) == NULL)
		return EXIT_FAILURE;

	printf("cgigetvalue\n");
	if ((content = cgigetvalue("content")) == NULL)
		return EXIT_FAILURE;

	/* path movements are forbidden */
	if (strstr(path, "..") != NULL)
		return EXIT_FAILURE;

	snprintf(mark_file, sizeof mark_file, "/htdocs/%s.md", path);
	snprintf(html_file, sizeof html_file, "/htdocs/%s.html", path);

	printf("open\n");
	/* safe markdown code to source file */
	if ((fd = open(mark_file, O_WRONLY|O_CREAT)) == -1)
		return EXIT_FAILURE;

	if (write(fd, content, strlen(content)) == -1)
		return EXIT_FAILURE;

	if (close(fd) == -1)
		return EXIT_FAILURE;

	printf("convert\n");
	convert(mark_file, html_file);

	fprintf(stdout, "Location: /%s.html\r\n\r\n", path);

	return EXIT_SUCCESS;
}
