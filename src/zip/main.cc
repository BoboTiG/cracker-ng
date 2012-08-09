
/*!
 * \file main.cc
 * \brief ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.09
 * 
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#include "main.h"


// C'est parti mon kiki !
Cracker::Cracker(ifstream & filei) :
	filei(filei), start_byte(0), end_byte(0),
	strong_encryption(false),
	lfh(lfh), cd(cd), ecd(ecd)
{}

Cracker::~Cracker() {
	cout << " ^ Ex(c)iting." << endl;
}

unsigned int Cracker::check() {
	if ( ! check_headers() ) {
		cout << " ! Bad ZIP file (wrong headers)." << endl;
		return 0;
	}
	if ( ! find_central_directory() ) {
		cout << " ! Unable to find Central Directory signatures." << endl;
		return 0;
	}
	read_central_directory();
	read_end_central_directory();
	init_lfh();
	determine_chosen_one();
	if ( cd.is_encrypted && lfh.is_encrypted ) {
		if ( cd.strong_encryption && lfh.strong_encryption ) {
			cout << " - Encryption: strong" << endl;
			strong_encryption = true;
		} else {
			cout << " - Encryption: standard (traditional PKWARE)" << endl;
		}
	} else {
		cout << " + The file is not protected." << endl;
		return 0;
	}
	if ( ! check_method() ) {
		return 0;
	}
	if ( strong_encryption ) {
		return 0;
	}
	return 1;
}

void Cracker::crack() {
	unsigned int len        = this->lfh.good_length;
	unsigned int check1     = this->lfh.last_mod_file_time >> 8;
	unsigned int check2     = this->lfh.good_crc_32 >> 24;
	bool least_ver          = this->lfh.version_needed_to_extract <= 20;
	char *encryption_header = new char[12];
	char *buf               = new char[len];
	char *data              = new char[len];
	uint8_t *buffer         = new uint8_t[12];
	char *password;
	char pwd_buffer[PWD_MAX];
	string chosen_one, decompressed;
	stringstream compressed;
	boost::iostreams::zlib_params p;
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	size_t num = 0, total = 0;
	unsigned int found = 0;
	functions::statistics s = { &num, &total, &found };
	pthread_t stat;
	
	if ( this->lfh.good_crc_32 == 0 ) {
		cerr 
			<< "\033[A"
			<< " ! CRC-32 empty, cannot work without it on this encryption scheme."
			<< endl;
		return;
	}
	
	// Read encrypted data
	this->filei.seekg(this->lfh.start_byte, ios::beg);
	this->filei.read(encryption_header, 12);
	this->filei.read(buf, len);
	this->filei.close();
	
	// Adjust decompression options
	// The ZLIB header and trailing ADLER-32 checksum should be omitted
	p.noheader = true;
	
	// Let's go!
	cout << " . Working ..." << endl;
	pthread_create(&stat, NULL, functions::stats, (void*)&s);
	pthread_detach(stat);
	while ( (password = functions::read_stdin(pwd_buffer, PWD_MAX)) != NULL ) {
		#if 0
			cout
				<< endl
				<< "----"
				<< endl
				<< "password = " << password
				<< endl;
		#endif
		
		// 1) Initialize the three 32-bit keys with the password.
		init_keys(password);
		
		// 2) Read and decrypt the 12-byte encryption header,
		//    further initializing the encryption keys.
		memcpy(buffer, encryption_header, 12);
		for ( register unsigned int i = 0; i < 12; ++i ) {
			zdecode(buffer[i]);
			#if 0
				cout
					<< "encryption_header[] = " << (int*)(encryption_header[i] & 0xFF)
					<< "  |  "
					<< "buffer[] = " << (int*)buffer[i]
					<< endl;
			#endif
		}
		
		// First verification ...
		if ( buffer[11] == check2 || (least_ver && buffer[11] == check1) ) {
			// 3) Read and decrypt the compressed data stream using
			//    the encryption keys.
			memcpy(data, buf, len);
			for ( register unsigned int i = 0; i < len; ++i ) {
				zdecode(data[i]);
			}
			
			// The file is stored (no compression)
			if ( this->lfh.compression_method == 0 ) {
				if ( this->create_crc32(data, &this->lfh.good_crc_32, len) ) {
					chosen_one = password;
					break;
				}
			}
			// The file is deflated
			else if ( this->lfh.compression_method == 8 ) {
				try {
					compressed.write(data, len);
					in.push(boost::iostreams::zlib_decompressor(p));
					in.push(compressed);
					boost::iostreams::copy(in, boost::iostreams::back_inserter(decompressed));
					if ( this->create_crc32((char*)decompressed.c_str(), &this->lfh.good_crc_32, len) ) {
						chosen_one = password;
						break;
					}
				} catch ( boost::iostreams::zlib_error & e ) {}
				in.reset();
				compressed.clear();
				decompressed = "";
			}
		}
		++num;
		++total;
	}
	delete[] encryption_header;
	delete[] buf;
	delete[] data;
	delete[] buffer;
	if ( found == 0 ) {
		found = 2;
	}
	pthread_join(stat, (void**)NULL);
	functions::result(chosen_one);
}

int main(int argc, char *argv[]) {
	if ( ! functions::argz_traitment(argc, argv, MODULE, VERSION) ) {
		return 0;
	}
	printf(" ~ %s Cracker-ng v.%s { Tiger-222 }\n", MODULE, VERSION);
	printf(" - File......: %s\n", argv[1]);
	
	ifstream filei(argv[1], ios::in | ios::binary);
	if ( ! filei.is_open() ) {
		cerr << " ! Could not open the file." << endl;
		return 1;
	}
	
	// Who I am? I'm a champion!
	Cracker zizi(filei);
	if ( zizi.check() ) {
		 zizi.crack();
	} else {
		filei.close();
		return 1;
	}
	return 0;
}

unsigned int Cracker::check_headers() {
	uint32_t * header_signature = new uint32_t;
	unsigned int zip_signature = 0x04034b50;
	bool found = false;
	
	this->filei.read((char*)header_signature, 4);
	found = *header_signature == zip_signature;
	delete header_signature;
	return found;
}

unsigned int Cracker::check_method() {
	bool okay = 1;
	
	cout << " - Method....: ";
	switch ( this->lfh.compression_method )
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
				<< this->lfh.compression_method << "."
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

void Cracker::determine_chosen_one() {
	this->filei.seekg(this->lfh.start_byte, ios::beg);
	if ( this->ecd.total_entries > 1 ) {
		for ( unsigned int i = 1; i <= this->ecd.total_entries; ++i ) {
			read_local_file_header(true);
		}
	} else {
		read_local_file_header(false);
	}
	#if 0
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

unsigned int Cracker::find_central_directory() {
	char * tmp = new char[4];
	unsigned long i = 0;
	bool found_end = false, found = false;
	
	this->filei.seekg(-4, ios::end);
	i = this->filei.tellg();
	while ( i > 3 && ! found ) {
		this->filei.seekg(i, ios::beg);
		this->filei.read(tmp, 4);
		
		if ( ! found_end )
		{
			if ( tmp[0] == 0x50 && tmp[1] == 0x4b &&
				 tmp[2] == 0x05 && tmp[3] == 0x06 )
			{
				this->end_byte = i;
				found_end = true;
				i -= 3;
			}
		}
		else {
			if ( tmp[0] == 0x50 && tmp[1] == 0x4b &&
				 tmp[2] == 0x01 && tmp[3] == 0x02 )
			{
				this->start_byte = i;
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

void Cracker::read_central_directory() {
	this->filei.seekg(this->start_byte, ios::beg);
	this->filei.read((char*)&this->cd.header_signature,            4);
	this->filei.read((char*)&this->cd.version_made_by,             2);
	this->filei.read((char*)&this->cd.version_needed_to_extract,   2);
	this->filei.read((char*)&this->cd.general_purpose_bit_flag,    2);
	this->filei.read((char*)&this->cd.compression_method,          2);
	this->filei.read((char*)&this->cd.last_mod_file_time,          2);
	this->filei.read((char*)&this->cd.last_mod_file_date,          2);
	this->filei.read((char*)&this->cd.crc_32,                      4);
	this->filei.read((char*)&this->cd.compressed_size,             4);
	this->filei.read((char*)&this->cd.uncompressed_size,           4);
	this->filei.read((char*)&this->cd.file_name_length,            2);
	this->filei.read((char*)&this->cd.extra_field_length,          2);
	this->filei.read((char*)&this->cd.file_comment_length,         2);
	this->filei.read((char*)&this->cd.disk_number_part,            2);
	this->filei.read((char*)&this->cd.internal_file_attributes,    2);
	this->filei.read((char*)&this->cd.external_file_attributes,    4);
	this->filei.read((char*)&this->cd.relative_offset_of_local_fh, 4);
	if ( this->cd.file_name_length > 0 ) {
		this->cd.file_name = new char[this->cd.file_name_length + 1];
		this->filei.read(this->cd.file_name, this->cd.file_name_length);
		this->cd.file_name[this->cd.file_name_length] = '\0';
	}
	if ( this->cd.extra_field_length > 0 ) {
		this->cd.extra_field = new char[this->cd.extra_field_length];
		this->filei.read(this->cd.extra_field, this->cd.extra_field_length);
	}
	if ( this->cd.file_comment_length > 0 ) {
		this->cd.file_comment = new char[this->cd.file_comment_length + 1];
		this->filei.read(this->cd.file_comment, this->cd.file_comment_length);
		this->cd.file_comment[this->cd.file_comment_length] = '\0';
	}
	// Check encryption
	this->cd.is_encrypted = this->cd.strong_encryption = false;
	if ( this->cd.general_purpose_bit_flag & 1 ) {
		this->cd.is_encrypted = true;
	}
	if ( this->cd.general_purpose_bit_flag & (1 << 6) ) {
		this->cd.strong_encryption = true;
	}
	
	#if 0
		cout << endl;
		cout << "--- Central Directory" << endl;
		cout << "header_signature ...........= " << (int*)this->cd.header_signature << endl;
		cout << "version_made_by ............= " << this->cd.version_made_by << endl;
		cout << "version_needed_to_extract ..= " << this->cd.version_needed_to_extract << endl;
		cout << "general_purpose_bit_flag ...= " << this->cd.general_purpose_bit_flag << endl;
		cout << "compression_method .........= " << this->cd.compression_method << endl;
		cout << "last_mod_file_time .........= " << this->cd.last_mod_file_time << endl;
		cout << "last_mod_file_date .........= " << this->cd.last_mod_file_date << endl;
		cout << "crc_32 .....................= " << (int*)this->cd.crc_32 << endl;
		cout << "compressed_size ............= " << this->cd.compressed_size << endl;
		cout << "uncompressed_size ..........= " << this->cd.uncompressed_size << endl;
		cout << "file_name_length ...........= " << this->cd.file_name_length << endl;
		cout << "extra_field_length .........= " << this->cd.extra_field_length << endl;
		cout << "file_comment_length ........= " << this->cd.file_comment_length << endl;
		cout << "disk_number_part ...........= " << this->cd.disk_number_part << endl;
		cout << "internal_file_attributes ...= " << (int*)this->cd.internal_file_attributes << endl;
		cout << "external_file_attributes ...= " << (int*)this->cd.external_file_attributes << endl;
		cout << "relative_offset_of_local_hd = " << (int*)this->cd.relative_offset_of_local_fh << endl;
		if ( this->cd.file_name_length > 0 ) {
			cout << "file_name ..................= " << this->cd.file_name << endl;
		}
		if ( this->cd.extra_field_length > 0 ) {
			cout << "extra_field ................= " << this->cd.extra_field << endl;
		}
		if ( this->cd.file_comment_length > 0 ) {
			cout << "file_comment ...............= " << this->cd.file_comment << endl;
		}
	#endif
	if ( this->cd.file_name_length > 0 ) {
		delete[] this->cd.file_name;
	}
	if ( this->cd.extra_field_length > 0 ) {
		delete[] this->cd.extra_field;
	}
	if ( this->cd.file_comment_length > 0 ) {
		delete[] this->cd.file_comment;
	}
}

void Cracker::read_end_central_directory() {
	this->filei.seekg(this->end_byte, ios::beg);
	this->filei.read((char*)&this->ecd.header_signature,            4);
	this->filei.read((char*)&this->ecd.number_of_disk,              2);
	this->filei.read((char*)&this->ecd.number_of_disk_with_cd,      2);
	this->filei.read((char*)&this->ecd.cd_on_this_disk,             2);
	this->filei.read((char*)&this->ecd.total_entries,               2);
	this->filei.read((char*)&this->ecd.cd_size,                     4);
	this->filei.read((char*)&this->ecd.offset,                      4);
	this->filei.read((char*)&this->ecd.zip_file_comment_length,     2);
	if ( this->ecd.zip_file_comment_length > 0 ) {
		this->ecd.zip_file_comment = new char[this->ecd.zip_file_comment_length + 1];
		this->filei.read(this->ecd.zip_file_comment, this->ecd.zip_file_comment_length);
		this->ecd.zip_file_comment[this->ecd.zip_file_comment_length] = '\0';
	}
	
	#if 0
		cout << endl;
		cout << "--- End of Central Directory" << endl;
		cout << "header_signature ...........= " << (int*)this->ecd.header_signature << endl;
		cout << "number_of_disk .............= " << this->ecd.number_of_disk << endl;
		cout << "number_of_disk_with_cd .....= " << this->ecd.number_of_disk_with_cd << endl;
		cout << "cd_on_this_disk ............= " << this->ecd.cd_on_this_disk << endl;
		cout << "total_entries ..............= " << this->ecd.total_entries << endl;
		cout << "cd_size ....................= " << this->ecd.cd_size << endl;
		cout << "offset .....................= " << this->ecd.offset << endl;
		cout << "zip_file_comment_length ....= " << this->ecd.zip_file_comment_length << endl;
		if ( ecd->zip_file_comment_length > 0 ) {
			cout << "zip_file_comment ...........= " << this->ecd.zip_file_comment << endl;
		}
	#endif
	
	if ( this->ecd.zip_file_comment_length > 0 ) {
		delete[] this->ecd.zip_file_comment;
	}
}

void Cracker::read_local_file_header(bool several) {
	local_file_header *local_lfh = new local_file_header;

	this->filei.read((char*)&local_lfh->header_signature,            4);
	this->filei.read((char*)&local_lfh->version_needed_to_extract,   2);
	this->filei.read((char*)&local_lfh->general_purpose_bit_flag,    2);
	this->filei.read((char*)&local_lfh->compression_method,          2);
	this->filei.read((char*)&local_lfh->last_mod_file_time,          2);
	this->filei.read((char*)&local_lfh->last_mod_file_date,          2);
	this->filei.read((char*)&local_lfh->crc_32,                      4);
	this->filei.read((char*)&local_lfh->compressed_size,             4);
	this->filei.read((char*)&local_lfh->uncompressed_size,           4);
	this->filei.read((char*)&local_lfh->file_name_length,            2);
	this->filei.read((char*)&local_lfh->extra_field_length,          2);
	if ( local_lfh->file_name_length > 0 ) {
		local_lfh->file_name = new char[local_lfh->file_name_length + 1];
		this->filei.read(local_lfh->file_name, local_lfh->file_name_length);
		local_lfh->file_name[local_lfh->file_name_length] = '\0';
	}
	if ( local_lfh->extra_field_length > 0 ) {
		local_lfh->extra_field = new char[local_lfh->extra_field_length + 1];
		this->filei.read(local_lfh->extra_field, local_lfh->extra_field_length);
		local_lfh->extra_field[local_lfh->extra_field_length] = '\0';
	}
	
	// Save the compressed data start byte
	local_lfh->start_byte = this->filei.tellg();
	
	// Data Descriptor
	local_lfh->has_data_descriptor = false;
	local_lfh->data_desc_crc_32 = 0;
	if ( local_lfh->general_purpose_bit_flag & (1 << 3) ) {
		local_lfh->has_data_descriptor = true;
	}
	if ( local_lfh->has_data_descriptor ) {
		this->filei.seekg(local_lfh->compressed_size, ios::cur);
		this->filei.read((char*)&local_lfh->data_desc_signature, 4);
		if ( local_lfh->data_desc_signature != 0x8074b50 ) {
			local_lfh->data_desc_crc_32 = local_lfh->data_desc_signature;
			local_lfh->data_desc_signature = 0;
		} else {
			this->filei.read((char*)&local_lfh->data_desc_crc_32, 4);
		}
		this->filei.read((char*)&local_lfh->data_desc_compressed_size, 4);
		this->filei.read((char*)&local_lfh->data_desc_uncompressed_size, 4);
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
	
	#if 0
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
			if ( local_lfh->compression_method <= this->lfh.compression_method ) {
				if ( local_lfh->good_length <= this->lfh.good_length ) {
					transpose_lfh(local_lfh);
				}
			} else {
				if ( local_lfh->good_length <= this->lfh.good_length ) {
					transpose_lfh(local_lfh);
				}
				// Go to the next one
				this->filei.seekg(local_lfh->start_byte, ios::beg);
				this->filei.seekg(local_lfh->compressed_size);
			}
		}
	} else {
		transpose_lfh(local_lfh);
	}
}

void Cracker::init_lfh() {
	this->lfh.good_crc_32               = 0;
	this->lfh.version_needed_to_extract = 0;
	this->lfh.compression_method        = 99;
	this->lfh.start_byte                = 0;
	this->lfh.good_length               = 1024*1024*1024;
	this->lfh.last_mod_file_time        = 0;
	this->lfh.strong_encryption         = true;
	this->lfh.is_encrypted              = true;
}

void Cracker::transpose_lfh(struct local_file_header * local_lfh) {
	this->lfh.good_crc_32               = local_lfh->good_crc_32;
	this->lfh.version_needed_to_extract = local_lfh->version_needed_to_extract;
	this->lfh.compression_method        = local_lfh->compression_method;
	this->lfh.start_byte                = local_lfh->start_byte;
	this->lfh.good_length               = local_lfh->good_length;
	this->lfh.last_mod_file_time        = local_lfh->last_mod_file_time;
	this->lfh.strong_encryption         = local_lfh->strong_encryption;
	this->lfh.is_encrypted              = local_lfh->is_encrypted;
}
