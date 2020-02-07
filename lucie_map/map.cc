#include <map>
#include "map.h"

#define cpp_map \
	std::map<char*, void*>
#define cpp_pair \
	std::pair<char*, void*>

extern "C" map* map_init() {
	map* m = (map*) malloc(sizeof(map));
	m->_the_map = (void*) new cpp_map();
	return m;
}

extern "C" void map_insert(map* _m, char* key, void* value) {
	cpp_map* m = (cpp_map*) _m->_the_map;
	m->insert(cpp_pair(key, value));
}

extern "C" void* map_find(map* _m, char* key) {
	cpp_map m = *( (cpp_map*) _m->_the_map );
	return (void*) m[key];
}

extern "C" void map_free(map* m) {
	free(m->_the_map);
	free(m);
}

