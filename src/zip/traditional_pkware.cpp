
/*!
 * \file traditional_pkware.cpp
 * \brief Traditional PKWARE encryption - ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 * 
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#include "traditional_pkware.hpp"


static inline unsigned int create_crc32(
	char           * buf,
	const uint32_t * crc_32,
	unsigned int     len
) {
	uint32_t c = 0xffffffffL;
	
	#ifdef DEBUG
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
		#ifdef DEBUG
			cout
				<< "do08 c = " << (int*)c
				<< " | buf = " << (int*)(*buf & 0xFF)
				<< " | len = " << len
			<< endl;
		#endif
	}
	if ( len ) do {
		DO1(c, buf);
		#ifdef DEBUG
			cout
				<< "do01 c = " << (int*)c
				<< " | buf = " << (int*)(*buf & 0xFF)
				<< " | len = " << len
			<< endl;
		#endif
	} while ( --len );
	c ^= 0xffffffffL;
	#ifdef DEBUG
		cout << "     c = " << (int*)c << endl;
	#endif
	return c == *crc_32;
}

int traditional_pkware(ifstream & zip, struct local_file_header_light * lfh) {
	unsigned int len         = lfh->good_length;
	unsigned int check1      = lfh->last_mod_file_time >> 8;
	unsigned int check2      = lfh->good_crc_32 >> 24;
	bool least_ver           = lfh->version_needed_to_extract <= 20;
	char *encryption_header  = new char[12];
	char *buf                = new char[len];
	char *data               = new char[len];
	uint8_t *buffer          = new uint8_t[12];
	char *password;
	char buffer2[PWD_MAX];
	string chosen_one, decompressed;
	stringstream compressed;
	boost::iostreams::zlib_params p;
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	//generator gen;
	
	// Stats
	#if (!defined(DEBUG) && defined(USE_STATS))
		unsigned int num = 0;
		unsigned long total = 0;
		pthread_t stat;
		statistics s;
		time_t start_time = current_time();
		
		s.num         = &num;
		s.total       = &total;
		s.start_time  = &start_time;
		pthread_create(&stat, NULL, stats, (void*)&s);
	#endif
	
	if ( lfh->good_crc_32 == 0 ) {
		cout 
			<< "\r ! CRC-32 empty, cannot work without it on this encryption scheme."
			<< endl;
		return -1;
	}
	
	zip.seekg(lfh->start_byte, ios::beg);
	zip.read(encryption_header, 12);
	zip.read(buf, len);
	zip.close();
	
	// Adjust decompression options
	// The ZLIB header and trailing ADLER-32 checksum should be omitted
	p.noheader = true;
	
	//gen.init(4, 0, 10);
	//while ( gen.get_word(password) ) {
	while ( (password = read_stdin(buffer2)) != NULL ) {
		#ifdef DEBUG
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
			#ifdef DEBUG
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
			if ( lfh->compression_method == 0 ) {
				if ( create_crc32(data, &lfh->good_crc_32, len) ) {
					chosen_one = password;
					break;
				}
			}
			// The file is deflated
			else if ( lfh->compression_method == 8 ) {
				try {
					compressed.write(data, len);
					in.push(boost::iostreams::zlib_decompressor(p));
					in.push(compressed);
					boost::iostreams::copy(in, boost::iostreams::back_inserter(decompressed));
					if ( create_crc32((char*)decompressed.c_str(), &lfh->good_crc_32, len) ) {
						chosen_one = password;
						break;
					}
				} catch ( boost::iostreams::zlib_error & e ) {}
				in.reset();
				compressed.clear();
				decompressed = "";
			}
		}
		#if (!defined(DEBUG) && defined(USE_STATS))
			++num;
			++total;
		#endif
	}
	delete[] encryption_header;
	delete[] buf;
	delete[] data;
	delete[] buffer;
	
	// Statistics and result
	#if (!defined(DEBUG) && defined(USE_STATS))
		#if !defined(_WIN32)
			pthread_cancel(stat);
		#endif
		stats_sumary(total, start_time);
	#endif
	result(chosen_one);
	return 0;
}
