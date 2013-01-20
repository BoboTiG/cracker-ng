
/*!
 * \file stats.h
 * \brief Statistics headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.03
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_SHARED_STATS_H_
#define SRC_SHARED_STATS_H_

#include <unistd.h>  // sleep()
#include <string>
#include <sstream>
#include "./functions.h"


class Stats {
public:
	size_t*       num;
	unsigned int* found;

	Stats(size_t*, unsigned int*);
	Stats(const Stats&);
	Stats& operator=(const Stats&);
	~Stats();
	void start();
	void stats_sumary();

protected:
private:
	size_t total;
	size_t sleeping_time;
	time_t start_time;

	time_t elapsed_seconds();
};

#endif  // SRC_SHARED_STATS_H_
