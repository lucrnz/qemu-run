#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lucie_map/map.h"

#define BUFFER_MAX 1048576

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

int config_load(const char* fpath, map* map_initialized) {
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
			if (setting_value && trimming_value_left && !isspace(c)) {
				trimming_value_left = 0;
				*(value_buffer +value_len) = c;
				value_len++;
			} else if (setting_value && !trimming_value_left) {
				value_buffer[value_len] = c;
				value_len++;
			} else if (!setting_value && trimming_key_left && !isspace(c)) {
				trimming_key_left = 0;
				*(key_buffer + key_len) = c;
				key_len++;
			} else if (!setting_value && !trimming_key_left)  {
				*(key_buffer + key_len) = c;
				key_len++;
			}
		}
		*(key_buffer + key_len) = 0;
		*(value_buffer + value_len) = 0;
		
		// Time to trim right! 😂👌
		key_len = cstr_trim_right(&key_buffer[0], key_len);
		value_len = cstr_trim_right(&value_buffer[0], value_len);

		*(key_buffer + key_len) = 0;
		*(value_buffer + value_len) = 0;
		map_insert(map_initialized, &key_buffer[0], &value_buffer[0]);
		fprintf(stderr, "%s=%s\n", &key_buffer[0], &value_buffer[0]);
	}
	fclose(fptr);
}

void config_set_default_values(map* config) {
	map_insert(config, "sys", "x64");
	map_insert(config, "efi", "no");
	map_insert(config, "cpu", "host");
	map_insert(config, "mem", "2G");
	map_insert(config, "acc", "yes");
	map_insert(config, "vga", "virtio");
	map_insert(config, "snd", "hda");
	map_insert(config, "boot", "c");
	map_insert(config, "fwd_ports", "2222:22");
	map_insert(config, "hdd_virtio", "yes");
	map_insert(config, "shared", "shared");
	map_insert(config, "net", "virtio-net-pci");
	map_insert(config, "rng_dev", "yes");
	map_insert(config, "host_video_acc", "no");
	map_insert(config, "localtime", "no");
	map_insert(config, "headless", "no");
	map_insert(config, "vnc_pwd", "");
	map_insert(config, "monitor_port", "5510");
	map_insert(config, "floppy", "floppy");
	map_insert(config, "cdrom", "cdrom");
	map_insert(config, "disk", "disk");
}

void program_build_cmd_line(map* config) {
	
}

int main(int argc, char** argv) {
	map* config = map_init();

	config_set_default_values(config);
	config_load("config", config);

	const char* val;
	if (!map_find(config, "sys", &val)) {
		printf("sys=%s\n", val);
	} else {
		printf("map_find returned error\n");
	}

	map_free(config);
	return 0;
}
