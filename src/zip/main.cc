
/*!
 * \file main.cc
 * \brief ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.21
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#include "./main.h"


// C'est parti mon kiki !
Cracker::Cracker(const std::string& filename, const std::string& from) :
	filename(filename),
	from(from),
	filei(filename.c_str(), std::ios::in | std::ios::binary),
	debug(0),
	start_byte(0),
	end_byte(0),
	strong_encryption(0),
	lfh(lfh),
	cd(cd),
	ecd(ecd),
	title(std::string()),
	file(std::string()),
	chosen_one(std::string()),
	encryption(std::string()),
	method(std::string()),
	generator(std::string())
{}

Cracker::~Cracker() {}

int main(int argc, char *argv[]) {
	std::string filename, input;
	size_t flag = functions_ng::NONE;
	functions_ng::arguments argz = {
		MODULE, std::string(VERSION), filename, input, flag, (size_t)argc, argv
	};

	if ( !functions_ng::argz_traitment(argz) ) {
		return 0;
	}
	
	// Signal handler
	signal(SIGABRT, &signal_handler);
	signal(SIGTERM, &signal_handler);
	signal(SIGINT, &signal_handler);

	// Who I am? I'm a champion!
	Cracker zizi(argz.filename, argz.input);

	size_t str_len = WIDTH - 2;
	char* str = new char[str_len];
	snprintf(str, str_len, "~ %s Cracker-ng v%s { Tiger-222 } ~", MODULE, VERSION);
	zizi.set_title(str);
	snprintf(str, str_len, "%s", functions_ng::basename(argz.filename).c_str());
	zizi.set_file(str);
	snprintf(str, str_len, "%s", functions_ng::basename(argz.input).c_str());
	zizi.set_generator(str);
	delete[] str;

	zizi.set_debug(argz.flag == functions_ng::DEBUG);
	if ( !zizi.is_ok() ) {
		return 1;
	}
	zizi.crack();
	return 0;
}

void signal_handler(int s) {
	UNUSED(s);
	this_is_now_we_fight = false;
}

bool Cracker::check() {
	if ( !this->check_headers() ) {
		fprintf(stderr, " ! Bad ZIP file (wrong headers).\n");
		return 0;
	}
	if ( !this->find_central_directory() ) {
		fprintf(stderr, " ! Unable to find Central Directory signatures.\n");
		return 0;
	}
	read_ng::read_central_directory(this->filei, &this->cd, this->start_byte, this->debug);
	read_ng::read_end_central_directory(this->filei, &this->ecd, this->end_byte, this->debug);
	this->init_lfh();
	this->determine_chosen_one();
	if ( !this->check_method() ) {
		fprintf(stderr, " ! Method not implemented (%d).\n", this->lfh.compression_method);
		return 0;
	}
	int ret = this->check_lfh();
	if ( ret != 1 ) {
		fprintf(stderr, " ! I found a bad parameter (%d) into the LFH struct.\n", ret);
		return 0;
	}
	if ( this->cd.is_encrypted && this->lfh.is_encrypted ) {
		if ( this->cd.strong_encryption && this->lfh.strong_encryption ) {
			fprintf(stderr, " ! Encryption: strong (no implemented).\n");
			this->strong_encryption = true;
		} else {
			this->set_encryption("Encryption: standard (traditional PKWARE)");
			if ( this->lfh.good_crc_32 == 0 ) {
				fprintf(stderr, " ! CRC-32 empty, cannot work without it on this encryption scheme.\n");
				return 0;
			}
		}
	} else {
		fprintf(stderr, " + This file is not encrypted.\n");
		return 0;
	}
	if ( this->debug ) {
		return 0;
	}
	return 1;
}

void Cracker::crack() {
	GUI gui(
		this->title, this->file, this->chosen_one,
		this->encryption, this->method, this->generator
	);
	/*if ( !gui.is_ok() ) {
		return;
	}*/
	
	size_t num = 0, i = 0;
	unsigned int found = 0;
	functions_ng::statistics s = { &num, &found };
	pthread_t stat;
	std::string chosen_one;
	size_t len              = this->lfh.good_length;
	unsigned int check1     = this->lfh.last_mod_file_time >> 8;
	unsigned int check2     = this->lfh.good_crc_32 >> 24;
	bool least_ver          = this->lfh.version_needed_to_extract <= 20;
	char* encryption_header = new char[12];
	char* buf               = new char[len];
	char* password          = new char[PWD_MAX];
	uint8_t* buffer         = new uint8_t[12];
	unsigned char* data     = new unsigned char[len];
	unsigned char* dest     = new unsigned char[this->lfh.uncompressed_size];
	unsigned long destlen   = this->lfh.uncompressed_size;
	unsigned long sourcelen = len;
	FILE* input             = NULL;
	struct state io_state;

	if ( this->from == "STDIN" ) {
		input = stdin;
	} else {
		input = fopen(this->from.c_str(), "r");
	}

	// Read encrypted data
	this->filei.seekg(this->lfh.start_byte, std::ios::beg);
	this->filei.read(encryption_header, 12);
	this->filei.read(buf, len);
	this->filei.close();

	// Let's go!
	gui.run();
	pthread_create(&stat, NULL, functions_ng::stats, reinterpret_cast<void*>(&s));
	
	//int c;
	//char* str = new char[MAXLEN];  // String from "Try a word" input
	
	while ( this_is_now_we_fight ) {
		if ( !functions_ng::read_input(input, password, PWD_MAX) ) {
			break;
		}
		
		/*move(9, SHIFT);
		for ( i = 0; i < MAXLEN; ++i ) {
			printw(" ");
		}
		move(9, SHIFT);
		printw("%s", password);
		refresh();*/
		// Go to the manual entry area
		/*move(9, SHIFT);
		for ( i = 0; i < MAXLEN; ++i ) {
			printw(" ");
		}
		i = 0;
		do {
			move(9, SHIFT + i);
			c = wgetch(stdscr);
			str[i] = c;
			++i;
		} while ( c != KEY_ESC && c != CTRL_C && c != '\n' && i < MAXLEN );
		strcpy(password, str);*/
		
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
			
			if ( this->lfh.compression_method == 8 ) {  // The file is deflated
				if ( puff(dest, destlen, data, sourcelen, io_state) == 0 ) {
					if ( this->create_crc32(dest, len) ) {
						chosen_one = password;
						found = 1;
						break;
					}
				}
			} else {  // The file is stored (no compression)
				if ( this->create_crc32(data, len) ) {
					chosen_one = password;
					found = 1;
					break;
				}
			}
		}
		++num;
	}
	/*move(HEIGHT, 0);
	refresh();*/
	delete[] buf;                             buf = 0;
	delete[] dest;                           dest = 0;
	delete[] data;                           data = 0;
	delete[] buffer;                       buffer = 0;
	delete[] password;                   password = 0;
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

bool Cracker::check_headers() {
	uint32_t* header_signature = new uint32_t;
	uint32_t zip_signature = 0x04034b50;

	this->filei.read(reinterpret_cast<char*>(header_signature), 4);
	bool found = *header_signature == zip_signature;
	delete header_signature;
	return found;
}

bool Cracker::check_method() {
	bool okay = 0;

	switch ( this->lfh.compression_method ) {
		case 0 :
			this->set_method("Method....: stored");
			okay = 1;
			break;
		case 8 :
			this->set_method("Method....: deflated");
			okay = 1;
			break;
	}
	return okay;
}

int Cracker::check_lfh() {
	size_t max16 = std::numeric_limits<uint16_t>::max();
	size_t max32 = std::numeric_limits<uint32_t>::max();
	
	if ( this->lfh.good_crc_32 > max32 ) {
		return -1;
	}
	if ( this->lfh.version_needed_to_extract > max16 ) {
		return -2;
	}
	if ( this->lfh.compression_method > 99 ) {
		return -3;
	}
	if ( this->lfh.start_byte > max32 ) {
		return -4;
	}
	if ( this->lfh.good_length > max32 ) {
		return -5;
	}
	if ( this->lfh.uncompressed_size > max32 ) {
		return -6;
	}
	if ( this->lfh.last_mod_file_time > max16 ) {
		return -7;
	}
	return 1;
}

void Cracker::determine_chosen_one() {
	this->filei.seekg(this->lfh.start_byte, std::ios::beg);
	if ( this->ecd.total_entries > 1 ) {
		for ( size_t i = 1; i <= this->ecd.total_entries; ++i ) {
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
	} else {
		size_t n = 10 + strlen(this->lfh.file_name) + functions_ng::format_number(this->lfh.good_length).length();
		char* str = new char[n + 1];
		snprintf(str, n, "%s (%s bytes)", this->lfh.file_name, functions_ng::format_number(this->lfh.good_length).c_str());
		this->set_chosen_one(str);
		delete[] str;
	}
}

bool Cracker::find_central_directory() {
	char* tmp = new char[4];
	bool found_end = false, found = false;
	size_t i;

	this->filei.seekg(-4, std::ios::end);
	i = this->filei.tellg();
	for ( ; i > 3 && !found; --i ) {
		this->filei.seekg(i, std::ios::beg);
		this->filei.read(tmp, 4);
		if ( !found_end ) {
			if ( tmp[0] == 0x50 && tmp[1] == 0x4b &&
				 tmp[2] == 0x05 && tmp[3] == 0x06 ) {
				this->end_byte = i;
				found_end = true;
				i -= 3;
			} else if ( tmp[2] != 0x06 ) {
				--i;
				if ( tmp[1] != 0x06 ) {
					--i;
					if ( tmp[0] != 0x06 ) {
						--i;
					}
				}
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
	bool res = true;
	
	if ( !this->filei.is_open() ) {
		fprintf(stderr, " ! I cannot open the file.\n");
		res = false;
	}
	if ( !this->check() ) {
		res = false;
	}
	return res;
}
