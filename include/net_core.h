#ifndef __net_core_h__
#define __net_core_h__

#include <functional>
#include "cache_queue.h"
#include "lock.h"

using namespace lwstar;

#define SOCKET_CALLBACK(__selector__,__target__, ...) std::bind(&__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__)

typedef void(*LW_PARSE_DATA_CALLFUNC)(int cmd, char* buf, int bufsize, void* userdata);

struct LW_NET_MESSAGE
{
	int cmd;
	char* buf;
	int size;
};

class NetCore
{
public:
	NetCore();
	~NetCore();

public:
	int send(int cmd, void* object, int objectSize, std::function<int(LW_NET_MESSAGE* p)> func);
	int parse(const char * buf, int size, LW_PARSE_DATA_CALLFUNC callback, void* userdata);
	
private:
	NetCore(const NetCore&);
	NetCore& operator=(const NetCore&);

private:
	CacheQueue	_cacheQueue;
	lw_fast_lock _lock;
};

#endif // !__net_core_h__


