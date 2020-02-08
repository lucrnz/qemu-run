#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lucie_map/map.h"

#define BUFFER_MAX 1048576

typedef struct {
	char* System;
	char* CpuType;
	char* MemorySize;
	char* Acceleration;
	char* DisplayDriver;
	char* SoundDriver;
	char* Boot;
	char* ForwardPorts; //@TODO 🤣
	char* HardDiskVirtio;
	char* MonitorPort;
	char* SharedFolder;
	char* NetworkDriver;
	char* RngDevice;
	char* HostVideoAcceleration;
	char* CDRomISO;
	char* HardDisk;
} config_t;

inline void copy_char_chunk(const char* src_ptr, char* dest_ptr, size_t data_length) {
	for (size_t i = 0; i < data_length; i++) *(dest_ptr + i) = *(src_ptr + i);
}

size_t cstr_trim_right(const char* cstr, const size_t length) {
	size_t difference = 0;
	for (size_t i = length - 1; i < length; i--) {
		if (! isspace(*(cstr + i))) break;
		difference++;
	}
	return length - difference;
}

int read_config(const char* fpath, map* map_initialized) {
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
		map_insert(map_initialized,&key_buffer[0], &value_buffer[0]);
	}
	fclose(fptr);
}

int main(int argc, char** argv) {
	map* config = map_init();
	read_config("config", config);
	char* val;
	if (map_find(config, "System", &val) == 0) {
		printf("System=%s\n", val);
	} else {
		printf("Map_find returned error\n");
	}
	map_free(config);
	return 0;
}

