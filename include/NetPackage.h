#ifndef __NetPackage_h__
#define __NetPackage_h__

#include <functional>
#include <iosfwd>
#include <iostream>
#include <string>

#include "NetHead.h"

namespace lwstar
{
	class NetPackage;

	class NetPackage
	{
	public:
		NetPackage();
		NetPackage(const NetHead* head);
		NetPackage(int cmd, void* msg = nullptr, int size = 0);

	public:
		~NetPackage();

	public:
		void setMessage(char* msg, int size);
		int setMessage(int cmd, void* msg, int size);

	public:
		char* getBuf() const;
		int getSize() const;
		const NetHead* getHead() const;

	public:
		std::string debug();

	private:
		NetHead _head;
		char *_buf;
		unsigned int _size;
	};

};

#endif	//__NetPackage_h__
