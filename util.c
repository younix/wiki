#include <sys/types.h>

#include <stdbool.h>
#include <ctype.h>

bool
check_path(const char *path)
{
	for (size_t i = 0; path[i] != '\0'; i++) {
		if (isalnum(path[i]) || path[i] == '_' || path[i] == '-')
			continue;

		return false;
	}

	return true;
}
