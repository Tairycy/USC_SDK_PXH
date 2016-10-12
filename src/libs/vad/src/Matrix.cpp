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
 * $Id: matrix.cpp,	v 1.1 2007/06/14 10:50:00 Sumu Exp $
 *
 *	
 * (C) Panasonic BeiJing Laboratory, PRDCC.
 *   18th Floor, Zhongguancun Building, No.27
 *             BeiJing, CHINA
 *
 * implementation file for template matrix utility tools
 */


#include "matrix.h"
#include <float.h>



template <class T>	void
Matrix<T>::Assign(const Matrix<int>& mat)
{
	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] = (T)mat._m->pT[0][i];
}

template <class T>	void
Matrix<T>::Assign(const Matrix<float>& mat)
{
	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] = (T)mat._m->pT[0][i];
}

template <class T>	void
Matrix<T>::Assign(const Matrix<double>& mat)
{
	if( _m->Refcnt>1 )
		clone();

	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] = (T)mat._m->pT[0][i];
}

template <class T>	void
Matrix<T>::Chop(const Matrix<T> &mat, int nFromLine, int nToLine)
{
	Matrix<T> tMat( mat );
	tMat.Chop( nFromLine,nToLine );
	(*this) = tMat;
}

template <class T>	void
Matrix<T>::Chop(int nFromLine, int nToLine)
{
	if( nFromLine<0||nFromLine>=_m->Row||nFromLine>=nToLine||
		nToLine<0||nToLine>=_m->Row){
		printf( "Matrix::Chop invalid dimension row= %d, fromLine= %d, toLine= %d\n",\
			_m->Row,nFromLine,nToLine);
		return;
	}
	Matrix<T> tMat( nToLine-nFromLine+1,_m->Col );
	memcpy( tMat._m->pT[0],_m->pT[nFromLine],(nToLine-nFromLine+1)*_m->Col*sizeof(T) );
	*this = tMat;
	return;
}

template <class T>	void
Matrix<T>::Append(const Matrix& mat)
{
	if( _m->Col!=mat._m->Col ){
		printf( "Matrix<T>::Append dim mismatch col = %d from append col = %d",_m->Col,mat._m->Col );
		return;
	}

	Matrix<T> tMat( *this );
	tMat.realloc( _m->Row+mat._m->Row,_m->Col );
	memcpy( tMat._m->pT[_m->Row],mat._m->pT[0],mat._m->Row*mat._m->Col*sizeof(T) );
	*this = tMat;
	return;
}

template <class T>	void
Matrix<T>::Fusion(const Matrix& mat)
{
	if( _m->Row!=mat._m->Row ){
		printf( "Matrix<T>::Append dim mismatch row = %d from append row = %d",_m->Row,mat._m->Row );
		return;
	}

	Matrix<T> tMat( *this );
	tMat.realloc( _m->Row,_m->Col+mat._m->Col );
	for( int i=0;i<_m->Row;i++ )
		memcpy( tMat._m->pT[i]+_m->Col,mat._m->pT[i],mat._m->Col*sizeof(T) );
	*this = tMat;
	return;
}


template <class T>	void
Matrix<T>::MLog()
{
	if( _m->Refcnt>1 )
		clone();
	double dOffSet = 0.0000000001 ;
	double min = FLT_MAX;
	for( int i=0;i<_m->Row*_m->Col;i++ ){
		if( min>_m->pT[0][i] )
			min = _m->pT[0][i];

		double Tmp = _m->pT[0][i]+dOffSet;
		if( Tmp>FLT_MIN )
			_m->pT[0][i] = (T) log( Tmp );
		else
			_m->pT[0][i] = (T)FLT_MIN_EXP;
	}

	if( min>=FLT_MAX||min<0.0f ){
		printf( "Matrix::MLog invalid values (min= %f)\n",min );
	}
	return;
}

template <class T>	void
Matrix<T>::Power(double value )
{
	if( _m->Refcnt>1 )
		clone();
	
	for( int i=0;i<_m->Row*_m->Col;i++ )
		_m->pT[0][i] = (T)pow( (double)_m->pT[0][i],value );

}

template <class T>	bool
Matrix<T>::Symtest(double thresh /* = 0 */)
{
	if( _m->Row!=_m->Col )
		return false;

	T** Ptr = _m->pT;
	for( int i=0;i<_m->Row;i++ )
		for( int j=0;j<_m->Col;j++ )
			if( fabs(Ptr[i][j]-Ptr[j][i])>thresh ){
				printf( "Matrix<T>::Symtest failed %f %f\n",Ptr[i][j],Ptr[j][i] );
				return false;
			}
	return true;
}
/*
template <class T>
inline ostream& operator<< (ostream &os, Matrix<T> &mat) {
	string s(" ");
	string c(",");
	string b("[");
	string e("]");
	int Rows = mat.GetRows();
	int Cols = mat.GetCols();

	os << b + s;
	for (int dimX=0;dimX<Rows;dimX++) {
		os << b + s;
		for (int dimY=0;dimY<Cols;dimY++) {
			os << mat(dimX,dimY) << s;
			if (dimY < Cols-1) {
        		os << c << s;
 			} else {
				os << e << s;
			}
		}
		if (dimX < Rows-1) {
			os << c << s;
		} else {
			os << e;
		}
	}
	os << endl;
	return os;

}

template <class T>	int
Matrix<T>::Read(vtifstream& is)
{
	int Rows;
	int Cols;
	is.readNum( (char* )&Rows,sizeof(int) );
	is.readNum( (char* )&Cols,sizeof(int) );
	
	if( Rows*Cols<0 ){
		ERROR( "Matrix<T>::Read Invalid dimension, row=%d, col=%d.\n",Rows,Cols );
		return 0;
	}

	Matrix<T> tMat( Rows,Cols );
	for( int i=0;i<Rows;i++ )
		is.readArray( (char* )tMat._m->pT[i],sizeof(T),Cols );
	
	*this = tMat;
	return 1;
}


template <class T>	int
Matrix<T>::Read(char *pchFileName)
{
	vtifstream is( pchFileName );
	if( !is ){
		ERROR( "Matrix::Read Unable to open the file(%s)\n",pchFileName );
		return 0;
	}

	Read( is );
	
	return is.close();
}


template <class T>	int
Matrix<T>::Write(vtofstream& os)
{
	int Rows = _m->Row;
	int Cols = _m->Col;

	if( Rows*Cols<0 ){
		ERROR( "Matrix<T>::Write Invalid dimension, row=%d, col=%d.\n",Rows,Cols );
		return 0;
	}

	os.writeNum( (char* )&Rows,sizeof(int) );
	os.writeNum( (char* )&Cols,sizeof(int) );
	for( int i=0;i<Rows;i++ )
		os.writeArray( (char* )_m->pT[i],sizeof(T),Cols );

	return 1;
}


template <class T>	int
Matrix<T>::Write(char* pchFileName)
{
	vtofstream os( pchFileName );
	if (!os)
	{
		ERROR("Matrix::Write Unable to open file(%s)\n", pchFileName) ;
		return 0 ;
	}
	Write( os );
	return os.close();

}
*/