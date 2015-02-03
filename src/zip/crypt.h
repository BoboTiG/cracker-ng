
/*!
 * \file crypt.h
 * \brief ZIP Cracker-ng headers for the Traditional PKWARE Encryption.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2014.01.04
 *
 * Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.
 * Copyright (C) 2012-2014 Mickaël 'Tiger-222' Schoentgen.
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
 *
 * Note: polynomial is 0xedb88320.
 */
extern const uint32_t pcrc_32_tab[8][256];


/*!
 * \fn create_crc32(const unsigned char* buf, size_t len)
 * \brief Calculate the CRC-32 of the decrypted data.
 * \param but Pointer to the decrypted data.
 * \param len Length of the decrypted data.
 * \return The calculated CRC32.
 */
inline uint32_t create_crc32(const unsigned char* buf, size_t len) {
	uint32_t* current = (uint32_t*)buf;
	uint32_t one, two;
	register uint32_t crc = 0xffffffff;

	for ( ; len > 7; len -= 8 ) {
		one = *current++ ^ crc;
		two = *current++;
		crc =
			pcrc_32_tab[7][ one      & 0xff] ^
			pcrc_32_tab[6][(one>> 8) & 0xff] ^
			pcrc_32_tab[5][(one>>16) & 0xff] ^
			pcrc_32_tab[4][ one>>24        ] ^
			pcrc_32_tab[3][ two      & 0xff] ^
			pcrc_32_tab[2][(two>> 8) & 0xff] ^
			pcrc_32_tab[1][(two>>16) & 0xff] ^
			pcrc_32_tab[0][ two>>24        ];
	}
	unsigned char* c = reinterpret_cast<unsigned char*>(current);
	for ( ; len; --len ) {
		crc = (crc >> 8) ^ pcrc_32_tab[0][(crc ^ *c++) & 0xff];
	}
	return ~crc;
}

/*!
 * \fn decrypt_byte(void)
 * \brief Return the next byte in the pseudo-random sequence.
 * \return Decrypted byte.
 * \bug POTENTIAL:  temp*(temp^1) may overflow in an unpredictable
 * manner on 16-bit systems; not a problem with any known compiler
 * so far, though.
 */
inline unsigned char decrypt_byte() {
    register uint16_t temp = keys[2] | 2;
    return (temp * (temp ^ 1)) >> 8;
}

/*!
 * \fn update_keys(int c)
 * \brief Update the encryption keys with the next byte of plain text.
 * \param c The next byte.
 */
inline void update_keys(const int& c) {
	keys[0] = (keys[0] >> 8) ^ pcrc_32_tab[0][(keys[0] ^ c) & 0xff];
	keys[1] = (keys[1] + (keys[0] & 0xff)) * 134775813 + 1;
	keys[2] = (keys[2] >> 8) ^ pcrc_32_tab[0][(keys[2] ^ (keys[1] >> 24)) & 0xff];
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
	while (*passwd) {
		update_keys(*passwd++);
	}
}

#endif  // SRC_ZIP_CRYPT_H_
