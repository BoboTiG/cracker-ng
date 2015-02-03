
/*!
 * \file stats.h
 * \brief Statistics headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2015.02.03
 *
 * Copyright (C) 2012-2015 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_SHARED_STATS_H_
#define SRC_SHARED_STATS_H_

#include <unistd.h>  // sleep()
#include "./functions.h"


typedef struct {
	size_t* num;
	bool* working;
} statistics;

void *stats(void*);


class Stats {
public:
	statistics& s;

	Stats(statistics&);
	~Stats();
	void start();
	void stats_sumary();

protected:
private:
	size_t sleeping_time;
	time_t start_time;
	time_t elapsed_seconds();
};

#endif  // SRC_SHARED_STATS_H_
