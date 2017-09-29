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
// 	 		lw_lock_guard l(&_send_m);
			c = func(&netMsg);
		}

		delete msg;
	}
	return c;
}

int NetCore::parse(const char * buf, int size, LW_PARSE_DATA_CALLFUNC func, void* userdata)
{
	if (size <= 0) return -1;
	if (NULL == buf) return -2;

	clock_t t = clock();
	{
		int dqs = 0;
		{
			lw_lock_guard l(&_read_m);
			_cq.push(const_cast<char*>(buf), size);
			dqs = (int)_cq.size();

			if (C_NETHEAD_SIZE <= dqs) {
				do
				{
					char * package = nullptr;
					{
						NetHead *nh = (NetHead*)_cq.front();
						if (dqs < nh->size) {
							char buf[256];
							sprintf(buf, "not a complete data packet [dqs: %d, head size: %d]", dqs, nh->size);
							LOGD(buf);

							break;
						}
						else
						{
							package = new char[nh->size];
							_cq.front_copyto(package, nh->size);
							_cq.pop(nh->size);
							dqs = (int)_cq.size();
						}

						NetHead *phd = (NetHead*)package;

						if (phd->size < 0) {
							LOGD(phd->debug());
						}

						char* buf = &package[C_NETHEAD_SIZE];
						int buf_len = phd->size - C_NETHEAD_SIZE;

						NetPackage* msg = new NetPackage(phd);
						if (nullptr != msg) {
							msg->setMessage(buf, buf_len);
							func(msg->getHead()->cmd, msg->getBuf(), msg->getSize(), userdata);
						}
						delete msg;

						if (package != nullptr) {
							delete package;
						}
					}

				} while (dqs >= C_NETHEAD_SIZE);
			}
			else {
				char buf[256];
				sprintf(buf, "not a complete data packet [dqs: %d]", dqs);
				LOGD(buf);
			}
		}
	}	
	
	clock_t t1 = clock();
	{
		char s[512];
		sprintf(s, "NetCore::parse time [%f]", ((double)t1 - t) / CLOCKS_PER_SEC);
		LOGD(s);
	}

	return 0;
}
