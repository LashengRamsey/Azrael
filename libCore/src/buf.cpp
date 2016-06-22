#include "celltype.h"
#include "buf.h"
#include <assert.h>
#include "arch.h"
#include <event.h>
#include "log.h"



Buf::Buf()
{
	//evbuffer_new()分配和返回一个新的空evbuffer
	buffer_ = evbuffer_new();
	assert(buffer_);
}

Buf::Buf(Buf *buf)
{
	buffer_ = evbuffer_new();
	//int evbuffer_add_buffer(struct evbuffer *dst, struct evbuffer *src);
	//evbuffer_add_buffer()将src中的所有数据移动到dst末尾，成功时返回0，失败时返回-1
	evbuffer_add_buffer(buffer_, buf->buffer_);
}

Buf::~Buf()
{
	if(buffer_)
	{
		//evbuffer_free()释放evbuffer和其内容
		evbuffer_free(buffer_);
		buffer_ = NULL;
	}
}

bool Buf::unpack(const char *fmt, ...) const
{
	uint need = (unsigned int)getFmtDataSize(fmt);
	if (getLength() < need)
	{
		return false;
	}

	LocalBuf data(need);
	char *pData = data;
	read(data, need);
	va_list va;
	int p = 0;
	va_start(va, fmt);
	char c = 0;
	while((c = fmt[p++]))
	{
		switch(c)
		{
		case 'i':
		case 'I':
			{
				int *i = va_arg(va, int*);
				*i = *(int*)pData;
				pData += 4;
			}
			break;
		
		case 'h':
			{
				short *i = va_arg(va, short*);
				*i = *(short*)pData;
				pData += 2;
			}
			break;

		case 'f':
			{
				float *i = va_arg(va, float*);
				*i = *(float*)pData;
				pData += 4;
			}
			break;

		case 'c':
			{
				int8 *i = va_arg(va, int8*);
				*i = *(int8*)pData;
				pData += 1;
			}
			break;

		default:
			ERRLOG("Buf::unpack,undefined argument typed specified");
			break;
		}
	}
	va_end(va);
	return true;
}

void Buf::pack(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	pack(fmt, va);
	va_end(va);
}

void Buf::pack(const char *fmt, va_list va)
{
	char c=0;
	int p=0;
	while((c=fmt[p++]))
	{
		switch(c)
		{
		case 'i':
		case 'I':
			{
				int i = va_arg(va, int);
				*this << i;
			}
			break;
		case 'h':
			{
				int i = va_arg(va, int);
				*this << (short)i;
			}
			break;
		case 'c':
			{
				int i = va_arg(va, int);
				*this << (int8)i;
			}
			break;
		case 'f':
			{
				double f = va_arg(va, double);
				*this << (float)f;
			}
			break;

		default:
			ERRLOG("Buf::unpack,undefined argement typed specified");
			break;
		}
	}
}

void Buf::drain(int size)
{
	//int evbuffer_drain(struct evbuffer *buf, size_t len);
	//int evbuffer_remove(struct evbuffer *buf, void *data, size_t datlen);
	//evbuffer_remove（）函数从buf前面复制和移除datlen字节到data处的内存中。
	//如果可用字节少于datlen，函数复制所有字节。失败时返回-1，否则返回复制了的字节数。
	//evbuffer_drain（）函数的行为与evbuffer_remove（）相同，只是它不进行数据复制：
	//而只是将数据从缓冲区前面移除。成功时返回0，失败时返回-1。
	evbuffer_drain(buffer_, size);
}

void Buf::dump() const
{
	uint len = this->getLength();
	LocalBuf buf(len);
	this->peek(buf, len);
	DumpMemory(buf, len);
}

uint Buf::getLength() const
{
	//返回evbuffer存储的字节数
	return evbuffer_get_length(buffer_);
}

void Buf::set(void *data, uint len)
{
	//这个函数通过引用向evbuffer末尾添加一段数据。
	//不会进行复制：evbuffer只会存储一个到data处的datlen字节的指针。
	//因此，在evbuffer使用这个指针期间，必须保持指针是有效的。
	//evbuffer会在不再需要这部分数据的时候调用用户提供的cleanupfn函数，带有提供的data指针、
	//datlen值和extra指针参数。函数成功时返回0，失败时返回-1。
	evbuffer_add_reference(buffer_, data, len, NULL, NULL);
}

void *Buf::getData(uint& len)
{
	len = this->getLength();
	void *pData = malloc(len);
	read(pData, len);
	return pData;
}

int Buf::peek(void *data, int size) const
{
	size = size < 0 ? getLength() : size;
	//evbuffer_copyout（）的行为与evbuffer_remove（）相同，但是它不从缓冲区移除任何数据。
	//也就是说，它从buf前面复制datlen字节到data处的内存中。如果可用字节少于datlen，函数会复制所有字节。
	//失败时返回-1，否则返回复制的字节数。
	return evbuffer_copyout(buffer_, data, size);
}

int Buf::read(void *data, int size) const
{
	size = size < 0 ? getLength() : size;
	if (getLength() < (uint)size)
	{
		ERRLOG("Buf::read,no enough data to read");
		return -1;
	}
	//evbuffer_remove（）函数从buf前面复制和移除datlen字节到data处的内存中。
	//如果可用字节少于datlen，函数复制所有字节。失败时返回-1，否则返回复制了的字节数。
	return evbuffer_remove(buffer_, data, size);
}

void Buf::write(const void *data, uint size)
{
	if (data && size >0)
	{
		//添加data处的datalen字节到buf的末尾，成功时返回0，失败时返回-1
		evbuffer_add(buffer_, data, size);
	}
}

void Buf::refWrite(const void *data, uint size, RefFree ref, void *userdata)
{
	//这个函数通过引用向evbuffer末尾添加一段数据。
	//不会进行复制：evbuffer只会存储一个到data处的datlen字节的指针。
	//因此，在evbuffer使用这个指针期间，必须保持指针是有效的。
	//evbuffer会在不再需要这部分数据的时候调用用户提供的cleanupfn函数，带有提供的data指针、
	//datlen值和extra指针参数。函数成功时返回0，失败时返回-1。
	evbuffer_add_reference(buffer_, data, size, ref, userdata);
}

//读取字符串
bool Buf::readString(std::string& s)
{
	//uint len = this->getLength();
	unsigned short len = 0;
	*this >> len;
	s.resize(len);
	if ((uint)read((void*)s.c_str(), len) == len)
	{
		return true;
	}
	return false;
}

//读取所有数据作为字符串
bool Buf::readText(std::string& s)
{
	uint len = this->getLength();
	s.resize(len);
	if ((uint)read((void*)s.c_str(), len) == len)
	{
		return true;
	}
	return false;
}

//没用，写入读取长字符串
bool Buf::writeLString(const std::string& s)
{
	//(1<<16) == 65536
	if (s.size() > 65536)
	{
		ERRLOG("writeLString too big %d to return", s.size());
		return false;
	}
	unsigned short len = (unsigned short)s.size();
	*this << len;
	write(s.data(), s.size());
	return true;
}

//没用，读取长字符串
bool Buf::readLString(std::string& s)
{
	uint len = (uint)this->readVarint();
	s.resize(len);
	if((uint)read((void*)s.data(), len) == len)
	{
		if (len > 366350)
		{
			ERRLOG("readLString too big string %d,pls check %s", len, s.data());
		}
		return true;
	}
	return true;
}

//写入字符串
bool Buf::writeString(const std::string &s)
{
	uint len = (uint)s.size();
	writeVarint(len);
	write(s.data(), s.size());
	return true;
}

int64 Buf::readVarint()
{
	char ch;
	*this >> ch;
	int64 value = ch & 0x7f;
	size_t shift = 7;
	while((ch & 0x80) && getLength())
	{
		*this >> ch;
		value |= ((uint64)(ch & 0x7f)) << shift;
		shift += 7;
	}
	return value;
}

void Buf::writeVarint(int64 v)
{
	uint64 value = (uint64)v;
	while(value >= 0x80)
	{
		*this << (char)(value | 0x80);
		value >>= 7;
	}
	*this << (char)(value);
}

//写入数据
Buf& Buf::operator<<(const Buf& v)
{
	if (v.getLength() > 0)
	{
		evbuffer_add_buffer(buffer_, v.buffer_);
	}
	return *this;
}

//写入字符串
Buf& Buf::writeLitteString(const char* str, const short len)
{
	writeShort((short)len);
	write(str, len);
	return *this;
}

//写入short
Buf& Buf::writeShort(short value)
{
	write((void*)&value, sizeof(value));
	return *this;
}

//写入Int
Buf& Buf::writeInt(int value)
{
	write((void*)&value, sizeof(value));
	return *this;
}

//写入Int64
Buf& Buf::writeInt64(int64 value)
{
	int len = sizeof(value);
	write((void*)&value, sizeof(value));
	return *this;
}

//读出short
short Buf::readShort()
{
	short value;
	read((void*)&value, sizeof(value));
	return value;
}

//读出Int
int Buf::readInt()
{
	int value;
	read((void*)&value, sizeof(value));
	return value;
}

//读出Int64
int64 Buf::readInt64()
{
	int64 value;
	read((void*)&value, sizeof(value));
	return value;
}

//读出字符串
int Buf::readLitteString(std::string& s)
{
	short len;
	read((void*)&len, sizeof(len));
	read((void*)&s, len);
	return 0;
}

