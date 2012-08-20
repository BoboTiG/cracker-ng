
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.13
 */


#include "stats.h"

Stats::Stats(size_t *num, unsigned int *found)
	: num(num), found(found), 
	  total(0), start_time(time(NULL)), sleeping_time(4)
{}

Stats::~Stats() {}

unsigned long Stats::elapsed_seconds() {
	return (time(NULL) - this->start_time);
}

string Stats::format_number(const size_t & num) {
	stringstream str, format;
	unsigned int i, len;
	
	str << num;
	len = str.str().size();
	i = len + 1;
	while ( --i ) {
		if ( i < len && i % 3 == 0 ) {
			format << ",";
		}
		format << str.str()[len - i];
	}
	return format.str();
}

void Stats::start() {
	for ( ; *this->found == 0 ; ) {
		sleep(this->sleeping_time);
		size_t n = *this->num;
		*this->num = 0;
		this->total += n;
		cout << "\033[A"
			 << " . Working at "
			 << format_number(n / this->sleeping_time).c_str()
			 << " pwd/sec ["
			 << format_number(this->total).c_str()
			 << " tries]" << endl;
	}
	stats_sumary();
}

void Stats::stats_sumary() {
	long unsigned int the_time = elapsed_seconds();
	if ( the_time > 0 ) {
		size_t pwd = this->total / the_time;
		cout << "\033[A . Worked at ~ "
			 << format_number(pwd).c_str()
			 << " pwd/sec for "
			 << format_number(this->total).c_str()
			 << " tries." << endl;
	}
}
