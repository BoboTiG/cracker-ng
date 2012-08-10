
/*!
 * \file functions.cc
 * \brief Cracker-ng (optimized) functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.10
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

void help(const string module) {
	cout
		<< "Copyright (C) 2011-2012 by Mickaël 'Tiger-222' Schoentgen." << endl << endl
		<< "Cracker-ng comes with ABSOLUTELY NO WARRANTY." << endl
		<< "This is free software, and you are welcome to redistribute it under" << endl
		<< "certain conditions. See the GNU General Public Licence for details." << endl << endl
		<< "Cracker-ng, a multiple file cracker." << endl << endl;
	functions_ng::usage(module);
	cout
		<< endl << "<file> could be an option like:" << endl
		<< "    -h, --help     display this message" << endl 
		<< "    -v, --version  display module version" << endl << endl
		<< "Do not hesitate to contact me at <tiger-222@matriux.com> for critics," << endl
		<< "suggestions, contributions (or whatever you want!)." << endl;
}

void result(const string password) {
	if ( password.empty() ) {
		cout << " ! Password not found." << endl;
	} else {
		cout << " + Password found: " << password << endl;
	}
}

void *stats(void *argz) {
	functions_ng::statistics *s = (functions_ng::statistics *)argz;
	Stats statistics(s->num, s->total, s->found);
	statistics.start();
	pthread_exit(NULL);
}

void usage(string module) {
	transform(module.begin(), module.end(), module.begin(), ::tolower);
	cout 
		<< "Usage: generator | " << module << "cracker-ng <file>" << endl
		<< " Where generator could be cat, crunch, john, jot or whatever you want."
		<< endl;
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
