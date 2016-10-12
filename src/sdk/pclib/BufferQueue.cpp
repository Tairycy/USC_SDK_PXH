#include "BufferQueue.h"
#include <stdio.h>
#include <string.h>

BufferQueue::BufferQueue(void)
	:all_cache_length(0), buffer_max_length(0)
{
#ifdef TEST_BUFFERQUQUQ
	push_num = 0;
	drag_num = 0;
#endif
}


BufferQueue::~BufferQueue(void)
{
	clear();
}

bool BufferQueue::push(cacheBuffer* cb)
{
	if (NULL == cb)
		return false;
	
#ifdef TEST_BUFFERQUQUQ
	push_num ++;
	printf("push\t:%d\n", push_num);
#endif

	while (all_cache_length + cb->length> buffer_max_length)
	{
		pop();
#ifdef TEST_BUFFERQUQUQ
		drag_num++;
		printf("pop\t:%d\n", drag_num);
#endif
	}
	BQ.push(cb);
	all_cache_length += cb->length;
	return true;
}

bool BufferQueue::push_copy(char* chBuff, int len)
{
	if (NULL == chBuff)
		return false;
	if (len > buffer_max_length)
		return false;
	
#ifdef TEST_BUFFERQUQUQ
	push_num ++;
	printf("push\t:%d\n", push_num);
#endif

	cacheBuffer* cb_tmp = new cacheBuffer();
	cb_tmp->buffer = new char[len];
	cb_tmp->length = len;
	memcpy(cb_tmp->buffer, chBuff, len);

	return push(cb_tmp);
}

bool BufferQueue::reset()
{
	while (!BQ.empty())
	{
		pop();
	}
	return true;
}

bool BufferQueue::pop()
{
	if (BQ.size() <= 0)
	{
		return false;
	}
	cacheBuffer* tmp = BQ.front();
	BQ.pop();
	delete[] tmp->buffer;
	tmp->buffer = NULL;
	all_cache_length -= tmp->length;
	return true;
}

bool BufferQueue::isNull()
{
	return BQ.empty();
}

bool BufferQueue::clear()
{
	while (pop());
	all_cache_length = 0;
	return true;
}

cacheBuffer* BufferQueue::get_front()
{
	if (BQ.size() <= 0)
	{
		return NULL;
	}
	cacheBuffer* tmp = BQ.front();
	return tmp;
}

cacheBuffer* BufferQueue::pop_front()
{
	cacheBuffer* tmp = get_front();
	if (tmp != NULL)
	{
		BQ.pop();
		all_cache_length -= tmp->length;
	}
	return tmp;
}

bool BufferQueue::set_buffer_byte_size(int len)
{
	buffer_max_length = len;
	return true;
}

