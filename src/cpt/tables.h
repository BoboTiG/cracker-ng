
/*!
 * \file tables.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2011.12.26
 *
 * Copyright (C) 2000-2009 Peter Selinger.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 */


#ifndef TABLES_H
#define TABLES_H

#include "rijndael.h"

extern word8x4 M0[4][256];
extern word8x4 M1[4][256];
extern int xrcon[30];
extern word8 xS[256];
extern word8 xSi[256];

#endif // TABLES_H
