
/*!
 * \file read.h
 * \brief Cracker-ng (optimized) functions headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.07
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_ZIP_READ_H_
#define SRC_ZIP_READ_H_

#include <stdint.h>
#include <cstring>
#include <fstream>
#include <iostream>

/*!
 * \struct central_directory
 * \brief ZIP Central Directory structure.
 */
struct central_directory {
	uint32_t header_signature;             //!< Central file header signature on 4 bytes (0x02014b50)
	uint32_t crc_32;                       //!< CRC-32 on 4 bytes
	uint32_t compressed_size;              //!< Compressed size on 4 bytes
	uint32_t uncompressed_size;            //!< Uncompressed size on 4 bytes
	uint32_t relative_offset_of_local_fh;  //!< Relative offset of local header on 4 bytes
	uint32_t external_file_attributes;     //!< External file attributes on 4 bytes
	uint16_t version_made_by;              //!< Version made by on 2 bytes
	uint16_t version_needed_to_extract;    //!< Version needed to extract on 2 bytes
	uint16_t general_purpose_bit_flag;     //!< General purpose bit flag on 2 bytes
	uint16_t compression_method;           //!< Compression method on 2 bytes
	uint16_t last_mod_file_time;           //!< Last mod file time on 2 bytes
	uint16_t last_mod_file_date;           //!< Last mod file date on 2 bytes
	uint16_t file_name_length;             //!< File name length on 2 bytes
	uint16_t extra_field_length;           //!< Extra field length on 2 bytes
	uint16_t file_comment_length;          //!< File comment length on 2 bytes
	uint16_t disk_number_part;             //!< Disk number start on 2 bytes
	uint16_t internal_file_attributes;     //!< Internal file attributes on 2 bytes
	uint16_t extra_field_id;               //!< Extra field Header ID on 2 bytes
	unsigned int is_encrypted;             //!< Encryption enable? (unsigned int)
	unsigned int strong_encryption;        //!< Use strong encryption? (unsigned int)
	char * file_name;                      //!< File name (variable size)
	char * extra_field;                    //!< Extra field (variable size)
	char * file_comment;                   //!< File comment (variable size)
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
	uint16_t _pad;                        //!< Padding to feet the good alignment
	char * zip_file_comment;              //!< ZIP file comment (variable size)
};

/*!
 * \struct local_file_header
 * \brief ZIP Local File header structure.
 */
struct local_file_header {
	uint32_t header_signature;             //!< Local file header signature on 4 bytes (0x04034b50)
	uint32_t crc_32;                       //!< CRC-32 on 4 bytes
	uint32_t compressed_size;              //!< Compressed size on 4 bytes
	uint32_t uncompressed_size;            //!< Uncompressed size on 4 bytes
	uint32_t data_desc_signature;          //!< Data Descriptor signature (not always used, but should) on 4 bytes (0x08074b50)
	uint32_t data_desc_crc_32;             //!< CRC-32 on 4 bytes
	uint32_t data_desc_compressed_size;    //!< Compressed size on 4 bytes
	uint32_t data_desc_uncompressed_size;  //!< Uncompressed size on 4 bytes
	uint32_t good_crc_32;                  //!< If crc_32 is 0, then check data_desc_crc_32 on 4 bytes
	uint16_t version_needed_to_extract;    //!< Version needed to extract on 2 bytes
	uint16_t general_purpose_bit_flag;     //!< General purpose bit flag on 2 bytes
	uint16_t compression_method;           //!< compression method on 2 bytes
	uint16_t last_mod_file_time;           //!< Last mod file time on 2 bytes
	uint16_t last_mod_file_date;           //!< Last mod file date on 2 bytes
	uint16_t file_name_length;             //!< File name length on 2 bytes
	uint16_t extra_field_length;           //!< Extra field length on 2 bytes
	bool has_data_descriptor;              //!< Data descriptor presents? (bool)
	bool is_encrypted;                     //!< Encryption enabled? (bool)
	unsigned int start_byte;               //!< Start byte of compressed data (unsigned int)
	unsigned int good_length;              //!< If uncompressed_size is 0, then check data_desc_uncompressed_size (unsigned int)
	unsigned int strong_encryption;        //!< Use strong encryption? (bool)
	char * file_name;                      //!< File name (variable size)
	char * extra_field;                    //!< Extra field (variable size)
};

/*!
 * \struct local_file_header_light
 * \brief Light ZIP Local File header structure - we take only what we need.
 */
struct local_file_header_light {
	uint32_t good_crc_32;                 //!< If crc_32 is 0, then check data_desc_crc_32 on 4 bytes
	uint16_t version_needed_to_extract;   //!< Version needed to extract on 2 bytes
	uint16_t compression_method;          //!< compression method on 2 bytes
	uint32_t start_byte;                  //!< Start byte of compressed data on 4 bytes
	uint32_t good_length;                 //!< If uncompressed_size is 0, then check data_desc_uncompressed_size on 4 bytes
	uint32_t uncompressed_size;           //!< Uncompressed size, we need it to reserve the good space to prevent segfault on few files on 4 bytes
	uint16_t last_mod_file_time;          //!< Last mod file time on 2 bytes
	bool strong_encryption;               //!< Use strong encryption? (bool)
	bool is_encrypted;                    //!< Encryption enabled? (bool)
	char file_name[512];                  //!< File name (custom fixed size)
};


void swap_lfh(const struct local_file_header&, struct local_file_header_light*);


namespace read_ng {

/*!
 * \fn read_central_directory(ifstream & zip, struct central_directory * cd, unsigned int start_byte)
 * \brief Read Central Directory.
 * \param zip Reference to the ZIP handler.
 * \param cd Poiter to the struct where to stock informations.
 * \param start_byte Byte where starts the Central Directory.
 * \param debug Print debug informations (for -i | --nfos option)?
 */
void read_central_directory(std::ifstream&, struct central_directory*, const unsigned int, const bool);

/*
 * \fn read_end_central_directory(ifstream & zip, struct end_central_directory * ecd, unsigned int start_byte)
 * \brief Read end of Central Directory.
 * \param zip Reference to the ZIP handler.
 * \param ecd Poiter to the struct where to stock informations.
 * \param start_byte Byte where starts the Central Directory.
 * \param debug Print debug informations (for -i | --nfos option)?
 */
void read_end_central_directory(std::ifstream&, struct end_central_directory*, const unsigned int, const bool);

/*!
 * \fn local_file_header(ifstream & zip, struct local_file_header * lfh, unsigned int start_byte, bool several)
 * \brief Read Local File header.
 * \param zip Reference to the ZIP handler.
 * \param lfh Poiter to the struct where to stock informations.
 * \param several Is there more than one file in the ZIP file?
 * \param debug Print debug informations (for -i | --nfos option)?
 */
void read_local_file_header(std::ifstream&, struct local_file_header_light*, const bool, const bool);

}

#endif  // SRC_ZIP_READ_H_
