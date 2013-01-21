
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.20
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
		printf("\033[A . Working at %.02fK pwd/sec [%.02fM tries]\n",
			(n / 1000.0 / this->sleeping_time),
			(this->total / 1000.0 / 1000.0)
		);
	}
	stats_sumary();
}

void Stats::stats_sumary() {
	time_t the_time = elapsed_seconds();
	if ( the_time > 0 ) {
		printf("\033[A . Worked at ~ %.02fK pwd/sec for %.02fM tries.\n",
			(this->total / 1000.0 / the_time),
			(this->total / 1000.0 / 1000.0)
		);
	}
}
