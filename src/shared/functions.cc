
/*!
 * \file functions.cc
 * \brief Cracker-ng (optimized) functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.13
 */


#include "functions.h"


namespace functions_ng {

unsigned int argz_traitment(arguments &argz) {
	if ( argz.argc == 1 ) {
		functions_ng::usage(argz.module);
		return 0;
	} else if ( ! strcmp(argz.argv[1], "-h") || ! strcmp(argz.argv[1], "--help") ) {
		functions_ng::help(argz.module);
		return 0;
	} else if ( ! strcmp(argz.argv[1], "-v") || ! strcmp(argz.argv[1], "--version") ) {
		functions_ng::version(argz.module, argz.version);
		return 0;
	} else {
		int i;
		for ( i = 1; i < argz.argc; ++i ) {
			if ( ! strcmp(argz.argv[i], "-f") || ! strcmp(argz.argv[i], "--file") ) {
				if ( argz.argv[++i] ) {
					ifstream filei(argz.argv[i], ios::in | ios::binary);
					if ( filei.is_open() ) {
						filei.close();
						argz.filename = argz.argv[i];
					} else {
						cerr << " ! Could not open the file." << endl;
						return 0;
					}
				} else {
					cerr << " ! Please gimme a file." << endl;
					return 0;
				}
			} else if ( ! strcmp(argz.argv[i], "-w") || ! strcmp(argz.argv[i], "--wordlist") ) {
				if ( argz.argv[++i] ) {
					if ( ! strcmp(argz.argv[i], "-") ) {
						argz.input = "stdin";
					} else {
						ifstream in(argz.argv[i]);
						if ( in.is_open() ) {
							in.close();
							argz.input = argz.argv[i];
						} else {
							cerr << " ! Could not open the wordlist." << endl;
							return 0;
						}
					}
				}
			}
		}
	}
	if ( argz.filename.length() < 1 ) {
		cerr << " ! Please gimme a file." << endl;
		return 0;
	} else if ( argz.input.length() < 1 ) {
		argz.input = "stdin";
	}
	return 1;
}

unsigned int get_cores() {
	unsigned int n = 0;
	char buf[256];

	FILE *f = fopen("/proc/cpuinfo", "r");
	if ( f ) {
		for ( ; ! feof(f) ; ) {
			memset(buf, 0, sizeof(buf));
			if ( fgets(buf, sizeof(buf), f) == NULL ) {
				break;
			}
			if ( string(buf).find("processor") != string::npos ) {
				++n;
			}
		}
		fclose(f);
	}
	return n == 0 ? 1 : n;
}

void help(const string module) {
	cout
		<< "Copyright (C) 2011-2012 by Mickaël 'Tiger-222' Schoentgen.\n\n"
		<< "Cracker-ng comes with ABSOLUTELY NO WARRANTY.\n"
		<< "This is free software, and you are welcome to redistribute it under\n"
		<< "certain conditions. See the GNU General Public Licence for details.\n\n"
		<< "Cracker-ng, a multiple file cracker.\n\n";
	functions_ng::usage(module);
	cout
		<< "\n<file> could be an option like:\n"
		<< "    -h, --help     display this message\n"
		<< "    -v, --version  display module version\n\n"
		<< "Do not hesitate to contact me at <tiger-222@matriux.com> for critics,\n"
		<< "suggestions, contributions (or whatever you want!).\n";
}

void result(const string password) {
	if ( password.empty() ) {
		cout << " ! Password not found.\n";
	} else {
		cout << " + Password found: " << password << endl;
	}
}

void *stats(void *argz) {
	functions_ng::statistics *s = (functions_ng::statistics *)argz;
	Stats statistics(s->num, s->found);
	statistics.start();
	pthread_exit(NULL);
}

void usage(string module) {
	transform(module.begin(), module.end(), module.begin(), ::tolower);
	cout 
		<< "Usage: generator | " << module << "cracker-ng <file>\n"
		<< " Where generator could be cat, crunch, john, jot or whatever you want.\n";
}

void version(const string module, const string version) {
	printf("%s Cracker-ng version %s.\n", module.c_str(), version.c_str());
	#if defined(ZIP)
		#include <boost/version.hpp> 
		unsigned int sm = BOOST_VERSION % 100;
		unsigned int m  = (BOOST_VERSION / 100) % 1000;
		unsigned int M  = BOOST_VERSION / 100000;
		printf("I use Boost C++ libraries version %d.%d.%d.\n", M, m, sm);
	#endif
}

}
