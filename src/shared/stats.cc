
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 *
 * Copyright (C) 2011-2021 Mickaël 'Tiger-222' Schoentgen.
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
	combinations(0),
	start_time(0)
{}

Stats::~Stats() {}

void Stats::start() {
	size_t n = 0;

	this->start_time = time(NULL);
	for ( ; *this->s.working ; ) {
		sleep(1);
		n = __sync_fetch_and_and(&*this->s.num, 0);
		this->combinations += n;
		std::cout << "\033[A . Working at " << functions_ng::format_number(n) << " pwd/sec          " << std::endl;
	}
	this->stats_sumary();
}

void Stats::stats_sumary() {
	time_t elapsed_time = time(NULL) - this->start_time;

	if ( elapsed_time > 0 ) {
		std::cout
			<< "\033[A . Worked at " << functions_ng::format_number(this->combinations / elapsed_time) << " pwd/sec          " << std::endl
			<< "   Combinations: " << functions_ng::format_number(this->combinations) << std::endl
			<< "   Working time: " << functions_ng::format_number(elapsed_time) << " sec" << std::endl;
	}
}
