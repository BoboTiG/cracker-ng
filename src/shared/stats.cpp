
/*!
 * \file stats.cpp
 * \brief Statistics functions.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.03.04
 */


#include "stats.hpp"

static unsigned long elapsed_seconds(const time_t & start_time) {
	time_t end_time = current_time();
	return (end_time - start_time);
}

static string format_number(const unsigned long & num) {
	stringstream str, format;
	unsigned int i, len;
	
	str << num;
	len = str.str().size();
	i = len + 1;
	while ( --i ) {
		if ( i < len && i % 3 == 0 ) {
			format << NUMBER_SEP;
		}
		format << str.str()[len - i];
	}
	return format.str();
}

void* stats(void* argz) {
	statistics *s = (statistics *)argz;
	
	while ( true ) {
		#if defined(_WIN32)
			Sleep(SLEEPING_TIME * 1000);
		#else
			sleep(SLEEPING_TIME);
		#endif
		printf("\r . Working at %s pwd/sec [%s tries]",
			format_number(*s->num / SLEEPING_TIME).c_str(),
			format_number(*s->total).c_str());
		*s->num = 0;
	}
	pthread_exit(0);
}

static void stats_sumary(const unsigned long & num, const time_t & start_time) {
	unsigned long the_time = elapsed_seconds(start_time);
	unsigned long pwd;
	
	if ( the_time > 0 ) {
		pwd = num / the_time;
		printf("\n . Worked at %s pwd/sec for %s tries.",
			format_number(pwd).c_str(), format_number(num).c_str());
	}
}

static time_t current_time() {
    return time(NULL);
}
