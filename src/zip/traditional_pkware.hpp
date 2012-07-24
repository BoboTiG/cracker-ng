
/*!
 * \file traditional_pkware.hpp
 * \brief Traditional PKWARE encryption headers - ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 * 
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


/*!
 * \fn static inline create_crc32(uint8_t * buf, const uint32_t & crc_32, uint32_t len)
 * \brief Calculate the CRC-32 of the decrypted data.
 * \param but Pointer to the decrypted data.
 * \param crc_32 CRC-32 from the Local File header.
 * \param len Length of the decrypted data.
 * \return \li 0 if CRC-32 are \b not equals;
 * \return \li 1 otherwise.
 */
static inline unsigned int create_crc32(
	char           * buf, 
	const uint32_t * crc_32, 
	unsigned int     len);

/*!
 * \fn traditional_pkware_encryption(ifstream& zip, struct local_file_header * lfh)
 * \brief Traditional PKWARE Encryption brute force attack.
 * \param zip Reference to the ZIP handler.
 * \param lfh Poiter to the struct where to stock informations.
 * \todo Voir pourquoi faire -2 lors du test_password().
 * 
\section title Traditional PKWARE Encryption

The following information discusses the decryption steps
required to support traditional PKWARE encryption.  This
form of encryption is considered weak by today's standards
and its use is recommended only for situations with
low security needs or for compatibility with older .ZIP 
applications.

\subsection desc Decryption

PKWARE is grateful to Mr. Roger Schlafly for his expert contribution 
towards the development of PKWARE's traditional encryption.

PKZIP encrypts the compressed data stream.  Encrypted files must
be decrypted before they can be extracted.

Each encrypted file has an extra 12 bytes stored at the start of
the data area defining the encryption header for that file.  The
encryption header is originally set to random values, and then
itself encrypted, using three, 32-bit keys.  The key values are
initialized using the supplied encryption password.  After each byte
is encrypted, the keys are then updated using pseudo-random number
generation techniques in combination with the same CRC-32 algorithm
used in PKZIP and described elsewhere in this document.

The following is the basic steps required to decrypt a file:

\li 1) Initialize the three 32-bit keys with the password.
\li 2) Read and decrypt the 12-byte encryption header, further
   initializing the encryption keys.
\li 3) Read and decrypt the compressed data stream using the
   encryption keys.

\subsection step1 Step 1 - Initializing the encryption keys

\code
Key(0) <- 305419896
Key(1) <- 591751049
Key(2) <- 878082192

loop for i <- 0 to length(password)-1
    update_keys(password(i))
end loop
\endcode

Where update_keys() is defined as:

\code
update_keys(char):
  Key(0) <- crc32(key(0),char)
  Key(1) <- Key(1) + (Key(0) & 000000ffH)
  Key(1) <- Key(1) * 134775813 + 1
  Key(2) <- crc32(key(2),key(1) >> 24)
end update_keys
\endcode

Where crc32(old_crc,char) is a routine that given a CRC value and a
character, returns an updated CRC value after applying the CRC-32
algorithm described elsewhere in this document.

\subsection step2 Step 2 - Decrypting the encryption header

The purpose of this step is to further initialize the encryption
keys, based on random data, to render a plaintext attack on the
data ineffective.

Read the 12-byte encryption header into Buffer, in locations
Buffer(0) thru Buffer(11).

\code
loop for i <- 0 to 11
    C <- buffer(i) ^ decrypt_byte()
    update_keys(C)
    buffer(i) <- C
end loop
\endcode

Where decrypt_byte() is defined as:

\code
unsigned char decrypt_byte()
    local unsigned short temp
    temp <- Key(2) | 2
    decrypt_byte <- (temp * (temp ^ 1)) >> 8
end decrypt_byte
\endcode

After the header is decrypted,  the last 1 or 2 bytes in Buffer
should be the high-order word/byte of the CRC for the file being
decrypted, stored in Intel low-byte/high-byte order.  Versions of
PKZIP prior to 2.0 used a 2 byte CRC check; a 1 byte CRC check is
used on versions after 2.0.  This can be used to test if the password
supplied is correct or not.

\subsection step3 Step 3 - Decrypting the compressed data stream

The compressed data stream can be decrypted as follows:

\code
loop until done
    read a character into C
    Temp <- C ^ decrypt_byte()
    update_keys(temp)
    output Temp
end loop
\endcode
 */
int traditional_pkware(ifstream & zip, struct local_file_header_light * lfh);
