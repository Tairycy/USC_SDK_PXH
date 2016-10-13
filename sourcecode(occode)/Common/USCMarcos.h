//
//  USCMarcos.h
//  usc
//
//  Created by hejinlai on 12-12-21.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

/*
 #ifndef __OPTIMIZE__                 //if not on release mode, output log
 #define NSLog(...) NSLog(__VA_ARGS__)
 #else                                //otherwise, output blank log
 #define NSLog(...) {}
 #endif
 */

#ifndef usc_USCMarcos_h
#define usc_USCMarcos_h

#define USC_BUG 0

#if USC_BUG
#define USCLog(...) NSLog(__VA_ARGS__)

#else
#define USCLog(...) ;
#endif



#endif
