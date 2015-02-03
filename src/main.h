
/*!
 * \file main.h
 * \brief ZIP Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2015.02.03
 *
 * Copyright (C) 2012-2015 Mickaël 'Tiger-222' Schoentgen.
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_


#include <algorithm>  // transform()
#include <csignal>    // SIGABRT, SIGTERM, SIGINT
#include <string>
#include "./shared/functions.h"
#include "./shared/stats.h"
#include "./shared/gui.h"
#include "./shared/cracker.h"
#ifdef CPT
	static const char* MODULE = "CPT";     //!< Module name.
#elif ZIP
	static const char* MODULE = "ZIP";     //!< Module name.
#endif

static const char* VERSION       = "2015.02-03";  //!< Cracker-ng version.


/*!
 * \enum FLAG
 * \brief Diffenret flags you can pass to Cracker class.
 */
enum FLAG {
	NONE  = 0x0,  //!< Nothing special
	DEBUG = 0x1   //!< Debug informations to print about one file
};

/*!
 * \struct arguments
 * \brief ZIP Central Directory structure.
 */
typedef struct {
	std::string   module;     //!< The module's name
	std::string   version;    //!< The module's version
	std::string & filename;   //!< The file name to crack
	std::string & input;      //!< The source of words (STDIN or a wordlist)
	size_t      & flag;       //!< A flag (to set debug)
	size_t        argc;       //!< Number of arguments
	char**        argv;       //!< Array of arguments
	std::string * false_pos;  //!< Array of false positives (8 max)
} arguments;


/*!
 * \fn argz_traitment(const arguments& argz)
 * \brief Arguments traitment.
 * \param argz Reference to a struct arguments.
 * \return \li 0 if \b arguments are valids;
 * \return \li 1 otherwise.
 */
bool argz_traitment(const arguments&);

/*!
 * \fn help(const std::string& module)
 * \brief Print help message.
 * \param module The module's name.
 */
void help(const std::string&);

/*!
 * \fn usage(const std::string& module)
 * \brief Print usage.
 * \param module The module's name.
 */
void usage(const std::string&);

/*!
 * \fn version(const std::string& module, const std::string& version)
 * \brief Print informations about a module.
 * \param module The module's name.
 * \param version The module's version.
 */
void version(const std::string&, const std::string&);

/*!
 * \fn signal_handler(int s)
 * \brief Action to do when CTRL + C raises.
 * \param s Signal number.
 */
void signal_handler(int);

#endif  // SRC_MAIN_H_
