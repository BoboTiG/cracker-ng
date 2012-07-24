
/*!
 * \file crypt.hpp
 * \brief ZIP Cracker-ng headers for the Traditional PKWARE Encryption.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2011.06.15
 * 
 * Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.
 * 
 * See the accompanying file LICENSE, version 2005-Feb-10 or later for 
 * terms of use. If, for some reason, all these files are missing, the
 * Info-ZIP license also may be found at: 
 * ftp://ftp.info-zip.org/pub/infozip/license.html
 */


// Defines -------------------------------------------------------------
/*!
 * \def zdecode(c)
 * \brief Decode byte c in place.
 * \param c Byte de decode.
 * \return Decoded byte.
 */
#define zdecode(c) update_keys(c ^= decrypt_byte())


// Prototypes ----------------------------------------------------------
/*!
 * \fn decrypt_byte(void)
 * \brief Return the next byte in the pseudo-random sequence.
 * \return Decrypted byte.
 * \bug POTENTIAL:  temp*(temp^1) may overflow in an unpredictable 
 * manner on 16-bit systems; not a problem with any known compiler 
 * so far, though.
 */
inline unsigned char decrypt_byte(void);


/*!
 * \fn init_keys(const char* passwd)
 * \brief Initialize the encryption keys and the random header according to the given password.
 * \param passwd The password.
 */
inline void init_keys(const char* passwd);


/*!
 * \fn update_keys(int c)
 * \brief Update the encryption keys with the next byte of plain text.
 * \param c The next byte.
 */
inline void update_keys(int c);


// Variables -----------------------------------------------------------
/*!
 * \var keys
 * \brief Three 32-bit keys for CRC-32 calculus.
 */
unsigned long keys[3];
