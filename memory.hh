#ifndef _MEMORY_HH
#define _MEMORY_HH

#include <string.h>

template <class T> class Memory
{
	private:
		ULONG						dwSize;					// Anzahl von Elementen des Typs T
		ULONG						dwSizeInByte;		// Größe des Speicherblocks in Byte
		T*          		lpMemory;				// Zeiger auf den Speicher
		#ifdef SECURE
		T								null;
		#endif
	public:
		Memory(void)
		{
			#ifdef DEBUG
			fprintf(stderr, "memory created without size\n");
			#endif
			lpMemory=NULL;
		}
		Memory(ULONG size){Init(size);}
		~Memory();
		void Init(ULONG size);
		void Clear(void);
		void Clear(T a);
		operator T*(){return lpMemory;}
		T& operator[](ULONG pos)
		{
			if (lpMemory && pos<dwSize)
			{
				return lpMemory[pos];
			}
			#ifndef SECURE
			return lpMemory[0];
			#else			
			else
			{
				if (lpMemory)
					fprintf(stderr, "Memory.operator[]: access outside it's buffer");
				else
					fprintf(stderr, "template class Memory: access without allocated buffer");
			}
			return null;
			#endif
	};
};

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
	T* ptr=lpMemory;
	ULONG i;

	for(i=0; i<dwSize; i++)
		*(ptr++)=a;

}
#endif
