
/*!
 * \file zipcracker-ng.hpp
 * \brief ZIP Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 * 
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#define MODULE  "ZIP"     //!< Module name.
#define VERSION "0.1a-6"  //!< Module version.
#define PWD_MAX  80       //!< Maximum password length
#define ZIP

#include "../shared/cracker-ng.cpp"
#include "crc32.hpp"
#include "crypt.cpp"

// For inflate (decompression)
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>


// Prototypes ----------------------------------------------------------
/*!
 * \fn check_headers(ifstream & zip)
 * \brief Check first 4 bytes to verify file is a valid ZIP.
 * \return \li 0 if \b not a ZIP file;
 * \return \li 1 otherwise.
 */
unsigned int check_headers(ifstream & zip);

/*!
 * \fn unsigned int check_method(unsigned int compression_method)
 * \brief Check if the compression method is implemented.
 * \return \li 0 if \b not implemented;
 * \return \li 1 otherwise.
 */
unsigned int check_method(unsigned int compression_method);

/*!
 * \fn determine_chosen_one(ifstream & zip, unsigned int & start_byte, unsigned int & good_length)
 * \brief Determine the best file to try the crack process against.
 */
void determine_chosen_one(
	ifstream                       & zip,
	struct local_file_header_light * lfh,
	unsigned int                     nb_entries);

/*!
 * \fn find_central_directory(ifstream & zip, unsigned int & start_byte, unsigned int & end_byte)
 * \brief Try to find the Central Directory Headers start and end.
 * \return \li 0 if signatures \b not found;
 * \return \li 1 otherwise.
 * 
 * Signatures: start = 0x02014b50, end = 0x06054b50.
 * 
 */
unsigned int find_central_directory(
	ifstream     & zip, 
	unsigned int & start_byte, 
	unsigned int & end_byte);

/*!
 * \fn read_central_directory(ifstream & zip, struct central_directory * cd, unsigned int start_byte)
 * \brief Read Central Directory.
 * \param zip Reference to the ZIP handler.
 * \param cd Poiter to the struct where to stock informations.
 * \param start_byte Byte where starts the Central Directory.
 */
void read_central_directory(
	ifstream                 & zip, 
	struct central_directory * cd, 
	unsigned int               start_byte);

/*!
 * \fn read_end_central_directory(ifstream & zip, struct end_central_directory * ecd, unsigned int start_byte)
 * \brief Read end of Central Directory.
 * \param zip Reference to the ZIP handler.
 * \param ecd Poiter to the struct where to stock informations.
 * \param start_byte Byte where starts the Central Directory.
 */
void read_end_central_directory(
	ifstream                     & zip, 
	struct end_central_directory * ecd, 
	unsigned int                   start_byte);

/*!
 * \fn read_local_file_header(ifstream & zip, struct local_file_header * lfh, unsigned int start_byte)
 * \brief Read Local File header.
 * \param zip Reference to the ZIP handler.
 * \param lfh Poiter to the struct where to stock informations.
 * \param start_byte Byte where starts the first Local File Header starts.
 */
void read_local_file_header(
	ifstream                       & zip, 
	struct local_file_header_light * lfh,
	bool                             several);

/*!
 * \fn read_zip64_end_of_cdr(ifstream & zip, struct end_central_directory * zip64, unsigned int start_byte)
 * \brief ZIP64 end of central directory.
 * \param zip Reference to the ZIP handler.
 * \param ecd Poiter to the struct where to stock informations.
 * \param start_byte Byte where starts the Central Directory.
 */
void read_zip64_end_of_cdr(
	ifstream                & zip, 
	struct zip64_end_of_cdr * zip64, 
	unsigned int              start_byte);

void transpose_lfh(
	struct local_file_header * local_lfh,
	struct local_file_header_light * lfh);

void init_lfh(struct local_file_header_light * lfh);

// Structures ----------------------------------------------------------
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
