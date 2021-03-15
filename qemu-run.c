/*Copyright (C) 2021 Lucie Cupcakes <lucie_linux [at] protonmail.com>
This file is part of qemu-run <https://github.com/lucie-cupcakes/qemu-run>.
qemu-run is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.
qemu-run is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.
You should have received a copy of the GNU General Public License
along with qemu-run; see the file LICENSE.  If not see <http://www.gnu.org/licenses/>.*/

#include <glib.h>
#include <gmodule.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/sysinfo.h> // @TODO: Is this Linux only ?
#include <sys/stat.h>

#define buffer_slice 1024
#define buffer_max buffer_slice*1024

#define cfg_add_kv(cfg, k, v) g_hash_table_insert(cfg, g_strdup(k), g_strdup(v));

#define log_msg(m) fprintf(stderr, "%s\n", m);
#define print_gpl_banner() \
	puts("qemu-run. Forever beta software. Use on production on your own risk!\nThis software is Free software - released under the GPLv3 License.\nRead the LICENSE file. Or go visit https://www.gnu.org/licenses/gpl-3.0.html\n");

gboolean file_exists(const char *fpath) {
	struct stat buffer;
	return (stat (fpath, &buffer) == 0) ? TRUE : FALSE;
}

gboolean g_dir_exists(const gchar *path) {
	gboolean rc = FALSE;
	GDir* gd = g_dir_open(path, 0, NULL);
	if (gd != NULL) {
		rc = TRUE;
		g_dir_close(gd);
	}
	return rc;
}

gboolean g_hash_table_match_key_alow(GHashTable *t, gpointer k, const char *s) {
	/* Returns TRUE if the value of the provided key for the HashTable t
	 * at lowercase is equal to s
	 * Otherwise, returns FALSE */
	gboolean res = FALSE;
	char *t_v_low;
	gpointer t_v = g_hash_table_lookup(t, k);
	
	if (t_v != NULL) {
		t_v_low = g_ascii_strdown(t_v, -1);		
		res = strcmp((const char*)t_v_low, s) == 0 ? TRUE : FALSE;
		g_free(t_v_low);
	}
	
	return res;
}

size_t cstr_trim_right(const char *cstr, const size_t length) {
	size_t difference = 0;
	for (size_t i = length - 1; i < length; i--) {
		if (! isspace(*(cstr + i))) break;
		difference++;
	}
	return length - difference;
}

int process_kv_pair(char *kv_cstr, GHashTable *cfg) {
	size_t kv_cstr_len = strlen(kv_cstr);
	kv_cstr[kv_cstr_len - 1] = '\0';
	if (kv_cstr_len < 3) return 1;

	char c = '\0';
	char value_buffer[buffer_slice], key_buffer[buffer_slice];

	size_t value_len = 0, key_len = 0;
	int setting_key = 1, trimming_left = 1;

	for (size_t i = 0; (c = kv_cstr[i]) != '\0'; i++) {
		if (c == '=') {
			setting_key = 0;
			trimming_left = 1;
			continue;
		}
		if (setting_key && trimming_left && !isspace(c)) {
			trimming_left = 0;
			key_buffer[key_len] = c;
			key_len++;
		} else if (setting_key && !trimming_left)  {
			key_buffer[key_len] = c;
			key_len++;
		} else if (!setting_key && trimming_left && !isspace(c)) {
			trimming_left = 0;
			value_buffer[value_len] = c;
			value_len++;
		} else if (!setting_key && !trimming_left) {
			value_buffer[value_len] = c;
			value_len++;
		}
	}
	key_buffer[key_len] = 0;
	value_buffer[value_len] = 0;
	key_len = cstr_trim_right(key_buffer, key_len);
	value_len = cstr_trim_right(value_buffer, value_len);
	key_buffer[key_len] = 0;
	value_buffer[value_len] = 0;
	g_hash_table_replace(cfg, g_strdup(key_buffer), g_strdup(value_buffer));
	return 0;
}

gboolean config_load(const char *fpath, GHashTable *cfg) {
	FILE *fptr = fopen(fpath, "r");
	if (fptr == NULL) return FALSE; //@TODO: error management
	char line[buffer_slice];
	while(fgets(line, buffer_slice, fptr) != NULL) {
		process_kv_pair(line, cfg);
	}
	fclose(fptr);
	return TRUE;
}

void program_get_cfg_values(GHashTable *cfg, char *vm_dir) {
	char path_buff[PATH_MAX] = {0};
	char nproc_str[4];
	snprintf(nproc_str, 4, "%d", get_nprocs());
	cfg_add_kv(cfg, "sys", "x64");
	cfg_add_kv(cfg, "efi", "no");
	cfg_add_kv(cfg, "cpu", "host");
	cfg_add_kv(cfg, "cores", nproc_str);
	cfg_add_kv(cfg, "mem", "2G");
	cfg_add_kv(cfg, "acc", "yes");
	cfg_add_kv(cfg, "vga", "virtio");
	cfg_add_kv(cfg, "snd", "hda");
	cfg_add_kv(cfg, "boot", "c");
	cfg_add_kv(cfg, "fwd_ports", "2222:22");
	cfg_add_kv(cfg, "hdd_virtio", "yes");
	cfg_add_kv(cfg, "net", "virtio-net-pci");
	cfg_add_kv(cfg, "rng_dev", "yes");
	cfg_add_kv(cfg, "host_video_acc", "no");
	cfg_add_kv(cfg, "localtime", "no");
	cfg_add_kv(cfg, "headless", "no");
	cfg_add_kv(cfg, "vnc_pwd", "");
	cfg_add_kv(cfg, "monitor_port", "5510");
	
	snprintf(path_buff, PATH_MAX, "%s/%s", vm_dir, "shared");
	if (g_dir_exists(path_buff)) {
		cfg_add_kv(cfg, "shared", path_buff);
	} else {
		cfg_add_kv(cfg, "shared", "");
	}
	
	snprintf(path_buff, PATH_MAX, "%s/%s", vm_dir, "floppy");
	if (file_exists(path_buff)) {
		cfg_add_kv(cfg, "floppy", path_buff);
	}
	snprintf(path_buff, PATH_MAX, "%s/%s", vm_dir, "cdrom");
	if (file_exists(path_buff)) {
		cfg_add_kv(cfg, "cdrom", path_buff);
	}
	snprintf(path_buff, PATH_MAX, "%s/%s", vm_dir, "disk");
	if (file_exists(path_buff)) {
		cfg_add_kv(cfg, "disk", path_buff);
	}
}

char *add_to_strbuff(char *dst,const char *src) {
	char c;
	for (;;) {
		c = *src; src++;
		if (c == '\0') break;
		*dst = c; dst++;
	}
	*dst = ' '; dst++;
	return dst;
}

gboolean program_build_cmd_line(GHashTable *cfg, char *vm_dir, char *vm_name, char *out_cmd) {
	gboolean rc = TRUE;
	int drive_index = 0, telnet_port = 55555; // @TODO: Get usable TCP port
	gpointer cfg_v;
	char cmd_slice[buffer_slice] = {0};

	gboolean vm_has_name = (strcmp(vm_name, "") != 0 ? TRUE : FALSE);
	gboolean vm_has_acc_enabled = g_hash_table_match_key_alow(cfg, "acc", "yes");
	gboolean vm_has_vncpwd = (strcmp((const char*)g_hash_table_lookup(cfg, "vnc_pwd"), "") != 0 ? TRUE : FALSE);
	gboolean vm_has_audio = (g_hash_table_match_key_alow(cfg, "snd", "no") ? FALSE : TRUE);
	gboolean vm_has_videoacc = g_hash_table_match_key_alow(cfg, "host_video_acc", "yes");
	gboolean vm_has_rngdev = g_hash_table_match_key_alow(cfg, "rng_dev", "yes");
	gboolean vm_is_headless = g_hash_table_match_key_alow(cfg, "headless", "yes");
	gboolean vm_clock_is_localtime = g_hash_table_match_key_alow(cfg, "localtime", "yes");
	gboolean vm_has_sharedf = (strcmp(g_hash_table_lookup(cfg, "shared"), "") != 0 ? TRUE : FALSE);
	gboolean vm_has_hddvirtio = g_hash_table_match_key_alow(cfg, "hdd_virtio", "yes");
	
	if (vm_has_sharedf) {
		vm_has_sharedf = g_dir_exists(g_hash_table_lookup(cfg, "shared"));
	}
	
	cfg_v = g_hash_table_lookup(cfg, "sys");
	if (strcmp((const char*)cfg_v, "x32") == 0) {
		out_cmd = add_to_strbuff(out_cmd, "qemu-system-i386");
	} else if (strcmp((const char*)cfg_v, "x64") == 0) {
		out_cmd = add_to_strbuff(out_cmd, "qemu-system-x86_64");
	} else {
		log_msg("Invalid value for sys"); //@TODO: Logger
		rc = FALSE;
	}
	
	snprintf(cmd_slice, buffer_slice, "%s-name %s -cpu %s -smp %s -m %s -boot order=%s -usb -device usb-tablet -vga %s %s%s",
		vm_has_acc_enabled ? "--enable-kvm " : "",
		vm_has_name ? vm_name : "QEMU",
		(char*)g_hash_table_lookup(cfg, "cpu"),
		(char*)g_hash_table_lookup(cfg, "cores"),
		(char*)g_hash_table_lookup(cfg, "mem"),
		(char*)g_hash_table_lookup(cfg, "boot"),
		(char*)g_hash_table_lookup(cfg, "vga"),
		vm_has_audio ? "-soundhw " : "",
		vm_has_audio ? (char*)g_hash_table_lookup(cfg, "snd") : ""
	);
	out_cmd = add_to_strbuff(out_cmd, cmd_slice);

	if (vm_is_headless) {
		snprintf(cmd_slice, buffer_slice, "-display none -monitor telnet:127.0.0.1:%d,server,nowait -vnc %s",
			telnet_port,
			vm_has_vncpwd ? "127.0.0.1:0,password" : "127.0.0.1:0");
		out_cmd = add_to_strbuff(out_cmd, cmd_slice);
	} else {
		out_cmd = add_to_strbuff(out_cmd, vm_has_videoacc ? "-display gtk,gl=on" : "-display gtk,gl=off");
	}
	
	// @TODO: Forward ports logic
	snprintf(cmd_slice, buffer_slice, "%s-nic user,model=%s%s%s",
		vm_has_rngdev ? "-object rng-random,id=rng0,filename=/dev/random -device virtio-rng-pci,rng=rng0 " : "",
		(char*)g_hash_table_lookup(cfg, "net"),
		vm_has_sharedf ? ",smb=" : "",
		vm_has_sharedf ? g_hash_table_lookup(cfg, "shared") : ""
	);
	out_cmd = add_to_strbuff(out_cmd, cmd_slice);
	
	cfg_v = g_hash_table_lookup(cfg, "floppy");
	if (file_exists((const char*) cfg_v)) {
		snprintf(cmd_slice, buffer_slice, "-drive index=%d,file=%s,if=floppy,format=raw", drive_index, (char*)cfg_v);
		out_cmd = add_to_strbuff(out_cmd, cmd_slice);
		drive_index++;
	}
	
	cfg_v = g_hash_table_lookup(cfg, "cdrom");
	if (file_exists((const char*) cfg_v)) {
		snprintf(cmd_slice, buffer_slice, "-drive index=%d,file=%s,media=cdrom", drive_index, (char*)cfg_v);
		out_cmd = add_to_strbuff(out_cmd, cmd_slice);
		drive_index++;
	}
	
	cfg_v = g_hash_table_lookup(cfg, "disk");
	if (file_exists((const char*) cfg_v)) {
		snprintf(cmd_slice, buffer_slice, "-drive index=%d,file=%s%s", drive_index, (char*)cfg_v, vm_has_hddvirtio ? ",if=virtio" : "");
		out_cmd = add_to_strbuff(out_cmd, cmd_slice);
		drive_index++;
	}
	
	if (vm_clock_is_localtime) {
		out_cmd = add_to_strbuff(out_cmd, "-rtc base=localtime");
	}
	
	out_cmd--; *out_cmd='\0';
	return rc;
}

gboolean program_find_vm_location(int argc, char **argv, char **out_vm_name, char **out_vm_dir, char **out_vm_cfg_file) {
	gboolean rc = FALSE;
	*out_vm_name = NULL;
	*out_vm_dir = NULL;
	*out_vm_cfg_file = NULL;
	
	if (argc == 1) {
		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
		*out_vm_dir = g_strdup(cwd);
	}
	
	const char *vm_name = argv[1];
	const char *vm_dir_env_str = getenv("QEMURUN_VM_PATH");
	gboolean vm_dir_exists = FALSE;
	char vm_dir[PATH_MAX];
	gchar **vm_dir_env = g_strsplit(vm_dir_env_str, ":", 0);

	for (int i = 0; vm_dir_env[i] != NULL && vm_dir_exists == FALSE; i++) {
		snprintf(vm_dir, PATH_MAX, "%s/%s", vm_dir_env[i], vm_name);
		vm_dir_exists = g_dir_exists(vm_dir);
	}
	
	if (vm_dir_exists) {
		char cfg_file[PATH_MAX];
		snprintf(cfg_file, PATH_MAX, "%s/%s", vm_dir, "config");
		*out_vm_name = g_strdup(vm_name);
		*out_vm_dir = g_strdup(vm_dir);
		*out_vm_cfg_file = g_strdup(cfg_file);
		rc = TRUE;
	} else {
		log_msg("Error: Cannot find VM, Check your VM_PATH env. variable ?");
	}
	
	g_strfreev(vm_dir_env);
	return rc;
}

int main(int argc, char **argv) {
	print_gpl_banner();
	char *vm_name = NULL;
	char *vm_dir = NULL;
	char *vm_cfg_file = NULL;
	char cmd[buffer_max];
	
	if (! program_find_vm_location(argc, argv, &vm_name, &vm_dir, &vm_cfg_file)) {
		return 1;
	}
	
	GHashTable *cfg = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	program_get_cfg_values(cfg, vm_dir);
	
	if (! config_load(vm_cfg_file, cfg)) {
		log_msg("Error: cannot load config.");
		return 1;
	}
	
	if (! program_build_cmd_line(cfg, vm_dir, vm_name, cmd)) {
		return 1;
	}

	g_free(vm_name); g_free(vm_dir); g_free(vm_cfg_file);
	g_hash_table_destroy(cfg);
	
	printf("Command line arguments:\n%s\n", cmd);
	system(cmd);
	return 0;
}
