#include <map>
#include <iostream>
#include "map.h"
#include <cstring>

#define cpp_map \
	std::map<std::string, std::string>
#define cpp_pair \
	std::pair<std::string, std::string>

int sys_set=0;

extern "C" map* map_init() {
	map* m = (map*) malloc(sizeof(map));
	m->_the_map = (void*) new cpp_map();
	return m;
}

extern "C" void map_insert(map* _m, const char* key, const char* value) {
	cpp_map* m = (cpp_map*) _m->_the_map;
	std::string k = std::string(key);

	cpp_map::iterator it;
	if( (it = m->find(k)) != m->end() ) {
		m->erase(it);
	}

	m->insert(cpp_pair(k, std::string(value)));
}

extern "C" int map_find(map* _m, const char* key, const char** value_ptr) {
	int rc = 1;
	cpp_map m = *( (cpp_map*) _m->_the_map );
	cpp_map::iterator it;

	if( (it = m.find(std::string(key))) != m.end() ) {
		rc = 0;
		const char* value_cstr = m.at(key).c_str();
		*value_ptr = value_cstr;
	}
	return rc;
}

extern "C" int map_erase(map* _m, const char* key) {
	int rc = 1;
	cpp_map m = *( (cpp_map*) _m->_the_map );
	cpp_map::iterator it;

	if( (it = m.find(std::string(key))) != m.end() ) {
		m.erase(it);
		rc = 0;
	}
	return rc;
}

extern "C" size_t map_count(map* _m) {
	cpp_map m = *( (cpp_map*) _m->_the_map );
	return m.size();
}

extern "C" void map_free(map* m) {
	free(m->_the_map);
	free(m);
}
