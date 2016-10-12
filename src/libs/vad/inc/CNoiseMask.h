// -------------------------------------------------
//
// Module	: Header file of quantile based noise masking
// Author	: JiaLei
// Date	: 04/27/2007
// $Id		: CNoiseMask.h
//
// -------------------------------------------------

#ifndef		CNOISE_MASK_H
#define		CNOISE_MASK_H
//#define		__LOG_NOISE_MASK__
#include		"commons.h"
#include		"vector.h"
#include 		"matrix.h"



// ------------------------
// CHistograme
// ------------------------

class CHistograme 
{
public:
	CHistograme();
	~CHistograme();
	void AllcateMem( int nInHisLen, int nInQuantileNum );
	

public:
	/* Histogram  variables */
	int				nHisLen;			/* Histogram length */
	int				nCurTimeMark;		/* The current time in the pHisgram */
	FVector			m_HistoG;		/* The mel bank energy list from t-nHisLen to t */
	IVector			m_HistoI;			/* The index in the pSeqTime of each time. */
	IVector			m_HistoT;			/* The time of pHisgram in sequence from large to samll. */
	/* Quantile variables */
	int				nQuantileNum;		/* Quantile number */
//	float			*pQuantile;			/* The quantile of mel bank energy list */
	FVector			m_Quant;
//	float			*pTarDiff;
//	float			*pTmpQuantile;		/* The quantile of mel bank energy list */
	
public:
	//
	int				Initialization();
	int				SetDefaultHisgram();
	int				GetInsertPos(float fInValue);
	int				GetQuantile();
	int				InsertToHisgram(float nInValue);
	//
	int				PrintQuantile();
	int 			PrintHisgram();

};

// ------------------------
// Noise masking
// ------------------------
class CNoiseMask
{
public:
	CNoiseMask(int nInWinLen, int nQuantNum, int nInMelBankNum);
	~CNoiseMask();
	//
	FMatrix 				m_FMMaxQuant;
	class CHistograme 		*m_pHistograme;
	//
	int 					m_nQuantiNum;
	int 					m_nWinLen;
	int 					m_MelBankNum;
public:
	//
	void 			IniSpHistograme();
	//	
	void 			ResetQuantile();
	void 			GetQuantMask(FVector & inVec,FMatrix& maskM );

private:
 	int	m_frameCount;
#ifdef __LOG_NOISE_MASK__
	FILE* fp;
#endif
};

//
#endif
