// FFT.cpp: implementation of the FFT class.
//
//////////////////////////////////////////////////////////////////////


#include "FFT.h"
#include "info.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FFT::FFT(const int nFrmLen )
{
	if( nFrmLen<=0 ){
		printf( "FFT::FFT nFrmLen = %d\n",nFrmLen );
		m_nFFT = -1;
		return;
	}
	int n = 1,count = 0;
	while( nFrmLen>n ){
		n *= 2;
		count++;
	}
	m_nOrder = count;
	m_nFFT = n;
}

void FFT::Execute(FVector& re,FVector& im )
{
	float *x = re.getDataBuf();
	float *y = im.getDataBuf();

	int nOrder = m_nOrder;
	
	int n1, n2, i, j, k, l;
	float xt, yt, c, s ;
	double e, a ;
	
	/*loop through all nOrder stages*/
	n2 = m_nFFT ;
	for (k = 0; k < nOrder ; k++) 
	{
		n1 = n2;
		n2 /= 2;
		e = 2*(double)PI / n1;
		for (j = 0; j < n2; j++) 
		{
			/* Compute Twiddle factors */
			a = j * e;
			c = (float) cos (a);
			s = (float) sin (a);
//			s = sqrt( 1.0f-c*c );
			/* Do the butterflies */
			for (i = j; i < m_nFFT; i += n1) 
			{
				l = i + n2;
				xt = x[i] - x[l];
				x[i] = x[i] + x[l];
				yt = y[i] - y[l];
				y[i] = y[i] + y[l];
				x[l] = c * xt + s * yt;
				y[l] = c * yt - s * xt;
			}
		}
	}
	/* Bit reversal: descrambling */
	j = 0;
	for (i = 0; i < m_nFFT - 1; i++) 
	{
		if (i < j) 
		{
			xt = x[j];
			x[j] = x[i];
			x[i] = xt;
			xt = y[j];
			y[j] = y[i];
			y[i] = xt;
		}
		k = m_nFFT / 2;
		while (k <= j) 
		{
			j -= k;
			k /= 2;
		}
		j += k;
	}
	
}

FFT::~FFT()
{

}
