#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cmark.h>
#include <kcgi.h>
#include <kcgihtml.h>

#include "util.h"

enum keyn { KEY_NAME, KEY_NAV, KEY_HTML, KEY_MAX };
const char *const keys[KEY_MAX] = { "name", "nav", "html" };

struct arg {
	int	 fd;
	char	*name;
	char	*nav;
	char	*html;
};

void
werror(struct kreq *r,const char *str)
{
	khttp_head(r, kresps[KRESP_CONTENT_TYPE], "%s",
	    kmimetypes[KMIME_TEXT_PLAIN]);
	khttp_body(r);
	khttp_printf(r, "%s: %s", str, strerror(errno));
	khttp_free(r);

	exit(EXIT_FAILURE);
}

int
nav(struct arg *arg)
{
	DIR		*dir;
	struct dirent	*ent;

	if ((dir = opendir("/wiki")) == NULL)
		return 0;

	while ((ent = readdir(dir)) != NULL) {

		if (strcmp(ent->d_name, ".") == 0 ||
		    strcmp(ent->d_name, "..") == 0 || ent->d_namlen < 5)
			continue;

		if (strcmp(&ent->d_name[ent->d_namlen - 5], ".html") != 0)
			continue;

		/* TODO: escaping HTML characters */
		dprintf(arg->fd, "<a href=\"%s\">%.*s</a>",
		    ent->d_name, ent->d_namlen - 5, ent->d_name);
	}

	if (closedir(dir) == -1)
		return 0;

	return 1;
}

int
template(size_t index, void *a)
{
	struct arg *arg = a;

	switch (index) {
	case KEY_NAME:
		if (write(arg->fd, arg->name, strlen(arg->name)) == -1)
			return 0;
		break;
	case KEY_NAV:
			return nav(arg);
		break;
	case KEY_HTML:
		if (write(arg->fd, arg->html, strlen(arg->html)) == -1)
			return 0;
		break;
	default:
		return 0;
	}

	return 1;
}

enum kcgi_err
writer(const char *buf, size_t size, void *a)
{
	struct arg *arg = a;

	if (write(arg->fd, buf, size) == -1)
		return KCGI_WRITER;

	return KCGI_OK;
}

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
	char pathmd[PATH_MAX];
	char pathht[PATH_MAX];
	struct arg arg;

	memset(&r, 0, sizeof r);

	if (KCGI_OK != khttp_parse(&r, key, 2, &page, 1, 0))
		return(EXIT_FAILURE);

	if ((content = r.fieldmap[1]) == NULL)
		goto out;

	/* validate variable */
	if ((path = r.fieldmap[0]) != NULL && check_path(path->val)) {
		arg.name = path->val;
		arg.nav = "";

		snprintf(pathmd, sizeof pathmd, "../wiki/%s.md", arg.name);
		snprintf(pathht, sizeof pathht, "../wiki/%s.html", arg.name);
	}

	if (pathmd[0] == '\0' || pathht[0] == '\0')
		goto out;

	/* save html file */
	if ((fd = open(pathmd, O_WRONLY|O_CREAT|O_TRUNC,
	    S_IRUSR|S_IWUSR|S_IRWXG|S_IRGRP)) == -1)
		werror(&r, "open");

	if (write(fd, content->val, content->valsz) == -1)
		werror(&r, "write");

	if (close(fd) == -1)
		werror(&r, "close");

	/* save html file */
	if ((arg.fd = open(pathht, O_WRONLY|O_CREAT|O_TRUNC,
	    S_IRUSR|S_IWUSR|S_IRWXG|S_IRGRP)) == -1)
		werror(&r, "open");

	arg.html = cmark_markdown_to_html(content->val, content->valsz, 0);

	const struct ktemplate temp = {
		.key = keys,
		.keysz = KEY_MAX,
		.arg = &arg,
		.cb = template
	};

	const struct ktemplatex tempx = {
		.writer = writer,
		.fbk = NULL
	};

	khttp_templatex(&temp, "/assets/page.html", &tempx, &arg);

	if (close(fd) == -1)
		werror(&r, "close");

	/* forward */
	khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[KHTTP_303]);
	khttp_head(&r, kresps[KRESP_LOCATION], "/%s.html", path->val);
	khttp_body(&r);
	khttp_free(&r);
	return EXIT_SUCCESS;

 out:
	/* display error message */
	khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[KHTTP_200]);
	khttp_head(&r, kresps[KRESP_CONTENT_TYPE], "%s",
	    kmimetypes[KMIME_TEXT_PLAIN]);
	khttp_body(&r);
	khttp_puts(&r, "error");
	khttp_free(&r);
	return EXIT_SUCCESS;
}
