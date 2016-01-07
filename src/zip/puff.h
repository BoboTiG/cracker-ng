
/*!
 * \file puff.h
 * \brief ZIP Cracker-ng headers for the deflate algorithm (optimized for the project).
 * \author Mickaël 'Tiger-222' Schoentgen
 *
 * Copyright (C) 2002-2010 Mark Adler
 * Copyright (C) 2011-2016 Mickaël 'Tiger-222' Schoentgen.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * In the comments below are "Format notes" that describe the inflate process
 * and document some of the less obvious aspects of the format.  This source
 * code is meant to supplement RFC 1951, which formally describes the deflate
 * format:
 *
 *    http://www.zlib.org/rfc-deflate.html
 *
 * Mark Adler    madler@alumni.caltech.edu
*/


#ifndef SRC_ZIP_PUFF_H_
#define SRC_ZIP_PUFF_H_


#include <string.h>  // for memcpy()


// input and output state
struct state {
    // output state
    unsigned char *out;         // output buffer
    unsigned long outlen;       // available space at out
    unsigned long outcnt;       // bytes written to out so far

    // input state
    const unsigned char *in;    // input buffer
    unsigned long inlen;        // available input at in
    unsigned long incnt;        // bytes read so far
    int bitbuf;                 // bit buffer
    int bitcnt;                 // number of bits in bit buffer
};

// Huffman code decoding tables.
struct huffman {
    int *count;       // number of symbols of each length
    int *symbol;      // canonically ordered symbols
};

extern const int  lens[29];  // Size base for length codes 257..285
extern const int  lext[29];  // Extra bits for length codes 257..285
extern const int dists[30];  // Offset base for distance codes 0..29
extern const int  dext[30];  // Extra bits for distance codes 0..29
extern const int order[19];  // permutation of code length codes

// Inflate source to dest.
int puff(
	unsigned char*       dest,       // pointer to destination pointer
	const unsigned long  destlen,    // amount of output space
	const unsigned char* source,     // pointer to source data pointer
	const unsigned long  sourcelen,  // amount of input available
	struct state&        s);         // input/output state

#endif  // SRC_ZIP_PUFF_H_
