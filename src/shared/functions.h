
/*!
 * \file functions.h
 * \brief Cracker-ng (optimized) functions headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.11.22
 *
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_SHARED_FUNCTIONS_H_
#define SRC_SHARED_FUNCTIONS_H_

#include <algorithm>  // transform()
#include <string>
#include "./stats.h"


namespace functions_ng {

typedef struct {
	size_t       * num;
	unsigned int * found;
} statistics;

typedef struct {
	std::string   module;
	std::string   version;
	std::string & filename;
	std::string & input;
	size_t        argc;
	char**        argv;
} arguments;

// Optimized read from stdin or wordlist
inline bool read_stdin(char *tmp_buffer, size_t len, FILE *input, char *&output) {
	output = fgets(tmp_buffer, len, input);
	if ( output != NULL ) {
		char *lf = strchr(output, '\n');
		if ( lf != NULL ) {
			*lf = '\0';
		}
		return true;
	}
	return false;
}

unsigned int argz_traitment(const arguments &);
unsigned int get_cores();
void help(const std::string&);
void result(const std::string&);
void *stats(void *argz);
void usage(const std::string&);
void version(const std::string&, const std::string&);
}

#endif  // SRC_SHARED_FUNCTIONS_H_
