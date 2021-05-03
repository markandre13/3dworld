/****************************************************************************
 *                                                                          *
 * CPP MATRIX CLASS v1.01                                                   *
 * (C)opyright 1994 by Mark-Andr� Hopf / Upundalsprung 44 / D-18069 Rostock *
 * (Jetzt einen Vektorrechner...)                                           *
 *                                                                          *
 * 10.05.94 : begonnen mit v1.00                                            *
 * 13.06.94 : geringf�gig �berarbeitet                                      *
 * 14.06.94 : - Beibehalten des mit new erzeugten Speicherbereichs bei      *
 *              doppeltem Aufruf von Destruktoren durch die Operatoren      *
 *            - Rotate3D und Translate3D zum Erzeugen einer Transformations-*
 *							matrix                                                      *
 * 16.06.94 : Schlau geworden und den ganzen SCHWACHSINN von doppelt aufge- *
 *            rufenen Destruktoren einfach mit & Referenzen umgangen!!!     *
 * 25.06.94 : Noch ein paar �berfl�ssige Dinge entfernt                     *
 * 26.06.94 : Verwendung von Referenzen konsequent auf alle Teile der Klasse*
 *            angewandt und bemerkt, da� das Transponieren von einreihigen  *
 *            Matrizen einfach durch Austauschen der x- & y-Gr��e machbar   *
 *            ist.                                                          *
 * 27.06.94 : Rotate3D ausprobiert und Fehler beseitigt, festgestellt, da�  *
 *            Routinen, die Referenzen auf Matrizen entgegennehmen, diese   *
 *            l�schen m�ssen, wenn matrix.Temp gesetzt ist. Das ist         *
 *            ungeschickt.                                                  *
 * 15.08.94 : Entdeckt, da� es Kopierkonstruktoren gibt und auf die         *
 *            Referenzen wieder verzichtet, da dies zu fehlertr�chtig ist,  *
 *            Kopie der alten Variante in MATHE2.CPP gespeichert, Versions- *
 *						nummer auf v1.01 erh�ht.                                      *
 * 24.05.95 : Unter Linux compiliert                                        *
 *                                                                          *
 ****************************************************************************/

/*  Hier ein paar Beispiel, wie mensch das ganze nutzen kann:

		matrix a;											: undefinierte Matrix

		matrix a(2,3);    						: (2,3)-Matrix erzeugen

		int p[]={ 0.0, 0.0, 0.0,      : gesamte Matrix setzen
							0.0, 0.0, 0.0 };
		a=p;

		a(0,0)=1.0;										: einzelne Komponenten setzen
		f=a(0,0);											: einzelne Komponenten holen

		a=b;													: Matrix gleich andere Matrix

		a*b;													: Matrizenmultiplikation

		float*matrix									: Multiplikation mit Skalar

		t(a);													: a transponiert


		geplant:
		a+b;													: Matrizenaddition
		a.x();												: x Gr��e (inline)
		a.y();												: y Gr��e (inline)
		a==b													: Vergleich von Matrizen
		double spur(matrix &a);				: Spur von a
		double det(matrix &a);				: Determinante von a
		matrix& inv(matrix &a);				: inverse Matrix zu a
		int rg(matrix &a);						: rg von a

		eine Klasse vector, die evlt. von matrix abgeleitet ist
		(he, bei (1,n) oder (n,1) Matrizen brauche ich beim Transponieren ja
		nur die x und y Gr��e miteinander zu vertauschen um zu Transponieren!!!)

		ACHTUNG !!!
		Wenn a*b, inv(a), etc. zu keinen Ergebnissen f�hren, so liefern sie eine
		(0,0)-Matrix zur�ck.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SECURITY

#include "matrix.hh"

/***************************************************************************
 *                                                                         *
 * class matrix                                                            *
 * ============                                                            *
 * Die Klasse enth�lt als einzige Variable lediglich einen Zeiger auf      *
 * alle ben�tigten Daten der Klasse, damit die Performance m�glichst gro�  *
 * ist, wenn Matrizen als Parameter �bergeben werden und beim Rechnen mit  *
 * Matrizen nicht riesige tempor�re Matrizen im Speicher hin- und herge-   *
 * schoben werden.                                                         *
 *                                                                         *
 * pMatrix[OWNERS].n    : Anzahl der Eigent�mer einer Matrix               *
 * pMatrix[TEMPFLAG].n  : Matrix ist tempor�r und hat demn�chst keinen     *
 *                        Eigent�mer mehr. Z.B. entsteht bei 7*(B*C) eine  *
 *                        tempor�re Matrize aus B*C, welche dann mit 7     *
 *                        multipliziert werden kann, ohne da� daf�r eine   *
 *                        neue Matrize erzeugt werden mu�.                 *
 * pMatrix[X_SIZE].n		: x-Gr��e der Matrize                              *
 * pMatrix[Y_SIZE].n    : y-Gr��e der Matrize                              *
 * pMatrix[FIRST_ELEM].k: Ab hier stehen alle Elemente der Matrize         *
 *                                                                         *
 ***************************************************************************/
enum {OWNERS, TEMPFLAG, X_SIZE, Y_SIZE, FIRST_ELEM};

#define HAS_BOOLEAN

#ifndef HAS_BOOLEAN
typedef int bool;
const bool true=1;
const bool false=0;
#endif

/***************************************************************************
 * Konstruktoren                                                           *
 ***************************************************************************/
matrix::matrix()
{
	init(0,0);
}

matrix::matrix(unsigned y,unsigned x)
{
	init(y,x);
}

void matrix::init(unsigned y,unsigned x)
{
	if (x!=0 && y!=0)
	{
		pMatrix=new Matrix[FIRST_ELEM+x*y];   	// Speicherplatz reservieren
		pMatrix[OWNERS].n=1;                 		// Anzahl Eigent�mer dieser Matrix
		pMatrix[TEMPFLAG].n=false;
		pMatrix[X_SIZE].n=x;                 		// Gr��e eintragen
		pMatrix[Y_SIZE].n=y;
	}
	else
		pMatrix=NULL;
}

inline matrix::matrix(const matrix &m)
{
	pMatrix=m.pMatrix;
	if (pMatrix) pMatrix[OWNERS].n++;
}

/***************************************************************************
 *                                                                         *
 * Destruktor                                                              *
 *                                                                         *
 ***************************************************************************/
matrix::~matrix()
{
	if (pMatrix && (--pMatrix[OWNERS].n == 0))				// ein Eigent�mer weniger
		delete[] pMatrix;                               // l�schen
}

/***************************************************************************
 *                                                                         *
 * matrix=double*                                                          *
 * ==============                                                          *
 * Eine Matrix mit den Elementen eines Arrays f�llen.                      *
 *                                                                         *
 ***************************************************************************/
void matrix::operator =(double* src)
{
	if (pMatrix)
		memcpy(pMatrix+FIRST_ELEM,
					 src,
					 pMatrix[X_SIZE].n*pMatrix[Y_SIZE].n*sizeof(Matrix));
}

/***************************************************************************
 *                                                                         *
 * matrix=matrix&                                                          *
 * ==============                                                          *
 * Eine Matrix mit dem Inhalt einer anderen Matrix f�llen.                 *
 *                                                                         *
 ***************************************************************************/
void matrix::operator =(matrix a)
{
	size_t srclen,dstlen;

	if (a.pMatrix)
	{ // begin of a.pMatrix!=NULL
		if (a.pMatrix[TEMPFLAG].n)
		{
			if (pMatrix)
			{
				if (--pMatrix[OWNERS].n==0)
				{
					delete pMatrix;
				}
			}
			pMatrix=a.pMatrix;
			pMatrix[OWNERS].n++;
			pMatrix[TEMPFLAG].n=false;
		}
		else
		{ // begin of a.pMatrix && a.pMatrix[TEMPFLAG].n==FALSE
			if (pMatrix)
			{
				srclen=a.pMatrix[X_SIZE].n*a.pMatrix[Y_SIZE].n+FIRST_ELEM;
				dstlen=  pMatrix[X_SIZE].n*  pMatrix[Y_SIZE].n+FIRST_ELEM;

				if (pMatrix[OWNERS].n==1)
				{
					if (dstlen>=srclen)
					{
						memcpy(pMatrix,
									 a.pMatrix,
									 srclen*sizeof(Matrix));
						pMatrix[OWNERS].n=1;
					}
					else
					{
						delete pMatrix;
						pMatrix=a.pMatrix;
						pMatrix[OWNERS].n++;
					}
				}
				else
				{
					pMatrix[OWNERS].n--;
					pMatrix=a.pMatrix;
					pMatrix[OWNERS].n++;
				}
			}
			else
			{
				pMatrix=a.pMatrix;
				pMatrix[OWNERS].n++;
			}
		} // end of a.pMatrix && a.pMatrix[TEMPFLAG].n==FALSE
	} // end of a.pMatrix!=NULL
	else
	{	// begin of a.pMatrix==NULL
		if (pMatrix)
		{
			if (--pMatrix[OWNERS].n==0)
				delete pMatrix;
		}
		pMatrix=NULL;
	} // end of a.pMatrix==NULL
}

/***************************************************************************
 *                                                                         *
 * double skalar * matrix                                                  *
 * ======================                                                  *
 * Eine Matrix mit einem Skalar multiplizieren                             *
 *                                                                         *
 ***************************************************************************/
matrix operator *(double k,matrix &m)
{
	unsigned i, len;

	if (m.pMatrix)
	{
		len=m.pMatrix[X_SIZE].n*m.pMatrix[Y_SIZE].n+FIRST_ELEM;
		if (m.pMatrix[TEMPFLAG].n)
		{
			for (i=FIRST_ELEM; i<len; i++)
				m.pMatrix[i].k*=k;
			m.pMatrix[OWNERS].n++;
			return m;
		}
		else
		{
			matrix n(m.pMatrix[Y_SIZE].n,m.pMatrix[X_SIZE].n);
			for (i=FIRST_ELEM; i<len; i++)
				n.pMatrix[i].k = m.pMatrix[i].k*k;
			n.pMatrix[TEMPFLAG].n=true;
			return n;
		}
	}
	else
		return m;
}

/***************************************************************************
 *                                                                         *
 * matrix * matrix                                                         *
 * ===============                                                         *
 * Das Produkt zweier Matrizen bestimmen.                                  *
 *                                                                         *
 ***************************************************************************/
matrix matrix::operator *(matrix &m)
{
	if (pMatrix && m.pMatrix)
	{
		if (pMatrix[X_SIZE].n == m.pMatrix[Y_SIZE].n)
		{
			matrix n(pMatrix[Y_SIZE].n,m.pMatrix[X_SIZE].n);

			double 		*p1=  &pMatrix[FIRST_ELEM].k,
								*p2=&m.pMatrix[FIRST_ELEM].k,
								*d =&n.pMatrix[FIRST_ELEM].k;
			unsigned  i,l=pMatrix[X_SIZE].n,
								y,ys=  pMatrix[Y_SIZE].n,
								x,xs=m.pMatrix[X_SIZE].n;
			for(y=0; y<ys; y++)
			{
				register double *memo=p2;
				for(x=0; x<xs; x++)
				{
					register double a=0.0;
					register double *q1=p1, *q2=p2;
					for(i=0; i<l; i++)
					{
						a += *(q1++) * *q2;
						q2+=xs;
					}
					*d=a;
					d++;
					p2++;
				}
				p2=memo;
				p1+=l;
			}

			return n;
		}
	}
	if (m.pMatrix)
	{
		matrix n;
		return n;
	}
	return m;
}

/***************************************************************************
 *                                                                         *
 * matrix t(matrix)                                                        *
 * ================                                                        *
 * Eine Matrix transponieren.                                              *
 *                                                                         *
 ***************************************************************************/
matrix t(matrix &m)
{
	if (m.pMatrix)
	{
		if ( m.pMatrix[TEMPFLAG].n &&
				 (m.pMatrix[X_SIZE].n==1 || m.pMatrix[Y_SIZE].n==1) )
		{
			register unsigned k;

			k=m.pMatrix[X_SIZE].n;
			m.pMatrix[X_SIZE].n=m.pMatrix[Y_SIZE].n;
			m.pMatrix[Y_SIZE].n=k;
			return m;
		}
		else
		{
			unsigned x=m.pMatrix[X_SIZE].n, y=m.pMatrix[Y_SIZE].n,
							 i,j;
			matrix n(x,y);
			register double *src=&m.pMatrix[FIRST_ELEM].k,
											*dst=&n.pMatrix[FIRST_ELEM].k,
											*memo=dst;

			for(j=0; j<y; j++)
			{
				for(i=0; i<x; i++)
				{
					*dst=*src;
					src++;
					dst+=y;
				}
				dst=++memo;
			}
			return n;
		}
	}
	return m;
}

/***************************************************************************
 *                                                                         *
 * double& matrix(y,x)                                                     *
 * ===================                                                     *
 * Ein Element einer Matrix zum Lesen bzw. Schreiben bereitstellen.        *
 *                                                                         *
 ***************************************************************************/
double& matrix::operator()(unsigned m,unsigned n)
{
	#ifdef SECURITY
	if((m<pMatrix[Y_SIZE].n) && (n<pMatrix[X_SIZE].n))
	{
	#endif
		return pMatrix[FIRST_ELEM+m*pMatrix[X_SIZE].n+n].k;
	#ifdef SECURITY
	}
	#endif
	printf("illegal matrix access\n");
	return *((double*) 0);		// wird evtl. ein NULL POINTER ASSIGNMENT verursachen
}

/***************************************************************************
 *                                                                         *
 * matrix.print()                                                          *
 * ==============                                                          *
 * Eine Matrix auf dem Bildschirm ausgeben                                 *
 *                                                                         *
 ***************************************************************************/
void matrix::print(void)
{
	if (pMatrix)
	{
		unsigned n,m,x,y;
		Matrix *src=pMatrix+FIRST_ELEM;
		x=pMatrix[X_SIZE].n;
		y=pMatrix[Y_SIZE].n;
		#ifdef DEBUG
		printf("%s:\n",name);
		#endif
		for(m=0; m<y; m++)
		{
			for(n=0; n<x; n++)
				printf("%5.1f ",(src++)->k);
			printf("\n");
		}
	}
	else
		printf("(NULL)\n");
}

/***************************************************************************
 *                                                                         *
 * Drehmatrix f�llen                                                       *
 *                                                                         *
 ***************************************************************************/
void Rotate3D(matrix &m, double x,double y,double z)
{
	m(0,0)=cos(y)*cos(z);
	m(0,1)=-cos(y)*sin(z);
	m(0,2)=sin(y);

	m(1,0)=cos(x)*sin(z) + sin(x)*sin(y)*cos(z);
	m(1,1)=cos(x)*cos(z) - sin(x)*sin(y)*sin(z);
	m(1,2)=-sin(x)*cos(y);

	m(2,0)=sin(x)*sin(z)-cos(x)*sin(y)*cos(z);
	m(2,1)=sin(x)*cos(z)+cos(x)*sin(y)*sin(z);
	m(2,2)=cos(x)*cos(y);
}

/***************************************************************************
 *                                                                         *
 * Drehmatrix f�llen                                                       *
 *                                                                         *
 ***************************************************************************/
void Translate3D(matrix &m, double x,double y,double z)
{
	m(0,3)=x;
	m(1,3)=y;
	m(2,3)=z;
}
