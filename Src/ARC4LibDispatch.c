// *****
// * PROJECT:		ARC4Lib (ARC4)
// * FILENAME: 		ARC4Lib.h
// * AUTHOR:		Hector Ho Fuentes
// *
// *				ARC4 is the reverse engeneered version of RC4
// * 
// * DESCRIPTION:	Installation routine for shared library's dispatch table.
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
// * FILENAME: 		MySharedLibDispatch.c
// * AUTHOR:		Jeff Ishaq 05/21/99
// * 
// * DESCRIPTION:	Installation routine for shared library's dispatch table.
// *
// *
// * HISTORY:		
// *
// *
// * COPYRIGHT:		As long as this 'copyright' is intact, this code is freely modifiable
// *				and distributable.
// *****

// Because we play with #defines that you're not normally expected to play with,
// we tend to run into cryptic link errors by including precompiled headers:
#ifndef PILOT_PRECOMPILED_HEADERS_OFF
	#define	PILOT_PRECOMPILED_HEADERS_OFF
#endif

#define EMULATION_LEVEL		EMULATION_NONE		// Force this to no emulation:

#undef 		__PALMOS_TRAPS__					// To prevent a redeclaration error
#define 	__PALMOS_TRAPS__ 	0				// Now, define this ourselves
#define		USE_TRAPS 			0				// To _make_ traps, we need to turn this off

#include <PalmOS.h>
#include "ARC4Lib.h"

Err __Startup__( UInt16 uRefNum, SysLibTblEntryPtr entryP );
static MemPtr	asm ARC4_DispatchTable(void);

// *****
// * FUNCTION: 		__Startup__
// * 
// * DESCRIPTION:	Called to install the library by SysLibLoad().  You mustn't change
// *				the function's signature.
// *
// * PARAMETERS:	uRefNum		-		Lib refnum
// *				entryP		-		Ptr to our entry in the OS's lib table
// *
// * RETURNS:		Always 0.  I don't know why uses this return value, or if it's needed.
// *			
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/19/99	Initial Implementation
// *****
Err __Startup__( UInt16 uRefNum, SysLibTblEntryPtr entryP )
{
	// Stash our dispatch table's address into the OS's shared libaray
	// table ptr slot that corresponds to this uRefNum:
	entryP->dispatchTblP = (MemPtr*) ARC4_DispatchTable();
	
	// Zero the globals ptr so that our call to MSLAllocGlobals() does the
	// right thing:
	entryP->globalsP = 0;

	return 0;
}

#define prvJmpSize	4				// How many bytes a JMP instruction occupies
#define NUMBER_OF_FUNCTIONS	7		// Don't forget to update this if necessary!!

#define TABLE_OFFSET 			2 * (NUMBER_OF_FUNCTIONS + 1)

#define ARC4_DISPATCH_SLOT(i)	(TABLE_OFFSET + ( (i) * prvJmpSize))

// *****
// * FUNCTION: 		MSL_DispatchTable
// * 
// * DESCRIPTION:	The actual dispatch table.  The linker will run through here and
// *				put the addresses of the JMP'd-to functions.
// *
// * RETURNS:		(Ptr)This
// *			
// * REVISION HISTORY:
// *	NAME	DATE		DESCRIPTION
// *    -------------------------------------------------------------------------------
// *	JeffI	05/19/99	Initial Implementation
// *****
static MemPtr	asm	ARC4_DispatchTable( void )
{
	/////
	// - Trap modification checklist -
	// 
	// If you add or remove or otherwise modify something here, be sure you've
	// also done all of the following steps!
	//
	// 0) All trap identifiers must always run sequentially; no gaps!
	// 1) Modify the ARC4TrapNumEnum in MySharedLib.h
	// 2) Modify the DC.W to ARC4_DispatchTable() in MySharedLibDispatch.c (no gaps!)
	// 3) Modify the JMP in ARC4_DispatchTable() in MySharedLibDispatch.c (no gaps!)
	// 4) ** Update NUMBER_OF_FUNCTIONS in MySharedLibDispatch.c ** (0-based)
	// 5) Add or remove an "extern MyFunc(...) SYS_TRAP(ARC4TrapMyFunc)" prototype somewhere
	//
	/////

	LEA	@TableStart, A0
	RTS
	
@TableStart:
	DC.W		@LibName
	
	DC.W		ARC4_DISPATCH_SLOT(0)						// ARC4Open()
	DC.W		ARC4_DISPATCH_SLOT(1)						// ARC4Close()
	DC.W		ARC4_DISPATCH_SLOT(2)						// ARC4Sleep()
	DC.W		ARC4_DISPATCH_SLOT(3)						// ARC4Wake()
	DC.W		ARC4_DISPATCH_SLOT(4)						// ARC4Initialize
	DC.W		ARC4_DISPATCH_SLOT(5)						// ARC4TrapEncrypt
	DC.W		ARC4_DISPATCH_SLOT(6)						// ARC4TrapDecrypt
	
	JMP			ARC4Open									// 0
	JMP			ARC4Close									// 1
	JMP 		ARC4Sleep									// 2
	JMP			ARC4Wake									// 3
	JMP			ARC4Initialize								// 4
	JMP			ARC4Encrypt									// 5
	JMP			ARC4Decrypt									// 6
	
	
@LibName:
	DC.B		ARC4_LIB_NAME								// SysLibFind()'s name key
}