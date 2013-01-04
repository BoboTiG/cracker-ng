
/*!
 * \file tables.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.09.15
 *
 * Copyright (C) 2000-2009 Peter Selinger.
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 */


#ifndef SRC_CPT_TABLES_H_
#define SRC_CPT_TABLES_H_

#include "./rijndael.h"

extern word8x4 M0[4][256];
extern word8x4 M1[4][256];
extern int xrcon[30];
extern word8 xS[256];
extern word8 xSi[256];

#endif  // SRC_CPT_TABLES_H_
