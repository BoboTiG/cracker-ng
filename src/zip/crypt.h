
/*!
 * \file crypt.h
 * \brief ZIP Cracker-ng headers for the Traditional PKWARE Encryption.
 * \author Mickaël 'Tiger-222' Schoentgen
 *
 * Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.
 * Copyright (C) 2011-2022 Mickaël 'Tiger-222' Schoentgen.
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
extern const uint32_t pcrc_32_tab[16][256];


/*!
 * \fn create_crc32(const unsigned char* buf, size_t len)
 * \brief Calculate the CRC-32 of the decrypted data.
 * \param but Pointer to the decrypted data.
 * \param len Length of the decrypted data.
 * \return The calculated CRC32.
 *
 * Source: http://create.stephan-brumme.com/crc32/#slicing-by-16-overview
 */
inline uint32_t create_crc32(const unsigned char* buf, size_t len) {
	uint32_t* current = (uint32_t*)buf;
	uint32_t one, two, three, four;
	uint32_t crc = 0xffffffff;
	unsigned int unroll;

	for ( ; len >= 320; len -= 64 ) {
		__builtin_prefetch(current + 256);
		for ( unroll = 0; unroll < 4; ++unroll ) {
			one   = *current++ ^ crc;
			two   = *current++;
			three = *current++;
			four  = *current++;
			crc   =
				pcrc_32_tab[ 0][(four >> 24)  & 0xff] ^
				pcrc_32_tab[ 1][(four >> 16)  & 0xff] ^
				pcrc_32_tab[ 2][(four >> 8)   & 0xff] ^
				pcrc_32_tab[ 3][ four         & 0xff] ^
				pcrc_32_tab[ 4][(three >> 24) & 0xff] ^
				pcrc_32_tab[ 5][(three >> 16) & 0xff] ^
				pcrc_32_tab[ 6][(three >> 8)  & 0xff] ^
				pcrc_32_tab[ 7][ three        & 0xff] ^
				pcrc_32_tab[ 8][(two >> 24)   & 0xff] ^
				pcrc_32_tab[ 9][(two >> 16)   & 0xff] ^
				pcrc_32_tab[10][(two >> 8)    & 0xff] ^
				pcrc_32_tab[11][ two          & 0xff] ^
				pcrc_32_tab[12][(one >> 24)   & 0xff] ^
				pcrc_32_tab[13][(one >> 16)   & 0xff] ^
				pcrc_32_tab[14][(one >> 8)    & 0xff] ^
				pcrc_32_tab[15][ one          & 0xff];
		}
	}
	unsigned char* c = reinterpret_cast<unsigned char*>(current);
	for ( ; len; --len ) {
		crc = (crc >> 8) ^ pcrc_32_tab[0][(crc & 0xff) ^ *c++];
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
    uint16_t temp = keys[2] | 2;
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
