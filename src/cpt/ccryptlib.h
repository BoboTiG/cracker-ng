
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
	roundkey&    rkk
) {
	unsigned int i = 0, keyblock[8] = {0}, rijndaelkey[8] = {0};

	// Generate the roundkeys
	for ( ; *key; ++key, ++i ) {
		reinterpret_cast<unsigned char*>(rijndaelkey)[i] ^= *key;
	}
	xrijndaelKeySched(rijndaelkey, &rkk);
	xrijndaelEncrypt(keyblock, &rkk);
	
	// Décrypt the block
	xrijndaelKeySched(keyblock, &rkk);
	xrijndaelDecrypt(inbuf, &rkk);
	
	// Check the magic number!
	return memcmp(inbuf, "c051", 4);
}

#endif  // SRC_CPT_CCRYPTLIB_H_
