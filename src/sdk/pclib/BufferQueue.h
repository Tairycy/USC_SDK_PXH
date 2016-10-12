#ifndef __BUFFERQUEUE_H__
#define __BUFFERQUEUE_H__


//#define TEST_BUFFERQUQUQ

#include <queue>

struct cacheBuffer{
	char *buffer;
	int length;
};

class BufferQueue
{

private:
	int all_cache_length;
	int buffer_max_length;
	
#ifdef TEST_BUFFERQUQUQ
	int push_num;
	int drag_num;
#endif

	std::queue<cacheBuffer *> BQ;


public:
	BufferQueue(void);
	~BufferQueue(void);
	
	/*
	create a new memory, and push.
	*/
	bool push_copy(char* chBuff, int len);
	bool push(cacheBuffer* cb);
	bool reset();
	bool isNull();
	bool clear();

	/*
	Warning !!!  Be careful of memory !!!
	*/
	cacheBuffer* get_front();

	/*
	Warning !!!  Be careful of memory !!!
	*/
	cacheBuffer* pop_front();
	
	/*
	free the front one 's memory and pop.
	*/
	bool pop();
	
	bool set_buffer_byte_size(int len);
	
};

#endif