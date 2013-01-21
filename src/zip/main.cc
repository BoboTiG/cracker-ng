
/*!
 * \file main.cc
 * \brief ZIP module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.21
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#include "./main.h"


// C'est parti mon kiki !
int main(int argc, char *argv[]) {
	std::string filename, input;
	size_t flag = functions_ng::NONE;
	functions_ng::arguments argz = {
		MODULE, std::string(VERSION), filename, input, flag, (size_t)argc, argv
	};

	if ( !functions_ng::argz_traitment(argz) ) {
		return 0;
	}
	
	// Signal handler
	signal(SIGABRT, &signal_handler);
	signal(SIGTERM, &signal_handler);
	signal(SIGINT, &signal_handler);

	// Who I am? I'm a champion!
	Cracker zizi(argz.filename, argz.input);

	size_t str_len = WIDTH - 2;
	char* str = new char[str_len];
	snprintf(str, str_len, "~ %s Cracker-ng v%s { Tiger-222 } ~", MODULE, VERSION);
	zizi.set_title(str);
	snprintf(str, str_len, "%s", functions_ng::basename(argz.filename).c_str());
	zizi.set_file(str);
	snprintf(str, str_len, "%s", functions_ng::basename(argz.input).c_str());
	zizi.set_generator(str);
	delete[] str;

	zizi.set_debug(argz.flag == functions_ng::DEBUG);
	if ( !zizi.is_ok() ) {
		return 1;
	}
	zizi.crack();
	return 0;
}

void signal_handler(int s) {
	UNUSED(s);
	this_is_now_we_fight = false;
}
