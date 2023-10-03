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

enum keyn { KEY_NAME, KEY_SRC, KEY_MAX };
const char *const keys[KEY_MAX] = { "name", "source" };
char *pathstr = NULL;
char *name = NULL;

static int
template(size_t index, void *arg)
{
	char buf[BUFSIZ];
	int fd;
	ssize_t sz;
	struct kreq *r = arg;

	switch (index) {
	case KEY_NAME:
		khttp_puts(r, name);
		break;
	case KEY_SRC:
		if (pathstr == NULL || *pathstr == '\0')
			break;
		if ((fd = open(pathstr, O_RDONLY)) == -1)
			break;

		while ((sz = read(fd, buf, sizeof buf)) > 0)
			khttp_write(r, buf, sz);

		close(fd);
		break;
	default:
		return 0;
	}

	return 1;
}

int
main(void)
{
	struct kreq r;
	struct kpair *path;
	struct kvalid key[1] = {{ kvalid_string, "path" }};
	struct ktemplate t = { keys, KEY_MAX, &r, template };
	const char *page = "index";

	if (KCGI_OK != khttp_parse(&r, key, 2, &page, 1, 0))
		return EXIT_FAILURE;

	/* validate variable */
	if ((path = r.fieldmap[0]) != NULL && check_path(path->val)) {
		asprintf(&pathstr, "/htdocs/%s.md", path->val);
		name = path->val;
	}

	/* start response */
	khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[KHTTP_200]);
	khttp_head(&r, kresps[KRESP_CONTENT_TYPE], "%s", kmimetypes[KMIME_TEXT_HTML]);
	khttp_body(&r);

	khttp_template(&r, &t, "../assets/edit.html");

	khttp_free(&r);

	return EXIT_SUCCESS;
}
