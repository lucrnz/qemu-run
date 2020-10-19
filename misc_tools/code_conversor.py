#!/usr/bin/env python3

code="""
	cfg['System'] = 'x64'
	cfg['UseUefi'] = 'No'
	cfg['CpuType'] = 'host'
	cfg['CpuCores'] = subprocess.check_output(['nproc']).decode(sys.stdout.encoding).strip()
	cfg['MemorySize'] = '2G'
	cfg['Acceleration'] = 'Yes'
	cfg['DisplayDriver'] = 'virtio'
	cfg['SoundDriver'] = 'hda'
	cfg['Boot'] = 'c'
	cfg['FwdPorts'] = ''
	cfg['HardDiskVirtio'] = 'Yes'
	cfg['SharedFolder'] = 'shared'
	cfg['NetworkDriver'] = 'virtio-net-pci'
	cfg['RngDevice'] = 'Yes'
	cfg['HostVideoAcceleration'] = 'No'
	cfg['LocalTime'] = 'No'
	cfg['Headless'] = 'No'
	cfg['MonitorPort'] = 5510
	cfg['CDRomISO'] = '{}/cdrom'.format(vm_dir) if os.path.isfile('{}/cdrom'.format(vm_dir)) else 'No'
	cfg['HardDisk'] = '{}/disk'.format(vm_dir) if os.path.isfile('{}/disk'.format(vm_dir)) else 'No'"""

values = {}
keys = []
for line in code.splitlines():
	if line.find("['") != -1:
		sline = line.strip()
		key = sline.split("['")[1].split("']")[0]
		value = sline.split("=")[1].strip()
		value_is_code = True
		if value[0] == "'" and value[-1] == "'":
			value = value.split("'", maxsplit=1)[1].split("'", maxsplit=1)[0]
			value_is_code = False
		values[key] = value
		keys.append(key)
		if value_is_code == False:
			print(f'	map_insert(config, "{key}", "{value}");')

