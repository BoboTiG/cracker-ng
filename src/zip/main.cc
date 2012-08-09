
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
	read_ng::read_central_directory(this->filei, &this->cd, this->start_byte);
	read_ng::read_end_central_directory(this->filei, &this->ecd, this->end_byte);
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
	functions_ng::statistics s = { &num, &total, &found };
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
	pthread_create(&stat, NULL, functions_ng::stats, (void*)&s);
	pthread_detach(stat);
	while ( (password = functions_ng::read_stdin(pwd_buffer, PWD_MAX)) != NULL ) {
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
	functions_ng::result(chosen_one);
}

int main(int argc, char *argv[]) {
	if ( ! functions_ng::argz_traitment(argc, argv, MODULE, VERSION) ) {
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
			read_ng::read_local_file_header(this->filei, &this->lfh, true);
		}
	} else {
		read_ng::read_local_file_header(this->filei, &this->lfh, false);
	}
	#if 0
		cout << endl;
		cout << "--- The chosen one" << endl;
		cout << "version_needed_to_extract ..= " << this->lfh.version_needed_to_extract << endl;
		cout << "compression_method .........= " << this->lfh.compression_method << endl;
		cout << "last_mod_file_time .........= " << this->lfh.last_mod_file_time << endl;
		cout << "start_byte .................= " << this->lfh.start_byte << endl;
		cout << "is_encrypted ...............= " << this->lfh.is_encrypted << endl;
		cout << "strong_encryption ..........= " << this->lfh.strong_encryption << endl;
		cout << "good_crc_32 ................= " << (int*)this->lfh.good_crc_32 << endl;
		cout << "good_length ................= " << this->lfh.good_length << endl;
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
