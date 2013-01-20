
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.03
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./stats.h"

Stats::Stats(size_t* num, unsigned int* found)
	: num(num), found(found),
	  total(0), sleeping_time(1), start_time(time(NULL))
{}

Stats::~Stats() {}

time_t Stats::elapsed_seconds() {
	return (time(NULL) - this->start_time);
}

void Stats::start() {
	for ( ; *this->found == 0 ; ) {
		sleep(this->sleeping_time);
		size_t n = *this->num;
		*this->num = 0;
		this->total += n;
		printf("\033[A . Working at %s pwd/sec [%s tries]\n",
			functions_ng::format_number(n / this->sleeping_time).c_str(),
			functions_ng::format_number(this->total).c_str());
	}
	stats_sumary();
}

void Stats::stats_sumary() {
	time_t the_time = elapsed_seconds();
	if ( the_time > 0 ) {
		size_t pwd = this->total / the_time;
		printf("\033[A . Worked at ~ %s pwd/sec for %s tries.\n",
			functions_ng::format_number(pwd).c_str(),
			functions_ng::format_number(this->total).c_str());
	}
}
