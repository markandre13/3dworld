// this file      : render.hh
// responsible for: header for render.cc

// ACHTUNG: Die Größe von Polybuffer ist im Moment noch hardcodiert auf 480!!!

#ifndef _RENDER_HH
#define _RENDER_HH

#ifndef _OWNDEF_HH
#include "owndef.hh"
#include "memory.hh"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#define GL_POLYGON	0x0001

struct Point2D {int x,y;};
struct Point3D {double x,y,z;};
struct StructRGB {BYTE r,g,b;};

class Node3D
{
	public:
		Node3D(double &ix, double &iy, double &iz){x=ix; y=iy; z=iz; next=NULL;}
		Node3D *next;
		double x,y,z;
};

class DisplayWindow
{
	public:
		DisplayWindow(ULONG x, ULONG y);
		virtual ~DisplayWindow();
		void Plot(ULONG x,ULONG y,int pen);
		void Line(int,int,int,int,BYTE);
		virtual void Clear();
		ULONG penSystem[16];
		int nPenSystem;
		Display *display;
		KeySym Getch();
	private:
		int			screen;
		Colormap colormap;
		Window 	window;
		GC			gcPen[16];
		ULONG		xs,ys;
		XEvent	event;
};

class View: public DisplayWindow
{
	protected:
		LONG xmax, ymax, xmin, ymin, xsize, ysize, xcenter,ycenter;
		double dist, zoom;

		// 2D Grafikprimitive
		bool Clipping(int &x1,int &y1, int &x2, int &y2);
		void Bresenham(int x1,int y1, int x2, int y2,BYTE f);

		int polybuffer[480][2];
		void FillPolyBuffer(Point2D op[],int n,int *top,int *buttom);

		Point2D* Projection(Point3D p[], int *n);
		Point2D* ClipPoly(Point2D s[],int *n);

		// für an OpenGL angelehnte Grafikprimitive
		BYTE rgbR,rgbG,rgbB;
		Node3D *List3D;
		bool AppendList3D(Node3D *newelem);
		bool ClearList3D();
		WORD LengthList3D();
	public:
		View(View *parent,LONG x,LONG y);
		virtual void Paint()=0;
		virtual void Plot2D(int x,int y, BYTE c)=0;
		virtual void Line2D(int x1,int y1, int x2, int y2,BYTE c)=0;
		virtual void Poly2D(Point2D op[],int n,BYTE f)=0;
		virtual void Line3D(double x1,double y1,double z1, double x2,double y2,double z2, BYTE f)=0;
		virtual void Poly3D(Point3D p[], int n, BYTE f)=0;

		// an OpenGL angelehnte Grafikprimitive
		WORD actualtype;
		bool BeginObject(WORD type);
		bool EndObject();
		bool Vect3D(double &x,double &y,double &z);
		bool ObjectColor(BYTE r,BYTE g,BYTE b){rgbR=r;rgbG=g;rgbB=b;return true;};
};

class OutlineView : public View
{
	protected:

		// Verwaltung der Bitmap für das Bild (Unter X-Windows noch nicht genutzt)
		void *pPicture;
		Memory<BYTE> memPicture;

		void Plot2D(int x,int y, BYTE c);
		void Line2D(int x1,int y1, int x2, int y2,BYTE c);
		void Poly2D(Point2D op[],int n,BYTE f);

		void Line3D(double x1,double y1,double z1, double x2,double y2,double z2, BYTE f);
		void Poly3D(Point3D p[], int n, BYTE f);
		
	public:
		OutlineView(View* parent,LONG x,LONG y);
		virtual ~OutlineView();
		void Paint();
		void Clear();
};

class ZBufferView : public OutlineView
{
	protected:
		// Verwaltung des z-Buffers
		Memory<float> memZBuffer;

		void Plot3D(int x,int y, float z,BYTE pen);

	public:
		ZBufferView(View* parent,LONG x,LONG y);
		virtual ~ZBufferView();
		void Clear();
		void Poly3D(Point3D p[], int n, BYTE f);
};
#endif
