//
//  USCUtil.h
//  usc
//
//  Created by hejinlai on 12-11-6.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIApplication.h>
#import <sys/socket.h>
#import <sys/sysctl.h>
#import <net/if.h>
#import <net/if_dl.h>

@interface USCUtil : NSObject
{
@protected
    NSString *_patternPartial_s_unk_s;
    NSString *_patternPartial_s_SIL;
    NSString *_templateBlankSymbol;
    NSString *_templateCommaSymbol;
    NSString *_fullStopSymbol;
    NSString *_questionMarkSymbol;
    
    NSString *_patternSentence;
    NSString *_patternSentence_comma_comma;
    NSString *_patternSentence_comma;
    
    NSString *_patternLeftBrace;
    NSString *_patternRightBrace;
    NSString *_patternCommaEnd;
    
    BOOL _isPunctuation;
}

@property (nonatomic, copy) NSString *patternPartial_s_unk_s;
@property (nonatomic, copy) NSString *patternPartial_s_SIL;
@property (nonatomic, copy) NSString *templateBlankSymbol;
@property (nonatomic, copy) NSString *templateCommaSymbol;
@property (nonatomic, copy) NSString *fullStopSymbol;
@property (nonatomic, copy) NSString *questionMarkSymbol;
@property (nonatomic, copy) NSString *patternSentence;
@property (nonatomic, copy) NSString *patternSentence_comma_comma;
@property (nonatomic, copy) NSString *patternSentence_comma;
@property (nonatomic, copy) NSString *patternLeftBrace;
@property (nonatomic, copy) NSString *patternRightBrace;
@property (nonatomic, copy) NSString *patternCommaEnd;
@property (nonatomic, assign) BOOL isPunctuation;

- (NSMutableString *)escapePartialTag:(NSMutableString *)sentence;
- (NSMutableString *)escapeSentenceTag:(NSMutableString *)sentence isRemoveBeginToken:(BOOL)flag;
- (NSMutableString *)escapeBrackets:(NSString *)str;
- (NSMutableString *)replaceHeadComma:(NSMutableString *)text;

+ (NSString *)getPackageName;

+ (NSString *)getCarrier;

+ (NSString *)getLocalMacAddress;

+ (int)dataNetworkTypeFromStatusBar;

+ (NSString *)getDeviceOS;


@end
