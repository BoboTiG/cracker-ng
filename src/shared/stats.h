
/*!
 * \file stats.h
 * \brief Statistics headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.08.09
 */


#ifndef HEADER_STATS
#define HEADER_STATS

#include <iostream>
#include <cstdio>
#include <ctime>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h> // sleep()

using namespace std;


class Stats {

public:
	size_t *num;
	size_t *total;
	unsigned int *found;
	
	Stats(size_t *, size_t *, unsigned int *);
	Stats(const Stats&);
	Stats & operator=(const Stats&);
	~Stats();
	void start();
	void stats_sumary();

protected:

private:
	time_t start_time;
	unsigned short int sleeping_time;
	char _pad[6];

	unsigned long elapsed_seconds();
	string format_number(const size_t &);

};

#endif // STATS_H
