
/*!
 * \file ccryptlib.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.28
 * 
 * Copyright (C) 2000-2009 Peter Selinger.
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
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
	roundkey&    rkk_hash,
	unsigned int keyblock[8],
	unsigned int rijndaelkey[8]
) {
	signed int i = -1;

	// Generate the roundkeys
	for ( ; *key; ++key ) {
		reinterpret_cast<unsigned char*>(rijndaelkey)[++i] ^= *key;
	}
	xrijndaelKeySched(rijndaelkey, &rkk_hash);
	xrijndaelEncrypt(keyblock, &rkk_hash);
	
	// Décrypt the block
	xrijndaelKeySched(keyblock, &rkk_hash);
	xrijndaelDecrypt(inbuf, &rkk_hash);
	
	// Check the magic number!
	return memcmp(inbuf, "c051", 4);
}

#endif  // SRC_CPT_CCRYPTLIB_H_
