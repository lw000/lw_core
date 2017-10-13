#ifndef __cache_queue_h__
#define __cache_queue_h__

#include "common_type.h"
#include <vector>
#include <queue>
#include <stddef.h>

class CacheQueue
{
public:
	CacheQueue(void);
	~CacheQueue(void);

public:
	int push(char* buf, int size);
	void pop(int size);

public:
	size_t size() const;
	char* front();
	void front_copyto(char* buffer, int size);
	std::vector<char>* front_copyto(std::vector<char>& dest, int size);
	void clear();

private:
	std::vector<char> _cq;
};

#endif // !__cache_queue_h__

