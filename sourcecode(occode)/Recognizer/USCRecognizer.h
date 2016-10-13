//
//  USCRecognizer.h
//  usc
//
//  Copyright (c) 2014年 yunzhisheng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIApplication.h>
#import "USCConstant.h"
#import "USCIAudioSource.h"

@class USCRecoginizerParam;

#pragma mark -
#pragma mark 语音识别代理
@protocol USCRecognizerDelegate <NSObject>

/**
 *  事件回调
 *
 *  @param type 事件类型 如SPPEECHSTART RECORDINGSTART RECORDINGSTOP
 *  @param timeMs
 */
- (void)onEvent:(int)type timeMs:(int)timeMs;
/**
 *   错误信息
 *
 *  @param type  错误类型
 *  @param error 错误
 */
- (void)onError:(int)type error:(NSError *)error;
/**
 *  结果回调
 *
 *  @param type       结果类型.ASR_FIX_RESULT ASR_ONLINE_RESULT ASR_ONLINE_LAST_RESULT ASR_NLU_RESULT
 *  @param jsonResult 结果
 */
- (void)onResult:(int)type jsonString:(NSString *)jsonString;

@end

#pragma mark -
#pragma mark 类
// @class - 语音识别类
// @brief - 通过语音识别转换成文字
@interface USCRecognizer : NSObject

@property (nonatomic, weak) id<USCRecognizerDelegate> delegate;

/*
 初始化, 请到开发者网站http://dev.hivoice.cn申请appKey
 */
//- (id)initWithAppKey:(NSString *)appkey;
- (id)initWithAppKey:(NSString *)appkey secret:(NSString *)secret;
/**
 *   设置audiosource
 *
 *  @param audioSource
 *
 *  @return 0 表示成功， 否则返回相应错误码
 */
- (int)setAudioSource:(USCIAudioSource *)audioSource;

/**
 *
 *  启动识别引擎
 */
- (void)start;

/**
 *
 *  停止录音，并开始等待识别结束
 */
- (void)stop;
/**
 *
 *  取消识别
 */
- (void)cancel;

/**
 *  获取设置选项
 *
 *  @param key 选项对应key
 *
 *  @return 选项值
 */
- (id)getOption:(int)key;

/**
 *  设置识别属性
 *
 *  @param key   选项key
 *  @param value 选项value
 */
- (void)setOption:(int)key value:(id)value;

/**
 *  获取当前状态
 *
 *  @return 状态如READY, RECOGNIZING, etc
 */
- (int)getStatus;

/**
 *  根据json初始化ASR引擎
 *
 *  @param jsonString json
 *
 *  @return
 */
- (int)init:(NSString *)jsonString;

/**
 *  将识别参数以json格式批量输入到引擎,加入到目前识别中,当postInfo和setOption设置同一参数时,以最新调用为准 可设置参数请参考setoption
 *
 *  @param jsonString json
 *
 *  @return
 */
- (int)postInfo:(NSString *)jsonString;

/**
 *  上传用户词表，非阻塞,上传完成后回调onEvent type=　ASR_EVENT_USERDATA_UPLOADED,用户设置命令词并上传，提高识别率,如果当前为在线识别，则上传词表
 *
 *  @param userData 用户词表
 */
- (void)uploadUserData:(NSDictionary *)userData;

//
- (void)recognizeAudioFile:(NSString *)audioFilePath;

#pragma mark -
#pragma mark old interface
///**
// *  设置参数对象
// *
// *  @param param 参数
// */
//- (void)setParam:(USCRecoginizerParam *)param;
//
///*!
// *
// *  @brief  每次请求不再返回语义理解结果
// */
//- (void)disableUnderstanderResult;
//
///*!
// *
// *  @brief  设置是否返回语义理解结果，默认每次识别结束后都会返回对应的语义理解结果。
// *
// *  @param enable YES:每次返回, NO:不返回
// */
//- (void)setNluEnable:(BOOL)enable;
//
///**
// *  上传用户词表，非阻塞,上传完成后回调onEvent type=　ASR_EVENT_USERDATA_UPLOADED,用户设置命令词并上传，提高识别率,如果当前为在线识别，则上传词表
// *
// *  @param userData 用户词表
// */
//- (void)uploadUserData:(NSDictionary *)userData;
//
///**
// *  根据json初始化ASR引擎
// *
// *  @param jsonString json
// *
// *  @return
// */
//- (int)init:(NSString *)jsonString;
//
///**
// *  将识别参数以json格式批量输入到引擎,加入到目前识别中,当postInfo和setOption设置同一参数时,以最新调用为准 可设置参数请参考setoption
// *
// *  @param jsonString json
// *
// *  @return
// */
//- (int)postInfo:(NSString *)jsonString;
//
//#pragma mark -
//#pragma mark Old interface
//
///** (DEPRECATED)
// *
// *  设置识别语言
// *
// *  @param language 语言 ,这个方法已不再推荐使用
// */
//- (void)setLanguage:(NSString *)language;
///*!
// *
// *  @brief  设置识别语言
// *
// *  @param language 语言
// */
//- (void)setRecognizeLanguage:(USCRecognizeLanguage)language;
///*!
// *
// *  @brief   设置远近讲
// *
// *  @param voiceField 近讲设置成@“near”，远讲设置成@“far”
// *
// *  @return 设置成功返回YES，识别返回NO
// */
//- (BOOL)setVoiceField:(NSString *)voiceField;
///**
// *
// *   设置vad超时时间，单位ms
// *
// *  @param frontTime 开始说话之前的停顿超时时间，默认3000ms
// *  @param backTime  开始说话之后的停顿超时时间，默认1000ms
// */
//- (void)setVadFrontTimeout:(int)frontTime BackTimeout:(int)backTime;
///**
// *
// *   设置录音采样率
// *
// *  @param rate 支持8000和16000，默认为16000
// */
//- (void)setSampleRate:(int)rate;
///**
// *
// *  设置识别参数
// *
// *  @param engine 识别领域
// *
// *  @return YES or NO
// */
//- (BOOL)setEngine:(NSString *)engine;
///*!
// *
// *  @brief  设置模型类型
// *
// *  @param modelType 模型
// *
// *  @return 成功返回YES
// *
// */
//- (BOOL)setModelType:(NSString *)modelType;
///**
// *
// *  设置识别超时时间
// *
// *  @param recognizationTime 超时时间，默认30s
// */
//- (void)setRecognizationTimeout:(float)recognizationTime;
//
///*!
// *  直接识别音频文件
// *
// *  @brief  识别音频
// *
// *  @param audioFilePath 音频文件的路径
// */
//- (void)recognizeAudioFile:(NSString *)audioFilePath;
//
///**
// *
// *  获取session id
// *
// *  @return 当前识别会话ID
// */
//- (NSString *)getSessionId;
//
///*
// 设置是否允许播放提示音
// */
//-(void)setPlayingBeep:(BOOL)isAllowed;
//
///*
// 设置标点符号
// */
//- (void)setPunctuation:(BOOL)isEnable;
//
///*
// 设置属性
// */
//- (void)setProperty:(NSString *)property forKey:(int)key;
//
///*!
// *
// *  @brief  设置语义的场景，默认为空
// *
// *  @param scenario 语义场景
// */
//- (void)setNluScenario:(NSString *)scenario;
//
///*!
// *  @author usc_zy, 15-04-10 16:04:12
// *
// *  @brief  是否开启log
// *
// *  @param show
// */
//+ (void)showLog:(BOOL)show;
//
///*!
// *  @author usc_zy, 15-04-10 16:04:25
// *
// *  @brief  log等级
// *
// *  @param level 
// */
//+ (void)setLogLevel:(int)level;
//
//+ (void)showLog;
///*
// 版本号
// */
//+ (NSString *)getVersion;
//
///*!
// *
// *  @brief  设置离线模型路径
// *
// *  @param modelPath 离线模型路径
// */
//+ (void)setModelPath:(NSString *)modelPath;
//
@end
