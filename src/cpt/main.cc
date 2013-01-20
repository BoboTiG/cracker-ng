
/*!
 * \file main.cc
 * \brief ccrypt module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.18
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./main.h"

// C'est parti mon kiki !
Cracker::Cracker(const std::string& filename, const std::string& from) :
	filename(filename), from(from),
	filei(filename.c_str(), std::ios::in | std::ios::binary
) {
	printf(" . Working ...\n");
}

Cracker::~Cracker() {
	printf(" ^ Ex(c)iting.\n\n");
}

void Cracker::crack() {
	std::string chosen_one;
	size_t num = 0;
	unsigned int found = 0;
	functions_ng::statistics s = { &num, &found };
	pthread_t stat;
	ccrypt_stream_s *b = new ccrypt_stream_s;
	ccrypt_state_s *st = new ccrypt_state_s;
	roundkey *rkks     = new roundkey;
	roundkey rkk_hash;
	char *encryption_header = new char[32];
	char *password          = new char[PWD_MAX];
	char *inbuf             = new char[32];
	FILE *input             = NULL;

	// Read encrypted data
	this->filei.seekg(0, std::ios::beg);
	this->filei.read(encryption_header, 32);
	this->filei.close();

	// Initializing
	st->rkks = rkks;

	// Read from input ...
	if ( this->from == "STDIN" ) {
		input = stdin;
	} else {
		input = fopen(this->from.c_str(), "r");
	}

	// Let's go!
	pthread_create(&stat, NULL, functions_ng::stats, reinterpret_cast<void*>(&s));
	while ( functions_ng::read_input(input, password, PWD_MAX) ) {
		ccdecrypt_init(b, st, password, rkk_hash);
		memcpy(inbuf, encryption_header, 32);
		b->next_in = inbuf;
		b->avail_in = 32;
		if ( ccdecrypt(b) == 0 ) {
			chosen_one = password;
			found = 1;
			break;
		}
		++num;
	}
	delete b;                                   b = 0;
	delete st;                                 st = 0;
	delete rkks;                             rkks = 0;
	delete[] inbuf;                         inbuf = 0;
	delete[] password;                   password = 0;
	delete[] encryption_header; encryption_header = 0;
	if ( this->from != "STDIN" ) {
		fclose(input);
	}
	if ( found == 0 ) {
		found = 2;
	}
	pthread_join(stat, reinterpret_cast<void**>(NULL));
	functions_ng::result(chosen_one);
}


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

bool Cracker::is_ok() {
	if ( !filei.is_open() ) {
		fprintf(stderr, " ! I cannot open the file.\n");
		return false;
	}
	return true;
}
