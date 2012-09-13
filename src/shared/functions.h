
/*!
 * \file functions.h
 * \brief Cracker-ng (optimized) functions headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.13
 */


#ifndef FUNCTIONS_NG_H
#define FUNCTIONS_NG_H

#include <iostream>
#include <cstring>
#include <algorithm> // transform()
#include <fstream>
#include "stats.h"

using namespace std;


namespace functions_ng {

typedef struct {
	size_t       * num;
	unsigned int * found;
} statistics;

typedef struct {
	string     module;
	string     version;
	string   & filename;
	string   & input;
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

unsigned int argz_traitment(arguments &);
unsigned int get_cores();
void help(const string);
void result(const string);
void *stats(void *argz);
void usage(string);
void version(const string, const string);

}

#endif // FUNCTIONS_NG_H
