//
//  USCPunctuationRestorer.m
//  usc
//
//  Created by jiqiangsheng on 14-4-15.
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import "USCPunctuationRestorer.h"

NSString *const patternStr = @"(should|should not|shouldn't|ought to|ought not to|oughtn't|may|may not|mayn't|need|need not|needn't|must|must not|mustn't|were|were not|weren't|could|could not|couldn't|will|will not|won’t|are|are not|aren't|can|can not|can't|have|have not|haven't|had|had not|hadn't|do|do not|don't|did|did not|didn't|shall|shall not|shan't) (you|we|I|they) .*;(should|should not|shouldn't|ought to|ought not to|oughtn't|may|may not|mayn't|need|need not|needn't|must|must not|mustn't|were|were not|weren't|could|could not|couldn't|will|will not|won’t|are|are not|aren't|can|can not|can't|have|have not|haven't|had|had not|hadn't|do|do not|don't|did|did not|didn't|shall|shall not|shan't) (you|we|I|they)$;(am|am not|ain't) (i) .*;(am|am not|ain't) (i)$;(was|was not|wasn't|is|is not|isn't|has|has not|hasn't|can|can not|can't|does|does not|doesn't|did|did not|didn't|had|had not|hadn't) (it|he|she) .*;(was|was not|wasn't|is|is not|isn't|has|has not|hasn't|can|can not|can't|does|does not|doesn't|did|did not|didn't|had|had not|hadn't) (it|he|she)$;^(has|has not|hasn't|can|can not|can't|does|does not|doesn't|did|did not|didn't|had|had not|hadn't) ;^(has|has not|hasn't|can|can not|can't|does|does not|doesn't|did|did not|didn't|had|had not|hadn't) (that) ;^(who|when|what|where|which|how|why|how much|how many|how long|how old|what's|what is) ;why|how much|how many|how long|how old$;(who|when|what|where|which|how|why|how much|how many|how long|how old|what's) (do|did|is|hava|had|had not) .*;(who|when|what|where|which|how|why|how much|how many|how long|how old|what's) (do|did|is|hava|had|had not) ";


@implementation USCPunctuationRestorer

-(id)init
{
    self = [super init];
    if (self)
    {
        NSMutableArray *arrayPd = [[NSMutableArray alloc] init];
        self.arrayPd = arrayPd;
//        [arrayPd release];
        arrayPd = nil;
        self.puntuation = @".";
        [self addPattern];
    }
    
    return self;
}

- (void) addPattern
{
    NSArray *arrayPattern = [patternStr componentsSeparatedByString:@";"];
    for (NSString *pstr in arrayPattern)
    {
        NSRegularExpression *regular = [[NSRegularExpression alloc] initWithPattern:pstr options:NSRegularExpressionCaseInsensitive error:nil];
        [self.arrayPd addObject:regular];
    }
}

- (NSString *)restorePunctuation:(NSString *)text
{
    NSString *result = text;
    self.puntuation = @".";
    for (NSRegularExpression *regular in self.arrayPd)
    {
        NSRange range = [regular rangeOfFirstMatchInString:result options:0 range:NSMakeRange(0, [result length])];
        if (!NSEqualRanges(range, NSMakeRange(NSNotFound, 0)))
        {
            self.puntuation = @"?";
            break;
        }
    }
    
    return self.puntuation;
}

- (void)dealloc
{
    self.arrayPd = nil;
    self.puntuation = nil;
    
//    [super dealloc];
}

@end
