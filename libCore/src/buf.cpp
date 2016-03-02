#include "celltype.h"
#include "buf.h"
#include <assert.h>
#include "arch.h"
#include <event.h>
#include "log.h"



Buf::Buf()
{
	//evbuffer_new()����ͷ���һ���µĿ�evbuffer
	buffer_ = evbuffer_new();
	assert(buffer_);
}

Buf::Buf(Buf *buf)
{
	buffer_ = evbuffer_new();
	//int evbuffer_add_buffer(struct evbuffer *dst, struct evbuffer *src);
	//evbuffer_add_buffer()��src�е����������ƶ���dstĩβ���ɹ�ʱ����0��ʧ��ʱ����-1
	evbuffer_add_buffer(buffer_, buf->buffer_);
}

Buf::~Buf()
{
	if(buffer_)
	{
		//evbuffer_free()�ͷ�evbuffer��������
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
			ERROR("Buf::unpack,undefined argument typed specified");
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
			ERROR("Buf::unpack,undefined argement typed specified");
			break;
		}
	}
}

void Buf::drain(int size)
{
	//int evbuffer_drain(struct evbuffer *buf, size_t len);
	//int evbuffer_remove(struct evbuffer *buf, void *data, size_t datlen);
	//evbuffer_remove����������bufǰ�渴�ƺ��Ƴ�datlen�ֽڵ�data�����ڴ��С�
	//��������ֽ�����datlen���������������ֽڡ�ʧ��ʱ����-1�����򷵻ظ����˵��ֽ�����
	//evbuffer_drain������������Ϊ��evbuffer_remove������ͬ��ֻ�������������ݸ��ƣ�
	//��ֻ�ǽ����ݴӻ�����ǰ���Ƴ����ɹ�ʱ����0��ʧ��ʱ����-1��
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
	//����evbuffer�洢���ֽ���
	return evbuffer_get_length(buffer_);
}

void Buf::set(void *data, uint len)
{
	//�������ͨ��������evbufferĩβ���һ�����ݡ�
	//������и��ƣ�evbufferֻ��洢һ����data����datlen�ֽڵ�ָ�롣
	//��ˣ���evbufferʹ�����ָ���ڼ䣬���뱣��ָ������Ч�ġ�
	//evbuffer���ڲ�����Ҫ�ⲿ�����ݵ�ʱ������û��ṩ��cleanupfn�����������ṩ��dataָ�롢
	//datlenֵ��extraָ������������ɹ�ʱ����0��ʧ��ʱ����-1��
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
	//evbuffer_copyout��������Ϊ��evbuffer_remove������ͬ�����������ӻ������Ƴ��κ����ݡ�
	//Ҳ����˵������bufǰ�渴��datlen�ֽڵ�data�����ڴ��С���������ֽ�����datlen�������Ḵ�������ֽڡ�
	//ʧ��ʱ����-1�����򷵻ظ��Ƶ��ֽ�����
	return evbuffer_copyout(buffer_, data, size);
}

int Buf::read(void *data, int size) const
{
	size = size < 0 ? getLength() : size;
	if (getLength() < (uint)size)
	{
		ERROR("Buf::read,no enough data to read");
		return -1;
	}
	//evbuffer_remove����������bufǰ�渴�ƺ��Ƴ�datlen�ֽڵ�data�����ڴ��С�
	//��������ֽ�����datlen���������������ֽڡ�ʧ��ʱ����-1�����򷵻ظ����˵��ֽ�����
	return evbuffer_remove(buffer_, data, size);
}

void Buf::write(const void *data, uint size)
{
	if (data && size >0)
	{
		//���data����datalen�ֽڵ�buf��ĩβ���ɹ�ʱ����0��ʧ��ʱ����-1
		evbuffer_add(buffer_, data, size);
	}
}

void Buf::refWrite(const void *data, uint size, RefFree ref, void *userdata)
{
	//�������ͨ��������evbufferĩβ���һ�����ݡ�
	//������и��ƣ�evbufferֻ��洢һ����data����datlen�ֽڵ�ָ�롣
	//��ˣ���evbufferʹ�����ָ���ڼ䣬���뱣��ָ������Ч�ġ�
	//evbuffer���ڲ�����Ҫ�ⲿ�����ݵ�ʱ������û��ṩ��cleanupfn�����������ṩ��dataָ�롢
	//datlenֵ��extraָ������������ɹ�ʱ����0��ʧ��ʱ����-1��
	evbuffer_add_reference(buffer_, data, size, ref, userdata);
}

//��ȡ�ַ���
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

//��ȡ����������Ϊ�ַ���
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

//û�ã�д���ȡ���ַ���
bool Buf::writeLString(const std::string& s)
{
	//(1<<16) == 65536
	if (s.size() > 65536)
	{
		ERROR("writeLString too big %d to return", s.size());
		return false;
	}
	unsigned short len = (unsigned short)s.size();
	*this << len;
	write(s.data(), s.size());
	return true;
}

//û�ã���ȡ���ַ���
bool Buf::readLString(std::string& s)
{
	uint len = (uint)this->readVarint();
	s.resize(len);
	if((uint)read((void*)s.data(), len) == len)
	{
		if (len > 366350)
		{
			ERROR("readLString too big string %d,pls check %s", len, s.data());
		}
		return true;
	}
	return true;
}

//д���ַ���
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

//д������
Buf& Buf::operator<<(const Buf& v)
{
	if (v.getLength() > 0)
	{
		evbuffer_add_buffer(buffer_, v.buffer_);
	}
	return *this;
}

//д���ַ���
Buf& Buf::writeLitteString(const char* str, const short len)
{
	writeShort((short)len);
	write(str, len);
	return *this;
}

//д��short
Buf& Buf::writeShort(short value)
{
	write((void*)&value, sizeof(value));
	return *this;
}

//д��Int
Buf& Buf::writeInt(int value)
{
	write((void*)&value, sizeof(value));
	return *this;
}

//д��Int64
Buf& Buf::writeInt64(int64 value)
{
	int len = sizeof(value);
	write((void*)&value, sizeof(value));
	return *this;
}

//����short
short Buf::readShort()
{
	short value;
	read((void*)&value, sizeof(value));
	return value;
}

//����Int
int Buf::readInt()
{
	int value;
	read((void*)&value, sizeof(value));
	return value;
}

//����Int64
int64 Buf::readInt64()
{
	int64 value;
	read((void*)&value, sizeof(value));
	return value;
}

//�����ַ���
int Buf::readLitteString(std::string& s)
{
	short len;
	read((void*)&len, sizeof(len));
	read((void*)&s, len);
	return 0;
}

