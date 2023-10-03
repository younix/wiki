#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <kcgi.h>
#include <kcgihtml.h>

#include "util.h"

int
main(void)
{
	struct kreq r;
	struct kpair *path;
	struct kpair *content;
	struct kvalid key[2] = {
		{ kvalid_string, "path" },
		{ kvalid_string, "content" }
	};
	const char *page = "index";
	int fd;
	char pathstr[PATH_MAX];

	memset(&r, 0, sizeof r);

	if (KCGI_OK != khttp_parse(&r, key, 2, &page, 1, 0))
		return(EXIT_FAILURE);

	/* validate variable */
	if ((path = r.fieldmap[0]) != NULL && check_path(path->val))
		snprintf(pathstr, sizeof pathstr, "../htdocs/%s.md", path->val);

	if ((content = r.fieldmap[1]) == NULL)
		goto out;

	if (pathstr[0] == '\0' || content == NULL)
		goto out;

	if ((fd = open(pathstr, O_WRONLY|O_CREAT|O_TRUNC,
	    S_IRUSR|S_IWUSR|S_IRWXG|S_IRGRP)) == -1)
		goto err;

	if (write(fd, content->val, content->valsz) == -1)
		goto err;

	if (close(fd) == -1)
		goto err;

	/* rebuild static html files */
	if (system("make -C ../assets -f ../assets/wiki.mk"))
		goto err;

	/* forward */
	khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[KHTTP_303]);
	khttp_head(&r, kresps[KRESP_LOCATION], "/%s.html", path->val);
	khttp_body(&r);
	khttp_free(&r);
	return EXIT_SUCCESS;

 out:
	/* display error message */
	khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[KHTTP_200]);
	khttp_head(&r, kresps[KRESP_CONTENT_TYPE], "%s", kmimetypes[KMIME_TEXT_PLAIN]);
	khttp_body(&r);
	khttp_puts(&r, "error");
	khttp_free(&r);
	return EXIT_SUCCESS;

 err:
	kutil_err(&r, "save", "errno: %s\n",  strerror(errno));
	return EXIT_SUCCESS;
}
