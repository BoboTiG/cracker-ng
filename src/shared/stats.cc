
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.23
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
		printf("\033[A . Working at %.02fK pwd/sec [%.02fM tries]\n",
			(*this->s.num / 1000.0 / (this->sleeping_time * n)),
			(*this->s.num / 1000.0 / 1000.0)
		);
	}
	this->stats_sumary();
}

void Stats::stats_sumary() {
	time_t the_time = this->elapsed_seconds();
	if ( the_time > 0 ) {
		printf("\033[A . Worked at ~ %.02fK pwd/sec for %.02fM tries.\n",
			(*this->s.num / 1000.0 / the_time),
			(*this->s.num / 1000.0 / 1000.0)
		);
	}
}
