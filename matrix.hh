// MATRIX.H
// Headerdatei zu MATRIX.CPP
// MatrixClass v1.01 (17.08.94)
// (c) 1994 Mark-André Hopf

union Matrix
{
	unsigned n;
	double k;
};

class matrix
{
	protected:
		Matrix *pMatrix;
		void init(unsigned y,unsigned x);
	public:
		matrix();
		matrix(unsigned y,unsigned x);
		matrix(const matrix& x);											// der Kopierkonstruktor
		#ifdef DEBUG
		char* name;
		matrix(unsigned y,unsigned x,char* name);
		#endif
		~matrix();

		void operator =(double* src);               	// matrix a=Feld von double
		void operator =(matrix);											// matrix a=matrix b
		friend matrix operator *(double k,matrix &m); // Multiplikation mit Skalar von links
																									// gemäß mathematische Definition
		matrix operator *(matrix &m);								  // Matrizenmultiplikation
		double& operator()(unsigned m,unsigned n);		// Komponenten ansprechen
		friend matrix t(matrix &i);
		void matrix::print(void);
};

void Translate3D(matrix &m, double x,double y,double z);
void Rotate3D(matrix &m, double x,double y,double z);
