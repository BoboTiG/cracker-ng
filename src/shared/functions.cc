
/*!
 * \file functions.cc
 * \brief Cracker-ng (optimized) functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.02.08
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./functions.h"


namespace functions_ng {

const std::string basename(const std::string& str) {
	size_t pos = str.find_last_of("/");
	return str.substr(++pos);
}

bool file_exists(const char* filename) {
	FILE *test = NULL;
	
	if ( filename != NULL ) {
		test = fopen(filename, "r");
		if ( test != NULL ) {
			fclose(test);
			return true;
		}
	}
	return false;
}

const std::string format_number(const size_t& num) {
	std::stringstream str, format;
	unsigned int i, len;

	str << num;
	len = str.str().size();
	i = len + 1;
	while ( --i ) {
		if ( i < len && i % 3 == 0 ) {
			format << ',';
		}
		format << str.str()[len - i];
	}
	return format.str();
}

/*unsigned int get_cores() {
	unsigned int n = 0;
	char *buf = new char[256];
	FILE *f = fopen("/proc/cpuinfo", "r");

	if ( f != NULL ) {
		for ( ; !feof(f) ; ) {
			memset(buf, 0, sizeof(buf));
			if ( fgets(buf, sizeof(buf), f) == NULL ) {
				break;
			}
			if ( std::string(buf).find("processor") != std::string::npos ) {
				++n;
			}
		}
		fclose(f);
	}
	delete[] buf;
	return n == 0 ? 1 : n;
}*/

const std::string substr(const std::string& str, unsigned int max, bool middle) {
	size_t len = str.length();
	bool need_sub = len > max;

	if ( !need_sub ) {
		return str;
	}
	if ( middle) {
		// File: 012...6789
		unsigned int shift = (len - max) / 2;
		unsigned int part1 = len / 2 - shift - 2;
		unsigned int part2 = len / 2 + shift + 1;
		return str.substr(0, part1) + std::string("...") + str.substr(part2);
	} else {
		// File: ...3456789
		unsigned int part  = len - max + 3;
		return std::string("...") + str.substr(part);
	}
}

}
