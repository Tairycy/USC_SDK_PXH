#ifndef _VAD_PARAS_
#define _VAD_PARAS_
// VAD_STATUS
enum {
	SILENCE = 0, // silence status
	END_VOICE,   // voice end
	ON_VOICE,    // voice persist
	ST_VOICE,    // voice start
	DO_NOTHING   // don't konw what
};

// VAD_PARA_TYPES
enum {
	MINBACKENG = 5,// min back energy, default 5e6 (float)
	MINBACKENGH,   // min back energy higher TH, just larger than this, may voice occur, 5e8(for very noise, if voice is good, set it low, like 5e6) (float)
	PITCHTH,       // pitch threshold, 0.7 (float)
	PITCHSTNUMTH,  // pitch persist length for start usage, 8 (int)
	PITCHENDNUMTH, // pitch drop length for end usage, 50 (int)
	LOWHIGHTH,     // high freq energy vs low freq energy, 0.8 (float)
	MINSIGLEN,     // min signal length for speech, 10 (int)
	MAXSILLEN,     // max silence length, 30 (int)
	SINGLEMAX,     // max single point max in spectral, 50 (float)
	NOISE2YTH,     // gloable noise to signal value threshold, 0.5 (float)
	NOISE2YTHVOWEL,// gloable noise to signal value threshold for vowel part, 0.1 (float)
	VOICEPROBTH,   // voice freq domain prob Th, 0.7 (float)
	USEPEAK,       // use pitch or peak, 0 for pitch, 1 for peak,1 (int)
	NOISE2YST,     // noise to y ratio, start point in freq domain, 10 (int)
	RESET_FEND,    // reset type, use this when a file is end, or a voice stream is end.
	RESET_VEND,    // reset type, use this when detect a voice end point.
	PITCHLASTTH,   // the pitch last number for speech re-apear, which for transition section or tail, 3 (int)
	USE_FIX_LOW_VAD, // use the fix version, very fast, but not so good version, it is for not noise enviroment, 0 (int) not use, 1 means use it
	CHECKQUALITY   // if do speech quality check
};

#endif

/*******************************************************************************\
重要参数说明：
1. MINBACKENGH，该参数是一个绝对能量值，只有超过这个能量值的语音才会被判为语音段，
                但其只影响语音开始状态，语音是否截止不依赖这个值；
2. VOICEPROBTH，语音概率值，0 ~ 1 之间，其表征在“能量”层面的概率值，语音质量好
                时可设的高些，如0.95，语音质量差时可设低些，如0.7；
3. MAXSILLEN，  最大延迟，这个值越小容易造成语音中间截断，越大容易引入多余杂音，
                短的命令词建议设短些，如30，比较长的语音建议设大些，如50/60；
4. USEPEAK，    3种检测谐波的算法，0、1、2设置值，2为最新的连续检测；
5. PITCHSTNUMTH, 语音开始状态需要能量和谐波两者均达到要求，MINSIGLEN是要求能量连
                 续超过阈值VOICEPROBTH至少多少帧，而PITCHSTNUMTH是指谐波至少达到
				 多少帧,越大屏蔽噪声越多，但越容易造成语音丢失；
6. PITCHENDNUMTH,是指在检测到语音后，连续多少帧未检测到谐波就返回语音停止状态，
                 值越大越不容易造成语音截断，但容易引入噪声；
7. PITCHLASTTH， 与6配合使用，谐波检测连续几帧才认为是真正有谐波，值越大越容易排除
                 噪声，但越容易截断语音；
8. PITCHTH，     谐波门限值，与VOICEPROBTH类似，越大越容易排除噪声，但容易截断语音，
                 只对USEPEAK的0、1设置有效，一般0设置较高，1设置较低，语音质量差
				 需要设小些；
9. USE_FIX_LOW_VAD，是用冷唤醒那套算法，速度快，不做谐波检测与频域计算；
10.RESET_VEND，  检测到语音截止时，调用reset函数使用这个配置；
11.RESET_FEND，  重新开启录音，调用reset函数使用这个配置；
********************************************************************************/