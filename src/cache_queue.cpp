#include "cache_queue.h"
#include <algorithm>

CacheQueue::CacheQueue(void) {
	
}
	
CacheQueue::~CacheQueue(void) {
	
}

int CacheQueue::push(char* buf, int size)
{
	if (buf == NULL) return -1;
	if (size <= 0) return -2;
	int i = 0;
	char* p = buf;
	while (i++ < size) {
		_cq.push_back(*p++);
	}
	return size;
}

void CacheQueue::pop(int size)
{
	if (size <= 0) return;
	if (_cq.empty()) return;
	if (size > _cq.size()) return;
		
	std::vector<char>::iterator iter = _cq.erase(_cq.begin(), _cq.begin() + size);
}

void CacheQueue::front_copyto(char* buffer, int size) {
	if (_cq.size() < size) { 
		return;
	}

	char* p = buffer;
	int i = 0;
	/*for (auto v : _cq) {*/
	std::for_each(_cq.begin(), _cq.end(), [&p, &i, &size](char c) {
		if (i++ < size) {
			*p++ = c;
		}
	});
}

std::vector<char>* CacheQueue::front_copyto(std::vector<char>& dest, int size) {
	if (_cq.size() < size)
	{
		return nullptr;
	}

	std::copy_n(_cq.begin(), size, dest.begin());

	return &dest;
}

size_t CacheQueue::size() const
{
	int l = 0;
	{
		l = _cq.size();
	}
	return l;
}

char* CacheQueue::front()
{
	return _cq.data();
}

void CacheQueue::clear()
{
	_cq.clear();
}
