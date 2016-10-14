#import "USCPlayThread.h"
#import <mach/mach.h>
#import <sys/sysctl.h>

#define kBgQueue dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0)

#define QUEUE_BUFFER_SIZE  3//队列缓冲个数
#define EVERY_READ_LENGTH  8000 //每次从文件读取的长度


@interface USCPlayThread()
{
    NSMutableArray *receiveDataArray; // audio data array
    BOOL isPlay;
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

#pragma mark - public

- (int)openAudioOut
{
    return 0;
}

- (int)writeData:(NSData *)buffer size:(int)size
{
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

}

- (void)resume
{
    if (playStatus != PlayStatusPaused)
        return;
    AudioQueueStart(audioQueue, NULL);
    playStatus = PlayStatusPlaying;
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

- (void)synthesizeFinish
{
    @synchronized(self)
    {
        synthesizeFinish = YES;
    }
}

static void BufferCallback(void *inUserData,AudioQueueRef inAQ,AudioQueueBufferRef buffer)
{
    @autoreleasepool {
    USCPlayThread* player=(__bridge USCPlayThread*)inUserData;
    [player fillBuffer:inAQ queueBuffer:buffer];
   }
}

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

            if (everAudioData.length > 0) {
                memcpy(buffer->mAudioData, [everAudioData bytes] , everAudioData.length);
                buffer->mAudioDataByteSize = (UInt32)everAudioData.length;
                AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
                break;
            }
            else if(everAudioData.length == 0 && synthesizeFinish == NO)
                [NSThread sleepForTimeInterval:0.05];
            else
            {
                bufferOverCount --;
                break;
            }
        }
    } // while

    [everAudioData resetBytesInRange:NSMakeRange(0, everAudioData.length)];
    [everAudioData setLength:0];

    if(bufferOverCount == 0){

        // stop audioqueue
        [self stopAudioQueue];
        dispatch_sync(dispatch_get_main_queue(), ^{
            NSError *error =  nil;
            if (isCanceled) {
            }

        });
    }
}

- (void)startPlay
{
    [self reset];
    [self open];
    for(int i=0; i<QUEUE_BUFFER_SIZE; i++)
    {
        [self fillBuffer:audioQueue queueBuffer:audioQueueBuffers[i]];
    }

    OSStatus error  =  AudioQueueStart(audioQueue, NULL);
    if (error != 0)
    {
        NSError *error = [NSError errorWithDomain:@"play error" code:-1 userInfo:nil];
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
@end
