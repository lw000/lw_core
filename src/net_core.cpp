#include "net_core.h"

#include <assert.h>
#include <string.h>

#include "net_package.h"
#include "log4z.h"

static const lw_int32 C_NETHEAD_SIZE = sizeof(NetHead);

//////////////////////////////////////////////////////////////////////////////////////////

NetCore::NetCore() {

}

NetCore::~NetCore() {

}

int NetCore::send(int cmd, void* object, int objectSize, std::function<int(LW_NET_MESSAGE* p)> func) {
	
	int c = -1;
	{
		NetPackage* msg = new NetPackage(cmd, object, objectSize);
		LW_NET_MESSAGE netMsg;
		netMsg.cmd = cmd;
		netMsg.buf = msg->getBuf();
		netMsg.size = msg->getSize();
		{
			c = func(&netMsg);
		}

		delete msg;
	}
	return c;
}

int NetCore::parse(const char * buf, int size, LW_PARSE_DATA_CALLFUNC callback, void* userdata)
{
	if (size <= 0) {
		LOGFMTD("size:%d less than 0.", size);
		return -1;
	}

	if (NULL == buf) {
		LOGFMTD("buf is null.");
		return -2;
	}

	if (NULL == callback) {
		LOGFMTD("callback is null.");
		return -2;
	}

/*	clock_t t = clock();*/

	{
		int cache_size = 0;
		{
			lw_lock_guard l(&_lock);
			_cacheQueue.push(const_cast<char*>(buf), size);
			cache_size = (int)_cacheQueue.size();

			if (C_NETHEAD_SIZE <= cache_size) {
				do
				{				
					char * package = nullptr;
					NetHead *nh = (NetHead*)_cacheQueue.front();
					if (cache_size < nh->size) {
						LOGFMTD("not a complete data packet [cache_size:%d, head_size:%d, cmd:%d]", cache_size, nh->size, nh->size, nh->cmd);
						LOGD(nh->debug());
						break;
					}

					package = new char[nh->size];
					_cacheQueue.front_copyto(package, nh->size);
					_cacheQueue.pop(nh->size);
					cache_size = (int)_cacheQueue.size();

					NetHead *phd = (NetHead*)package;

					if (phd->size < 0) {
						LOGD(phd->debug());
					}
					NetPackage* nmsg = new NetPackage(phd);
					if (nullptr != nmsg) {
						char* tbuf = &package[C_NETHEAD_SIZE];
						int tbuf_len = phd->size - C_NETHEAD_SIZE;
						nmsg->setMessage(tbuf, tbuf_len);
						if (callback != nullptr) {
							callback(nmsg->getHead()->cmd, nmsg->getBuf(), nmsg->getSize(), userdata);
						}
					}
					delete nmsg;

					if (package != nullptr) {
						delete package;
					}
				} while (cache_size >= C_NETHEAD_SIZE);
			}
			else {
				LOGFMTD("not a complete data packet [cache_size:%d head_size:%d]", cache_size, C_NETHEAD_SIZE);
			}
		}
	}	
	
// 	clock_t t1 = clock();
// 	{
// 		LOGFMTD("NetCore::parse time [%f]", ((double)t1 - t) / CLOCKS_PER_SEC);
// 	}

	return 0;
}
