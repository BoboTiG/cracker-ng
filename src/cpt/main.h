
/*!
 * \file main.h
 * \brief ccrypt Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.13
 */


#ifndef MAIN_H
#define MAIN_H

#define MODULE  "CPT"     //!< Module name.
#define VERSION "0.1-3"   //!< Module version.
#define PWD_MAX  32       //!< Maximum password length

#include <pthread.h>
#include "../shared/functions.h"
#include "tables.h"
#include "rijndael.h"
#include "ccryptlib.h"


class Cracker {

public:
	string filename, from;
	
	Cracker(string, string);
	~Cracker();
	void crack();

protected:

private:
	ifstream filei;

};

#endif // MAIN_H
