#import "USCPlayThread.h"
#import <mach/mach.h>
#import <sys/sysctl.h>
#import "USCTTSErrorCode.h"

#define kBgQueue dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)

@interface USCPlayThread()
{
  @private
    NSMutableArray *receiveDataArray; // audio data array
    BOOL isPlay; // if audioqueue start,isPlay = yes,
    USCPlayStatus playStatus;
    NSMutableData *everAudioData;
    BOOL isCanceled;  // 状态，当前播放是否被外部停止取消
    //音频参数
    AudioStreamBasicDescription audioDescription;
    // 音频播放队列
    AudioQueueRef audioQueue;
    // 音频缓存
    AudioQueueBufferRef audioQueueBuffers[QUEUE_BUFFER_SIZE];
    int bufferOverCount;
    // 表示要播放的语音数据是否全部传递完成
    BOOL synthesizeFinish;
}

@property (nonatomic,strong) NSMutableData *allData;
@end

#pragma mark class implementation

@implementation USCPlayThread

#pragma mark -
#pragma mark - public method
- (instancetype)init
{
    if (self = [super init]) {
        receiveDataArray = [[NSMutableArray alloc]init];
        isPlay = NO;
        bufferOverCount = QUEUE_BUFFER_SIZE;
        audioQueue = nil;
        playStatus = PlayStatusReady;
        everAudioData = [[NSMutableData alloc]initWithLength:0];
        isCanceled = NO;
        synthesizeFinish = NO;
        _allData = [[NSMutableData alloc]init];
    }
    return self;
}

- (USCPlayStatus)playStatus
{
    return playStatus;
}

- (void)requestStop
{
    @synchronized(self)
    {
        isCanceled = YES;
    }
}

- (void)pause
{
    if (playStatus != PlayStatusPlaying)
        return;

    OSStatus error = AudioQueuePause(audioQueue);
    playStatus = PlayStatusPaused;

    if(self.delegate && [self.delegate respondsToSelector:@selector(playPause)])
        [self.delegate playPause];
}
    - (void)resume
{
    if (playStatus != PlayStatusPaused)
        return;

    AudioQueueStart(audioQueue, NULL);
    playStatus = PlayStatusPlaying;

    if (self.delegate && [self.delegate respondsToSelector:@selector(playResume)]) {
        [self.delegate playResume];
    }
}

-(void)addPlayData:(NSData *)data
{
    int count = 0;
    @synchronized(receiveDataArray){
               [receiveDataArray addObject:data];
        count = (int)receiveDataArray.count;
    }

    /* 当队列中有三个数据就开始播放 */
    if(isPlay == NO && count >= QUEUE_BUFFER_SIZE) {
        [self startPlay];
    }
}

- (int)openAudioOut
{
    return 0;
}

- (int)writeData:(NSData *)buffer size:(int)size
{
//    [USCLog log_d:@"%s ,size=%d",__func__,size];
    if (size == -1) {
        [self synthesizeFinish];
    }
    
    [self addPlayData:buffer];
    return 0;
}

- (void)closeAudioOut
{
    [self requestStop];
}

#pragma mark -
#pragma mark private method
/*!
 *  @brief  表示全部语音数据传递完成
 *
 */
- (void)synthesizeFinish
{
    @synchronized(self)
    {
        synthesizeFinish = YES;
    }
}

// audio queue callback function
static void BufferCallback(void *inUserData,AudioQueueRef inAQ,AudioQueueBufferRef buffer)
{
    @autoreleasepool {
    USCPlayThread* player=(__bridge USCPlayThread*)inUserData;
    [player fillBuffer:inAQ queueBuffer:buffer];
    // 暂时不用
    //    if([player playStatus] == PlayStatusEnd)
    //    {
    //        [player disposeQueue];
    //    }
   }
}

// fill buffer
-(void)fillBuffer:(AudioQueueRef)queue queueBuffer:(AudioQueueBufferRef)buffer
{
    while (true)
    {
        // 0.is canceled ,break
        if (isCanceled) {
            bufferOverCount --;
            break;
        }

        @autoreleasepool
        {
            @synchronized(receiveDataArray){
                if(receiveDataArray.count > 0){
                    [everAudioData appendData:[receiveDataArray objectAtIndex:0]];
                    [self.allData appendData:[receiveDataArray objectAtIndex:0]];
                    [receiveDataArray removeObjectAtIndex:0];
                }
            }

            // 1. hava audio data play
            if (everAudioData.length > 0) {
                memcpy(buffer->mAudioData, [everAudioData bytes] , everAudioData.length);
                buffer->mAudioDataByteSize = (UInt32)everAudioData.length;
                AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
                break;
            }// 2.no audio data and tranfer not complete ,wait
            else if(everAudioData.length == 0 && synthesizeFinish == NO)
                [NSThread sleepForTimeInterval:0.05];
            // 3.no audio data and tranfer complete ,break
            else
            {
                bufferOverCount --;
                break;
            }
        }
    } // while

    // clear everAudioData
    [everAudioData resetBytesInRange:NSMakeRange(0, everAudioData.length)];
    [everAudioData setLength:0];

    if(bufferOverCount == 0){

        // stop audioqueue
        [self stopAudioQueue];
        dispatch_sync(dispatch_get_main_queue(), ^{
            NSError *error =  nil;
            if (isCanceled) {
                NSString *msg = [USCTTSErrorCode getErrorMsg:SYNTHESIZE_CANCLEED];
                error = [NSError errorWithDomain:msg code:SYNTHESIZE_CANCLEED userInfo:nil];
            }
            if ([self.delegate respondsToSelector:@selector(playEnd:)]) {
                NSString *savePath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Documents"] stringByAppendingPathComponent:@"audio.pcm"];
                [self.allData writeToFile:savePath atomically:YES];
                [self.delegate playEnd:error];
            }
        });
    }
}

//  start audioqueue
- (void)startPlay
{
//    [USCLog log_usc:@"%s",__func__];

    [self reset];

    [self open];

    for(int i=0; i<QUEUE_BUFFER_SIZE; i++)
    {
        [self fillBuffer:audioQueue queueBuffer:audioQueueBuffers[i]];
    }

    if ([self.delegate respondsToSelector:@selector(playBegin)])
        [self.delegate playBegin];

    // audioqueuestart
    OSStatus error  =  AudioQueueStart(audioQueue, NULL);
    if (error != 0)
    {
        NSError *error = [NSError errorWithDomain:@"play error" code:-1 userInfo:nil];
        if ([self.delegate respondsToSelector:@selector(playEnd:)]) {
            [self.delegate playEnd:error];
        }
        return;
    }
    playStatus = PlayStatusPlaying;

    @synchronized(self){
        isPlay = YES;
    }
}

-(void)createAudioQueue
{
    if(audioQueue)
        return;

    AudioQueueNewOutput(&audioDescription, BufferCallback, (__bridge void *)(self), nil, nil, 0, &audioQueue);
    if(audioQueue)
        for(int i=0;i<QUEUE_BUFFER_SIZE;i++){
            AudioQueueAllocateBuffer(audioQueue, EVERY_READ_LENGTH, &audioQueueBuffers[i]);
        }
}

-(void)stopAudioQueue
{
    if(audioQueue == nil)
        return;

    @synchronized(self)
    {
        playStatus = PlayStatusEnd;
        isPlay = NO;
        synthesizeFinish = NO;
    }
    AudioQueueStop(audioQueue, TRUE);
}

-(void)setAudioFormat
{
    audioDescription.mSampleRate = 16000;
    audioDescription.mFormatID = kAudioFormatLinearPCM;
    audioDescription.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioDescription.mChannelsPerFrame = 1;
    audioDescription.mFramesPerPacket = 1;
    audioDescription.mBitsPerChannel = 16;
    audioDescription.mBytesPerFrame = (audioDescription.mBitsPerChannel/8) * audioDescription.mChannelsPerFrame;
    audioDescription.mBytesPerPacket = audioDescription.mBytesPerFrame ;
}

-(void)close
{
    if (audioQueue) {
        AudioQueueStop(audioQueue, true);
        AudioQueueDispose(audioQueue, true);
        audioQueue = nil;
        isPlay = NO;
    }
}

-(BOOL)open
{
    if([self isOpen])
        return YES;

    [self close];
    [self setAudioFormat];
    [self createAudioQueue];
    return YES;
}

-(BOOL)isOpen
{
    return (audioQueue != nil);
}

// reset property
- (void)reset
{
    bufferOverCount = QUEUE_BUFFER_SIZE;
    isCanceled = NO;
    playStatus = PlayStatusReady;
    synthesizeFinish = NO;
}

- (BOOL)isPlaying
{
    return isPlay;
}

- (void)disposeQueue
{
    if (audioQueue) {
        AudioQueueDispose(audioQueue, TRUE);
        audioQueue = nil;
    }
    playStatus = PlayStatusReady;
}

- (void)dealloc
{
    [self disposeQueue];
}

vm_size_t usedMemory(void) {
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
    return (kerr == KERN_SUCCESS) ? info.resident_size : 0; // size in bytes
}

vm_size_t freeMemory(void) {
    mach_port_t host_port = mach_host_self();
    mach_msg_type_number_t host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    vm_size_t pagesize;
    vm_statistics_data_t vm_stat;

    host_page_size(host_port, &pagesize);
    (void) host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size);
    return vm_stat.free_count * pagesize;
}

void logMemUsage(void) {
    // compute memory usage and log if different by >= 100k
    static long prevMemUsage = 0;
    long curMemUsage = usedMemory();
    long memUsageDiff = curMemUsage - prevMemUsage;

    if (memUsageDiff > 1000 || memUsageDiff < -1000) {
        prevMemUsage = curMemUsage;
       // NSLog(@"Memory used %7.1f (%+5.0f), free %7.1f kb", curMemUsage/1000.0f, memUsageDiff/1000.0f, freeMemory()/1000.0f);
    }
}
@end
