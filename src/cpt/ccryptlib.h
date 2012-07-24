
/*!
 * \file ccryptlib.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2011.12.28
 * 
 * Copyright (C) 2000-2009 Peter Selinger.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 * 
 * ccryptlib.h: library for decrypting a character stream.
 * 
 * This is an optimized version for Cracker-ng.
 */


#include "rijndael.h"

#define MAGIC "c051"   /* magic string for this version of ccrypt */


typedef struct {
	void          *state;    /* internal state, not visible by applications */
	char          *next_in;  /* next input byte */
	char          *next_out; /* next output byte should be put there */
	unsigned int  avail_in;  /* number of bytes available at next_in */
	unsigned int  avail_out; /* remaining free space at next_out */
} ccrypt_stream_s;

typedef struct {  
	roundkey *rkks;        /* array of n keys */
	unsigned int ak;       /* rkks[ak] is the active key */
	unsigned int iv;       /* are we reading/writing the IV? */
	unsigned int bufindex; /* in bytes */
	word32 buf[8];         /* current buffer; partly ciphertext, partly mask */
	char pad[4];           /* padding for alignment */
} ccrypt_state_s;


static inline unsigned int ccdecrypt(ccrypt_stream_s *b);
static inline void ccdecrypt_init(ccrypt_stream_s *b, const char *key);
static inline void hashstring(const char *keystring, word32 *hash);
