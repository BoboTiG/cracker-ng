
/*!
 * \file read.cc
 * \brief Cracker-ng (optimized) functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.15
 *
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./read.h"


void transpose_lfh(
	struct local_file_header & local_lfh,
	struct local_file_header_light * lfh
) {
	lfh->good_crc_32               = local_lfh.good_crc_32;
	lfh->version_needed_to_extract = local_lfh.version_needed_to_extract;
	lfh->compression_method        = local_lfh.compression_method;
	lfh->start_byte                = local_lfh.start_byte;
	lfh->good_length               = local_lfh.good_length;
	lfh->last_mod_file_time        = local_lfh.last_mod_file_time;
	lfh->strong_encryption         = local_lfh.strong_encryption;
	lfh->is_encrypted              = local_lfh.is_encrypted;
}


namespace read_ng {
void read_central_directory(
	std::ifstream & filei,
	struct central_directory * cd,
	unsigned int start_byte
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

	#if 0
		cout << endl;
		cout << "--- Central Directory" << endl;
		cout << "header_signature ...........= " << reinterpret_cast<unsigned int*>(cd->header_signature) << endl;
		cout << "version_made_by ............= " << cd->version_made_by << endl;
		cout << "version_needed_to_extract ..= " << cd->version_needed_to_extract << endl;
		cout << "general_purpose_bit_flag ...= " << cd->general_purpose_bit_flag << endl;
		cout << "compression_method .........= " << cd->compression_method << endl;
		cout << "last_mod_file_time .........= " << cd->last_mod_file_time << endl;
		cout << "last_mod_file_date .........= " << cd->last_mod_file_date << endl;
		cout << "crc_32 .....................= " << reinterpret_cast<unsigned int*>(cd->crc_32) << endl;
		cout << "compressed_size ............= " << cd->compressed_size << endl;
		cout << "uncompressed_size ..........= " << cd->uncompressed_size << endl;
		cout << "file_name_length ...........= " << cd->file_name_length << endl;
		cout << "extra_field_length .........= " << cd->extra_field_length << endl;
		cout << "file_comment_length ........= " << cd->file_comment_length << endl;
		cout << "disk_number_part ...........= " << cd->disk_number_part << endl;
		cout << "internal_file_attributes ...= " << reinterpret_cast<unsigned int*>(cd->internal_file_attributes) << endl;
		cout << "external_file_attributes ...= " << reinterpret_cast<unsigned int*>(cd->external_file_attributes) << endl;
		cout << "relative_offset_of_local_hd = " << reinterpret_cast<unsigned int*>(cd->relative_offset_of_local_fh) << endl;
		if ( cd->file_name_length > 0 ) {
			cout << "file_name ..................= " << cd->file_name << endl;
		}
		if ( cd->extra_field_length > 0 ) {
			cout << "extra_field ................= " << cd->extra_field << endl;
		}
		if ( cd->file_comment_length > 0 ) {
			cout << "file_comment ...............= " << cd->file_comment << endl;
		}
	#endif

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
	std::ifstream & filei,
	struct end_central_directory * ecd,
	unsigned int start_byte
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

	#if 0
		cout << endl;
		cout << "--- End of Central Directory" << endl;
		cout << "header_signature ...........= " << reinterpret_cast<unsigned int*>(ecd->header_signature) << endl;
		cout << "number_of_disk .............= " << ecd->number_of_disk << endl;
		cout << "number_of_disk_with_cd .....= " << ecd->number_of_disk_with_cd << endl;
		cout << "cd_on_this_disk ............= " << ecd->cd_on_this_disk << endl;
		cout << "total_entries ..............= " << ecd->total_entries << endl;
		cout << "cd_size ....................= " << ecd->cd_size << endl;
		cout << "offset .....................= " << ecd->offset << endl;
		cout << "zip_file_comment_length ....= " << ecd->zip_file_comment_length << endl;
		if ( ecd->zip_file_comment_length > 0 ) {
			cout << "zip_file_comment ...........= " << ecd->zip_file_comment << endl;
		}
	#endif

	if ( ecd->zip_file_comment_length > 0 ) {
		delete[] ecd->zip_file_comment;
	}
}

void read_local_file_header(
	std::ifstream & filei,
	struct local_file_header_light * lfh,
	bool several
) {
	local_file_header local_lfh;

	filei.read(reinterpret_cast<char*>(&local_lfh.header_signature),            4);
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
	local_lfh.has_data_descriptor = false;
	local_lfh.data_desc_crc_32 = 0;
	if ( local_lfh.general_purpose_bit_flag & (1 << 3) ) {
		local_lfh.has_data_descriptor = true;
	}
	if ( local_lfh.has_data_descriptor ) {
		filei.seekg(local_lfh.compressed_size, std::ios::cur);
		filei.read(reinterpret_cast<char*>(&local_lfh.data_desc_signature), 4);
		if ( local_lfh.data_desc_signature != 0x8074b50 ) {
			local_lfh.data_desc_crc_32 = local_lfh.data_desc_signature;
			local_lfh.data_desc_signature = 0;
		} else {
			filei.read(reinterpret_cast<char*>(&local_lfh.data_desc_crc_32), 4);
		}
		filei.read(reinterpret_cast<char*>(&local_lfh.data_desc_compressed_size), 4);
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
	local_lfh.is_encrypted = false;
	local_lfh.strong_encryption = false;
	if ( local_lfh.general_purpose_bit_flag & 1 ) {
		local_lfh.is_encrypted = true;
	}
	if ( local_lfh.general_purpose_bit_flag & (1 << 6) ) {
		local_lfh.strong_encryption = true;
	}

	#if 0
		cout << endl;
		cout << "--- Local File Header" << endl;
		cout << "header_signature ...........= " << reinterpret_cast<unsigned int*>(local_lfh.header_signature) << endl;
		cout << "version_needed_to_extract ..= " << local_lfh.version_needed_to_extract << endl;
		cout << "general_purpose_bit_flag ...= " << local_lfh.general_purpose_bit_flag << endl;
		cout << "compression_method .........= " << local_lfh.compression_method << endl;
		cout << "last_mod_file_time .........= " << local_lfh.last_mod_file_time << endl;
		cout << "last_mod_file_date .........= " << local_lfh.last_mod_file_date << endl;
		cout << "crc_32 .....................= " << reinterpret_cast<unsigned int*>(local_lfh.crc_32) << endl;
		cout << "compressed_size ............= " << local_lfh.compressed_size << endl;
		cout << "uncompressed_size ..........= " << local_lfh.uncompressed_size << endl;
		cout << "file_name_length ...........= " << local_lfh.file_name_length << endl;
		cout << "extra_field_length .........= " << local_lfh.extra_field_length << endl;
		if ( local_lfh.file_name_length > 0 ) {
			cout << "file_name ..................= " << local_lfh.file_name << endl;
		}
		if ( local_lfh.extra_field_length > 0 ) {
			cout << "extra_field ................= " << reinterpret_cast<unsigned int*>(local_lfh.extra_field) << endl;
		}
		cout << "---" << endl;
		cout << "start_byte .................= " << local_lfh.start_byte << endl;
		cout << "is_encrypted ...............= " << local_lfh.is_encrypted << endl;
		cout << "strong_encryption ..........= " << local_lfh.strong_encryption << endl;
		cout << "has_data_descriptor ........= " << local_lfh.has_data_descriptor << endl;
		if ( local_lfh.has_data_descriptor ) {
			cout << "data_desc_signature ........= " << reinterpret_cast<unsigned int*>(local_lfh.data_desc_signature) << endl;
			cout << "data_desc_crc_32 ...........= " << reinterpret_cast<unsigned int*>(local_lfh.data_desc_crc_32) << endl;
			cout << "data_desc_compressed_size ..= " << local_lfh.data_desc_compressed_size << endl;
			cout << "data_desc_uncompressed_size = " << local_lfh.data_desc_uncompressed_size << endl;
		}
		cout << "good_crc_32 ................= " << reinterpret_cast<unsigned int*>(local_lfh.good_crc_32) << endl;
		cout << "good_length ................= " << local_lfh.good_length << endl;
		cout << endl;
	#endif

	if ( local_lfh.file_name_length > 0 ) {
		delete[] local_lfh.file_name;
	}
	if ( local_lfh.extra_field_length > 0 ) {
		delete[] local_lfh.extra_field;
	}

	// Now, let's decide if we need this entry o_0
	if ( several ) {
		if ( local_lfh.good_length > 0 ) {
			if ( local_lfh.compression_method <= lfh->compression_method ) {
				if ( local_lfh.good_length <= lfh->good_length ) {
					transpose_lfh(local_lfh, lfh);
				}
			} else {
				if ( local_lfh.good_length <= lfh->good_length ) {
					transpose_lfh(local_lfh, lfh);
				}
				// Go to the next one
				filei.seekg(local_lfh.start_byte, std::ios::beg);
				filei.seekg(local_lfh.compressed_size);
			}
		}
	} else {
		transpose_lfh(local_lfh, lfh);
	}
}
}
