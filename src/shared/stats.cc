
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.27
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./stats.h"


void *stats(void* argz) {
	statistics* s = (statistics*)argz;
	Stats statistics(*s);
	statistics.start();
	return NULL;
}


Stats::Stats(statistics& stats) :
	s(stats),
	sleeping_time(1),
	start_time(time(NULL))
{}

Stats::~Stats() {}

time_t Stats::elapsed_seconds() {
	return (time(NULL) - this->start_time);
}

void Stats::start() {
	size_t n = 1;
	for ( ; *this->s.working ; ++n ) {
		sleep(this->sleeping_time);
		printf("\033[A . Working at %luK pwd/sec [%luM tries]\n",
			(*this->s.num / (this->sleeping_time * n) / 1024),
			(*this->s.num / 1024 / 1024)
		);
	}
	this->stats_sumary();
}

void Stats::stats_sumary() {
	time_t the_time = this->elapsed_seconds();
	if ( the_time > 0 ) {
		printf("\033[A . Worked at ~ %luK pwd/sec for ~ %luM tries.\n",
			(*this->s.num / the_time / 1000),
			(*this->s.num / 1000 / 1000)
		);
	}
}
