
/*!
 * \file ccryptlib.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 *
 * Copyright (C) 2000-2009 Peter Selinger.
 * Copyright (C) 2011-2021 Mickaël 'Tiger-222' Schoentgen.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 *
 * ccryptlib.h: library for decrypting a character stream.
 *
 * This is an epured/optimized version for Cracker-ng.
 */

#ifndef SRC_CPT_CCRYPTLIB_H_
#define SRC_CPT_CCRYPTLIB_H_


#include "./rijndael.h"


inline int ccdecrypt(
	unsigned int inbuf[8],
	const char*  key,
	roundkey&    rkk
) {
	unsigned int i, keyblock[8] = {0}, rijndaelkey[8] = {0};

	// Generate the roundkeys
	do {
		for ( i = 0; i < 32 && *key; ++key, ++i ) {
			reinterpret_cast<unsigned char*>(rijndaelkey)[i] ^= *key;
		}
		xrijndaelKeySched(rijndaelkey, &rkk);
		xrijndaelEncrypt(keyblock, &rkk);
	} while ( *key );

	// Décrypt the block
	xrijndaelKeySched(keyblock, &rkk);
	xrijndaelDecrypt(inbuf, &rkk);

	// Check the magic number!
	return memcmp(inbuf, "c051", 4);
}

#endif  // SRC_CPT_CCRYPTLIB_H_
