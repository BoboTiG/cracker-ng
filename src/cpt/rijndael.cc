
/*!
 * \file rijndael.c
 * \brief Part of CPT Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.28
 *
 * Copyright (C) 2000-2009 Peter Selinger.
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * This file is part of ccrypt. It is free software and it is covered
 * by the GNU general public license. See the file COPYING for details.
 *
 * $Id: rijndael.c 258 2009-08-26 17:46:10Z selinger $
 *
 * derived from original source: rijndael-alg-ref.c   v2.0   August '99
 * Reference ANSI C code for NIST competition
 * authors: Paulo Barreto
 *          Vincent Rijmen
 *
 * This is an optimized version for Cracker-ng.
 */


#include "./rijndael.h"

const int xshifts[3][2][4] = {
	{{0, 1, 2, 3},
	 {0, 3, 2, 1}},
	{{0, 1, 2, 3},
	 {0, 5, 4, 3}},
	{{0, 1, 3, 4},
	 {0, 7, 5, 4}}
};

/* Exor corresponding text input and round key input bytes */
/* the result is written to res, which can be the same as a */
static inline void xKeyAddition(
	word32 res[8], word32 a[8], word32 rk[8]
) {
	int j;
	for (j = 0; j < 8; j++) {
		res[j] = a[j] ^ rk[j];
	}
}

/* profiling shows that the ccrypt program spends about 50% of its
   time in the function xShiftSubst. Splitting the inner "for"
   statement into two parts - versus using the expensive "%" modulo
   operation, makes this function about 44% faster, thereby making the
   entire program about 28% faster. With -O3 optimization, the time
   savings are even more dramatic - ccrypt runs between 55% and 65%
   faster on most platforms. */

// do ShiftRow and Substitution together. res must not be a.
static inline void xShiftSubst(
	word32 res[8], word32 a[8], int shift[4], word8 box[256]
) {
	int i, j, s;
	word8 (*a8)[4] = (word8 (*)[4]) a;
	word8 (*res8)[4] = (word8 (*)[4]) res;
	for (j = 0; j < 8; j++) {
		res8[j][0] = box[a8[j][0]];
	}
	for (i = 1; i < 4; i++) {
		s = shift[i];
		for (j = 0; j < 8 - s; j++) {
			res8[j][i] = box[a8[(j + s)][i]];
		}
		for (j = 8 - s; j < 8; j++) {
			res8[j][i] = box[a8[(j + s) - 8][i]];
		}
	}
}

// do MixColumn and KeyAddition together
static inline void xMixAdd(
	word32 res[8], word32 a[8], word32 rk[8]
) {
	int j;
	word32 b;
	word8 (*a8)[4] = (word8 (*)[4]) a;
	for (j = 0; j < 8; j++) {
		b = M0[0][a8[j][0]].w32;
		b ^= M0[1][a8[j][1]].w32;
		b ^= M0[2][a8[j][2]].w32;
		b ^= M0[3][a8[j][3]].w32;
		b ^= rk[j];
		res[j] = b;
	}
}

/* Mix the four bytes of every column in a linear way
 * This is the opposite operation of xMixColumn
 * the result is written to res, which may equal a */
static inline void xInvMixColumn(word32 res[8], word32 a[8]) {
	int j;
	word32 b;
	word8 (*a8)[4] = (word8 (*)[4]) a;
	for (j = 0; j < 8; j++) {
		b = M1[0][a8[j][0]].w32;
		b ^= M1[1][a8[j][1]].w32;
		b ^= M1[2][a8[j][2]].w32;
		b ^= M1[3][a8[j][3]].w32;
		res[j] = b;
	}
}

int xrijndaelKeySched(word32 key[], roundkey* rkk) {
	// Calculate the necessary round keys
	int ROUNDS = 15, i, j, t = 0, rconpointer = 0;
	word8 (*k8)[4] = (word8 (*)[4]) key;

	// copy values into round key array
	for (j = 0; j < 8; j++, t++) {
		rkk->rk[t] = key[j];
	}
	while (t < ROUNDS * 8) {  // while not enough round key material
		// calculate new values
		k8[0][0] ^= xS[k8[7][1]];
		k8[0][1] ^= xS[k8[7][2]];
		k8[0][2] ^= xS[k8[7][3]];
		k8[0][3] ^= xS[k8[7][0]];
		k8[0][0] ^= xrcon[rconpointer++];
		key[1] ^= key[0];
		key[2] ^= key[1];
		key[3] ^= key[2];
		k8[4][0] ^= xS[k8[3][0]];
		k8[4][1] ^= xS[k8[3][1]];
		k8[4][2] ^= xS[k8[3][2]];
		k8[4][3] ^= xS[k8[3][3]];
		key[5] ^= key[4];
		key[6] ^= key[5];
		key[7] ^= key[6];
		// copy values into round key array
		for (j = 0; j < 8; j++, t++) {
			rkk->rk[t] = key[j];
		}
	}
	// make roundkey structure
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			rkk->shift[i][j] = xshifts[4 >> 1][i][j];
		}
	}
	return 0;
}

// Encryption of one block.
void xrijndaelEncrypt(word32 block[], roundkey* rkk) {
	word32 block2[8];  // hold intermediate result
	int *shift = rkk->shift[0];
	int r, ROUNDS = 14;
	word32 *rp = rkk->rk;

	// begin with a key addition
	xKeyAddition(block, block, rp);
	rp += 8;

	// ROUNDS-1 ordinary rounds
	for (r = 1; r < ROUNDS; r++, rp += 8) {
		xShiftSubst(block2, block, shift, xS);
		xMixAdd(block, block2, rp);
	}

	// Last round is special: there is no xMixColumn
	xShiftSubst(block2, block, shift, xS);
	xKeyAddition(block, block2, rp);
}

void xrijndaelDecrypt(word32 block[], roundkey* rkk) {
	word32 block2[8];  // hold intermediate result
	int *shift = rkk->shift[1];
	int r = 13, ROUNDS = 14;
	word32 *rp = rkk->rk + ROUNDS * 8;

	/* To decrypt: apply the inverse operations of the encrypt routine,
	*             in opposite order
	*
	* (xKeyAddition is an involution: it's equal to its inverse)
	* (the inverse of xSubstitution with table S is xSubstitution with the
	* inverse table of S)
	* (the inverse of xShiftRow is xShiftRow over a suitable distance)
	*/

	/* First the special round:include
	*   without xInvMixColumn
	*   with extra xKeyAddition
	*/
	xKeyAddition(block2, block, rp);
	xShiftSubst(block, block2, shift, xSi);
	rp -= 8;

	// ROUNDS-1 ordinary rounds
	for ( ; r > 0; r--, rp -= 8 ) {
		xKeyAddition(block, block, rp);
		xInvMixColumn(block2, block);
		xShiftSubst(block, block2, shift, xSi);
	}

	// End with the extra key addition
	xKeyAddition(block, block, rp);
}
