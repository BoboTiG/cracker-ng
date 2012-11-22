
/*!
 * \file ccryptlib.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.22.11
 * 
 * Copyright (C) 2000-2009 Peter Selinger.
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 * 
 * ccryptlib.h: library for decrypting a character stream.
 * 
 * This is an optimized version for Cracker-ng.
 */

#ifndef SRC_CPT_CCRYPTLIB_H_
#define SRC_CPT_CCRYPTLIB_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "./rijndael.h"
#include "../shared/functions.h"

#define MAGIC "c051"  // magic string for this version of ccrypt

typedef struct {
	size_t avail_in;         // number of bytes available at next_in
	char * next_in;          // next input byte
	void * state;            // internal state, not visible by applications
} ccrypt_stream_s;

typedef struct {
	size_t     bufindex;     // in bytes
	word32     buf[8];       // current buffer; partly ciphertext, partly mask
	roundkey * rkks;         // array of n keys
} ccrypt_state_s;


// hash a keystring into a 256-bit cryptographic random value.
inline void hashstring(const char *keystring, word32 *hash, roundkey &rkk) {
	register unsigned int i;
	word32 key[8] = {0};  // rijndael key

	for ( ;; ) {
		for ( i = 0; i < 32; ++i ) {
			if ( *keystring == '\0' ) {
				break;
			}
			reinterpret_cast<word8 *>(key)[i] ^= *keystring;
			++keystring;
		}
		xrijndaelKeySched(key, &rkk);
		xrijndaelEncrypt(hash, &rkk);
		if ( *keystring == '\0' ) {
			break;
		}
	}
}

inline void ccdecrypt_init(
	ccrypt_stream_s *b, ccrypt_state_s *st,
	const char *key, roundkey &rkk_hash
) {
	word32 keyblock[8] = {0};

	b->state = NULL;
	// generate the roundkeys
	hashstring(key, keyblock, rkk_hash);
	xrijndaelKeySched(keyblock, &st->rkks[0]);
	// Initialize rest of the state.
	st->bufindex = 0;
	b->state = reinterpret_cast<void *>(st);
}

inline int ccdecrypt(ccrypt_stream_s *b) {
	ccrypt_state_s *st = reinterpret_cast<ccrypt_state_s *>(b->state);
	char *cbuf = reinterpret_cast<char *>(st->buf);

	for ( ;; ) {
		cbuf[st->bufindex] = *b->next_in;
		++b->next_in;
		--b->avail_in;
		++st->bufindex;
		if ( st->bufindex == 32 ) {
			// check the "magic number"
			xrijndaelDecrypt(st->buf, &st->rkks[0]);
			return memcmp(st->buf, MAGIC, 4);
		}
	}
	return 1;
}

#endif  // SRC_CPT_CCRYPTLIB_H_
