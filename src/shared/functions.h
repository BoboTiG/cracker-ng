
/*!
 * \file functions.h
 * \brief Cracker-ng (optimized) functions headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.15
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
	std::string     module;
	std::string     version;
	std::string   & filename;
	std::string   & input;
	int argc;
	char _pad[4];
	char**argv;
} arguments;

// Optimized strncmp()/memcmp()
template<class T>
inline int memcmp_ng(const T *st, const T *nd, size_t n) {
	const T *end = st + n / sizeof(T);
	for ( ; st < end ; ) {
		if ( *st > *nd ) return 1;
		if ( *st++ < *nd++ ) return -1;
	}
	return 0;
}

// Optimized read from stdin or wordlist
inline char *read_stdin(char *buffer, int len, FILE *input) {
	char *result, *lf;
	result = fgets(buffer, len, input);
	if ( result != NULL ) {
		lf = strchr(buffer, '\n');
		if ( lf != NULL )
			*lf = '\0';
	}
	return result;
}

unsigned int argz_traitment(const arguments &);
unsigned int get_cores();
void help(const std::string);
void result(const std::string);
void *stats(void *argz);
void usage(std::string);
void version(const std::string, const std::string);
}

#endif  // SRC_SHARED_FUNCTIONS_H_
