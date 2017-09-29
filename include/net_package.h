#ifndef __NetPackage_h__
#define __NetPackage_h__

#include <functional>
#include <iosfwd>
#include <iostream>
#include <string>

namespace lwstar
{
	class NetPackage;

#pragma  pack(1)
	typedef struct tagNetHead
	{
		friend std::ostream& operator<<(std::ostream & os, tagNetHead & o);

	public:
		int	size;			// 数据包大小
		unsigned short	v;				// 通讯版本
		int	cmd;			// 指令
		unsigned int	ctime;			// 发送时间

	public:
		tagNetHead();

	public:
		std::string debug();

	}  NetHead;

#pragma pack()

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
		NetHead* _head;
		char *_buf;
		unsigned int _size;
	};

};

#endif	//__NetPackage_h__
