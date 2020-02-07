#ifndef LUCIE_MAP
#define LUCIE_MAP

#ifdef __cplusplus
struct map {
	void* _the_map;	
};
#else
typedef struct{
	void* _the_map;
} map;
#endif

#ifdef __cplusplus
extern "C" {
#endif

map* map_init();
void map_insert(map* m, char* key, void* value);
void* map_find(map* m, char* key);
void map_free(map* m);

#ifdef __cplusplus
}
#endif

#endif
