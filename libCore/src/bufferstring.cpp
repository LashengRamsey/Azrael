#include "bufferstring.h"
#include "arch.h"
#include <string.h>

BufferString::BufferString()
{
	DEBUG_TRY;
	DEBUG_CATCH;
}

BufferString::~BufferString()
{
	mContent.clear();
}

BufferString::BufferString(const BufferString& string)
{
	DEBUG_TRY;
	mContent = string.getContent();
	DEBUG_CATCH;
}

const char* BufferString::getContent() const
{
	return mContent.c_str();
}

BufferString& BufferString::operator =(const BufferString &string)
{
	DEBUG_TRY;
	mContent.clear();
	mContent.append(string.getContent());
	return *this;
	DEBUG_CATCH;
}

BufferString& BufferString::operator +(const BufferString& string)
{
	DEBUG_TRY;
	mContent.append(string.getContent());
	return *this;
	DEBUG_CATCH;
}

BufferString& BufferString::append(const char* v)
{
	DEBUG_TRY;
	mContent.append(v);
	return *this;
	DEBUG_CATCH;
}

BufferString& BufferString::append(char v)
{
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%d",v);
	mContent.append(buffer);
	return *this;
}

BufferString& BufferString::append(unsigned char v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%u",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}


BufferString& BufferString::append(short v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%d",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}
BufferString& BufferString::append(unsigned short v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%u",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}


BufferString& BufferString::append(int v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%d",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}
BufferString& BufferString::append(unsigned int v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%u",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}


BufferString& BufferString::append(long v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	int size = sizeof(long);
	if(size == 4)
		sprintf(buffer,"%d",v);
	else
		sprintf(buffer,"%ld",v);

	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}

BufferString& BufferString::append(unsigned long v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	int size = sizeof(long);
	if(size == 4)
		sprintf(buffer,"%u",v);
	else
		sprintf(buffer,"%lu",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}

BufferString& BufferString::append(long long v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%lld",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}

BufferString& BufferString::append(unsigned long long v)
{
	DEBUG_TRY;
	char buffer[128];
	memset(buffer,0,128);
	sprintf(buffer,"%llu",v);
	mContent.append(buffer);
	return *this;
	DEBUG_CATCH;
}
