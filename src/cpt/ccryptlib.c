
/*!
 * \file ccryptlib.c
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 * 
 * Copyright (C) 2000-2009 Peter Selinger.
 * Copyright (C) 2011-2012 Mickaël 'Tiger-222' Schoentgen.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 * 
 * ccryptlib implements a stream cipher based on the block cipher
 *  Rijndael, the candidate for the AES standard.
 *
 * This is an optimized version for Cracker-ng.
 */

#include "ccryptlib.h"

/* ---------------------------------------------------------------------- */
/* some private functions dealing with hashes, keys, and nonces */

/* hash a keystring into a 256-bit cryptographic random value. */
static inline void hashstring(const char *keystring, word32 *hash) {
	register unsigned int i;
	roundkey rkk;
	word32 key[8];      /* rijndael key */
	
	for ( i = 0; i < 8; ++i ) {
		key[i] = hash[i] = 0;
	}
	for ( ;; ) {
		for ( i = 0; i < 32; ++i ) {
			if ( *keystring == '\0' ) {
				break;
			}
			((word8 *)key)[i] ^= *keystring;
			++keystring;
		}
		xrijndaelKeySched(key, 256, 256, &rkk);
		xrijndaelEncrypt(hash, &rkk);
		if ( *keystring == '\0' ) {
			break;
		}
	}
}

static inline void ccdecrypt_init(ccrypt_stream_s *b, const char *key) {
	word32 keyblock[8];
	ccrypt_state_s *st;
	roundkey *rkks;
	
	b->state = NULL;
	st = (ccrypt_state_s*)malloc(sizeof(ccrypt_state_s));
	rkks = (roundkey*)malloc(sizeof(roundkey));
	st->rkks = rkks;
	st->ak = 0;
	/* generate the roundkeys */
	hashstring(key, keyblock);
	xrijndaelKeySched(keyblock, 256, 256, &st->rkks[0]);
	/* Initialize rest of the state. */
	st->iv = 1;
	st->bufindex = 0;
	b->state = (void *)st;
}

static inline unsigned int ccdecrypt(ccrypt_stream_s *b) {
	ccrypt_state_s *st = (ccrypt_state_s *)b->state;
	word32 lbuf[8];
	char *cbuf = (char *)st->buf;
	register unsigned int i, return_code = 0;
	char c, cc;
	
	for ( ;; ) {
		/* handle the typical case efficiently, one block at a time */
		if ( st->iv == 0 && st->bufindex == 32 ) {
			while ( b->avail_in >= 32 && b->avail_out >= 32 ) {
				/* block-encrypt buffer */
				xrijndaelEncrypt(st->buf, &st->rkks[st->ak]);
				memcpy(lbuf, st->buf, 32);
				
				/* read input */
				memcpy(st->buf, b->next_in, 32);
				b->next_in += 32;
				b->avail_in -= 32;
				
				/* compute plaintext */
				for ( i = 0; i < 8; ++i ) {
					lbuf[i] ^= st->buf[i];
				}
				
				/* write output */
				memcpy(b->next_out, lbuf, 32);
				b->next_out += 32;
				b->avail_out -= 32;
			}
		}
		/* handle the general case systematically, one byte at a time */
		if ( b->avail_in == 0 || b->avail_out == 0 ) {
			return_code = 1;
			break;
		}
		if ( st->iv ) {  /* read IV byte */
			cbuf[st->bufindex] = *b->next_in;
			++b->next_in;
			--b->avail_in;
			++st->bufindex;
			if ( st->bufindex == 32 ) {
				st->iv = 0;
				/* check the "magic number" */
				memcpy(lbuf, st->buf, 32);
				xrijndaelDecrypt(lbuf, &st->rkks[0]);
				if ( memcmp_ng<char>((char *)lbuf, MAGIC, 4) == 0 ) {
				/*if ( memcmp(lbuf, MAGIC, 4) == 0 ) {*/
					/* key matches */
					break;
				}
				/* not found */
				free(st->rkks);
				free(st);
				return_code = 2;
				break;
			}
		} else {
			/* decrypt one byte */
			if ( st->bufindex == 32 ) {
				xrijndaelEncrypt(st->buf, &st->rkks[st->ak]);
				st->bufindex = 0;
			}
			cc = *b->next_in;
			++b->next_in;
			--b->avail_in;
			c = cc ^ cbuf[st->bufindex];
			cbuf[st->bufindex] = cc;
			*b->next_out = c;
			++b->next_out;
			--b->avail_out;
			++st->bufindex;
		}
	}
	return return_code;
}
