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
��Ҫ����˵����
1. MINBACKENGH���ò�����һ����������ֵ��ֻ�г����������ֵ�������Żᱻ��Ϊ�����Σ�
                ����ֻӰ��������ʼ״̬�������Ƿ��ֹ���������ֵ��
2. VOICEPROBTH����������ֵ��0 ~ 1 ֮�䣬������ڡ�����������ĸ���ֵ������������
                ʱ����ĸ�Щ����0.95������������ʱ�����Щ����0.7��
3. MAXSILLEN��  ����ӳ٣����ֵԽС������������м�ضϣ�Խ�������������������
                �̵�����ʽ������Щ����30���Ƚϳ��������������Щ����50/60��
4. USEPEAK��    3�ּ��г�����㷨��0��1��2����ֵ��2Ϊ���µ�������⣻
5. PITCHSTNUMTH, ������ʼ״̬��Ҫ������г�����߾��ﵽҪ��MINSIGLEN��Ҫ��������
                 ��������ֵVOICEPROBTH���ٶ���֡����PITCHSTNUMTH��ָг�����ٴﵽ
				 ����֡,Խ����������Խ�࣬��Խ�������������ʧ��
6. PITCHENDNUMTH,��ָ�ڼ�⵽��������������֡δ��⵽г���ͷ�������ֹͣ״̬��
                 ֵԽ��Խ��������������ضϣ�����������������
7. PITCHLASTTH�� ��6���ʹ�ã�г�����������֡����Ϊ��������г����ֵԽ��Խ�����ų�
                 ��������Խ���׽ض�������
8. PITCHTH��     г������ֵ����VOICEPROBTH���ƣ�Խ��Խ�����ų������������׽ض�������
                 ֻ��USEPEAK��0��1������Ч��һ��0���ýϸߣ�1���ýϵͣ�����������
				 ��Ҫ��СЩ��
9. USE_FIX_LOW_VAD�������份�������㷨���ٶȿ죬����г�������Ƶ����㣻
10.RESET_VEND��  ��⵽������ֹʱ������reset����ʹ��������ã�
11.RESET_FEND��  ���¿���¼��������reset����ʹ��������ã�
********************************************************************************/