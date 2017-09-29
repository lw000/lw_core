#include "net_package.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>

namespace lwstar
{
	static const int C_NETHEAD_SIZE = sizeof(NetHead);

	//////////////////////////////////////////////////////////////////////////////////////
	static unsigned short __make_package_version(unsigned char major, unsigned char minor) {
		unsigned int version = major;
		version = (version << 8);
		version = (version) | minor;
		return version;
	}

	static void __package_version(unsigned short version, unsigned char &major, unsigned char &minor) {
		minor = (version & 0x00FF);

		version = (version >> 8);
		major = (version & 0x00FF);
	}

	std::ostream& operator<<(std::ostream & os, tagNetHead & o)
	{
		os << o.debug();
		return os;
	}

	tagNetHead::tagNetHead()
	{
		this->size = 0;			// 数据包大小
		this->cmd = 0;			// 指令
		this->ctime = 0;		// 发送时间
		this->v = __make_package_version(1, 1);
	}

	std::string tagNetHead::debug()
	{
		char buf[512];
		unsigned char v1;
		unsigned char v2;
		__package_version(this->v, v1, v2);
		sprintf(buf, "NetHead >> size:%d, cmd:%d, time:%u, v:%d.%d", this->size, this->cmd, this->ctime, v1, v2);
		return std::string(buf);
	}

	//////////////////////////////////////////////////////////////////////////////////////

	NetPackage::NetPackage() : _size(0), _buf(NULL)
	{
		_buf = NULL;
		this->_head = new NetHead;
	}

	NetPackage::NetPackage(int cmd, void* msg, int size) : _size(0), _buf(NULL)
	{
		this->_head = new NetHead;
		this->setMessage(cmd, msg, size);
	}

	NetPackage::NetPackage(const NetHead* head) : _size(0), _buf(NULL)
	{
		if (head != NULL) {
			this->_head = new NetHead;
			*this->_head = *head;
		}
		else {

		}
	}

	NetPackage::~NetPackage()
	{
		if (this->_buf != NULL)
		{
			free(this->_buf);
			this->_buf = NULL;
		}

		if (this->_head != NULL)
		{
			delete this->_head;
			this->_head = NULL;
		}
	}

	void NetPackage::setMessage(char* msg, int size)
	{
		if (msg == NULL) return;
		if (size <= 0) return;

		this->_size = size;
		_buf = (char*)malloc(size * sizeof(char));
		memcpy(this->_buf, msg, size);
	}

	int NetPackage::setMessage(int cmd, void* msg, int size)
	{
		if ((msg != NULL) && (size <= 0)) {
			return -1;
		}

		do
		{
			this->_size = size + C_NETHEAD_SIZE;

			this->_head->size = this->_size;
			this->_head->cmd = cmd;
			this->_head->ctime = (unsigned int)time(NULL);
			this->_buf = (char*)malloc(this->_size * sizeof(char));
			::memcpy(this->_buf, this->_head, C_NETHEAD_SIZE);

			if ((NULL != msg) && (size > 0)) {
				::memcpy(&this->_buf[C_NETHEAD_SIZE], (void*)msg, size);
			}
			else {
				printf("size: %d, NetHead: [cmd: %6d size: %6d ctime: %6d]", this->_size, this->_head->cmd, this->_head->size, this->_head->ctime);
			}
		} while (0);

		return 0;
	}

	std::string NetPackage::debug()
	{
		char buf[256];
		sprintf(buf, "size: %d, NetHead: [cmd: %6d size: %6d ctime: %6d]", this->_size, this->_head->cmd, this->_head->size, this->_head->ctime);
		return std::string(buf);
	}

	char* NetPackage::getBuf() const
	{
		return this->_buf;
	}

	int NetPackage::getSize() const
	{
		return this->_size;
	}

	const NetHead* NetPackage::getHead() const
	{
		return this->_head;
	}
}
