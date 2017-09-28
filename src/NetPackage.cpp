#include "NetPackage.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

namespace lwstar
{
	static const int C_NETHEAD_SIZE = sizeof(NetHead);

	////////////////////////////////////////////////////////////////////////////////////////////////

	NetPackage::NetPackage() : _size(0), _buf(NULL)
	{
		_buf = NULL;
	}

	NetPackage::NetPackage(int cmd, void* msg, int size) : _size(0), _buf(NULL)
	{
		this->setMessage(cmd, msg, size);
	}

	NetPackage::NetPackage(const NetHead* head) : _size(0), _buf(NULL)
	{
		if (head != NULL) {
			this->_head = *head;
		}
	}

	NetPackage::~NetPackage()
	{
		if (this->_buf != NULL)
		{
			free(this->_buf);
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
			this->_head.size = this->_size;
			this->_head.cmd = cmd;
			this->_head.ctime = (unsigned int)time(NULL);
			this->_buf = (char*)malloc(this->_size * sizeof(char));
			::memcpy(this->_buf, &this->_head, C_NETHEAD_SIZE);

			if ((NULL != msg) && (size > 0)) {
				::memcpy(&this->_buf[C_NETHEAD_SIZE], (void*)msg, size);
			}
			else {
				printf("size: %d, NetHead: [cmd: %6d size: %6d ctime: %6d]", this->_size, this->_head.cmd, this->_head.size, this->_head.ctime);
			}
		} while (0);

		return 0;
	}

	std::string NetPackage::debug()
	{
		char buf[256];
		sprintf(buf, "size: %d, NetHead: [cmd: %6d size: %6d ctime: %6d]", this->_size, this->_head.cmd, this->_head.size, this->_head.ctime);
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
		return &this->_head;
	}
}
