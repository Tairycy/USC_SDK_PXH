// FFT.h: interface for the FFT class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	__FFT_H
#define __FFT_H 
#include "vector.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FFT  
{
public:
	FFT( const int nFrmLen );
	virtual ~FFT();
	void Execute( FVector& re,FVector& im );
private:
	int m_nFFT;
	int	m_nOrder;
};

#endif // !defined __FFT_H
