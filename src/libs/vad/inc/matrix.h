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
 * $Id: matrix.h,	v 1.1 2007/06/14 10:50:00 Sumu Exp $
 *
 *	
 * (C) Panasonic BeiJing Laboratory, PRDCC.
 *   18th Floor, Zhongguancun Building, No.27
 *             BeiJing, CHINA
 *
 * declaration file for template matrix utility tools
 */

#ifndef __MATRIX_H
#define __MATRIX_H
//#define _NO_TEMPLATE

#include <float.h>

#include "commons.h"
//#include "vtfstream.h"

#ifndef __cplusplus
#error Must use C++ for template class
#endif



template <class T>
class Matrix
{
public:
	//constructor...
	Matrix();
	Matrix(size_t nInRow, size_t nInCol) ;
	Matrix(const Matrix<T>& mat) ;//deep copy constructor
//	Matrix(const Matrix& fromMatrix, int nFromLine, int nToLine, int inOwner);//Use data between nFromLine to nToLine of fromMatrix to construct the matrix...
	
	//destructor...
	~Matrix(); 
	
public://data member...	
	//assignment...
	void Assign(const Matrix<int>& mat) ;
	void Assign(const Matrix<float>& mat) ;
	void Assign(const Matrix<double>& mat) ;
	void Assign(const T value) ;
	void Chop(const Matrix<T> &mat, int nFromLine, int nToLine);//replace the original matrix with nFromLine to nToLine of fromMatrix ;
	void Chop(int nFromLine, int nToLine);	

	//operator overload...
	Matrix<T>&	operator=	(const Matrix<T> &mat);

	Matrix<T>	operator+	(const Matrix<T> &mat);
	Matrix<T>	operator-	(const Matrix<T> &mat);
	Matrix<T>	operator*	(const Matrix<T> &mat);
	Matrix<T>	operator/	(const T value);
	Matrix<T>&	operator~	();

	Matrix<T>&	operator+=	(const Matrix<T> &mat);
	Matrix<T>&	operator-=	(const Matrix<T> &mat);	
	Matrix<T>&	operator*=	(const Matrix<T> &mat);
	Matrix<T>&	operator+=	(const T value);
	Matrix<T>&	operator*=	(const T value);
	Matrix<T>&	operator/=	(const T value);
	
	T*			operator[]	(const int nInRow);//msu appended 2007/06/14
	T			operator()	(const int nInRow,const int nInCol);

	//Combination operation...
	void Append(const Matrix& mat) ;//Append apMatrix following the original matrix in row sequence
	void Fusion(const Matrix& mat) ;//Row of current matrix is created by the rows of two input matrix..

	//matrix operation...
	Matrix<T> Transpose() ;//transpose the original matrix...
	
	//mathematical operations
	void MLog();
	void Power( double value );

	// Symmetry Test
	bool Symtest(double thresh = 0);


	//io....
//	int Read(vtifstream& is) ;
//	int Read(char* pchFileName) ;

//	int Write(vtofstream& os) ;
//	int Write(char* pchFileName) ;
public://data member retrieval...
	size_t	GetRows();
	size_t	GetCols();
	T*		GetData();
	void	SetSize( size_t nRows,size_t nCols );
	T	GetRowSum( int nRows );
	T	GetColSum( int nCols );
protected:
    struct base_mat
    {
		T **pT;
		size_t Row, Col;
		int Refcnt;

		base_mat (size_t row, size_t col, T** v)
		{
			pT = NULL;
			Row = row;
			Col = col;
			Refcnt = 1;
			int i;
			char* mem = (char* )malloc( sizeof(T*)*row+sizeof(T)*row*col );
			if( !mem ){	
				printf( "base_mat::Val alloc fail\n" );
				return;
			}

			pT = (T** )(mem+sizeof(T)*row*col);
			pT[0] = (T* )mem;
			if( v&&v[0] )	memcpy( pT[0],v[0],sizeof(T)*col );
			for( i=1;i<row;i++ ){
				pT[i] = pT[i-1] + col;
				if( v&&v[i] )	memcpy( pT[i],v[i],sizeof(T)*col );
			}
			
			if( !v )
				memset( pT[0],0,sizeof(T)*row*col );
		}
		
		~base_mat ()
		{
			if( pT==NULL||pT[0]==NULL ){
				printf( "~base_mat destructor fail\n" );
				return;
			}
			free( (char* )pT[0] );
		}
    };
	base_mat* _m;
	void realloc( size_t row, size_t col );
	void clone();
} ;

//some definition....
// typedefs
typedef Matrix<char>      			CMatrix;
typedef Matrix<unsigned char>     	UCMatrix;
typedef Matrix<int>       			IMatrix;
typedef Matrix<float>     			FMatrix;
typedef Matrix<double>    			DMatrix;

template <class T>	inline
Matrix<T>::Matrix()
{
	_m = new base_mat( 1,1,NULL );	
}

template <class T>	inline
Matrix<T>::Matrix(size_t nInRow, size_t nInCol)
{
	_m = new base_mat( nInRow,nInCol,NULL );
} 

template <class T>	inline
Matrix<T>::Matrix(const Matrix<T>& mat)
{
	_m = mat._m;
	_m->Refcnt++;
}


template <class T>
Matrix<T>::~Matrix()
{
	if (--_m->Refcnt==0)
		delete _m;
}

template <class T>	inline size_t
Matrix<T>::GetCols()
{
	if( _m==NULL ){
		printf( "Matrix is not allocated\n" );
		return 0;
	}
	return _m->Col;
}

template <class T>	inline size_t
Matrix<T>::GetRows()
{
	if( _m==NULL ){
		printf( "Matrix is not allocated\n" );
		return 0;
	}
	return _m->Row;
}

template <class T>	inline T*
Matrix<T>::GetData()
{
	if( _m==NULL ){
		printf( "Matrix<T>::GetData Matrix is not allocated\n" );
		return NULL;
	}
	if( _m->Refcnt>1 )
		clone();

	return _m->pT[0];
}


template <class T>	inline T
Matrix<T>::GetColSum(int nCols )
{
	if( nCols<0||nCols>=_m->Col ){
		printf( "Matrix<T>::GetColSum nCols=%d exceeds Maximum Cols\n",nCols );
		return T(0);
	}

	T sum = T(0);
	for( int i=0;i<_m->Row;i++ )
		sum += _m->pT[i][nCols];
	return sum;
}

template <class T>	inline T
Matrix<T>::GetRowSum(int nRows )
{
	if( nRows<0||nRows>=_m->Row ){
		printf( "Matrix<T>::GetRowSum nCols=%d exceeds Maximum Cols\n",nRows );
		return T(0);
	}

	T sum = T(0);
	for( int i=0;i<_m->Col;i++ )
		sum += _m->pT[nRows][i];

	return sum;
}

template <class T>	inline void
Matrix<T>::clone()
{
	--_m->Refcnt;
	_m = new base_mat( _m->Row,_m->Col,_m->pT );
	if( _m==NULL ){
		printf( "Matrix<T>::clone alloc mem failed\n" );
	}
}

template <class T>	inline void
Matrix<T>::realloc(size_t row, size_t col )
{
	base_mat* mat = new base_mat( row,col,NULL );
	if( !mat ){
		printf( "Matrix<T>::realloc fail\n" );
		return;
	}

	size_t minRow = min( _m->Row,row );
	size_t minColSize = min( _m->Col,col )*sizeof(T);

	for( size_t i=0;i<minRow;i++ )
		memcpy( mat->pT[i],_m->pT[i],minColSize );

	if( --_m->Refcnt==0 )
		delete _m;

	_m = mat;
}

template <class T>	inline void
Matrix<T>::SetSize(size_t nRows,size_t nCols)
{
	size_t i,j;
	size_t OldRows = _m->Row;
	size_t OldCols = _m->Col;
	if( _m->Row!=nRows||_m->Col!=nCols ){
		realloc( nRows,nCols );
	}

	for( i=OldRows;i<nRows;i++ )
		for( j=0;j<nCols;j++ )
			_m->pT[i][j] = T(0);

	for( i=0;i<nRows;i++ )
		for( j=OldCols;j<nCols;j++ )
			_m->pT[i][j] = T(0);

}

/* 
 *	operator overload =
 */

template <class T>	inline Matrix<T>&
Matrix<T>::operator =( const Matrix<T>& mat )
{
	if( this==&mat ){
		return (*this);
	}
	
	mat._m->Refcnt++;
	if( --_m->Refcnt==0 )
		delete _m;

	_m = mat._m;
//	name = mat.name;
	return (*this);
}

/*
 *	operator overload [], Set operations, left-value granted
 */
template <class T>	inline T*
Matrix<T>::operator [](const int nInRow)
{
	if( nInRow>=_m->Row||nInRow<0 ){
		printf( "Matrix<T>::operator [] Index out of range!\n" );
		return NULL;
	}

	if( _m->Refcnt>1 )
		clone();

	return _m->pT[nInRow];
}

/*
 *	operator overload (), Get operations, Right-value only
 */

template <class T>	inline T
Matrix<T>::operator () (const int nInRow,const int nInCol)
{
	if( nInRow>=_m->Row||nInRow<0||nInCol>=_m->Col||nInCol<0 ){
		printf( "Matrix<T>::operator () Index out of range!\n" );
		return T(0);
	}
	return _m->pT[nInRow][nInCol];
}



/*
 *	operator overload +=
 */

template <class T >	inline Matrix<T>&
Matrix<T>::operator += (const Matrix<T>& mat)
{
	if( _m->Row!=mat._m->Row||_m->Col!=mat._m->Col ){
		printf( "Matrix<T>::operator += Inconsistant matrix sizes in addition\n" );
		return (*this);
	}
	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] += mat._m->pT[0][i];

	return (*this);
}

/*
 *	operator overload +=
 */

template <class T>	inline Matrix<T>&
Matrix<T>::operator += (const T value)
{
	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] += value;

	return (*this);
}

template <class T>	inline Matrix<T>&
Matrix<T>::operator -= (const Matrix<T> &mat)
{
	if( _m->Row!=mat._m->Row||_m->Col!=mat._m->Col ){
		printf( "Matrix<T>::operator -= Inconsistant matrix sizes in Subtraction\n" );
		return (*this);
	}
	if( _m->Refcnt>1 )
		clone();
	
	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] -= mat._m->pT[0][i];
	
	return (*this);
	
}

template <class T>	inline Matrix<T>
Matrix<T>::operator - (const Matrix<T> &mat)
{
	Matrix<T> tMat(*this);
	tMat -= mat;
	return tMat;
}

/*
 *	operator overload *=
 */

template <class T>	inline Matrix<T>&
Matrix<T>::operator *= (const T value)
{
	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] *= value;

	return (*this);

}

/*
 *	operator overload *=
 */

template <class T>	inline Matrix<T>&
Matrix<T>::operator *= (const Matrix<T> &mat)
{
	if( mat._m->Col*mat._m->Row==0||_m->Col!=mat._m->Row ){
		printf( "Matrix<T>::operator *= Inconsistant matrix sizes in multiplication\n" );
		return (*this);	
	}

	Matrix<T> tMat( _m->Row,mat._m->Col );
	for( int i=0;i<_m->Row;i++ )
		for( int j=0;j<mat._m->Col;j++ ){
			tMat._m->pT[i][j] = T(0);
			for( int k=0;k<_m->Col;k++ )
				tMat._m->pT[i][j] += _m->pT[i][k]*mat._m->pT[k][j];
		}
		
	(*this) = tMat;
	
	return (*this);
}

template <class T>	inline Matrix<T>
Matrix<T>::operator / (const T value)
{
	Matrix<T> tMat( *this );
	tMat /= value;
	return tMat;
}

template <class T>	inline Matrix<T>&
Matrix<T>::operator /= (const T value)
{
	if( value<(T)FLT_MIN ){
		printf( "Matrix<T>::operator /= Divided by value=%f\n",(float)value );
		return *this;
	}

	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] /= value;

	return (*this);
}
/*
 *	operator overload *
 */

template <class T>	inline Matrix<T>
Matrix<T>::operator * (const Matrix<T> &mat)
{
	Matrix<T> tMat( *this );
	tMat *= mat;
	return tMat;
}

/*
 *	operator overload +
 */

template <class T>	inline Matrix<T>
Matrix<T>::operator + (const Matrix<T> &mat)
{
	Matrix<T> tMat( *this );
	tMat += mat;
	return tMat;
}

template <class T>	inline Matrix<T>&
Matrix<T>::operator ~ ()
{

	Matrix<T> tMat( _m->Col,_m->Row );
	for( int i=0;i<_m->Col;i++ )
		for( int j=0;j<_m->Row;j++ )
			tMat._m->pT[i][j] = _m->pT[j][i];

	*this = tMat;
	return (*this);

}

template <class T>	inline Matrix<T>
Matrix<T>::Transpose()
{
	Matrix<T> tMat( *this );
	return ~tMat;
}

template <class T>	void
Matrix<T>::Assign(const T value)
{
	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] = value;
}

// Formated (ASCii) Input/Output
//template <class T> istream& operator>> (istream &is, Matrix<T> &mat);
//template <class T> ostream& operator<< (ostream &os, Matrix<T> &mat);


#endif
