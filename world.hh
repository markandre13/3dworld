// world.hh
#ifndef _RENDER
#include "render.hh"
#endif

typedef unsigned long XID;
typedef XID KeySym;

#define DISPLAYTYPE_VECTOR	0
#define DISPLAYTYPE_ZBUFFER	1

class World
{
	private:
		View *pView;									// Zeiger auf ein Ausgabefenster
	public:
		World(View *parent);
		void SetDisplayType(int);
		void Calculate();		 					// Ein Bild berechnen und darstellen
		KeySym Getch(){return pView->Getch();}
};
