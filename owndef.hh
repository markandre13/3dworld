// owndef.hh
// an <windows.h> und C++ angepasste Definitionen

// #define DEBUG
#define SECURE

#ifndef __OWNDEF_HH
#define __OWNDEF_HH

/****************************************************************************
 *																																					*
 * Typen                                                                    *
 *																																					*
 ****************************************************************************/
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   UINT;
typedef long		 			 LONG;
typedef unsigned long  ULONG;
typedef unsigned long  DWORD;

/*
typedef int bool;
const bool true =1;
const bool false=0;
*/

#ifdef DEBUG
	#ifndef SECURE
		#define SECURE
	#endif
#endif

#ifdef SECURE
	#include <stdio.h>
	#include <stdlib.h>
	#define RTMESSAGE(text) printf("RUNTIMEERROR IN "__FILE__" AT LINE %lu: "#text"\n",(ULONG)__LINE__)
	#define MESSAGE(text) printf("MESSAGE FROM "__FILE__" AT LINE %lu: "#text"\n",(ULONG)__LINE__)
#endif	

#endif