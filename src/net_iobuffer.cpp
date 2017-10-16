#include "net_iobuffer.h"

#include <assert.h>
#include <string.h>

#include "net_package.h"
#include "log4z.h"

static const int C_NETHEAD_SIZE = sizeof(NetHead);

//////////////////////////////////////////////////////////////////////////////////////////

NetIOBuffer::NetIOBuffer() {

}

NetIOBuffer::~NetIOBuffer() {

}

int NetIOBuffer::send(int cmd, void* object, int objectSize, std::function<int(NET_MESSAGE* p)> func) {
	int c = -1;
	{
		NetPackage* msg = new NetPackage(cmd, object, objectSize);
		NET_MESSAGE netMsg;
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

int NetIOBuffer::parse(const char * buf, int size, PARSE_DATA_CALLFUNC call, void* userdata)
{
	if (size <= 0) {
		LOGFMTD("size:%d less than 0.", size);
		return -1;
	}

	if (NULL == buf) {
		LOGFMTD("buf is null.");
		return -2;
	}

	if (NULL == call) {
		LOGFMTD("callback is null.");
		return -2;
	}

	int ret = -3;

// 	clock_t t = clock();

	{
		lw_lock_guard l(&_rlock);
		_cacheQueue.push(const_cast<char*>(buf), size);
		int cacheLength = (int)_cacheQueue.size();

		if (cacheLength < C_NETHEAD_SIZE) {
			return -3;
			LOGFMTD("not a complete data packet [cache_size:%d head_size:%d]", cacheLength, C_NETHEAD_SIZE);
		}

		do
		{
			NetHead *nh = (NetHead*)_cacheQueue.front();

			if (cacheLength < nh->size) {
				LOGFMTD("not a complete data packet [cache_size:%d, head_size:%d, cmd:%d]", cacheLength, nh->size, nh->cmd);
				break;
			}

			NetPackage* nmsg = new NetPackage(nh);
			if (nullptr != nmsg) {
				char* buf = _cacheQueue.front();
				char* tbuf = &buf[C_NETHEAD_SIZE];
				int tbuf_len = nh->size - C_NETHEAD_SIZE;
				nmsg->setMessage(tbuf, tbuf_len);
				call(nmsg->getHead()->cmd, nmsg->getBuf(), nmsg->getSize(), userdata);
			}
			delete nmsg;

			_cacheQueue.pop(nh->size);
			cacheLength = (int)_cacheQueue.size();
		} while (cacheLength >= C_NETHEAD_SIZE);

// 		clock_t t1 = clock();
// 		LOGFMTD("NetCore::parse time [%f]", ((double)t1 - t) / CLOCKS_PER_SEC);
	}
	return 0;
}
