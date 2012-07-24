
/*!
 * \file crypt.cpp
 * \brief ZIP Cracker-ng needed functions from Info-ZIP crypt.c file.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 * 
 * Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.
 * Copyright (C) 2011-2012 Mickaël 'Tiger-222' Schoentgen.
 * 
 * See the accompanying file LICENSE, version 2005-Feb-10 or later for 
 * terms of use. If, for some reason, all these files are missing, the
 * Info-ZIP license also may be found at: 
 * ftp://ftp.info-zip.org/pub/infozip/license.html
 */


#include "crypt.hpp"


inline unsigned char decrypt_byte(void) {
    static unsigned short temp;
    temp = keys[2] | 2;
    return (temp * (temp ^ 1)) >> 8;
}

inline void init_keys(const char* passwd) {
	keys[0] = 0x12345678;
	keys[1] = 0x23456789;
	keys[2] = 0x34567890;
	for ( ;; ) {
		if ( *passwd == '\0' ) {
			break;
		}
		update_keys((int)*passwd);
		++passwd;
	}
}

inline void update_keys(int c) {
	keys[0] = crc32(keys[0], c);
	keys[1] = (keys[1] + (keys[0] & 0xff)) * 134775813 + 1;
	{
		register int keyshift = (int)(keys[1] >> 24);
		keys[2] = crc32(keys[2], keyshift);
    }
}
