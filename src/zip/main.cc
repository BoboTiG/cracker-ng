
/*!
 * \file main.cc
 * \brief ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.04
 * 
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#include "./main.h"


// C'est parti mon kiki !
Cracker::Cracker(const std::string& filename, const std::string& from) :
	filename(filename), from(from),
	debug(0), start_byte(0), end_byte(0), strong_encryption(0),
	filei(filename.c_str(), std::ios::in | std::ios::binary),
	lfh(lfh), cd(cd), ecd(ecd)
{}

Cracker::~Cracker() {
	printf(" ^ Ex(c)iting.\n\n");
}

unsigned int Cracker::check() {
	if ( !check_headers() ) {
		fprintf(stderr, " ! Bad ZIP file (wrong headers).\n");
		return 0;
	}
	if ( !find_central_directory() ) {
		fprintf(stderr, " ! Unable to find Central Directory signatures.\n");
		return 0;
	}
	read_ng::read_central_directory(this->filei, &this->cd, this->start_byte, this->debug);
	read_ng::read_end_central_directory(this->filei, &this->ecd, this->end_byte, this->debug);
	init_lfh();
	determine_chosen_one();
	if ( cd.is_encrypted && lfh.is_encrypted ) {
		if ( cd.strong_encryption && lfh.strong_encryption ) {
			printf(" - Encryption: strong\n");
			strong_encryption = true;
		} else {
			printf(" - Encryption: standard (traditional PKWARE)\n");
			if ( this->lfh.good_crc_32 == 0 ) {
				fprintf(stderr, "\033[A ! CRC-32 empty, cannot work without it on this encryption scheme.\n");
				return 0;
			}
		}
	} else {
		printf(" + The file is not protected.\n");
		return 0;
	}
	if ( !check_method() ) {
		return 0;
	}
	if ( strong_encryption ) {
		return 0;
	}
	if ( this->debug ) {
		return 0;
	}
	return 1;
}

void Cracker::crack() {
	size_t num = 0, i = 0;
	unsigned int found = 0;
	functions_ng::statistics s = { &num, &found };
	pthread_t stat;
	std::string chosen_one;
	unsigned int len        = this->lfh.good_length;
	unsigned int check1     = this->lfh.last_mod_file_time >> 8;
	unsigned int check2     = this->lfh.good_crc_32 >> 24;
	bool least_ver          = this->lfh.version_needed_to_extract <= 20;
	char *encryption_header = new char[12];
	char *buf               = new char[len];
	char *data              = new char[len];
	char *password          = new char[PWD_MAX];
	uint8_t *buffer         = new uint8_t[12];
	FILE *input             = NULL;
	std::stringstream compressed;
	std::string decompressed;
	boost::iostreams::zlib_params params;
	boost::iostreams::zlib_decompressor zdec;
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;

	if ( this->from == "STDIN" ) {
		input = stdin;
	} else {
		input = fopen(from.c_str(), "r");
	}

	// Read encrypted data
	filei.seekg(this->lfh.start_byte, std::ios::beg);
	filei.read(encryption_header, 12);
	filei.read(buf, len);
	filei.close();

	// Adjust decompression options
	// The ZLIB header and trailing ADLER-32 checksum should be omitted
	params.noheader = true;
	zdec = boost::iostreams::zlib_decompressor(params);
	
	// Reserve needed bytes to not end on a segfault, it happened with some files.
	decompressed.reserve(this->lfh.uncompressed_size);

	// Let's go!
	printf(" . Working ...\n");
	pthread_create(&stat, NULL, functions_ng::stats, reinterpret_cast<void*>(&s));
	while ( functions_ng::read_input(input, password, PWD_MAX) ) {
		// 1) Initialize the three 32-bit keys with the password.
		init_keys(password);
		
		// 2) Read and decrypt the 12-byte encryption header,
		//    further initializing the encryption keys.
		memcpy(buffer, encryption_header, 12);
		for ( i = 0; i < 12; ++i ) {
			zdecode(buffer[i]);
		}
		
		// First verification ...
		if ( buffer[11] == check2 || (least_ver && buffer[11] == check1) ) {
			// 3) Read and decrypt the compressed data stream using
			//    the encryption keys.
			memcpy(data, buf, len);
			for ( i = 0; i < len; ++i ) {
				zdecode(data[i]);
			}

			if ( this->lfh.compression_method == 0 ) {  // The file is stored (no compression)
				if ( this->create_crc32(data, len) ) {
					chosen_one = password;
					found = 1;
					break;
				}
			} else if ( this->lfh.compression_method == 8 ) {  // The file is deflated
				try {
					compressed.write(data, len);
					in.push(zdec);
					in.push(compressed);
					boost::iostreams::copy(in, boost::iostreams::back_inserter(decompressed));
					if ( this->create_crc32(decompressed.c_str(), len) ) {
						chosen_one = password;
						found = 1;
						break;
					}
				}
				catch(const boost::iostreams::zlib_error& e) {}
				in.reset();
				decompressed = "";
				compressed.str(std::string());
			}
		}
		++num;
	}
	delete[] buffer;                       buffer = 0;
	delete[] password;                   password = 0;
	delete[] data;                           data = 0;
	delete[] buf;                             buf = 0;
	delete[] encryption_header; encryption_header = 0;
	if ( this->from != "STDIN" ) {
		fclose(input);
	}
	if ( found == 0 ) {
		found = 2;
	}
	pthread_join(stat, reinterpret_cast<void**>(NULL));
	functions_ng::result(chosen_one);
}

int main(int argc, char *argv[]) {
	std::string filename, input;
	size_t flag = functions_ng::NONE;
	functions_ng::arguments argz = {
		MODULE, std::string(VERSION), filename, input, flag, (size_t)argc, argv };

	if ( !functions_ng::argz_traitment(argz) ) {
		return 0;
	}
	printf("\n ~ %s Cracker-ng v.%s { Tiger-222 }\n", MODULE, VERSION);
	printf(" - Generator.: %s\n", functions_ng::get_filename(argz.input).c_str());
	printf(" - File......: %s\n", functions_ng::get_filename(argz.filename).c_str());

	// Who I am? I'm a champion!
	Cracker zizi(argz.filename, argz.input);
	if ( !zizi.is_ok() ) {
		return 1;
	}
	zizi.set_debug(argz.flag == functions_ng::DEBUG);
	if ( !zizi.check() ) {
		return 1;
	}
	zizi.crack();
	return 0;
}

bool Cracker::check_headers() {
	uint32_t * header_signature = new uint32_t;
	unsigned int zip_signature = 0x04034b50;

	this->filei.read(reinterpret_cast<char*>(header_signature), 4);
	bool found = *header_signature == zip_signature;
	delete header_signature;
	return found;
}

bool Cracker::check_method() {
	bool okay = 1;

	printf(" - Method....: ");
	switch ( this->lfh.compression_method ) {
		case 0 :
			// No compression
			printf("stored\n");
			break;
		case 8 :
			printf("deflated\n");
			break;
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
		case 98:  // Still not implemented
			printf(
				"still not implemented"
				" - its number is %d.\n"
				"   You could send me an email with the compression number\n"
				"   and why not attach your file for my own tests.\n"
				"   Try --help to know my email ID.\n",
				this->lfh.compression_method);
			okay = 0;
		break;
		case 99:  // AES encryption
			printf("AES\n");
			okay = 0;
		break;
	}
	return okay;
}

void Cracker::determine_chosen_one() {
	this->filei.seekg(this->lfh.start_byte, std::ios::beg);
	if ( this->ecd.total_entries > 1 ) {
		for ( unsigned int i = 1; i <= this->ecd.total_entries; ++i ) {
			read_ng::read_local_file_header(this->filei, &this->lfh, true, this->debug);
		}
	} else {
		read_ng::read_local_file_header(this->filei, &this->lfh, false, this->debug);
	}
	if ( this->debug ) {
		std::cout << std::endl;
		std::cout << "--- The chosen one" << std::endl;
		std::cout << "version_needed_to_extract ..= " << this->lfh.version_needed_to_extract << std::endl;
		std::cout << "compression_method .........= " << this->lfh.compression_method << std::endl;
		std::cout << "last_mod_file_time .........= " << this->lfh.last_mod_file_time << std::endl;
		std::cout << "start_byte .................= " << this->lfh.start_byte << std::endl;
		std::cout << "is_encrypted ...............= " << this->lfh.is_encrypted << std::endl;
		std::cout << "strong_encryption ..........= " << this->lfh.strong_encryption << std::endl;
		std::cout << "good_crc_32 ................= " << reinterpret_cast<unsigned int*>(this->lfh.good_crc_32) << std::endl;
		std::cout << "good_length ................= " << this->lfh.good_length << std::endl;
		std::cout << "file_name ..................= " << this->lfh.file_name << std::endl;
		std::cout << std::endl;
	}
	printf(" > Chosen one: \"%s\" (%s bytes)\n",
		this->lfh.file_name,
		functions_ng::format_number(this->lfh.good_length).c_str());
}

bool Cracker::find_central_directory() {
	char * tmp = new char[4];
	bool found_end = false, found = false;

	this->filei.seekg(-4, std::ios::end);
	size_t i = this->filei.tellg();
	while ( i > 3 && !found ) {
		this->filei.seekg(i, std::ios::beg);
		this->filei.read(tmp, 4);
		if ( !found_end ) {
			if ( tmp[0] == 0x50 && tmp[1] == 0x4b &&
				 tmp[2] == 0x05 && tmp[3] == 0x06 ) {
				this->end_byte = i;
				found_end = true;
				i -= 3;
			}
		} else {
			if ( tmp[0] == 0x50 && tmp[1] == 0x4b &&
				 tmp[2] == 0x01 && tmp[3] == 0x02 ) {
				this->start_byte = i;
				found = true;
			} else if ( tmp[2] != 0x02 ) {
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

void Cracker::init_lfh() {
	this->lfh.good_crc_32               = 0;
	this->lfh.version_needed_to_extract = 0;
	this->lfh.compression_method        = 99;
	this->lfh.start_byte                = 0;
	this->lfh.good_length               = 1024*1024*1024;
	this->lfh.uncompressed_size         = 0;
	this->lfh.last_mod_file_time        = 0;
	this->lfh.strong_encryption         = true;
	this->lfh.is_encrypted              = true;
}

bool Cracker::is_ok() {
	if ( !this->filei.is_open() ) {
		fprintf(stderr, " ! I cannot open the file.\n");
		return false;
	}
	return true;
}

void Cracker::set_debug(bool yesno) {
	this->debug = yesno;
}
