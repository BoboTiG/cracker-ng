
/*!
 * \file main.h
 * \brief ZIP Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.21
 * 
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * See http://www.pkware.com/documents/casestudies/APPNOTE.TXT for
 * more details about ZIP specifications.
 */


#ifndef SRC_ZIP_MAIN_H_
#define SRC_ZIP_MAIN_H_

#include "./../shared/functions.h"
#include "./cracker.h"


static const char* MODULE = "ZIP";     //!< Module name.
static const char* VERSION = "0.1-1";  //!< Module version.


/*!
 * \fn signal_handler(int s)
 * \brief Action to do when CTRL + C raises.
 * \param s Signal number.
 */
void signal_handler(int);

#endif  // SRC_ZIP_MAIN_H_
