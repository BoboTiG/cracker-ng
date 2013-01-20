
/*!
 * \file read.cc
 * \brief Cracker-ng (optimized) functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.20
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./read.h"


void swap_lfh(
	const struct local_file_header& local_lfh,
	struct local_file_header_light* lfh
) {
	lfh->good_crc_32               = local_lfh.good_crc_32;
	lfh->version_needed_to_extract = local_lfh.version_needed_to_extract;
	lfh->compression_method        = local_lfh.compression_method;
	lfh->start_byte                = local_lfh.start_byte;
	lfh->good_length               = local_lfh.good_length;
	lfh->uncompressed_size         = local_lfh.uncompressed_size;
	lfh->last_mod_file_time        = local_lfh.last_mod_file_time;
	lfh->strong_encryption         = local_lfh.strong_encryption;
	lfh->is_encrypted              = local_lfh.is_encrypted;
	if ( local_lfh.file_name_length > 0 ) {
		strncpy(lfh->file_name, local_lfh.file_name, 512);
	}
}


namespace read_ng {
void read_central_directory(
	std::ifstream&            filei,
	struct central_directory* cd,
	const unsigned int        start_byte,
	const bool                debug
) {
	filei.seekg(start_byte, std::ios::beg);
	filei.read(reinterpret_cast<char*>(&cd->header_signature),            4);
	filei.read(reinterpret_cast<char*>(&cd->version_made_by),             2);
	filei.read(reinterpret_cast<char*>(&cd->version_needed_to_extract),   2);
	filei.read(reinterpret_cast<char*>(&cd->general_purpose_bit_flag),    2);
	filei.read(reinterpret_cast<char*>(&cd->compression_method),          2);
	filei.read(reinterpret_cast<char*>(&cd->last_mod_file_time),          2);
	filei.read(reinterpret_cast<char*>(&cd->last_mod_file_date),          2);
	filei.read(reinterpret_cast<char*>(&cd->crc_32),                      4);
	filei.read(reinterpret_cast<char*>(&cd->compressed_size),             4);
	filei.read(reinterpret_cast<char*>(&cd->uncompressed_size),           4);
	filei.read(reinterpret_cast<char*>(&cd->file_name_length),            2);
	filei.read(reinterpret_cast<char*>(&cd->extra_field_length),          2);
	filei.read(reinterpret_cast<char*>(&cd->file_comment_length),         2);
	filei.read(reinterpret_cast<char*>(&cd->disk_number_part),            2);
	filei.read(reinterpret_cast<char*>(&cd->internal_file_attributes),    2);
	filei.read(reinterpret_cast<char*>(&cd->external_file_attributes),    4);
	filei.read(reinterpret_cast<char*>(&cd->relative_offset_of_local_fh), 4);
	if ( cd->file_name_length > 0 ) {
		cd->file_name = new char[cd->file_name_length + 1];
		filei.read(cd->file_name, cd->file_name_length);
		cd->file_name[cd->file_name_length] = '\0';
	}
	if ( cd->extra_field_length > 0 ) {
		cd->extra_field = new char[cd->extra_field_length];
		filei.read(cd->extra_field, cd->extra_field_length);
	}
	if ( cd->file_comment_length > 0 ) {
		cd->file_comment = new char[cd->file_comment_length + 1];
		filei.read(cd->file_comment, cd->file_comment_length);
		cd->file_comment[cd->file_comment_length] = '\0';
	}
	// Check encryption
	cd->is_encrypted = cd->strong_encryption = false;
	if ( cd->general_purpose_bit_flag & 1 ) {
		cd->is_encrypted = true;
	}
	if ( cd->general_purpose_bit_flag & (1 << 6) ) {
		cd->strong_encryption = true;
	}

	if ( debug ) {
		std::cout << std::endl;
		std::cout << "--- Central Directory" << std::endl;
		std::cout << "header_signature ...........= " << reinterpret_cast<unsigned int*>(cd->header_signature) << std::endl;
		std::cout << "version_made_by ............= " << cd->version_made_by << std::endl;
		std::cout << "version_needed_to_extract ..= " << cd->version_needed_to_extract << std::endl;
		std::cout << "general_purpose_bit_flag ...= " << cd->general_purpose_bit_flag << std::endl;
		std::cout << "compression_method .........= " << cd->compression_method << std::endl;
		std::cout << "last_mod_file_time .........= " << cd->last_mod_file_time << std::endl;
		std::cout << "last_mod_file_date .........= " << cd->last_mod_file_date << std::endl;
		std::cout << "crc_32 .....................= " << reinterpret_cast<unsigned int*>(cd->crc_32) << std::endl;
		std::cout << "compressed_size ............= " << cd->compressed_size << std::endl;
		std::cout << "uncompressed_size ..........= " << cd->uncompressed_size << std::endl;
		std::cout << "file_name_length ...........= " << cd->file_name_length << std::endl;
		std::cout << "extra_field_length .........= " << cd->extra_field_length << std::endl;
		std::cout << "file_comment_length ........= " << cd->file_comment_length << std::endl;
		std::cout << "disk_number_part ...........= " << cd->disk_number_part << std::endl;
		std::cout << "internal_file_attributes ...= " << reinterpret_cast<unsigned int*>(cd->internal_file_attributes) << std::endl;
		std::cout << "external_file_attributes ...= " << reinterpret_cast<unsigned int*>(cd->external_file_attributes) << std::endl;
		std::cout << "relative_offset_of_local_hd = " << reinterpret_cast<unsigned int*>(cd->relative_offset_of_local_fh) << std::endl;
		if ( cd->file_name_length > 0 ) {
			std::cout << "file_name ..................= " << cd->file_name << std::endl;
		}
		if ( cd->extra_field_length > 0 ) {
			std::cout << "extra_field ................= " << cd->extra_field << std::endl;
		}
		if ( cd->file_comment_length > 0 ) {
			std::cout << "file_comment ...............= " << cd->file_comment << std::endl;
		}
	}

	if ( cd->file_name_length > 0 ) {
		delete[] cd->file_name;
	}
	if ( cd->extra_field_length > 0 ) {
		delete[] cd->extra_field;
	}
	if ( cd->file_comment_length > 0 ) {
		delete[] cd->file_comment;
	}
}

void read_end_central_directory(
	std::ifstream&                filei,
	struct end_central_directory* ecd,
	const unsigned int            start_byte,
	const bool                    debug
) {
	filei.seekg(start_byte, std::ios::beg);
	filei.read(reinterpret_cast<char*>(&ecd->header_signature),            4);
	filei.read(reinterpret_cast<char*>(&ecd->number_of_disk),              2);
	filei.read(reinterpret_cast<char*>(&ecd->number_of_disk_with_cd),      2);
	filei.read(reinterpret_cast<char*>(&ecd->cd_on_this_disk),             2);
	filei.read(reinterpret_cast<char*>(&ecd->total_entries),               2);
	filei.read(reinterpret_cast<char*>(&ecd->cd_size),                     4);
	filei.read(reinterpret_cast<char*>(&ecd->offset),                      4);
	filei.read(reinterpret_cast<char*>(&ecd->zip_file_comment_length),     2);
	if ( ecd->zip_file_comment_length > 0 ) {
		ecd->zip_file_comment = new char[ecd->zip_file_comment_length + 1];
		filei.read(ecd->zip_file_comment, ecd->zip_file_comment_length);
		ecd->zip_file_comment[ecd->zip_file_comment_length] = '\0';
	}

	if ( debug ) {
		std::cout << std::endl;
		std::cout << "--- End of Central Directory" << std::endl;
		std::cout << "header_signature ...........= " << reinterpret_cast<unsigned int*>(ecd->header_signature) << std::endl;
		std::cout << "number_of_disk .............= " << ecd->number_of_disk << std::endl;
		std::cout << "number_of_disk_with_cd .....= " << ecd->number_of_disk_with_cd << std::endl;
		std::cout << "cd_on_this_disk ............= " << ecd->cd_on_this_disk << std::endl;
		std::cout << "total_entries ..............= " << ecd->total_entries << std::endl;
		std::cout << "cd_size ....................= " << ecd->cd_size << std::endl;
		std::cout << "offset .....................= " << ecd->offset << std::endl;
		std::cout << "zip_file_comment_length ....= " << ecd->zip_file_comment_length << std::endl;
		if ( ecd->zip_file_comment_length > 0 ) {
			std::cout << "zip_file_comment ...........= " << ecd->zip_file_comment << std::endl;
		}
	}
	
	if ( ecd->zip_file_comment_length > 0 ) {
		delete[] ecd->zip_file_comment;
	}
}

void read_local_file_header(
	std::ifstream&                  filei,
	struct local_file_header_light* lfh,
	const bool                      several,
	const bool                      debug
) {
	local_file_header local_lfh;

	filei.read(reinterpret_cast<char*>(&local_lfh.header_signature),            4);
	/*
		int n = 0;
		do {
			++n;
			filei.read(reinterpret_cast<char*>(&local_lfh.header_signature), 4);
		} while ( local_lfh.header_signature != 0x04034b50 && filei.seekg(-7, std::ios::cur) );
		if ( n > 1 ) std::cout << "BAD BAD BADASS!!! -> décalage n = " << (n*-3) << std::endl;
	*/
	filei.read(reinterpret_cast<char*>(&local_lfh.version_needed_to_extract),   2);
	filei.read(reinterpret_cast<char*>(&local_lfh.general_purpose_bit_flag),    2);
	filei.read(reinterpret_cast<char*>(&local_lfh.compression_method),          2);
	filei.read(reinterpret_cast<char*>(&local_lfh.last_mod_file_time),          2);
	filei.read(reinterpret_cast<char*>(&local_lfh.last_mod_file_date),          2);
	filei.read(reinterpret_cast<char*>(&local_lfh.crc_32),                      4);
	filei.read(reinterpret_cast<char*>(&local_lfh.compressed_size),             4);
	filei.read(reinterpret_cast<char*>(&local_lfh.uncompressed_size),           4);
	filei.read(reinterpret_cast<char*>(&local_lfh.file_name_length),            2);
	filei.read(reinterpret_cast<char*>(&local_lfh.extra_field_length),          2);
	if ( local_lfh.file_name_length > 0 ) {
		local_lfh.file_name = new char[local_lfh.file_name_length + 1];
		filei.read(local_lfh.file_name, local_lfh.file_name_length);
		local_lfh.file_name[local_lfh.file_name_length] = '\0';
	}
	if ( local_lfh.extra_field_length > 0 ) {
		local_lfh.extra_field = new char[local_lfh.extra_field_length + 1];
		filei.read(local_lfh.extra_field, local_lfh.extra_field_length);
		local_lfh.extra_field[local_lfh.extra_field_length] = '\0';
	}

	// Save the compressed data start byte
	local_lfh.start_byte = filei.tellg();

	// Data Descriptor
	local_lfh.has_data_descriptor         = false;
	local_lfh.data_desc_crc_32            = 0;
	local_lfh.data_desc_compressed_size   = 0;
	local_lfh.data_desc_uncompressed_size = 0;
	filei.seekg(local_lfh.compressed_size, std::ios::cur);
	if ( local_lfh.general_purpose_bit_flag & (1 << 3) ) {
		local_lfh.has_data_descriptor = true;
		filei.read(reinterpret_cast<char*>(&local_lfh.data_desc_signature),         4);
		// Data Description signature could be omitted, so take care of that!
		if ( local_lfh.data_desc_signature != 0x8074b50 ) {
			local_lfh.data_desc_crc_32 = local_lfh.data_desc_signature;
			local_lfh.data_desc_signature = 0;
		} else {
			filei.read(reinterpret_cast<char*>(&local_lfh.data_desc_crc_32),        4);
		}
		filei.read(reinterpret_cast<char*>(&local_lfh.data_desc_compressed_size),   4);
		filei.read(reinterpret_cast<char*>(&local_lfh.data_desc_uncompressed_size), 4);
	}

	// Data length
	local_lfh.good_length =
	(
		local_lfh.uncompressed_size > 0 ?
			local_lfh.uncompressed_size : local_lfh.data_desc_uncompressed_size
	);

	// CRC-32
	local_lfh.good_crc_32 =
	(
		local_lfh.crc_32 > 0 ?
			local_lfh.crc_32 :
			(
				local_lfh.data_desc_crc_32 > 0 ? local_lfh.data_desc_crc_32 :	0
			)
	);

	// Check encryption
	local_lfh.is_encrypted      = false;
	local_lfh.strong_encryption = false;
	if ( local_lfh.general_purpose_bit_flag & 1 ) {
		local_lfh.is_encrypted = true;
		if ( local_lfh.general_purpose_bit_flag & (1 << 6) ) {
			local_lfh.strong_encryption = true;
		}
	}

	if ( debug ) {
		std::cout << std::endl;
		std::cout << "--- Local File Header" << std::endl;
		std::cout << "header_signature ...........= " << reinterpret_cast<unsigned int*>(local_lfh.header_signature) << std::endl;
		std::cout << "version_needed_to_extract ..= " << local_lfh.version_needed_to_extract << std::endl;
		std::cout << "general_purpose_bit_flag ...= " << local_lfh.general_purpose_bit_flag << std::endl;
		std::cout << "compression_method .........= " << local_lfh.compression_method << std::endl;
		std::cout << "last_mod_file_time .........= " << local_lfh.last_mod_file_time << std::endl;
		std::cout << "last_mod_file_date .........= " << local_lfh.last_mod_file_date << std::endl;
		std::cout << "crc_32 .....................= " << reinterpret_cast<unsigned int*>(local_lfh.crc_32) << std::endl;
		std::cout << "compressed_size ............= " << local_lfh.compressed_size << std::endl;
		std::cout << "uncompressed_size ..........= " << local_lfh.uncompressed_size << std::endl;
		std::cout << "file_name_length ...........= " << local_lfh.file_name_length << std::endl;
		std::cout << "extra_field_length .........= " << local_lfh.extra_field_length << std::endl;
		if ( local_lfh.file_name_length > 0 ) {
			std::cout << "file_name ..................= " << local_lfh.file_name << std::endl;
		}
		if ( local_lfh.extra_field_length > 0 ) {
			std::cout << "extra_field ................= " << reinterpret_cast<unsigned int*>(local_lfh.extra_field) << std::endl;
		}
		std::cout << "---" << std::endl;
		std::cout << "has_data_descriptor ........= " << local_lfh.has_data_descriptor << std::endl;
		if ( local_lfh.has_data_descriptor ) {
			std::cout << "data_desc_signature ........= " << reinterpret_cast<unsigned int*>(local_lfh.data_desc_signature) << std::endl;
			std::cout << "data_desc_crc_32 ...........= " << reinterpret_cast<unsigned int*>(local_lfh.data_desc_crc_32) << std::endl;
			std::cout << "data_desc_compressed_size ..= " << local_lfh.data_desc_compressed_size << std::endl;
			std::cout << "data_desc_uncompressed_size = " << local_lfh.data_desc_uncompressed_size << std::endl;
		}
		std::cout << "---" << std::endl;
		std::cout << "start_byte .................= " << local_lfh.start_byte << std::endl;
		std::cout << "is_encrypted ...............= " << local_lfh.is_encrypted << std::endl;
		std::cout << "strong_encryption ..........= " << local_lfh.strong_encryption << std::endl;
		std::cout << "good_crc_32 ................= " << reinterpret_cast<unsigned int*>(local_lfh.good_crc_32) << std::endl;
		std::cout << "good_length ................= " << local_lfh.good_length << std::endl;
		std::cout << std::endl;
	}

	// Now, let's decide if we need this entry o_0
	if ( several ) {
		// Check for folders: uncompressed size is 0 byte
		if ( local_lfh.uncompressed_size > 0 ) {
			// First, check file length: smaller bytes => faster crack process
			if ( local_lfh.good_length <= lfh->good_length ) {
				// Then, chek compression method: smaller number => simpler method => should be faster
				if ( local_lfh.compression_method <= lfh->compression_method ) {
					swap_lfh(local_lfh, lfh);
				}
			} else if ( local_lfh.compression_method < lfh->compression_method ) {
				swap_lfh(local_lfh, lfh);
			}
		}
	} else {
		// There is only one file, so we choose this entry
		swap_lfh(local_lfh, lfh);
	}

	if ( local_lfh.file_name_length > 0 ) {
		delete[] local_lfh.file_name;
	}
	if ( local_lfh.extra_field_length > 0 ) {
		delete[] local_lfh.extra_field;
	}
}
}
