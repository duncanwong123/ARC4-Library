// *****
// * PROJECT:		ARC4Lib (ARC4)
// * FILENAME: 		ARC4LibPrv.h
// * AUTHOR:		Hector Ho Fuentes
// *
// *				
// * 
// * DESCRIPTION:	Shared library functionality interface definition for PRIVATE
// *				functions.  These should be used to build a library, but should
// *				not be distributed with that library; instead, just distribute
// *				ARC4Lib.h
// *
// *
// * HISTORY:		Hector Ho Fuentes 1/23/2001
// * 
// *
// * COPYRIGHT:		
/*	$NetBSD: arc4.h,v 1.1 2000/07/04 14:27:57 onoe Exp $	*/

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
// * PROJECT:		MySharedLib (MSL)
// * FILENAME: 		MySharedLibPrv.h
// * AUTHOR:		Jeff Ishaq 05/21/99
// * 
// * DESCRIPTION:	Shared library functionality interface definition for PRIVATE
// *				functions.  These should be used to build a library, but should
// *				not be distributed with that library; instead, just distribute
// *				MySharedLib.h
// *
// * COPYRIGHT:		As long as this 'copyright' is intact, this code is freely modifiable
// *				and distributable.
// *****

#pragma once

// This is the Globals struct that we use throughout our library.
typedef struct tagARC4GlobalsType
{
	Int16		iOpenCount;				// Our internal open-count of the lib
	
	/////
	// Your globals go here...
	/////

} ARC4GlobalsType;
typedef ARC4GlobalsType*	ARC4GlobalsTypePtr;

// These are some utility functions.  We don't actually use these in our dispatch
// table, so we don't need to define traps for them nor extern them.

ARC4GlobalsTypePtr	ARC4AllocGlobals	( UInt16 uRefNum );
ARC4GlobalsTypePtr	ARC4LockGlobals	( UInt16 uRefNum );
Err 				ARC4FreeGlobals	( UInt16 uRefNum );
Err					ARC4UnlockGlobals( ARC4GlobalsTypePtr gP );

int ARC4_Setkey (ARC4_CTX * context, unsigned char * key, int keylen);
int ARC4_Encrypt(ARC4_CTX * context, unsigned char * out, unsigned char * in, int len);
int ARC4_Decrypt(ARC4_CTX * context, unsigned char * out, unsigned char * in, int len);
