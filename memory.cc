
/****************************************************************************
 *                                                                          *
 * KlassenTemplate zur Speicherverwaltung                                   *
 * ======================================                                   *
 * (C)opyright 21.08.1994 by Mark-André Hopf, MAGIC INC.                    *
 *                                                                          *
 * 24.05.1995 : Eine UNIX Variante programmiert. Diese Art des Speicher-    *
 *              zugriffs sollte für einen späteren Wechsel zurück auf       *
 *              Windows beibehalten werden.                                 *
 *                                                                          *
 ****************************************************************************/

#include "memory.hh"

#ifndef ULONG
#define ULONG unsigned long
#endif

template <class T> void Memory<T>::Init(ULONG size)
{
	dwSizeInByte=size*sizeof(T);
	dwSize=size;
	lpMemory=new T[size];
}

template <class T> Memory<T>::~Memory()
{
	if (lpMemory)
		delete[] lpMemory;
	lpMemory=NULL;
}

template <class T> void Memory<T>::Clear()
{
	memset(lpMemory, 0, dwSizeInByte);
}

template <class T> void Memory<T>::Clear(T a)
{
	T FAR* ptr=lpMemory;
	ULONG i;

	for(i=0; i<dwSize; i++)
		*(ptr++)=a;
}

