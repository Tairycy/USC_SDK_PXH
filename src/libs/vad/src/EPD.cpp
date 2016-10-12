// EPD.cpp: implementation of the EPD class.
//
//////////////////////////////////////////////////////////////////////


#include "EPD.h"

// DWORD start;
// DWORD clock_1 = 0;
// DWORD clock_2 = 0;
// DWORD clock_3 = 0;
// DWORD clock_4 = 0;

typedef long long __int64;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef __FIX_POINT__
void EPD::rfft_fix( int* x,int n,int m ){
	int j, i, k, is, id;
	int i0, i1, i2, i3, i4, i5, i6, i7, i8;
	int n2, n4, n8;
	int xt, a0;
	__int64 t1, t2, t3, t4, t5, t6;
//	int t1, t2, t3, t4, t5, t6;
	int cc1, ss1, cc3, ss3,tabCount;
	int *r0;
	int* tab = (int* )SinTab;
	/* Digit reverse counter */

	j = 0;
	r0 = x;
	tabCount = 0;
	for (i = 0; i < n - 1; i++) {

		if (i < j) {
			xt = x[j];
			x[j] = *r0;
			*r0 = xt;
		}
		r0++;

		k = n >> 1;

		while (k <= j) {
			j = j - k;
			k >>= 1;
		}
		j += k;
	}

	/* Length two butterflies */
	is = 0;
	id = 4;

	while (is < n - 1) {

		for (i0 = is; i0 < n; i0 += id) {
			i1 = i0 + 1;
			a0 = x[i0];
			x[i0] += x[i1];
			x[i1] = a0 - x[i1];
		}

		is = (id << 1) - 2;
		id <<= 2;
	}

	/* L shaped butterflies */
	n2 = 2;
	for (k = 1; k < m; k++) {
		n2 <<= 1;
		n4 = n2 >> 2;
		n8 = n2 >> 3;
//		e = (M_PI * 2) / n2;
		is = 0;
		id = n2 << 1;
		while (is < n) {
			for (i = is; i <= n - 1; i += id) {
				i1 = i;
				i2 = i1 + n4;
				i3 = i2 + n4;
				i4 = i3 + n4;
				t1 = x[i4] + x[i3];
				x[i4] -= x[i3];
				x[i3] = x[i1] - t1;
				x[i1] += t1;

				if (n4 != 1) {
					i1 += n8;
					i2 += n8;
					i3 += n8;
					i4 += n8;
					t1 = ((x[i3] + x[i4]) * FIX_QN_FFT) >> FFT_SHIFT;
					t2 = ((x[i3] - x[i4]) * FIX_QN_FFT) >> FFT_SHIFT;
					x[i4] = x[i2] - t1;
					x[i3] = -x[i2] - t1;
					x[i2] = x[i1] - t2;
					x[i1] = x[i1] + t2;
				}
			}
			is = (id << 1) - n2;
			id <<= 2;
		}

		for (j = 1; j < n8; j++) {
			cc1 = *tab++;
			ss1 = *tab++;
			cc3 = *tab++;
			ss3 = *tab++;
			is = 0;
			id = n2 << 1;

			while (is < n) {
				for (i = is; i <= n - 1; i += id) {
					i1 = i + j;
					i2 = i1 + n4;
					i3 = i2 + n4;
					i4 = i3 + n4;
					i5 = i + n4 - j;
					i6 = i5 + n4;
					i7 = i6 + n4;
					i8 = i7 + n4;
					t1 = (__int64)x[i3] * (__int64)cc1 + (__int64)x[i7] * (__int64)ss1;
					t2 = (__int64)x[i7] * (__int64)cc1 - (__int64)x[i3] * (__int64)ss1;
					t3 = (__int64)x[i4] * (__int64)cc3 + (__int64)x[i8] * (__int64)ss3;
					t4 = (__int64)x[i8] * (__int64)cc3 - (__int64)x[i4] * (__int64)ss3;
					t1 >>= FFT_SHIFT;
					t2 >>= FFT_SHIFT;
					t3 >>= FFT_SHIFT;
					t4 >>= FFT_SHIFT;

					t5 = t1 + t3;
					t6 = t2 + t4;
					t3 = t1 - t3;
					t4 = t2 - t4;
					t2 = x[i6] + t6;
					x[i3] = t6 - x[i6];
					x[i8] = t2;
					t2 = x[i2] - t3;
					x[i7] = -x[i2] - t3;
					x[i4] = t2;
					t1 = x[i1] + t5;
					x[i6] = x[i1] - t5;
					x[i1] = t1;
					t1 = x[i5] + t4;
					x[i5] = x[i5] - t4;
					x[i2] = t1;
				}
				is = (id << 1) - n2;
				id <<= 2;
			}
		}
	}

	for (i = n; i --; ) {
		*x++ >>= 6;
	}


}

#endif
EPD::EPD(const int nFrmLen,const int nSftLen,const int nHstLen, const int nDownLen /* = 16 */,
		 const int SnrS /* = EPD_SNR_THRD_ST */,const int SnrE /* = EPD_SNR_THRD_ED */)
		 :m_pFFT(NULL)
{
	m_nFrmLen = nFrmLen;
	m_nSftLen = nSftLen;
	m_nHisLen = nHstLen;
	m_nDowLen = nDownLen;
	m_nCurFrm = 0;
	m_FrmWin.resize( nFrmLen );
#ifdef __FIX_POINT__
	m_last_sample = 0;
	m_FrmWin.assign( 0 );

#else
	m_last_sample = 0.0f;
	m_FrmWin.assign( 0.0f );
#endif

	m_HamWin.resize(nFrmLen);
	float a = TPI/(nFrmLen - 1);
#ifdef __FIX_POINT__
	for( int i=0;i<nFrmLen;i++ ){
		m_HamWin.pT[i] = (PARAM_TYPE)((0.54f - 0.46f * cos(a*i))*(1<<15));
	}
#else
	for( int i=0;i<nFrmLen;i++ )
		m_HamWin[i] = 0.54f - 0.46f * cos(a*i);
#endif
	
	m_pFFT = new FFT( nFrmLen );
	
	
	m_pMask = new CNoiseMask( nHstLen,MASK_PRECISION,nDownLen );
	m_pMask->IniSpHistograme();
	m_nFFTLen = 1;
	while( m_nFFTLen<nFrmLen )
		m_nFFTLen *= 2;

	
	

	m_maskLo = 0.0f;
	m_maskHi = 0.0f;
	m_measure = 0;
	m_nbSpeechFrm = 0;
	m_nHangOver = 0;

	m_chBufMin.resize( MIN_FILTER );
	m_chBufMax.resize( MAX_FILTER );
	
	m_chBufMin.assign( 0 );
	m_chBufMax.assign( 0 );

	m_SnrS = SnrS;
	m_SnrE = SnrE;

	m_bST = false;
	m_nST = 0;

	m_noiseM.resize( nDownLen );
	m_noiseM.assign( 0.0f );

	m_SigM = -1.0f;
	m_SigT = 0.0f;

	m_bTH = false;
#ifdef __LOG_EPD__
	fp = fopen( "epd.log","wt" );
#endif
}

EPD::~EPD()
{
	if( m_pFFT )
		delete m_pFFT;
	if( m_pMask )
		delete m_pMask;
#ifdef __LOG_EPD__
	if( fp )
		fclose( fp );
#endif

}



void EPD::PreEmph(PARAM_VECTOR& re )
{
	int i,nDim = re.nDim;
	PARAM_TYPE last = re[nDim-1];
#ifdef __FIX_POINT__
	int t1,t2;
	for( i=nDim-1;i>=1;i-- ){
		t1 = re.pT[i]<<15;
		t2 = re.pT[i-1] * PRE_EMPHASIS;
		re[i] = (t1-t2)>>15;
	}
	t1 = re.pT[0]<<15;
	t2 = m_last_sample * PRE_EMPHASIS;
	re[0] = (t1-t2)>>15;

#else
	for( i=nDim-1;i>=1;i-- )
		re[i] -= re[i-1]*PRE_EMPHASIS;
	re[0] -= m_last_sample*PRE_EMPHASIS;
#endif
	m_last_sample = last;
}

void EPD::HammWin(PARAM_VECTOR& re )
{
	re.Dot( m_HamWin );
}

float EPD::SpectralEnergy(Spectrum& frame )
{
	int i,j;
	if( m_nSftLen!=frame.audio_buffer.nDim ){
		printf( "frame length = %d not coincident with input frame %d\n",m_nFrmLen,frame.audio_buffer.nDim );
		return -1.0f;
	}


	PARAM_VECTOR re;
	PARAM_VECTOR im( m_nFFTLen );
	re.assign( frame.audio_buffer );
	im.assign( 0 );

/************************************************************************/
/*	Pre-emphasize                                                       */
/************************************************************************/
	PreEmph( re );

/************************************************************************/
/*	Circulate Buffer for one frame length                               */
/************************************************************************/
	for( i=0;i<m_FrmWin.nDim-re.nDim;i++ )
		m_FrmWin[i] = m_FrmWin[i+re.nDim];

	for( j=0;i<m_FrmWin.nDim;i++,j++ )
		m_FrmWin[i] = re[j];
	
/************************************************************************/
/*	Hamming Window  Q6 for fix-point                                    */
/************************************************************************/
#ifdef __FIX_POINT__
	int* pWinI = m_FrmWin.pT;
	int* pWinO = m_FrmWin.pT;
	int* pHam = m_HamWin.pT;
	for( i=0;i<m_FrmWin.nDim;i++ ){
		*pWinO++ = (*pWinI++ * *pHam++)>>9;
	}
#else
	m_FrmWin.Dot( m_HamWin );
#endif
/************************************************************************/
/*	FFT                                                                 */
/************************************************************************/
	re.assign( m_FrmWin );
	re.resize( m_nFFTLen );

#ifdef __FIX_POINT__
	rfft_fix( re.pT,m_nFFTLen,9 );
#else
	m_pFFT->Execute( re,im );
#endif


#ifdef __FIX_POINT__
	frame.spectrum[0] = (float)((__int64)re.pT[0] * (__int64)re.pT[0]);
	for( i=1;i<256;i++ ){
		frame.spectrum[i] = (float)((__int64) re.pT[i] * (__int64) re.pT[i] + (__int64) re.pT[FFT_LENGTH - i] * (__int64) re.pT[FFT_LENGTH - i]);

	} 
	frame.spectrum[256] = (float)((__int64)re.pT[256] * (__int64)re.pT[256]);

#else
	frame.spectrum[0] = re[0]*re[0]+im[0]*im[0];
	for( i=0;i<m_nFFTLen/2;i++ )
		frame.spectrum[i] = 2*(re[i]*re[i]+im[i]*im[i]);
	frame.spectrum[i] = re[i]*re[i]+im[i]*im[i];
#endif
	

	float en = 0;
//	for( i=0;i<frame.spectrum.nDim;i++ ){
//		//en += frame.spectrum[i];
//		if(frame.spectrum[i]<0)
//		{
//			printf("%f\n",frame.spectrum[i]);
//			en += 1;
//		}
//		if (en > 10)
//			exit(0);
//	}

	
/************************************************************************/
/*	Subtract Mean Value                                                 */
/************************************************************************/

// 	for( i=0;i<frame.spectrum.nDim;i++ ){
// 		frame.spectrum[i] -= en;
// 		if( frame.spectrum[i]<0.0f )
// 			frame.spectrum[i] = 1e-5f;
// 	}
	//printf("%f\n",en);
	return en;


}

void EPD::DownSample(Spectrum& frame,FVector& spectrum )
{
	int down_size = spectrum.nDim;
	float sum;
	float floor = m_noiseM.Sum()/m_noiseM.nDim;
	for( int i=0;i<down_size;i++ ){
		sum = 0.3f*floor;//little constant mask. can be very tricky!!!
		for( int j=m_nFFTLen*i/(2*down_size);j<m_nFFTLen*(i+1)/(2*down_size);j++ ){
#ifdef BAND_LIMIT
			float Hz;
			Hz = j*SM_FREQ*1.0f/m_nFFTLen;
			if( Hz<LO_FREQ||Hz>HI_FREQ )
				continue;
#endif
			sum += frame.spectrum[j];
		}
		spectrum[i] = sum;
	}
	
	

}

void EPD::Feed(Spectrum& frame,Result* res )
{

	char measure;
	int i;
	m_nCurFrm++;
	
	m_SigM = -1.0f;
	for( i=0;i<frame.audio_buffer.nDim;i++ ){
		float val = frame.audio_buffer[i];
		if( m_SigM<fabs(val) )
			m_SigM = fabs(val);
	}

/************************************************************************/
/*	Get FFT Power Spectrum                                              */
/************************************************************************/
//	start = GetTickCount();
	float frameEn = SpectralEnergy( frame );

//	clock_1 += (GetTickCount() - start);

/************************************************************************/
/*	Down-sample from 256->4 Bands                                       */
/************************************************************************/
	FVector spectrum;
	spectrum.resize( m_nDowLen );

//	start = GetTickCount();
	DownSample( frame,spectrum );
//	clock_2 += ( GetTickCount()-start );

/************************************************************************/
/*	Get Noise masking factor for each of 8 Bands                        */
/************************************************************************/
//	FVector mask(spectrum);

	FMatrix maskM;
//	start = GetTickCount();
	m_pMask->GetQuantMask( spectrum,maskM );
//	clock_3 += ( GetTickCount()-start );

/************************************************************************/
/*	EPD Decision based on frame energy <---> mask                       */
/************************************************************************/
//	start = GetTickCount();
	measure = EstimateMeasure( spectrum,maskM );
//	clock_4 += ( GetTickCount()-start );

	for( i=0;i<m_noiseM.nDim;i++ )
		m_noiseM[i] = maskM[i][MASK_LO];

	
	if( res ){
		res->frameEn = 10.0f*log10(spectrum.Sum());
		res->mask = 10.0f*log10(maskM.GetColSum( MASK_HI ));
		res->type = measure;
	}
}

char EPD::EstimateMeasure(FVector& spectrum,FMatrix& maskM )
{
	char	minmax = 0;
	int i;
	float	LamdaLTE;
	float	frameEn = 10.0f*log(spectrum.Sum());
// 	float	frameEn = 10.0f*pow( spectrum.Sum(),0.1f );
// 	float	frameEn = 10.0f*log10( spectrum.Sum() );
	if( m_nCurFrm<EPD_FRAME_THRD_LTE )
		LamdaLTE = 1-1.0f/m_nCurFrm;
	else
		LamdaLTE = (float)EPD_LAMDA_LTE;	

	float difEnST = frameEn-m_maskLo;
	float difEnED;
	if( m_nCurFrm<m_nHisLen ){
		if( difEnST<EPD_SNR_UPD_LTE||m_nCurFrm<EPD_MIN_FRAME )
			if( difEnST<0||m_nCurFrm<EPD_MIN_FRAME )
				m_maskLo += (1-LamdaLTE)*difEnST;
			else
				m_maskLo += (1-EPD_LAMDA_LTE_HIGH_E)*difEnST;	
		m_maskHi = m_maskLo;
	}else{
		m_maskHi = 10.0f*log(maskM.GetColSum( MASK_HI ));
		m_maskLo = 10.0f*log(maskM.GetColSum( MASK_LO ));

// 		m_maskHi = maskM.GetColSum( MASK_HI );
// 		m_maskLo = maskM.GetColSum( MASK_LO );

		
		
	}//temporary code



	difEnST = frameEn-m_maskHi;
	difEnED = frameEn-m_maskLo;


	if( m_nCurFrm<EPD_MIN_FRAME ){
#ifdef __LOG_EPD__
//	FLOG( "%f\n",m_maskLo,fp );
	fprintf( fp,"MEASURE[%04d]=%d-->%+5.3f\t%+5.3f\t%+5.3f\t%f::%f\n",
		m_nCurFrm,m_measure,frameEn,difEnST,difEnED,m_SigM,m_SigT );

#endif
		return 0;
	}

	
//	if( difEnST>m_SnrS ){
	if( difEnST>m_SnrS&&m_SigM>m_SigT ){
		m_nST++;
	}
	else{
		m_nST = 0;
	}



	if( m_nST==EPD_MIN_HANGOVER&&!m_bST||(difEnED>m_SnrE&&m_measure) ){
		m_nST = 0;
		m_nbSpeechFrm++;
		m_measure = 1;
		m_bST = true;
		m_bTH = true;
	}else{
		if( m_nbSpeechFrm>EPD_MIN_HANGOVER )
			m_nHangOver = EPD_HANGOVER;
		m_nbSpeechFrm = 0;
		if( m_nHangOver ){
			m_nHangOver--;
			m_measure = 1;
		}
		else{
			m_measure = 0;
			m_bST = false;
			if( m_bTH ){
				m_SigT = maskM.GetColSum( MASK_PRECISION );
				m_SigT = 0.1f*sqrt(m_SigT/m_nFrmLen);
				if( m_SigT>5000.0f )
					m_SigT = 5000.0f;
				m_bTH = false;
			}

		}
	}



	for( int i=m_chBufMin.nDim-1;i>0;i-- ){
		m_chBufMin[i] = m_chBufMin[i-1];
	}
	m_chBufMin[0] = m_measure;


	minmax = 1;
	for( i=0;i<m_chBufMin.nDim;i++ )
		minmax = minmax&&m_chBufMin[i];
	
	

	for( i=m_chBufMax.nDim-1;i>0;i-- )
		m_chBufMax[i] = m_chBufMax[i-1];
	m_chBufMax[0] = minmax;
	
	minmax = 0;	
	for( i=0;i<m_chBufMax.nDim;i++ )
		minmax = minmax||m_chBufMax[i];

	
	if(minmax==1){
		int we = 0;
	}
#ifdef __LOG_EPD__
//	FLOG( "%f\n",m_maskLo,fp );
	fprintf( fp,"MEASURE[%04d]=%d-->%+5.3f\t%+5.3f\t%+5.3f\t%f::%f\n",
		m_nCurFrm,m_measure,frameEn,difEnST,difEnED,m_SigM,m_SigT );
	
#endif
	return minmax;
}

void EPD::Reset()
{
	m_chBufMin.assign( 0 );
	m_chBufMax.assign( 0 );
	m_pMask->ResetQuantile();
	m_measure = 0;
	m_nCurFrm = 0;

#ifdef __FIX_POINT__
	m_last_sample = 0;
	m_FrmWin.assign( 0 );

#else
	m_last_sample = 0.0f;
	m_FrmWin.assign( 0.0f );
#endif

	m_maskLo = 0.0f;
	m_maskHi = 0.0f;
	m_nbSpeechFrm = 0;
	m_nHangOver = 0;

	m_bST = false;
	m_nST = 0;

	m_noiseM.assign( 0.0f );

	m_SigM = -1.0f;
	m_SigT = 0.0f;

	m_bTH = false;
}

int EPD::GetOffset()
{
	return 2*MIN_FILTER+MAX_FILTER;
}
