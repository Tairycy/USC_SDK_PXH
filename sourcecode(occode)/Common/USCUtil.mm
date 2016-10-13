//
//  USCUtil.m
//  usc
//
//  Created by hejinlai on 12-11-6.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import "USCUtil.h"
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>
#import "USCOpenUDID.h"
#import "function.h"

@interface USCUtil()
{

}
@property (nonatomic,strong) NSString *patter;
@property (nonatomic,strong) NSString *patter2;

@property (nonatomic,strong) NSString *patter3;

@end

@implementation USCUtil

@synthesize patternPartial_s_unk_s = _patternPartial_s_unk_s;
@synthesize patternPartial_s_SIL = _patternPartial_s_SIL;
@synthesize templateBlankSymbol = _templateBlankSymbol;
@synthesize templateCommaSymbol = _templateCommaSymbol;
@synthesize fullStopSymbol = _fullStopSymbol;
@synthesize questionMarkSymbol = _questionMarkSymbol;
@synthesize patternSentence = _patternSentence;
@synthesize patternSentence_comma_comma = _patternSentence_comma_comma;
@synthesize patternSentence_comma = _patternSentence_comma;
@synthesize patternLeftBrace = _patternLeftBrace;
@synthesize patternRightBrace = _patternRightBrace;
@synthesize patternCommaEnd = _patternCommaEnd;
@synthesize isPunctuation = _isPunctuation;

-(id)init
{
    self = [super init];
    if (self)
    {
      self.patternPartial_s_unk_s = @"<s>|<unk>";
//        self.patternPartial_s_unk_s = @"<s>|<unk>|\\s+";
        self.patter = @"([^a-z]) ";
        self.patter2 = @"^\\s*|\\s*$";
        self.patter3 = @"\\s*[u4e00-u9fa5]\\s* ";

        self.patternPartial_s_SIL = @"(</s>)+|<SIL>";
        self.templateBlankSymbol = @"";                //空字符串
        self.templateCommaSymbol = @"，";

        
        self.patternSentence_comma_comma = @"(^，)|(，$)";
        self.patternSentence_comma = @"(，$)";
        self.patternSentence = @"([吗呢]$)|(^(请问)|(为(什么|啥))|(怎么))";
        self.questionMarkSymbol = @"？";
        self.fullStopSymbol = @"。";
        self.patternLeftBrace = @"<";
        self.patternRightBrace = @">";
        self.patternCommaEnd = @"(^，)";
        
        self.isPunctuation = YES;
    }
    return self;
}

//- (NSMutableString *)escapePartialTag:(NSMutableString *)sentence
//{
//    NSMutableString *result = sentence;
//
//    NSRegularExpression *regex = [[NSRegularExpression alloc] initWithPattern:self.patter options:NSRegularExpressionCaseInsensitive error:nil];
//
//    [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:@"$1"];
//
//    NSLog(@"1 =~%@~",result);
//
////    regex = [[NSRegularExpression alloc] initWithPattern:self.patternPartial_s_unk_s options:NSRegularExpressionCaseInsensitive error:nil];
////    [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
////    NSLog(@"3 =~%@~",result);
//
//    regex = [[NSRegularExpression alloc] initWithPattern:self.patternPartial_s_SIL options:NSRegularExpressionCaseInsensitive error:nil];
//
//    if (self.isPunctuation)     //如果添加标点符号，中文和粤语添加相应的，否则添加空
//    {
//        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateCommaSymbol];
//    }
//    else
//    {
//        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
//    }
//
//    regex = [[NSRegularExpression alloc] initWithPattern:self.patter2 options:NSRegularExpressionCaseInsensitive error:nil];
//    [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
//
//    NSLog(@"2 =~%@~",result);
//
//    regex = [[NSRegularExpression alloc] initWithPattern:self.patter3 options:NSRegularExpressionCaseInsensitive error:nil];
//    [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:@"$1"];
//    NSLog(@"3 =~%@~",result);
//
//    return result;
//}

- (NSMutableString *)escapePartialTag:(NSMutableString *)sentence
{
    NSMutableString *result = sentence;

    /**********************/
     NSRegularExpression *regex1 = [[NSRegularExpression alloc] initWithPattern:self.patter options:NSRegularExpressionCaseInsensitive error:nil];
    [regex1 replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:@"$1"];
//    NSLog(@"3 =~%@~",result);
    /*********** ***********/

    NSRegularExpression *regex = [[NSRegularExpression alloc] initWithPattern:self.patternPartial_s_unk_s options:NSRegularExpressionCaseInsensitive error:nil];

    [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];

//    NSLog(@"1 =~%@~",result);

    regex = [[NSRegularExpression alloc] initWithPattern:self.patternPartial_s_SIL options:NSRegularExpressionCaseInsensitive error:nil];

    if (self.isPunctuation)        // 如果添加标点符号，中文和粤语添加相应的，否则添加空
    {
        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateCommaSymbol];
    }
    else
    {
        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    }
//   NSLog(@"2 =~%@~",result);

    NSRegularExpression *regex2 = [[NSRegularExpression alloc] initWithPattern:self.patter options:NSRegularExpressionCaseInsensitive error:nil];
    [regex2 replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:@"$1"];
//    NSLog(@"4 =~%@~",result);

    return result;
}

- (NSMutableString *) escapeSentenceTag:(NSMutableString *)sentence isRemoveBeginToken:(BOOL)flag
{
    NSMutableString *result = sentence;
    result = [self escapePartialTag:result];

    NSRegularExpression *regex;          //如果
    if (flag) {
        regex = [[NSRegularExpression alloc] initWithPattern:self.patternSentence_comma_comma options:NSRegularExpressionCaseInsensitive error:nil];
        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    }else{
        regex = [[NSRegularExpression alloc] initWithPattern:self.patternSentence_comma options:NSRegularExpressionCaseInsensitive error:nil];
        [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    }
//    NSLog(@"5 =~%@~",result);
    if (self.isPunctuation)     //如果添加标点符号，那么就加上句尾的标点符号，然后返回，否则什么也不加，直接返回
    {
        result = [self getLastSentence:result];
    }
//   NSLog(@"6 =~%@~",result);
    return result;
}

// 获取最后的标点符号，返回最终的识别结果，该函数的标点符号控制在外部。
- (NSMutableString *) getLastSentence:(NSMutableString *)text
{
    NSMutableString *result = text;
    if ([text length] > 0)
    {
        NSRegularExpression *regex = [[NSRegularExpression alloc] initWithPattern:self.patternSentence options:NSRegularExpressionCaseInsensitive error:nil];
        NSRange range = [regex rangeOfFirstMatchInString:result options:0 range:NSMakeRange(0, [result length])];
        
        if (!NSEqualRanges(range, NSMakeRange(NSNotFound, 0)))
        {
            [result appendString:self.questionMarkSymbol];
        }
        else
        {
            [result appendString:self.fullStopSymbol];
        }
//        NSLog(@"7 =~%@~",result);
        NSString *substr = [result substringWithRange:NSMakeRange(result.length -2, 1)];

        if ([[result substringWithRange:NSMakeRange(result.length - 2, 1)]  isEqual: @" "]) {
            [result deleteCharactersInRange:NSMakeRange(result.length -2, 1)];
//            NSLog(@"8 =~%@~",result);
        }
    }
    return result;
}

//去掉尖括号
- (NSMutableString *)escapeBrackets:(NSString *)str
{
    if (str == nil || [str length] == 0) {
        return nil;
    }
    
    NSMutableString *result = [NSMutableString stringWithString:str];
    
    NSRegularExpression *regexLeft = [[NSRegularExpression alloc] initWithPattern:self.patternLeftBrace options:NSRegularExpressionCaseInsensitive error:nil];
    [regexLeft replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
//    [regexLeft release];
    
    NSRegularExpression *regexLeft2 = [[NSRegularExpression alloc] initWithPattern:self.patternLeftBrace options:NSRegularExpressionCaseInsensitive error:nil];
    
    if (self.isPunctuation)    //如果添加标点符号，中文和粤语添加相应的标点符号，否则添加空
    {
        [regexLeft2 replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateCommaSymbol];
    }
    else
    {
        [regexLeft2 replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    }
//    [regexLeft2 release];
    
    
    NSRegularExpression *regexRight = [[NSRegularExpression alloc] initWithPattern:self.patternRightBrace options:NSRegularExpressionCaseInsensitive error:nil];
    
    if (self.isPunctuation)
    {
        [regexRight replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateCommaSymbol];
    }
    else
    {
        [regexRight replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    }
//    [regexRight release];
    
    NSRegularExpression *regexRight2 = [[NSRegularExpression alloc] initWithPattern:self.patternRightBrace options:NSRegularExpressionCaseInsensitive error:nil];
    if (self.isPunctuation)
    {
        [regexRight2 replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateCommaSymbol];
    }
    else
    {
        [regexRight2 replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];

    }
//    [regexRight2 release];
    
    return result;
}

- (NSMutableString *)replaceHeadComma:(NSMutableString *)text
{
    NSMutableString *result = text;
    NSRegularExpression *regex = [[NSRegularExpression alloc] initWithPattern:self.patternCommaEnd options:NSRegularExpressionCaseInsensitive error:nil];
    [regex replaceMatchesInString:result options:0 range:NSMakeRange(0, [result length]) withTemplate:self.templateBlankSymbol];
    
    return result;
}

+(NSString *)getLocalMacAddress
{
    int                 mib[6];
    size_t              len;
    char                *buf;
    unsigned char       *ptr;
    struct if_msghdr    *ifm;
    struct sockaddr_dl  *sdl;
    
    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_LINK;
    mib[4] = NET_RT_IFLIST;
    
    if ((mib[5] = if_nametoindex("en0")) == 0) {
        //printf("Error: if_nametoindex error/n");
        return nil;
    }
    
    if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
        //printf("Error: sysctl, take 1/n");
        return nil;
    }
    
    if ((buf = (char *)malloc(len)) == NULL) {
        //printf("Could not allocate memory. error!/n");
        return nil;
    }
    
    if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
        //printf("Error: sysctl, take 2");
        
        //添加的释放内存的代码
        if (buf) {
            free(buf);
        }
        return nil;
    }
    
    ifm = (struct if_msghdr *)buf;
    sdl = (struct sockaddr_dl *)(ifm + 1);
    ptr = (unsigned char *)LLADDR(sdl);
    NSString *outstring = [NSString stringWithFormat:@"%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
    //NSString *outstring = [NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
    free(buf);
    
    if (outstring && outstring.length > 0) {
        NSString *macAddress = [outstring uppercaseString];
        //iOS7 -> 02:00:00:00:00:00
        if (![macAddress isEqualToString:@"02:00:00:00:00:00"]) {
            return macAddress;
        }
    }
    
    return [USCOpenUDID value];
}

+ (NSString *)getPackageName  //获取包名限制
{
    NSBundle *bundle =[NSBundle mainBundle];
    NSDictionary *info =[bundle infoDictionary];
    return [info objectForKey:@"CFBundleIdentifier"];
}

+ (NSString *)getCarrier    //获取运营商
{
    CTTelephonyNetworkInfo *ctNetInfo = [[CTTelephonyNetworkInfo alloc] init];
    CTCarrier *carrier = ctNetInfo.subscriberCellularProvider;
    NSMutableString *carrierCode = [[NSMutableString alloc] init];
    if (carrier) {
        if (carrier.mobileCountryCode) {
             [carrierCode appendString:carrier.mobileCountryCode];
        }
        if (carrier.mobileNetworkCode) {
            [carrierCode appendString:carrier.mobileNetworkCode];
        }
    }
    
    if (carrierCode.length == 0) {
        
        [carrierCode appendString:@"0"];
    }
    
//    [ctNetInfo release];
    return carrierCode;
}

/*
     检测网络类型，2G，3G，wifi
 */
+ (int)dataNetworkTypeFromStatusBar {
    
    UIApplication *app = [UIApplication sharedApplication];
    NSArray *subviews = [[[app valueForKey:@"statusBar"] valueForKey:@"foregroundView"] subviews];
    NSNumber *dataNetworkItemView = nil;
    
    for (id subview in subviews) {
        if([subview isKindOfClass:[NSClassFromString(@"UIStatusBarDataNetworkItemView") class]]) {
            dataNetworkItemView = subview;
            break;
        }
    }
    
    int netType = NETWORK_TYPE_NONE;
    NSNumber * num = [dataNetworkItemView valueForKey:@"dataNetworkType"];
    if (num == nil) {
        
        netType = NETWORK_TYPE_NONE;
        
    }else{
        
        int n = [num intValue];
        if (n == 0) {
            netType = NETWORK_TYPE_NONE;
        }else if (n == 1){
            netType = NETWORK_TYPE_2G;
        }else if (n == 2){
            netType = NETWORK_TYPE_3G;
        }else{
            netType = NETWORK_TYPE_WIFI;
        }
    }
    
    return netType;
}

+ (NSString *)getDeviceOS
{
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char *machine = (char*)malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        NSString *platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];

        NSDictionary * infoDict = [[NSBundle mainBundle] infoDictionary];

        NSString *strName = [[UIDevice currentDevice] name];
        //    NSLog(@"设备名称：%@", strName);//e.g. "My iPhone"

        NSString *strId = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        //    NSLog(@"设备唯一标识：%@", strId);//UUID,5.0后不可用

        NSString *strSysName = [[UIDevice currentDevice] systemName];
        //    NSLog(@"系统名称：%@", strSysName);// e.g. @"iOS"

        NSString *strSysVersion = [[UIDevice currentDevice] systemVersion];
        //    NSLog(@"系统版本号：%@", strSysVersion);// e.g. @"4.0"

    return [NSString stringWithFormat:@"%@%@",strSysName,strSysVersion];
}
@end
