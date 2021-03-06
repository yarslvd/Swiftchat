#include "../inc/libmx.h"

char *mx_strjoin(char const *s1, char const *s2) {
	if(!s1 && !s2) {
		return NULL;
	}
	else if (!s1) {
		return mx_strdup(s2);
	}
	else if (!s2) {
		return mx_strdup(s1);
	}
	else {
		char *temp_str = mx_strnew(mx_strlen(s1) + mx_strlen(s2));
		mx_strcpy(temp_str, s1);
		temp_str = mx_strcat(temp_str, s2);

		return temp_str;
	}
}

