/****************************************************************************
 *                                                                          *
 * ViewKlasse mit 3D-Grafikprimitiven                                       *
 * =====================================                                    *
 * (C)opyright 1994,95 by Mark-André Hopf                                   *
 *                                                                          *
 ****************************************************************************/

#include "render.hh"

// #define DEBUG 

// Ich LIEBE C++....
#ifdef DEBUG
class MsgEntryExit
{
	private:
		const char *text;
	public:
		MsgEntryExit(char *t){text=t;fprintf(stderr, "%s : entry\n",text);}
		~MsgEntryExit(){fprintf(stderr,"%s : exit\n",text);}
};
#endif
// ... oder ist SO eine Klasse etwa nicht Klasse?

// deprimierender Ersatz für eine Windowsfunktion...
inline long MulDiv(long a,long b,long c)
{
	#ifdef SECURE
	if (!c)
	{
		MESSAGE("division by zero");
		exit(1);
	}	
	#endif
	return (a*b)/c;
}

/****************************************************************************
 *                                                                          *
 * DisplayWindow                                                            *
 *                                                                          *
 ****************************************************************************/
// Hahaha.... natürlich, eine Übergangslösung.
class StatischesFenster
{
	public:
		Display *display;
		Window	window;
		XID screen;

		StatischesFenster()
		{
			#ifdef DEBUG
			MESSAGE("sf created");
			#endif

			// 1:2.2 Breitwandformat für Kinofreaks
			int x=640;
			int y=290;
			
			display	= XOpenDisplay("");
			if (!display)
			{
				printf("Couldn't find display...\n");
				exit(1); 
			}
			screen		= DefaultScreen(display);
			window=XCreateSimpleWindow(	display,
															DefaultRootWindow(display),
															0,0, x,y,
															0,
															BlackPixel(display,screen), WhitePixel(display,screen) );
			XSizeHints hint;
			hint.x 			= 0;
			hint.y 			= 0;
			hint.width  = x;
			hint.height = y;
			hint.flags = PPosition | PSize;
			XSetStandardProperties(			display,
															window,
															__FILE__,
															__FILE__,
															None,
															NULL,0,
															&hint);
			XSelectInput(display, window, ExposureMask | KeyPressMask);
			XMapRaised(display, window);																														
			// XNextEvent(display, &event);
		}
		~StatischesFenster()
		{
			if (window)  XDestroyWindow(display,window);
			if (display) XCloseDisplay(display);
			#ifdef DEBUG
			MESSAGE("sf destroyed");
			#endif
		}
	
} sf;


DisplayWindow::DisplayWindow(ULONG x, ULONG y)
{
	display		= sf.display;
	screen		= sf.screen;

	colormap	= DefaultColormap(display,screen);
	
	XColor color;
	char *sColor[] = {"Navy", "White", "Bisque", "Linen", "IndianRed", "SaddleBrown", "Green4", 
	                  "Gray90", NULL};
	
	nPenSystem=0;
	while(sColor[nPenSystem]!=NULL)
	{
		if (!XParseColor(display, colormap, sColor[nPenSystem], &color)) goto failed;
		if (!XAllocColor(display, colormap, &color)) goto failed;
		penSystem[nPenSystem++]=color.pixel;
	}
	failed:
	if (nPenSystem<2)
	{
		RTMESSAGE("Couldn't allocate all colors");
		exit(1);
	}
	
	xs=x; ys=y;
	window=sf.window;

	for(int i=0; i<nPenSystem; i++)
	{
		gcPen[i]		= XCreateGC(display,window,0,0);
		XSetBackground(display, gcPen[i], penSystem[i]);
		XSetForeground(display, gcPen[i], penSystem[i]);
	}	
}

DisplayWindow::~DisplayWindow()
{
	for(int i=0; i<nPenSystem; i++)
	{
		XFreeGC(display, gcPen[i]);
		XFreeColors(display, colormap, &penSystem[i], 1 ,0);
	}	
//	XCloseDisplay(display);
}

KeySym DisplayWindow::Getch()
{
	char buffer[10];
	bool done=false;
	KeySym keysym;
	XComposeStatus compose;
	
	while(!done)
	{
		XNextEvent(display, &event);
		switch(event.type)
		{
			case Expose:
				break;
			case KeyPress:
				XLookupString(&event.xkey, buffer, 10, &keysym, &compose); 
				done=true;
				break;				
			case MappingNotify:
				XRefreshKeyboardMapping(&event.xmapping);
				break;
		}
	}
	return keysym;
}

void DisplayWindow::Plot(ULONG x,ULONG y, int pen)
{
	if (pen>=nPenSystem)
	{
	 	pen%=nPenSystem;
	} 
	XDrawPoint(display, window, gcPen[pen], x,y);
}

void DisplayWindow::Line(int x1,int y1, int x2,int y2, BYTE pen)
{
	if (pen>=nPenSystem)
	{
	 	pen%=nPenSystem;
	} 
	XDrawLine(display, window, gcPen[pen], x1,y1, x2,y2);
}

void DisplayWindow::Clear()
{
	const int i=0;
	XFillRectangle(display, window, gcPen[i],
								0,0, xs,ys);
}


/****************************************************************************
 *                                                                          *
 * View                                                                     *
 *                                                                          *
 ****************************************************************************/
View::View(View *parent,LONG x,LONG y):DisplayWindow(x,y)
{
	if (x<=0 || y<=0)
	{
		fprintf(stderr, "display is to small\n");
		exit(0);
	}
	// Größenverhältnisse merken
	xsize=x; ysize=y;
	xmax=x-1; ymax=y-1;
	xmin=0; ymin=0;
	xcenter=x>>1; ycenter=y>>1;
	dist=1.0;
	zoom=250.0;
	// Das Fenster erzeugen
	// Init(parent,"View");
	// Create();
	// Show();
}

/***************************************************************************
 * Clipping von Linien nach Dan Cohen und Ivan Sutherland                  *
 * ======================================================                  *
 * Ein Clipping erfolgt im Bereich 0,0 bis XMAY,YMAX                       *
 ***************************************************************************/
bool View::Clipping(int &x1,int &y1, int &x2, int &y2)
{
	#ifdef DEBUG
	MsgEntryExit dummy("View.Clipping");
	#endif
	char c1,c2;

	c1=c2=0;
/*	
	x1=-174;
	y1=13;
	x2=7;
	y2=-12;
*/

//	printf("clippe (%i,%i)-(%i,%i)\n",x1,y1, x2,y2);

	// (-,+)									  (+,+)    
	//       0401   0400   0420
	//
	//       0001   0000   0020
	//
	//       8001   8000   8020
  // (-,-)                    (+,-)

	if (x1<0)	c1|=1; else if (x1>xmax) c1|=2;
	if (y1<0) c1|=8; else if (y1>ymax) c1|=4;

	if (x2<0)	c2|=1; else if (x2>xmax) c2|=2;
	if (y2<0) c2|=8; else if (y2>ymax) c2|=4;

	if (c1&c2) return false;					// Linie außerhalb des Fensters

	if (c1) 	// Clipping nötig
	{
		if (c1&1)	// links clippen
		{
			#ifdef SECURE
			if(x1==x2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			y1 = y2 - MulDiv(y2-y1, x2, x2-x1);
			x1 = 0;
			c1 = 0;
			if (y1<0) c1|=8; else if (y1>ymax) c1|=4;
			if (c1&c2) return false;					// Linie außerhalb des Fensters
		}
		else
		if (c1&2) // rechts clippen
		{
			#ifdef SECURE
			if(x1==x2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			y1 = y2 + MulDiv(y1-y2, xmax-x2, x1-x2);
			x1 = xmax;
			c1 = 0;
			if (y1<0) c1|=8; else if (y1>ymax) c1|=4;
			if (c1&c2) return false;					// Linie außerhalb des Fensters
		}

		if (c1&8)	// oben clippen
		{
			#ifdef SECURE
			if(y1==y2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			x1 = x2 - MulDiv(x2-x1, y2, y2-y1);
			y1 = 0;
			c1 = 0;
			if (x1<0)	c1|=1; else if (x1>xmax) c1|=2;
		}
		else
		if (c1&4) // unten clippen
		{
			#ifdef SECURE
			if(y1==y2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			x1 = x2 - MulDiv(x2-x1, ymax-y2, y1-y2);
			y1 = ymax;
			c1 = 0;
			if (x1<0)	c1|=1; else if (x1>xmax) c1|=2;
		}
	}

	if (c1&c2) return false;					// Linie außerhalb des Fensters

	if (c2) 	// Clipping nötig
	{
		if (c2&1)	// links clippen
		{
			#ifdef SECURE
			if(x1==x2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			y2 = y1 + MulDiv(y2-y1, x1, x1-x2);
			x2 = 0;
			c2 = 0;
			if (y2<0) c2|=8; else if (y2>ymax) c2|=4;
			if (c1&c2) return false;					// Linie außerhalb des Fensters
		}
		else
		if (c2&2) // rechts clippen (X)
		{
			#ifdef SECURE
			if(x1==x2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			y2 = y1 + MulDiv(y2-y1,xmax-x1, x2-x1 );
			x2 = xmax;
			c2 = 0;
			if (y2<0) c2|=8; else if (y2>ymax) c2|=4;
			if (c1&c2) return false;					// Linie außerhalb des Fensters
		}
		if (c2&8)	// oben clippen
		{
			#ifdef SECURE
			if(y1==y2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			x2 = x1 + MulDiv(x2-x1, y1, y1-y2);
			y2 = 0;
		}
		else
		if (c2&4) // unten clippen
		{
			#ifdef SECURE
			if(y1==y2)
			{
				MESSAGE("division by zero");
				fprintf(stderr,"wanted to clip (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
				fprintf(stderr,"c1=%i, c2=%i\n",(int)c1,(int)c2);
			}
			#endif
			x2 = x1 + MulDiv(x2-x1, ymax-y1, y2-y1);
			y2 = ymax;
		}
	}
	// Rechenungenauigkeiten beheben
	if (x1>xmax) x1=xmax; else if (x1<0) x1=0;
	if (y1>ymax) y1=ymax; else if (y1<0) y1=0;
	if (x2>xmax) x2=xmax; else if (x2<0) x2=0;
	if (y2>ymax) y2=ymax; else if (y2<0) y2=0;

	if (x1<0 || x1>xmax || y1<0 || y1>ymax
		||x2<0 || x2>xmax || y2<0 || y2>ymax )
	{
		fprintf(stderr,"Clipping war ungenau. Geklippt auf (%i,%i),(%i,%i)\n",x1,y1, x2,y2);
		return false;
	}
	return true;
}

/***************************************************************************
 * Linien zeichnen mit dem Bresenham Algorithmus                           *
 ***************************************************************************/
void View::Bresenham(int x1,int y1, int x2, int y2,BYTE f)
{
	#ifdef DEBUG
	MsgEntryExit dummy("View.Bresenham");
	#endif
	int dx,dy, xstep,ystep, e;

	dx=x2-x1; dy=y2-y1;
	if(dx>0)
		xstep=1;
	else
	{
		xstep=-1;
		dx=-dx;
	}
	if(dy>0)
		ystep=1;
	else
	{
		ystep=-1;
		dy=-dy;
	}

	if (dy>dx)
	{
		e=-dy;
		dx<<=1;
		dy<<=1;
		while(y1!=y2)
		{
			Plot2D(x1,y1,f);
			e+=dx;
			y1+=ystep;
			if (e>0)
			{
				x1+=xstep;
				e-=dy;
			}
		}
	}
	else
	{
		e=-dx;
		dy<<=1;
		dx<<=1;
		while(x1!=x2)
		{
			Plot2D(x1,y1,f);
			x1+=xstep;
			e+=dy;
			if (e>0)
			{
				y1+=ystep;
				e-=dx;
			}
		}
	}
	Plot2D(x1,y1,f);
}

/****************************************************************************
 *                                                                          *
 * Outline zeichnet nur die Umgebungslinien                                 *
 *                                                                          *
 ****************************************************************************/
OutlineView::OutlineView(View* parent,LONG x,LONG y):View(parent,x,y)
{
	Clear();
};

OutlineView::~OutlineView()
{
}

void OutlineView::Paint()
{
}

void OutlineView::Clear()
{
	View::Clear();
	if (pPicture)
	{
		register BYTE* ptr=(BYTE*)pPicture;
		register ULONG l=xsize*ysize;
		while(l--)
			*(ptr++)=0;

		for (int i=0; i<100; i++)
		{
			Line2D(0,    i<<1, 320,    i<<1, 255-(BYTE)i);
			Line2D(0,(i<<1)+1, 320,(i<<1)+1, 255-(BYTE)i);
		}
	}
}

/***************************************************************************
 * Punkt setzen bei x,y in der Farbe c                                     *
 ***************************************************************************/
void OutlineView::Plot2D(int x,int y,BYTE c)
{
	if ((UINT)x<=(UINT)xmax || (UINT)y<=(UINT)ymax)
		Plot(x,y,c);
//		*((BYTE*)pPicture+((ULONG)ymax-y)*(ULONG)xsize+(ULONG)x)=c;
}

/***************************************************************************
 * Zeichnen einer Linie von x1,y1 nach x2,y2 in der Farbe c                *
 ***************************************************************************/
void OutlineView::Line2D(int x1,int y1, int x2,int y2, BYTE c)
{
	#ifdef DEBUG
	MsgEntryExit dummy("OutlineView.Line2D");
	#endif
	if ( Clipping(x1,y1,x2,y2) )
			//	Bresenham(x1,y1,x2,y2,c);
			Line(x1,y1,x2,y2,c);
}

/****************************************************************************
 *                                                                          *
 * ZBuffer                                                                  *
 *                                                                          *
 ****************************************************************************/
ZBufferView::ZBufferView(View* parent,LONG x,LONG y):OutlineView(parent,x,y)
{
	memZBuffer.Init((ULONG)x*(ULONG)y);
	memZBuffer.Clear(-1.0);
};

ZBufferView::~ZBufferView()
{
}

/***************************************************************************
 * Punkt setzen bei x,y,z in der Farbe c                                   *
 ***************************************************************************/
void ZBufferView::Plot3D(int x,int y,float z,BYTE pen)
{
	#ifdef SECURE
	if (pPicture==NULL)
	{
	//	RTMESSAGE("pPicture==NULL (no bitmap)");
	//	exit(1);
	}
	#endif
	if (x<0 || x>xmax || y<0 || y>ymax) return;

	register float *ptr = memZBuffer;
	ptr+=((ULONG)y*(ULONG)xsize+(ULONG)x);

	if (*ptr>z || *ptr==-1.0)
	{
		*ptr=z;
		Plot(x,y,pen);
		// direkt in die Bitmap zeichnen
		// *((BYTE*)pPicture+((ULONG)ymax-y)*(ULONG)xsize+(ULONG)x)=pen;
	}
}

void ZBufferView::Clear()
{
	memZBuffer.Clear(-1.0);
	OutlineView::Clear();
}


/////////////////////////////////////////////////////////////////////
/****************************************************************************
 *                                                                          *
 * Polygonclipping nach Sutherland-Hodgan (noch nicht optimiert)            *
 * Der zurückgelierte Zeiger muß mit delete gelöscht werden                 *
 *                                                                          *
 ****************************************************************************/
Point2D* View::ClipPoly(Point2D s[],int *n)
{
	#ifdef DEBUG
	MsgEntryExit dummy("View::ClipPoly");
	#endif

	Point2D *p, *q, *r;
	int i,j, Px,Py, Sx,Sy, Fx,Fy, Ix,Iy;

	q=new Point2D[((*n)<<1)+4];		// jede Ecke kann beim Abschneiden zu _2_ Ecken
	r=new Point2D[((*n)<<1)+4];   // werden und in den _4_ Bildschirmecken zu 3
	p=s;

	Sx=Sy=0;
	// oben kappen
	j=0;
	for(i=0; i<*n; i++)
	{
		Px=p[i].x; Py=p[i].y;
		if(i==0)								// P erster Punkt ?
		{
			Fx=Px; Fy=Py;					// ja: F <- P
		}
		else
		{
														// nein: Wird e von SP geschnitten ?
			if ((Sy<ymin && Py>=ymin) || (Sy>ymin && Py<=ymin))
			{
														// ja: berechne Schnittpunkt I von SP und e
				Ix=MulDiv(Px-Sx,ymin-Sy,Py-Sy)+Sx;
				Iy=ymin;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
			}
		}
		Sx=Px; Sy=Py;						// S <- P
		if (Sy>=ymin)					// befindet sich S auf der sichtbaren Seite von e?
		{
			q[j].x=Sx; q[j].y=Sy; // ja: Ausgabe von Eckpunkt s
			j++;
		}
	}
														// wird e von SF überlaufen ?
	if ((Sy<ymin && Fy>=ymin) || (Sy>ymin && Fy<=ymin))
	{
														// ja: berechne Schnittpunkt I von SF und e
				Ix=MulDiv(Fx-Sx,ymin-Sy,Fy-Sy)+Sx;
				Iy=ymin;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
	}
	*n=j;

	p=q;
	q=r;

	// unten kappen
	j=0;
	for(i=0; i<*n; i++)
	{
		Px=p[i].x; Py=p[i].y;
		if(i==0)								// P erster Punkt ?
		{
			Fx=Px; Fy=Py;					// ja: F <- P
		}
		else
		{
														// nein: Wird e von SP geschnitten ?
			if ((Sy<ymax && Py>=ymax) || (Sy>ymax && Py<=ymax))
			{
														// ja: berechne Schnittpunkt I von SP und e
				Ix=MulDiv(Px-Sx,ymax-Sy,Py-Sy)+Sx;
				Iy=ymax;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
			}
		}
		Sx=Px; Sy=Py;						// S <- P
		if (Sy<=ymax)						// befindet sich S auf der sichtbaren Seite von e?
		{
			q[j].x=Sx; q[j].y=Sy; // ja: Ausgabe von Eckpunkt s
			j++;
		}
	}
														// wird e von SF überlaufen ?
	if ((Sy<ymax && Fy>=ymax) || (Sy>ymax && Fy<=ymax))
	{
														// ja: berechne Schnittpunkt I von SF und e
				Ix=MulDiv(Fx-Sx,ymax-Sy,Fy-Sy)+Sx;
				Iy=ymax;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
	}
	*n=j;
	r=p;
	p=q;
	q=r;

	// rechts kappen
	j=0;
	for(i=0; i<*n; i++)
	{
		Px=p[i].x; Py=p[i].y;
		if(i==0)								// P erster Punkt ?
		{
			Fx=Px; Fy=Py;					// ja: F <- P
		}
		else
		{
														// nein: Wird e von SP geschnitten ?
			if ((Sx<xmax && Px>xmax) || (Sx>xmax && Px<xmax))
			{
														// ja: berechne Schnittpunkt I von SP und e
				Iy=MulDiv(Py-Sy,xmax-Sx,Px-Sx)+Sy;
				Ix=xmax;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
			}
		}
		Sx=Px; Sy=Py;						// S <- P
		if (Sx<=xmax)					// befindet sich S auf der sichtbaren Seite von e?
		{
			q[j].x=Sx; q[j].y=Sy; // ja: Ausgabe von Eckpunkt s
			j++;
		}
	}
														// wird e von SF überlaufen ?
	if ((Sx<xmax && Fx>xmax) || (Sx>xmax && Fx<xmax))
	{
														// ja: berechne Schnittpunkt I von SF und e
				Iy=MulDiv(Fy-Sy,xmax-Sx,Fx-Sx)+Sy;
				Ix=xmax;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
	}
	*n=j;
	r=p;
	p=q;
	q=r;

	// links kappen
	j=0;
	for(i=0; i<*n; i++)
	{
		Px=p[i].x; Py=p[i].y;
		if(i==0)								// P erster Punkt ?
		{
			Fx=Px; Fy=Py;					// ja: F <- P
		}
		else
		{
														// nein: Wird e von SP geschnitten ?
			if ((Sx<xmin && Px>xmin) || (Sx>xmin && Px<xmin))
			{
														// ja: berechne Schnittpunkt I von SP und e
				Iy=MulDiv(Py-Sy,xmin-Sx,Px-Sx)+Sy;
				Ix=xmin;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
			}
		}
		Sx=Px; Sy=Py;						// S <- P
		if (Sx>=xmin)					// befindet sich S auf der sichtbaren Seite von e?
		{
			q[j].x=Sx; q[j].y=Sy; // ja: Ausgabe von Eckpunkt s
			j++;
		}
	}
														// wird e von SF überlaufen ?
	if ((Sx<xmin && Fx>xmin) || (Sx>xmin && Fx<xmin))
	{
														// ja: berechne Schnittpunkt I von SF und e
				Iy=MulDiv(Fy-Sy,xmin-Sx,Fx-Sx)+Sy;
				Ix=xmin;
														// Ausgabe von Eckpunkt I
				q[j].x=Ix; q[j].y=Iy;
				j++;
	}
	*n=j;

	delete p;
	return q;
}

/****************************************************************************
 *                                                                          *
 * Seitenränder eines 2dimensionalen, konvexen Polygons bestimmen           *
 *																																					*
 ****************************************************************************/
void View::FillPolyBuffer(Point2D op[],int n,int *top,int *bottom)
{
	#ifdef DEBUG
	MsgEntryExit dummy("View.FillPolyBuffer");
	#endif
	Point2D *p;
	int i, tp, bo, punkt;
	int dx,dy, xstep, e, x1,y1, x2,y2;

	// Polygon kappen, damit es in den Bildschirm und somit auch in den Buffer passt
	p=ClipPoly(op,&n);
	if(n<3)							// Polygon liegt außerhalb des Sichtbereichs
	{
		*top=1;
		*bottom=0;
		delete[] p;
		return;
	}

	// obersten & untersten Polygonpunkt suchen
	*top=*bottom=p[0].y;
	tp=0;	// top point, ein oberster Punkt des Polygons
	// obersten & untersten Punkt des Polygons bestimmen
	for(i=1; i<n; i++)
	{
		if (*top>p[i].y) {*top=p[i].y; tp=i;}
		if (*bottom<p[i].y) *bottom=p[i].y;
	}

	// Das hier ist ein Spezialfall, der gesondert behandelt werden muß
	if (*top==*bottom)
	{
		register int xmin, xmax;
		// Suche die linke & rechte Grenze
		xmin=xmax=p[0].x;
		for(i=1; i<n; i++)
		{
			if (xmin>p[i].x) xmin=p[i].x;
			if (xmax<p[i].x) xmax=p[i].x;
		}
		polybuffer[*top][0]=xmin;
		polybuffer[*top][1]=xmax;
		delete[] p;
		return;
	}

	// beide Seiten zeichnen
	for(bo=0; bo<=1; bo++)
	{
		punkt=tp;
		x2=p[punkt].x; y2=p[punkt].y;
		while(-1)
		{
			if (bo)							// zum nächsten Punkt
			{
				punkt++;
				if (punkt>=n)
					punkt=0;
			}
			else
			{
				punkt--;
				if (punkt<0)
					punkt=n-1;
			}

			x1=x2; y1=y2;
			x2=p[punkt].x; y2=p[punkt].y;

			if (y2<y1)					// untersten Polygonpunkt erreicht
				break;
			// x2,y2 dürfen jetzt nicht zerstört werden
			dy=y2-y1;
			if (dy==0)          // horizontale Line, interessiert nicht
				continue;
			// Bresenham
			dx=x2-x1;
			if(dx>0)
				xstep=1;
			else
			{
				xstep=-1;
				dx=-dx;
			}
			if (dx<0) dx=-dx;

			if (dy>dx)
			{
				e=-dy;
				dx<<=1;
				dy<<=1;
				while(y1!=y2)
				{
				polybuffer[y1][bo]=x1;
					e+=dx;
					y1++;
					if (e>0)
					{
						x1+=xstep;
						e-=dy;
			}	} }
			else
			{
				e=-dy;
				dx<<=1;
				dy<<=1;
				while(y1!=y2)
				{
					polybuffer[y1][bo]=x1;
					y1++;
					e+=dx;
					while (e>0)
					{
						x1+=xstep;
						e-=dy;
			}	}	}
		polybuffer[y1][bo]=x1;
		}
	}
	delete[] p;		// Polygonränder sind bestimmt, das gekappte Polygon wegschmeißen
}

/****************************************************************************
 *                                                                          *
 * 2dimensionale, konvexe Polygone zeichnen                                 *
 *																																					*
 ****************************************************************************/
void OutlineView::Poly2D(Point2D op[],int n,BYTE f)
{
	int top, buttom, i,j, x1,x2;

	FillPolyBuffer(op,n,&top,&buttom);
	for(i=top; i<=buttom; i++)
	{
		x1=polybuffer[i][0];
		x2=polybuffer[i][1];
		if (x1>x2) {register j; j=x1; x1=x2; x2=j;}
		for(j=x1; j<=x2; j++)
			Plot2D(j,i,f);
	}
}

/****************************************************************************
 *                                                                          *
 *  eine 3dimensionale Linie zeichnen                                       *
 *                                                                          *
 ****************************************************************************/
void OutlineView::Line3D(double x1,double y1,double z1, double x2,double y2,double z2, BYTE f)
{
	if (z1<dist && z2<dist)
		return;

	if (z1<dist)
	{
		x1=(x1-x2)/(z1-z2)*(dist-z2)+x2;
		y1=(y1-y2)/(z1-z2)*(dist-z2)+y2;
		z1=dist;
	}

	if (z2<dist)
	{
		x2=(x2-x1)/(z2-z1)*(dist-z1)+x1;
		y2=(y2-y1)/(z2-z1)*(dist-z1)+y1;
		z2=dist;
	}
	x1=x1/z1*dist*zoom;
	y1=y1/z1*dist*zoom;
	x2=x2/z2*dist*zoom;
	y2=y2/z2*dist*zoom;
	Line2D((int)x1+xcenter,ycenter-(int)y1, (int)x2+xcenter,ycenter-(int)y2, f);
}

void OutlineView::Poly3D(Point3D p[], int n, BYTE f)
{
	#ifdef DEBUG
	MsgEntryExit dummy("OutlineView.Poly3D");
	#endif

	#ifdef VERSION2
	MESSAGE("Line3D macht gelegentlich Fehler...");
	for(int i=1; i<n; i++)
	{
		Line3D(p[i-1].x, p[i-1].y, p[i-1].z,
					 p[i].x	 , p[i].y	 , p[i].z  , f);
	}
	i--;
		Line3D(p[0].x, p[0].y, p[0].z,
					 p[i].x, p[i].y, p[i].z  , f);
	#else
	Point2D *q=Projection(p,&n);
	int i;
	for(i=1; i<n; i++)
	{
		Line2D(q[i-1].x, q[i-1].y,
					 q[i].x	 , q[i].y	 , f);
	}
	i--;
		Line2D(q[0].x, q[0].y,
					 q[i].x, q[i].y, f);
					 
	delete[] q;
	#endif
}

// p an der Sichtebene kappen und als 2dimensionale Koordinaten zurückliefern
Point2D* View::Projection(Point3D p[], int *n)
{
	#ifdef DEBUG
	MsgEntryExit dummy("View.Projection");
	#endif
	double Px,Py,Pz, Sx,Sy,Sz, Ix,Iy,Iz, Fx,Fy,Fz;
	int i,j;
	Point2D *q;

	q=new Point2D[*n+1];												// Speicher für q reservieren
	Sx=Sy=Sz=0.0;
	j=0;
	for(i=0; i<*n; i++)
	{
		Px=(double)p[i].x; Py=(double)p[i].y; Pz=(double)p[i].z;
		if (i==0)								// erster Punkt ?
		{	Fx=Px; Fy=Py;	Fz=Pz;	// ja: F <- P
		}
		else
		{												// nein: Wird e von SP geschnitten ?
			if (((Sz<dist) && (Pz>=dist)) || ((Sz>=dist) && (Pz<dist)))
			{
				Ix=(Px-Sx)*(dist-Sz)/(Pz-Sz)+Sx;							// ja: berechne Schnittpunkt I von SP und e
				Iy=(Py-Sy)*(dist-Sz)/(Pz-Sz)+Sy;
				Iz=dist;
				q[j].x=((int)(Ix/Iz*dist*zoom))+xcenter;			// Ausgabe von Eckpunkt I
				q[j].y=ycenter-((int)(Iy/Iz*dist*zoom));
				j++;
			}
		}
		Sx=Px; Sy=Py;	Sz=Pz;		// S <- P
		if (Sz>=dist) 					// befindet sich S auf der sichtbaren Seite von e?
		{
			q[j].x=((int)(Sx/Sz*dist*zoom))+xcenter;				// ja: Ausgabe von Eckpunkt S
			q[j].y=ycenter-((int)(Sy/Sz*dist*zoom));
			j++;
		}
	}
	if ((Sz<dist && Fz>=dist) || (Sz>=dist && Fz<dist))	// wird e von SF überlaufen ?
	{
				Ix=(Fx-Sx)*(dist-Sz)/(Fz-Sz)+Sx;
				Iy=(Fy-Sy)*(dist-Sz)/(Fz-Sz)+Sy;
				Iz=dist;
				q[j].x=((int)(Ix/Iz*dist*zoom))+xcenter;			// Ausgabe von Eckpunkt I
				q[j].y=ycenter-((int)(Iy/Iz*dist*zoom));
				j++;
	}
	*n=j;
	return q;
}

/****************************************************************************
 *                                                                          *
 *  3dimensionale, konvexe Polygone zeichnen                                *
 *                                                                          *
 ****************************************************************************/
void ZBufferView::Poly3D(Point3D p[], int n, BYTE f)
{
	#ifdef DEBUG
	MsgEntryExit dummy("ZBufferView.Poly3D");
	#endif
	int i,j, x1,x2;
	// double Px,Py,Pz, Sx,Sy,Sz, Ix,Iy,Iz, Fx,Fy,Fz;

	// p an der Sichtebene kappen und als 2dimensionale Koordinaten nach q schreiben
	// =============================================================================
	j=n;
	Point2D *q=Projection(p,&j);

	// in den Polygonbuffer den linken & rechten Rand eintragen
	// ========================================================
	int top,bottom;
	FillPolyBuffer(q,j,&top,&bottom);
	delete[] q;																					// q wird nicht mehr benötigt
	if (top>bottom) return;															// das Polygon ist nicht sichtbar

	// Koordinatendarstellung der Ebene für Tiefenalgorithmus bestimmen
	// ================================================================
	double ax,ay,az, bx,by,bz, nx,ny,nz, sx,sy,sz, d, dr;

	ax=(double)p[1].x-(double)p[0].x;										// Parameterdarstellung der Ebene ermitteln
	ay=(double)p[1].y-(double)p[0].y;
	az=(double)p[1].z-(double)p[0].z;

	bx=(double)p[2].x-(double)p[0].x;
	by=(double)p[2].y-(double)p[0].y;
	bz=(double)p[2].z-(double)p[0].z;

	nx=ay*bz-az*by;																			// Normalenvektor der Ebene bestimmen
	ny=az*bx-ax*bz;
	nz=ax*by-ay*bx;

	d=nx*(double)p[0].x +																// Koordinatendarstellung der Ebene bestimmen
		ny*(double)p[0].y +
		nz*(double)p[0].z;

	// Zu jedem Punkt die Tiefe bestimmen und zeichnen
	// ===============================================
	for(i=top; i<=bottom; i++)													// von oben nach unten und...
	{
		x1=polybuffer[i][0];x2=polybuffer[i][1];if (x1>x2) {register j; j=x1; x1=x2; x2=j;}
		for(j=x1; j<=x2; j++)															// ...von links nach rechts alle Punkte bearbeiten.
		{
			sx=(double)(j-xcenter)/zoom; 										// s ist Sichtvektor vom Nullpunkt aus
			sy=(double)(-(i-ycenter))/zoom;
			sz=dist;
			dr=nx*sx+ny*sy+nz*sz;
			#ifdef SECURE
			if (dr==0)
				MESSAGE("dr==0");
			else	
			#endif
			Plot3D(j,i,d/dr,f);
		}
	}
}

/****************************************************************************
 *                                                                          *
 *  View                                                                    *
 *                                                                          *
 ****************************************************************************/
// an OpenGL angelehnte Grafikprimitive
bool View::BeginObject(WORD type)
{
	if (actualtype==0)					// es wird zur Zeit nichts gezeichnet
	{
		actualtype=type;
		List3D=NULL;
		return true;
	}
	else
	{
		return false;
	}
}

bool View::Vect3D(double &x,double &y,double &z)
{
	switch(actualtype)
	{
		case GL_POLYGON:
			AppendList3D(new Node3D(x,y,z));
			break;
		default:
			return false;
	}
	return true;
}

bool View::AppendList3D(Node3D *newnode)
{
	if (List3D)
	{
		register Node3D *ptr=List3D;
		while(ptr->next) ptr=ptr->next;
		ptr->next=newnode;
	}
	else
	{
		List3D=newnode;
	}
	return true;
}

bool View::ClearList3D()
{
	register Node3D *ptr2, *ptr=List3D;
	List3D=NULL;
	while(ptr)
	{
		ptr2=ptr->next;
		delete ptr;
		ptr=ptr2;
	}
	return true;
}

WORD View::LengthList3D()
{
	register WORD i=0;
	register Node3D *ptr=List3D;
	while(ptr)
	{
		i++;
		ptr=ptr->next;
	}
	return i;
}

bool View::EndObject()
{
	switch(actualtype)
	{
		case GL_POLYGON:
			{
				WORD n=LengthList3D();
				if (n<=2) return false;
				// Daten für die alten Routinen konvertieren (vorläufig so gelöst)
				Point3D *P=new Point3D[n];
				register Node3D *ptr=List3D;
				WORD i=0;
				while(ptr)
				{
					P[i].x = ptr->x;
					P[i].y = ptr->y;
					P[i++].z = ptr->z;
					ptr=ptr->next;
				}
				Poly3D(P,n,rgbR);
				delete[] P;
				//
				ClearList3D();
			}
			break;
		default:
			return false;
	}
	actualtype=0;
	return true;
}
