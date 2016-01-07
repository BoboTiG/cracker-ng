
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 *
 * Copyright (C) 2011-2016 Mickaël 'Tiger-222' Schoentgen.
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
	start_time(0),
	elapsed_time(0)
{}

Stats::~Stats() {}

time_t Stats::elapsed_seconds() {
	return (time(NULL) - this->start_time);
}

void Stats::start() {
	size_t n = 0;
	this->start_time = time(NULL);
	for ( ; *this->s.working ; ) {
		sleep(1);
		n = __sync_fetch_and_and(&*this->s.num, 0);
		this->combinations += n;
		printf("\033[A . Working at %s pwd/sec [%luM tries]\n",
			(functions_ng::format_number(n).c_str()),
			(this->combinations / 1000 / 1000)
		);
	}
	this->elapsed_time = this->elapsed_seconds();
	this->stats_sumary();
}

void Stats::stats_sumary() {
	if ( this->elapsed_time > 0 ) {
		printf("\033[A . Worked at ~ %s pwd/sec for ~ %luM tries.\n",
			(functions_ng::format_number(this->combinations / this->elapsed_time).c_str()),
			(this->combinations / 1000 / 1000)
		);
		std::cout << "   Working time: " << functions_ng::format_number(this->elapsed_time) << " sec" << std::endl;
		std::cout << "   Combinations: " << functions_ng::format_number(this->combinations) << std::endl;
	}
}
