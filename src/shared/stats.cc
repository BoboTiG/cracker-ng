
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.11.22
 *
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./stats.h"

Stats::Stats(size_t *num, unsigned int *found)
	: num(num), found(found),
	  total(0), sleeping_time(1), start_time(time(NULL))
{}

Stats::~Stats() {}

time_t Stats::elapsed_seconds() {
	return (time(NULL) - this->start_time);
}

std::string Stats::format_number(const size_t & num) {
	std::stringstream str, format;
	unsigned int i, len;

	str << num;
	len = str.str().size();
	i = len + 1;
	while ( --i ) {
		if ( i < len && i % 3 == 0 ) {
			format << ',';
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
		printf("\033[A . Working at %s pwd/sec [%s tries]\n",
			format_number(n / this->sleeping_time).c_str(),
			format_number(this->total).c_str());
	}
	stats_sumary();
}

void Stats::stats_sumary() {
	time_t the_time = elapsed_seconds();
	if ( the_time > 0 ) {
		size_t pwd = this->total / the_time;
		printf("\033[A . Worked at ~ %s pwd/sec for %s tries.\n",
			format_number(pwd).c_str(),
			format_number(this->total).c_str());
	}
}
