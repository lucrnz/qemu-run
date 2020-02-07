#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define BUFFER_MAX 1048576

size_t cstr_trim_right(const char* cstr, const size_t length) {
	size_t difference = 0;
	for (size_t i = length - 1; i < length; i--) {
		if (! isspace(*(cstr + i))) break;
		difference++;
	}
	return length - difference;
}

int read_config(const char* fpath) {
	FILE* fptr = fopen(fpath, "r");
	if (fptr == NULL) return 1; //@TODO: error management 🤣

	char line[BUFFER_MAX];
	while(fgets(line, BUFFER_MAX, fptr) != NULL) {
		size_t line_len = strlen(line);
		line[line_len - 1] = '\0';
		if (line_len < 3) continue;

		char c = '\0';
		char value_buffer[BUFFER_MAX];
		char key_buffer[BUFFER_MAX];

		size_t value_len = 0, key_len = 0;
		int setting_value = 0, trimming_value_left = 0, trimming_key_left = 1;

		for (size_t i = 0; (c = line[i]) != '\0'; i++) {
			if (c == '=') {
				setting_value = 1;
				trimming_value_left = 1;
				continue;
			}
			if (setting_value) {
				if (trimming_value_left) {
					if (! isspace(c)) {
						trimming_value_left = 0;
						*(value_buffer +value_len) = c;
						value_len++;
					}
				} else {
					value_buffer[value_len] = c;
					value_len++;
				}
			} else {
				if (trimming_key_left) {
					if (! isspace(c)) {
						trimming_key_left = 0;
						*(key_buffer + key_len) = c;
						key_len++;
					}
				} else {
					*(key_buffer + key_len) = c;
					key_len++;
				}
			}
		}
		*(key_buffer + key_len) = 0;
		*(value_buffer + value_len) = 0;
		
		// Time to trim right! 😂👌
		key_len = cstr_trim_right(&key_buffer[0], key_len);
		value_len = cstr_trim_right(&value_buffer[0], value_len);

		*(key_buffer + key_len) = 0;
		*(value_buffer + value_len) = 0;
		printf("k=[%s], [%s]\n", &key_buffer[0], &value_buffer[0]);
	}

	fclose(fptr);
}

int main(int argc, char** argv) {
	read_config("config");
	return 0;
}

