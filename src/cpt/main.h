
/*!
 * \file main.h
 * \brief ccrypt Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.15
 *
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_CPT_MAIN_H_
#define SRC_CPT_MAIN_H_

#define MODULE  "CPT"     //!< Module name.
#define VERSION "0.1-4"   //!< Module version.
#define PWD_MAX  32       //!< Maximum password length

#include <pthread.h>
#include <string>
#include <fstream>
#include "../shared/functions.h"
#include "./tables.h"
#include "./rijndael.h"
#include "./ccryptlib.h"


class Cracker {
public:
	std::string filename, from;

	Cracker(std::string, std::string);
	~Cracker();
	void crack();

private:
	std::ifstream filei;
};

#endif  // SRC_CPT_MAIN_H_
