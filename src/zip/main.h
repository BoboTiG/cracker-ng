
/*!
 * \file main.h
 * \brief ZIP Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.09
 * 
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#ifndef MAIN_H
#define MAIN_H

#define MODULE  "ZIP"     //!< Module name.
#define VERSION "0.1a-7"  //!< Module version.
#define PWD_MAX  80       //!< Maximum password length

// For inflate (decompression)
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include "../shared/functions.h"
#include "crypt.h"
#include "read.h"


class Cracker {

public:
	ifstream &filei;
	
	Cracker(ifstream &);
	~Cracker();
	unsigned int check();
	void crack();

protected:

private:
	size_t start_byte, end_byte;
	bool strong_encryption;
	char _pad[7];
	local_file_header_light lfh;
	central_directory cd;
	end_central_directory ecd;
	
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
		unsigned int     len
	) {
		uint32_t c = 0xffffffffL;
		
		#if 0
			cout << "\n--- Create CRC-32" << endl;
			cout
				<< "     c = " << (int*)c
				<< " | buf = " << (int*)(*buf & 0xFF)
				<< " | len = " << len
			<< endl;
		#endif
		while ( len >= 8 ) {
			DO8(c, buf);
			len -= 8;
			#if 0
				cout
					<< "do08 c = " << (int*)c
					<< " | buf = " << (int*)(*buf & 0xFF)
					<< " | len = " << len
				<< endl;
			#endif
		}
		if ( len ) do {
			DO1(c, buf);
			#if 0
				cout
					<< "do01 c = " << (int*)c
					<< " | buf = " << (int*)(*buf & 0xFF)
					<< " | len = " << len
				<< endl;
			#endif
		} while ( --len );
		c ^= 0xffffffffL;
		#if 0
			cout << "     c = " << (int*)c << endl;
		#endif
		return c == *crc_32;
	}

	/*!
	 * \fn check_headers()
	 * \brief Check first 4 bytes to verify file is a valid ZIP.
	 * \return \li 0 if \b not a ZIP file;
	 * \return \li 1 otherwise.
	 */
	unsigned int check_headers();

	/*!
	 * \fn unsigned int check_method()
	 * \brief Check if the compression method is implemented.
	 * \return \li 0 if \b not implemented;
	 * \return \li 1 otherwise.
	 */
	unsigned int check_method();
	
	/*!
	 * \fn determine_chosen_one()
	 * \brief Determine the best file to try the crack process against.
	 */
	void determine_chosen_one();

	/*!
	 * \fn find_central_directory()
	 * \brief Try to find the Central Directory Headers start and end.
	 * \return \li 0 if signatures \b not found;
	 * \return \li 1 otherwise.
	 * 
	 * Signatures: start = 0x02014b50, end = 0x06054b50.
	 * 
	 */
	unsigned int find_central_directory();

	void init_lfh();

};

#endif // MAIN_H
