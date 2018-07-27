
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
	char buf[BUFSIZ];
	int fd;
	char pathstr[PATH_MAX];

	memset(&r, 0, sizeof r);

	if (KCGI_OK != khttp_parse(&r, key, 2, &page, 1, 0))
		return(EXIT_FAILURE);

	/* start response */
	khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[KHTTP_200]);
	khttp_head(&r, kresps[KRESP_CONTENT_TYPE], "%s", kmimetypes[KMIME_TEXT_PLAIN]);
	khttp_body(&r);

	/* validate variable */
	if ((path = r.fieldmap[0]) != NULL) {
		snprintf(buf, sizeof buf, "file: %s\nsize: %zu\nval: %s\n",
		    path->key, path->valsz, path->val);
		khttp_puts(&r, buf);
		if (check_path(path->val))
			snprintf(pathstr, sizeof pathstr,
			    "../htdocs/%s.md", path->val);
	}

	if ((content = r.fieldmap[1]) != NULL) {
		snprintf(buf, sizeof buf, "file: %s\nsize: %zu\nval: %s\n",
		    content->key, content->valsz, content->val);
		khttp_puts(&r, buf);
	}

	if (path == NULL || content == NULL)
		return EXIT_FAILURE;

	if ((fd = open(path->val, O_WRONLY|O_TRUNC)) != -1)
		return EXIT_FAILURE;

	khttp_free(&r);

	return EXIT_SUCCESS;
}
