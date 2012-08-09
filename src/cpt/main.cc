
/*!
 * \file main.cc
 * \brief ccrypt module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.09
 */


#include "main.h"

// C'est parti mon kiki !
Cracker::Cracker(ifstream & filei) : filei(filei) {
	cout << " . Working ..." << endl;
}

Cracker::~Cracker() {
	cout << " ^ Ex(c)iting." << endl;
}

void Cracker::crack() {
	char *password, *encryption_header = new char[PWD_MAX];
	char buffer[PWD_MAX], inbuf[PWD_MAX];
	ccrypt_stream_s *b;
	ccrypt_state_s *st;
	roundkey *rkks;
	string chosen_one;
	size_t num = 0, total = 0;
	unsigned int found = 0;
	functions::statistics s = { &num, &total, &found };
	pthread_t stat;
	
	// Read encrypted data
	this->filei.seekg(0, ios::beg);
	this->filei.read(encryption_header, PWD_MAX);
	this->filei.close();
	
	// Initializing
	b = (ccrypt_stream_s*)malloc(sizeof(ccrypt_stream_s));
	rkks = (roundkey*)malloc(sizeof(roundkey));
	st = (ccrypt_state_s*)malloc(sizeof(ccrypt_state_s));
	st->rkks = rkks;
	
	// Let's go!
	pthread_create(&stat, NULL, functions::stats, (void*)&s);
	pthread_detach(stat);
	while ( (password = functions::read_stdin(buffer, PWD_MAX)) != NULL ) {
		ccdecrypt_init(b, st, password);
		memcpy(inbuf, encryption_header, PWD_MAX);
		b->next_in = inbuf;
		b->avail_in = PWD_MAX;
		if ( ! ccdecrypt(b) ) {
			chosen_one = password;
			found = 1;
			break;
		}
		++num;
		++total;
	}
	delete[] encryption_header;
	delete[] b;
	delete[] rkks;
	delete[] st;
	if ( found == 0 ) {
		found = 2;
	}
	pthread_join(stat, (void**)NULL);
	functions::result(chosen_one);
}


int main(int argc, char *argv[]) {
	if ( ! functions::argz_traitment(argc, argv, MODULE, VERSION) ) {
		return 0;
	}
	printf(" ~ %s Cracker-ng v.%s { Tiger-222 }\n", MODULE, VERSION);
	printf(" - File: %s\n", argv[1]);
	
	ifstream filei(argv[1], ios::in | ios::binary);
	if ( ! filei.is_open() ) {
		cerr << " ! Could not open the file." << endl;
		return 1;
	}
	
	// Who I am? I'm a champion!
	Cracker zizi(filei);
	zizi.crack();
	return 0;
}
