#ifndef LUCIE_MAP
#define LUCIE_MAP

struct map {
	void* _the_map;	
};

extern "C" map* map_init();
extern "C" void map_insert(map* m, char* key, void* value);
extern "C" void map_find(map* m, char* key, void** value);
extern "C" void map_free(map* m);

#endif
