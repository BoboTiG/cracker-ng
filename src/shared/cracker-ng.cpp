
/*!
 * \file cracker-ng.cpp
 * \brief Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 */


#include "cracker-ng.hpp"


unsigned int argz_traitment(char *argv[]) {
	bool okay = 1;
	
	if ( ! argv[1] ) {
		usage();
		okay = 0;
	} else if ( ! strcmp(argv[1], "-h") || ! strcmp(argv[1], "--help") ) {
		help();
		okay = 0;
	} else if ( ! strcmp(argv[1], "-v") || ! strcmp(argv[1], "--version") ) {
		version();
		okay = 0;
	}
	return okay;
}

void help() {
	cout
		<< "Copyright (C) 2011-2012 by Mickaël 'Tiger-222' Schoentgen." << endl << endl
		<< "Cracker-ng comes with ABSOLUTELY NO WARRANTY." << endl
		<< "This is free software, and you are welcome to redistribute it under" << endl
		<< "certain conditions. See the GNU General Public Licence for details." << endl << endl
		<< "Cracker-ng, a multiple file cracker." << endl << endl;
	usage();
	cout
		<< endl << "<file> could be an option like:" << endl
		<< "    -h, --help     display this message" << endl 
		<< "    -v, --version  display module version" << endl << endl
		<< "Do not hesitate to contact me at <tiger-222@matriux.com> for critics," << endl
		<< "suggestions, contributions (or whatever you want!)." << endl;
}

static inline char *read_stdin(char *buffer) {
	char *result, *lf;
	
	result = fgets(buffer, PWD_MAX, stdin);
	if ( result != NULL ) {
		lf = strchr(buffer, '\n');
		if ( lf != NULL )
			*lf = '\0';
	}
	return result;
}

void result(const string password) {
	cout << endl;
	if ( password.size() > 0 ) {
		cout << " + Password found: " << password << endl;
	} else {
		cout << " ! Password not found." << endl;
	}
}

void usage() {
	string module = MODULE;
	
	transform(module.begin(), module.end(), module.begin(), ::tolower);
	cout 
		<< "Usage: generator | " << module << "cracker-ng <file>" << endl
		<< " Where generator could be cat, crunch, john, jot or whatever you want."
		<< endl;
}

void version() {
	printf("%s Cracker-ng version %s.\n", MODULE, VERSION);
	#if defined(ZIP)
		#include <boost/version.hpp> 
		unsigned int sm = BOOST_VERSION % 100;
		unsigned int m  = (BOOST_VERSION / 100) % 1000;
		unsigned int M  = BOOST_VERSION / 100000;
		printf("I use Boost C++ libraries version %d.%d.%d.\n", M, m, sm);
	#endif
}
