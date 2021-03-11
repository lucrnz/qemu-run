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
void map_insert(map* m, const char* key, const char* value);
int map_find(map* m, const char* key, const char** value_ptr);
int map_erase(map* _m, const char* key);
void map_free(map* m);
size_t map_count(map* m);

#ifdef __cplusplus
}
#endif

#endif
