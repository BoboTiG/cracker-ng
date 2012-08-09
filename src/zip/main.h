
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

#include "../shared/functions-ng.h"
#include "crypt.h"


class Cracker {

public:
	ifstream &filei;
	
	Cracker(ifstream &);
	~Cracker();
	unsigned int check();
	void crack();

protected:

private:
	/*!
	 * \struct central_directory
	 * \brief ZIP Central Directory structure.
	 */
	struct central_directory {
		uint32_t header_signature;            //!< Central file header signature on 4 bytes (0x02014b50)
		uint32_t crc_32;                      //!< CRC-32 on 4 bytes
		uint32_t compressed_size;             //!< Compressed size on 4 bytes
		uint32_t uncompressed_size;           //!< Uncompressed size on 4 bytes
		uint32_t relative_offset_of_local_fh; //!< Relative offset of local header on 4 bytes
		uint32_t external_file_attributes;    //!< External file attributes on 4 bytes
		uint16_t version_made_by;             //!< Version made by on 2 bytes
		uint16_t version_needed_to_extract;   //!< Version needed to extract on 2 bytes
		uint16_t general_purpose_bit_flag;    //!< General purpose bit flag on 2 bytes
		uint16_t compression_method;          //!< Compression method on 2 bytes
		uint16_t last_mod_file_time;          //!< Last mod file time on 2 bytes
		uint16_t last_mod_file_date;          //!< Last mod file date on 2 bytes
		uint16_t file_name_length;            //!< File name length on 2 bytes
		uint16_t extra_field_length;          //!< Extra field length on 2 bytes
		uint16_t file_comment_length;         //!< File comment length on 2 bytes
		uint16_t disk_number_part;            //!< Disk number start on 2 bytes
		uint16_t internal_file_attributes;    //!< Internal file attributes on 2 bytes
		uint16_t extra_field_id;              //!< Extra field Header ID on 2 bytes
		unsigned int is_encrypted;            //!< Encryption enable? (unsigned int)
		unsigned int strong_encryption;       //!< Use strong encryption? (unsigned int)
		char * file_name;                     //!< File name (variable size)
		char * extra_field;                   //!< Extra field (variable size)
		char * file_comment;                  //!< File comment (variable size)
	};

	/*!
	 * \struct end_central_directory
	 * \brief ZIP end of Central Directory structure.
	 */
	struct end_central_directory {
		uint32_t header_signature;            //!< End of central dir signature on 4 bytes (0x06054b50)
		uint32_t cd_size;                     //!< Size of the central directory on 4 bytes
		uint32_t offset;                      //!< Offset of start of central directory with respect to the starting disk number on 4 bytes
		uint16_t number_of_disk;              //!< Number of this disk on 2 bytes
		uint16_t number_of_disk_with_cd;      //!< Number of the disk with thet start of the central directory on 2 bytes
		uint16_t cd_on_this_disk;             //!< Total number of entries in the central directory on this disk on 2 bytes
		uint16_t total_entries;               //!< Total number of entries in the central directory on 2 bytes
		uint16_t zip_file_comment_length;     //!< ZIP file comment length on 2 bytes
		char pad[2];                          //!< Padding to feet the good alignment
		char * zip_file_comment;              //!< ZIP file comment (variable size)
	};

	/*!
	 * \struct local_file_header
	 * \brief ZIP Local File header structure.
	 */
	struct local_file_header {
		uint32_t header_signature;            //!< Local file header signature on 4 bytes (0x04034b50)
		uint32_t crc_32;                      //!< CRC-32 on 4 bytes
		uint32_t compressed_size;             //!< Compressed size on 4 bytes
		uint32_t uncompressed_size;           //!< Uncompressed size on 4 bytes
		uint32_t data_desc_signature;         //!< Data Descriptor signature (not always used, but should) on 4 bytes (0x08074b50)
		uint32_t data_desc_crc_32;            //!< CRC-32 on 4 bytes
		uint32_t data_desc_compressed_size;   //!< Compressed size on 4 bytes
		uint32_t data_desc_uncompressed_size; //!< Uncompressed size on 4 bytes
		uint32_t good_crc_32;                 //!< If crc_32 is 0, then check data_desc_crc_32 on 4 bytes
		uint16_t version_needed_to_extract;   //!< Version needed to extract on 2 bytes
		uint16_t general_purpose_bit_flag;    //!< General purpose bit flag on 2 bytes
		uint16_t compression_method;          //!< compression method on 2 bytes
		uint16_t last_mod_file_time;          //!< Last mod file time on 2 bytes
		uint16_t last_mod_file_date;          //!< Last mod file date on 2 bytes
		uint16_t file_name_length;            //!< File name length on 2 bytes
		uint16_t extra_field_length;          //!< Extra field length on 2 bytes
		bool has_data_descriptor;             //!< Data descriptor presents? (bool)
		bool is_encrypted;                    //!< Encryption enabled? (bool)
		unsigned int start_byte;              //!< Start byte of compressed data (unsigned int)
		unsigned int good_length;             //!< If uncompressed_size is 0, then check data_desc_uncompressed_size (unsigned int)
		unsigned int strong_encryption;       //!< Use strong encryption? (bool)
		char * file_name;                     //!< File name (variable size)
		char * extra_field;                   //!< Extra field (variable size)
	};

	/*!
	 * \struct local_file_header_light
	 * \brief Light ZIP Local File header structure - we take only what we need.
	 */
	struct local_file_header_light {
		uint32_t good_crc_32;                 //!< If crc_32 is 0, then check data_desc_crc_32 on 4 bytes
		uint16_t version_needed_to_extract;   //!< Version needed to extract on 2 bytes
		uint16_t compression_method;          //!< compression method on 2 bytes
		unsigned int start_byte;              //!< Start byte of compressed data (unsigned int)
		unsigned int good_length;             //!< If uncompressed_size is 0, then check data_desc_uncompressed_size (unsigned int)
		uint16_t last_mod_file_time;          //!< Last mod file time on 2 bytes
		bool strong_encryption;               //!< Use strong encryption? (bool)
		bool is_encrypted;                    //!< Encryption enabled? (bool)
		char pad[4];                          //!< Padding to feet the good alignment
	};
	
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

	/*!
	 * \fn read_central_directory()
	 * \brief Read Central Directory.
	 */
	void read_central_directory();

	/*!
	 * \fn read_end_central_directory()
	 * \brief Read end of Central Directory.
	 */
	void read_end_central_directory();

	/*!
	 * \fn read_local_file_header()
	 * \brief Read Local File header.
	 */
	void read_local_file_header(bool);

	/*!
	 * \fn read_zip64_end_of_cdr()
	 * \brief ZIP64 end of central directory.
	 */
	void read_zip64_end_of_cdr();

	void transpose_lfh(struct local_file_header *);

	void init_lfh();

};

#endif // MAIN_H
