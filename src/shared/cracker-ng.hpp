
/*!
 * \file cracker-ng.hpp
 * \brief Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 */


#include <cstring> // strcmp()
#include <algorithm> // transform()
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <pthread.h>
#include <stdint.h>
using namespace std;

// Shared --------------------------------------------------------------
// Stats
#if (!defined(DEBUG) && defined(USE_STATS))
	#if defined(_WIN32)
		#include <windows.h> // Sleep()
	#else
		#include <unistd.h> // sleep()
	#endif
	#include "stats.cpp"
#endif
#include "functions-ng.cpp" // optimized native functions

// Password generator
//#include "generator.cpp"

// Avoid few warnings
#ifdef DEBUG
	#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif


// Prototypes ---------------------------------------------------------- 
/*!
 * \fn unsigned int argz_traitment(char *argv[])
 * \param argv Array containing arguments.
 */
unsigned int argz_traitment(const char *argv[]);

/*!
 * \fn help()
 * \brief Print help.
 */
void help();

/*!
 * \fn static inline char *read_stdin(char *buffer)
 * \brief Retrieve one line from STDIN.
 * \param buffer The buffer.
 */
static inline char *read_stdin(char *buffer);

/*!
 * \fn result(const string password)
 * \brief Print results.
 * \param password The password found (or not!).
 */
void result(const string password);

/*!
 * \fn usage()
 * \brief Print usage.
 */
void usage();

/*!
 * \fn version()
 * \brief Print version.
 */
void version();

