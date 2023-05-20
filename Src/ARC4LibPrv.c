/*	$NetBSD: arc4.c,v 1.1 2000/07/04 14:27:57 onoe Exp $	*/

/*
 * ARC4 implementation
 *	A Stream Cipher Encryption Algorithm "Arcfour"
 *	<draft-kaukonen-cipher-arcfour-03.txt>
 */

/*        This code illustrates a sample implementation
 *                 of the Arcfour algorithm
 *         Copyright (c) April 29, 1997 Kalle Kaukonen.
 *                    All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that this copyright
 * notice and disclaimer are retained.
 *
 * THIS SOFTWARE IS PROVIDED BY KALLE KAUKONEN AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALLE
 * KAUKONEN OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// *****
// * PROJECT:		ARC4Lib (ARC4)
// * FILENAME: 		ARC4LibPrv.c
// * AUTHOR:		Hector Ho Fuentes
// *
// *				
// * 
// * DESCRIPTION:	ARC4 library functionality interface implementation.
// *
// * HISTORY:		Hector Ho Fuentes 1/23/2001
// *
// *
// * COPYRIGHT:		
// *
// *****

#include <PalmOS.h>											// Standard Palm stuff
#include "ARC4Lib.h"
#include "ARC4LibPrv.h"


int ARC4_Setkey(ARC4_CTX * ctx, unsigned char * key, int keylen)
{
	unsigned int i, t, u, ki, si;

	ctx->x = 0;
	ctx->y = 0;
	for (i = 0; i < 256; i++)
	       ctx->state[i] = i;
	ki = si = 0;
	for (i = 0; i < 256; i++) {
		t = ctx->state[i];
		si = (si + key[ki] + t) & 0xff;
		u = ctx->state[si];
		ctx->state[si] = t;
		ctx->state[i] = u;
		if (++ki >= keylen)
			ki = 0;
	}
	
	return 0;
}

int ARC4_Encrypt(ARC4_CTX * ctx, unsigned char * dst, unsigned char * src, int len)
{
	unsigned int x, y, sx, sy;
	const unsigned char *endsrc;

	x = ctx->x;
	y = ctx->y;
	for (endsrc = src + len; src != endsrc; src++, dst++) {
		x = (x + 1) & 0xff;
		sx = ctx->state[x];
		y = (sx + y) & 0xff;
		ctx->state[x] = sy = ctx->state[y];
		ctx->state[y] = sx;
		*dst = *src ^ (ctx->state[(sx + sy) & 0xff]);
	}
	ctx->x = x;
	ctx->y = y;

	return 0;
}

int ARC4_Decrypt(ARC4_CTX * ctxp, unsigned char * dst, unsigned char * src, int len)
{
	return ARC4_Encrypt(ctxp, dst, src, len);
}
