
/*!
 * \file functions-ng.cpp
 * \brief Cracker-ng optimized functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 * 
 * Native functions optimized to speed up crack processes.
 * See CREDITS for informations about authors/helpers.
 */


#include "functions-ng.hpp"


// Optimized strncmp()/memcmp() ----------------------------------------
template<class T>
inline int memcmp_ng(const T *st, const T *nd, size_t n) {
	const T *end = st + n / sizeof(T);
	for ( ; st < end; ) {
		if ( *st > *nd ) return 1;
		if ( *st++ < *nd++ ) return -1;
	}
	return 0;
}
