
/*!
 * \file main.cc
 * \brief ccrypt module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.21
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./main.h"

// C'est parti mon kiki !
int main(int argc, char *argv[]) {
	std::string filename, input;
	size_t flag;
	functions_ng::arguments argz = {
		MODULE, std::string(VERSION), filename, input, flag, (size_t)argc, argv
	};

	if ( !functions_ng::argz_traitment(argz) ) {
		return 0;
	}
	printf(" ~ %s Cracker-ng v.%s { Tiger-222 }\n", MODULE, VERSION);
	printf(" - Generator.: %s\n", functions_ng::basename(argz.input).c_str());
	printf(" - File......: %s\n", functions_ng::basename(argz.filename).c_str());

	// Who I am? I'm a champion!
	Cracker zizi(argz.filename, argz.input);
	if ( !zizi.is_ok() ) {
		return 1;
	}
	zizi.crack();
	return 0;
}
