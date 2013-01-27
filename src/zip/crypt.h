
/*!
 * \file crypt.h
 * \brief ZIP Cracker-ng headers for the Traditional PKWARE Encryption.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.27
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


/*!
 * \var keys
 * \brief Three 32-bit keys for CRC-32 calculus.
 */
extern uint32_t keys[3];

/*!
 * \var pcrc_32_tab
 * \brief Table of CRC-32's of all single-byte values.
 */
extern const uint32_t pcrc_32_tab[256];


/*!
 * \fn create_crc32(const unsigned char* buf, size_t len, uint32_t& good)
 * \brief Calculate the CRC-32 of the decrypted data.
 * \param but Pointer to the decrypted data.
 * \param len Length of the decrypted data.
 * \param good CRC32 to match.
 * \return \li 0 if CRC-32 are \b not equals;
 * \return \li 1 otherwise.
 */
inline bool create_crc32(const unsigned char* buf, size_t len, uint32_t& good) {
	register uint32_t c = 0xffffffff;
	for ( ; len; --len ) {
		c = pcrc_32_tab[(c ^ *buf++) & 0xff] ^ (c >> 8);
	}
	return c == good;
}

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
	keys[0] = pcrc_32_tab[(keys[0] ^ c) & 0xff] ^ (keys[0] >> 8);
	keys[1] = (keys[1] + (keys[0] & 0xff)) * 134775813 + 1;
	keys[2] = pcrc_32_tab[(keys[2] ^ (keys[1] >> 24)) & 0xff] ^ (keys[2] >> 8);
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
