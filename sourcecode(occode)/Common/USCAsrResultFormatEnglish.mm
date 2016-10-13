//
//  USCAsrResultFormatEnglish.m
//  usc
//
//  Created by jiqiangsheng on 14-4-16.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCAsrResultFormatEnglish.h"

@implementation USCAsrResultFormatEnglish

@synthesize punctuationRestorer = _punctuationRestorer;
@synthesize patternBlankComma = _patternBlankComma;

-(id)init
{
    self = [super init];
    if (self)
    {
        self.templateCommaSymbol = @",";
        
//        self.patternPartial_s_unk_s = @"<s>|<unk>|\\s+";
//        self.patternPartial_s_SIL = @"(</s>)+|<SIL>";
        
        self.patternPartial_s_unk_s = @"<s>|<unk>";
        self.patternPartial_s_SIL = @"(</s>)+|<SIL>";
        
        self.patternSentence_comma_comma = @"(^,)|(,$)";
        self.patternSentence_comma = @"(,$)";
        self.patternSentence = @"";
        self.questionMarkSymbol = @"?";
        self.fullStopSymbol = @".";
        self.patternCommaEnd = @"(^,)";
        self.patternBlankComma = @" ,";
        
        USCPunctuationRestorer *punctuationRestorer = [[USCPunctuationRestorer alloc] init];
        self.punctuationRestorer = punctuationRestorer;
//        [punctuationRestorer release];
        punctuationRestorer = nil;
    }
    
    return self;
}

- (NSMutableString *)escapeSentenceTag:(NSMutableString *)sentence isRemoveBeginToken:(BOOL)flag
{
    NSMutableString *result = sentence;
    result = [self escapePartialTag:result];
    
    NSRegularExpression *regex;
    if (flag) {
        regex = [[NSRegularExpression alloc] initWithPattern:self.patternSentence_comma_comma options:NSRegularExpressionCaseInsensitive error:nil];
        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    }else{
        regex = [[NSRegularExpression alloc] initWithPattern:self.patternSentence_comma options:NSRegularExpressionCaseInsensitive error:nil];
        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    }
    
    NSRegularExpression *regex2 = [[NSRegularExpression alloc] initWithPattern:self.patternBlankComma options:NSRegularExpressionCaseInsensitive error:nil];
    [regex2 replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    
    if ([result length] > 0)
    {
        if (self.isPunctuation)      //如果允许添加标点符号
        {
            NSString *endSymbol = [self.punctuationRestorer restorePunctuation:result];
            [result appendString:endSymbol];
        }
    }
    
    return result;
}

@end
