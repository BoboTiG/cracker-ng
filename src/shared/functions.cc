
/*!
 * \file functions.cc
 * \brief Cracker-ng (optimized) functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.30
 */


#include "functions.h"


namespace functions_ng {

unsigned int argz_traitment(int argc, char**argv, string module, string version) {
	bool okay = 1;
	if ( argc == 1 ) {
		functions_ng::usage(module);
		okay = 0;
	} else if ( ! strcmp(argv[1], "-h") || ! strcmp(argv[1], "--help") ) {
		functions_ng::help(module);
		okay = 0;
	} else if ( ! strcmp(argv[1], "-v") || ! strcmp(argv[1], "--version") ) {
		functions_ng::version(module, version);
		okay = 0;
	}
	return okay;
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
