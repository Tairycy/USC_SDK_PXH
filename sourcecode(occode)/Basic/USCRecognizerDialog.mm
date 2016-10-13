//
//  USCRecognizerDialog.m
//  USCUIView
//
//  Created by hejinlai on 12-12-4.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import "USCRecognizerDialog.h"
#import "USCRecognizerView.h"
#import "USCRecognizer.h"
#import "USCErrorCode.h"
#import "USCVersion.h"

@interface USCRecognizerDialog () <USCRecognizerViewDelegate, USCRecognizerDelegate>
{
    USCRecognizerView *recognizerView;
    
    NSString *appKey;
    USCRecognizer *recognizer;
    NSMutableString *lastResult;
    
    NSTimeInterval lastTime;
}
@property (nonatomic,weak,readwrite) id<USCRecognizerDialogDelegate> mdelegate;

@end

@implementation USCRecognizerDialog

@synthesize delegate = _mDelegate;

- (id)initWithAppKey:(NSString *)appkey
{
    self = [super init];
    if (self) {
        
        appKey = [appkey copy];
        
        recognizerView = [[USCRecognizerView alloc] init];
        recognizerView.delegate = self;
        [self addSubview:recognizerView];
        
        recognizer = [[USCRecognizer alloc] initWithAppKey:appkey secret:nil];
        recognizer.delegate = self;
        
        lastResult = [[NSMutableString alloc] init];
        self.windowLevel = UIWindowLevelAlert;
        
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(resignActive)
                                                     name:UIApplicationWillResignActiveNotification
                                                   object:nil];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(enterForeground)
                                                     name:UIApplicationWillEnterForegroundNotification
                                                   object:nil];
        
        // 当屏幕翻转的时候
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(changeFrame) name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];

        // 如果一开始就是横屏，那么就调用changeFrame方法进行旋转
        [self changeFrame];
    }
    return self;
}

- (void) cancel
{
    [self onButtonCancelled];
}

//屏幕翻转的时候，修改子视图的frame
- (void)changeFrame
{
        //获取当前电池条的方向
        UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;

        //获取当前屏幕的大小
        CGRect frame = [UIScreen mainScreen].applicationFrame;
        
        //设置我们的View的中心点
        recognizerView.center = CGPointMake(frame.origin.x + ceil(frame.size.width/2), frame.origin.y + ceil(frame.size.height/2));
        
        //根据当前电池条的方向，获取需要旋转的角度的大小
        if (orientation == UIInterfaceOrientationLandscapeLeft)
        {
            recognizerView.transform = CGAffineTransformMakeRotation(M_PI*1.5);
        }
        else if (orientation == UIInterfaceOrientationLandscapeRight)
        {
            recognizerView.transform = CGAffineTransformMakeRotation(M_PI/2);
        }
        else if (orientation == UIInterfaceOrientationPortraitUpsideDown)
        {
            recognizerView.transform = CGAffineTransformMakeRotation(-M_PI);
        }
        else
        {
            recognizerView.transform = CGAffineTransformIdentity;
        }
        
        // 可以动画的改变我们view的显示方式了
        [[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeRight animated:YES];
        
         CGFloat duration = [UIApplication sharedApplication].statusBarOrientationAnimationDuration;//（获取当前电池条动画改变的时间）

        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:duration];
        
        // 在这里设置view.transform需要匹配的旋转角度的大小就可以了。
        [UIView commitAnimations];
}

- (void)showInView:(UIView *)view
{
    [self show];
}

/*
 默认调用show函数显示在屏幕中间
 */
- (void)show
{
    [self makeKeyAndVisible];
    recognizerView.center = self.center;
    [self onButtonSayBegin];
}

// 切换到后台
- (void)resignActive
{
    //[recognizer cancel];
    [self onButtonCancelled];
}

// 切换到前台
- (void)enterForeground
{
    //AudioSessionSetActive(true);
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.frame = [[UIScreen mainScreen] bounds];
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}
- (void)onButtonCancelled
{
    [recognizer cancel];
    [self resignKeyWindow];
    self.hidden = YES;
}

- (void)onButtonSayOver
{
    [recognizerView setModeProcessing];
    [recognizer stop];
}

- (void)onButtonSayBegin
{
    NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
    double diff = startTime - lastTime;
    lastTime = startTime;
    if (diff <= 0.5) {
        [recognizerView setModePrompt:@"操作时间太频繁"];
        if (_mDelegate && [_mDelegate respondsToSelector:@selector(onEnd:)])
        {
            NSString *msg = [USCErrorCode getErrorMsg:OPERATION_TOO_FAST];
            [_mDelegate onEnd:[NSError errorWithDomain:msg code:OPERATION_TOO_FAST userInfo:nil]];
        }
        return;
    }

    [lastResult deleteCharactersInRange:NSMakeRange(0, lastResult.length)];
    [recognizerView setModeInit];
    [recognizer start];
}

- (void)onStart
{
    [recognizerView setModeListening];
}

- (void)onResult:(NSString *)result isLast:(BOOL)isLast
{
    if (_mDelegate && [_mDelegate respondsToSelector:@selector(onResult: isLast:)])
    {
        [_mDelegate onResult:result isLast:isLast];
    }
    
    if (result) {
        [lastResult appendString:result];
    }
    
}

- (void)onEnd:(NSError *)error
{

    if (_mDelegate && [_mDelegate respondsToSelector:@selector(onEnd:)])
    {
        [_mDelegate onEnd:error];
    }
    
    if (error) {
        [recognizerView setModePrompt:error.domain];
    }else{
        
        if (lastResult.length == 0) {
            [recognizerView setModePrompt:@"没有听到声音"];
        }else{            
            [recognizerView dismissRotateImage];
            [self resignKeyWindow];
            self.hidden = YES;
//            [self removeFromSuperview];
        }
    }
}

- (void)onCancel
{
    
}

- (void)onVADTimeout
{
    [recognizerView setModeProcessing];
    [recognizer stop];
}

- (void)onUpdateVolume:(int)volume
{
    [recognizerView setVolumnImage:volume];
}

- (void)onUploadUserData:(NSError *)error
{
    
}

-(void)onRecognizationTimeout
{
    
}

- (void)onRecordingStop:(NSMutableData *)recordingData
{
    
}

/*
     设置说话停顿的超时时间，单位ms
     frontTime：说话之前的停顿超时时间，默认3000ms
     backTime： 说话之后的停顿超时时间，默认1000ms
 */
//- (void)setVadFrontTimeout:(int)frontTime BackTimeout:(int)backTime
//{
//    [recognizer setVadFrontTimeout:frontTime BackTimeout:backTime];
//}
//
////设置识别超时时间
//- (void)setRecognizationTimeout:(float)recognizationTime
//{
//    [recognizer setRecognizationTimeout:recognizationTime];
//}
//
//- (void)setProperty:(NSString *)property forKey:(int)key
//{
//    [recognizer setProperty:property forKey:key];
//}
//
///*
//     设置采样率，支持8k和16k，默认为16k
// */
//- (void)setSampleRate:(int)rate
//{
//    [recognizer setSampleRate:rate];
//}
//
//- (void) setLanguage:(NSString *)language
//{
//    [recognizer setLanguage:language];
//}
//
//- (void) setPunctuation:(BOOL)isEnable
//{
//    [recognizer setPunctuation:isEnable];
//}
//
///*
//     设置识别参数
// */
//- (BOOL)setEngine:(NSString *)engine
//{
//    return [recognizer setEngine:engine];
//}
//
//+ (NSString *)getVersion
//{
//    return sdk_version;
//}
//
////设置是否允许播放提示音
//- (void) setPlayingBeep:(BOOL)isAllowed
//{
//    [recognizer setPlayingBeep:isAllowed];
//}
//
- (void)dealloc
{
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    if (recognizerView) {
//        [recognizerView release];
    }
    if (appKey) {
//        [appKey release];
    }
    appKey = nil;
    if (recognizer)
    {
//        [recognizer release];
        recognizer = nil;
    }
    if (lastResult) {
//        [lastResult release];
    }
    
//    [super dealloc];
    
}


@end
