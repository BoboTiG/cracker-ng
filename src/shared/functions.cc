
/*!
 * \file functions.cc
 * \brief Cracker-ng (optimized) functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.21
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./functions.h"


namespace functions_ng {

bool argz_traitment(const arguments& argz) {
	if ( argz.argc == 1 ) {
		functions_ng::usage(argz.module);
		return false;
	} else if ( !strcmp(argz.argv[1], "-h") || !strcmp(argz.argv[1], "--help") ) {
		functions_ng::help(argz.module);
		return false;
	} else if ( !strcmp(argz.argv[1], "-v")
			 || !strcmp(argz.argv[1], "--version") ) {
		functions_ng::version(argz.module, argz.version);
		return false;
	} else {
		size_t i;
		bool filename = false;
		bool wordlist = false;
		for ( i = 1; i < argz.argc; ++i ) {
			
			// -f | --file to set the file to crack
			if ( !filename && 
				(!strcmp(argz.argv[i], "-f") || !strcmp(argz.argv[i], "--file")) ) {
				if ( !argz.argv[++i] ) {
					fprintf(stderr, " ! Please gimme a file.\n");
					return false;
				}
				if ( !functions_ng::file_exists(argz.argv[i]) ) {
					fprintf(stderr, " ! Could not open the file.\n");
					return false;
				}
				argz.filename = argz.argv[i];
				filename = true;
			}
			
			// -i | --infos to get informations about a file
			// Useful for the ZIP module, for others it will just do as -f | --file option
			else if ( !filename && 
				     (!strcmp(argz.argv[i], "-i") || !strcmp(argz.argv[i], "--infos")) ) {
				if ( !argz.argv[++i] ) {
					fprintf(stderr, " ! Please gimme a wordlist.\n");
					return false;
				}
				if ( !functions_ng::file_exists(argz.argv[i]) ) {
					fprintf(stderr, " ! Could not open the file.\n");
					return false;
				}
				argz.filename = argz.argv[i];
				argz.flag = functions_ng::DEBUG;
				filename = true;
			}
			
			// -w | --wordlist to set the wordlist file to use
			else if ( !wordlist && 
				    (!strcmp(argz.argv[i], "-w") || !strcmp(argz.argv[i], "--wordlist")) ) {
				if ( !argz.argv[++i] ) {
					fprintf(stderr, " ! Please gimme a wordlist.\n");
					return false;
				}
				if ( !functions_ng::file_exists(argz.argv[i]) ) {
					fprintf(stderr, " ! Could not open the wordlist.\n");
					return false;
				}
				argz.input = argz.argv[i];
				wordlist = true;
			}
		}
	}
	if ( argz.filename.empty() ) {
		fprintf(stderr, " ! Please gimme a file.\n");
		return false;
	} else if ( argz.input.empty() ) {
		argz.input = "STDIN";
	}
	return true;
}

std::string basename(const std::string& str) {
	size_t pos = str.find_last_of("/");
	return str.substr(++pos);
}

bool file_exists(const char* filename) {
	FILE *test = NULL;
	
	if ( filename != NULL ) {
		test = fopen(filename, "r");
		if ( test != NULL ) {
			fclose(test);
			return true;
		}
	}
	return false;
}

std::string format_number(const size_t& num) {
	std::stringstream str, format;
	unsigned int i, len;

	str << num;
	len = str.str().size();
	i = len + 1;
	while ( --i ) {
		if ( i < len && i % 3 == 0 ) {
			format << ',';
		}
		format << str.str()[len - i];
	}
	return format.str();
}

unsigned int get_cores() {
	unsigned int n = 0;
	char *buf = new char[256];
	FILE *f = fopen("/proc/cpuinfo", "r");

	if ( f != NULL ) {
		for ( ; !feof(f) ; ) {
			memset(buf, 0, sizeof(buf));
			if ( fgets(buf, sizeof(buf), f) == NULL ) {
				break;
			}
			if ( std::string(buf).find("processor") != std::string::npos ) {
				++n;
			}
		}
		fclose(f);
	}
	delete[] buf;
	return n == 0 ? 1 : n;
}

void help(const std::string& module) {
	printf(
		"Copyright (C) 2011-2013 by Mickaël 'Tiger-222' Schoentgen.\n\n"
		"Cracker-ng comes with ABSOLUTELY NO WARRANTY.\n"
		"This is free software, and you are welcome to redistribute it under\n"
		"certain conditions. See the GNU General Public Licence for details.\n\n"
		"Cracker-ng, a multiple file cracker.\n\n");
	functions_ng::usage(module);
	printf(
		"\nAvailable options:\n"
		"    -f, --file     file to crack\n"
		"    -i, --infos    print informations about a file to crack (only for ZIP)\n"
		"    -,  --stdin    read from STDIN\n"
		"    -w, --wordlist dictionnary tu use\n"
		"    -h, --help     display this message\n"
		"    -v, --version  display module version\n\n"
		"Do not hesitate to contact me at <tiger-222@matriux.com> for critics,\n"
		"suggestions, contributions (or whatever you want!).\n");
}

void result(const std::string& password) {
	if ( password.empty() ) {
		printf(" ! Password not found.\n");
	} else {
		const char *p = password.c_str();
		printf(" + Password found: %s\n", p);
		printf("   HEXA[ ");
		for ( size_t i = 0; i < strlen(p); ++i ) {
			printf("%02X ", p[i] & 0xff);
		}
		printf("]\n");
	}
}

void *stats(void* argz) {
	functions_ng::statistics *s = (functions_ng::statistics *)argz;
	Stats statistics(s->num, s->found);
	statistics.start();
	return NULL;
}

std::string substr(const std::string& str, unsigned int max, bool middle) {
    size_t len = str.length();
    bool need_sub = len > max;
    
    if ( !need_sub ) {
        return str;
    }
    if ( middle) {
    	// File: azert...yuiop
	    unsigned int shift = (len - max) / 2;
	    unsigned int part1 = len / 2 - shift - 2;
	    unsigned int part2 = len / 2 + shift + 1;
	    return str.substr(0, part1) + std::string("...") + str.substr(part2);
	} else {
    	// File: ...azertyuiop
	    unsigned int part  = len - max + 3;
	    return std::string("...") + str.substr(part);
	}
}

void usage(const std::string& module) {
	std::string mo = module;
	transform(mo.begin(), mo.end(), mo.begin(), ::tolower);
	printf(
		"Usage:\n"
		"\t%scracker-ng -f <file> [-w <wordlist>]\n"
		"\tgenerator | %scracker-ng -f <file> -\n"
		"Where generator could be cat, crunch, john, jot or whatever you want.\n",
		mo.c_str(), mo.c_str());
}

void version(const std::string& module, const std::string& version) {
	printf("%s Cracker-ng version %s.\n", module.c_str(), version.c_str());
}

}
