/* ----------------------------------------------------------- */
/*                                                             */
/*                     __   __  __ __                          */
/*                    |__| |__|   | |     SPEECH               */
/*                    |    |__| __| |__   RECOGNITION          */
/*                    =================   SOFTWARE             */
/*                                                             */
/*                                                             */
/* ----------------------------------------------------------- */
/*            Copyright: Panasonic Corporation                 */
/*                    2007 Beijing, CHINA                      */
/*                                                             */
/*        Use of this software is forbidden outside PBJL       */
/*    **     This banner notice must not be removed      **    */
/*                                                             */
/* ----------------------------------------------------------- */





/*
 * Basic Memory Management and I/O Tools 
 *
 * $Id: vector.h,	v 1.0 2007/06/14 10:50:00 Sumu Exp $
 *
 *	
 * (C) Panasonic BeiJing Laboratory, PRDCC.
 *   18th Floor, Zhongguancun Building, No.27
 *             BeiJing, CHINA
 *
 * declaration file for template vector utility tools
 */


#ifndef __VECTOR_H
#define __VECTOR_H

#include "commons.h"
#include "matrix.h"
//#include "vtfstream.h"

template <class T>
class Vector
{
public:
	int nHolders ;
	int nDim ;
	T* pT ;
	T* getDataBuf() const {return pT ; } ; 

	//constuctor...
	Vector() ;
	Vector(int nInDim);
	Vector(const Vector &vec);
	Vector(const Matrix<T> &mat, int column);
	~Vector();

	//memory allocation...
	int  resize(int nInDim) ;
	int  share(const Matrix<T>& inMatrix, int nRow) ;
	void chop(int nFrom, int nTo, const Vector<T>& inVec) ;
	void chop(int nFrom, int nTo) ;

	// Operator overloading...
	bool    operator==	(const Vector &inVec);
	Vector& operator=	(const Vector &inVec);
	Vector& operator+=	(const Vector &inVec);
	Vector  operator+	(const Vector &inVec);
	Vector& operator-=	(const Vector &inVec);
	Vector  operator-	(const Vector &inVec);
	T       operator*	(const Vector &inVec);
	T       operator*	(const T      *inVec);
	Vector& operator/=	(const T	  value);
	//added by msu
	T&		operator[] (const int nInRow);
	// Vector-Matrix operations
	Vector& operator*= (const Matrix<T> &inMatrix);
	Vector  operator*  (const Matrix<T> &inMatrix);

	// Vector-Scalar operation
	Vector& operator+= (const T value);
	Vector& operator*= (const T value);
	//add by zhu...
	Vector operator * (const T value);

	//assignment....
	T*	 move( int desIdx,int srcIdx,int blocks );	//memmove added by msu
	int  assign(const Vector<short>  &inVec) ;
	int  assign(const Vector<int>    &inVec) ;
	int  assign(const Vector<float>  &inVec) ;
	int  assign(const Vector<double> &inVec) ;
	int  assign(T value) ;//
	int  randomAssign(T lBound, T uBound) ;//assign the vector by random value between lBound and uBound...
	int  assign(const Matrix<T>& inMat, int nRow) ;//assign the vector by random value between lBound and uBound...

	//2006-5-8 io part...
//	int  read(char* pchFileName);
//	int  read(vtifstream& is);
//	int  read(char* pchFileName, Vector& outVec) ;
//	int  read(vtifstream& is, Vector& outVec) ;

//	int  write(char* pchFileName);
//	int  write(vtofstream& os) ;
//	int  write(char* pchFileName, const Vector& inVec) ;
//	int  write(vtofstream& os, const Vector& inVec) ;

	//normalize....
	void norm() ;

	T Sum();
	T Avg();
	Vector<T>& Dot( const Vector<T>& Vec );
} ;
//
//template<class T> istream& operator>> (istream &is, Vector<T> &vec);
//template<class T> ostream& operator<< (ostream &os, Vector<T> &vec);

// vector of chars, shorts, integer, floats, doubles
typedef Vector<char>   CVector;
typedef Vector<short>  SVector;
typedef Vector<int>    IVector;
typedef Vector<float>  FVector;
typedef Vector<double> DVector;


template<class T>
Vector<T>::Vector()
{
	pT   = NULL;
	nDim     = 0;
	nHolders = 1;
}

template <class T>
inline Vector<T>::Vector(int nInDim) 
{
	pT   = NULL;
	nDim     = 0;
	nHolders = 1;
	resize(nInDim);
}

template <class T>
inline Vector<T>::Vector(const Vector &inVec)
{
	pT   = NULL ;
	nDim     = 0 ;
	nHolders = 1 ;
	(*this) = inVec ;
}


template <class T>
inline Vector<T>::Vector(const Matrix<T> &inMatrix, int nRow)
{
	pT  = NULL;
	nDim    = 0;
	nHolders= 1;
	share(inMatrix,nRow);
}


// -------------------------------------------------
// Vector destructor
// -------------------------------------------------

template <class T>
inline Vector<T>::~Vector() 
{
	if (nHolders)
	{
		resize(0) ;
	}
}

//resize the vector...
template<class T>
int Vector<T>::resize(int nInDim)
{
	if (nDim == nInDim && pT != NULL )
	{
		return 1 ;
	}

	if (!nHolders) 
	{
		printf("Vector::resize no ownership\n");
		return 0 ;
	}

	// free memory
	if (nInDim == 0)
	{
		nDim = 0;
		if (pT)
		{
			free(pT);
		}
		pT = NULL;
		return 1 ;
	}

	// allocate vector
	pT = (T*) realloc((void*)pT,nInDim*sizeof(T));
	if (!pT)
	{
		printf("Vector allocation failure: dim= %d\n",nInDim);
		abort();
	}
	// init vector
	for (int i=nDim ; i<nInDim ; i++)
	{
		pT[i]= 0;
	}
	nDim = nInDim ;

	return 1 ;
} 

//share a memory with a matrix....
template <class T>
inline int Vector<T>::share(const Matrix<T> &inMatrix, int nRow)
{
	if (inMatrix.nRowDim <= nRow)
	{
	  printf("Vector::share invalid row %d (%d)\n",nRow,inMatrix.nRowDim);
	  return 0 ;
	}
    if (nHolders)
	{
		resize(0);
	}
    nHolders = 0;
	nDim  = inMatrix.nColDim;
	pT= inMatrix.pT[nRow];
  return 1 ;
}

//construct the vector with data from new vector between dim from and to...
//operator overload...
template <class T>
inline void Vector<T>::chop(int nFrom, int nTo,const Vector<T> &inVec)
{
	if (nFrom < 0 || nFrom >= inVec.nDim || nTo<0 || nTo>= inVec.nDim || nTo<=nFrom)
	{
		printf("Vector::chop invalid dimension dim= %d from= %d to= %d\n",inVec.nDim,nFrom,nTo);
		resize(0);
		return;
	}
	
	resize(nTo-nFrom+1);
	memcpy(pT,inVec.pT+nFrom,(nTo-nFrom+1)*sizeof(T));
	return;
}

template <class T>
inline void Vector<T>::chop(int nFrom, int nTo) 
{
	Vector tmpVec = *this ;
	chop(nFrom,nTo,tmpVec) ;
	return ;
}

// -------------------------------------------------
// Vector operator==
// -------------------------------------------------

template <class T>
inline bool Vector<T>::operator== (const Vector &inVec)
{
	if (nDim != inVec.nDim)
	{
		return false;
	}

	if (nDim*inVec.nDim ==0)
	{
		return false;
	}
	return (0 == memcmp((const void*)pT, (const void*)inVec.pT, sizeof(T)*nDim)) ;
}

// -------------------------------------------------
// Vector operator=
// -------------------------------------------------

template <class T>
inline Vector<T>& Vector<T>::operator= (const Vector &inVec)
{
	if (this == &inVec)
	{
		return (*this);
	}
	resize(inVec.nDim) ;

	if (nDim>0)
	{
		memcpy(pT,inVec.pT,nDim*sizeof(T));
	}
	return (*this);
}

// -------------------------------------------------
// Vector operator+
// -------------------------------------------------

template <class T>
inline Vector<T>& Vector<T>::operator+= (const Vector &inVec)
{
	if (nDim != inVec.nDim)
	{
		printf("Vector:operator+= dimension mismatch dim=%d/%d\n",nDim,inVec.nDim);
		return (*this);
	}

	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] += inVec.pT[i];
	}
	return (*this);
}

template <class T>
inline Vector<T>  Vector<T>::operator+ (const Vector &inVec)
{
	Vector<T> tmpVec(*this);
	tmpVec += inVec;
	return tmpVec ;
}

template <class T>
inline Vector<T>& Vector<T>::operator-= (const Vector &inVec)
{
	if (nDim != inVec.nDim)
	{
		printf("Vector:operator-= dimension mismatch dim=%d/%d\n",nDim,inVec.nDim);
		return (*this);
	}

	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] -= inVec.pT[i] ;
	}

	return (*this);
}


template <class T>
inline Vector<T>  Vector<T>::operator- (const Vector &inVec)
{
	Vector<T> tmpVec(*this) ;
	tmpVec -= inVec ;
	return tmpVec ;
}

template <class T>	inline Vector<T>&
Vector<T>::operator /= (const T value)
{
	if( value==T(0) ){
		printf( "Vector<T>::operator /= divided by zero\n" );
		return (*this);
	}

	for( int i=0;i<nDim;i++ )
		pT[i] /= value;

	return (*this);
}

// -------------------------------------------------
// Vector operator[]
// -------------------------------------------------
template <class T>
inline T& Vector<T>::operator[] (const int nInRow)
{
	static T a(0);
	if( nDim<nInRow ){
		printf( "Vector::operator[] nInRow=%d exceed dimension margin",nInRow);
		return a;
	}
	return pT[nInRow];
}
// -------------------------------------------------
// dot product
// -------------------------------------------------

template <class T>
inline T Vector<T>::operator*(const Vector &inVec)
{
	if (nDim != inVec.nDim) 
	{
		printf("Vector::operator* dimension mismatch dim1= %d dim2= %d\n",nDim,inVec.nDim);
		return 0;
	}
	return (*this) * inVec.pT ;
}

template <class T>
inline T Vector<T>::operator*(const T *inPt) 
{
	T res = 0;
	T *pt1 = pT ;
	T *pt2 = pT + nDim;
	for (;pt1!=pt2;)
	{
		res += (*pt1++)*(*inPt++);
	}
	return res ;
}

// -------------------------------------------------
// Vector-Matrix operations
// -------------------------------------------------

template <class T>
inline Vector<T>& Vector<T>::operator*= (const Matrix<T> &inMatrix) 
{
	if (nDim != inMatrix.nColDim) 
	{
		printf("Vector:operator*= dimension mismatch dim=%d with matrix dim=%d\n",nDim,inMatrix.nColDim);
		return (*this);
	}
	Vector<T> tmpVec(*this);
	(*this) = tmpVec * inMatrix;

	return (*this);
}

template <class T>
inline Vector<T>  Vector<T>::operator* (const Matrix<T> &inMatrix) 
{
	Vector<T> tmpVec(inMatrix.nRowDim);
	if (nDim != inMatrix.nColDim) 
	{
		printf("Vector:operator* dimension mismatch im=%d/%d\n",nDim,inMatrix.nColDim);
		return tmpVec;
	}
	for (int i=0; i<inMatrix.nRowDim;i++) 
	{
		for (int j=0; j<inMatrix.nColDim;j++) 
		{
			tmpVec.pT[i] += inMatrix.pT[i][j]*pT[j];
		}
	}
	return tmpVec ;
}

// -------------------------------------------------
// Vector-Scalar operations
// -------------------------------------------------

template <class T>
inline Vector<T>& Vector<T>::operator+= (const T value) 
{
	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] += value ;
	}

	return (*this);
}

template <class T>
inline Vector<T>& Vector<T>::operator*= (const T value) 
{
	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] *= value ;
	}
	return (*this);
}

template <class T>
inline Vector<T> Vector<T>::operator * (const T value)
{
	Vector<T> tmpVec(*this) ;
	for (int i=0 ; i<nDim ; i++)
	{
		tmpVec.pT[i] *= value ;
	}
	return tmpVec ;
}
//assignment....
// -------------------------------------------------
// Vector assign const value...
// -------------------------------------------------

template <class T>
inline int Vector<T>::assign(T value) 
{
	for (int i=0 ; i<nDim ; i++)
	{
		pT[i]= value ;
	}
  return 1 ;
}

// -------------------------------------------------
// Vector::conversion
// -------------------------------------------------

template <class T>
inline int Vector<T>::assign(const Vector<short> &inVec) 
{
	resize(inVec.nDim);

	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] = (T)inVec.pT[i];
	}
	return 1 ;
}

template <class T>
inline int Vector<T>::assign(const Vector<int> &inVec) 
{
	resize(inVec.nDim);

	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] = (T)inVec.pT[i];
	}
	return 1 ;
}

template <class T>
inline int Vector<T>::assign(const Vector<float> &inVec) 
{
	resize(inVec.nDim);

	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] = (T)inVec.pT[i];
	}
	return 1 ;
}

template <class T>
inline int Vector<T>::assign(const Vector<double> &inVec) 
{
	resize(inVec.nDim);

	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] = (T)inVec.pT[i];
	}
	return 1 ;
}

template <class T>
inline int Vector<T>::randomAssign(T lBound, T uBound) 
{
  for (int i=0;i<nDim;i++)
  {
	  pT[i]= lBound+(uBound-lBound)*rand()/RAND_MAX;
  }
  return 1 ;
}

template <class T>
inline int Vector<T>::assign(const Matrix<T>& inMat, int nRow)
{
	if (nDim != inMat.nColDim)
	{
		resize(inMat.nColDim) ;
	}

	for (int i=0 ; i<nDim ; i++)
	{
		pT[i] = inMat.pT[nRow][i] ;
	}
	return 1 ;
}

template <class T>
inline T* Vector<T>::move(int desIdx,int srcIdx,int blocks )
{
	if( desIdx<0||desIdx>=nDim||srcIdx<0||srcIdx>=nDim ){
		printf( "Vector<T>::move exceed dimension margin\n" );
		return NULL;
	}

	memmove( &pT[desIdx],&pT[srcIdx],sizeof(T)*blocks );
	return pT+desIdx;
}



template <class T> T
Vector<T>::Sum()
{
	T sum = T(0);
	for( int i=0;i<nDim;i++ )
		sum += pT[i];
	return sum;
}

template <class T> T
Vector<T>::Avg()
{
	T avg = Sum();
	return avg/nDim;
}



template <class T>	Vector<T>&
Vector<T>::Dot(const Vector<T>& Vec )
{
	if( nDim!=Vec.nDim ){
		printf("Vector<T>::Dot dim=%d\n", Vec.nDim) ;
		return (*this);
	}

	for( int i=0;i<nDim;i++ )
		pT[i] *= Vec.pT[i];

	return (*this);
}




//#include "vector.cpp"
#endif
