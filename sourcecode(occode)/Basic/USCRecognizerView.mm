//
//  USCView.m
//  USCUIView
//
//  Created by hejinlai on 12-12-4.
//  Copyright (c) 2012年 yunzhisheng. All rights reserved.
//

#import "USCRecognizerView.h"

#define USC_BG_WIDTH 286
#define USC_BG_HEIGHT 217

#define USC_BUTTON_WIDTH 138
#define USC_BUTTON_HEIGHT 38

#define USC_BG_WIDTH_NEW 271
#define USC_BG_HEIGHT_NEW 190

#define USC_BUTTON_WIDTH_NEW 136
#define USC_BUTTON_HEIGHT_NEW 44

@implementation USCRecognizerView

@synthesize delegate = _delegate;
@synthesize isNewUI = _isNewUI;


- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        self.isNewUI = NO;
        
        [self loadImages];
        [self initLayout];
        
        rotateTimer = nil;
        rotateAngle = 0.0f;
    }
    return self;
}

// 加载图片
- (void)loadImages
{
    NSString *resPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"usc.bundle"];
    
    //判断vol5.usc这个文件是否存在，如果存在，说明就是旧UI，否则就是新UI
    NSString *vol5_imagePath = [resPath stringByAppendingPathComponent:@"vol5.usc"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:vol5_imagePath])
    {
        self.isNewUI = NO;
        
    }
    else
    {
        self.isNewUI = YES;
     
    }
    
    // 背景图片
    NSString *bgImagePath = [resPath stringByAppendingPathComponent:@"bg.usc"];
    NSData *bgImageEncodeData = [NSData dataWithContentsOfFile:bgImagePath];
    NSData *bgImageDecodeData = [self decodeImage:bgImageEncodeData];
    bgImage = [UIImage imageWithData:bgImageDecodeData];
    
    
    // 左边按钮图片
    NSString *leftButtonImagePath = [resPath stringByAppendingPathComponent:@"left_button.usc"];    
    NSData *leftButtonImageEncodeData = [NSData dataWithContentsOfFile:leftButtonImagePath];
    NSData *leftButtonImageDecodeData = [self decodeImage:leftButtonImageEncodeData];
    leftButtonImage = [UIImage imageWithData:leftButtonImageDecodeData];
    
    
    // 左边选中按钮图片
    NSString *leftButtonImagePressedPath  = [resPath stringByAppendingPathComponent:@"left_button_pressed.usc"];
    NSData *leftButtonImagePressedEncodeData = [NSData dataWithContentsOfFile:leftButtonImagePressedPath];
    NSData *leftButtonImagePressedDecodeData = [self decodeImage:leftButtonImagePressedEncodeData];
    leftButtonImagePressed = [UIImage imageWithData:leftButtonImagePressedDecodeData];
    
    
    // 右边按钮图片
    NSString *rightButtonImagePath = [resPath stringByAppendingPathComponent:@"right_button.usc"];
    NSData *rightButtonImageEncodeData = [NSData dataWithContentsOfFile:rightButtonImagePath];
    NSData *rightButtonImageDecodeData = [self decodeImage:rightButtonImageEncodeData];
    rightButtonImage = [UIImage imageWithData:rightButtonImageDecodeData];
    
    
    // 右边选中按钮图片
    NSString *rightButtonImagePressedPath = [resPath stringByAppendingPathComponent:@"right_button_pressed.usc"];
    NSData *rightButtonImagePressedEncodeData = [NSData dataWithContentsOfFile:rightButtonImagePressedPath];
    NSData *rightButtonImagePressedDecodeData = [self decodeImage:rightButtonImagePressedEncodeData];
    rightButtonImagePressed = [UIImage imageWithData:rightButtonImagePressedDecodeData];

    
    // 整个按钮图片
    NSString *wholeButtonImagePath = [resPath stringByAppendingPathComponent:@"whole_button.usc"];
    NSData *wholeButtonImageEncodeData = [NSData dataWithContentsOfFile:wholeButtonImagePath];
    NSData *wholeButtonImageDecodeData = [self decodeImage:wholeButtonImageEncodeData];
    wholeButtonImage = [UIImage imageWithData:wholeButtonImageDecodeData];
    
    
    // 整个按钮选中图片
    NSString *wholeButtonImagePressedPath = [resPath stringByAppendingPathComponent:@"whole_button_pressed.usc"];
    NSData *wholeButtonImagePressedEncodeData = [NSData dataWithContentsOfFile:wholeButtonImagePressedPath];
    NSData *wholeButtonImagePressedDecodeData = [self decodeImage:wholeButtonImagePressedEncodeData];
    wholeButtonImagePressed = [UIImage imageWithData:wholeButtonImagePressedDecodeData];
    
    
    // 警告图片
    NSString *alertImagePath = [resPath stringByAppendingPathComponent:@"alert.usc"];
    NSData *alertImageEncodeData = [NSData dataWithContentsOfFile:alertImagePath];
    NSData *alertImageDecodeData = [self decodeImage:alertImageEncodeData];
    alertImage = [UIImage imageWithData:alertImageDecodeData];
    
    
    // 说话时的分割线图片
    if (!self.isNewUI) {
        NSString *speakingLineImagePath = [resPath stringByAppendingPathComponent:@"speaking_line.usc"];
        NSData *speakingLineImageEncode = [NSData dataWithContentsOfFile:speakingLineImagePath];
        NSData *speakingLineImageDecode = [self decodeImage:speakingLineImageEncode];
        speakingLineImage = [UIImage imageWithData:speakingLineImageDecode];
    }
    
    
    
    // 等待时的分割线图片
    NSString *waitingLineImagePath = [resPath stringByAppendingPathComponent:@"waiting_line.usc"];
    NSData *waitingLineImageEncodeData = [NSData dataWithContentsOfFile:waitingLineImagePath];
    NSData *waitingLineImageDecodeData = [self decodeImage:waitingLineImageEncodeData];
    waitingLineImage = [UIImage imageWithData:waitingLineImageDecodeData];
    
    
    // 停止时的分割线图片
    NSString *stopLineImagePath = [resPath stringByAppendingPathComponent:@"stop_line.usc"];
    NSData *stopLineImageEncodeData = [NSData dataWithContentsOfFile:stopLineImagePath];
    NSData *stopLineImageDecodeData = [self decodeImage:stopLineImageEncodeData];
    stopLineImage = [UIImage imageWithData:stopLineImageDecodeData];
    
    
    // spinner 背景图片
    if (!self.isNewUI) {
        NSString *spinnerBgImagePath = [resPath stringByAppendingPathComponent:@"spinner_bg.usc"];
        NSData *spinnerBgImageEncodeData = [NSData dataWithContentsOfFile:spinnerBgImagePath];
        NSData *spinnerBgImageDecodeData = [self decodeImage:spinnerBgImageEncodeData];
        spinnerBgImage = [UIImage imageWithData:spinnerBgImageDecodeData];
    }
    
    // spinner 图片
    NSString *spinnerImagePath = [resPath stringByAppendingPathComponent:@"spinner.usc"];
    NSData *spinnerImageEncodeData = [NSData dataWithContentsOfFile:spinnerImagePath];
    NSData *spinnerImageDecodeData = [self decodeImage:spinnerImageEncodeData];
    spinnerImage = [UIImage imageWithData:spinnerImageDecodeData];
    
    
    // 初始化动画图片
    initImages = [[NSMutableArray alloc] init];
    for (int i = 1; i <= 6; i++) {
        NSString *initImageFileName = [NSString stringWithFormat:@"init%i.usc",i];
        NSString *initImageFilePath = [resPath stringByAppendingPathComponent:initImageFileName];
        NSData *initImageFileEncodeData = [NSData dataWithContentsOfFile:initImageFilePath];
        NSData *initImageFileDecodeData = [self decodeImage:initImageFileEncodeData];
        [initImages addObject:[UIImage imageWithData:initImageFileDecodeData]];
    }
    
    // 音量动画图片
    volumnImages = [[NSMutableArray alloc] init];
    int picNum = 8;
    if (self.isNewUI)
    {
        picNum = 4;
    }
    for (int i = 1; i <= picNum; i++) {
        NSString *volImageFileName = [NSString stringWithFormat:@"vol%i.usc",i];
        NSString *volImageFilePath = [resPath stringByAppendingPathComponent:volImageFileName];
        NSData *volImageFileEncodeData = [NSData dataWithContentsOfFile:volImageFilePath];
        NSData *volImageFileDecodeData = [self decodeImage:volImageFileEncodeData];
        [volumnImages addObject:[UIImage imageWithData:volImageFileDecodeData]];
    }


    // logo图片
    NSString *logoImagePath = [resPath stringByAppendingPathComponent:@"logo.usc"];
    NSData *logoImageEncodeData = [NSData dataWithContentsOfFile:logoImagePath];
    NSData *logoImageDecodeData = [self decodeImage:logoImageEncodeData];
    logoImage = [UIImage imageWithData:logoImageDecodeData];
    
    // 文字颜色
    NSString *textColorPath = [resPath stringByAppendingPathComponent:@"textcolor.usc"];
    NSData *textColorEncodeData = [NSData dataWithContentsOfFile:textColorPath];
    NSData *textColorDecodeData = [self decodeImage:textColorEncodeData];
    NSString *textColorStr = [[NSString alloc] initWithData:textColorDecodeData encoding:NSUTF8StringEncoding];
    NSArray *textColorArray = [textColorStr componentsSeparatedByString:@" "];
    float red = [textColorArray[0] floatValue];
    float green = [textColorArray[1] floatValue];
    float blue = [textColorArray[2] floatValue];
    textColor = [UIColor colorWithRed:red green:green blue:blue alpha:1.0];
} 


// 解密图片
- (NSData *)decodeImage:(NSData *)encodeData
{
    NSUInteger len = [encodeData length];
    Byte *b = (Byte *)malloc(len);
    [encodeData getBytes:b length:len];
    for (int i  = 0; i < len; i++) {
        b[i] ^= i;
    }
    
    NSData *decodeData = [NSData dataWithBytes:b length:len];
    free(b);

    return decodeData;
}

- (void)initLayout
{
    // 背景
    if (self.isNewUI)
    {
        self.frame = CGRectMake(0, 0, USC_BG_WIDTH_NEW, USC_BG_HEIGHT_NEW);
    }
    else
    {
        self.frame = CGRectMake(0, 0, USC_BG_WIDTH, USC_BG_HEIGHT);
    }
    
    CGRect screen = [[UIScreen mainScreen] bounds];
    self.center = CGPointMake(screen.size.width/2, screen.size.height/2 - 20);
    bgImageView = [[UIImageView alloc] init];
    bgImageView.center = CGPointMake(screen.size.width/2, screen.size.height/2 - 20);
    if (self.isNewUI)
    {
        bgImageView.frame = CGRectMake(0, 0, USC_BG_WIDTH_NEW, USC_BG_HEIGHT_NEW);
    }
    else
    {
        bgImageView.frame = CGRectMake(0, 0, USC_BG_WIDTH, USC_BG_HEIGHT);
    }
    
    bgImageView.image = bgImage;
    
    
    // 标题
    titleLabel = [[UILabel alloc] init];
    if (self.isNewUI)
    {
        titleLabel.frame = CGRectMake(0, 5, USC_BUTTON_WIDTH_NEW * 2, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        titleLabel.frame = CGRectMake(0, 5, USC_BUTTON_WIDTH * 2, USC_BUTTON_HEIGHT);
    }
//    titleLabel.textAlignment = UITextAlignmentCenter;
    titleLabel.textAlignment = NSTextAlignmentCenter;
    titleLabel.textColor = textColor;
    titleLabel.backgroundColor = [UIColor clearColor];
    
    // 说话时的分割线
    if (!self.isNewUI) {
        speakingLineImageView = [[UIImageView alloc] init];
        speakingLineImageView.frame = CGRectMake(5, 5, 552/2, 76/2);
        speakingLineImageView.image = speakingLineImage;
    }
    
    
    // 等待时的分割线
    waitingLineImageView = [[UIImageView alloc] init];
    waitingLineImageView.frame = CGRectMake(5, 5, 552/2, 76/2);
    waitingLineImageView.image = waitingLineImage;
    waitingLineImageView.hidden = YES;
    
    // 停止时的分割线
    stopLineImageView = [[UIImageView alloc] init];
    stopLineImageView.frame = CGRectMake(5, 5, 552/2, 76/2);
    stopLineImageView.image = stopLineImage;
    stopLineImageView.hidden = YES;
    
    // 左边按钮
    leftButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [leftButton setBackgroundImage:leftButtonImage forState:UIControlStateNormal];
    [leftButton setBackgroundImage:leftButtonImagePressed forState:UIControlStateHighlighted];
    [leftButton setTitle:@"取消" forState:UIControlStateNormal];
    if (!self.isNewUI)
    {
        [leftButton setTitleColor:textColor forState:UIControlStateNormal];
    }
    else
    {
        [leftButton setTitleColor:[self colorWithHexString:@"#157EFB"] forState:UIControlStateNormal];
    }
    
    [leftButton addTarget:self action:@selector(leftButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
    if (self.isNewUI)
    {
        leftButton.frame = CGRectMake(0, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW, USC_BUTTON_WIDTH_NEW, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        leftButton.frame = CGRectMake(5, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 5, USC_BUTTON_WIDTH, USC_BUTTON_HEIGHT);
    }
    leftButton.hidden = YES;
    
    // 右边按钮
    rightButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [rightButton setBackgroundImage:rightButtonImage forState:UIControlStateNormal];
    [rightButton setBackgroundImage:rightButtonImagePressed forState:UIControlStateHighlighted];
    [rightButton setTitle:@"说完了" forState:UIControlStateNormal];
    if (!self.isNewUI)
    {
        [rightButton setTitleColor:textColor forState:UIControlStateNormal];
    }
    else
    {
        [rightButton setTitleColor:[self colorWithHexString:@"#157EFB"] forState:UIControlStateNormal];
    }
    
    [rightButton addTarget:self action:@selector(rightButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
    if (self.isNewUI)
    {
        rightButton.frame = CGRectMake(USC_BUTTON_WIDTH_NEW - 1, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW, USC_BUTTON_WIDTH_NEW, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        rightButton.frame = CGRectMake(5 + USC_BUTTON_WIDTH, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 5, USC_BUTTON_WIDTH, USC_BUTTON_HEIGHT);
    }
    rightButton.hidden = YES;
    
    // 整个按钮
    wholeButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [wholeButton setBackgroundImage:wholeButtonImage forState:UIControlStateNormal];
    [wholeButton setBackgroundImage:wholeButtonImagePressed forState:UIControlStateHighlighted];
    [wholeButton setTitle:@"取消" forState:UIControlStateNormal];
    [wholeButton setTitleColor:textColor forState:UIControlStateNormal];
    [wholeButton addTarget:self action:@selector(wholeButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
    if (self.isNewUI)
    {
        wholeButton.frame = CGRectMake(0, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW, USC_BG_WIDTH_NEW, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        wholeButton.frame = CGRectMake(5, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 5, 552/2, USC_BUTTON_HEIGHT);
    }
    
    wholeButton.hidden = YES;
    
    // 警告提示
    if (self.isNewUI)
    {
        alertImageView = [[UIImageView alloc] init];
        alertImageView.frame = CGRectMake(0, 0, 44, 44);
        alertImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
        alertImageView.image = alertImage;
        alertImageView.hidden = YES;
    }
    else
    {
        alertImageView = [[UIImageView alloc] init];
        alertImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
        alertImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
        alertImageView.image = alertImage;
        alertImageView.hidden = YES;
    }
    
    
    // spinner bg
    if (!self.isNewUI)
    {
        spinnerBgImageView = [[UIImageView alloc] init];
        spinnerBgImageView.frame = CGRectMake(0, 0, 180/2, 180/2);
        spinnerBgImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
        spinnerBgImageView.image = spinnerBgImage;
        spinnerBgImageView.hidden = YES;
    }
    
    
    // spinner
    if (self.isNewUI)
    {
        spinnerImageView = [[UIImageView alloc] init];
        spinnerImageView.frame = CGRectMake(0, 0, 44, 44);
        spinnerImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
        spinnerImageView.image = spinnerImage;
        spinnerImageView.hidden = YES;
    }
    else
    {
        spinnerImageView = [[UIImageView alloc] init];
        spinnerImageView.frame = CGRectMake(0, 0, 180/2, 180/2);
        spinnerImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
        spinnerImageView.image = spinnerImage;
    }
    
    if (!self.isNewUI) {
        spinnerImageView.hidden = YES;
    }
    
    
    // init
    if (self.isNewUI)
    {
        initImageView = [[UIImageView alloc] init];
        initImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
        initImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
        //initImageView.hidden = YES;
    }
    else
    {
        initImageView = [[UIImageView alloc] init];
        initImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
        initImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
        //initImageView.hidden = YES;
    }
    
    
    // volumn
    if (self.isNewUI) {
        volumnImageView = [[UIImageView alloc] init];
        volumnImageView.frame = CGRectMake(0, 0, 95, 40);
        volumnImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
        volumnImageView.hidden = YES;
    }
    else
    {
        volumnImageView = [[UIImageView alloc] init];
        volumnImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
        volumnImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
        volumnImageView.hidden = YES;
    }
    
    
    // logo
    if (self.isNewUI) {
        logoImageView = [[UIImageView alloc] init];
        logoImageView.contentMode = UIViewContentModeScaleAspectFill;
        logoImageView.frame = CGRectMake((USC_BG_WIDTH_NEW - 78) / 2, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW - 20, 78, 13);
        logoImageView.image = logoImage;
    }
    else
    {
        logoImageView = [[UIImageView alloc] init];
         logoImageView.contentMode = UIViewContentModeScaleAspectFill;
        
        logoImageView.frame = CGRectMake(-63, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 25, 552/2, 30/2);
        logoImageView.image = logoImage;
    }
    
    
    
    [self addSubview:bgImageView];
    [self addSubview:titleLabel];
    [self addSubview:leftButton];
    [self addSubview:rightButton];
    [self addSubview:wholeButton];
    [self addSubview:alertImageView];
    if (!self.isNewUI)
    {
        [self addSubview:speakingLineImageView];
        [self addSubview:waitingLineImageView];
        [self addSubview:stopLineImageView];
        [self addSubview:spinnerBgImageView];
    }

    [self addSubview:spinnerImageView];
    [self addSubview:initImageView];
    [self addSubview:volumnImageView];
    [self addSubview:logoImageView];
    
}


// 左边 按钮
- (void)leftButtonPressed:(UIButton *)sender
{
    [self dismissRotateImage];
    [_delegate onButtonCancelled];
}

// 右边 按钮
- (void)rightButtonPressed:(UIButton *)sender
{
    if (sender.tag == 1) {
        [_delegate onButtonSayOver];
    }else if (sender.tag == 2){
        [_delegate onButtonSayBegin];
    }

}

// 整个 按钮
- (void)wholeButtonPressed:(UIButton *)sender
{
    [self dismissRotateImage];
    [initImageView stopAnimating];
    
    [_delegate onButtonCancelled];
}


// 旋转图片
- (void)rotateImage
{
    rotateAngle += 0.5f;
    // 防止越界
    if (rotateAngle >= 1.0E6) {
        rotateAngle = 0.0f;
    }
    
    spinnerImageView.transform = CGAffineTransformMakeRotation(rotateAngle);
}

// 关闭定时器图片旋转
- (void)dismissRotateImage
{
    if (rotateTimer) {
        [rotateTimer invalidate];
        rotateTimer = nil;
    }
    
    rotateAngle = 0.0f;
    spinnerImageView.transform = CGAffineTransformMakeRotation(0.0f);
}

// 设置音量大小变化的图片
- (void)setVolumnImage:(int)volumn
{
    if (self.isNewUI)
    {
        if (volumn <= 40) {
            volumnImageView.image = [volumnImages objectAtIndex:0];
        }else if (volumn <= 60){
            volumnImageView.image = [volumnImages objectAtIndex:1];
        }else if (volumn <= 80){
            volumnImageView.image = [volumnImages objectAtIndex:2];
        }else{
            volumnImageView.image = [volumnImages objectAtIndex:3];
        }
    }
    else
    {
        if (volumn <= 30) {
            volumnImageView.image = [volumnImages objectAtIndex:0];
        }else if (volumn <= 40){
            volumnImageView.image = [volumnImages objectAtIndex:1];
        }else if (volumn <= 50){
            volumnImageView.image = [volumnImages objectAtIndex:2];
        }else if (volumn <= 60){
            volumnImageView.image = [volumnImages objectAtIndex:3];
        }else if (volumn <= 70){
            volumnImageView.image = [volumnImages objectAtIndex:4];
        }else if (volumn <= 80){
            volumnImageView.image = [volumnImages objectAtIndex:5];
        }else if (volumn <= 90){
            volumnImageView.image = [volumnImages objectAtIndex:6];
        }else{
            volumnImageView.image = [volumnImages objectAtIndex:7];
        }
    }
    
    
}

// 正在初始化
- (void)setModeInit
{
    // 标题
    titleLabel.text = @"正在初始化...";
    
    // 初始化动画
    initImageView.animationImages = initImages;
    initImageView.animationDuration = 1;
    initImageView.animationRepeatCount = 0;
    [initImageView startAnimating];
    initImageView.hidden = NO;

    // 其他图片隐藏
    alertImageView.hidden = YES;
    spinnerImageView.hidden = YES;
    if (!self.isNewUI)
    {
        spinnerBgImageView.hidden = YES;
    }
    
    volumnImageView.hidden = YES;
    
    // 底部按钮
    wholeButton.hidden = NO;
    leftButton.hidden = YES;
    rightButton.hidden = YES;
    
    // 分割线
    if (!self.isNewUI)
    {
        speakingLineImageView.hidden = NO;
        waitingLineImageView.hidden = YES;
        stopLineImageView.hidden = YES;
    }
    

}


// 正在说话
- (void)setModeListening
{
    // 标题
    titleLabel.text = @"请开始说话";
    
    // 显示音量动画
    volumnImageView.hidden = NO;
    
    // 其他图片隐藏
    [initImageView stopAnimating];
    initImageView.hidden = YES;
    alertImageView.hidden = YES;
    spinnerImageView.hidden = YES;
    if (!self.isNewUI)
    {
        spinnerBgImageView.hidden = YES;
    }
    
    
    // 底部按钮
    wholeButton.hidden = YES;
    leftButton.hidden = NO;
    rightButton.hidden = NO;
    [rightButton setTitle:@"说完了" forState:UIControlStateNormal];
    rightButton.tag = 1;
    
    // 分割线
    if (!self.isNewUI) {
        speakingLineImageView.hidden = NO;
        waitingLineImageView.hidden = YES;
        stopLineImageView.hidden = YES;
    }
    
}


// 正在处理
- (void)setModeProcessing
{
    // 标题
    titleLabel.text = @"正在识别中...";
    
    // 中间图片
    spinnerImageView.hidden = NO;
    if (!self.isNewUI) {
        spinnerBgImageView.hidden = NO;
    }
    
    
    volumnImageView.hidden = YES;
    alertImageView.hidden = YES;
    initImageView.hidden = YES;
   
    if (rotateTimer) {
        [rotateTimer invalidate];
        rotateTimer = nil;
    }
    rotateTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(rotateImage) userInfo:nil repeats:YES];
    
    // 底部按钮
    wholeButton.hidden = NO;
    leftButton.hidden = YES;
    rightButton.hidden = YES;
    
    // 分割线
    if (!self.isNewUI) {
        speakingLineImageView.hidden = YES;
        waitingLineImageView.hidden = NO;
        stopLineImageView.hidden = YES;
    }
    
}

// 警告提示
- (void)setModePrompt:(NSString *)prompt
{
    // 标题
    titleLabel.text = prompt;
    
    // 中间图片
    initImageView.hidden = YES;
    volumnImageView.hidden = YES;
    [self dismissRotateImage];
    spinnerImageView.hidden = YES;
    
    if (!self.isNewUI)
    {
        spinnerBgImageView.hidden  = YES;
    }
    
    alertImageView.hidden = NO;
    
    // 底部按钮
    wholeButton.hidden = YES;
    leftButton.hidden = NO;
    rightButton.hidden = NO;
    [rightButton setTitle:@"再说一遍" forState:UIControlStateNormal];
    rightButton.tag = 2;
    
    // 分割线
    if (!self.isNewUI)
    {
        speakingLineImageView.hidden = YES;
        waitingLineImageView.hidden = YES;
        stopLineImageView.hidden = NO;
    }
    
}

-(UIColor *) colorWithHexString: (NSString *) stringToConvert
{
    NSString *cString = [[stringToConvert stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]] uppercaseString];
    
    // String should be 6 or 8 characters
    if ([cString length] < 6) return 0;
    
    // strip 0X if it appears
    if ([cString hasPrefix:@"0X"]) cString = [cString substringFromIndex:2];
    if ([cString hasPrefix:@"#"]) cString = [cString substringFromIndex:1];
    if ([cString length] != 6) return 0;
    // Separate into r, g, b substrings
    NSRange range;
    range.location = 0;
    range.length = 2;
    NSString *rString = [cString substringWithRange:range];
    
    range.location = 2;
    NSString *gString = [cString substringWithRange:range];
    
    range.location = 4;
    NSString *bString = [cString substringWithRange:range];
    
    // Scan values
    unsigned int r, g, b;
    [[NSScanner scannerWithString:rString] scanHexInt:&r];
    [[NSScanner scannerWithString:gString] scanHexInt:&g];
    [[NSScanner scannerWithString:bString] scanHexInt:&b];
    
    return [UIColor colorWithRed:((float) r / 255.0f)
                           green:((float) g / 255.0f)
                            blue:((float) b / 255.0f)
                           alpha:1.0f];
}

@end
