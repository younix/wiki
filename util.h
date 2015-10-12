#ifndef UTIL_H
#define UTIL_H

struct templ {
	const char *key;
	const char *value;
};

enum templ_pos {TMPL_TIME, TMPL_USER, TMPL_PATH, TMPL_CONTENT, TMPL_MAX};

int load_template(const char *file, int out, int(*content)(const char *, int));

#endif
