/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/*
 * $XConsortium: getretmips.c,v 1.4 94/04/17 20:59:44 keith Exp $
 *
 Copyright (c) 1992  X Consortium

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 Except as contained in this notice, the name of the X Consortium shall not
 be used in advertising or otherwise to promote the sale, use or other dealings
 in this Software without prior written authorization from the X Consortium.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/* Return stack generation for MIPS processors
 * This is tricky as MIPS stack frames aren't
 * easily unrolled -- we look for pc restoration
 * and stack adjustment instructions beyond the return
 * address to discover the correct values
 */

/* lw $31,const($sp) is : 100 011 11101 11111 const */
/*                        1000 1111 1011 1111       */

#define RESTORE_RETURNVAL 0x8fbf0000
#define RESTORE_RETURNVAL_MASK 0xffff0000

/* ld $31,const($sp) is : 110 111 11101 11111 const */
/*                        1101 1111 1011 1111       */

#define RESTORE_RETURNVAL2 0xdfbf0000

/* lq $31,const($sp) is : 011110 11101 11111 const */
/* EE-CORE SPECIFIC       0111 1011 1011 1111      */

#define RESTORE_RETURNVAL3  0x7bbf0000

/* addiu $sp, $sp, const is 001 001 11101 11101 const */
/*                          0010 0111 1011 1101 const */

#define ADJUST_STACKP_C  0x27bd0000
#define ADJUST_STACKP_C_MASK 0xffff0000

/* addu $sp, $sp, $at is 000 000 11101 00001 11101 00000 100 001  */
/*                       0000 0011 1010 0001 1110 1000 0010 0001 */

#define ADJUST_STACKP_V  0x03a1e821
#define ADJUST_STACKP_V_MASK 0xffffffff

/* lui $at, const is 001 111 00000 00001 const */
/*       0011 1100 0000 0001 const */

#define SET_UPPER_C  0x3c010000
#define SET_UPPER_C_MASK 0xffff0000

/* ori $at, $at, const is 001 101 00001 00001 const */
/*                        0011 0100 0010 0001 const */

#define OR_LOWER_C  0x34210000
#define OR_LOWER_C_MASK  0xffff0000

/* ori $at, $zero, const is 001 101 00000 00001 const */
/*                          0011 0100 0000 0001 const */

#define SET_LOWER_C  0x34010000
#define SET_LOWER_C_MASK 0xffff0000

/* jr $ra */
#define RETURN     0x03e00008

#define CALL(f)     (0x0c000000 | (((int) (f)) >> 2))

/*
 * This computation is expensive, so we cache the results;
 * a simple hash function and straight-forward replacement.
 */

#define HASH_SIZE   256

typedef struct _returnCache {
	unsigned int    *returnAddress;
	int      raOffset;
	int      spAdjust;
} ReturnCacheRec, *ReturnCachePtr;

static ReturnCacheRec returnCache[HASH_SIZE];

#define HASH(ra)    ((((int) (ra)) >> 2) & (HASH_SIZE - 1))

typedef int Bool;

#define TRUE 1
#define FALSE 0

static unsigned* get_return_address()
{
	register unsigned* r;
	asm("move %0, $31":"=r"(r));
	return r;
}

static int backtrace(void** results, int maxin)
{
	extern int     main ();
	unsigned int    *ra, *ra_limit;
	unsigned int    *sp;
	unsigned int    inst;
	unsigned int    mainCall;
	unsigned short  const_upper;
	unsigned short  const_lower;
	int      ra_offset;
	int      sp_adjust;
	Bool     found_ra_offset, found_sp_adjust;
	Bool     found_const_upper, found_const_lower;
	ReturnCachePtr  rc;
	int max = maxin;

	ra = get_return_address();
	asm("move %0, $sp" : "=r"(sp));

	mainCall = (unsigned int)CALL(main);

	while (ra && max)
	{
		rc = &returnCache[HASH(ra)];
		if (rc->returnAddress != ra)
		{
			found_ra_offset = FALSE;
			found_sp_adjust = FALSE;
			found_const_upper = FALSE;
			found_const_lower = FALSE;
			const_upper = 0;
			const_lower = 0;
			rc->returnAddress = ra;
			ra_limit = (unsigned int  *) 0x10000000;
			ra_offset = 0;
			sp_adjust = -1;
			while ((!found_ra_offset || !found_sp_adjust) && ra < ra_limit)
			{
				inst = *ra;
				/* look for the offset of the PC in the stack frame */
				if ((inst & RESTORE_RETURNVAL_MASK) == RESTORE_RETURNVAL)
				{
					ra_offset = (int)( inst & ~RESTORE_RETURNVAL_MASK );
					found_ra_offset = TRUE;
				}
				else if ((inst & RESTORE_RETURNVAL_MASK) == RESTORE_RETURNVAL2)
				{
					ra_offset = (int)( inst & ~RESTORE_RETURNVAL_MASK );
					found_ra_offset = TRUE;
				}
				else if ((inst & RESTORE_RETURNVAL_MASK) == RESTORE_RETURNVAL3)
				{
					ra_offset =(int)(inst & ~RESTORE_RETURNVAL_MASK );
					found_ra_offset = TRUE;
				}
				else if ((inst & ADJUST_STACKP_C_MASK) == ADJUST_STACKP_C)
				{
					sp_adjust = (short)(inst & ~ADJUST_STACKP_C_MASK);
					found_sp_adjust = TRUE;
				}
				else if ((inst & ADJUST_STACKP_V_MASK) == ADJUST_STACKP_V)
				{
					sp_adjust = 0;
					found_sp_adjust = TRUE;
				}
				else if ((inst & SET_UPPER_C_MASK) == SET_UPPER_C)
				{
					const_upper = (unsigned short)( inst & ~SET_UPPER_C_MASK ); 
					const_lower = 0;
					found_const_upper = TRUE;
				}
				else if ((inst & OR_LOWER_C_MASK) == OR_LOWER_C)
				{
					const_lower = (unsigned short)( inst & ~OR_LOWER_C_MASK );
					found_const_lower = TRUE;
				}
				else if ((inst & SET_LOWER_C_MASK) == SET_LOWER_C)
				{
					const_lower = (unsigned short)( inst & ~SET_LOWER_C_MASK );
					const_upper = 0;
					found_const_lower = TRUE;
				}
				else if (inst == RETURN)
					ra_limit = ra + 2;
				ra++;
			}
			if (sp_adjust == 0 && (found_const_upper || found_const_lower))
			{
				sp_adjust = (const_upper << 16) | const_lower;
			}
			rc->raOffset = ra_offset;
			rc->spAdjust = sp_adjust;
		}
		/* if something went wrong, punt */
		if (rc->spAdjust <= 0)
		{
			break;
		}
		ra = (unsigned int *) sp[rc->raOffset>>2];
		sp += rc->spAdjust >> 2;
		*results++ = (void*)(((unsigned int) ra) - 8);
		if (ra[-2] == mainCall)
		{
			break;
		}
		max--;
	}
	return maxin - max;
}

static char** backtrace_symbols(void* const* addr, int numaddr)
{
	return 0;
}

static void backtrace_symbols_free(char** syms)
{
}


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060902)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
