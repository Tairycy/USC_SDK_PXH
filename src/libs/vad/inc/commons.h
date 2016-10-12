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
 * $Id: commons.h,	v 1.0 2007/06/14 10:50:00 Sumu Exp $
 *
 *	
 * (C) Panasonic BeiJing Laboratory, PRDCC.
 *   18th Floor, Zhongguancun Building, No.27
 *             BeiJing, CHINA
 *
 * declaration file for global header files inclusion and macros
 */



#ifndef __COMMONS_H
#define __COMMONS_H

#include "info.h"

// -------------------------------------------------
// C Library
// -------------------------------------------------

//#include <cstdlib>
//#include <cstdio>
//#include <climits>
//#include <cfloat>
//#include <cmath>
#include <stdlib.h>
//#include <stdio.h>
//#include <limits.h>
//#include <float.h>
#include <math.h>
//#include <assert.h>
#include "string.h"
// -------------------------------------------------
// C++ STL Library
// -------------------------------------------------
/*
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <iterator>
*/
//using namespace std;

//-------------------------------------------------
//Typedefs and common macros
//-------------------------------------------------
// typedef unsigned char   UCHAR;
// typedef   signed char    CHAR;
// typedef unsigned short USHORT;
// typedef   signed short  SHORT;
// typedef unsigned int     UINT;
// typedef   signed int      INT;
// typedef unsigned long   ULONG;
// typedef   signed long    LONG;


#  define max(a,b)    (((a) > (b)) ? (a) : (b))
#  define min(a,b)    (((a) < (b)) ? (a) : (b))

#ifdef PI
#undef PI                /* PI is defined in Linux */
#endif
#define PI		3.14159265358979
#define	TPI		6.28318530717959

typedef struct EPD_ResultS
{
	float frameEn;
	float MaxS;
	float mask;
    int   type;
}
Result;

class NODE
{
public:
//	string name ;
	char name[256];
} ;

#endif
