
/*!
 * \file ccryptlib.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.09
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

#ifndef CCRYPTLIB_H
#define CCRYPTLIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rijndael.h"
#include "../shared/functions-ng.h"

#define MAGIC "c051"   /* magic string for this version of ccrypt */

typedef struct {
	unsigned int avail_in; /* number of bytes available at next_in */
	char _pad[4];          /* padding to feet the good alignment */
	char *next_in;         /* next input byte */
	void *state;           /* internal state, not visible by applications */
} ccrypt_stream_s;

typedef struct {  
	unsigned int bufindex; /* in bytes */
	char _pad[4];          /* padding to feet the good alignment */
	word32 buf[8];         /* current buffer; partly ciphertext, partly mask */
	roundkey *rkks;        /* array of n keys */
} ccrypt_state_s;


/* ---------------------------------------------------------------------- */
/* some private functions dealing with hashes, keys, and nonces */

/* hash a keystring into a 256-bit cryptographic random value. */
inline void hashstring(const char *keystring, word32 *hash) {
	register unsigned int i;
	roundkey rkk;
	word32 key[8] = {0};      /* rijndael key */
	for ( ;; ) {
		for ( i = 0; i < 32; ++i ) {
			if ( *keystring == '\0' ) {
				break;
			}
			((word8 *)key)[i] ^= *keystring;
			++keystring;
		}
		xrijndaelKeySched(key, &rkk);
		xrijndaelEncrypt(hash, &rkk);
		if ( *keystring == '\0' ) {
			break;
		}
	}
}

inline void ccdecrypt_init(ccrypt_stream_s *b, ccrypt_state_s *st, const char *key) {
	word32 keyblock[8] = {0};
	b->state = NULL;
	/* generate the roundkeys */
	hashstring(key, keyblock);
	xrijndaelKeySched(keyblock, &st->rkks[0]);
	/* Initialize rest of the state. */
	st->bufindex = 0;
	b->state = (void *)st;
}

inline unsigned int ccdecrypt(ccrypt_stream_s *b) {
	ccrypt_state_s *st = (ccrypt_state_s *)b->state;
	word32 lbuf[8];
	char *cbuf = (char *)st->buf;
	for ( ;; ) {
		cbuf[st->bufindex] = *b->next_in;
		++b->next_in;
		--b->avail_in;
		++st->bufindex;
		if ( st->bufindex == 32 ) {
			/* check the "magic number" */
			memcpy(lbuf, st->buf, 32);
			xrijndaelDecrypt(lbuf, &st->rkks[0]);
			if ( functions::memcmp_ng<char>((char *)lbuf, MAGIC, 4) == 0 ) {
				/* key matches */
				return 0;
			}
			/* not found */
			return 2;
		}
	}
	return 1;
}

#endif // CCRYPTLIB_H
