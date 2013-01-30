
/*!
 * \file tables.h
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.30
 *
 * Copyright (C) 2000-2009 Peter Selinger.
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 */


#ifndef SRC_CPT_TABLES_H_
#define SRC_CPT_TABLES_H_


typedef unsigned char word8;
typedef unsigned int word32;
union word8x4_u {
	word8 w8[4];
	word32 w32;
};
typedef union word8x4_u word8x4;

extern const word8x4 M0[4][256];
extern const word8x4 M1[4][256];
extern const int xrcon[30];
extern word8 xS[256];
extern word8 xSi[256];

#endif  // SRC_CPT_TABLES_H_
