
/*!
 * \file cptcracker-ng.cpp
 * \brief ccrypt module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.30
 */


#include "cptcracker-ng.hpp"

// C'est parti mon kiki !
int main(int argc, char *argv[]) {
	
	if ( argc && ! argz_traitment(argv) ) {
		return 0;
	}
	
	cout << " ~ ZIP Cracker-ng v." << VERSION << " { Tiger-222 }" << endl;
	
	// 0x00. Variables -------------------------------------------------
	ifstream filei;
	
	// 0x01. Open the file ---------------------------------------------
	cout << " - File: " << argv[1] << endl;
	filei.open(argv[1], ios::in | ios::binary);
	if ( ! filei.is_open() ) {
		cout << " ! Could not open the file." << endl;
		return 1;
	}
	
	// 0x02. Who I am? I'm a champion! Crack it! -----------------------
	cout << " . Working ...";
	ccrypt_crack(filei);
	
	// 0x03. Ex(c)iting ------------------------------------------------
	cout << " ^ Ex(c)iting." << endl;
	if ( filei.is_open() ) {
		filei.close();
	}
	return 0;
}

void ccrypt_crack(ifstream & filei) {
	string chosen_one;
	char *password;
	char *encryption_header = new char[PWD_MAX];
	ccrypt_stream_s *b;
	char buffer[PWD_MAX], inbuf[PWD_MAX], outbuf[PWD_MAX];
	
	// Stats
	#if (!defined(DEBUG) && defined(USE_STATS))
		unsigned int num = 0;
		unsigned long total = 0;
		pthread_t stat;
		statistics s;
		time_t start_time = current_time();
		
		s.num         = &num;
		s.total       = &total;
		s.start_time  = &start_time;
		pthread_create(&stat, NULL, stats, (void*)&s);
	#endif
	
	filei.seekg(0, ios::beg);
	filei.read(encryption_header, PWD_MAX);
	filei.close();
	
	// Init
	b = (ccrypt_stream_s*)malloc(sizeof(ccrypt_stream_s));
	
	while ( (password = read_stdin(buffer)) != NULL ) {
		ccdecrypt_init(b, password);
		/* fill input buffer */
		memcpy(inbuf, encryption_header, PWD_MAX);
		b->next_in = inbuf;
		b->next_out = outbuf;
		b->avail_in = b->avail_out = PWD_MAX;
		
		/* try to decrypt */
		if ( ccdecrypt(b) == 0 ) {
			chosen_one = password;
			break;
		}
		
		#if (!defined(DEBUG) && defined(USE_STATS))
			++num;
			++total;
		#endif
	}
	
	// Free
	delete[] encryption_header;
	delete[] b;
	
	// Statistics and result
	#if (!defined(DEBUG) && defined(USE_STATS))
		#if !defined(_WIN32)
			pthread_cancel(stat);
		#endif
		stats_sumary(total, start_time);
	#endif
	result(chosen_one);
}
