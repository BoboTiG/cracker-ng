
/*!
 * \file cracker.h
 * \brief Cracker class header for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 *
 * Copyright (C) 2011-2022 Mickaël 'Tiger-222' Schoentgen.
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#ifndef SRC_SHARED_CRACKER_H_
#define SRC_SHARED_CRACKER_H_

#include <stdint.h>
#include <fstream>
#include "./stats.h"
#include "./gui.h"
#ifdef CPT
	#include "./../cpt/tables.h"
	#include "./../cpt/rijndael.h"
	#include "./../cpt/ccryptlib.h"
	static const size_t PWD_MAX = 80;  //!< Maximum password length
#elif ZIP
	#include <limits>
	#include "./../zip/crypt.h"
	#include "./../zip/puff.h"
	#include "./../zip/read.h"
	static const size_t PWD_MAX  = 80;  //!< Maximum password length
	static const size_t STORED   = 0;  //!< No compression
	static const size_t DEFLATED = 8;  //!< Compression used: deflated
#endif


static bool this_is_now_we_fight = true;  //!< Boolean to know when to stop the cracking process


class Cracker {

public:
	const std::string filename, from;

	Cracker(const std::string&, const std::string&);
	~Cracker();
	void crack();
	bool is_ok();
	void set_debug(const bool yesno)     { this->debug      = yesno; };
	void set_title(const char* str)      { this->title      = str; };
	void set_file(const char* str)       { this->file       = str; };
	void set_chosen_one(const char* str) { this->chosen_one = str; };
	void set_encryption(const char* str) { this->encryption = str; };
	void set_method(const char* str)     { this->method     = str; };
	void set_generator(const char* str)  { this->generator  = str; };
	void set_false_pos(const std::string&, const size_t&);

private:
	std::ifstream filei;
	size_t debug;
	std::string title, file, chosen_one, encryption, method, generator;
	std::string false_pos[8];
#ifdef ZIP
	size_t start_byte, end_byte, strong_encryption;
	local_file_header_light lfh;
	central_directory cd;
	end_central_directory ecd;
#endif

	Cracker(const Cracker &);
	Cracker & operator=(const Cracker &);

	inline size_t istrlen(const char *s) {
		size_t len = 0;
		unsigned x;
		for( ;; ) {
			x = *(unsigned*)s;
			if ( (x & 0xFF) == 0 ) return len;
			if ( (x & 0xFF00) == 0 ) return len + 1;
			if ( (x & 0xFF0000) == 0 ) return len + 2;
			if ( (x & 0xFF000000) == 0 ) return len + 3;
			s += 4, len += 4;
		}
	}

	// Optimized read from stdin
	inline int cfgets(FILE* input, char*& output, const size_t& len) {
		if ( fgets(output, len, input) ) {
			//output[strcspn(output, "\n")] = 0;
			//output[strlen(output) - 1] = 0;  // Faster than previous line
			//output[strnlen(output, len) - 1] = 0;  // Slightly faster than previous line
			output[istrlen(output) - 1] = 0;  // Even faster than previous line
			return 1;
		}
		return 0;
	}

	// Optimized read from string
	inline bool csgets(char** input, char*& output, const size_t& len)
	{
		char *next = *input;
		unsigned int numread = 0;
		for ( ; numread + 1 < len && *next; ++numread ) {
			int isnewline = (*next == '\n');
			if ( isnewline ) {
				*output = *next++;
				break;
			}
			*output++ = *next++;
		}
		if ( numread == 0 ) {
			return false;
		}
		*output = '\0';
		*input = next;
		return next != output;
	}

	/*!
	 * \fn check()
	 * \brief Launch all checks to see if we can go to work.
	 * \return \li 0 if \b it is good;
	 * \return \li 1 otherwise.
	 */
	bool check();

	/*!
	 * \fn result(const std::string& password)
	 * \brief Print the password if found.
	 * \param password The password (str::string() if empty).
	 */
	void result(const std::string&);

#ifdef ZIP
	/*!
	 * \fn check_headers()
	 * \brief Check first bytes to verify file is a valid one to crack.
	 * \return \li 0 if \b not a good file;
	 * \return \li 1 otherwise.
	 */
	bool check_headers();

	/*!
	 * \fn check_lfh()
	 * \brief Check all variables of the LFH struct.
	 * \return \li 0 if \b not a good LFH;
	 * \return \li 1 otherwise.
	 */
	int check_lfh();

	/*!
	 * \fn unsigned int check_method()
	 * \brief Check if the compression/encryption method is implemented.
	 * \return \li 0 if \b not implemented;
	 * \return \li 1 otherwise.
	 */
	bool check_method();

	/*!
	 * \fn determine_chosen_one()
	 * \brief Determine the best file to try the crack process against.
	 */
	void determine_chosen_one();

	/*!
	 * \fn find_central_directory()
	 * \brief Try to find the Central Directory Headers start and end.
	 * \return \li 0 if signatures \b not found;
	 * \return \li 1 otherwise.
	 *
	 * Signatures: start = 0x02014b50, end = 0x06054b50.
	 *
	 */
	bool find_central_directory();

	/*!
	 * \fn init_lfh()
	 * \brief Initialize the light Local File Header.
	 * Contains useful informations about file to crack.
	 */
	void init_lfh();

	/*!
	 * \fn is_false_positive(const std::string&)
	 * \brief Check the the given pasword is not a false positive.
	 * \param password The password to check.
	 * \return \li 0 if is \b false positive;
	 * \return \li 1 otherwise.
	 */
	bool is_false_positive(const std::string&);
#endif
};

#endif  // SRC_SHARED_CRACKER_H_
