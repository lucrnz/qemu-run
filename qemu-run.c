#include "imports.h"

enum {ERR_UNKOWN,ERR_ARGS,ERR_ENV,ERR_CHDIR_VM_DIR,ERR_OPEN_CONFIG,ERR_FIND_CONFIG,ERR_SYS,ERR_NETCONF_IP,ERR_SHAREDF,ERR_EXEC,ERR_ENDLIST};
void fatal(unsigned int errcode) {
	char *errs[]={
		"Unkown error.",// ERR_UNKOWN
		"Invalid argument count. Did you specified the VM Name?",
		"Cannot find VM. Please check your QEMURUN_VM_PATH env. variable",
		"Cannot access VM folder. Maybe check its permissions",
		"Cannot find VM config file. Is it created?",
		"Cannot open config file. Maybe check file permissions",
		"Config file has an invalid value for sys",
		"Invalid configuration: VM has enabled network, but has IPv6 and IPv4 disabled. Please enable at least one",
		"Invalid configuration: VM has disabled network, and specified a shared folder. Enable network or disable the shared folder.",
		"There was an error trying to execute qemu. Is it installed?"
	};
	dprint();
	printf("There was an error in the program:\n\t%s.\n",errs[errcode<ERR_ENDLIST?errcode:0]);
	exit(1);
}

int sym_hash_generate(char *str) {
	int pos=0,hash=0xCAFEBABE;
	while(str[pos]) {
		hash=~(((((hash&0xFF)^str[pos])<<5)|
			(((hash>>26)&0x1f)^(pos&31)))|
			(hash<<18));
		pos++;
	}
	hash&=0xFFFFFFFF;
	return hash;
}

bool sym_put_kv(char *key,char *val) {
	int cnt=0,ret=0,hash=sym_hash_generate(key);
	for(;cnt<KEY_ENDLIST;cnt++) {
		if(cfg[cnt].hash==hash) { ret=1;cfg[cnt].val=l_str_dup(val); }
	}
	return ret;
}

enum {FT_TYPE,FT_UNKNOWN=0,FT_PATH,FT_FILE};
int filetype(const char *fpath,int type) {
	int ret=0; struct stat sb;
	if((ret=(access (fpath,0)==0))) {
		stat(fpath,&sb);
		ret=S_ISREG(sb.st_mode)>0?FT_FILE:(S_ISDIR(sb.st_mode)?FT_PATH:FT_UNKNOWN);
	}
	return ret=(type)?ret==type:ret;
}

bool get_binary_full_path(char *bin_fname, char *out_bin_fpath, char *out_dir) {
	dprint();
	bool found = 0, have_slice = 0;
	size_t slice_len = 0;
	char fp_b[PATH_MAX], dir_pb[PATH_MAX], *dir_pq, *env, *slice;
	if (! (env=getenv("PATH"))){ fatal(ERR_EXEC); }
	slice = env;
	do {
		have_slice = l_str_slice(slice, PSEP_C, &slice_len);
		mzero_ca(fp_b); mzero_ca(dir_pb);
		strncpy(dir_pb, slice, slice_len);
		dir_pq = l_str_rm_surrc(dir_pb, '\"');
		l_str_catx(fp_b, dir_pq, DSEP, bin_fname, NULL); found = filetype(fp_b, FT_FILE);
		if (!found) { mzero_ca(fp_b); l_str_catx(fp_b, dir_pq, DSEP, bin_fname, ".exe", NULL); found = filetype(fp_b, FT_FILE); }
		if (!found) { mzero_ca(fp_b); l_str_catx(fp_b, dir_pq, DSEP, bin_fname, ".bat", NULL); found = filetype(fp_b, FT_FILE); }
		if (!found) { mzero_ca(fp_b); l_str_catx(fp_b, dir_pq, DSEP, bin_fname, ".com", NULL); found = filetype(fp_b, FT_FILE); }
		if (found && out_dir) { strcpy(out_dir, dir_pq); }
		slice = slice + slice_len + 1;
	} while (have_slice && !found);
	if (found && out_bin_fpath) { strcpy(out_bin_fpath, fp_b); }
	return found;
}

void program_load_config(const char *fpath) {
	dprint();
	FILE *fptr=fopen(fpath,"r");
#ifdef DEBUG
	printf("fpath=%s\n",fpath);
#endif
	if(!fptr) { fatal(ERR_OPEN_CONFIG); }
	char line[BUFF_AVG*2]={0}, key[BUFF_AVG], val[BUFF_AVG], *slice;
	size_t slice_len = 0;
	int i = 0, have_slice = 0;
	while(fgets(line,BUFF_AVG*2,fptr)) {
		if(!strchr(line,'=') || line[0]=='#') { continue; }
		size_t len=strlen(line);
		if(len&&line[len-2]=='\r') { len--; line[len-1]='\0'; } 
		if(len&&line[len-1]=='\n') { len--; line[len]='\0'; }
		if(len<3) { continue; }
		slice = &line[0]; i = 0;
		do {
			have_slice = l_str_slice(slice, '=', &slice_len);
			if(i==0) { mzero_ca(key); strncpy(key, slice, slice_len);}
			if(i==1) { mzero_ca(val); strncpy(val, slice, slice_len); break; }
			slice = slice + slice_len + 1; i++;
		} while(have_slice && i<2);
		sym_put_kv(key, val);
	}
	fclose(fptr);
}

void program_set_default_cfg_values() {
	dprint();
#ifdef __WINDOWS__
	sym_put_kv("acc","no");
	sym_put_kv("cpu","max");
	sym_put_kv("rng_dev","no");
#endif
	/* Because now qemu-run chdirs into vm_dir,
	it's not needed to append vm_dir to the filename. */
	if(filetype("shared",FT_PATH))			{ sym_put_kv("shared","shared"); }
	if(filetype("floppy",FT_FILE))			{ sym_put_kv("floppy","floppy"); }
	if(filetype("floppy.img",FT_FILE))		{ sym_put_kv("floppy","floppy.img"); }
	if(filetype("cdrom",FT_FILE))			{ sym_put_kv("cdrom","cdrom"); }
	if(filetype("cdrom.iso",FT_FILE))		{ sym_put_kv("cdrom","cdrom.iso"); }
	if(filetype("disk",FT_FILE))			{ sym_put_kv("disk","disk"); }
	if(filetype("disk.qcow2",FT_FILE))		{ sym_put_kv("disk","disk.qcow2"); }
	if(filetype("disk.raw",FT_FILE))		{ sym_put_kv("disk","disk.raw"); }
	if(filetype("disk.img",FT_FILE))		{ sym_put_kv("disk","disk.img"); }
	if(filetype("disk.vmdk",FT_FILE))		{ sym_put_kv("disk","disk.vmdk"); }
	if(filetype("disk.vdi",FT_FILE))		{ sym_put_kv("disk","disk.vdi"); }
	if(filetype("disk.vpc",FT_FILE))		{ sym_put_kv("disk","disk.vpc"); }
	if(filetype("disk.vhdx",FT_FILE))		{ sym_put_kv("disk","disk.vhdx"); }
}

void program_build_cmd_line(char *vm_name, char *out_cmd) {
	int drive_index = 0; // telnet_port = 55555; // @TODO: Get usable TCP port
	char drive_str[4]={0};
	dprint();
#ifdef __WINDOWS__
	char qemu_binary_file[64] = {0};
#else
	bool vm_has_rngdev=stricmp(cfg[KEY_RNG_DEV].val,"yes")==0;
#endif
	bool vm_has_name = (strcmp(vm_name, "") != 0 );
	bool vm_has_acc_enabled = stricmp(cfg[KEY_ACC].val, "yes")==0;
	bool vm_has_vncpwd = (strcmp(cfg[KEY_VNC_PWD].val, "") != 0);
	bool vm_has_audio = stricmp(cfg[KEY_SND].val, "no")!=0;
	bool vm_has_videoacc = stricmp(cfg[KEY_HOST_VIDEO_ACC].val, "yes")==0;
	bool vm_is_headless = stricmp(cfg[KEY_HEADLESS].val, "yes")==0;
	bool vm_clock_is_localtime = stricmp(cfg[KEY_LOCALTIME].val, "yes")==0;
	bool vm_has_sharedf = (strcmp(cfg[KEY_SHARED].val, "") != 0 && filetype(cfg[KEY_SHARED].val, FT_PATH));
	bool vm_has_hddvirtio = stricmp(cfg[KEY_HDD_VIRTIO].val, "yes")==0;
	bool vm_has_network = (strcmp(cfg[KEY_NET].val, "no") != 0 );
	bool vm_has_fwd_ports = strcmp(cfg[KEY_FWD_PORTS].val, "no") != 0;
	bool vm_has_ipv4 = stricmp(cfg[KEY_IPV4].val, "yes")==0;
	bool vm_has_ipv6 = stricmp(cfg[KEY_IPV6].val, "yes")==0;
	vm_has_sharedf = vm_has_sharedf ? filetype(cfg[KEY_SHARED].val,FT_PATH) : 0;

	if (strcmp(cfg[KEY_SYS].val, "x32") == 0) {
		strcpy(out_cmd, "qemu-system-i386");
#ifdef __WINDOWS__
		strcpy(qemu_binary_file, "qemu-system-i386");
#endif
	} else if (strcmp(cfg[KEY_SYS].val, "x64") == 0) {
		strcpy(out_cmd, "qemu-system-x86_64");
#ifdef __WINDOWS__
		strcpy(qemu_binary_file, "qemu-system-x86_64");
#endif
	} else { fatal(ERR_SYS); }
	
	if(vm_has_acc_enabled) { strcat(out_cmd, " --enable-kvm"); }
	if(vm_has_name) { l_str_catx(out_cmd, " -name ",  vm_name, NULL); }

	l_str_catx(out_cmd,
		" -cpu ", cfg[KEY_CPU].val,
		" -smp ", cfg[KEY_CORES].val,
		" -m ", cfg[KEY_MEM].val,
		" -boot order=", cfg[KEY_BOOT].val,
		" -usb -device usb-tablet -vga ", cfg[KEY_VGA].val,
		NULL);

	if(vm_has_audio) { l_str_catx(out_cmd, " -soundhw ", cfg[KEY_SND].val, NULL); }

	if (vm_is_headless) {
		l_str_catx(out_cmd, " -display none -monitor telnet:127.0.0.1:55555,server,nowait -vnc 127.0.0.1:0", vm_has_vncpwd ? ",password" : "", NULL);
	} else {
		l_str_catx(out_cmd, vm_has_videoacc ? " -display gtk,gl=on" : " -display gtk,gl=off", NULL);
	}

	if (vm_has_network && !vm_has_ipv6 && !vm_has_ipv4) { fatal(ERR_NETCONF_IP); }
	if (!vm_has_network && vm_has_sharedf) { fatal(ERR_SHAREDF); }
	if (vm_has_network) {
		l_str_catx(out_cmd,
				" -nic user,model=", cfg[KEY_NET].val,
				vm_has_ipv4 ? ",ipv4=on" : ",ipv4=off",
				vm_has_ipv6 ? ",ipv6=on" : ",ipv6=off",
				NULL);
	}
	if (vm_has_network && vm_has_sharedf) { l_str_catx(out_cmd, ",smb=", cfg[KEY_SHARED].val, NULL); }
	if (vm_has_network && vm_has_fwd_ports) {
		char fwd_port_a[6], fwd_port_b[6], *slice;
		if (strchr(cfg[KEY_FWD_PORTS].val, ':') != NULL) { // If have fwd_ports=<HostPort>:<GuestPort>
			int i = 0, have_slice = 0;
			size_t slice_len;
			slice = &cfg[KEY_FWD_PORTS].val[0];
			do {
				have_slice = l_str_slice(slice, ':', &slice_len);
				if(i==0) { mzero_ca(fwd_port_a); strncpy(fwd_port_a, slice, slice_len); }
				if(i==1) { mzero_ca(fwd_port_b); strncpy(fwd_port_b, slice, slice_len); }
				slice = slice + slice_len + 1; i++;
			} while(have_slice && i<2);
			l_str_catx(out_cmd,",hostfwd=tcp::",fwd_port_a,"-:",fwd_port_b,",hostfwd=udp::",fwd_port_a, "-:",fwd_port_b, NULL);
		} else { // Else use the same port for Host and Guest.
			char *port = cfg[KEY_FWD_PORTS].val;
			l_str_catx(out_cmd,",hostfwd=tcp::",port,"-:",port,",hostfwd=udp::",port, "-:",port, NULL);
		}
	}

	if (filetype(cfg[KEY_FLOPPY].val,FT_FILE)) {
		l_int_to_str(drive_index, drive_str);
		l_str_catx(out_cmd, " -drive index=", drive_str, ",file=", cfg[KEY_FLOPPY].val, ",format=raw", NULL);
		drive_index++;
	}

	if (filetype(cfg[KEY_CDROM].val,FT_FILE)) {
		l_int_to_str(drive_index, drive_str);
		l_str_catx(out_cmd, " -drive index=", drive_str, ",file=", cfg[KEY_CDROM].val, ",media=cdrom", NULL);
		drive_index++;
	}

	if (filetype(cfg[KEY_DISK].val,FT_FILE)) {
		l_int_to_str(drive_index, drive_str);
		l_str_catx(out_cmd, " -drive index=", drive_str, ",file=", cfg[KEY_DISK].val, vm_has_hddvirtio ? ",if=virtio" : "", NULL);
		drive_index++;
	}

#ifdef __WINDOWS__
	/* QEMU on Windows needs,for some reason,to have an additional argument with the program path on it,
	* For example if you have it on: "C:\Program Files\Qemu",You have to run it like this: qemu-system-i386.exe -L "C:\Program Files\Qemu"
	* Otherwise it wont find the BIOS file.. */
	char qemu_binary_full_path[BUFF_AVG]={0};
	char *qemu_binary_full_path_p = &qemu_binary_full_path[0];
	if (! get_binary_full_path(qemu_binary_file, NULL, qemu_binary_full_path_p)) { fatal(ERR_EXEC); }
	l_str_catx(out_cmd, " -L \"", qemu_binary_full_path_p, "\"", NULL);
#else
	if (vm_has_rngdev) { strcat(out_cmd, " -object rng-random,id=rng0,filename=/dev/random -device virtio-rng-pci,rng=rng0"); }
#endif
	if (vm_clock_is_localtime) { strcat(out_cmd, " -rtc base=localtime"); }
}

void program_find_vm_and_chdir(int argc,char **argv,char *out_vm_name,char *out_vm_cfg_file) {
	char vm_dir[PATH_MAX+1], env_dir[PATH_MAX]={0}, *env, *slice, *env_dir_q;
	bool vm_dir_exists = 0, cfg_file_exists = 0, have_slice = 0;
	size_t slice_len;
	dprint();
	if (argc < 2) { fatal(ERR_ARGS); }
	strcpy(out_vm_name, argv[1]);
	if (!(env = getenv("QEMURUN_VM_PATH"))) { fatal(ERR_ENV); }
	if (strcmp(env, "") == 0) { fatal(ERR_ENV); }
	slice = &env[0];
	do {
		have_slice = l_str_slice(slice, PSEP_C, &slice_len);
		mzero_ca(env_dir);
		mzero_ca(vm_dir);
		strncpy(env_dir, slice, slice_len);
		env_dir_q = l_str_rm_surrc(env_dir, '\"');
		l_str_catx(vm_dir, env_dir_q, DSEP, out_vm_name, NULL);
		vm_dir_exists = filetype(vm_dir, FT_PATH);
		slice = slice + slice_len + 1;
	} while(have_slice && !vm_dir_exists);
	if(! vm_dir_exists) { fatal(ERR_ENV); }
	if(chdir(vm_dir) != 0) { fatal(ERR_CHDIR_VM_DIR); }
	strcpy(out_vm_cfg_file,"config"); cfg_file_exists=filetype(out_vm_cfg_file,FT_FILE);
	if(! cfg_file_exists) { strcpy(out_vm_cfg_file,"config.ini"); cfg_file_exists=filetype(out_vm_cfg_file,FT_FILE); }
	if(! cfg_file_exists) { fatal(ERR_FIND_CONFIG); }
}

int main(int argc,char **argv) {
	char cmd[BUFF_MAX],vm_name[BUFF_AVG],vm_cfg_file[BUFF_AVG];
	dprint();
	program_find_vm_and_chdir(argc,argv,vm_name,vm_cfg_file);
	program_set_default_cfg_values();
	program_load_config(vm_cfg_file);
#ifdef DEBUG
	puts("Hash table:"); for(int i=0;i<KEY_ENDLIST;i++) printf("%d 0x%8.8X='%s'\n",i,cfg[i].hash,cfg[i].val);
#endif
	program_build_cmd_line(vm_name,cmd);
	puts("QEMU Command line arguments:");
	puts(cmd);
	return system(cmd); // Just Pass Qemu's Err code on
}
