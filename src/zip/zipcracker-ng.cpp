
/*!
 * \file zipcracker-ng.cpp
 * \brief ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 * 
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#include "zipcracker-ng.hpp"
#include "traditional_pkware.cpp"


// C'est parti mon kiki !
int main(int argc, char *argv[]) {
	
	if ( argc && ! argz_traitment(argv) ) {
		return 0;
	}
	
	cout << " ~ ZIP Cracker-ng v." << VERSION << " { Tiger-222 }" << endl;
	
	// 0x00. Variables -------------------------------------------------
	ifstream zip;
	local_file_header_light lfh;
	central_directory cd;
	end_central_directory ecd;
	unsigned int start_byte = 0, end_byte = 0;
	bool strong_encryprion = false;
	
	// 0x01. Open the file ---------------------------------------------
	cout << " - File......: " << argv[1] << endl;
	zip.open(argv[1], ios::in | ios::binary);
	if ( ! zip.is_open() ) {
		cout << " ! Could not open the file." << endl;
		return 1;
	}
	
	// 0x02. Read headers ----------------------------------------------
	if ( ! check_headers(zip) ) {
		cout << " ! Bad ZIP file (wrong headers)." << endl;
		zip.close();
		return 1;
	}
	if ( ! find_central_directory(zip, start_byte, end_byte) ) {
		cout << " ! Unable to find Central Directory signatures." << endl;
		zip.close();
		return 1;
	}
	read_central_directory(zip, &cd, start_byte);
	read_end_central_directory(zip, &ecd, end_byte);
	init_lfh(&lfh);
	determine_chosen_one(zip, &lfh, ecd.total_entries);
	
	// 0x03. Check encryption (or not) and method used -----------------
	if ( cd.is_encrypted && lfh.is_encrypted ) {
		if ( cd.strong_encryption && lfh.strong_encryption ) {
			cout << " - Encryption: strong" << endl;
			strong_encryprion = true;
		} else {
			//cout << " - Encryption: traditional PKWARE" << endl;
			cout << " - Encryption: standard" << endl;
		}
	} else {
		cout << " + The file is not protected." << endl;
		zip.close();
		return 0;
	}
	
	// 0x04. Who I am? I'm a champion! Crack it! -----------------------
	if ( ! check_method(lfh.compression_method) ) {
		return 1;
	}
	if ( strong_encryprion ) {
		
	} else {
		traditional_pkware(zip, &lfh);
	}
	
	// 0x05. Ex(c)iting ------------------------------------------------
	cout << " ^ Ex(c)iting." << endl;
	if ( zip.is_open() ) {
		zip.close();
	}
	return 0;
}

unsigned int check_headers(ifstream & zip) {
	uint32_t * header_signature = new uint32_t;
	unsigned int zip_signature = 0x04034b50;
	bool found = false;
	
	zip.read((char*)header_signature, 4);
	found = *header_signature == zip_signature;
	delete header_signature;
	return found;
}

unsigned int check_method(unsigned int compression_method) {
	bool okay = 1;
	
	cout << " - Method....: ";
	switch ( compression_method )
	{
		case 0 :
			// No compression
			cout << "stored" << endl; break;
		case 8 :
			cout << "deflated" << endl; break;
		case 1 :
		case 2 :
		case 3 :
		case 4 :
		case 5 :
		case 6 :
		case 9 :
		case 12:
		case 14:
		case 97:
		case 98: // Still not implemented
			cout
				<< "still not implemented"
				<< " - its number is "
				<< compression_method << "."
				<< endl
				<< "   You could send me an email with the compression number"
				<< endl
				<< "   and why not attach your file for my own tests."
				<< endl
				<< "   Try --help to know my email ID."
				<< endl;
			okay = 0;
		break;
		case 99: // AES encryption
			cout << "AES" << endl;
			okay = 0;
		break;
	}
	return okay;
}

void determine_chosen_one(
	ifstream                       & zip,
	struct local_file_header_light * lfh,
	unsigned int                     nb_entries
) {
	zip.seekg(lfh->start_byte, ios::beg);
	if ( nb_entries > 1 ) {
		for ( unsigned int i = 1; i <= nb_entries; ++i ) {
			read_local_file_header(zip, lfh, true);
		}
	} else {
		read_local_file_header(zip, lfh, false);
	}
	#ifdef DEBUG
		cout << endl;
		cout << "--- The chosen one" << endl;
		cout << "version_needed_to_extract ..= " << lfh->version_needed_to_extract << endl;
		cout << "compression_method .........= " << lfh->compression_method << endl;
		cout << "last_mod_file_time .........= " << lfh->last_mod_file_time << endl;
		cout << "start_byte .................= " << lfh->start_byte << endl;
		cout << "is_encrypted ...............= " << lfh->is_encrypted << endl;
		cout << "strong_encryption ..........= " << lfh->strong_encryption << endl;
		cout << "good_crc_32 ................= " << (int*)lfh->good_crc_32 << endl;
		cout << "good_length ................= " << lfh->good_length << endl;
		cout << endl;
	#endif
}

unsigned int find_central_directory(
	ifstream     & zip,
	unsigned int & start_byte,
	unsigned int & end_byte
) {
	char * tmp = new char[4];
	unsigned long i = 0;
	bool found_end = false, found = false;
	
	zip.seekg(-4, ios::end);
	i = zip.tellg();
	while ( i > 3 && ! found ) {
		zip.seekg(i, ios::beg);
		zip.read(tmp, 4);
		
		if ( ! found_end )
		{
			if ( tmp[0] == 0x50 && tmp[1] == 0x4b &&
				 tmp[2] == 0x05 && tmp[3] == 0x06 )
			{
				end_byte = i;
				found_end = true;
				i -= 3;
			}
		}
		else {
			if ( tmp[0] == 0x50 && tmp[1] == 0x4b &&
				 tmp[2] == 0x01 && tmp[3] == 0x02 )
			{
				start_byte = i;
				found = true;
			}
			else if ( tmp[2] != 0x02 ) {
				--i;
				if ( tmp[1] != 0x02 ) {
					--i;
					if ( tmp[0] != 0x02 ) {
						--i;
					}
				}
			}
		}
		--i;
	}
	delete[] tmp;
	return found;
}

void read_central_directory(
	ifstream                 & zip,
	struct central_directory * cd,
	unsigned int               start_byte
) {
	zip.seekg(start_byte, ios::beg);
	zip.read((char*)&cd->header_signature,            4);
	zip.read((char*)&cd->version_made_by,             2);
	zip.read((char*)&cd->version_needed_to_extract,   2);
	zip.read((char*)&cd->general_purpose_bit_flag,    2);
	zip.read((char*)&cd->compression_method,          2);
	zip.read((char*)&cd->last_mod_file_time,          2);
	zip.read((char*)&cd->last_mod_file_date,          2);
	zip.read((char*)&cd->crc_32,                      4);
	zip.read((char*)&cd->compressed_size,             4);
	zip.read((char*)&cd->uncompressed_size,           4);
	zip.read((char*)&cd->file_name_length,            2);
	zip.read((char*)&cd->extra_field_length,          2);
	zip.read((char*)&cd->file_comment_length,         2);
	zip.read((char*)&cd->disk_number_part,            2);
	zip.read((char*)&cd->internal_file_attributes,    2);
	zip.read((char*)&cd->external_file_attributes,    4);
	zip.read((char*)&cd->relative_offset_of_local_fh, 4);
	if ( cd->file_name_length > 0 ) {
		cd->file_name = new char[cd->file_name_length + 1];
		zip.read(cd->file_name, cd->file_name_length);
		cd->file_name[cd->file_name_length] = '\0';
	}
	if ( cd->extra_field_length > 0 ) {
		cd->extra_field = new char[cd->extra_field_length];
		zip.read(cd->extra_field, cd->extra_field_length);
	}
	if ( cd->file_comment_length > 0 ) {
		cd->file_comment = new char[cd->file_comment_length + 1];
		zip.read(cd->file_comment, cd->file_comment_length);
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
	
	#ifdef DEBUG
		cout << endl;
		cout << "--- Central Directory" << endl;
		cout << "header_signature ...........= " << (int*)cd->header_signature << endl;
		cout << "version_made_by ............= " << cd->version_made_by << endl;
		cout << "version_needed_to_extract ..= " << cd->version_needed_to_extract << endl;
		cout << "general_purpose_bit_flag ...= " << cd->general_purpose_bit_flag << endl;
		cout << "compression_method .........= " << cd->compression_method << endl;
		cout << "last_mod_file_time .........= " << cd->last_mod_file_time << endl;
		cout << "last_mod_file_date .........= " << cd->last_mod_file_date << endl;
		cout << "crc_32 .....................= " << (int*)cd->crc_32 << endl;
		cout << "compressed_size ............= " << cd->compressed_size << endl;
		cout << "uncompressed_size ..........= " << cd->uncompressed_size << endl;
		cout << "file_name_length ...........= " << cd->file_name_length << endl;
		cout << "extra_field_length .........= " << cd->extra_field_length << endl;
		cout << "file_comment_length ........= " << cd->file_comment_length << endl;
		cout << "disk_number_part ...........= " << cd->disk_number_part << endl;
		cout << "internal_file_attributes ...= " << (int*)cd->internal_file_attributes << endl;
		cout << "external_file_attributes ...= " << (int*)cd->external_file_attributes << endl;
		cout << "relative_offset_of_local_hd = " << (int*)cd->relative_offset_of_local_fh << endl;
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
	ifstream                     & zip,
	struct end_central_directory * ecd,
	unsigned int                   start_byte
) {
	zip.seekg(start_byte, ios::beg);
	zip.read((char*)&ecd->header_signature,            4);
	zip.read((char*)&ecd->number_of_disk,              2);
	zip.read((char*)&ecd->number_of_disk_with_cd,      2);
	zip.read((char*)&ecd->cd_on_this_disk,             2);
	zip.read((char*)&ecd->total_entries,               2);
	zip.read((char*)&ecd->cd_size,                     4);
	zip.read((char*)&ecd->offset,                      4);
	zip.read((char*)&ecd->zip_file_comment_length,     2);
	if ( ecd->zip_file_comment_length > 0 ) {
		ecd->zip_file_comment = new char[ecd->zip_file_comment_length + 1];
		zip.read(ecd->zip_file_comment, ecd->zip_file_comment_length);
		ecd->zip_file_comment[ecd->zip_file_comment_length] = '\0';
	}
	
	#ifdef DEBUG
		cout << endl;
		cout << "--- End of Central Directory" << endl;
		cout << "header_signature ...........= " << (int*)ecd->header_signature << endl;
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
	ifstream                       & zip,
	struct local_file_header_light * lfh,
	bool                             several
) {
	local_file_header *local_lfh = new local_file_header;

	zip.read((char*)&local_lfh->header_signature,            4);
	zip.read((char*)&local_lfh->version_needed_to_extract,   2);
	zip.read((char*)&local_lfh->general_purpose_bit_flag,    2);
	zip.read((char*)&local_lfh->compression_method,          2);
	zip.read((char*)&local_lfh->last_mod_file_time,          2);
	zip.read((char*)&local_lfh->last_mod_file_date,          2);
	zip.read((char*)&local_lfh->crc_32,                      4);
	zip.read((char*)&local_lfh->compressed_size,             4);
	zip.read((char*)&local_lfh->uncompressed_size,           4);
	zip.read((char*)&local_lfh->file_name_length,            2);
	zip.read((char*)&local_lfh->extra_field_length,          2);
	if ( local_lfh->file_name_length > 0 ) {
		local_lfh->file_name = new char[local_lfh->file_name_length + 1];
		zip.read(local_lfh->file_name, local_lfh->file_name_length);
		local_lfh->file_name[local_lfh->file_name_length] = '\0';
	}
	if ( local_lfh->extra_field_length > 0 ) {
		local_lfh->extra_field = new char[local_lfh->extra_field_length + 1];
		zip.read(local_lfh->extra_field, local_lfh->extra_field_length);
		local_lfh->extra_field[local_lfh->extra_field_length] = '\0';
	}
	
	// Save the compressed data start byte
	local_lfh->start_byte = zip.tellg();
	
	// Data Descriptor
	local_lfh->has_data_descriptor = false;
	local_lfh->data_desc_crc_32 = 0;
	if ( local_lfh->general_purpose_bit_flag & (1 << 3) ) {
		local_lfh->has_data_descriptor = true;
	}
	if ( local_lfh->has_data_descriptor ) {
		zip.seekg(local_lfh->compressed_size, ios::cur);
		zip.read((char*)&local_lfh->data_desc_signature, 4);
		if ( local_lfh->data_desc_signature != 0x8074b50 ) {
			local_lfh->data_desc_crc_32 = local_lfh->data_desc_signature;
			local_lfh->data_desc_signature = 0;
		} else {
			zip.read((char*)&local_lfh->data_desc_crc_32, 4);
		}
		zip.read((char*)&local_lfh->data_desc_compressed_size, 4);
		zip.read((char*)&local_lfh->data_desc_uncompressed_size, 4);
	}
	
	// Data length
	local_lfh->good_length =
	(
		local_lfh->uncompressed_size > 0 ?
			local_lfh->uncompressed_size : local_lfh->data_desc_uncompressed_size
	);
	
	// CRC-32
	local_lfh->good_crc_32 =
	(
		local_lfh->crc_32 > 0 ?
			local_lfh->crc_32 :
			(
				local_lfh->data_desc_crc_32 > 0 ? local_lfh->data_desc_crc_32 :	0
			)
	);
	
	// Check encryption
	local_lfh->is_encrypted = false;
	local_lfh->strong_encryption = false;
	if ( local_lfh->general_purpose_bit_flag & 1 ) {
		local_lfh->is_encrypted = true;
	}
	if ( local_lfh->general_purpose_bit_flag & (1 << 6) ) {
		local_lfh->strong_encryption = true;
	}
	
	#ifdef DEBUG
		cout << endl;
		cout << "--- Local File Header" << endl;
		cout << "header_signature ...........= " << (int*)local_lfh->header_signature << endl;
		cout << "version_needed_to_extract ..= " << local_lfh->version_needed_to_extract << endl;
		cout << "general_purpose_bit_flag ...= " << local_lfh->general_purpose_bit_flag << endl;
		cout << "compression_method .........= " << local_lfh->compression_method << endl;
		cout << "last_mod_file_time .........= " << local_lfh->last_mod_file_time << endl;
		cout << "last_mod_file_date .........= " << local_lfh->last_mod_file_date << endl;
		cout << "crc_32 .....................= " << (int*)local_lfh->crc_32 << endl;
		cout << "compressed_size ............= " << local_lfh->compressed_size << endl;
		cout << "uncompressed_size ..........= " << local_lfh->uncompressed_size << endl;
		cout << "file_name_length ...........= " << local_lfh->file_name_length << endl;
		cout << "extra_field_length .........= " << local_lfh->extra_field_length << endl;
		if ( local_lfh->file_name_length > 0 ) {
			cout << "file_name ..................= " << local_lfh->file_name << endl;
		}
		if ( local_lfh->extra_field_length > 0 ) {
			cout << "extra_field ................= " << (int*)local_lfh->extra_field << endl;
		}
		cout << "---" << endl;
		cout << "start_byte .................= " << local_lfh->start_byte << endl;
		cout << "is_encrypted ...............= " << local_lfh->is_encrypted << endl;
		cout << "strong_encryption ..........= " << local_lfh->strong_encryption << endl;
		cout << "has_data_descriptor ........= " << local_lfh->has_data_descriptor << endl;
		if ( local_lfh->has_data_descriptor ) {
			cout << "data_desc_signature ........= " << (int*)local_lfh->data_desc_signature << endl;
			cout << "data_desc_crc_32 ...........= " << (int*)local_lfh->data_desc_crc_32 << endl;
			cout << "data_desc_compressed_size ..= " << local_lfh->data_desc_compressed_size << endl;
			cout << "data_desc_uncompressed_size = " << local_lfh->data_desc_uncompressed_size << endl;
		}
		cout << "good_crc_32 ................= " << (int*)local_lfh->good_crc_32 << endl;
		cout << "good_length ................= " << local_lfh->good_length << endl;
		cout << endl;
	#endif
	
	if ( local_lfh->file_name_length > 0 ) {
		delete[] local_lfh->file_name;
	}
	if ( local_lfh->extra_field_length > 0 ) {
		delete[] local_lfh->extra_field;
	}

	// Now, let's decide if we need this entry o_0
	if ( several ) {
		if ( local_lfh->good_length > 0 ) {
			if ( local_lfh->compression_method <= lfh->compression_method ) {
				if ( local_lfh->good_length <= lfh->good_length ) {
					transpose_lfh(local_lfh, lfh);
				}
			} else {
				if ( local_lfh->good_length <= lfh->good_length ) {
					transpose_lfh(local_lfh, lfh);
				}
				// Go to the next one
				zip.seekg(local_lfh->start_byte, ios::beg);
				zip.seekg(local_lfh->compressed_size);
			}
		}
	} else {
		transpose_lfh(local_lfh, lfh);
	}
}

void init_lfh(struct local_file_header_light * lfh) {
	lfh->good_crc_32               = 0;
	lfh->version_needed_to_extract = 0;
	lfh->compression_method        = 99;
	lfh->start_byte                = 0;
	lfh->good_length               = 1024*1024*1024;
	lfh->last_mod_file_time        = 0;
	lfh->strong_encryption         = true;
	lfh->is_encrypted              = true;
}

void transpose_lfh(
	struct local_file_header * local_lfh,
	struct local_file_header_light * lfh

) {
	lfh->good_crc_32               = local_lfh->good_crc_32;
	lfh->version_needed_to_extract = local_lfh->version_needed_to_extract;
	lfh->compression_method        = local_lfh->compression_method;
	lfh->start_byte                = local_lfh->start_byte;
	lfh->good_length               = local_lfh->good_length;
	lfh->last_mod_file_time        = local_lfh->last_mod_file_time;
	lfh->strong_encryption         = local_lfh->strong_encryption;
	lfh->is_encrypted              = local_lfh->is_encrypted;
}
