
/*!
 * \file puff.cc
 * \brief ZIP Cracker-ng headers for the deflate algorithm.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2013.01.20
 * 
 * Copyright (C) 2002-2010 Mark Adler
 * Copyright (C) 2012-2013 Mickaël 'Tiger-222' Schoentgen.
 * 
 * For conditions of distribution and use, see copyright notice in puff.h
 * version 2.2, 25 Apr 2010
 *
 * puff.cc is a simple inflate written to be an unambiguous way to specify the
 * deflate format.  It is not written for speed but rather simplicity.  As a
 * side benefit, this code might actually be useful when small code is more
 * important than speed, such as bootstrap applications.  For typical deflate
 * data, zlib's inflate() is about four times as fast as puff().  zlib's
 * inflate compiles to around 20K on my machine, whereas puff.c compiles to
 * around 4K on my machine (a PowerPC using GNU cc).  If the faster decode()
 * function here is used, then puff() is only twice as slow as zlib's
 * inflate().
 *
 * All dynamically allocated memory comes from the stack.  The stack required
 * is less than 2K bytes.  This code is compatible with 16-bit int's and
 * assumes that long's are at least 32 bits.  puff.cc uses the short data type,
 * assumed to be 16 bits, for arrays in order to to conserve memory.  The code
 * works whether integers are stored big endian or little endian.
*/


#include "./puff.h"
#include <cstdio>


static int bits(struct state *s, int need)
{
    int val = s->bitbuf;  // load at least need bits into val

	for ( ; s->bitcnt < need ; s->bitcnt += 8 ) {
        if (s->incnt == s->inlen)
            longjmp(s->env, 1);  // out of input
        val |= s->in[s->incnt++] << s->bitcnt;  // load eight bits
    }

    // drop need bits and update buffer, always zero to seven bits left
    s->bitbuf = val >> need;
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
    while (len--)
        s->out[s->outcnt++] = s->in[s->incnt++];

    // done with a valid stored block
    return 0;
}

static int decode(struct state *s, const struct huffman *h)
{
    int len;      // current number of bits in code
    int code;     // len bits being decoded
    int first;    // first code of length len
    int count;    // number of codes of length len
    int index;    // index of first code of length len in symbol table
    int bitbuf;   // bits from stream
    int left;     // bits left in next or left to process
    short *next;  // next number of codes

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
            code <<= 1;
            len++;
        }
        left = (MAXBITS+1) - len;
        if (left == 0)
            break;
        if (s->incnt == s->inlen)
            longjmp(s->env, 1);  // out of input
        bitbuf = s->in[s->incnt++];
        if (left > 8)
            left = 8;
    }
    return -10;  // ran out of codes
}

static int construct(struct huffman *h, const short *length, int n)
{
    int symbol;             // current symbol when stepping through length[]
    int len;                // current length when stepping through h->count[]
    int left;               // number of possible codes left of current length
    short offs[MAXBITS+1];  // offsets in symbol table for each length

    // count number of codes of each length
    for (len = 0; len <= MAXBITS; ++len)
        h->count[len] = 0;
    for (symbol = 0; symbol < n; ++symbol)
        h->count[length[symbol]]++;   // assumes lengths are within bounds
    if (h->count[0] == n)             // no codes!
        return 0;                     // complete, but decode() will fail

    // check for an over-subscribed or incomplete set of lengths
    left = 1;                         // one possible code of zero length
    for (len = 1; len <= MAXBITS; ++len) {
        left <<= 1;                   // one more bit, double codes left
        left -= h->count[len];        // deduct count from possible codes
        if (left < 0)
            return left;              // over-subscribed--return negative
    }                                 // left > 0 means incomplete

    // generate offsets into symbol table for each length for sorting
    offs[1] = 0;
    for (len = 1; len < MAXBITS; ++len)
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
    int symbol;     // decoded symbol
    int len;        // length for copy
    unsigned dist;  // distance for copy

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
            while (len--) {
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
    static short lencnt[MAXBITS+1], lensym[FIXLCODES];
    static short distcnt[MAXBITS+1], distsym[MAXDCODES];
    static struct huffman lencode, distcode;
    int symbol;
    short lengths[FIXLCODES];

    // build fixed huffman tables

    // construct lencode and distcode
    lencode.count = lencnt;
    lencode.symbol = lensym;
    distcode.count = distcnt;
    distcode.symbol = distsym;

    // literal/length table
    for (symbol = 0; symbol < 144; ++symbol)
        lengths[symbol] = 8;
    for (; symbol < 256; ++symbol)
        lengths[symbol] = 9;
    for (; symbol < 280; ++symbol)
        lengths[symbol] = 7;
    for (; symbol < FIXLCODES; ++symbol)
        lengths[symbol] = 8;
    construct(&lencode, lengths, FIXLCODES);

    // distance table
    for (symbol = 0; symbol < MAXDCODES; ++symbol)
        lengths[symbol] = 5;
    construct(&distcode, lengths, MAXDCODES);

    // decode data until end-of-block code
    return codes(s, &lencode, &distcode);
}

static int dynamic(struct state *s)
{
    int nlen, ndist, ncode;                        // number of lengths in descriptor
    int index;                                     // index of lengths[]
    int err;                                       // construct() return value
    short lengths[MAXCODES];                       // descriptor code lengths
    short lencnt[MAXBITS+1], lensym[MAXLCODES];    // lencode memory
    short distcnt[MAXBITS+1], distsym[MAXDCODES];  // distcode memory
    struct huffman lencode, distcode;              // length and distance codes

    // construct lencode and distcode
    lencode.count = lencnt;
    lencode.symbol = lensym;
    distcode.count = distcnt;
    distcode.symbol = distsym;

    // get number of lengths in each table, check lengths
    nlen = bits(s, 5) + 257;
    ndist = bits(s, 5) + 1;
    ncode = bits(s, 4) + 4;
    if (nlen > MAXLCODES || ndist > MAXDCODES)
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
    if (lengths[256] <= 0)
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
	unsigned long        destlen,
	const unsigned char* source,
	unsigned long        sourcelen,
	struct state&        s)
{
    int last, type;  // block information
    int err;         // return value

    // initialize output state
    s.out = dest;
    s.outlen = destlen; // ignored if dest is NIL
    s.outcnt = 0;

    // initialize input state
    s.in = source;
    s.inlen = sourcelen;
    s.incnt = 0;
    s.bitbuf = 0;
    s.bitcnt = 0;

    // return if bits() or decode() tries to read past available input
    if (setjmp(s.env) != 0)  // if came back here via longjmp()
        err = 2;             // then skip do-loop, return error
    else {
        // process blocks until last block or error
        do {
            last = bits(&s, 1);  // one if last block
            type = bits(&s, 2);  // block type 0..3 
            if ( type == 2 )
            	err = dynamic(&s);
            else if ( type == 1 )
            	err = fixed(&s);
            else if ( type == 0 )
            	err = stored(&s);
            else {
            	err = -1;  // return with error
            }
            if ( err != 0 )
            	break;
        } while (!last);
    }
    return err;
}
