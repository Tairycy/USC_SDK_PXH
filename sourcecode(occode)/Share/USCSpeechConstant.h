//
//  USCSpeechConstant.h
//  USC_ASR_nlu_USC_TTS
//
//  Created by iOSDeveloper-zy on 15-6-2.
//  Copyright (c) 2015年 usc. All rights reserved.
//


/**
 * 公共常量
 *
 * @author unisound Copyright (c) 2015, unisound.com All Rights Reserved.
 */
    // 常量定义
    // USC_ASR & NLU-> 1
    // USC_TTS -> 2
    // WakeUp -> 3
    // VPR -> 4

    // key ->0
    // event ->1
    // result ->2
    // error ->3
    // release type ->4

    // Example:
    // USC_ASR event 10xx
    // USC_ASR event 11xx
    // USC_ASR result 12xx
    // USC_ASR error 13xx

#pragma mark -
#pragma mark Option
    /**
     * 识别模式。如云端识别、本地识别、云+端的混合模式
     */
     static const  int USC_ASR_SERVICE_MODE = 1001; // 如云端识别、本地识别、云+端的混合模式。
    // 服务模式，如用户可以在无网络的情况下选择本地识别。

    /**
     * 在线识别带宽
     */
      static     const int USC_ASR_BANDWIDTH = 1002; // 在线识别带宽

    /**
     * 远近讲语言模型<br>
     */
      static     const int USC_ASR_VOICE_FIELD = 1003;// 远近讲语言模型

    /**
     * 语言
     */
      static    const int USC_ASR_LANGUAGE = 1004;// 取值
    // "chinese"中文，"english"英文，"cantonese"粤语

    /**
     * 预留，暂不支持 方言
     */
      static   const int USC_ASR_DIALECT = 1005;

    /**
     * 预留，目前只支持opus 音频编解码算法名称，用户可以根据网络状况等实际情况选择合适的压缩等级。
     */
      static   const int USC_ASR_AUDIO_ENCODE = 1006;// 音频编解码算法名称，用户可以根据网络状况等实际情况选择合适的压缩等级。

    /**
     * 不同领域有不同的词汇集，指明所属领域更有利于提高识别率
     */
      static   const int USC_ASR_DOMAIN = 1008; // 不同领域有不同的词汇集，指明所属领域更有利于提高识别率

    /**
     * 识别服务器地址
     */
      static    const int USC_ASR_SERVER_ADDR = 1009;

    /**
     * 设置vad前段超时
     */
      static    const int USC_ASR_VAD_TIMEOUT_FRONTSIL = 1010;

    /**
     * 设置vad后端超时
     */
      static    const int USC_ASR_VAD_TIMEOUT_BACKSIL = 1011;

    /**
     * 设置识别sessionid
     */
      static  const int USC_ASR_SESSION_ID = 1012;

    /**
     * 设置唤醒词
     */
      static    const int USC_ASR_WAKEUP_WORD = 1013;

    /**
     * 设置网络交互超时
     */
      static    const int USC_ASR_NET_TIMEOUT = 1014;

    /**
     * 设置是否结果是否返回标点符号
     */
    static    const int USC_ASR_NET_PNUCTUATION = 1015;

    /**
     *  设置recognizer param
     */
    static const int  USC_ASR_RECOGNIZER_PARAM = 1016;

    /**
     *  设置是否开启定位
     */
    static const int  USC_ASR_LOCATION_ENABLE = 1017;

    /**
     * 同步请求语义结果
     */
      static    const int USC_NLU_ENABLE = 1020;

    /**
     * 设置语义识别场景
     */
      static    const int USC_NLU_SCENARIO = 1021;
    /**
     * 设置语义服务器
     */
      static    const int USC_NLU_SERVER_ADDR = 1022;

    /**
     * 设置历史
     */
      static    const int USC_GENERAL_HISTORY = 1030;

    /**
     * 设置城市
     */
      static    const int USC_GENERAL_CITY = 1031;

    /**
     * 设置voiceid
     */
      static    const int  USC_GENERAL_VOICEID = 1032;

    /**
     * 设置GPS信息
     */
      static    const int  USC_GENERAL_GPS = 1033;

    /**
     * 设置时间
     */
      static    const int  USC_GENERAL_TIME = 1034;

    /**
     * 设置屏幕dpi
     */
      static   const int USC_GENERAL_DPI = 1035;
  
    /**
     * 设置识别udid
     */
      static    const int USC_GENERAL_UDID = 1036;

    /**
     * 设置imei
     */
      static    const int USC_GENERAL_IMEI = 1037;

    /**
     * 设置appname
     */
      static    const int USC_GENERAL_APP_NAME = 1038;

    /**
     * 设置carrier
     */
      static    const int USC_GENERAL_CARRIER = 1039;

    /**
     * 设置phonemodel
     */
      static    const int USC_GENERAL_PHONE_MODEL = 1040;

    /**
     * 设置操作系统
     */
      static    const int USC_GENERAL_PHONE_OS = 1041;

    /**
     * 设置操作系统版本
     */
      static    const int USC_GENERAL_PHONE_OS_VERSION = 1042;

    /**
     * 设置设备网络类型
     */
      static    const int USC_GENERAL_PHONE_NETWORK = 1043;

    /**
     * 采样率
     */
      static    const int USC_ASR_SAMPLING_RATE = 1044; // 采样率

    /**
     * TAG
     */
      static    const int USC_ASR_OPT_ENGINE_TAG = 1050;

    /**
     * 是否过滤 如不过滤，显示离线标签
     */
      static    const int USC_ASR_OPT_RESULT_FILTER = 1051;

    /**
     *
     */
      static    const int USC_ASR_OPT_PCM_DATA = 1052;

    /**
     * 设置录音是否可用
     */
      static    const int USC_ASR_OPT_RECORDING_ENABLED = 1053;

    /**
     * 打印log]
     */
      static    const int USC_ASR_OPT_PRINT_LOG = 1054;

    /**
     * 录音不暂停
     */
      static    const int USC_ASR_OPT_FIX_USC_ASR_CONTINUOUS = 1055;

    /**
     * 前端 vad是否可用
     */
      static    const int USC_ASR_OPT_FRONT_VAD_ENABLED = 1056;

    /**
     *
     */
      static    const int USC_ASR_OPT_LOG_LISTENER = 1057;

    /**
     * 保存录音数据，传入路径
     */
      static    const int USC_ASR_OPT_SAVE_RECORDING_DATA = 1058;

    /**
     * 离线结果转为 json
     */
      static    const int USC_ASR_OPT_RESULT_JSON = 1059;

    /**
     * 释放引擎类型,释放整个引擎
     */
      static    const int USC_ASR_RELEASE_ENGINE = 1401;
    /**
     * 释放引擎类型,释放用户字典
     */
      static    const int USC_ASR_RELEASE_USERDICT = 1402;
    /**
     * 释放引擎类型,释放语法
     */
      static    const int USC_ASR_RELEASE_GRAMMAR = 1403;
    /**
     * 释放引擎类型,释放词表
     */
      static    const int USC_ASR_RELEASE_VOCAB = 1404;

    /**
     * 识别模式 USC_ASR_SERVICE_MODE可选项 USC_ASR_SERVICE_MODE_LOCAL 本地识别
     */
      static    const int USC_ASR_SERVICE_MODE_LOCAL = 2;
    /**
     * 识别模式 USC_ASR_SERVICE_MODE可选项 USC_ASR_SERVICE_MODE_NET 在线识别
     */
      static    const int USC_ASR_SERVICE_MODE_NET = 1;
    /**
     * 识别模式 USC_ASR_SERVICE_MODE可选项 USC_ASR_SERVICE_MODE_MIX 在线离线混合识别
     */
      static    const int USC_ASR_SERVICE_MODE_MIX = 0;

    /**
     * 语言模型  USC_ASR_VOICE_FIELD可选项  VOICE_FIELD_FAR 远讲
     */
      static      NSString *USC_VOICE_FIELD_FAR = @"far";
    /**
     * 语言模型  USC_ASR_VOICE_FIELD可选项  VOICE_FIELD_NEAR 近讲
     */
      static      NSString *USC_VOICE_FIELD_NEAR = @"near";

    /**
     * 语言  USC_ASR_LANGUAGE可选项 LANGUAGE_MANDARIN 普通话
     */
      static      NSString *USC_LANGUAGE_MANDARIN = @"mandarin";

    /**
     * 语言  USC_ASR_LANGUAGE可选项 LANGUAGE_CANTONESE 粤语
     */
      static      NSString *USC_LANGUAGE_CANTONESE = @"cantonese";

    /**
     * 语言  USC_ASR_LANGUAGE可选项 LANGUAGE_ENGLISH 英语
     */
      static      NSString *USC_LANGUAGE_ENGLISH = @"english";

    /**
     * 是否同步请求语义 NLU_ISENABLE可选项 NLU_ISENABLE_ENABLE 不同步请求语义
     */
      static      NSString *USC_NLU_ISENABLE_DISABLE = @"nlu_isEnable_disable";

    /**
     * USC_TTS使用到的key 20
     */
    /**
     * 设置合成语速spd
     */
      static    const int USC_TTS_KEY_VOICE_SPEED = 2001;
    /**
     * 设置合成音高
     */
      static    const int USC_TTS_KEY_VOICE_PITCH = 2002;
    /**
     * 设置合成音量vol
     */
      static    const int USC_TTS_KEY_VOICE_VOLUME = 2003;
    /**
     * 设置合成码率
     */
      static    const int USC_TTS_KEY_SAMPLE_RATE = 2004;
    /**
     * 设置合成角色vcn
     */
      static    const int USC_TTS_KEY_VOICE_NAME = 2005;

    /**
     * 设置在线离线合成类型(net,fix)
     */
    static    const int USC_TTS_KEY_TYPE = 2006;

    /**
     * 设置前端点时间
     */
    static    const int TTS_KEY_FRONT_SILENCE = 2007;

    /**
     * 设置后端点时间
     */
    static   const int TTS_KEY_BACK_SILENCE = 2008;

    /**
     * 设置合成领域
     */
      static    const int USC_TTS_KEY_FIELD = 2010;

    /**
     * 设置USC_TTS服务器地址
     */
      static    const int USC_TTS_KEY_SERVER_ADDR = 2011;

    /**
     * 设置播放开始延迟时间
     */
      static    const int USC_TTS_KEY_PLAY_START_BUFFER_TIME = 2012;

    /**
     * 设置语音采样率
     */
      static    const int USC_TTS_KEY_STREAM_TYPE = 2013;

    /**
     * VPR服务器地址
     */
      static    const int USC_VPR_SERVER_ADDR = 4001;

    /*
     * 1.往服务器传数据多久传递一次params 2.网络交互超时
     */

    /*
     * 设置识别领域 SpeechConstants.USC_ASR_ENGINE  设置在线识别带宽
     * SpeechConstants.USC_ASR_BANDWIDTH 设置远近讲 SpeechConstants.USC_ASR_VOICE_FIELD
     * 可选项为VOICE_FIELD_NEAR、VOICE_FIELD_FAR 默认为VOICE_FIELD_NEAR 设置识别语言
     * SpeechConstants.LANGUAGE  设置语义理解场景 SpeechConstants.NLU_SCENARIO
     *  设置语义解析服务器 SpeechConstants.NLU_SERVER server=ip:port
     * 例如："http://192.168.1.1:80" 语义云平台返回的历史信息
     * SpeechConstants.NLU_HISTORY 设置语义解析城市信息 SpeechConstants.NLU_CITY
     * ,参看城市Cnstants->TODO 设置语义日志ID SpeechConstants.NLU_VOICEID
     */

    /**
     * USC_TTS onEvent 类型 21
     */
    /**
     * USC_TTS onEvent type 初始化成功事件
     */
         static const int USC_TTS_EVENT_INIT = 2101;

    /**
     * USC_TTS onEvent type 开始合成事件
     */
         static const int USC_TTS_EVENT_SYNTHESIZE_START = 2102;
    /**
     * USC_TTS onEvent type 结束合成事件
     */
         static const int USC_TTS_EVENT_SYNTHESIZE_END = 2103;

    /**
     * USC_TTS onEvent type 开始缓冲
     */
      static    const int USC_TTS_EVENT_BUFFER_BEGIN = 2104;
    /**
     * USC_TTS onEvent type 缓冲就绪
     */
      static    const int USC_TTS_EVENT_BUFFER_READY = 2105;

    /**
     * USC_TTS onEvent type 开始播放事件
     */
         static const int USC_TTS_EVENT_PLAYING_START = 2106;
    /**
     * USC_TTS onEvent type 结束播放事件
     */
         static const int USC_TTS_EVENT_PLAYING_END = 2107;

    /**
     * USC_TTS onEvent type 暂停事件
     */
         static const int USC_TTS_EVENT_PAUSE = 2108;
    /**
     * USC_TTS onEvent type 恢复事件
     */
         static const int USC_TTS_EVENT_RESUME = 2109;

    /**
     * USC_TTS onEvent type 取消事件
     */
         static const int USC_TTS_EVENT_CANCEL = 2110;
    /**
     * USC_TTS onEvent type 停止事件
     */
         static const int USC_TTS_EVENT_STOP = 2111;

    /**
     * USC_TTS onEvent type 释放事件（引擎、模型、用户字典）
     */
         static const int USC_TTS_EVENT_RELEASE = 2112;

    /**
     * USC_TTS onEvent type 模型加载
     */
      static    const int USC_TTS_EVENT_MODEL_LOAD = 2113;

    /**
     * USC_s result 22开始(USC_TTS 暂无)
     */

    /**
     * USC_TTS error 23开始
     */
      static    const int USC_TTS_ERROR = 2301;

    /**
     * USC_TTS release 24开始
     */
    /**
     * USC_TTS release type 释放引擎
     */
      static    const int USC_TTS_RELEASE_ENGINE = 2401;
    /**
     * USC_TTS release type 释放模型
     */
      static    const int USC_TTS_RELEASE_MODEL_DATA = 2402;
    /**
     * USC_TTS release type 释放用户
     */
      static    const int USC_TTS_RELEASE_USERDICT = 2403;
#pragma mark -
#pragma mark asr event
    /**
     * USC_ASR onEvent type 开始录音
     */
      static    const int USC_ASR_EVENT_RECORDING_START = 1101;
    /**
     * USC_ASR onEvent type 结束录音
     */
      static    const int USC_ASR_EVENT_RECORDING_STOP = 1102;
    /**
     * USC_ASR onEvent type VAD 超时
     */
      static    const int USC_ASR_EVENT_VAD_TIMEOUT = 1103;
    /**
     * USC_ASR onEvent type 检测到说话
     */
      static    const int USC_ASR_EVENT_SPEECH_DETECTED = 1104;
    /**
     * USC_ASR onEvent type 说话停止
     */
      static    const int USC_ASR_EVENT_SPEECH_END = 1105;
//    /**
//     * USC_ASR onEvent type 声音改变
//     */
      static    const int USC_ASR_EVENT_FX_VOLUME_CHANGE = 1106;
    /**
     * USC_ASR onEvent type VAD 识别结束
     */
      static    const int USC_ASR_EVENT_RECOGNIZITION_END = 1107;

    /**
     * USC_ASR onEvent type 用户数据上传结束
     */
      static    const int USC_ASR_EVENT_USERDATA_UPLOADED = 1108;

    /**
     * USC_ASR onEvent type 用户grammar模型编译结束
     */
      static    const int USC_ASR_EVENT_GRAMMAR_COMPILED = 1109;

    /**
     * USC_ASR onEvent type 用户grammar模型加载结束
     */
      static    const int USC_ASR_EVENT_GRAMMAR_LOADED = 1110;

    /**
     * USC_ASR onEvent type 用户grammar模型插入host grammar模型结束,可以开始识别
     */
      static    const int USC_ASR_EVENT_GRAMMAR_INSERTED = 1111;

    /**
     * USC_ASR onEvent type 用户词表插入host grammar模型结束,可以开始识别
     */
      static    const int USC_ASR_EVENT_VOCAB_INSERTED = 1112;

    /**
     * USC_ASR onEvent type 检测音量过大
     */
      static    const int USC_ASR_EVENT_FX_ABNORMAL_TOO_LOUD = 1113;
    /**
     * USC_ASR onEvent type 检测音量过小
     */
      static    const int USC_ASR_EVENT_FX_ABNORMAL_TOO_QUIET = 1114;
    /**
     * USC_ASR onEvent type
     */
      static    const int USC_ASR_EVENT_FX_ABNORMAL_SNR_BAD = 1115;
    /**
     * USC_ASR onEvent type
     */
      static    const int USC_ASR_EVENT_FX_ABNORMAL_NO_LEADINGSILENCE = 1116;
    /**
     * USC_ASR onEvent type 取消
     */
      static    const int USC_ASR_EVENT_CANCEL = 1117;

    /**
     * USC_ASR LOCAL onEvent type LOCAL识别结束
     */
      static    const int USC_ASR_EVENT_LOCAL_END = 1118;
    
    /**
     * USC_ASR NETonEvent type NET识别结束
     */
      static    const int USC_ASR_EVENT_NET_END = 1119;
    
    /**
     * USC_ASR onEvent type 识别结束
     */
      static    const int USC_ASR_EVENT_END = 1120;
    
    /**
     * USC_ASR_NLU onEvent type 语义理解结束
     */
      static    const int USC_ASR_NLU_EVENT_END = 1121;
    
    /**
     * USC_ASR_NLU onEvent type 语义理解结束
     */
      static    const int USC_ASR_EVENT_VOLUMECHANGE = 1122;

    /**
     *  识别超时
     */
    static    const int USC_ASR_EVENT_RECOGNITION_TIMEOUT = 1123;

    
    /**
     * USC_ASR_NLU onError type 语义理解结束
     */
      static    const int USC_ASR_NLU_ERROR = 1301;

    /**
     * USC_ASR_NLU_ERROR_RECOGNITION_TIMEOUT onError type 识别超时
     */
    static    const int USC_ASR_NLU_ERROR_RECOGNITION_TIMEOUT = 13010;


    /**
     * USC_ASR_NLU onError type 操作过于频繁
     */
    static    const int USC_ASR_NLU_ERROR_OPERATETOOFAST = 1311;

    /**
     * USC_ASR onResult type 识别在线返回
     */
      static    const int USC_ASR_RESULT_NET = 1201;
    
    /**
     * USC_ASR onResult type 识别离线理解返回
     */
      static    const int USC_ASR_RESULT_LOCAL = 1202;
    
    /**
     * USC_ASR_NLU onResult type 语义理解返回
     */
      static    const int USC_ASR_RESULT_NLU = 1210;


    /**
     * VPR onEvent type 录音开始
     */
    static    const int USC_VPR_PARAMETER= 4001;


    /**
     * VPR onEvent type 录音开始
     */
      static    const int USC_VPR_EVENT_RECORDING_START = 4101;
    /**
     * VPR onEvent type 录音结束
     */
      static    const int USC_VPR_EVENT_RECORDING_STOP = 4102;
    
    /**
     * VPR onEvent type 声音改变
     */
      static    const int USC_VPR_EVENT_VOLUME_UPDATED = 4103;
    
    /**
     * VPR onEvent type 声纹识别结束
     */
      static    const int USC_VPR_EVENT_RECOGNITION_END = 4104;

    /**
     * VPR onEvent 下载vpr音频完成
     */
    static    const int USC_VPR_EVENT_DOWNLOADAUDIO_END = 4105;

    /**
     * VPR onEvent VAD 超时
     */
    static    const int USC_VPR_EVENT_VADTIMEOUT= 4106;

    /**
     * VPR onEvent speechStart 开始说话
     */
    static    const int USC_VPR_EVENT_SpeechStart= 4107;

    /**
     * VPR onEvent 录音开始
     */
    static    const int USC_VPR_EVENT_RECORDSTART= 4108;

    /**
     * VPR onEvent 录音识别都开始了
     */
    static    const int USC_VPR_EVENT_RECOGNITION_RECORD_START= 4109;

    /**
     * VPR onEvent 录音识别都开始了
     */
    static    const int USC_VPR_EVENT_RECORD_STOP= 4110;

    /**
     * VPR onEvent 录音识别都开始了
     */
    static    const int USC_VPR_RESULT= 4201;


    /**
     * VPR onError 频繁操作
     */
    static    const int USC_VPR_ERROR_OPERATION_TOO_FAST = 4301;

    /**
     * VPR onError 下载音频出错
     */
    static    const int USC_VPR_ERROR_DOWNLOADAUDIO = 4302;

    /**
     * VPR onError 录音启动失败
     */
    static    const int USC_VPR_ERROR_RECORD_ERROR = 4303;

    /**
     * VPR onError vpr 出错,即服务器返回返回了错误信息
     */
    static    const int USC_VPR_ERROR_RESULT = 4304;

    /**
     * VPR onError vpr 出错,vpr 结果转化成json 中出错
     */
    static    const int USC_VPR_ERROR_RESULT_JSON = 4305;

/**
 * VPR onError vpr 出错,vpr 识别出错
 */
static    const int USC_VPR_ERROR_RECOGNITION = 4306;

    /**
     * WAKEUP onEvent type 录音开始,进行唤醒识别
     */
      static    const int USC_WAKEUP_EVENT_RECORDING_START = 3101;
    /**
     * WAKEUP onEvent type 录音结束,停止唤醒识别
     */
      static    const int USC_WAKEUP_EVENT_RECORDING_STOP = 3102;