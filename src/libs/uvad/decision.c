#include "decision.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "table.h"



#define CIRCULAR_BUF_LEN    2048
#define POS( x )    (x) & 0x7FF

static const double AC_8K[9]={ 0.00063902832335,0.0000000000000,-0.0025561132934,0.0000000000000,0.0038341699401, 0.0000000000000,-0.0025561132934,0.0000000000000,0.00063902832335};
static const double BC_8K[9]={1.0000000000000,-6.9576672372925,21.312174917105,-37.549938506820,41.631699391870,-29.746574056242,13.377806561302,-3.4623479623757,0.3948477508267};

static const double AC_16K[9]={ 0.0000488732840,0.0000000000000,-0.0001954931358,0.0000000000000,0.0002932397038, 0.0000000000000,-0.0001954931358,0.0000000000000, 0.0000488732840};
static const double BC_16K[9]={1.0000000000000,-7.5239452355417,24.7907337728694,-46.7226043935874,55.0917662822736,-41.6175479263044,19.6699349028760,-5.3180551545137,0.6297177521051};

static const float meTab[] = {
1.00f, 1.00f, 0.99f, 0.99f, 0.98f, 0.98f, 0.97f, 0.97f, 0.96f, 0.96f, 
0.95f, 0.95f, 0.94f, 0.94f, 0.93f, 0.93f, 0.93f, 0.92f, 0.92f, 0.91f, 
0.91f, 0.90f, 0.90f, 0.89f, 0.89f, 0.89f, 0.88f, 0.88f, 0.87f, 0.87f, 
0.87f, 0.86f, 0.86f, 0.85f, 0.85f, 0.84f, 0.84f, 0.84f, 0.83f, 0.83f, 
0.82f, 0.82f, 0.82f, 0.81f, 0.81f, 0.80f, 0.80f, 0.80f, 0.79f, 0.79f, 
0.79f, 0.78f, 0.78f, 0.77f, 0.77f, 0.77f, 0.76f, 0.76f, 0.76f, 0.75f, 
0.75f, 0.74f, 0.74f, 0.74f, 0.73f, 0.73f, 0.73f, 0.72f, 0.72f, 0.72f, 
0.71f, 0.71f, 0.71f, 0.70f, 0.70f, 0.70f, 0.69f, 0.69f, 0.69f, 0.68f, 
0.68f, 0.68f, 0.67f, 0.67f, 0.67f, 0.66f, 0.66f, 0.66f, 0.65f, 0.65f, 
0.65f, 0.64f, 0.64f, 0.64f, 0.63f, 0.63f, 0.63f, 0.63f, 0.62f, 0.62f, 
0.62f, 0.61f, 0.61f, 0.61f, 0.60f, 0.60f, 0.60f, 0.60f, 0.59f, 0.59f, 
0.59f, 0.58f, 0.58f, 0.58f, 0.58f, 0.57f, 0.57f, 0.57f, 0.56f, 0.56f, 
0.56f, 0.56f, 0.55f, 0.55f, 0.55f, 0.55f, 0.54f, 0.54f, 0.54f, 0.53f, 
0.53f, 0.53f, 0.53f, 0.52f, 0.52f, 0.52f, 0.52f, 0.51f, 0.51f, 0.51f, 
0.51f, 0.50f, 0.50f, 0.50f, 0.50f, 0.49f, 0.49f, 0.49f, 0.49f, 0.48f, 
0.48f, 0.48f, 0.48f, 0.47f, 0.47f, 0.47f, 0.47f, 0.47f, 0.46f, 0.46f, 
0.46f, 0.46f, 0.45f, 0.45f, 0.45f, 0.45f, 0.45f, 0.44f, 0.44f, 0.44f, 
0.44f, 0.43f, 0.43f, 0.43f, 0.43f, 0.43f, 0.42f, 0.42f, 0.42f, 0.42f, 
0.42f, 0.41f, 0.41f, 0.41f, 0.41f, 0.40f, 0.40f, 0.40f, 0.40f, 0.40f, 
0.39f, 0.39f, 0.39f, 0.39f, 0.39f, 0.38f, 0.38f, 0.38f, 0.38f, 0.38f, 
0.38f, 0.37f, 0.37f, 0.37f, 0.37f, 0.37f, 0.36f, 0.36f, 0.36f, 0.36f, 
0.36f, 0.35f, 0.35f, 0.35f, 0.35f, 0.35f, 0.35f, 0.34f, 0.34f, 0.34f, 
0.34f, 0.34f, 0.34f, 0.33f, 0.33f, 0.33f, 0.33f, 0.33f, 0.32f, 0.32f, 
0.32f, 0.32f, 0.32f, 0.32f, 0.31f, 0.31f, 0.31f, 0.31f, 0.31f, 0.31f, 
0.30f, 0.30f, 0.30f, 0.30f, 0.30f, 0.30f, 0.29f, 0.29f, 0.29f, 0.29f, 
0.29f, 0.29f, 0.29f, 0.28f, 0.28f, 0.28f, 0.28f, 0.28f, 0.28f, 0.27f, 
0.27f, 0.27f, 0.27f, 0.27f, 0.27f, 0.27f, 0.26f, 0.26f, 0.26f, 0.26f, 
0.26f, 0.26f, 0.26f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 
0.24f, 0.24f, 0.24f, 0.24f, 0.24f, 0.24f, 0.24f, 0.23f, 0.23f, 0.23f, 
0.23f, 0.23f, 0.23f, 0.23f, 0.22f, 0.22f, 0.22f, 0.22f, 0.22f, 0.22f, 
0.22f, 0.21f, 0.21f, 0.21f, 0.21f, 0.21f, 0.21f, 0.21f, 0.21f, 0.20f, 
0.20f, 0.20f, 0.20f, 0.20f, 0.20f, 0.20f, 0.19f, 0.19f, 0.19f, 0.19f, 
0.19f, 0.19f, 0.19f, 0.18f, 0.18f, 0.18f, 0.18f, 0.18f, 0.18f, 0.18f, 
0.18f, 0.17f, 0.17f, 0.17f, 0.17f, 0.17f, 0.17f, 0.17f, 0.16f, 0.16f, 
0.16f, 0.16f, 0.16f, 0.16f, 0.16f, 0.16f, 0.15f, 0.15f, 0.15f, 0.15f, 
0.15f, 0.15f, 0.15f, 0.14f, 0.14f, 0.14f, 0.14f, 0.14f, 0.14f, 0.14f, 
0.13f, 0.13f, 0.13f, 0.13f, 0.13f, 0.13f, 0.12f, 0.12f, 0.12f, 0.12f, 
0.12f, 0.12f, 0.11f, 0.11f, 0.11f, 0.11f, 0.11f, 0.11f, 0.10f, 0.10f, 
0.10f, 0.10f, 0.10f, 0.09f, 0.09f, 0.09f, 0.09f, 0.08f, 0.08f, 0.08f, 
0.08f, 0.07f, 0.07f, 0.07f, 0.06f, 0.06f, 0.05f, 0.05f, 0.04f, 0.00f, 
};
/************************************************************************/
/* Internal Functions                                                   */
/************************************************************************/
static void applyIIRFilter( BWIIR_S* pIIR, float* pBuf, int nSamples ){
	double* acoeffs = pIIR->a;
	double* bcoeffs = pIIR->b;
	double* xPrev = pIIR->xp;
	double* yPrev = pIIR->yp;
	int i;


	for( i=0; i<nSamples; i++  ){
		double aux = (double)pBuf[i];
		double sum = aux * acoeffs[0];
		int j = 0;
		for( j=0; j<8; j++ ){
			sum += xPrev[j] * acoeffs[j+1];
			sum -= yPrev[j] * bcoeffs[j+1];
		}
		pBuf[i] = (float)sum;

		xPrev[7] = xPrev[6];
		xPrev[6] = xPrev[5];
		xPrev[5] = xPrev[4];
		xPrev[4] = xPrev[3];
		xPrev[3] = xPrev[2];
		xPrev[2] = xPrev[1];
		xPrev[1] = xPrev[0];
		xPrev[0] = aux;

		yPrev[7] = yPrev[6];
		yPrev[6] = yPrev[5];
		yPrev[5] = yPrev[4];
		yPrev[4] = yPrev[3];
		yPrev[3] = yPrev[2];
		yPrev[2] = yPrev[1];
		yPrev[1] = yPrev[0];
		yPrev[0] = sum;

	}


}






static void rfft_fix( Int32* x,Int32 n,Int32 m ){
	Int32 j, i, k, is, id;
	Int32 i0, i1, i2, i3, i4, i5, i6, i7, i8;
	Int32 n2, n4, n8;
	Int32 xt, a0;
	Int64 t1, t2, t3, t4, t5, t6;
//	Int32 t1, t2, t3, t4, t5, t6;
	Int32 cc1, ss1, cc3, ss3,tabCount;
	Int32 *r0;
	Int32* tab = (Int32* )sinTabRP;
	/* Digit reverse counter */
//	float e,a,e3,a3;
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
					t1 = ((x[i3] + x[i4]) * CC_PITCH_QN_FFT) >> CC_PITCH_SHIFT;
					t2 = ((x[i3] - x[i4]) * CC_PITCH_QN_FFT) >> CC_PITCH_SHIFT;
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
//			a = j * e;
//			a3 = 3 * a;
//			cc1 = (Int32)(cos (a)* (1<<CC_PITCH_SHIFT));
//			ss1 = (Int32)(sin (a)* (1<<CC_PITCH_SHIFT));
//			cc3 = (Int32)(cos (a3)* (1<<CC_PITCH_SHIFT));
//			ss3 = (Int32)(sin (a3)* (1<<CC_PITCH_SHIFT));
			cc1 = *tab++;
			ss1 = *tab++;
			cc3 = *tab++;
			ss3 = *tab++;
			is = 0;
			id = n2 << 1;
/*			{
				FILE* fp = fopen( "sin.tab","at" );
				fprintf( fp,"%d,\t%d,\t%d,\t%d,\n",cc1,ss1,cc3,ss3 );
				fclose( fp );
			}
*/
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
					t1 = (Int64)x[i3] * (Int64)cc1 + (Int64)x[i7] * (Int64)ss1;
					t2 = (Int64)x[i7] * (Int64)cc1 - (Int64)x[i3] * (Int64)ss1;
					t3 = (Int64)x[i4] * (Int64)cc3 + (Int64)x[i8] * (Int64)ss3;
					t4 = (Int64)x[i8] * (Int64)cc3 - (Int64)x[i4] * (Int64)ss3;
					t1 >>= CC_PITCH_SHIFT;
					t2 >>= CC_PITCH_SHIFT;
					t3 >>= CC_PITCH_SHIFT;
					t4 >>= CC_PITCH_SHIFT;

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

//	for (i = 0; i < n; i++) {
//		x[i] >>= 6;
//	}


}


static void ufft_fix( Int64* x,Int32 n,Int32 m ){
	Int32 j, i, k, is, id;
	Int32 i0, i1, i2, i3, i4, i5, i6, i7, i8;
	Int32 n2, n4, n8;
	Int64 xt, a0;
	Int64 t1, t2, t3, t4, t5, t6;
//	Int32 t1, t2, t3, t4, t5, t6;
	Int32 cc1, ss1, cc3, ss3,tabCount;
	Int64 *r0;
	Int32* tab = (Int32* )sinTabUP;
	/* Digit reverse counter */
//	float e,a,e3,a3;
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
					t1 = ((x[i3] + x[i4]) * CC_PITCH_QN_FFT_U) >> CC_PITCH_SHIFT_U;
					t2 = ((x[i3] - x[i4]) * CC_PITCH_QN_FFT_U) >> CC_PITCH_SHIFT_U;
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
//			a = j * e;
//			a3 = 3 * a;
//			cc1 = (Int32)(cos (a)* (1<<CC_PITCH_SHIFT_U));
//			ss1 = (Int32)(sin (a)* (1<<CC_PITCH_SHIFT_U));
//			cc3 = (Int32)(cos (a3)* (1<<CC_PITCH_SHIFT_U));
//			ss3 = (Int32)(sin (a3)* (1<<CC_PITCH_SHIFT_U));
			cc1 = *tab++;
			ss1 = *tab++;
			cc3 = *tab++;
			ss3 = *tab++;
			is = 0;
			id = n2 << 1;
/*			{
				FILE* fp = fopen( "sin.tab","at" );
				fprintf( fp,"%d,\t%d,\t%d,\t%d,\n",cc1,ss1,cc3,ss3 );
				fclose( fp );
			}
*/
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
					t1 = (Int64)x[i3] * (Int64)cc1 + (Int64)x[i7] * (Int64)ss1;
					t2 = (Int64)x[i7] * (Int64)cc1 - (Int64)x[i3] * (Int64)ss1;
					t3 = (Int64)x[i4] * (Int64)cc3 + (Int64)x[i8] * (Int64)ss3;
					t4 = (Int64)x[i8] * (Int64)cc3 - (Int64)x[i4] * (Int64)ss3;
					t1 >>= CC_PITCH_SHIFT_U;
					t2 >>= CC_PITCH_SHIFT_U;
					t3 >>= CC_PITCH_SHIFT_U;
					t4 >>= CC_PITCH_SHIFT_U;
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



}

//Rm is Q13,Hz is Q6,remember this
static int getCand( void* self, Int32* pData, Int32 time, int checkDing ){
	int k = 0;
	Int32 iData[MAX_WIN_SIZE];
	Int32 iRm[MAX_WIN_SIZE];
	Int32 processBuffer1[1024];
	Int64 processBuffer2[1024];
	Int32 mean = 0;
	int	bestVoiceIdx = 0;
	PITCH* pitch = (PITCH* )self;
	FRMDATA* frm = &pitch->pFrmData[POS(time)];
    int t = frm->prevIdx;
	UInt32 bEn = 0;
    Int32 fftQ = pitch->nFFT;
    Int32 fftS = pitch->sFFT;
	memset(frm, 0, sizeof(FRMDATA));
	memset(iData, 0, sizeof(Int32)*MAX_WIN_SIZE);
	memset(processBuffer1,0,sizeof(Int32)*1024);
	frm->time = time;

	frm->nCand = 1;	/** the first candidate will always be the un-voiced speech */
	frm->nRm[0] = 0;
	frm->nHz[0] = CC_HZ_QN;//shoud be Q4 value,1*16
    frm->prevIdx = t;

	memcpy( iData,pData,sizeof(Int32)*pitch->nFrmlen );

	/** pre-emphasis */
	for(k=0; k<pitch->nFrmlen; k++){
		mean += iData[k];
	}


	mean /= pitch->nFrmlen;
	
	/** remove bias and get energy*/
	for(k=0; k<pitch->nFrmlen; k++){
		iData[k] -= mean;
		iRm[k] = 0;
		frm->iEn += iData[k] * iData[k];
	}

	if( checkDing  ==1)
		pitch->ding.fuE = (frm->iEn>>10);

	frm->iEn /= pitch->nFrmlen;
	/** low energy */
	if( frm->iEn < 1 ){
		frm->iEn = 1;
		return frm->iEn;
	}


	/** fft-->pd->ifft->acf */
	memcpy( processBuffer1,iData,sizeof(Int32)*pitch->nFrmlen );

    rfft_fix( processBuffer1,fftS*2,fftQ );
    
	bEn = processBuffer1[0]*processBuffer1[0];
	for( k=32; k < 128/*128*/; k++ ){
		bEn += ((Int64)processBuffer1[k]*(Int64)processBuffer1[k] + (Int64)processBuffer1[fftS*2-k]*(Int64)processBuffer1[fftS*2-k])>>(fftQ-1);
	}
	bEn /= pitch->nFrmlen;
	frm->iEn = bEn;
	if( frm->iEn<1 )	frm->iEn = 1;

	processBuffer2[0] = ((Int64)processBuffer1[0] * (Int64)processBuffer1[0])>>(fftQ-1);
	for( k=1;k<fftS;k++ ){
		processBuffer2[k] = ((Int64)processBuffer1[k] * (Int64)processBuffer1[k] + (Int64)processBuffer1[fftS*2-k] * (Int64)processBuffer1[fftS*2-k])>>(fftQ-1);
	}
	processBuffer2[fftS] = ((Int64)processBuffer1[fftS] * (Int64)processBuffer1[fftS])>>(fftQ-1);
	for( k=fftS+1;k<fftS*2;k++ ){
		processBuffer2[k] = processBuffer2[fftS*2-k];
	}

	if(checkDing==1){
		pitch->ding.hiE = 0;
		for( k=pitch->ding.hiTonBin[0];k<=pitch->ding.hiTonBin[1];k++ )
			pitch->ding.hiE += (processBuffer2[k]>>10);
	}

    ufft_fix( processBuffer2,fftS*2,fftQ);


    /** get RM */
	for(k=pitch->nMinLag; k < pitch->nMaxLag; k++){
		/** ACF normalized by R0 and ACF of hanning window, according to praat pitch */
		processBuffer2[k] *= pitch->nRmWin[k];
		iRm[k] = processBuffer2[k] / processBuffer2[0];
	}

	frm->nRm[0] = abs( iRm[k=pitch->nMinLag] / 20 );
	/** searching from lower Lag, which favors higher frequency */


	for( k = pitch->nMinLag; k <= pitch->nMaxLag; k++ ){
		int st;
		int cnt = 0;
		if( iRm[k] < PITCH_THRESHOLD ){
			iRm[k] = 0;
			continue;
		}
		st = k;
		for( ;k <= pitch->nMaxLag && iRm[k]>0; k++ );
		cnt = k - st;
		/** at least 3 points to determine a local maxima */
		if( cnt >= 3 ){
			int best = st;
			int start = st;
			int end = k;
			for( ; st < k; st++ ){
				if( iRm[best] < iRm[st] )
					best = st;
			}
			if( best == start || best == end-1 )
				continue;
			if( frm->nCand < MAX_PITCH_CAND ){
				/** parabolic interpolation,added 2010.11.17*/
/*				int prev = best - 1;
				int next = best + 1;
				Int32 rmPrev = iRm[prev];
				Int32 rmNext = iRm[next];
				Int32 t1 = 2*iRm[best] - rmPrev - rmNext;
				Int32 t2 = rmNext - rmPrev;*/
				int prev = best - 1;
				int next = best + 1;
				Int32 rmPrev = iRm[prev];
				Int32 rmNext = iRm[next];
				float temp = (rmNext - rmPrev)*0.5 / ( 2*iRm[best] - rmPrev - rmNext );

				frm->nRm[frm->nCand] = iRm[best];//(rmNext-rmPrev) * temp / 8.0f;
				frm->nHz[frm->nCand] = pitch->nKHz * CC_HZ_QN* 1000 / (best+temp); 
				frm->nCand++;
			}else{
				/** might not just discard the rest candidates */
				break;
			}
			
		}
	}

#ifdef PITCH_DEBUG
	if( frm->nCand>pitch->nMaxPitchCand )
		pitch->nMaxPitchCand = frm->nCand;
#endif
	return frm->iEn;

}


static int runDP( void* self,int t ){
	PITCH* pitch = (PITCH* )self;
	int GlobalOptima = 1;
	int prevBest = -1;
	int k=0;
	if( t==0 ){
		GlobalOptima = 0;
		for(;k < pitch->pFrmData[0].nCand; k++){
			pitch->pFrmData[0].prevCand[k] = -1;
			pitch->pFrmData[0].score[k] = pitch->pFrmData[0].nRm[k];
		}
	}else{

		int bestIdx;
		Int32 bestScore, score;
        FRMDATA* currFRM = &pitch->pFrmData[POS(t)];
        FRMDATA* prevFRM = &pitch->pFrmData[currFRM->prevIdx];

		for( k=0; k<currFRM->nCand; k++ ){
			Int32 j=0;
			Int32 localCost = currFRM->nRm[k];//Q12
			bestScore = -1000000000;
			bestIdx = 0;
            if( prevFRM->nCand==0 ){
                printf ( "warning t = %d|%d,prevIdx = %d\n",t,POS(t),currFRM->prevIdx );
            }
			for( j=0; j<prevFRM->nCand; j++ ){
				score = prevFRM->score[j];// + localCost;
				/** voice connection penalty */
				if( k>0 && j>0 ){
					float det = currFRM->nHz[k] * 1.0 / prevFRM->nHz[j];
					det = (float) (0.35 * exp( 3.0 * fabs(det-1) ));
//					score -= (float) (0.35 * exp( 3.0 * fabs(det-1) ));
//					Int64 det = 13 * ( currFRM->nHz[k]-prevFRM->nHz[j] ) / (3 * prevFRM->nHz[j]) +5;
//					det = 90 * ( 1<<det );
					if( det>100000 )
						score = -1000000000;
					else
						score -= (Int32)(det*(1<<13));//Q13,0.35
				}
				/** voice-2-unvoice or unvoice-2-voice transition panalty */
				else if( (k>0 && j==0) || (k==0 && j>0) ){
					score -= 6554;//Q13,0.8f;
				}
				/** unvoice transition, no penalty */
				else{
				}
				if( bestScore<score ){
					bestScore = score;
					bestIdx = j;
				}

			}
			if( prevBest==-1 )
				prevBest = bestIdx;
			else if( prevBest != bestIdx )
				GlobalOptima = 0;
			bestScore += localCost;
			currFRM->prevCand[k] = bestIdx;
			currFRM->score[k] = bestScore;
//			printf( "[%03d][%d]=>%d\n",t,k,bestScore );
			prevBest = bestIdx;
		}
	}
	return GlobalOptima;
}


static int partialTrace( void* self,int t , feature * features, int * pNumber){
	PITCH* pitch = (PITCH* )self;
	Int32 k = t-1;
	Int32 curBest = pitch->pFrmData[POS(t)].prevCand[0];
	Int32 traceIn = 0;
	Int32* traceBuffer = pitch->traceBuffer;
    FRMDATA* currFRM = &pitch->pFrmData[POS(k)];
	int i = 0;
	FILE* fout = NULL;
	while( curBest>-1 ){
		traceBuffer[traceIn*(PITCH_DIM+1)+PITCH_DIM-1] = currFRM->nRm[curBest];
		currFRM->nHz[0] = currFRM->nHz[curBest];
		traceBuffer[traceIn*(PITCH_DIM+1)] = currFRM->nHz[0];//(float)log(currFRM->nHz[0]);	
		traceBuffer[traceIn*(PITCH_DIM+1)+PITCH_DIM] = (Int32)(20.0 * log((double)currFRM->iEn));
//		pushData( pitchSlot,traceBuffer,k );
		curBest = currFRM->prevCand[curBest];
        currFRM = &pitch->pFrmData[currFRM->prevIdx];
		k--;

		traceIn++;
	}
	
	for( k=traceIn-1,i=0;k>=0;k--,i++ ){
		features[*pNumber+i].Vo = traceBuffer[k*(PITCH_DIM+1)+PITCH_DIM-1];
		features[*pNumber+i].En = traceBuffer[k*(PITCH_DIM+1)+PITCH_DIM];
        features[*pNumber+k].pitch = traceBuffer[k*(PITCH_DIM+1)];
//		printf( "%d\n",traceBuffer[k*(PITCH_DIM+1)+PITCH_DIM] );
	}
	*pNumber += traceIn;
    currFRM = &pitch->pFrmData[POS(t)];
	for( k=0; k<currFRM->nCand; k++ )
		currFRM->prevCand[k] = -1;
	return traceIn;
}

static int tracePitch( void* self,int nFrm , feature* features, int *pNumber){
	int bestIdx=-1;
	int k;
	Int32 bestScore = -1;
	PITCH* pitch = (PITCH* )self;
	int traceIn = 0;
	Int32* traceBuffer = pitch->traceBuffer;
    FRMDATA* currFRM = &pitch->pFrmData[POS(nFrm-1)];
	FILE* fpitch  =NULL;
	for( k=0; k<currFRM->nCand; k++ ){
		if( bestScore < currFRM->score[k] ){
			bestScore = currFRM->score[k];
			bestIdx = k;
		}
	}
	
	k = nFrm-1;
	while( bestIdx>-1 ){
		traceBuffer[traceIn*(PITCH_DIM+1)+PITCH_DIM-1] = currFRM->nRm[bestIdx];
		currFRM->nHz[0] = currFRM->nHz[bestIdx];
		traceBuffer[traceIn*(PITCH_DIM+1)] = currFRM->nHz[0];//logT_32(currFRM->nHz[0]);	
		traceBuffer[traceIn*(PITCH_DIM+1)+PITCH_DIM] = (Int32)log((double)currFRM->iEn);


		bestIdx = currFRM->prevCand[bestIdx];
        currFRM = &pitch->pFrmData[currFRM->prevIdx];
		k--;
		traceIn++;
	}
	
//	fpitch = fopen("feature.txt", "a");
	*pNumber = traceIn;
	for( k=traceIn-1;k>=0;k-- ){
		features[k].Vo = traceBuffer[traceIn*(PITCH_DIM+1)+PITCH_DIM-1];
		features[k].En = traceBuffer[traceIn*(PITCH_DIM+1)+PITCH_DIM];
        features[k].pitch = traceBuffer[traceIn*(PITCH_DIM+1)];
//		fprintf(fpitch, "%d\n", traceBuffer[traceIn*(PITCH_DIM+1)+PITCH_DIM-1]);
	}
//	fclose(fpitch);

	return traceIn;
}

PITCH* initPITCH_USC( int kHZ,int nFrmMs,int nSftMs,float fmaxpitch,float fminpitch){
	int k = 0;
	int j = 0;
	PITCH* pitch = (PITCH* )malloc(sizeof(PITCH));
	if( NULL == pitch ){
		return NULL;
	}

	pitch->pIIR = (BWIIR_S* )malloc( sizeof(BWIIR_S) );
	if( NULL == pitch->pIIR ){
		free(pitch);
		return NULL;
	}

	if( kHZ==8 ){
		memcpy( pitch->pIIR->a,AC_8K,sizeof(double)*9 );
		memcpy( pitch->pIIR->b,BC_8K,sizeof(double)*9 );
		pitch->nRmWin = (Int32* )rmWinTab_8K;
		pitch->nFFT = 9;
		pitch->sFFT = 256;

	}else{//kHZ==16
		memcpy( pitch->pIIR->a,AC_16K,sizeof(double)*9 );
		memcpy( pitch->pIIR->b,BC_16K,sizeof(double)*9 );
		pitch->nRmWin = (Int32* )rmWinTab_16K;
		pitch->nFFT = 10;
		pitch->sFFT = 512;
		
	}
	memset( pitch->pIIR->xp,0,sizeof(double)*8 );
	memset( pitch->pIIR->yp,0,sizeof(double)*8 );
	pitch->pFrmData = (FRMDATA* )malloc(sizeof(FRMDATA) * CIRCULAR_BUF_LEN );
	if( NULL == pitch->pFrmData ){
		delPITCH_USC(pitch);
		return NULL;
	}

    for( j=1;j<CIRCULAR_BUF_LEN;j++ ){
        pitch->pFrmData[j].prevIdx = j-1;
    }
    pitch->pFrmData[0].prevIdx = CIRCULAR_BUF_LEN-1;

	// only for ding detection
	pitch->ding.hiTonBin[0] = PATTERN_PITCH_VALUE * pitch->sFFT / (kHZ*1000/2) - 5;
	pitch->ding.hiTonBin[1] = PATTERN_PITCH_VALUE * pitch->sFFT / (kHZ*1000/2) + 5;
	pitch->ding.hiE = 0;
	pitch->ding.fuE = 0;	

	pitch->nKHz = kHZ;

	pitch->nFrmlen = kHZ * nFrmMs;
	pitch->nFrmSft = kHZ * nSftMs;
	pitch->nMaxLag = (Int32)(kHZ * fmaxpitch * 1000 + 0.5);
	pitch->nMinLag = (Int32)(kHZ * fminpitch * 1000 + 0.5);
	pitch->frameBuffer = (Int32* )calloc( pitch->nFrmlen,sizeof(Int32) );
	if( pitch->frameBuffer==NULL ){
		delPITCH_USC( pitch );
		return NULL;
	}

	pitch->traceBuffer = (Int32* )calloc( 6000,sizeof(Int32)*(PITCH_DIM+1) );
	if( pitch->traceBuffer==NULL ){
		delPITCH_USC( pitch );
		return NULL;
	}


	pitch->firsTime = 1;
	pitch->frameCounter = 0;
    pitch->frameDP = 0;

/*	for( k=0;k<pitch->nMaxLag;k++ ){
		pitch->nRmWin[k] = rmWinTab[k];
	}*/
#ifdef PITCH_DEBUG
	pitch->nMaxPitchCand = 1;
#endif
	return pitch;	
}

void delPITCH_USC( void* handle ){
	PITCH* engine = (PITCH* )handle;
	if( engine ){
		if( engine->pIIR != NULL )
			free( engine->pIIR );
	
        if( engine->pFrmData != NULL )
			free( engine->pFrmData);
		if( engine->frameBuffer )
			free( engine->frameBuffer );
		if( engine->traceBuffer )
			free( engine->traceBuffer );
		free( engine );

	}
}

int push2PITCH_USC( void* handle,Int16* pWaveBuffer,Int32 nSamples,Int32 Flush , feature* features, int* pnumber,int* ave){
	int isCheckDing = 0;
	dingEn* pdingen = NULL;
	int EnNumber = 0;

	return push2PITCH_full(handle,pWaveBuffer,nSamples,Flush,features,pnumber,ave,pdingen,&EnNumber,isCheckDing);

}
int push2PITCH_full( void* handle,short* pWaveBuffer,int nSamples,int Flush , feature * features, int* pNumber,int* ave,dingEn* pdingen,int* pEnNumber,int isCheckDing){
	int nRet = 0;
	float en = 0;
	int count = 0;
	float* start = NULL;
	PITCH* engine = (PITCH* )handle;
	Int32* buf = engine->frameBuffer;
	float bufFloat[480];
	int nFrmLen,nFrmSft;
	int samplesIn = nSamples;
	int frameCounter = engine->frameCounter;
    int frameDP = engine->frameDP;
	int i;
	nFrmLen = engine->nFrmlen;
	nFrmSft = engine->nFrmSft;
	start = bufFloat+(nFrmLen-nFrmSft);
	*pNumber = 0;

	if( engine->firsTime==1 ){
		for( i=0;i<nFrmLen-nFrmSft;i++ )
			bufFloat[i] = (float)pWaveBuffer[i];
		applyIIRFilter( engine->pIIR,bufFloat,nFrmLen-nFrmSft );
			
		for( i=0;i<nFrmLen-nFrmSft;i++ )
			buf[i] = (Int32)bufFloat[i];
		engine->firsTime = 0;
		samplesIn -= (nFrmLen-nFrmSft);
		pWaveBuffer += (nFrmLen-nFrmSft);
	}

	while( samplesIn>0 ){
		int nTrace,frmEn;
		for( i=0;i<nFrmSft;i++ )
			bufFloat[i+nFrmLen-nFrmSft] = (float)pWaveBuffer[i];

		applyIIRFilter( engine->pIIR,start,nFrmSft );
		for( i=0;i<nFrmSft;i++ )
			buf[i+nFrmLen-nFrmSft] = (Int32)start[i];

		frmEn = getCand( engine,buf,frameCounter ,isCheckDing);
		
		if(isCheckDing ==1 && pdingen != NULL){
		pdingen[frameCounter].high= engine->ding.hiE;
		pdingen[frameCounter].full = engine->ding.fuE;
		*pEnNumber = frameCounter;
		}


		nTrace = runDP( engine,frameCounter );
		if( nTrace==1 || frameDP > 40 ){
			nRet = 1;
            frameDP = 0;
			partialTrace( engine,frameCounter, features, pNumber);
		}
		samplesIn -= nFrmSft;
		pWaveBuffer += nFrmSft;

		for( i=0;i<nFrmLen-nFrmSft;i++ )
			buf[i] = buf[i+nFrmSft];
		frameCounter++;
        frameDP ++;
		en += frmEn;
		count ++;
	}

	if( Flush==1 && frameCounter>0 ) {
		nRet = 1;
		tracePitch( engine,frameCounter , features, pNumber);
	}


	en = 10*log10(en / count) - 60;
	if(en<-50){
		*ave = 0;
	}else if(en>0){
		*ave = 1;
	}else{
		*ave = (int)(100 * meTab[(int)(-en*399/50)]);
	}
	engine->frameCounter = frameCounter;
	engine->frameDP = frameDP;
//    printf("return %d\n",nRet);
	return nRet;
	
}

int FindVaildPitchSeg(int* pitchbuf,int len,int* begin,int* end){

	 int begincount = 0; int endcount = 0;
	 int canbegin = 0; int canend = -1;
	 int i=0;
 	 *begin = 0; *end = -1;
	 for( i=0; i<len; i++){
		 if(*begin == 0){//find  begin
			 if(pitchbuf[i] > MIN_PITCH_DINGDONG1 && pitchbuf[i] < MAX_PITCH_DINGDONG1){// pitch range
				 if(canbegin == 0){
					 canbegin = i;
				 }
				 begincount++;
				 if(begincount > 5){
					 *begin = canbegin;
				 }
			 }
			 else{
				 canbegin = 0;
				 begincount == 0;
			 }
		 }
		 else if(*end == -1){//find end
			 if(pitchbuf[i] < MIN_PITCH_DINGDONG1 || pitchbuf[i] > MAX_PITCH_DINGDONG1){// pitch range
				 if(canend == -1){
					 canend = i;
				 }
				 endcount++;
				 if(endcount > 5){
					 *end = canend;
					 break;
				 }
			 }
			 else{
				 canend = -1;
				 endcount = 0;
			 }
		 }
	 }

	 if(*begin > 0 && *end == -1)
		 *end = len-1;

	return *end-*begin;
}


float CalcEnergyRatio(Int64* hiEnergy,Int64* fuEnergy, int begin, int end){
	Int64 high = 0; Int64 full = 0;
	int i=begin;
	for(i=begin;i<=end;i++){
		high += hiEnergy[i];
		full += fuEnergy[i];
	}
	if(full == 0)
		return 100000000000000.0f;
	else
		return 1.0f*high/full;
}

void resetPITCH_USC( void* handle ){
	PITCH* engine = (PITCH* )handle;
	if( engine ){
		memset( engine->pIIR->xp,0,sizeof(double)*8 );
		memset( engine->pIIR->yp,0,sizeof(double)*8 );
		
		engine->frameCounter = 0;
        engine->frameDP = 0;
		engine->firsTime = 1;
	}
}


VAD * initVAD_USC( int kHZ ) {
	VAD * handler = (VAD*) malloc( sizeof(VAD) );
	int maxFSil = 300;
	int maxBSil = HANGOVER;
	if( handler==NULL )
		return NULL;
/*	if( maxBSil < 100 )
		maxBSil = 0;
	else if( maxBSil > 300 )
		maxBSil = 200;
	else
		maxBSil -= 100;*/
	handler->nbSpeechFrame = 0;
	handler->nbSilFrame	= 0;
	handler->meanFt        = 0;
	handler->flagVAD       = WAITING;
	handler->hangOver      = 0;
	handler->frameIndex    = 0;
	handler->frameSkip    = 0;

	handler->lambdaLTF     = 0;
	handler->front         = 0;
    handler->imp_front = 0;
	handler->end           = 0; 
	//handler->tmpFront      = 0;
	//handler->tmpEnd        = 0; 
	handler->meanEn        = 0;
	handler->speechEn = 0;
	handler->maxFSil = maxFSil;
	handler->maxBSil = maxBSil;
	handler->maxEn = -1.0f;
	handler->features = (feature*)calloc(FEATURE_NUMBER,sizeof(feature) );
	handler->engine = initPITCH_USC(kHZ,30, 10,MAX_PITCH_P,MIN_PITCH_P);
	if( handler->features==NULL || handler->engine==NULL )
		delVAD_USC( handler );
	return handler;
}

void setParam_USC( VAD* handler,int maxFSil,int maxBSil ){
	if( maxBSil < 20 )
		maxBSil = 20;
	else if( maxBSil > 300 )
		maxBSil = 300;
	handler->maxFSil = maxFSil;
	handler->maxBSil = maxBSil;
}

int push2VAD_USC(VAD* handler, short* pWaveBuffer, int nSamples,int* ave) {
	Int32 frameFt = 0;
	int i = 0;
	FILE* fen = NULL;
	Int32 energy = 0;
	Int32 eMeasure;
	Int32 rMeasure;
	int ret = push2PITCH_USC(handler->engine, pWaveBuffer,
			nSamples, 0, handler->features, &handler->number,ave); 


	if(0 == ret ) {
		return handler->flagVAD;
	}

	while(i < handler->number) {
		frameFt = handler->features[i].Vo;
		energy = handler->features[i].En;
		i ++;
        if( handler->maxFSil > 0 && handler->frameSkip > handler->maxFSil ){
            handler->flagVAD = MAX_SIL;
            break;
        }
		if( energy<1.0f && handler->flagVAD==WAITING ){
			handler->frameSkip ++;
			continue;
		}
		handler->frameIndex ++;
		if(handler->frameIndex < NB_FRAME_THRESHOLD_LTF)  {
			handler->lambdaLTF = 1.0 - 1.0/handler->frameIndex;
		}else {
			handler->lambdaLTF = LAMBDA_LTF;
		}
		
		if( (energy - handler->meanEn)< EN_THRESHOLD_UPD_LTE || handler->frameIndex < MIN_FRAME ) {
			if(energy < handler->meanEn || handler->frameIndex < MIN_FRAME) {
				handler->meanEn += (1-handler->lambdaLTF) * (energy - handler->meanEn);
			}else {
				handler->meanEn += (1-LAMBDA_LTF_HIGHER_E) * (energy - handler->meanEn);
			}

			if(handler->meanEn < FEATURE_FLOOR){
			   handler->meanEn = FEATURE_FLOOR;
			}
		}
//		printf( "[%04d]=%04d\t%.2f\t%04d\t%.2f\n",handler->frameIndex,energy,handler->meanEn,frameFt,handler->speechEn );

		if(handler->frameIndex < 10)
			continue;
		eMeasure =  energy - handler->meanEn;
		rMeasure = frameFt;// - handler->meanFt;

		if( handler->flagVAD==NONSPEECH || handler->flagVAD==MAX_SIL ){
			break;
		}else if( handler->flagVAD==WAITING ){
			if( rMeasure> THRESHOLD_VAD_LOW && eMeasure > EN_THRESHOLD_VAD ){
				handler->nbSpeechFrame++;
				if( handler->nbSpeechFrame == CN_FRAME_THRESHOLD ){
					handler->nbSilFrame = 0;
                    handler->imp_front = handler->frameIndex - CN_FRAME_THRESHOLD;
                    handler->front = handler->imp_front + (handler->frameSkip - 30);
//					handler->front = handler->frameIndex + handler->frameSkip - CN_FRAME_THRESHOLD;
//					handler->front -= 30;
					if(handler->front<handler->frameSkip)	handler->front = handler->frameSkip;
					handler->flagVAD = SPEECH;
				}
			}else{
				handler->nbSilFrame++;
				if( handler->maxFSil >0 && handler->nbSilFrame > handler->maxFSil ){
					handler->flagVAD = MAX_SIL;
					break;
				}
			}
		}else if( handler->flagVAD==SPEECH ){
			int elapse = handler->frameIndex - handler->imp_front;
			float measure;
			if( handler->maxEn < energy )
				handler->maxEn = energy;
			if( elapse < 100 ){
				handler->speechEn += (1.0f/elapse) * ( energy - handler->speechEn );
            }
			else
				handler->speechEn += 0.01f * ( energy - handler->speechEn );
			if( handler->speechEn + 100 < handler->maxEn ){
				handler->speechEn = handler->maxEn - 100;
			}
			measure = handler->speechEn - energy;
			if( measure > EN_THRESHOLD_LOW ){
				if (handler->nbSpeechFrame > MIN_SPEECH_FRAME_HANGOVER) {
					handler->hangOver = handler->maxBSil;//HANGOVER;
//					printf( "start to hangover %d frames\n",handler->nbSpeechFrame );
				}
				handler->nbSpeechFrame = 0;
				if( handler->hangOver != 0 ){
					handler->hangOver --;
				}else{
					handler->flagVAD = NONSPEECH;
					handler->end = handler->frameIndex + handler->frameSkip;
//					handler->front += (handler->frameSkip - CN_FRAME_THRESHOLD);
				}
			}else if( rMeasure > THRESHOLD_VAD_HGH && eMeasure > EN_THRESHOLD_VAD ){
				handler->nbSpeechFrame++;
			}
		}else/*HOLDING*/{

		}
	}

//	printf( "[%04d]=%d\n",handler->frameIndex,handler->flagVAD );
	return handler->flagVAD;
}

void delVAD_USC(VAD* handler) {
	if(handler != NULL)  {
		if(handler->features != NULL) 
			free(handler->features);
		
		if(handler->engine != NULL) 
			delPITCH_USC(handler->engine);

		free(handler);
	}
}

void resetVAD_USC( VAD* handler ){
	if( handler ){
		if(handler->engine)
			resetPITCH_USC( handler->engine );
		handler->nbSpeechFrame = 0;
		handler->nbSilFrame	= 0;

		handler->meanFt        = 0;
		handler->flagVAD       = WAITING;
		handler->hangOver      = 0;
		handler->frameIndex    = 0;
		handler->frameSkip    = 0;

		handler->lambdaLTF     = 0;
		handler->imp_front         = 0;
		handler->front         = 0;
		handler->end           = 0; 
		handler->meanEn        = 0;
		handler->speechEn = 0;
		handler->maxEn = -1.0f;
	}
}
