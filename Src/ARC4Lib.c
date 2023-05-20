// *****
// * PROJECT:		ARC4Lib (ARC4)
// * FILENAME: 		ARC4Lib.c
// * AUTHOR:		Hector Ho Fuentes
// * 
// * DESCRIPTION:	ARC4 library functionality implementation.  
// *
// *
// * HISTORY:		Hector Ho Fuentes 1/23/2001
// *
// *
// * COPYRIGHT:		
// *
// *****


// *****
// * PROJECT:		MySharedLib (MSL)
// * FILENAME: 		MySharedLib.c
// * AUTHOR:		Jeff Ishaq 05/21/99
// * 
// * DESCRIPTION:	Shared library functionality implementation.  This code is influenced
// *				by the design outlined in Palm's "Shared Libraries and Other Advanced 
// *				Project Types" white paper, article #1143.  This can be found on Palm's
// *				knowledge base.
// *
// * COPYRIGHT:		As long as this 'copyright' is intact, this code is freely modifiable
// *				and distributable.
// *****

// Because we play with #defines that you're not normally expected to play with,
// we tend to run into cryptic link errors by including precompiled headers:

#ifndef PILOT_PRECOMPILED_HEADERS_OFF
	#define	PILOT_PRECOMPILED_HEADERS_OFF
#endif

#include <PalmOS.h>											// Standard Palm stuff
#include "ARC4Lib.h"									// Our interface definition
#include "ARC4LibPrv.h"									// Private routiens (globals stuff)
 

#pragma mark - 
// Utility functions for globals structure access

// *****
// * FUNCTION: 		ARC4AllocGlobals
// * 
// * DESCRIPTION:	Allocate AND LOCK library globals ptr for a given RefNum
// *
// * PARAMETERS:	uRefNum		-		Lib refnum whose globals we'll create
// *			
// * RETURNED:		Ptr to new globals		-	success
// *				NULL					-	failure, MemHandleNew failed (!)
// *
// * POSTCONDITION:	Since this routine locks the handle it returns if successful, the
// *				caller is responsible for calling ARC4UnlockGlobals() when s/he is done.
// *				Remember, the sooner you do this, the more you prevent heap fragmentation.
// *
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/18/99	Initial Implementation
// *****
ARC4GlobalsTypePtr ARC4AllocGlobals( UInt16 uRefNum )
{

	ARC4GlobalsTypePtr		gP = NULL;
	SysLibTblEntryPtr		sysLibEntryP;
	MemHandle				gH = NULL;
	
	ErrFatalDisplayIf(sysInvalidRefNum == uRefNum, "Invalid refnum.");

	// Fetch a ptr to our lib's table entry in the OS's array (refnum is index)
	sysLibEntryP = SysLibTblEntry( uRefNum );
	ErrFatalDisplayIf( NULL == sysLibEntryP, "Invalid refnum.");
	ErrFatalDisplayIf( sysLibEntryP->globalsP, "Lib globals ptr already exists.");
	
	gH = MemHandleNew( sizeof(ARC4GlobalsType) );				// Alloc mem for globals here
	if ( !gH )
		return ( NULL );
		
	sysLibEntryP->globalsP = (void*)gH;							// Store handle in lib entry
	
	gP = (ARC4GlobalsTypePtr) ARC4LockGlobals( uRefNum );			
	ErrFatalDisplayIf( !gP, "Unable to lock lib globals ptr.");

	// We need to set the owner of this chunk to 'system'.  If we don't do this, then
	// the memory manager will automatically free this when the first application to
	// call ARC4Open() exits.  Since we intend this library (and its globals) to hang around
	// regardless of which app begins and exits, we need to do this:
	MemPtrSetOwner( gP, 0 );									// 0 == OS
	
	MemSet( gP, sizeof(ARC4GlobalsType), 0 );					// Clean it out

	// Globals should be initialized in your lib's Open entry point... see ARC4Open()
	return ( gP );
}


// *****
// * FUNCTION: 		ARC4FreeGlobals
// * 
// * DESCRIPTION:	Deallocate a lib's globals ptr, given its RefNum.
// *
// * PARAMETERS:	uRefNum		-		Lib refnum whose globals we'll deallocate
// *			
// * RETURNED:		ErrNone					-	MemHandleNew success
// *				!0						-	failure, MemHandleNew failed 
// *
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/18/99	Initial Implementation
// *****
Err ARC4FreeGlobals( UInt16 uRefNum )
{
	SysLibTblEntryPtr		sysLibEntryP;
	MemHandle				gH = NULL;
	
	ErrFatalDisplayIf( sysInvalidRefNum == uRefNum, "Invalid refnum.");
	
	sysLibEntryP = SysLibTblEntry( uRefNum );
	ErrFatalDisplayIf( NULL == sysLibEntryP, "Invalid refnum.");
	
	gH = (MemHandle) (sysLibEntryP->globalsP);					// Get our globals handle
	ErrFatalDisplayIf(!gH, "Lib globals ptr does not exist.");

	sysLibEntryP->globalsP = NULL;
	return( MemHandleFree(gH) );
}

// *****
// * FUNCTION: 		ARC4LockGlobals
// * 
// * DESCRIPTION:	Return a ptr to a particular lib's ARC4GlobalsType structure
// *
// * PARAMETERS:	uRefNum		-		Lib refnum whose globals we'll lock
// *			
// * RETURNED:		0			-		Caller needs to allocate them first with ARC4AllocGlobals()!
// *				Valid ptr	-		success
// *
// * POSTCONDITION:	If I return 0, the caller needs to ARC4AllocGlobals().
// *
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/18/99	Initial Implementation
// *****
ARC4GlobalsTypePtr ARC4LockGlobals( UInt16 uRefNum )
{
	ARC4GlobalsTypePtr		gP 				= NULL;						// Necessary!
	SysLibTblEntryPtr		sysLibEntryP 	= NULL;
	MemHandle				gH				= NULL;
	
	ErrFatalDisplayIf( sysInvalidRefNum == uRefNum, "Invalid refnum.");
	
	sysLibEntryP = SysLibTblEntry( uRefNum );
	ErrFatalDisplayIf( NULL == sysLibEntryP, "Invalid refnum.");
	
	gH = (MemHandle) (sysLibEntryP->globalsP);

	// We don't ErrFatalDisplay here if !gH.  This is so the caller can check the return
	// value and if it's null, the caller knows s/he needs to ARC4AllocGlobals(), similar
	// to the behavior of SysLibFind() and SysLibLoad()ing something.
	if (gH)
	{
		gP = (ARC4GlobalsTypePtr)MemHandleLock( gH );
	}
	
	// Notice we want to return NULL if this handle hasn't yet been allocated!
	
	return gP;
}

// *****
// * FUNCTION: 		ARC4UnlockGlobals
// * 
// * DESCRIPTION:	Unlock a ptr to a ARC4GlobalsType structure
// *
// * PRECONDITION:	gP has been locked down by a call to ARC4LoclGlobals.
// *
// * PARAMETERS:	gP			-		Locked ptr to structure
// *			
// * RETURNED:		!0			-		MemPtrUnlock failure (!)
// *				ErrNone		-		MemPtrUnlock success
// *
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/18/99	Initial Implementation
// *****
Err ARC4UnlockGlobals( ARC4GlobalsTypePtr gP )
{
	return( MemPtrUnlock(gP) );											// No magic here..
}



#pragma mark -
// OS-Required entry point implementations:

// *****
// * FUNCTION: 		ARC4Open
// * 
// * DESCRIPTION:	Open ARC4; alloc globals if necessary
// *
// * PRECONDITION:	Caller has already done a SysLibFind and SysLibLoad to get refnum
// *
// * PARAMETERS:	uRefNum		-		Lib refnum 
// *			
// * RETURNED:		ARC4ErrNone			-	success
// *				ARC4ErrNoGlobals		-   unable to allocate globals
// *
// * POSTCONDITION:	Caller should ARC4Close() this lib as soon as s/he is done using it.
// *				Multiple ARC4Opens() are ok, but each one should always have a 
// *				correspoding ARC4Close() to balance it out.
// *
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/19/99	Initial Implementation
// *****
ARC4Err ARC4Open( UInt16 uRefNum )
{
	Err						err;
	ARC4GlobalsTypePtr		gP = NULL;
	
	// Allocate globals
	ErrFatalDisplayIf( sysInvalidRefNum == uRefNum, "Invalid refnum.");
	
	gP = ARC4LockGlobals( uRefNum );
	
	// If this returns NULL, that means we need to allocate the globals.  This also
	// implies that this is the first time we've opened this shared library.  
	if ( !gP )
	{
		gP = ARC4AllocGlobals( uRefNum );
		if ( !gP )
			return ARC4ErrNoGlobals;
			
		// Initialize globals here:
		gP->iOpenCount = 1;

		// Dump diagnostic info, i.e. "ARC4 ref# %d initially opened; globals initialized.\n", uRefNum
	}
	else
		gP->iOpenCount++;
		
	err = ARC4UnlockGlobals( gP );
	ErrFatalDisplayIf( err, "Unable to unlock lib globals.");			
	
	return ARC4ErrNone;
}

// *****
// * FUNCTION: 		ARC4Close
// * 
// * DESCRIPTION:	Close ARC4; free globals if necessary
// *
// * PARAMETERS:	uRefNum			-				Lib refnum 
// *				dwRefCountP		- (Modified)	DWord into which we put the open count
// *			
// * RETURNED:		ARC4ErrNone		-	success
// *				ARC4ErrNoGlobals	-	Unable to lock down the globals, this is bad
// *
// * POSTCONDITION:	Caller should ALWAYS check dwRefCount upon successful return.  If it's
// *				zero, caller should SysLibRemove() this library as it's no longer in use.
// *
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/19/99	Initial Implementation
// *****
ARC4Err ARC4Close( UInt16 uRefNum, UInt32* dwRefCountP )
{
	Err						err;
	ARC4GlobalsTypePtr		gP = NULL;
	
	ErrFatalDisplayIf( sysInvalidRefNum == uRefNum, "Invalid refnum.");
	
	if ( !dwRefCountP )														// Validate param
		return ARC4ErrParam;
		
	gP = ARC4LockGlobals ( uRefNum );
	if ( !gP )
		return ARC4ErrNoGlobals;
	
	gP->iOpenCount--;
	ErrNonFatalDisplayIf( gP->iOpenCount < 0, "Library globals underlock." );

	*dwRefCountP = gP->iOpenCount;
		
	ARC4UnlockGlobals( gP );

	if ( *dwRefCountP <= 0 )		// Use this instead of gP->iOpenCount, since we just
	{								// unlocked gp!
		// Dump diagnostic info i.e.  "ARC4 ref# %d closed; globals freed.", uRefNum		
		err = ARC4FreeGlobals( uRefNum );								
		ErrFatalDisplayIf( err, "Unable to free lib globals.");
	}
	
	return ARC4ErrNone;
}

// *****
// * FUNCTION: 		ARC4Sleep
// * 
// * DESCRIPTION:	Called when device goes to sleep.  Since this routine can sometimes be
// *				called from an interrupt handler, you can never spend a lot of time in 
// *				this routine or else you'll make the system unstable and probably cause
// *				mysterious crashes.  In addition, this routine is called as a result of
// *				a battery pull situation; in that case, the Palm is running off of its
// *				super cap, which means there's about 1 ms of processor time remaining
// *				before there is no power.  To avoid catastrophic failure, you and any
// *				other installed shared libraries had better not take up too many cycles
// *				in their respective Sleep function!!
// *
// * PARAMETERS:	uRefNum		-		Lib refnum
// *			
// * RETURNS:		Always 0.  I don't know who uses this return value, or if it's needed.
// *
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/19/99	Initial Implementation
// *****
Err ARC4Sleep( UInt16 uRefNum )
{
	// If you were implementing custom hardware, you'd do something like
	// this to put it to sleep to conserve power, and to prevent it from
	// sapping the super cap in the event of a battery pull:

	#ifdef MY_HARDWARE_INSTALLED	// ... fictitious example #define ...
		// Tell MyHardware to power down, and then return ASAP!
		MyHardwareBaseAddr->pwrCtlReg |= SLEEP_MODE;
	#endif
	
	return 0;
}


// *****
// * FUNCTION: 		ARC4Wake
// * 
// * DESCRIPTION:	Called when device wakes up from sleep.  Since this routine is sometimes
// *				called from an interrupt handler, you can never spend a lot of time in 
// *				this routine or else you'll make the system unstable and probably cause
// *				mysterious crashes.  If you have a time-consuming chore to do, consider
// *				using an interrupt-safe routine like EvtEnqueueKey() to set a flag.  In
// *				an EvtGetEvent hook, you can see this flag come through.  Since you're no
// *				longer in an intrreupt handler, you can do your time-consuming chore at 
// *				that time.
// *
// * PARAMETERS:	uRefNum		-		Lib refnum
// *
// * RETURNS:		Always 0.  I don't know why uses this return value, or if it's needed.
// *			
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/19/99	Initial Implementation
// *****
Err ARC4Wake( UInt16 uRefNum )
{
	// If you were implementing custom hardware, you'd do something like
	// this to wake your hardware back up:

	#ifdef MY_HARDWARE_INSTALLED	// ... fictitious example #define ...
		// Tell MyHardware to wake up from sleep mode
		MyHardwareBaseAddr->pwrCtlReg &= ~SLEEP_MODE;
	#endif
	
	return 0;
}


#pragma mark -
// Custom lib function implementation.  These are the extern'd functions.

/***********************************************************************
 *
 * FUNCTION:    ARC4Initialize
 *
 * DESCRIPTION: This routine Initializes ARC4.
 *
 * PARAMETERS: 
 *				UInt refNum:  A reference number 
 *
 * RETURNED:    
 *				
 *
 *
 *
 * REVISION HISTORY:
 *			Name	Date		Description
 *			----	----		-----------
 *		
 *
 ***********************************************************************/
extern ARC4Err	ARC4Initialize(UInt16 refNum, unsigned char * keystring, int keysize, ARC4_CTX * key)
{
	ARC4_Setkey (key, keystring, keysize);
	return ARC4ErrNone;
}

/***********************************************************************
 *
 * FUNCTION:    ARC4EncryptARC4
 *
 * DESCRIPTION: This routine encrypts a string using ARC4.
 *
 * PARAMETERS: 
 *				UInt refNum:  A reference number 
 *
 * RETURNED:    error if found
 *
 * REVISION HISTORY:
 *			Name	Date		Description
 *			----	----		-----------
 *		
 *
 ***********************************************************************/

extern Int16	ARC4Encrypt
(UInt16 refNum, ARC4_CTX * key, unsigned char * message, unsigned char * out, unsigned long size)
{
	return ARC4_Encrypt(key, out, message, size);
}	

/***********************************************************************
 *
 * FUNCTION:    ARC4DecryptARC4
 *
 * DESCRIPTION: This routine decrypts a string using ARC4.
 *
 * PARAMETERS: 
 *				UInt refNum:  				A reference number 
 *				ARC4_CTX * key:  			A pointer to the Key information.
 *				unsigned char * message: 	A pointer to some allocated memory that contains the message 
 *											to be encrypted.
 *				unsigned char * out:  		A pointer to to some allocated memory where the output will be 
 * 											stored.  Same size as the message.
 *
 * RETURNED:    Error if found.
 *
 * REVISION HISTORY:
 *			Name	Date		Description
 *			----	----		-----------
 *	
 *
 ***********************************************************************/
extern Int16 ARC4Decrypt
	(UInt16 refNum, ARC4_CTX * key, unsigned char * message, unsigned char * out, unsigned long size)
{
	return ARC4_Decrypt(key, out, message, size);
}	