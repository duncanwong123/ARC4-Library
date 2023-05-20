// *****
// * PROJECT:		ARC4Lib (ARC4)
// * FILENAME: 		ARC4Lib.h
// * AUTHOR:		Hector Ho Fuentes
// *
// *				ARC4 is the reverse engeneered version of RC4
// * 
// * DESCRIPTION:	ARC4 Shared library functionality interface definition
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
// * FILENAME: 		MySharedLib.h
// * AUTHOR:		Jeff Ishaq 05/21/99
// * 
// * DESCRIPTION:	Shared library functionality interface definition
// *
// * COPYRIGHT:		As long as this 'copyright' is intact, this code is freely modifiable
// *				and distributable.
// *****
#pragma once

// Use this for SysLibFind calls.  This is what we 'name' our dispatch table, too:
#define ARC4_LIB_NAME	"ARC4Library"
#define ARC4_LIB_CREATOR	'HRC4'		 						// Register this with Palm

// These are possible error types that ARC4 might return:
typedef enum tagARC4ErrEnum
{
	ARC4ErrNone 			= 0,		 	
	ARC4ErrParam			= -1,
	ARC4ErrNoGlobals		= -2,

	/////
	// Your custom return codes go here...
	/////
	ARC4ErrKeySize			= -3
	
} ARC4Err;

// These are ARC4's trap identifiers.  The PalmOS constant 'sysLibTrapCustom' is
// the first trap number we can use after open, close, sleep, and wake.
typedef enum tagARC4TrapNumEnum
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

	ARC4TrapInitialize = sysLibTrapCustom,		// libDispatchEntry(4)
	ARC4TrapEncrypt,							// libDispatchEntry(5)
	ARC4TrapDecrypt								// libDispatchEntry(6)
} ARC4TrapNumEnum;


typedef struct {
	unsigned int x;
	unsigned int y;
	unsigned int state[256];
} ARC4_CTX;

#ifdef __cplusplus
extern "C" {
#endif

// These are the four required entry points:
extern ARC4Err	ARC4Open	( UInt16 uRefNum )						SYS_TRAP ( sysLibTrapOpen);
extern ARC4Err	ARC4Close( UInt16 uRefNum, UInt32* dwRefCountP )	SYS_TRAP ( sysLibTrapClose);
extern Err		ARC4Sleep( UInt16 uRefNum )						SYS_TRAP ( sysLibTrapSleep);
extern Err		ARC4Wake	( UInt16 uRefNum )						SYS_TRAP ( sysLibTrapWake	);

// Here are the actual functions we want the library to extend to callers.
extern ARC4Err	ARC4Initialize(UInt16 refNum, unsigned char * keystring, int keysize, ARC4_CTX * key) 
				SYS_TRAP(ARC4TrapInitialize);
				
extern Int16	ARC4Encrypt(UInt16 refNum, ARC4_CTX * key, unsigned char * in, unsigned char * out, unsigned long size) 
				SYS_TRAP(ARC4TrapEncrypt);
				
extern Int16 ARC4Decrypt(UInt16 refNum, ARC4_CTX * key, unsigned char * in, unsigned char * out, unsigned long size) 
				SYS_TRAP(ARC4TrapDecrypt);
				
#ifdef __cplusplus
}
#endif
