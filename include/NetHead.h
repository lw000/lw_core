#ifndef __NetHead_H_
#define __NetHead_H_

#include <string>

namespace lwstar 
{

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
}

#endif // !__NetHead_H_
