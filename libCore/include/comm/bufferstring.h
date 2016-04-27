#ifndef _BUFFERSTRING_H
#define _BUFFERSTRING_H
#include <string>
class BufferString
{
public:
	BufferString();
	virtual ~BufferString();
	BufferString(const BufferString& string);

	BufferString& operator=(const BufferString& string);
	BufferString& operator+(const BufferString& string);

	BufferString& append(char v);
	BufferString& append(unsigned char v);


	BufferString& append(short v);
	BufferString& append(unsigned short v);


	BufferString& append(int v);
	BufferString& append(unsigned int v);

	BufferString& append(long v);
	BufferString& append(unsigned long v);

	BufferString& append(long long v);
	BufferString& append(unsigned long long v);

	BufferString& append(const char* v);
	const char*  getContent() const;
protected:
	void autoChange(unsigned int contentSize);
	void clear();
	void release();
private:
	std::string mContent;
	/*
	char* mContent;
	unsigned int  mSize;
	unsigned int  mStart;
	*/
};
#endif