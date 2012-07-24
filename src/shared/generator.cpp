
/*!
 * \file generator.cpp
 * \brief Cracker-ng password generator.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2011.12.28
 */


#include "generator.hpp"


/*inline void recurse(
	const string       & charset, 
	const unsigned int & charset_len, 
	unsigned int       & max_len, 
	unsigned int         position,
	const string       & word
) {
	unsigned int i, l = max_len - 1;
	
	for ( i = 0; i < charset_len; ++i ) {
		cout << word + charset[i] << endl;
		if ( position < l )
			recurse(charset, charset_len, max_len, position + 1, word + charset[i]);
	}
}*/

/*inline void permute(
	const string       & charset, 
	const unsigned int & charset_len, 
	unsigned int         position,
	const unsigned int & max_len,
	unsigned int         start, 
	const unsigned int & end, 
	const string       & word,
	unsigned long      & num
) {
	string password;
	
	for ( register unsigned int i = start; i < charset_len && ! found ; ++i ) {
		if ( position == 0 && i == end ) break;
		num += 1;
		password = word + charset[i];
		//cout << password << endl;
		
		if ( position < max_len - 1 )
			permute(
				charset, 
				charset_len, 
				position + 1, 
				max_len, 
				0,
				end,
				password, 
				num
			);
	}
}*/

class generator {

public:
	// Constructor
	generator() {
		this->num = 0;
		this->need_to_run = true;
	}
	
	// Destructor
	~generator() {}
	
	// Initialize the generator
	void init(int charset, int pos, int start, int max_len) {
		switch ( charset ) {
			case 0 : this->charset = charset_a; break;
			case 1 : this->charset = charset_A; break;
			case 2 : this->charset = charset_0; break;
			case 3 : this->charset = charset__; break;
			case 4 : this->charset = charset_h; break;
			case 5 : this->charset = charset_H; break;
			default: this->charset = charset_a; break;
		}
		this->position    = position;
		this->i           = start;
		this->max_len     = max_len;
		this->charset_len = this->charset.size();
	}
	
	// Store the next word into password scalar
	unsigned int get_word(string & password) {
		this->next();
		password = this->word;
		sleep(1);
		return this->need_to_run;
	}
	
	// Get the total of generated words
	unsigned long get_total() {
		return this->num;
	}


private:
	unsigned int charset_len;
	unsigned int position;
	//unsigned int start;
	unsigned int end;
	unsigned int max_len;
	unsigned int i;
	unsigned long num;
	string charset;
	string word;
	bool need_to_run;
	
	void next() {
		for ( register unsigned int i = start; i < charset_len && ! found ; ++i ) {
		if ( position == 0 && i == end ) break;
		num += 1;
		password = word + charset[i];
		
		++this->i;
		if ( this->i >= this->charset_len )
		++this->num;
		this->word = this->word + this->charset[this->i];
	}
};
