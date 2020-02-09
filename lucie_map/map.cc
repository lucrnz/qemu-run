#include <map>
#include <iostream>
#include "map.h"

#define cpp_map \
	std::map<std::string, std::string>
#define cpp_pair \
	std::pair<std::string, std::string>

extern "C" map* map_init() {
	map* m = (map*) malloc(sizeof(map));
	m->_the_map = (void*) new cpp_map();
	return m;
}

extern "C" void map_insert(map* _m, char* key, char* value) {
	cpp_map* m = (cpp_map*) _m->_the_map;
	m->insert(cpp_pair(std::string(key), std::string(value)));
}

extern "C" int map_find(map* _m, char* key, char** value_ptr) {
	int rc = 1; // 1 = Error, 0 = OK
	cpp_map m = *( (cpp_map*) _m->_the_map );
	cpp_map::iterator it;

	if( (it = m.find(std::string(key))) != m.end() ) {
		rc = 0;
	}
	return rc;
}

extern "C" void map_erease(map* _m, char* key) {
	
}

extern "C" void map_free(map* m) {
	free(m->_the_map);
	free(m);
}

