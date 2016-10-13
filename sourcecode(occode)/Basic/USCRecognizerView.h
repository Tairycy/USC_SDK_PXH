//
//  USCView.h
//  USCUIView
//
//  Created by hejinlai on 12-12-4.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol USCRecognizerViewDelegate <NSObject>

- (void)onButtonCancelled;
- (void)onButtonSayOver;
- (void)onButtonSayBegin;

@end

@interface USCRecognizerView : UIView
{
    // 背景
    UIImageView     *bgImageView;
    UIImage         *bgImage;
    
    // 标题
    UILabel         *titleLabel;
    
    // 左边按钮
    UIButton        *leftButton;
    UIImage         *leftButtonImage;
    UIImage         *leftButtonImagePressed;
    
    // 右边按钮
    UIButton        *rightButton;
    UIImage         *rightButtonImage;
    UIImage         *rightButtonImagePressed;
    
    // 整个按钮
    UIButton        *wholeButton;
    UIImage         *wholeButtonImage;
    UIImage         *wholeButtonImagePressed;
    
    // 警告图片
    UIImageView     *alertImageView;
    UIImage         *alertImage;
    
    // 说话时的分割线
    UIImageView     *speakingLineImageView;
    UIImage         *speakingLineImage;
    
    // 等待时的分割线
    UIImageView     *waitingLineImageView;
    UIImage         *waitingLineImage;
    
    // 停止时的分割线
    UIImageView     *stopLineImageView;
    UIImage         *stopLineImage;

    // spinner
    UIImageView     *spinnerBgImageView;
    UIImage         *spinnerBgImage;
    UIImageView     *spinnerImageView;
    UIImage         *spinnerImage;
    
    // 初始化动画图片
    NSMutableArray  *initImages;
    UIImageView     *initImageView;
    
    // 音量动画图片
    NSMutableArray  *volumnImages;
    UIImageView     *volumnImageView;
    
    // 字体颜色
    UIColor         *textColor;
    
    // 定时器
    NSTimer         *rotateTimer;
    float           rotateAngle;
    
    // logo
    UIImageView     *logoImageView;
    UIImage         *logoImage;
    
    BOOL _isNewUI;  //判断是新UI，还是旧UI
}

@property (nonatomic, assign) id<USCRecognizerViewDelegate> delegate;
@property (nonatomic, assign, readwrite) BOOL isNewUI;

- (void)setVolumnImage:(int)volumn;
- (void)dismissRotateImage;

- (void)setModeInit;
- (void)setModeListening;
- (void)setModeProcessing;
- (void)setModePrompt:(NSString *)prompt;

@end
