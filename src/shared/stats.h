
/*!
 * \file stats.h
 * \brief Statistics headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.15
 *
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_SHARED_STATS_H_
#define SRC_SHARED_STATS_H_

#include <unistd.h>  // sleep()
#include <cstdio>
#include <string>
#include <cstring>
#include <sstream>


class Stats {
public:
	size_t *num;
	unsigned int *found;

	Stats(size_t *, unsigned int *);
	Stats(const Stats&);
	Stats & operator=(const Stats&);
	~Stats();
	void start();
	void stats_sumary();

protected:
private:
	size_t total;
	time_t start_time;
	unsigned int sleeping_time;
	char _pad[4];

	time_t elapsed_seconds();
	std::string format_number(const size_t &);
};

#endif  // SRC_SHARED_STATS_H_
