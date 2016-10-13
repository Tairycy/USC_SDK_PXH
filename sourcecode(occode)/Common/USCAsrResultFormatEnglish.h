//
//  USCAsrResultFormatEnglish.h
//  usc
//
//  Created by jiqiangsheng on 14-4-16.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCUtil.h"
#import "USCPunctuationRestorer.h"

@interface USCAsrResultFormatEnglish : USCUtil
{
    USCPunctuationRestorer *_punctuationRestorer;
    NSString *_patternBlankComma;
}

@property (nonatomic, retain, readwrite) USCPunctuationRestorer *punctuationRestorer;
@property (nonatomic, copy) NSString *patternBlankComma;

@end
