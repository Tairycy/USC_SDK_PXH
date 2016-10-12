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
 * $Id: vector.cpp,	v 1.0 2007/06/14 10:50:00 Sumu Exp $
 *
 *	
 * (C) Panasonic BeiJing Laboratory, PRDCC.
 *   18th Floor, Zhongguancun Building, No.27
 *             BeiJing, CHINA
 *
 * implementation file for template vector utility tools
 */

#include "vector.h"


template <class T>
void Vector<T>::norm()
{
	float fSum = 0.0 ;
	for (int i=0 ; i<nDim ; i++)
	{
		fSum += pT[i] * pT[i] ;
	}
	fSum = sqrt(fSum) ;

	*this *= 1/fSum ;

	return ;
}
// -------------------------------------------------
// Vector operator>>
// -------------------------------------------------

/*template <class T>
inline istream& operator>> (istream &is, Vector<T> &vec) 
{
	char s = ' ';
	char c = ',';
	char b = '[';
	char e = ']';
	int dimX = 0;
	T    val = (T) 0;
	CVector buf(65536);

	// get line and remove delimiters
	is.getline(buf.pT,buf.nDim,']');
	for (dimX=0;dimX<buf.nDim && buf.pT[dimX];dimX++) 
	{
		if (buf.pT[dimX] == c || buf.pT[dimX] == b || buf.pT[dimX] == e)
		{
			buf.pT[dimX] = s;
		}
	}
	istringstream line(buf.pT);

	dimX = 0;
	while (true) 
	{
		line >> val;
		if (line.fail())
		{
			break;
		}
		if (vec.nDim <= dimX)
		{
			vec.resize(dimX+10);
		}
		vec.pT[dimX++] = val;
	}
	vec.resize(dimX);
	return is;
}
*/
// -------------------------------------------------
// Vector operator<<
// -------------------------------------------------
/*
template <class T>
inline ostream& operator<< (ostream &os, Vector<T> &vec) 
{
	string s(" ");
	string c(",");
	string b("[");
	string e("]");
	os << b + s;
	for (int dimX=0;dimX<vec.nDim-1;dimX++)
	{
		os << vec.pT[dimX] << s << c << s;
	}
	if (vec.nDim)
	{
		os << vec.pT[vec.nDim-1] << s;
	}
	os << e;
	os << endl;
	return os;
}
*/
//2006-5-8...
//io parts....
/*template <class T>
int Vector<T>::read(char *pchFileName)
{
	return read(pchFileName, (*this)) ;
}

template <class T>
int Vector<T>::read(char* pchFileName, Vector& outVec)
{
	vtifstream is(pchFileName) ;
	if (!is)
	{
		ERROR("vector::read Unable to open the file(%s).\n", pchFileName) ;
		return 0 ;
	}
	read(is, outVec) ;

	return is.close() ;
}

template <class T>
int Vector<T>::read(vtifstream& is)
{
	return read(is, (*this)) ;
}

template <class T>
int Vector<T>::read(vtifstream& is, Vector& outVec)
{
	is.readNum((char*)&outVec.nDim, sizeof(int)) ;
	if (outVec.nDim <= 0)
	{
		ERROR("vector::read Invalid vector dimension, dim=%d.\n", outVec.nDim) ;
		return 0 ;
	}
	return is.readArray((char*)outVec.pT, sizeof(T), outVec.nDim) ;
}


template <class T>
int Vector<T>::write(char* pchFileName)
{
	return write(pchFileName, (*this)) ;
}

template <class T>
int Vector<T>::write(char *pchFileName, const Vector& inVec)
{
	vtofstream os(pchFileName) ;
	if (!os)
	{
		ERROR("vector::write Unable to open the file.(%s)\n", pchFileName) ;
		return 0 ;
	}
	write(os, inVec) ;

	return os.close() ;
}

template <class T>
int Vector<T>::write(vtofstream& os)
{
	return write(os, (*this)) ;
}

template <class T>
int Vector<T>::write(vtofstream& os, const Vector& inVec)
{
	if (inVec.nDim > 0)
	{
		os.writeNum((char*)&inVec.nDim, sizeof(int)) ;
		return os.writeArray((char*)inVec.pT, sizeof(T), inVec.nDim) ;
	}

	ERROR("vector::write Wrong dimension. dim=%d\n", inVec.nDim) ;
	return 0 ;
}
*/

