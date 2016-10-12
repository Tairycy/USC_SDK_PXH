// -------------------------------------------------
// Module	: Executable file of quantile based noise masking
// Author	: JiaLei
// Date	: 04/27/2007
// $Id		: CNoiseMask.cpp
//
// -------------------------------------------------


#include	"CNoiseMask.h"
#include	<memory.h>
#include	<math.h>
#include	<assert.h>
#include	<time.h>
//-------------------------------------------------
//  class CHistograme
//-------------------------------------------------
//-------------------------
//  CHistograme::CHistograme()
//-------------------------
CHistograme::CHistograme ()
{
//	pQuantile		= NULL;
//	pTmpQuantile	= NULL;
//	pTarDiff		= NULL;
}

//-------------------------
//  CHistograme::~CHistograme()
//-------------------------
CHistograme::~CHistograme()
{
}


//-------------------------
//  CHistograme::AllcateMem()
//-------------------------
void CHistograme::AllcateMem (int nInHisLen, int nInQuantileNum)
{
	nHisLen		= nInHisLen;
	nQuantileNum	= nInQuantileNum;
	//

	m_HistoG.resize( nInHisLen );
	m_HistoI.resize( nInHisLen );
	m_HistoT.resize( nInHisLen );
	m_Quant.resize( nInQuantileNum+1 );
	return;
}


//-------------------------
//  CHistograme::Initialization()
//-------------------------
int CHistograme::Initialization ()
{
	int ii;
	//
	nCurTimeMark = 0;
	//

	m_HistoG.assign( 0.0f );
	for (ii=0; ii<nHisLen; ii++) {
//		m_HistoG[ii]		= 0.0f;
		m_HistoI[ii]		= ii;
		m_HistoT[ii]		= ii;
	}
	//
	m_Quant.assign( 0.0f );
// 	for (ii=0; ii<=nQuantileNum; ii++) {
// 		pQuantile[ii] = (float) 0.0;	
// 	}
	return 0;
}

//-------------------------
//  CHistograme::SetDefaultHisgram()
//-------------------------
int CHistograme::SetDefaultHisgram()
{
	int ii;
	float fMinValue = m_HistoG[ m_HistoT[0] ];
	for (ii=0; ii<nHisLen; ii++) {
		m_HistoG[ii] = float(fMinValue+ ii*0.001);
	}
	return 0;
}

//-------------------------
//  CHistograme::GetQuantile()
//-------------------------
int CHistograme::GetQuantile ()
{
	float nTmp= nHisLen*1.0f/nQuantileNum;
	m_Quant[0] = m_HistoG[ m_HistoT[0] ];
	m_Quant[nQuantileNum] = m_HistoG[ m_HistoT[nHisLen-1] ];
	for (int ii=1; ii<nQuantileNum; ii++) {
		int idx = (int)(ii*nTmp-1);
		m_Quant[ii] = m_HistoG[ m_HistoT[idx] ];
	}
	return 0;
}

//-------------------------
//  CHistograme::GetQuantile()
//-------------------------
int CHistograme::PrintQuantile ()
{
	printf("\n");
	for (int ii=0; ii<=nQuantileNum; ii++) {
		if (ii != nQuantileNum)
			printf("%f(%f)\t", m_Quant[ii], (m_Quant[nQuantileNum]-m_Quant[ii]) );
		else
			printf("%f(%f)\t", m_Quant[ii], 0. );
	}
	printf("\n");
	return 0;
}

//-------------------------
//  CHistograme::GetQuantile()
//-------------------------
int CHistograme::PrintHisgram ()
{
	printf("\n");
	for (int ii=nHisLen-1; ii>=0; ii--) {
		printf("%f\t", m_HistoG[ m_HistoT[ii] ]);
	}
	printf("\n");
	return 0;
}


//-------------------------
//  CHistograme::GetInsertPos()
//-------------------------
int CHistograme::GetInsertPos( float fInValue )
{
	int nIdx, nTime;
	if (fInValue >= m_Quant[nQuantileNum])
		return nHisLen-1;
	if (fInValue <= m_Quant[0])
		return -1;
	for(nIdx=nQuantileNum; nIdx>=0; nIdx-- ) {
		if (m_Quant[nIdx] < fInValue)
			break;
	}


	float nTmp= nHisLen*1.0f/nQuantileNum;
	unsigned short nSt = (unsigned short)( (nIdx*nTmp-1 > 0)?  (nIdx*nTmp-1) : 0 );
	unsigned short nEd = (unsigned short)( (nIdx+1)*nTmp-1 );
	unsigned short nMid= ((nSt+nEd)>>1);
	float fMidValue;
	float st = m_HistoG[m_HistoT[nSt]];
	float ed = m_HistoG[m_HistoT[nEd]];
// 	if( fInValue>ed||fInValue<st ){
// 		printf( "\n Sort error in GetInsertPos\n");
// 		getchar();
// 		exit(1);
// 	}
	while(1) {

		nTime = m_HistoT[nMid];
		fMidValue = m_HistoG[ nTime ];
		if (fInValue == fMidValue || nEd <= (nSt+1) ) {
			//We assume the value to be inserted is larger than the current value in position "nMid"
			if (fInValue < fMidValue)
				return nMid-1;	
			else
				return nMid;
		}

		if (fInValue < fMidValue)
			nEd = nMid;
		else
			nSt = nMid;
		nMid= ((nSt+nEd)>>1);
	}
	return -100000000;
}

//-------------------------
//  CHistograme::InsertValue()
//-------------------------
int CHistograme::InsertToHisgram( float fInValue )
{
	int ii, nTime, nInsPos, nDelPos;
	//Get the insert and delete position
	nDelPos	= m_HistoI [nCurTimeMark];
	nInsPos = GetInsertPos (fInValue);
	
	//Move the position of "pHisSeqIdx" and "pHisSeqTime"
	if ( nDelPos == (nInsPos+1) ) {
	
		m_HistoG[ nCurTimeMark] = fInValue; 
		m_HistoI[ nCurTimeMark] = nInsPos+1;
		m_HistoT[ nInsPos+1]	= nCurTimeMark; 
	
	}
	if ((nInsPos+1) > nDelPos) {		
		for( ii=nDelPos+1; ii<=nInsPos; ii++ ){
			nTime = m_HistoT[ii];
			m_HistoI[nTime] = ii-1;
		}
//		memmove( &m_HistoT[nDelPos], &m_HistoT[nDelPos+1], (nInsPos-nDelPos)*sizeof(int) );
		if( nDelPos+1<nHisLen )
			m_HistoT.move( nDelPos,nDelPos+1,(nInsPos-nDelPos) );	
		//Insert into current histogram
		m_HistoG[ nCurTimeMark] = fInValue; 
		m_HistoI[ nCurTimeMark] = nInsPos;
		m_HistoT[ nInsPos]		 = nCurTimeMark; 

	}
	if ((nInsPos+1) < nDelPos) {
	
		for( ii=nDelPos-1; ii >= nInsPos+1; ii-- ) {
			nTime 	= m_HistoT[ii];
			m_HistoI[nTime] = ii+1;
		}				
		
		m_HistoT.move( nInsPos+2,nInsPos+1,(nDelPos-nInsPos-1) );		
		m_HistoG[ nCurTimeMark] = fInValue; 
		m_HistoI[ nCurTimeMark] = nInsPos+1;
		m_HistoT[ nInsPos+1 ]	 = nCurTimeMark;		
	}
	
	//error check
// 	for ( ii=0; ii<nHisLen; ii++)
// 	{
// 		if ( ii != m_HistoT[ m_HistoI[ii] ]  ) {
// 			printf( "\n Failure of pointer used for quick sorting in CHistograme::InsertToHisgram()!\n");
// 			exit(1);
// 		}
// 		for (int ii=nHisLen-1; ii>0; ii--)
// 			if ( m_HistoG[ m_HistoT[ii] ] - m_HistoG[ m_HistoT[ii-1]] < 0 ) {
// 				printf( "\n Error of quick sorting in CHistograme::InsertToHisgram()!\n");
// 				exit(1);
// 			}
// 	}
	//end error check
		
	//Set the time mark for deleting 
	nCurTimeMark++;
	if (nCurTimeMark == nHisLen)
		nCurTimeMark = 0;
	//
	GetQuantile ();
	return 0;
}

//-------------------------------------------------
//  class CNoiseMask
//-------------------------------------------------

//-------------------------
//  CNoiseMask::CNoiseMask()
//-------------------------
CNoiseMask::CNoiseMask (int nInWinLen, int nQuantNum, int nInMelBankNum)
{
	m_nWinLen 		= nInWinLen;
	m_nQuantiNum	= nQuantNum;
	m_MelBankNum	= nInMelBankNum;	
	m_pHistograme 	= NULL;
//	m_nBankFactor	= 1;
	m_frameCount = 0;

#ifdef __LOG_NOISE_MASK__
	fp = fopen( "mask1.log","wt" );
#endif
	return;
}

//-------------------------
//  CNoiseMask::~CNoiseMask()
//-------------------------
CNoiseMask::~CNoiseMask()
{
	if (m_pHistograme != NULL) {
		delete []m_pHistograme;
		m_pHistograme = NULL;
	}
#ifdef __LOG_NOISE_MASK__
	if( fp )
		fclose( fp );
#endif

}

//-------------------------
//  CNoiseMask::IniHistograme()
//-------------------------
void CNoiseMask::IniSpHistograme ()
{
	if (m_pHistograme != NULL) {
		delete []m_pHistograme;
	}
	//
	m_pHistograme = new CHistograme[m_MelBankNum];
	if (m_pHistograme == NULL) {
	//	printf( "\nCan't allocate memory in the constructor of CNoiseMask!\n");
		exit(1);
	}
	for (int ii=0; ii<m_MelBankNum; ii++) {
		m_pHistograme[ii].AllcateMem (m_nWinLen, m_nQuantiNum);
		m_pHistograme[ii].Initialization();
	}
	//
	m_FMMaxQuant.SetSize( m_MelBankNum, m_nQuantiNum+1 );
	m_FMMaxQuant.Assign(0.0f);

	return;
}



//-------------------------
//  CNoiseMask::ResetQuantile()
//-------------------------
void CNoiseMask::ResetQuantile()
{
	for (int ii=0; ii<m_MelBankNum; ii++) {
		m_pHistograme[ii].Initialization();
	}
	m_FMMaxQuant.Assign(0.0f);
	return;
}


void CNoiseMask::GetQuantMask(FVector & inVec,FMatrix& maskM )
{
	int dim;//, ii;
	if( m_MelBankNum!=inVec.nDim ){
		printf( "Bands=%d not coincident with Output Bands=%d",m_MelBankNum,inVec.nDim );
	}
	int nFFTDim = inVec.nDim;
//	float scale = 8.0f;	
	int nPinNum = nFFTDim/m_MelBankNum;	
	int nBandNum = m_MelBankNum;	

	// Set the max quant to be zero 
//	m_FMMaxQuant.Assign(0.0f);
	
	// Get the max quant for this utterance
			
	for (dim=0; dim<nBandNum; dim++) {
		m_pHistograme[dim].InsertToHisgram (inVec[dim]);
	}
	
	//  Get the maximum Quantile
	for (dim=0; dim<nBandNum; dim++) {
// 		float dis 	= m_pHistograme[dim].pQuantile[m_nQuantiNum] - m_pHistograme[dim].pQuantile[0];
// 		float disMax 	= m_FMMaxQuant(dim,m_nQuantiNum) - m_FMMaxQuant(dim,0);
// 		if (dis >= disMax) 
		float* pT = m_pHistograme[dim].m_Quant.getDataBuf();
		memcpy( m_FMMaxQuant[dim], pT, sizeof(float)*(m_nQuantiNum+1) );
	}
	
	m_FMMaxQuant += 1e-5;
	maskM = m_FMMaxQuant;
	
// 	for( dim=0;dim<ouVec.nDim;dim++ ){
// 		ouVec[dim] = 1e-5+m_FMMaxQuant(dim,2);
// 	}
// 
// 	for( dim = 0;dim<difVec.nDim;dim++ ){
// 		difVec[dim] = 1e-5+m_FMMaxQuant(0,dim);
// 	}

	//for debug
#ifdef __LOG_NOISE_MASK__
	
	m_frameCount++;
	float fSumDis = 0;

	
	FLOG( "Frame.No.%d\n",m_frameCount,fp );
	for (dim=0; dim<nBandNum; dim++) {
		fSumDis +=  m_FMMaxQuant[dim][m_nQuantiNum]- m_FMMaxQuant[dim][0];
		//
		FLOG("\n=================== MaxiMum Quantile =====================\n",fp);
		for (int ii=0; ii<=m_nQuantiNum; ii++) {
		//
			if (ii != m_nQuantiNum)
				FLOG("%5.3f(%5.3f)  ",  log(m_FMMaxQuant[dim][ii] ), log( m_FMMaxQuant[dim][m_nQuantiNum])- log(m_FMMaxQuant[dim][ii]),fp );
//				FLOG("%5.3f(%5.3f)  ",  log(m_FMMaxQuant[dim][ii] ), log( m_FMMaxQuant[dim][ii+1])- log(m_FMMaxQuant[dim][ii]),fp );

			else
				FLOG("%5.3f(%5.3f)  ",  log(m_FMMaxQuant[dim][ii]+1e-5), 0.,fp );

		}
	}
	FLOG("\n fAveDis = %f  \n", (fSumDis/(nBandNum-1)),fp );
#endif
	//end debug
	return;
}

//-------------------------
//  CNoiseMask::GetMaxQuant()
//-------------------------
// int CNoiseMask::GetMaxQuant (FMatrix & FeaMat)
// {
// 	int tt, dim, ii;
// 	float fValue;
// 	//
// 	int nFrmNum = FeaMat.GetRows();
// 	int nFFTDim = FeaMat.GetCols();
// 	int nPinNum = nFFTDim/m_MelBankNum;	
// 	int nBandNum = m_MelBankNum/m_nBankFactor;	
// 
// 	// Set the max quant to be zero 
// 	m_FMMaxQuant.Assign(0.0f);
// 	
// 	// Get the max quant for this utterance
// 	for (tt=0; tt<nFrmNum; tt++) {
// 			
// 		//for (dim=0; dim<nBandNum; dim++) {
// 		for (dim=1; dim<nBandNum; dim++) {
// 		
// 			// Average the FFT value in each bank
// 		    fValue = 0.;
// 		    for(ii=0; ii<nPinNum; ii++)
// 		        fValue += FeaMat[tt][dim*nPinNum + ii];
// 		    fValue /= nPinNum;
// 		    
// 		    // Take log operation
// 		    fValue = log(fValue);
// 		
// 			 // Insert to histogram
// 			 m_pHistograme[dim].InsertToHisgram (fValue);
// 		}
// 		
// 		//  Get the maximum Quantile
// 		for (dim=1; dim<nBandNum; dim++) {
// 			float dis 	= m_pHistograme[dim].pQuantile[m_nQuantiNum] - m_pHistograme[dim].pQuantile[0];
// 			float disMax 	= m_FMMaxQuant[dim][m_nQuantiNum] - m_FMMaxQuant[dim][0];
// 			if (dis >= disMax) 
// 				memcpy( m_FMMaxQuant[dim], m_pHistograme[dim].pQuantile, sizeof(float)*(m_nQuantiNum+1) );
// 		}
// 	}
// 	//for debug
// 	
// 	float fSumDis = 0;
// 	for (dim=1; dim<nBandNum; dim++) { 
// 		fSumDis +=  m_FMMaxQuant[dim][m_nQuantiNum]- m_FMMaxQuant[dim][0];
// 		//
// 		printf("=================== MaxiMum Quantile =====================\n");
// 		for (ii=0; ii<=m_nQuantiNum; ii++) {
// 		//
// 			if (ii != m_nQuantiNum)
// 				printf("%f(%f)\t",  m_FMMaxQuant[dim][ii], ( m_FMMaxQuant[dim][m_nQuantiNum]- m_FMMaxQuant[dim][ii]) );
// 			else
// 				printf("%f(%f)\t",  m_FMMaxQuant[dim][ii], 0. );
// 		}
// 	}
// 	printf("\n fAveDis = %f  \n", (fSumDis/(nBandNum-1)) );
// 	//end debug
// 
// 	return 0;
// }

/*
	//-------------------------
//  CNoiseMask::GetNoiseMaskingRate()
//-------------------------
//10db SNR :   fMaskThresold = 8.5 - 9.5  
float CNoiseMask::GetNoiseMaskingRate(float fMaskThresold) 
{
	int dim;
	float fNoiseFactor = 0;
	// Car noise masking, only the first 1/3 banks are used
	int nBandNum = m_MelBankNum/m_nBankFactor;	

	// calculate the average log FFT over all banks
	float fAveDis(0.), fMin(0.), fMax(0.), fAveNoise(0.); 
	for (dim=1; dim<nBandNum; dim++) {
		//
		fMax      += m_FMMaxQuant.A[dim][m_nQuantiNum];
		fMin	 	+= m_FMMaxQuant.A[dim][0];
		fAveDis 	+= m_FMMaxQuant.A[dim][m_nQuantiNum] - m_FMMaxQuant.A[dim][0];
		fAveNoise += m_FVNizLogFFT.A[dim];
	}
	fMax 	 	/= (nBandNum-1);
	fMin    	/= (nBandNum-1);
	fAveDis 	/= (nBandNum-1);	
	fAveNoise /= (nBandNum-1);
	
	//If current speech is a little noisy, don't add car noise 
	if (fAveDis < fMaskThresold || fMin > fMax-fMaskThresold) {
		return fNoiseFactor ;
	}
	//If current speech is relative clean, car noise is added to mask the samll nonstationary noise
	fNoiseFactor = log ( exp(fMax-fMaskThresold) - exp(fMin) ) / fAveNoise;
	fNoiseFactor = exp(fNoiseFactor);
	
	printf("=====================================\n");
	printf("The noise enlarge factor = %f\n", fNoiseFactor );
	printf("fMax = %f 	\t 	fMin = %f    \t   fAveDis=%f\n", fMax, fMin, fAveDis);
	printf("=====================================\n");
	return fNoiseFactor;
}

//-------------------------
//  CNoiseMask::PerformNoiseMask()
//-------------------------
void CNoiseMask::PerformNoiseMask( FMatrix & FMSpeechFFT, FMatrix & FMNoiseFFT, float fNoiseFactor )
{
	int ii, jj;
	int nFrmNum = FMSpeechFFT.dimM;
	int nFFTDim = FMSpeechFFT.dimN;
	//
	srand(time(NULL));
	int stFrm = int(   rand() / (float)RAND_MAX * (FMNoiseFFT.dimM-FMSpeechFFT.dimM-1)   );
	//
	for (ii=0; ii<nFrmNum; ii++) 
		for (jj=0; jj<nFFTDim; jj++) {
			FMSpeechFFT.A[ii][jj] += fNoiseFactor * FMNoiseFFT.A[stFrm+ii][jj];
		}
	return;
}*/

//-------------------------
//  CNoiseMask::GetNoiseMaskingRate()
//-------------------------
