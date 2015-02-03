
/*!
 * \file puff.cc
 * \brief ZIP Cracker-ng deflate algorithm (optimized for the project).
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2014.01.04
 *
 * Copyright (C) 2002-2010 Mark Adler
 * Copyright (C) 2012-2014 Mickaël 'Tiger-222' Schoentgen.
 *
 * Why using puff() instead of optimized boost libraries?
 * This is simple: puff() aborts early when there is one bad operation.
 * Boost inflates all datas and then thows an error.
 * So for this king of software, where there is only one matching password,
 * we need to stop the inflate process the most early possible to save
 * memory and increase number of tested passwords by seconds.
 *
 * BTW Boost is really more efficient to inflate datas with the good password.
*/


#include "./puff.h"


const int lens[29] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
	35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
const int lext[29] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
const int dists[30] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
	257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
	8193, 12289, 16385, 24577};
const int dext[30] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
	12, 12, 13, 13};
const int order[19] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

static jmp_buf jump;

static int bits(struct state *s, const int &need)
{
	int val = s->bitbuf;

	for ( ; s->bitcnt < need ; s->bitcnt += 8 ) {
		if (s->incnt == s->inlen)
			longjmp(jump, 1);  // out of input
		val |= s->in[s->incnt++] << s->bitcnt;  // load eight bits
	}

	// drop need bits and update buffer, always zero to seven bits left
	s->bitbuf  = val >> need;
	s->bitcnt -= need;

	// return need bits, zeroing the bits above that
	return val & ((1L << need) - 1);
}

static int stored(struct state *s)
{
	unsigned int len;  // length of stored block

	// discard leftover bits from current byte (assumes s->bitcnt < 8)
	s->bitbuf = 0;
	s->bitcnt = 0;

	// get length and check against its one's complement
	if (s->incnt + 4 > s->inlen)
		return 2;  // not enough input
	len = s->in[s->incnt++];
	len |= s->in[s->incnt++] << 8;
	if (s->in[s->incnt++] != (~len & 0xff) ||
		s->in[s->incnt++] != ((~len >> 8) & 0xff))
		return -2;  // didn't match complement!

	// copy len bytes from in to out
	if (s->incnt + len > s->inlen)
		return 2;  // not enough input
	if (s->outcnt + len > s->outlen)
		return 1;  // not enough output space
	for ( ; len; --len )
		s->out[s->outcnt++] = s->in[s->incnt++];

	// done with a valid stored block
	return 0;
}

static int decode(struct state *s, const struct huffman *h)
{
	int len, code, first, count, index, bitbuf, left;
	int *next;

	bitbuf = s->bitbuf;
	left = s->bitcnt;
	code = first = index = 0;
	len = 1;
	next = h->count + 1;
	for ( ;; ) {
		for ( ; left; --left ) {
			code |= bitbuf & 1;
			bitbuf >>= 1;
			count = *next++;
			if (code - count < first) {  // if length len, return symbol
				s->bitbuf = bitbuf;
				s->bitcnt = (s->bitcnt - len) & 7;
				return h->symbol[index + (code - first)];
			}
			index += count;  // else update for next length
			first += count;
			first <<= 1;
			code  <<= 1;
			++len;
		}
		left = 16 - len;
		if (left == 0)
			break;
		if (s->incnt == s->inlen)
			longjmp(jump, 1);  // out of input
		bitbuf = s->in[s->incnt++];
		if (left > 8)
			left = 8;
	}
	return -10;  // ran out of codes
}

static int construct(struct huffman *h, const int *length, const int &n)
{
	int symbol, len, left, offs[16];

	// count number of codes of each length
	for (len = 0; len < 16; ++len)
		h->count[len] = 0;
	for (symbol = 0; symbol < n; ++symbol)
		h->count[length[symbol]]++;   // assumes lengths are within bounds
	if (h->count[0] == n)             // no codes!
		return 0;                     // complete, but decode() will fail

	// check for an over-subscribed or incomplete set of lengths
	left = 1;                         // one possible code of zero length
	for (len = 1; len < 16; ++len) {
		left <<= 1;                   // one more bit, double codes left
		left -= h->count[len];        // deduct count from possible codes
		if (left < 0)
			return left;              // over-subscribed--return negative
	}                                 // left > 0 means incomplete

	// generate offsets into symbol table for each length for sorting
	offs[1] = 0;
	for (len = 1; len < 15; ++len)
		offs[len + 1] = offs[len] + h->count[len];

	/*
	 * put symbols in table sorted by length, by symbol order within each
	 * length
	*/
	for (symbol = 0; symbol < n; ++symbol)
		if (length[symbol] != 0)
			h->symbol[offs[length[symbol]]++] = symbol;

	// return zero for complete set, positive for incomplete set
	return left;
}

static int codes(
	struct state *s,
	const struct huffman *lencode,
	const struct huffman *distcode)
{
	int symbol, len;
	unsigned int dist;

	// decode literals and length/distance pairs
	do {
		symbol = decode(s, lencode);
		if (symbol < 0)
			return symbol;  // invalid symbol
		if (symbol < 256) {  // literal: symbol is the byte
			// write out the literal
			if (s->outcnt == s->outlen)
				return 1;
			s->out[s->outcnt] = symbol;
			++s->outcnt;
		}
		else if (symbol > 256) {  // length
			// get and compute length
			symbol -= 257;
			if (symbol >= 29)
				return -10;  // invalid fixed code
			len = lens[symbol] + bits(s, lext[symbol]);

			// get and check distance
			symbol = decode(s, distcode);
			if (symbol < 0)
				return symbol;  // invalid symbol
			dist = dists[symbol] + bits(s, dext[symbol]);
			if (dist > s->outcnt)
				return -11;  // distance too far back

			// copy length bytes from distance bytes back
			if (s->outcnt + len > s->outlen)
				return 1;
			for ( ; len; --len ) {
				s->out[s->outcnt] = s->out[s->outcnt - dist];
				++s->outcnt;
			}
		}
	} while (symbol != 256);  // end of block symbol

	// done with a valid fixed or dynamic block
	return 0;
}

static int fixed(struct state *s)
{
	int symbol;
	int lencnt[16], lensym[288], distcnt[16], distsym[30], lengths[288];
	struct huffman lencode, distcode;

	// build fixed huffman tables

	// construct lencode and distcode
	lencode.count   = lencnt;
	lencode.symbol  = lensym;
	distcode.count  = distcnt;
	distcode.symbol = distsym;

	// literal/length table
	for (symbol = 0; symbol < 144; ++symbol)
		lengths[symbol] = 8;
	for (; symbol < 256; ++symbol)
		lengths[symbol] = 9;
	for (; symbol < 280; ++symbol)
		lengths[symbol] = 7;
	for (; symbol < 288; ++symbol)
		lengths[symbol] = 8;
	construct(&lencode, lengths, 288);

	// distance table
	for (symbol = 0; symbol < 30; ++symbol)
		lengths[symbol] = 5;
	construct(&distcode, lengths, 30);

	// decode data until end-of-block code
	return codes(s, &lencode, &distcode);
}

static int dynamic(struct state *s)
{
	int nlen, ndist, ncode, index, err;
	int lengths[316], lencnt[16], lensym[286],distcnt[16], distsym[30];
	struct huffman lencode, distcode;

	// construct lencode and distcode
	lencode.count = lencnt;
	lencode.symbol = lensym;
	distcode.count = distcnt;
	distcode.symbol = distsym;

	// get number of lengths in each table, check lengths
	nlen  = bits(s, 5) + 257;
	ndist = bits(s, 5) + 1;
	ncode = bits(s, 4) + 4;
	if (nlen > 286 || ndist > 30)
		return -3;  // bad counts

	// read code length code lengths (really), missing lengths are zero
	for (index = 0; index < ncode; ++index)
		lengths[order[index]] = bits(s, 3);
	for (; index < 19; ++index)
		lengths[order[index]] = 0;

	// build huffman table for code lengths codes (use lencode temporarily)
	err = construct(&lencode, lengths, 19);
	if (err != 0)  // require complete code set here
		return -4;

	// read length/literal and distance code length tables
	index = 0;
	int nn = nlen + ndist;
	for ( ; index < nn ; ) {
		int symbol;  // decoded value
		int len;     // last length to repeat

		symbol = decode(s, &lencode);
		if (symbol < 0)
			return symbol;  // invalid symbol
		if (symbol < 16)  // length in 0..15
			lengths[index++] = symbol;
		else {  // repeat instruction
			len = 0;  // assume repeating zeros
			if (symbol == 16) {  // repeat last length 3..6 times
				if (index == 0)
					return -5;  // no last length!
				len = lengths[index - 1];  // last length
				symbol = 3 + bits(s, 2);
			}
			else if (symbol == 17)  // repeat zero 3..10 times
				symbol = 3 + bits(s, 3);
			else  // == 18, repeat zero 11..138 times
				symbol = 11 + bits(s, 7);
			if (index + symbol > nn)
				return -6;  // too many lengths!
			for ( ; symbol; --symbol )  // repeat last or zero symbol times
				lengths[index++] = len;
		}
	}

	// check for end-of-block code -- there better be one!
	if (lengths[256] == 0)
		return -9;

	// build huffman table for literal/length codes
	err = construct(&lencode, lengths, nlen);
	if (err && (err < 0 || nlen != lencode.count[0] + lencode.count[1]))
		return -7;  // incomplete code ok only for single length 1 code

	// build huffman table for distance codes
	err = construct(&distcode, lengths + nlen, ndist);
	if (err && (err < 0 || ndist != distcode.count[0] + distcode.count[1]))
		return -8;  // incomplete code ok only for single length 1 code

	// decode data until end-of-block code
	return codes(s, &lencode, &distcode);
}

int puff(
	unsigned char*       dest,
	const unsigned long  destlen,
	const unsigned char* source,
	const unsigned long  sourcelen,
	struct state&        s)
{
	int last, type, err;

	// initialize output state
	s.out    = dest;
	s.outlen = destlen;
	s.outcnt = 0;

	// initialize input state
	s.in     = source;
	s.inlen  = sourcelen;
	s.incnt  = 0;
	s.bitbuf = 0;
	s.bitcnt = 0;

	// return if bits() or decode() tries to read past available input
	if (setjmp(jump) != 0)  // if came back here via longjmp()
		err = 2;             // then skip do-loop, return error
	else {
		// process blocks until last block or error
		do {
			last = bits(&s, 1);  // one if last block
			type = bits(&s, 2);  // block type 0..3
			if ( type == 1 ) {
				err = fixed(&s);
			} else if ( type == 2 ) {
				err = dynamic(&s);
			} else if ( type == 0 ) {
				err = stored(&s);
			} else {
				err = -1;  // return with error
			}
		} while (!last && err == 0);
	}
	return err;
}
