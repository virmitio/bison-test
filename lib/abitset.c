/* Array bitsets.
   Copyright (C) 2002 Free Software Foundation, Inc.
   Contributed by Michael Hayes (m.hayes@elec.canterbury.ac.nz).

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "abitset.h"
#include <stdlib.h>
#include <string.h>

/* This file implements fixed size bitsets stored as an array
   of words.  Any unused bits in the last word must be zero.  */

typedef struct abitset_struct
{
  unsigned int n_bits;	/* Number of bits.  */
  bitset_word words[1];	/* The array of bits.  */
}
*abitset;


struct bitset_struct
{
  struct bbitset_struct b;
  struct abitset_struct a;
};

static void abitset_unused_clear PARAMS ((bitset));

static int abitset_small_list PARAMS ((bitset, bitset_bindex *, bitset_bindex,
				       bitset_bindex *));

static void abitset_set PARAMS ((bitset, bitset_bindex));
static void abitset_reset PARAMS ((bitset, bitset_bindex));
static int abitset_test PARAMS ((bitset, bitset_bindex));
static int abitset_size PARAMS ((bitset));
static int abitset_op1 PARAMS ((bitset, enum bitset_ops));
static int abitset_op2 PARAMS ((bitset, bitset, enum bitset_ops));
static int abitset_op3 PARAMS ((bitset, bitset, bitset, enum bitset_ops));
static int abitset_op4 PARAMS ((bitset, bitset, bitset, bitset,
				enum bitset_ops));
static int abitset_list PARAMS ((bitset, bitset_bindex *, bitset_bindex,
				 bitset_bindex *));
static int abitset_reverse_list
PARAMS ((bitset, bitset_bindex *, bitset_bindex, bitset_bindex *));

#define ABITSET_N_WORDS(N) (((N) + BITSET_WORD_BITS - 1) / BITSET_WORD_BITS)
#define ABITSET_WORDS(X) ((X)->a.words)
#define ABITSET_N_BITS(X) ((X)->a.n_bits)


/* Return size in bits of bitset SRC.  */
static int
abitset_size (src)
     bitset src;
{
  return ABITSET_N_BITS (src);
}


/* Find list of up to NUM bits set in BSET starting from and including
 *NEXT and store in array LIST.  Return with actual number of bits
 found and with *NEXT indicating where search stopped.  */
static int
abitset_small_list (src, list, num, next)
     bitset src;
     bitset_bindex *list;
     bitset_bindex num;
     bitset_bindex *next;
{
  bitset_bindex bitno;
  bitset_bindex count;
  bitset_windex size;
  bitset_word word;

  word = ABITSET_WORDS (src)[0];

  /* Short circuit common case.  */
  if (!word)
    return 0;

  size = ABITSET_N_BITS (src);
  bitno = *next;
  if (bitno >= size)
    return 0;

  word >>= bitno;

  /* If num is 1, we could speed things up with a binary search
     of the word of interest.  */

  if (num >= BITSET_WORD_BITS)
    {
      for (count = 0; word; bitno++)
	{
	  if (word & 1)
	    list[count++] = bitno;
	  word >>= 1;
	}
    }
  else
    {
      for (count = 0; word; bitno++)
	{
	  if (word & 1)
	    {
	      list[count++] = bitno;
	      if (count >= num)
		{
		  bitno++;
		  break;
		}
	    }
	  word >>= 1;
	}
    }

  *next = bitno;
  return count;
}


/* Set bit BITNO in bitset DST.  */
static void
abitset_set (dst, bitno)
     bitset dst ATTRIBUTE_UNUSED;
     bitset_bindex bitno ATTRIBUTE_UNUSED;
{
  /* This should never occur for abitsets.  */
  abort ();
}


/* Reset bit BITNO in bitset DST.  */
static void
abitset_reset (dst, bitno)
     bitset dst ATTRIBUTE_UNUSED;
     bitset_bindex bitno ATTRIBUTE_UNUSED;
{
  /* This should never occur for abitsets.  */
  abort ();
}


/* Test bit BITNO in bitset SRC.  */
static int
abitset_test (src, bitno)
     bitset src ATTRIBUTE_UNUSED;
     bitset_bindex bitno ATTRIBUTE_UNUSED;
{
  /* This should never occur for abitsets.  */
  abort ();
  return 0;
}


/* Find list of up to NUM bits set in BSET in reverse order, starting
   from and including NEXT and store in array LIST.  Return with
   actual number of bits found and with *NEXT indicating where search
   stopped.  */
static int
abitset_reverse_list (src, list, num, next)
     bitset src;
     bitset_bindex *list;
     bitset_bindex num;
     bitset_bindex *next;
{
  bitset_bindex bitno;
  bitset_bindex rbitno;
  bitset_bindex count;
  bitset_windex windex;
  unsigned int bitcnt;
  bitset_bindex bitoff;
  bitset_word word;
  bitset_word *srcp = ABITSET_WORDS (src);
  bitset_bindex n_bits = ABITSET_N_BITS (src);

  rbitno = *next;

  /* If num is 1, we could speed things up with a binary search
     of the word of interest.  */

  if (rbitno >= n_bits)
    return 0;

  count = 0;

  bitno = n_bits - (rbitno + 1);

  windex = bitno / BITSET_WORD_BITS;
  bitcnt = bitno % BITSET_WORD_BITS;
  bitoff = windex * BITSET_WORD_BITS;

  for (; windex != ~0U; windex--, bitoff -= BITSET_WORD_BITS,
	 bitcnt = BITSET_WORD_BITS - 1)
    {
      word = srcp[windex] << (BITSET_WORD_BITS - 1 - bitcnt);
      for (; word; bitcnt--)
	{
	  if (word & BITSET_MSB)
	    {
	      list[count++] = bitoff + bitcnt;
	      if (count >= num)
		{
		  *next = n_bits - (bitoff + bitcnt);
		  return count;
		}
	    }
	  word <<= 1;
	}
    }

  *next = n_bits - (bitoff + 1);
  return count;
}


/* Find list of up to NUM bits set in BSET starting from and including
 *NEXT and store in array LIST.  Return with actual number of bits
 found and with *NEXT indicating where search stopped.  */
static int
abitset_list (src, list, num, next)
     bitset src;
     bitset_bindex *list;
     bitset_bindex num;
     bitset_bindex *next;
{
  bitset_bindex bitno;
  bitset_bindex count;
  bitset_windex windex;
  bitset_bindex bitoff;
  bitset_windex size = src->b.csize;
  bitset_word *srcp = ABITSET_WORDS (src);
  bitset_word word;

  bitno = *next;

  count = 0;
  if (!bitno)
    {
      /* Many bitsets are zero, so make this common case fast.  */
      for (windex = 0; windex < size && !srcp[windex]; windex++)
	continue;
      if (windex >= size)
	return 0;

      /* If num is 1, we could speed things up with a binary search
	 of the current word.  */

      bitoff = windex * BITSET_WORD_BITS;
    }
  else
    {
      if (bitno >= ABITSET_N_BITS (src))
	return 0;

      windex = bitno / BITSET_WORD_BITS;
      bitno = bitno % BITSET_WORD_BITS;

      if (bitno)
	{
	  /* Handle the case where we start within a word.
	     Most often, this is executed with large bitsets
	     with many set bits where we filled the array
	     on the previous call to this function.  */

	  bitoff = windex * BITSET_WORD_BITS;
	  word = srcp[windex] >> bitno;
	  for (bitno = bitoff + bitno; word; bitno++)
	    {
	      if (word & 1)
		{
		  list[count++] = bitno;
		  if (count >= num)
		    {
		      *next = bitno + 1;
		      return count;
		    }
		}
	      word >>= 1;
	    }
	  windex++;
	}
      bitoff = windex * BITSET_WORD_BITS;
    }

  for (; windex < size; windex++, bitoff += BITSET_WORD_BITS)
    {
      if (!(word = srcp[windex]))
	continue;

      if ((count + BITSET_WORD_BITS) < num)
	{
	  for (bitno = bitoff; word; bitno++)
	    {
	      if (word & 1)
		list[count++] = bitno;
	      word >>= 1;
	    }
	}
      else
	{
	  for (bitno = bitoff; word; bitno++)
	    {
	      if (word & 1)
		{
		  list[count++] = bitno;
		  if (count >= num)
		    {
		      *next = bitno + 1;
		      return count;
		    }
		}
	      word >>= 1;
	    }
	}
    }

  *next = bitoff;
  return count;
}


/* Ensure that any unused bits within the last word are clear.  */
static inline void
abitset_unused_clear (dst)
     bitset dst;
{
  unsigned int last_bit;

  last_bit = ABITSET_N_BITS (dst) % BITSET_WORD_BITS;
  if (last_bit)
    ABITSET_WORDS (dst)[dst->b.csize - 1] &=
      (bitset_word) ((1 << last_bit) - 1);
}


static int
abitset_op1 (dst, op)
     bitset dst;
     enum bitset_ops op;
{
  unsigned int i;
  bitset_word *dstp = ABITSET_WORDS (dst);
  unsigned int bytes;

  bytes = sizeof (bitset_word) * dst->b.csize;

  switch (op)
    {
    case BITSET_OP_ZERO:
      memset (dstp, 0, bytes);
      break;

    case BITSET_OP_ONES:
      memset (dstp, ~0, bytes);
      abitset_unused_clear (dst);
      break;

    case BITSET_OP_EMPTY_P:
      for (i = 0; i < dst->b.csize; i++)
	if (dstp[i])
	  return 0;
      break;

    default:
      abort ();
    }

  return 1;
}


static int
abitset_op2 (dst, src, op)
     bitset dst;
     bitset src;
     enum bitset_ops op;
{
  unsigned int i;
  bitset_word *srcp = ABITSET_WORDS (src);
  bitset_word *dstp = ABITSET_WORDS (dst);
  bitset_windex size = dst->b.csize;

  switch (op)
    {
    case BITSET_OP_COPY:
      if (srcp == dstp)
	break;
      memcpy (dstp, srcp, sizeof (bitset_word) * size);
      break;

    case BITSET_OP_NOT:
      for (i = 0; i < size; i++)
	*dstp++ = ~(*srcp++);
      abitset_unused_clear (dst);
      break;

    case BITSET_OP_EQUAL_P:
      for (i = 0; i < size; i++)
	if (*srcp++ != *dstp++)
	  return 0;
      break;
      
    case BITSET_OP_SUBSET_P:
      for (i = 0; i < size; i++, dstp++, srcp++)
	if (*dstp != (*srcp | *dstp))
	  return 0;
      break;
      
    case BITSET_OP_DISJOINT_P:
      for (i = 0; i < size; i++)
	if (*srcp++ & *dstp++)
	  return 0;
      break;
      
    default:
      abort ();
    }

  return 1;
}


static int
abitset_op3 (dst, src1, src2, op)
     bitset dst;
     bitset src1;
     bitset src2;
     enum bitset_ops op;
{
  unsigned int i;
  int changed = 0;
  bitset_word *src1p = ABITSET_WORDS (src1);
  bitset_word *src2p = ABITSET_WORDS (src2);
  bitset_word *dstp = ABITSET_WORDS (dst);
  bitset_windex size = dst->b.csize;

  switch (op)
    {
    case BITSET_OP_OR:
      for (i = 0; i < size; i++, dstp++)
	{
	  bitset_word tmp = *src1p++ | *src2p++;

	  if (*dstp != tmp)
	    {
	      changed = 1;
	      *dstp = tmp;
	    }
	}
      break;

    case BITSET_OP_AND:
      for (i = 0; i < size; i++, dstp++)
	{
	  bitset_word tmp = *src1p++ & *src2p++;

	  if (*dstp != tmp)
	    {
	      changed = 1;
	      *dstp = tmp;
	    }
	}
      break;

    case BITSET_OP_XOR:
      for (i = 0; i < size; i++, dstp++)
	{
	  bitset_word tmp = *src1p++ ^ *src2p++;

	  if (*dstp != tmp)
	    {
	      changed = 1;
	      *dstp = tmp;
	    }
	}
      break;

    case BITSET_OP_ANDN:
      for (i = 0; i < size; i++, dstp++)
	{
	  bitset_word tmp = *src1p++ & ~(*src2p++);

	  if (*dstp != tmp)
	    {
	      changed = 1;
	      *dstp = tmp;
	    }
	}
      break;

    default:
      abort ();
    }

  return changed;
}


static int
abitset_op4 (dst, src1, src2, src3, op)
     bitset dst;
     bitset src1;
     bitset src2;
     bitset src3;
     enum bitset_ops op;
{
  unsigned int i;
  int changed = 0;
  bitset_word *src1p = ABITSET_WORDS (src1);
  bitset_word *src2p = ABITSET_WORDS (src2);
  bitset_word *src3p = ABITSET_WORDS (src3);
  bitset_word *dstp = ABITSET_WORDS (dst);
  bitset_windex size = dst->b.csize;

  switch (op)
    {
    case BITSET_OP_OR_AND:
      for (i = 0; i < size; i++, dstp++)
	{
	  bitset_word tmp = (*src1p++ | *src2p++) & *src3p++;

	  if (*dstp != tmp)
	    {
	      changed = 1;
	      *dstp = tmp;
	    }
	}
      break;

    case BITSET_OP_AND_OR:
      for (i = 0; i < size; i++, dstp++)
	{
	  bitset_word tmp = (*src1p++ & *src2p++) | *src3p++;

	  if (*dstp != tmp)
	    {
	      changed = 1;
	      *dstp = tmp;
	    }
	}
      break;

    case BITSET_OP_ANDN_OR:
      for (i = 0; i < size; i++, dstp++)
	{
	  bitset_word tmp = (*src1p++ & ~(*src2p++)) | *src3p++;

	  if (*dstp != tmp)
	    {
	      changed = 1;
	      *dstp = tmp;
	    }
	}
      break;

    default:
      abort ();
    }

  return changed;
}


/* Vector of operations for single word bitsets.  */
struct bitset_ops_struct abitset_small_ops = {
  abitset_set,
  abitset_reset,
  abitset_test,
  abitset_size,
  abitset_op1,
  abitset_op2,
  abitset_op3,
  abitset_op4,
  abitset_small_list,
  abitset_reverse_list,
  NULL,
  BITSET_ARRAY
};


/* Vector of operations for multiple word bitsets.  */
struct bitset_ops_struct abitset_ops = {
  abitset_set,
  abitset_reset,
  abitset_test,
  abitset_size,
  abitset_op1,
  abitset_op2,
  abitset_op3,
  abitset_op4,
  abitset_list,
  abitset_reverse_list,
  NULL,
  BITSET_ARRAY
};


int
abitset_bytes (n_bits)
     bitset_bindex n_bits;
{
  unsigned int bytes, size;

  size = ABITSET_N_WORDS (n_bits);
  bytes = size * sizeof (bitset_word);
  return sizeof (struct bitset_struct) + bytes - sizeof (bitset_word);
}


bitset
abitset_init (bset, n_bits)
     bitset bset;
     bitset_bindex n_bits;
{
  bitset_windex size;

  size = ABITSET_N_WORDS (n_bits);
  ABITSET_N_BITS (bset) = n_bits;

  /* Use optimized routines if bitset fits within a single word.
     There is probably little merit if using caching since
     the small bitset will always fit in the cache.  */
  if (size == 1)
    bset->b.ops = &abitset_small_ops;
  else
    bset->b.ops = &abitset_ops;

  bset->b.cindex = 0;
  bset->b.csize = size;
  bset->b.cdata = ABITSET_WORDS (bset);
  return bset;
}