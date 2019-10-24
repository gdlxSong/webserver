#include "buffer.hpp"
#include<stdlib.h>
#include<algorithm>

gdl::BufferTool::Buffer::Buffer(size_t extendSize)
{
	//initialize _extendSize.
	_extendSize = extendSize > MaxSize<MAX_BUFFER_EXTEND_SIZE>::size ? MaxSize<MAX_BUFFER_EXTEND_SIZE>::size : extendSize;
	//alloc buffer.
	buf = std::unique_ptr<char[]>(new char[_extendSize]);
	//initial buf.
	memset(buf.get(), 0, _extendSize);
	_beg = _end = 0;
	_cap = _extendSize;
}

gdl::BufferTool::Buffer::Buffer(Buffer&& other)
{
	buf = std::move(other.buf);
	_beg = other._beg;
	_end = other._end;
	_cap = other._cap;
	_extendSize = other._extendSize;
}

gdl::BufferTool::Buffer::~Buffer()
{
	//TODO...
}

char* gdl::BufferTool::Buffer::data()
{
	return buf.get() + _beg;
}

char* gdl::BufferTool::Buffer::end()
{
	return buf.get() + _end;
}

size_t gdl::BufferTool::Buffer::size()
{
	return _end - _beg;
}

size_t gdl::BufferTool::Buffer::capcity()
{
	return _cap;
}

size_t gdl::BufferTool::Buffer::usableSize()
{
	//如果缓冲区可用区域大小：_cap - _end < std::min(_extendSize/4, MIN_EMPTY_SIZE) ， 那么久extend
	if (reserved() < std::min<size_t>(_extendSize / 4, MIN_EMPTY_SIZE))
		extend(0);

	return reserved();
}

void gdl::BufferTool::Buffer::eat(size_t sz)
{
	size_t n = sz > size() ? size() : sz;
	_beg += n;
}

void gdl::BufferTool::Buffer::extend(size_t sz)
{
	size_t n = sz == 0 ? 2 * _cap : _cap + sz;
	auto buffer =  std::unique_ptr<char[]>(new char[n]);
	bzero(buffer.get(), n);
	memcpy(buffer.get(), data(), size());// size(buffer) > size(buf).
	buf = std::move(buffer);
	//set _beg, _end, _cap.
	_end -= _beg;
	_beg = 0;
	_cap = n;
}

void gdl::BufferTool::Buffer::loaded(size_t n)
{
	size_t nreserved = reserved();
	n = n > nreserved ? nreserved : n;

	_end += n;
}

void gdl::BufferTool::Buffer::clear()
{
	//logical clear.
	//bzero(buf.get(), capcity());
	_end = _beg = 0;
}

void gdl::BufferTool::Buffer::clean()
{
	//realloc buffer if buffer.size > ??, and bzero.
	_beg = _end = 0;
	if (REALLOC_BUFFER_SIZE <= capcity()) {
		buf = std::unique_ptr<char[]>(new char[_extendSize]);
		_cap = _extendSize;
	}
	bzero(buf.get(), _cap);
}

size_t gdl::BufferTool::Buffer::reserved()
{
	return _cap - _end;
}
