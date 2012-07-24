
/*!
 * \file cptcracker-ng.hpp
 * \brief ccrypt Cracker-ng headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.07.24
 */


#define MODULE  "CPT"     //!< Module name.
#define VERSION "0.1-2"   //!< Module version.
#define PWD_MAX  32       //!< Maximum password length (32)
#define CPT

#include "../shared/cracker-ng.cpp"
#include "tables.c"
#include "rijndael.c"
#include "ccryptlib.c"


// Prototypes ----------------------------------------------------------
/*!
 * \fn ccrypt_crack(ifstream & filei)
 * \brief Launch the crack.
 * \param filei Reference to the input file
 */
void ccrypt_crack(ifstream & filei);
