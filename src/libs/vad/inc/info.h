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
 * $Id: info.h,	v 1.0 2007/06/14 10:50:00 Sumu Exp $
 *
 *	
 * (C) Panasonic BeiJing Laboratory, PRDCC.
 *   18th Floor, Zhongguancun Building, No.27
 *             BeiJing, CHINA
 *
 *  declaration file for output information gatherd during the running of program.
 *  Mainly it will handle the error and warning information, but it also can output
 *  some intermediate information needed by users..
 *
 */

#include <stdio.h>

#ifndef __INFO_H
#define __INFO_H

// Global Variables
const int STR_SIZE     = 1024 ;
const int MAX_STR_SIZE = 10000 ;


static char*  pchFileName;
static char   pchFileLineNo[STR_SIZE];
static int    nInfoType;
static char   pchInfoBuf[MAX_STR_SIZE] ;
static FILE*  fpFILE;
//Global Function
typedef void (*InfoLogPtr) (char *pchFormat, ...) ;
void  infoLog(char*pchFormat, ...) ;
InfoLogPtr  getInfoLogPtr(char *pchFileName, int nLineNo, int nType,FILE* fp = NULL) ;

//define information of different level..
#define FILE_NAME __FILE__  
#define FILE_LINE __LINE__		

#define INFO	getInfoLogPtr(FILE_NAME, FILE_LINE, 0)
#define WARN	getInfoLogPtr(FILE_NAME, FILE_LINE, 1)
#define ERROR	getInfoLogPtr(FILE_NAME, FILE_LINE, 2)
#define DEBUG	getInfoLogPtr(FILE_NAME, FILE_LINE, 3)
#define FLOG	getInfoLogPtr(FILE_NAME, FILE_LINE, 4,fp)			
#endif
