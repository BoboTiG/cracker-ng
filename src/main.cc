
/*!
 * \file main.cc
 * \brief ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 *
 * Copyright (C) 2011-2021 Mickaël 'Tiger-222' Schoentgen.
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#include "./main.h"


// C'est parti mon kiki !
int main(int argc, char *argv[]) {
	std::string filename, input, password;
	std::string* false_pos = new std::string[8];
	size_t flag = NONE;
	arguments argz = {
		MODULE, std::string(VERSION), filename, input, flag, (size_t)argc, argv, false_pos
	};

	if ( !argz_traitment(argz) ) {
		delete[] false_pos;
		return 0;
	}

	// Signal handler
	/*signal(SIGABRT, &signal_handler);
	signal(SIGTERM, &signal_handler);
	signal(SIGINT,  &signal_handler);*/

	// Who I am? I'm a champion!
	Cracker zizi(argz.filename, argz.input);

	size_t str_len = WIDTH - 2;
	char* str = new char[str_len];
	snprintf(str, str_len, "~ %s Cracker-ng v%s ~", MODULE, VERSION);
	zizi.set_title(str);
	snprintf(str, str_len, "%s", functions_ng::basename(argz.filename).c_str());
	zizi.set_file(str);
	snprintf(str, str_len, "%s", functions_ng::basename(argz.input).c_str());
	zizi.set_generator(str);
	delete[] str;

	// Set false positives
	size_t i, n = 0;
	for ( i = 0; i < 8; ++i ) {
		if ( !false_pos[i].empty() ) {
			zizi.set_false_pos(false_pos[i], n);
			++n;
		}
	}
	delete[] false_pos;

	zizi.set_debug(argz.flag == DEBUG);
	if ( !zizi.is_ok() ) {
		return 1;
	}
	zizi.crack();
	return 0;
}

bool argz_traitment(const arguments& argz) {
	if ( argz.argc == 1 ) {
		usage(argz.module);
		return false;
	}
	std::string arg = std::string(argz.argv[1]);
	if ( arg == "-h" || arg == "--help" ) {
		help(argz.module);
		return false;
	}
	if ( arg == "-v" || arg == "--version" ) {
		version(argz.module, argz.version);
		return false;
	}

	size_t i, fp = 0;
	for ( i = 1; i < argz.argc; ++i )
	{
		arg = std::string(argz.argv[i]);
		// -f | --file to set the file to crack
		if ( argz.filename.empty() && (arg == "-f" || arg == "--file") ) {
			if ( !argz.argv[++i] ) {
				std::cerr << " ! Please gimme a file." << std::endl;
				return false;
			}
			if ( !functions_ng::file_exists(argz.argv[i]) ) {
				std::cerr << " ! Could not open the file." << std::endl;
				return false;
			}
			argz.filename = argz.argv[i];
		}
		// -fp | --false-pos to set up to 8 false positives
		if ( arg == "-fp" || arg == "--false-pos" ) {
			if ( argz.argv[++i] && fp++ <= 8 ) {
				argz.false_pos[fp] = std::string(argz.argv[i]);
				++fp;
			}
		}
		// -i | --infos to get informations about a file
		// Useful for the ZIP module, for others it will just do as -f | --file option
		else if ( argz.filename.empty() && (arg == "-i" || arg == "--infos") ) {
			if ( !argz.argv[++i] ) {
				std::cerr << " ! Please gimme a file." << std::endl;
				return false;
			}
			if ( !functions_ng::file_exists(argz.argv[i]) ) {
				std::cerr << " ! Could not open the file." << std::endl;
				return false;
			}
			argz.filename = argz.argv[i];
			argz.flag = DEBUG;
		}
		// -w | --wordlist to set the wordlist file to use
		else if ( argz.input.empty() && (arg == "-w" || arg == "--wordlist") ) {
			if ( !argz.argv[++i] ) {
				std::cerr << " ! Please gimme a wordlist." << std::endl;
				return false;
			}
			if ( !functions_ng::file_exists(argz.argv[i]) ) {
				std::cerr << " ! Could not open the wordlist." << std::endl;
				return false;
			}
			argz.input = argz.argv[i];
		}
	}
	if ( argz.filename.empty() ) {
		std::cerr << " ! Please gimme a file." << std::endl;
		return false;
	} else if ( argz.input.empty() ) {
		argz.input = "STDIN";
	}
	return true;
}

void help(const std::string& module) {
	std::cout <<
		"Copyright (C) 2011-2021 by Mickaël 'Tiger-222' Schoentgen.\n\n"
		"Cracker-ng comes with ABSOLUTELY NO WARRANTY.\n"
		"This is free software, and you are welcome to redistribute it under\n"
		"certain conditions. See the GNU General Public Licence for details.\n\n"
		"Cracker-ng, a multiple file cracker.\n\n";
	usage(module);
	std::cout <<
		"\nAvailable options:\n"
		"    -f,  --file       file to crack\n"
		"    -,   --stdin      read from STDIN\n"
		"    -w,  --wordlist   dictionary to use\n"
		"    -h,  --help       display this message\n"
		"    -v,  --version    display module version\n"
		"\nZIP specific options:\n"
		"    -fp, --false-pos  set one false positive (accumulate up to 8)\n"
		"                      ex: -fp word1 -fp word2 ...\n"
		"    -i,  --infos      print informations about a file to crack\n";
}

void signal_handler(int s) {
	UNUSED(s);
	this_is_now_we_fight = false;
}

void usage(const std::string& module) {
	std::string mo = module;
	transform(mo.begin(), mo.end(), mo.begin(), ::tolower);
	std::cout <<
		"Usage:\n"
		"\t" << mo << "cracker-ng -f <file> [-w <wordlist>] [-fp <word>...]\n"
		"\tgenerator | " << mo << "cracker-ng -f <file> -\n"
		"Where generator could be cat, crunch, john, jot or whatever you want.\n";
}

void version(const std::string& module, const std::string& version) {
	std::cout << module << " Cracker-ng version " << version << std::endl;
}
