/*****************************************************************************
 *                                                                           *
 * 3D World v0.02 - a three dimensional graphic program                      *
 * (C)opyright 1994,95 Mark-Andre Hopf                                       *
 *                     Baumgarten 33                                         *
 *                     D-30966 Hemmingen                                     *
 *                                                                           *
 * e-mail: hopf@informatik.uni-rostock.de                                    *
 * www   : http://www.informatik.uni-rostock.de/~hopf.homepage.html          *
 *                                                                           *
 * Greetings to Tim, Martin, Gita, Jens, Gerd & Juli, Tilo & Jule, Bärbel,   *
 * Dieter & Ilona, Jan, Elli, Miner Willy, Martin Galway, Rob Hubard, David  *
 * Whitaker, Pete Cooke and the authors of Elite, Spindizzy and Alien 8.     *
 *                                                                           *
 * 04.06.1994	: Begonnen mit einer Routine zum Linienzeichnen (DOS)    		   *
 * 07.06.1994		Eine eigene, kleine Version von Borlands OWL2 begonnen       *
 * 08.06.1994		Den OWL2 Thunk-Mechanismus übernommen und begriffen.         *
 * 08.06.1994		MWindow und den Thunk-Mechanismus auf meinen eigenen         *
 *              Programmierstill abgestimmt, der näher an Windows ist.       *
 * 29.06.1994 : Erste Poly Routine zum Zeichnen von 2dimensionalen Drei-     *
 *              ecken begonnen. (DOS)                                        *
 * 30.06.1994 : Eine nicht optimierte Variante von ClipPoly fertiggestellt   *
 *              und einen Fehler in Bresenham korrigiert (DOS)               *
 * 01.07.1994 : Den Z-Buffer begonnen und nach Windows gewechselt            *
 * 04.07.1994 : Z-Buffer funktioniert, Matrixklasse eingefügt, eigene OWL2   *
 *              Variante entfernt                                            *
 * 15.08.1994 : Den seit einem Monat gesuchten Fehler im z-Buffer-Alg.       *
 *              gefunden: sy des Sichtvektors mußte noch negiert werden...   *
 * 17.08.1994 : Neu programmierte Matrixklasse eigefügt.                     *
 * 20.08.1994 : Fensterklassen Untermenge aus OWL2 wieder eingefügt und ver- *
 *              feinert                                                      *
 * 21.08.1994 : Debug Fenster entwickelt                                     *
 *              Seltsamem Fehler begegnet: Der Thunkmechanismus stürzte an-  *
 *              dauernd ab, da der Compiler LoByte & HiByte verkehrtherum    *
 *              hinter einem 'gepokten' call rel (0xE8) ablegte. Nachdem ich *
 *              zufällig die Compileroption Datenausrichtung Byte-Word umge- *
 *              schaltet hatte, funktionierte alles wieder wie gewohnt.      *
 * 23.08.1994 : Die Ursache dieses "seltsamen" Fehlers ist jetzt klar        *
 *              an der OWL weiterprogrammiert, die nun MAGIC OWL 1.0 heißt   *
 * 29.08.1994 : Eine Ahnung davon bekommen, wie Borland das mit den Dialog-  *
 *              fenstern gemacht haben könnte.                               *
 * 06.09.1994 : Dialogboxen funktionieren, restliche Variablen in Klassen    *
 *              gepackt                                                      *
 * 07.09.1994 : die Klasse 'Display' begonnen                                *
 * 14.10.1994 : Drei der MagicOWL Dateien in eine Datei zusammengefasst      *
 * 03.11.1994 : Auf Win32s umgestiegen, Versionsnummer auf 0.01 geändert,    *
 *              alle Graphikfunktionen nach RENDER.CPP verschoben            *
 * 04.11.1994 : RENDER.CPP vorläufig wieder nach 3DWORL32.CPP genommen, um   *
 *              unnötiges Compilieren in den Headerdateien zu vermeiden;     *
 *              Display hat ein eigenes ChildWindow bekommen                 *
 * 11.12.1994 : Befehle der Grafik Primitive OpenGL angenähert               *
 * 18.01.1995 : MDI Clientfenster sind keine Hauptfenster, sondern lediglich *
 *              Fenster, die den Clientbereich verwalten und sollten (müßen?)*
 *              nach Microsoft Kind einer Hauptfensters sein.                *
 * 26.01.1995 : Bis läuft das Erzeugen von ChildWindows ganz einfach ab,     *
 *              indem das ChildWindow mit einem this-Zeiger auf das Eltern-  *
 *              fenster erzeugt wird. Eine Modifikation ist nun bei Create() *
 *							so vorzunehmen, daß, wenn das Elternfenster ein              *
 *							MDI FrameWindow ist, daß dann das Handel des MDI Client      *
 *							Window verwendet wird:                                       *
 *              (1) Diesen Fall speziell in Create() berücksichtigen, oder   *
 *              (2) Die Zugriffsmöglichkeit auf HWindow einschränken         *
 *              Vorher herausfinden, welche Nachrichten von den Kindern an   *
 *              MDI Frame- und Clientwindow geschickt werden.                *
 * 24.05.1995 : Die Windowsklassenbibliothek müßte noch einmal gründlich     *
 *              überarbeitet werden; eine UNIX Variante des Programms        *
 *              probiert, Versionsnummer auf v0.02 erhöht                    *
 * 25.05.1995 : Kleinere Bugs gefunden; OutlineView funktioniert jetzt;      *
 *              Bewegen mit der Tastatur; Fehler im Clipping-Algorithmus     *
 *              notdürftig beseitigt; Programm hängt, wenn man auf dem       *
 *              Boden steht und in die Höhe schaut, sollte den ganzen Code   *
 *              nocheinmal komplett gründlich durchlesen.                    * 
 * 26.05.1995 : Noch 'nen Bug in FillPolybuffer gefunden; aufräumen und      *
 *              Codeoptimierung tun Not; kleinere Probleme beim Clippen      *
 * 29.05.1995 : Durch Bug Beseitigung in FillPolybuffer entstandenen Bug     *
 *              beseitigt und diesen Fehler entdeckt.
 *              - in ganz seltenen Fällen werden gefüllte Polygone nicht     *
 *                richtig gezeichnet (Gehwegpolygon ohne rechte untere       *
 *                Grenze)                                                    *
 *                                                                           *
 *****************************************************************************/

#include <X11/keysym.h>
#include <math.h>
#include "world.hh"
#include "render.hh"
#include "matrix.hh"

// #define WORLD_SMALL

void test(View &P);

double px,py,pz, dx,dy,dz;

int main()
{
	printf("3D World v0.02\n"
				 "(C)opyright 1994,95 by Mark-Andre Hopf\n"
				 "(X-Windows version, the whole stuff is still containing bugs)\n\n"
				 "A,Y                : look up & down\n"
				 "D,C                : move up & down\n"
				 "Cursor up & down   : go forward & backward\n"
				 "Cursor left & right: turn left & right\n"
				 "V                  : vector display\n"
				 "Z                  : z-buffer display\n"
				 "Q                  : quit\n");
	World *pWorld=new World(NULL);	// erzeugt das neue Fenster

	px=-25.0;												// unsere Position
	py=-10.0;
	pz=-50.0;
	
	dx=dz=0.0;
	dy=0.0; //M_PI_2 * 3.0;
	
	bool done=false;
	while(!done)
	{
		pWorld->Calculate();
		switch(pWorld->Getch())
		{
			case XK_Q: case XK_q:
				done=true;
				break;
			case XK_Left: case XK_m: case XK_M:
				dy+=0.2;
				break;	
			case XK_Right: case XK_n: case XK_N:
				dy-=0.2;
				break;
			case XK_Up: case XK_space:
				px+=sin(dy)*5.0;
				pz-=cos(dy)*5.0;
				break;		
			case XK_Down: case XK_b: case XK_B:
				px-=sin(dy)*5.0;
				pz+=cos(dy)*5.0;
				break;
			case XK_z: case XK_Z:
				pWorld->SetDisplayType(DISPLAYTYPE_ZBUFFER);
				break; 
			case XK_v: case XK_V:
				pWorld->SetDisplayType(DISPLAYTYPE_VECTOR);
				break; 
			case XK_a: case XK_A:
				dx-=0.1;
				break;	
			case XK_y: case XK_Y:
				dx+=0.1;
				break;
			case XK_d: case XK_D:
				py-=5.0;
				break;	
			case XK_c: case XK_C:
				py+=5.0;
				break;
			case XK_g: case XK_G:
				break;											// for debugging add breakpoint here!
		}
	}
	delete pWorld;
}
 
World::World(View *parent)
{
	pView=NULL;
	SetDisplayType(DISPLAYTYPE_VECTOR);
}

void World::SetDisplayType(int type)
{
	if (pView) delete pView;
	switch(type)
	{
		case DISPLAYTYPE_ZBUFFER:
			pView=new ZBufferView(NULL, 640,290);
			break;
		default:
			pView=new OutlineView(NULL, 640,290);
	}
}

void World::Calculate()
{
	test(*pView);
	XFlush(pView->display);
}

// ein kleines Häuschen zeichnen
void test(View &D)
{
	// Drehmatrix
	double na[]={	1.0, 0.0, 0.0, 0.0,
								0.0, 1.0, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0,
								0.0, 0.0, 0.0, 1.0 };
	// Haus
	//                                                                                            |          Rasen        |     Fenster 
	//             0     1     2     3    4     5     6     7     8     9    10    11    12    13    14    15    16    17    18   19   20   21
	double ca[]={0.0,100.0,100.0, 50.0, 0.0,  0.0,100.0,100.0, 50.0,  0.0,100.0,100.0,100.0,100.0,-50.0,150.0,150.0,-50.0,  0.0, 0.0, 0.0, 0.0,  
							 0.0,  0.0, 75.0,100.0,75.0,  0.0,  0.0, 75.0,100.0, 75.0,  0.0, 65.0,  0.0, 65.0,  0.0,  0.0,  0.0,  0.0, 20.0,60.0,60.0,20.0,
							 0.0,  0.0,  0.0,  0.0, 0.0,150.0,150.0,150.0,150.0,150.0, 55.0, 55.0, 95.0, 95.0,-50.0,-50.0,200.0,200.0, 55.0,55.0,95.0,95.0,
							 1.0,  1.0,  1.0,  1.0, 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, 1.0, 1.0, 1.0 };

	// Straße
	double sa[]={150.0, 200.0, 200.0, 150.0,
							   0.0,   0.0,   0.0,   0.0,
							 -50.0, -50.0, 450.0, 450.0,
							   1.0,   1.0,   1.0,   1.0  };

	matrix d(4,4),c(4,22), c2(4,22), s(4,4), s2(4,4);

	d=na;																// Einheitsmatrix
	c=ca;																// Hausmatrix
	s=sa;																// Straßenmatrix

	Translate3D(d,px, py, pz);					// Position wählen
	c2=d*c;															// eine Sauerei hier zu multiplizieren
	s2=d*s;

	d=na;
	Rotate3D(d, dx, dy, dz);						// Drehen
	c2=d*c2;
	s2=d*s2;

	D.Clear();													// Zeichnen beginnen

	//  Straße
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(s2(0,0), s2(1,0), s2(2,0));
	D.Vect3D(s2(0,1), s2(1,1), s2(2,1));
	D.Vect3D(s2(0,2), s2(1,2), s2(2,2));
	D.Vect3D(s2(0,3), s2(1,3), s2(2,3));
	D.EndObject();

#ifndef WORLD_SMALL	
	//  vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(3,0,0);
	D.Vect3D(c2(0,0), c2(1,0), c2(2,0));
	D.Vect3D(c2(0,1), c2(1,1), c2(2,1));
	D.Vect3D(c2(0,2), c2(1,2), c2(2,2));
	D.Vect3D(c2(0,3), c2(1,3), c2(2,3));
	D.Vect3D(c2(0,4), c2(1,4), c2(2,4));
	D.EndObject();

	//  hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(1,0,0);
	D.Vect3D(c2(0,5), c2(1,5), c2(2,5));
	D.Vect3D(c2(0,6), c2(1,6), c2(2,6));
	D.Vect3D(c2(0,7), c2(1,7), c2(2,7));
	D.Vect3D(c2(0,8), c2(1,8), c2(2,8));
	D.Vect3D(c2(0,9), c2(1,9), c2(2,9));
	D.EndObject();

	//  links von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 1), c2(1, 1), c2(2, 1));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,10), c2(1,10), c2(2,10));
	D.EndObject();

	//  über der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  rechts von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0,12), c2(1,12), c2(2,12));
	D.Vect3D(c2(0, 6), c2(1, 6), c2(2, 6));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  Dach hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(5,0,0);
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.EndObject();

	//  Dach vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(4,0,0);
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.EndObject();

	//  Rasen
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(6,0,0);
	D.Vect3D(c2(0,14), c2(1,14), c2(2,14));
	D.Vect3D(c2(0,15), c2(1,15), c2(2,15));
	D.Vect3D(c2(0,16), c2(1,16), c2(2,16));
	D.Vect3D(c2(0,17), c2(1,17), c2(2,17));
	D.EndObject();


///////////////////////////////////////////////////////////////7
	d=na;																// Einheitsmatrix
	c=ca;																// Hausmatrix

	Translate3D(d,px, py, pz+250.0);					// Position wählen
	c2=d*c;															// eine Sauerei hier zu multiplizieren

	d=na;
	Rotate3D(d, dx, dy, dz);						// Drehen
	c2=d*c2;

	//  vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(3,0,0);
	D.Vect3D(c2(0,0), c2(1,0), c2(2,0));
	D.Vect3D(c2(0,1), c2(1,1), c2(2,1));
	D.Vect3D(c2(0,2), c2(1,2), c2(2,2));
	D.Vect3D(c2(0,3), c2(1,3), c2(2,3));
	D.Vect3D(c2(0,4), c2(1,4), c2(2,4));
	D.EndObject();

	//  hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(1,0,0);
	D.Vect3D(c2(0,5), c2(1,5), c2(2,5));
	D.Vect3D(c2(0,6), c2(1,6), c2(2,6));
	D.Vect3D(c2(0,7), c2(1,7), c2(2,7));
	D.Vect3D(c2(0,8), c2(1,8), c2(2,8));
	D.Vect3D(c2(0,9), c2(1,9), c2(2,9));
	D.EndObject();

	//  links von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 1), c2(1, 1), c2(2, 1));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,10), c2(1,10), c2(2,10));
	D.EndObject();

	//  über der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  rechts von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0,12), c2(1,12), c2(2,12));
	D.Vect3D(c2(0, 6), c2(1, 6), c2(2, 6));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  Dach hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(5,0,0);
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.EndObject();

	//  Dach vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(4,0,0);
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.EndObject();

	//  Rasen
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(6,0,0);
	D.Vect3D(c2(0,14), c2(1,14), c2(2,14));
	D.Vect3D(c2(0,15), c2(1,15), c2(2,15));
	D.Vect3D(c2(0,16), c2(1,16), c2(2,16));
	D.Vect3D(c2(0,17), c2(1,17), c2(2,17));
	D.EndObject();

	d=na;																// Einheitsmatrix
	c=ca;																// Hausmatrix

	Translate3D(d,px+350.0, py, pz+400.0);					// Position wählen
	Rotate3D(d, 0.0, M_PI, 0.0);
	c2=d*c;															// eine Sauerei hier zu multiplizieren

	d=na;
	Rotate3D(d, dx, dy, dz);						// Drehen
	c2=d*c2;

	//  vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(1,0,0);
	D.Vect3D(c2(0,0), c2(1,0), c2(2,0));
	D.Vect3D(c2(0,1), c2(1,1), c2(2,1));
	D.Vect3D(c2(0,2), c2(1,2), c2(2,2));
	D.Vect3D(c2(0,3), c2(1,3), c2(2,3));
	D.Vect3D(c2(0,4), c2(1,4), c2(2,4));
	D.EndObject();

	//  hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(3,0,0);
	D.Vect3D(c2(0,5), c2(1,5), c2(2,5));
	D.Vect3D(c2(0,6), c2(1,6), c2(2,6));
	D.Vect3D(c2(0,7), c2(1,7), c2(2,7));
	D.Vect3D(c2(0,8), c2(1,8), c2(2,8));
	D.Vect3D(c2(0,9), c2(1,9), c2(2,9));
	D.EndObject();

	//  links von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 1), c2(1, 1), c2(2, 1));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,10), c2(1,10), c2(2,10));
	D.EndObject();

	//  über der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  rechts von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0,12), c2(1,12), c2(2,12));
	D.Vect3D(c2(0, 6), c2(1, 6), c2(2, 6));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  Dach hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(4,0,0);
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.EndObject();

	//  Dach vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(5,0,0);
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.EndObject();

	//  Rasen
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(6,0,0);
	D.Vect3D(c2(0,14), c2(1,14), c2(2,14));
	D.Vect3D(c2(0,15), c2(1,15), c2(2,15));
	D.Vect3D(c2(0,16), c2(1,16), c2(2,16));
	D.Vect3D(c2(0,17), c2(1,17), c2(2,17));
	D.EndObject();

	d=na;																// Einheitsmatrix
	c=ca;																// Hausmatrix

	Translate3D(d,px+250.0, py, pz);					// Position wählen
	c2=d*c;															// eine Sauerei hier zu multiplizieren

	d=na;
	Rotate3D(d, dx, dy, dz);						// Drehen
	c2=d*c2;

	//  vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(3,0,0);
	D.Vect3D(c2(0,0), c2(1,0), c2(2,0));
	D.Vect3D(c2(0,1), c2(1,1), c2(2,1));
	D.Vect3D(c2(0,2), c2(1,2), c2(2,2));
	D.Vect3D(c2(0,3), c2(1,3), c2(2,3));
	D.Vect3D(c2(0,4), c2(1,4), c2(2,4));
	D.EndObject();

	//  hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(1,0,0);
	D.Vect3D(c2(0,5), c2(1,5), c2(2,5));
	D.Vect3D(c2(0,6), c2(1,6), c2(2,6));
	D.Vect3D(c2(0,7), c2(1,7), c2(2,7));
	D.Vect3D(c2(0,8), c2(1,8), c2(2,8));
	D.Vect3D(c2(0,9), c2(1,9), c2(2,9));
	D.EndObject();

	//  links von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 1), c2(1, 1), c2(2, 1));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,10), c2(1,10), c2(2,10));
	D.EndObject();

	//  über der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0,11), c2(1,11), c2(2,11));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  rechts von der Tür
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(2,0,0);
	D.Vect3D(c2(0,12), c2(1,12), c2(2,12));
	D.Vect3D(c2(0, 6), c2(1, 6), c2(2, 6));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0,13), c2(1,13), c2(2,13));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 0), c2(1, 0), c2(2, 0));
	D.Vect3D(c2(0,18), c2(1,18), c2(2,18));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,21), c2(1,21), c2(2,21));
	D.Vect3D(c2(0, 5), c2(1, 5), c2(2, 5));
	D.EndObject();

	//  hintere Wand
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(7,0,0);
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.Vect3D(c2(0,20), c2(1,20), c2(2,20));
	D.Vect3D(c2(0,19), c2(1,19), c2(2,19));
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.EndObject();

	//  Dach hinten
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(5,0,0);
	D.Vect3D(c2(0, 4), c2(1, 4), c2(2, 4));
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.Vect3D(c2(0, 9), c2(1, 9), c2(2, 9));
	D.EndObject();

	//  Dach vorne
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(4,0,0);
	D.Vect3D(c2(0, 3), c2(1, 3), c2(2, 3));
	D.Vect3D(c2(0, 2), c2(1, 2), c2(2, 2));
	D.Vect3D(c2(0, 7), c2(1, 7), c2(2, 7));
	D.Vect3D(c2(0, 8), c2(1, 8), c2(2, 8));
	D.EndObject();

	//  Rasen
	D.BeginObject(GL_POLYGON);
	D.ObjectColor(6,0,0);
	D.Vect3D(c2(0,14), c2(1,14), c2(2,14));
	D.Vect3D(c2(0,15), c2(1,15), c2(2,15));
	D.Vect3D(c2(0,16), c2(1,16), c2(2,16));
	D.Vect3D(c2(0,17), c2(1,17), c2(2,17));
	D.EndObject();
#endif

}
