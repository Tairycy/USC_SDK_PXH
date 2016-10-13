//
//  USCRecognizeView.m
//  asr_old_vad_project
//
//  Created by yunzhisheng-zy on 15-1-10.
//  Copyright (c) 2015年 usc. All rights reserved.
//

#import "USCRecognizeDialog.h"
#import "UIView+USCExtension.h"
#import "USCRecognizer.h"
#import "USCErrorCode.h"

@interface USCRecognizeDialog()<USCRecognizerDelegate>
{
    NSString *appKey;
    USCRecognizer *recognizer;
    NSMutableString *lastResult;
    NSTimeInterval lastTime;
}
// 背景
@property (nonatomic,weak) UIImageView *bgImageView;
@property (nonatomic,strong) UIImage *bgImage;

// 标题
@property (nonatomic,weak) UILabel *titleLabel;

// 左边按钮
@property (nonatomic,weak) UIButton *leftBtn;
@property (nonatomic,strong) UIImage *leftBtnImage;
@property (nonatomic,strong) UIImage *leftBtnImagePressed;

// 右边按钮
@property (nonatomic,weak) UIButton *rightBtn;
@property (nonatomic,strong) UIImage *rightBtnImage;
@property (nonatomic,strong) UIImage *rightBtnImagePressed;

// 整个按钮
@property (nonatomic,weak) UIButton *wholeBtn;
@property (nonatomic,strong) UIImage *wholeBtnImage;
@property (nonatomic,strong) UIImage *wholeBtnImagePressed;

// 警告图片
@property (nonatomic,strong) UIImageView *alertImageView;
@property (nonatomic,strong) UIImage *alertImage;

// 说话时的分割线
@property (nonatomic,weak) UIImageView *speakingLineImageView;
@property (nonatomic,strong) UIImage *speakingLineImage;

// 等待时的分割线
@property (nonatomic,weak) UIImageView *waitingLineImageView;
@property (nonatomic,strong) UIImage *waitingLineImage;

// 停止时的分割线
@property (nonatomic,weak) UIImageView *stopLineImageView;
@property (nonatomic,strong) UIImage *stopLineImage;

// spinner
@property (nonatomic,weak) UIImageView *spinnerBgImageView;
@property (nonatomic,strong) UIImage *spinnerBgImage;
@property (nonatomic,weak) UIImageView *spinnerImageView;
@property (nonatomic,strong) UIImage *spinnerImage;

// 初始化动画图片
@property (nonatomic,strong) NSMutableArray *initalImages;
@property (nonatomic,weak) UIImageView *initalImageView;

// 音量动画图片
@property (nonatomic,strong) NSMutableArray *volumnImages;
@property (nonatomic,weak) UIImageView *volumnImageView;

// 字体颜色
@property (nonatomic,strong) UIColor *textColor;

// 定时器
@property (nonatomic,strong) NSTimer *rotateTimer;
@property (nonatomic,assign) float rotateAngle;

// logo
@property (nonatomic,weak) UIImageView *logoImageView;
@property (nonatomic,strong) UIImage *logoImage;

@property (nonatomic,weak) UIActivityIndicatorView *activity;

//判断是新UI，还是旧UI
@property (nonatomic,assign) BOOL isNewUI;

@end

@implementation USCRecognizeDialog

#pragma mark - public method
- (id)initWithAppkey:(NSString *)appKey center:(CGPoint)point
{
    if (self = [super init]) {

        // 1.创建asr对象
        recognizer = [[USCRecognizer alloc] initWithAppKey:appKey secret:nil];
        recognizer.delegate = self;

        // 2.
        self.size = CGSizeMake(USC_BG_WIDTH_NEW, USC_BG_HEIGHT_NEW);
        CGPoint origin = [self orginWithSize:self.size andCenter:point];

        
        self.x = origin.x;
        self.y = origin.y;
    }
    return self;
}

- (CGPoint)orginWithSize:(CGSize)size andCenter:(CGPoint)point
{
    CGFloat width = size.width;
    CGFloat height = size.height;
    CGFloat x = point.x - width / 2;
    CGFloat y = point.y - height / 2;

    return CGPointMake(x, y);
}

/*!
 *  @author usc_zy, 15-01-12 17:01:20
 *
 *  @brief   第一步：人点击按钮显示这个view，并经过初始化开始录音
 *
 */
- (void)start
{
    [[UIApplication sharedApplication].keyWindow addSubview:self];
    self.hidden = NO;
    [self onButtonSayBegin];
}

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame]) {

        // 加载图片
        [self loadResore];

//        [self setBackgroundColor:[UIColor redColor]];

        UIImageView *bgImageView = [[UIImageView alloc]initWithImage:self.bgImage];

        self.bgImageView = bgImageView;

        // 标题
        UILabel *titleLable  = [[UILabel alloc]init];
        self.titleLabel = titleLable;
        self.titleLabel.textAlignment = UITextAlignmentCenter;
        self.titleLabel.textAlignment = NSTextAlignmentCenter;
        self.titleLabel.textColor = self.textColor;
        self.titleLabel.backgroundColor = [UIColor clearColor];

        // 说话时的分割线
        if (!self.isNewUI) {
            UIImageView *speakingLineImageView = [[UIImageView alloc]init];
            self.speakingLineImageView = speakingLineImageView;
            speakingLineImageView.image = self.speakingLineImage;
        }

        // 等待时的分割线
        UIImageView *waitingLineImageView = [[UIImageView alloc] init];
        self.waitingLineImageView = waitingLineImageView;
        waitingLineImageView.image = self.waitingLineImage;
        waitingLineImageView.hidden = YES;


        // 停止时的分割线
        UIImageView *stopLineImageView = [[UIImageView alloc] init];
        self.stopLineImageView = stopLineImageView;
        stopLineImageView.image = self.stopLineImage;
        stopLineImageView.hidden = YES;


        // 左边按钮
        UIButton *leftBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        self.leftBtn = leftBtn;
        [leftBtn setBackgroundImage:self.leftBtnImage forState:UIControlStateNormal];
        [leftBtn setBackgroundImage:self.leftBtnImagePressed forState:UIControlStateHighlighted];
        [leftBtn setTitle:@"取消" forState:UIControlStateNormal];
        if (!self.isNewUI)
        {
            [leftBtn setTitleColor:self.textColor forState:UIControlStateNormal];
        }
        else
        {
            [leftBtn setTitleColor:[self colorWithHexString:@"#157EFB"] forState:UIControlStateNormal];
        }
        
        [leftBtn addTarget:self action:@selector(leftButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
        leftBtn.hidden = YES;

        // 右边按钮
        UIButton *rightBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        self.rightBtn = rightBtn;
        [rightBtn setBackgroundImage:self.rightBtnImage forState:UIControlStateNormal];
        [rightBtn setBackgroundImage:self.rightBtnImagePressed forState:UIControlStateHighlighted];
        [rightBtn setTitle:@"说完了" forState:UIControlStateNormal];
        if (!self.isNewUI)
        {
            [rightBtn setTitleColor:self.textColor forState:UIControlStateNormal];
        }
        else
        {
            [rightBtn setTitleColor:[self colorWithHexString:@"#157EFB"] forState:UIControlStateNormal];
        }

        [rightBtn addTarget:self action:@selector(rightButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
        rightBtn.hidden = YES;
//
        // 整个按钮
         UIButton *wholeButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.wholeBtn =wholeButton;
        [wholeButton setBackgroundImage:self.wholeBtnImage forState:UIControlStateNormal];
        [wholeButton setBackgroundImage:self.wholeBtnImagePressed forState:UIControlStateHighlighted];
        [wholeButton setTitle:@"取消" forState:UIControlStateNormal];
        [wholeButton setTitleColor:self.textColor forState:UIControlStateNormal];
        [wholeButton addTarget:self action:@selector(wholeButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
        wholeButton.hidden = YES;

        // 警告提示
        UIImageView *alertImageView = [[UIImageView alloc] init];
        self.alertImageView = alertImageView;
        self.alertImageView.image = self.alertImage;
        self.alertImageView.hidden = YES;

        // spinner bg
        if (!self.isNewUI)
        {
            UIImageView *spinnerBgImageView = [[UIImageView alloc] init];
            self.spinnerBgImageView = spinnerBgImageView;
            spinnerBgImageView = [[UIImageView alloc] init];
            spinnerBgImageView.image = self.spinnerBgImage;
            spinnerBgImageView.hidden = YES;
        }

        // spinner
        UIImageView *spinnerImageView = [[UIImageView alloc] init];
        self.spinnerImageView = spinnerImageView;
        spinnerImageView.image = self.spinnerImage;

        // activity
        UIActivityIndicatorView *activity = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
        self.activity = activity;

        if (!self.isNewUI) {
            self.spinnerImageView.hidden = YES;
        }

        // init
        UIImageView *initImageView = [[UIImageView alloc] init];
        self.initalImageView = initImageView;

        // volumn
        UIImageView *volumnImageView = [[UIImageView alloc] init];
        self.volumnImageView = volumnImageView;
        volumnImageView.hidden = YES;
//
        // logo
        UIImageView *logoImageView = [[UIImageView alloc] init];
        self.logoImageView = logoImageView;
        logoImageView.contentMode = UIViewContentModeScaleAspectFill;
        logoImageView.image = self.logoImage;

        [self addSubview:bgImageView];

        [self addSubview:self.titleLabel];
        [self addSubview:leftBtn];
        [self addSubview:rightBtn];
        [self addSubview:wholeButton];

        [self addSubview:self.alertImageView];
        
        if (!self.isNewUI)
        {
            [self addSubview:self.speakingLineImageView];
            [self addSubview:waitingLineImageView];
            [self addSubview:stopLineImageView];
            [self addSubview:self.spinnerBgImageView];
        }

//        [self addSubview:self.spinnerImageView];

        [self addSubview:self.activity];

        [self addSubview:self.initalImageView];
        [self addSubview:self.volumnImageView];
        [self addSubview:self.logoImageView];
    }
    return self;
}

- (void)layoutSubviews
{
    [super layoutSubviews];

    [self.bgImageView setSize:self.size];

    if (self.isNewUI)
    {
        self.titleLabel.frame = CGRectMake(0, 5, USC_BUTTON_WIDTH_NEW * 2, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        self.titleLabel.frame = CGRectMake(0, 5, USC_BUTTON_WIDTH * 2, USC_BUTTON_HEIGHT);
    }

    self.speakingLineImageView.frame = CGRectMake(5, 5, 552/2, 76/2);

    // 等待时的分割线
    self.waitingLineImageView.frame = CGRectMake(5, 5, 552/2, 76/2);

    self.stopLineImageView.frame = CGRectMake(5, 5, 552/2, 76/2);

    if (self.isNewUI)
    {
        self.leftBtn.frame = CGRectMake(0, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW, USC_BUTTON_WIDTH_NEW, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        self.leftBtn.frame = CGRectMake(5, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 5, USC_BUTTON_WIDTH, USC_BUTTON_HEIGHT);
    }

    // 右边按钮
    if (self.isNewUI)
    {
     self.rightBtn.frame = CGRectMake(USC_BUTTON_WIDTH_NEW - 1, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW, USC_BUTTON_WIDTH_NEW, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        self.rightBtn.frame = CGRectMake(5 + USC_BUTTON_WIDTH, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 5, USC_BUTTON_WIDTH, USC_BUTTON_HEIGHT);
    }

    if (self.isNewUI)
    {
        self.wholeBtn.frame = CGRectMake(0, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW, USC_BG_WIDTH_NEW, USC_BUTTON_HEIGHT_NEW);
    }
    else
    {
        self.wholeBtn.frame = CGRectMake(5, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 5, 552/2, USC_BUTTON_HEIGHT);
    }

    // 警告提示
    if (self.isNewUI)
    {
        self.alertImageView.frame = CGRectMake(0, 0, 44, 44);
        self.alertImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
    }
    else
    {
       self.alertImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
       self.alertImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
    }

    // spinner bg
    if (!self.isNewUI)
    {
        self.spinnerBgImageView.frame = CGRectMake(0, 0, 180/2, 180/2);
        self.spinnerBgImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
    }

    // spinner
    if (self.isNewUI)
    {
        self.spinnerImageView.frame = CGRectMake(0, 0, 44, 44);
        self.spinnerImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
        self.activity.frame = CGRectMake(0, 0, 44, 44);
        self.activity.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
    }
    else
    {
        self.spinnerImageView.frame = CGRectMake(0, 0, 180/2, 180/2);
        self.spinnerImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);

        self.activity.frame = CGRectMake(0, 0, 180/2, 180/2);
        self.activity.center =  CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
    }

    // init
    if (self.isNewUI)
    {
        self.initalImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
        self.initalImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
    }
    else
    {
        self.initalImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
        self.initalImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
    }


    // volumn
    if (self.isNewUI) {
        self.volumnImageView.frame = CGRectMake(0, 0, 95, 40);
        self.volumnImageView.center = CGPointMake(USC_BG_WIDTH_NEW/2, USC_BG_HEIGHT_NEW/2);
    }
    else
    {
        self.volumnImageView.frame = CGRectMake(0, 0, 120/2, 262/2);
        self.volumnImageView.center = CGPointMake(USC_BG_WIDTH/2, USC_BG_HEIGHT/2);
    }


    // logo
    if (self.isNewUI) {
        self.logoImageView.frame = CGRectMake((USC_BG_WIDTH_NEW - 78) / 2, USC_BG_HEIGHT_NEW - USC_BUTTON_HEIGHT_NEW - 20, 78, 13);
    }
    else
    {
        self.logoImageView.frame = CGRectMake(-63, USC_BG_HEIGHT - USC_BUTTON_HEIGHT - 25, 552/2, 30/2);
    }
}

- (void)loadResore
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
    self.bgImage = [UIImage imageWithData:bgImageDecodeData];

    // 左边按钮图片
    NSString *leftButtonImagePath = [resPath stringByAppendingPathComponent:@"left_button.usc"];
    NSData *leftButtonImageEncodeData = [NSData dataWithContentsOfFile:leftButtonImagePath];
    NSData *leftButtonImageDecodeData = [self decodeImage:leftButtonImageEncodeData];
    self.leftBtnImage = [UIImage imageWithData:leftButtonImageDecodeData];

    // 左边选中按钮图片
    NSString *leftButtonImagePressedPath  = [resPath stringByAppendingPathComponent:@"left_button_pressed.usc"];
    NSData *leftButtonImagePressedEncodeData = [NSData dataWithContentsOfFile:leftButtonImagePressedPath];
    NSData *leftButtonImagePressedDecodeData = [self decodeImage:leftButtonImagePressedEncodeData];
    self.leftBtnImagePressed = [UIImage imageWithData:leftButtonImagePressedDecodeData];


    // 右边按钮图片
    NSString *rightButtonImagePath = [resPath stringByAppendingPathComponent:@"right_button.usc"];
    NSData *rightButtonImageEncodeData = [NSData dataWithContentsOfFile:rightButtonImagePath];
    NSData *rightButtonImageDecodeData = [self decodeImage:rightButtonImageEncodeData];
    self.rightBtnImage = [UIImage imageWithData:rightButtonImageDecodeData];


    // 右边选中按钮图片
    NSString *rightButtonImagePressedPath = [resPath stringByAppendingPathComponent:@"right_button_pressed.usc"];
    NSData *rightButtonImagePressedEncodeData = [NSData dataWithContentsOfFile:rightButtonImagePressedPath];
    NSData *rightButtonImagePressedDecodeData = [self decodeImage:rightButtonImagePressedEncodeData];
    self.rightBtnImagePressed = [UIImage imageWithData:rightButtonImagePressedDecodeData];


    // 整个按钮图片
    NSString *wholeButtonImagePath = [resPath stringByAppendingPathComponent:@"whole_button.usc"];
    NSData *wholeButtonImageEncodeData = [NSData dataWithContentsOfFile:wholeButtonImagePath];
    NSData *wholeButtonImageDecodeData = [self decodeImage:wholeButtonImageEncodeData];
    self.wholeBtnImage = [UIImage imageWithData:wholeButtonImageDecodeData];


    // 整个按钮选中图片
    NSString *wholeButtonImagePressedPath = [resPath stringByAppendingPathComponent:@"whole_button_pressed.usc"];
    NSData *wholeButtonImagePressedEncodeData = [NSData dataWithContentsOfFile:wholeButtonImagePressedPath];
    NSData *wholeButtonImagePressedDecodeData = [self decodeImage:wholeButtonImagePressedEncodeData];
    self.wholeBtnImagePressed = [UIImage imageWithData:wholeButtonImagePressedDecodeData];

    // 警告图片
    NSString *alertImagePath = [resPath stringByAppendingPathComponent:@"alert.usc"];
    NSData *alertImageEncodeData = [NSData dataWithContentsOfFile:alertImagePath];
    NSData *alertImageDecodeData = [self decodeImage:alertImageEncodeData];
    self.alertImage = [UIImage imageWithData:alertImageDecodeData];


    // 说话时的分割线图片
    if (!self.isNewUI) {
        NSString *speakingLineImagePath = [resPath stringByAppendingPathComponent:@"speaking_line.usc"];
        NSData *speakingLineImageEncode = [NSData dataWithContentsOfFile:speakingLineImagePath];
        NSData *speakingLineImageDecode = [self decodeImage:speakingLineImageEncode];
        self.speakingLineImage = [UIImage imageWithData:speakingLineImageDecode];
    }

    // 等待时的分割线图片
    NSString *waitingLineImagePath = [resPath stringByAppendingPathComponent:@"waiting_line.usc"];
    NSData *waitingLineImageEncodeData = [NSData dataWithContentsOfFile:waitingLineImagePath];
    NSData *waitingLineImageDecodeData = [self decodeImage:waitingLineImageEncodeData];
    self.waitingLineImage = [UIImage imageWithData:waitingLineImageDecodeData];


    // 停止时的分割线图片
    NSString *stopLineImagePath = [resPath stringByAppendingPathComponent:@"stop_line.usc"];
    NSData *stopLineImageEncodeData = [NSData dataWithContentsOfFile:stopLineImagePath];
    NSData *stopLineImageDecodeData = [self decodeImage:stopLineImageEncodeData];
    self.stopLineImage = [UIImage imageWithData:stopLineImageDecodeData];

    // spinner 背景图片
    if (!self.isNewUI) {
        NSString *spinnerBgImagePath = [resPath stringByAppendingPathComponent:@"spinner_bg.usc"];
        NSData *spinnerBgImageEncodeData = [NSData dataWithContentsOfFile:spinnerBgImagePath];
        NSData *spinnerBgImageDecodeData = [self decodeImage:spinnerBgImageEncodeData];
        self.spinnerBgImage = [UIImage imageWithData:spinnerBgImageDecodeData];
    }

    // spinner 图片
    NSString *spinnerImagePath = [resPath stringByAppendingPathComponent:@"spinner.usc"];
    NSData *spinnerImageEncodeData = [NSData dataWithContentsOfFile:spinnerImagePath];
    NSData *spinnerImageDecodeData = [self decodeImage:spinnerImageEncodeData];
    self.spinnerImage = [UIImage imageWithData:spinnerImageDecodeData];


    // 初始化动画图片
    self.initalImages = [[NSMutableArray alloc] init];
    for (int i = 1; i <= 6; i++) {
        NSString *initImageFileName = [NSString stringWithFormat:@"init%i.usc",i];
        NSString *initImageFilePath = [resPath stringByAppendingPathComponent:initImageFileName];
        NSData *initImageFileEncodeData = [NSData dataWithContentsOfFile:initImageFilePath];
        NSData *initImageFileDecodeData = [self decodeImage:initImageFileEncodeData];
        [self.initalImages addObject:[UIImage imageWithData:initImageFileDecodeData]];
    }

    // 音量动画图片
    self.volumnImages = [[NSMutableArray alloc] init];
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
        [self.volumnImages addObject:[UIImage imageWithData:volImageFileDecodeData]];
    }


    // logo图片
    NSString *logoImagePath = [resPath stringByAppendingPathComponent:@"logo.usc"];
    NSData *logoImageEncodeData = [NSData dataWithContentsOfFile:logoImagePath];
    NSData *logoImageDecodeData = [self decodeImage:logoImageEncodeData];
    self.logoImage = [UIImage imageWithData:logoImageDecodeData];

    // 文字颜色
    NSString *textColorPath = [resPath stringByAppendingPathComponent:@"textcolor.usc"];
    NSData *textColorEncodeData = [NSData dataWithContentsOfFile:textColorPath];
    NSData *textColorDecodeData = [self decodeImage:textColorEncodeData];
    NSString *textColorStr = [[NSString alloc] initWithData:textColorDecodeData encoding:NSUTF8StringEncoding];
    NSArray *textColorArray = [textColorStr componentsSeparatedByString:@" "];
    float red = [textColorArray[0] floatValue];
    float green = [textColorArray[1] floatValue];
    float blue = [textColorArray[2] floatValue];
    self.textColor = [UIColor colorWithRed:red green:green blue:blue alpha:1.0];

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

-(UIColor *) colorWithHexString: (NSString *)stringToConvert
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

- (void)setModeInit
{
    // 标题
    self.titleLabel.text = @"正在初始化...";

    // 初始化动画
    self.initalImageView.animationImages = self.initalImages;
    self.initalImageView.animationDuration = 1;
    self.initalImageView.animationRepeatCount = 0;
    [self.initalImageView startAnimating];
    self.initalImageView.hidden = NO;

    // 其他图片隐藏
    self.alertImageView.hidden = YES;
    self.spinnerImageView.hidden = YES;
    if (!self.isNewUI)
    {
        self.spinnerBgImageView.hidden = YES;
    }

    self.volumnImageView.hidden = YES;

    // 底部按钮
    self.wholeBtn.hidden = NO;
    self.leftBtn.hidden = YES;
    self.rightBtn.hidden = YES;

    // 分割线
    if (!self.isNewUI)
    {
        self.speakingLineImageView.hidden = NO;
        self.waitingLineImageView.hidden = YES;
        self.stopLineImageView.hidden = YES;
    }
}

// 正在说话
- (void)setModeListening
{
    // 标题
    self.titleLabel.text = @"请开始说话";

    // 显示音量动画
    self.volumnImageView.hidden = NO;

    // 其他图片隐藏
    [self.initalImageView stopAnimating];
    self.initalImageView.hidden = YES;
    self.alertImageView.hidden = YES;
    self.spinnerImageView.hidden = YES;
    if (!self.isNewUI)
    {
        self.spinnerBgImageView.hidden = YES;
    }

    // 底部按钮
    self.wholeBtn.hidden = YES;
    self.leftBtn.hidden = NO;
    self.rightBtn.hidden = NO;
    [self.rightBtn setTitle:@"说完了" forState:UIControlStateNormal];
    self.rightBtn.tag = 1;

    // 分割线
    if (!self.isNewUI) {
        self.speakingLineImageView.hidden = NO;
        self.waitingLineImageView.hidden = YES;
        self.stopLineImageView.hidden = YES;
    }
}

// 正在处理
- (void)setModeProcessing
{
    // 标题
    self.titleLabel.text = @"正在识别中...";

    // 中间图片
    self.spinnerImageView.hidden = NO;
    if (!self.isNewUI) {
        self.spinnerBgImageView.hidden = NO;
    }

    self.volumnImageView.hidden = YES;
    self.alertImageView.hidden = YES;
    self.initalImageView.hidden = YES;

    if (self.rotateTimer) {
        [self.rotateTimer invalidate];
        self.rotateTimer = nil;
    }
//    self.rotateTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(rotateImage) userInfo:nil repeats:YES];

    // 底部按钮
    self.wholeBtn.hidden = NO;
    self.leftBtn.hidden = YES;
    self.rightBtn.hidden = YES;

    // 分割线
    if (!self.isNewUI) {
        self.speakingLineImageView.hidden = YES;
        self.waitingLineImageView.hidden = NO;
        self.stopLineImageView.hidden = YES;
    }

    [self.activity startAnimating];
}

// 警告提示
- (void)setModePrompt:(NSString *)prompt
{
    // 标题
    self.titleLabel.text = prompt;

    // 中间图片
    self.initalImageView.hidden = YES;
    self.volumnImageView.hidden = YES;
    [self dismissRotateImage];
    self.spinnerImageView.hidden = YES;

    if (!self.isNewUI)
    {
        self.spinnerBgImageView.hidden  = YES;
    }

    self.alertImageView.hidden = NO;

    // 底部按钮
    self.wholeBtn.hidden = YES;
    self.leftBtn.hidden = NO;
    self.rightBtn.hidden = NO;
    [self.rightBtn setTitle:@"再说一遍" forState:UIControlStateNormal];
    self.rightBtn.tag = 2;

    // 分割线
    if (!self.isNewUI)
    {
        self.speakingLineImageView.hidden = YES;
        self.waitingLineImageView.hidden = YES;
        self.stopLineImageView.hidden = NO;
    }
}

// 左边 按钮
- (void)leftButtonPressed:(UIButton *)sender
{
    [self dismissRotateImage];
    [self onButtonCancelled];
}

// 右边 按钮
- (void)rightButtonPressed:(UIButton *)sender
{
    if (sender.tag == 1) {
        [self onButtonSayOver];
    }else if (sender.tag == 2){
        [self onButtonSayBegin];
    }
}

// 整个 按钮
- (void)wholeButtonPressed:(UIButton *)sender
{
    [self dismissRotateImage];

    [self.initalImageView stopAnimating];

    [self  onButtonCancelled];
}

// 旋转图片
- (void)rotateImage
{
    self.rotateAngle += 0.5f;
    // 防止越界
    if (self.rotateAngle >= 1.0E6) {
        self.rotateAngle = 0.0f;
    }
    self.spinnerImageView.transform = CGAffineTransformMakeRotation(self.rotateAngle);
}

// 关闭定时器图片旋转
- (void)dismissRotateImage
{
    if (self.rotateTimer) {
        [self.rotateTimer invalidate];
        self.rotateTimer = nil;
    }

    self.rotateAngle = 0.0f;
//    self.spinnerImageView.transform = CGAffineTransformMakeRotation(0.0f);
}

// 设置音量大小变化的图片
- (void)setVolumnImage:(int)volumn
{
    if (self.isNewUI)
    {
        if (volumn <= 40) {
            self.volumnImageView.image = [self.volumnImages objectAtIndex:0];
        }else if (volumn <= 60){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:1];
        }else if (volumn <= 80){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:2];
        }else{
            self.volumnImageView.image = [self.volumnImages objectAtIndex:3];
        }
    }
    else
    {
        if (volumn <= 30) {
            self.volumnImageView.image = [self.volumnImages objectAtIndex:0];
        }else if (volumn <= 40){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:1];
        }else if (volumn <= 50){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:2];
        }else if (volumn <= 60){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:3];
        }else if (volumn <= 70){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:4];
        }else if (volumn <= 80){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:5];
        }else if (volumn <= 90){
            self.volumnImageView.image = [self.volumnImages objectAtIndex:6];
        }else{
            self.volumnImageView.image = [self.volumnImages objectAtIndex:7];
        }
    }
}

#pragma mark - 按钮事件
- (void)onButtonSayBegin
{
    NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
    double diff = startTime - lastTime;
    lastTime = startTime;
    if (diff <= 0.5) {
        [self setModePrompt:@"操作时间太频繁"];
        if (self.delegate && [self.delegate respondsToSelector:@selector(onEnd:)])
        {
            NSString *msg = [USCErrorCode getErrorMsg:OPERATION_TOO_FAST];
            [self.delegate onEnd:[NSError errorWithDomain:msg code:OPERATION_TOO_FAST userInfo:nil]];
        }
        return;
    }

    [lastResult deleteCharactersInRange:NSMakeRange(0, lastResult.length)];
    [self setModeInit];

    // 3.开始识别
    [recognizer start];
}


- (void)onButtonCancelled
{
    [recognizer cancel];

    self.hidden = YES;
}

- (void)onButtonSayOver
{
    [self setModeProcessing];
    [recognizer stop];
}


#pragma mark - asr delegate
- (void)onStart
{
    [self setModeListening];
}

- (void)onUpdateVolume:(int)volume
{
    [self setVolumnImage:volume];

}

- (void)onEnd:(NSError *)error
{
    [self.activity stopAnimating];

    if (self.delegate && [_delegate respondsToSelector:@selector(onEnd:)])
    {
        [_delegate onEnd:error];
    }

    if (error) {
        [self setModePrompt:error.domain];
    }else{

        if (lastResult.length == 0) {
            [self setModePrompt:@"没有听到声音"];
        }else{
            [self dismissRotateImage];
            self.hidden = YES;
        }
    }
}
- (void)onVADTimeout
{
    [self setModeProcessing];
    [recognizer stop];
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

- (void)onResult:(NSString *)result isLast:(BOOL)isLast
{
    if (_delegate && [_delegate respondsToSelector:@selector(onResult: isLast:)])
    {
        [_delegate onResult:result isLast:isLast];
    }

    if (result) {

        [lastResult appendString:result];
    }
}

#pragma mark - public method

- (void)cancel
{
    [self onButtonCancelled];
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
// 设置采样率，支持8k和16k，默认为16k
// */
//- (void)setSampleRate:(int)rate
//{
//    [recognizer setSampleRate:rate];
//}
//
//- (void)setLanguage:(NSString *)language
//{
//    [recognizer setLanguage:language];
//}
//
//- (void)setPunctuation:(BOOL)isEnable
//{
//    [recognizer setPunctuation:isEnable];
//}
//
///*
// 设置识别参数
// */
//- (BOOL)setEngine:(NSString *)engine
//{
//    return [recognizer setEngine:engine];
//}
//
//+ (NSString *)getVersion
//{
//    return [USCRecognizer getVersion];
//}
//
////设置是否允许播放提示音
//- (void) setPlayingBeep:(BOOL)isAllowed
//{
//    [recognizer setPlayingBeep:isAllowed];
//}

@end

