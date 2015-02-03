
/*!
 * \file functions.h
 * \brief Cracker-ng (optimized) functions headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2015.02.03
 *
 * Copyright (C) 2012-2015 Mickaël 'Tiger-222' Schoentgen.
 */


#ifndef SRC_SHARED_FUNCTIONS_H_
#define SRC_SHARED_FUNCTIONS_H_

#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>   // fopen()
#include <cstring>  // memset()

#define UNUSED(s) (void)s


namespace functions_ng {

/*!
 * \fn basename(const std::string& str)
 * \brief Return the basename of filepath.
 * \param str the filepath.
 * \return The basename.
 */
const std::string basename(const std::string&);

/*!
 * \fn file_exists(const char* file)
 * \brief Check the existence of a given file.
 * \param file The file to check.
 * \return \li 0 if \b exists;
 * \return \li 1 otherwise.
 */
bool file_exists(const char*);

/*!
 * \fn format_number(const size_t& num)
 * \brief Format a number, i.e: 123456789 => 123,456,789.
 * \param num The numbrer to format.
 * \return The formatted number.
 */
const std::string format_number(const size_t&);

/*!
 * \fn
 * \brief .
 * \param .
 * \return \li 0 if \b arguments are valids;
 * \return \li 1 otherwise.
 */
//unsigned int get_cores();

/*!
 * \fn
 * \brief .
 * \param .
 * \return \li 0 if \b arguments are valids;
 * \return \li 1 otherwise.
 */
const std::string substr(const std::string&, unsigned int, bool = false);

}

#endif  // SRC_SHARED_FUNCTIONS_H_
