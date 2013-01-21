
/*!
 * \file cracker.h
 * \brief Cracker class headers for CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.21
 *
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_CPT_CRACKER_H_
#define SRC_CPT_CRACKER_H_


#define PWD_MAX  32       //!< Maximum password length

#include <pthread.h>
#include <string>
#include <fstream>
#include "./cracker.h"
#include "./tables.h"
#include "./rijndael.h"
#include "./ccryptlib.h"


class Cracker {
public:
	std::string filename, from;

	Cracker(const std::string&, const std::string&);
	~Cracker();
	void crack();
	bool is_ok();

private:
	std::ifstream filei;
};

#endif  // SRC_CPT_CRACKER_H_
