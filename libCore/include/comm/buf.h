#ifndef BUF_H
#define BUF_H

#include <string>
#include <stdlib.h>
#include <stdarg.h>

typedef unsigned int uint;
struct evbuffer;


//��װָ��void*���ڴ�
struct LocalBuf
{
	LocalBuf(uint len)
	{
		ptr_ = malloc(len);
		size_= len;
	}
	~LocalBuf()
	{
		if(ptr_)
		{
			free(ptr_);
		}
		ptr_ = NULL;
	}
	uint size() const
	{
		return size_;
	}
	operator void*()
	{
		return ptr_;
	}

	operator char*()
	{
		return (char*)ptr_;
	}

	void* ptr_;
	uint size_;
};

//��װָ��const void*���ڴ�
struct BufData
{
	BufData(const void* pData, uint len)
	{
		pData_ = pData;
		len_ = len;
	}

	const void* pData_;
	uint len_;
};

//��װevbuffer*
//libevent��evbufferʵ����Ϊ�����������ݺʹ�ǰ���Ƴ����ݶ��Ż����ֽڶ��С�
//evbuffer���ڴ���������IO�ġ����塱����.�����ṩ����IO���ߵ�IO����ʱ����IO�Ĺ��ܣ�����bufferevent�Ĺ�����
class Buf
{
public:
	Buf();
	Buf(Buf* buf);
	virtual ~Buf();

	uint getLength() const;
	evbuffer* get() const {return buffer_;}
	void set(void* data, uint size);
	void* getData(uint& len);

	bool unpack(const char* fmt, ...) const;
	void pack(const char* fmt, ...);
	void pack(const char* fmt, va_list va);
	void drain(int size);
	void dump() const;

	int peek(void* data, int size) const;
	int read(void* data, int size) const;

	typedef void (*RefFree)(const void* data, size_t datalen, void* extra);
	void write(const void* data, uint size);
	void refWrite(const void* data, uint size, RefFree ref, void* userdata=NULL);

	bool readString(std::string& s);
	bool readText(std::string& s);
	bool writeString(const std::string& s);

	bool readLString(std::string& s);
	bool writeLString(const std::string& s);

	int64 readVarint();
	void writeVarint(int64 v);

	template<typename T>
	Buf& operator<<(const T& v)
	{
		write(&v, sizeof(T));
		return *this;
	}

	template<typename T>
	Buf& operator>>(T& v)
	{
		read(&v, sizeof(T));
		return *this;
	}

	Buf& operator<<(const Buf& v);
	Buf& operator<<(const BufData& v)
	{
		write(v.pData_, v.len_);
		return *this;
	}

	Buf& writeByte(unsigned char value)
	{
		write((void*)&value, sizeof(value));
		return *this;
	}

	Buf& writeShort(short value);
	Buf& writeInt(int value);
	Buf& writeInt64(int64 value);
	Buf& writeLitteString(const char* str, short len);
	Buf& operator<<(const std::string& value)
	{
		this->writeString(value);
		return *this;
	}
	Buf& operator>>(std::string& value)
	{
		this->readString(value);
		return *this;
	}

	unsigned char readByte()
	{
		unsigned char value;
		read((void*)&value, sizeof(value));
		return value;
	}
	short readShort();
	int readInt();
	int64 readInt64();
	int readLitteString(std::string& s);

private:
	evbuffer* buffer_;
};

#endif //BUF_H

