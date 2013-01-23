
/*!
 * \file functions.h
 * \brief Cracker-ng (optimized) functions headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.23
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_SHARED_FUNCTIONS_H_
#define SRC_SHARED_FUNCTIONS_H_

#include <algorithm>  // transform()
#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <csignal>

#define UNUSED(s) (void)s


namespace functions_ng {

enum FLAG {
	NONE  = 0x0,
	DEBUG = 0x1
};

typedef struct {
	std::string   module;
	std::string   version;
	std::string & filename;
	std::string & input;
	size_t      & flag;
	size_t        argc;
	char**        argv;
} arguments;

// Optimized read from stdin or wordlist
inline bool read_input(FILE* input, char*& output, const size_t& len) {
	if ( fgets(output, len, input) != NULL ) {
		char *lf = strchr(output, '\n');
		if ( lf != NULL ) {
			 *lf = '\0';
		}
		return true;
	}
	return false;
}

bool argz_traitment(const arguments&);
std::string basename(const std::string&);
bool file_exists(const char*);
std::string format_number(const size_t&);
unsigned int get_cores();
void help(const std::string&);
void result(const std::string&);
std::string substr(const std::string&, unsigned int, bool = false);
void usage(const std::string&);
void version(const std::string&, const std::string&);

}

#endif  // SRC_SHARED_FUNCTIONS_H_
