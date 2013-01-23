
/*!
 * \file crypt.h
 * \brief ZIP Cracker-ng headers for the Traditional PKWARE Encryption.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.04
 * 
 * Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * 
 * See the accompanying file LICENSE, version 2005-Feb-10 or later for 
 * terms of use. If, for some reason, all these files are missing, the
 * Info-ZIP license also may be found at: 
 * ftp://ftp.info-zip.org/pub/infozip/license.html
 */


#ifndef SRC_ZIP_CRYPT_H_
#define SRC_ZIP_CRYPT_H_

#include "./crc32.h"

/*!
 * \var keys
 * \brief Three 32-bit keys for CRC-32 calculus.
 */
extern uint32_t keys[3];

/*!
 * \def zdecode(c)
 * \brief Decode byte c in place.
 * \param c Byte de decode.
 * \return Decoded byte.
 */
#define zdecode(c) update_keys(c ^= decrypt_byte())

/*!
 * \fn decrypt_byte(void)
 * \brief Return the next byte in the pseudo-random sequence.
 * \return Decrypted byte.
 * \bug POTENTIAL:  temp*(temp^1) may overflow in an unpredictable 
 * manner on 16-bit systems; not a problem with any known compiler 
 * so far, though.
 */
inline unsigned char decrypt_byte(void) {
    register uint16_t temp = keys[2] | 2;
    return (temp * (temp ^ 1)) >> 8;
}

/*!
 * \fn update_keys(int c)
 * \brief Update the encryption keys with the next byte of plain text.
 * \param c The next byte.
 */
inline void update_keys(int c) {
	keys[0] = pcrc_32_tab[(keys[0] ^ c) & 0xff] ^ (keys[0] >> 8); //crc32(keys[0], c);
	keys[1] = (keys[1] + (keys[0] & 0xff)) * 134775813 + 1;
	keys[2] = pcrc_32_tab[(keys[2] ^ (keys[1] >> 24)) & 0xff] ^ (keys[2] >> 8); // crc32(keys[2], keys[1] >> 24);
}

/*!
 * \fn init_keys(const char* passwd)
 * \brief Initialize the encryption keys and the random header according to the given password.
 * \param passwd The password.
 */
inline void init_keys(const char* passwd) {
	keys[0] = 0x12345678;
	keys[1] = 0x23456789;
	keys[2] = 0x34567890;
	while (*passwd) update_keys(*passwd++);
}

#endif  // SRC_ZIP_CRYPT_H_
