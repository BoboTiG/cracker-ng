
/*!
 * \file main.cc
 * \brief ccrypt module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.15
 *
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./main.h"

// C'est parti mon kiki !
Cracker::Cracker(std::string filename, std::string from) :
	filename(filename), from(from),
	filei(filename.c_str(), std::ios::in | std::ios::binary
) {
	printf(" . Working ...\n");
}

Cracker::~Cracker() {
	printf(" ^ Ex(c)iting.\n");
}

void Cracker::crack() {
	char *p, *encryption_header = new char[32];
	char buffer[32], inbuf[32];
	FILE *input;
	ccrypt_stream_s *b;
	ccrypt_state_s *st;
	roundkey *rkks;
	roundkey rkk_hash;
	std::string chosen_one;
	size_t num = 0;
	unsigned int found = 0;
	functions_ng::statistics s = { &num, &found };
	pthread_t stat;

	// Read encrypted data
	this->filei.seekg(0, std::ios::beg);
	this->filei.read(encryption_header, 32);
	this->filei.close();

	// Initializing
	b = reinterpret_cast<ccrypt_stream_s*>(malloc(sizeof(ccrypt_stream_s)));
	st = reinterpret_cast<ccrypt_state_s*>(malloc(sizeof(ccrypt_state_s)));
	rkks = reinterpret_cast<roundkey*>(malloc(sizeof(roundkey)));
	st->rkks = rkks;

	// Read from input ...
	if ( this->from == "stdin" ) {
		input = stdin;
	} else {
		input = fopen(this->from.c_str(), "r");
	}

	// Let's go!
	pthread_create(&stat, NULL, functions_ng::stats, reinterpret_cast<void*>(&s));
	while ( (p = functions_ng::read_stdin(buffer, PWD_MAX, input)) != NULL ) {
		ccdecrypt_init(b, st, p, rkk_hash);
		memcpy(inbuf, encryption_header, 32);
		b->next_in = inbuf;
		b->avail_in = 32;
		if ( ccdecrypt(b) == 0 ) {
			chosen_one = p;
			found = 1;
			break;
		}
		++num;
	}
	delete[] encryption_header;
	free(b);
	free(st);
	free(rkks);
	if ( this->from != "stdin" ) {
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
	functions_ng::arguments argz = {
		MODULE, std::string(VERSION), filename, input, argc, {0}, argv
	};

	if ( !functions_ng::argz_traitment(argz) ) {
		return 0;
	}
	printf(" ~ %s Cracker-ng v.%s { Tiger-222 }\n", MODULE, VERSION);
	printf(" - File......: %s\n", argz.filename.c_str());
	printf(" - Input.....: %s\n", argz.input.c_str());

	// Who I am? I'm a champion!
	Cracker zizi(argz.filename, argz.input);
	zizi.crack();
	return 0;
}
