#include <limit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cgi.h>

int
main(int argc, char *argv[])
{
	const char *path;
	const char *content;
	char file[PATH_MAX];

	if ((path = cgigetvalue("path")) == NULL)
		return EXIT_FAILURE;

	if ((content = cgigetvalue("content")) == NULL)
		return EXIT_FAILURE;

	if (strstr(path, "..") != NULL)
		return EXIT_FAILURE;

	snprintf(file, sizeof file, "/htdocs/%d.md", path);

	if ((fd = open(file, O_WRONLY|O_CREATE)) == -1)
		return EXIT_FAILURE;

	if (write(fd, content, strlen(content)) == -1)
		return EXIT_FAILURE;

	if (close(fd) == -1)
		return EXIT_FAILURE;

	fprintf(stdout, "Location: /%s.html\r\n\r\n", path);

	return EXIT_SUCCESS;
}
