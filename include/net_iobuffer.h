#ifndef __net_core_h__
#define __net_core_h__

#include <functional>
#include "cache_queue.h"
#include "lock.h"


#define SOCKET_CALLBACK(__selector__,__target__, ...) std::bind(&__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__)

typedef void(*PARSE_DATA_CALLFUNC)(int cmd, char* buf, int bufsize, void* userdata);

struct NET_MESSAGE
{
	int cmd;
	char* buf;
	int size;
};

class NetIOBuffer
{
public:
	NetIOBuffer();
	~NetIOBuffer();

public:
	int send(int cmd, void* object, int objectSize, std::function<int(NET_MESSAGE* p)> func);
	int parse(const char * buf, int size, PARSE_DATA_CALLFUNC call, void* userdata);
	
private:
	NetIOBuffer(const NetIOBuffer&);
	NetIOBuffer& operator=(const NetIOBuffer&);

private:
	CacheQueue	_cacheQueue;
	lw_fast_lock _rlock;
/*	lw_fast_lock _wlock;*/
};

#endif // !__net_core_h__


