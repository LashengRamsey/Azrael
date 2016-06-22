#include "celltype.h"
#include "bevstream.h"
#include "log.h"
#include "arch.h"
#include <string.h>


static const uint kHeaderLen = 5;	//包头长度
static const uint kIdLen = 9;		//
static const uint kEidLen = 5;		//
static const uint kTailLen = 1;		//包尾长度

BevStream::BevStream(bufferevent* bev)
	:bev_(bev),
	packetWriteSN_(0),
	packetReadSN_(0),
	packLen_(0),
	seed_(0),
	conn_(false)
{

}

void BevStream::decodeInplace(void* data, uint size)
{

}

void BevStream::innerWrite(void* data, uint size)
{
	int rc = bufferevent_write(bev_, data, size);
	if (rc < 0)
		ERRLOG("buffer write failed");
}

void BevStream::innerWrite(const Buf& buf)
{
	int rc = bufferevent_write_buffer(bev_, buf.get());
	if (rc < 0)
		ERRLOG("buffer write failed");
}

//
void BevStream::decode()
{
	struct evbuffer* input = bufferevent_get_input(bev_);
	size_t readable_len = evbuffer_get_length(input);

	unsigned char header[5] = {0};
	uint8 fid = 0;
	while( readable_len >= kHeaderLen + packLen_ + kTailLen)
	{
		if (packLen_==0)
		{
			evbuffer_copyout(input, &header, sizeof(header));
			if (header[0] != PACKET_HEAD)
			{
				ERRLOG("Connection closed due to error header %x,%d", header, packLen_);
				this->close();
				break;
			}
			memcpy(&packLen_, header+1, sizeof(packLen_));
			packLen_ = conv_num(packLen_);
			if (packLen_ <= 1 || packLen_ > 10240)
			{
				ERRLOG("Connection closed due to error len %d", packLen_);
				this->close();
				break;
			}

			if (packLen_ > 4096)
			{
				ERRLOG("Too large packet size %d", packLen_);
			}
		}

		if (readable_len >= kHeaderLen + packLen_ + kTailLen)
		{
			evbuffer_drain(input, kHeaderLen);
			LocalBuf data(packLen_);
			int ret = evbuffer_remove(input, data, packLen_);

			if ((uint)ret != packLen_)
			{
				ERRLOG("Can't read data from bev, need %d,return %d", packLen_,ret);
			}

			unsigned char tail = 0;
			evbuffer_remove(input, &tail, kTailLen);
			if (tail != PACKET_END)
			{
				ERRLOG("Connection closed due to error tail %.2,%d", tail, packLen_);
				this->close();
				Buf buf;
				buf.set(data, packLen_);

				readable_len = evbuffer_get_length(input);
				LocalBuf data1(readable_len);
				evbuffer_remove(input, data1, readable_len);

				Buf buf1;
				buf1.set(data1, readable_len);
				buf << buf1;

				buf.dump();
				break;
			}
			Buf buf;
			buf.set(data, packLen_);

			readable_len = evbuffer_get_length(input);
			packLen_ = 0;
			doMsg(&buf);
		}

	}

}

//以“\0”为一个包，log服务器
void BevStream::decodeText()
{
	struct evbuffer* input = bufferevent_get_input(bev_);

	if (evbuffer_get_length(input) > 65535)
	{
		INFO("Recv too larget text");
		this->close();
		return;
	}

	evbuffer_ptr it = evbuffer_search(input, "\0", 1, NULL);
	if (it.pos<0)
	{
		return;
	}
	size_t ncopy = it.pos;

	LocalBuf line(ncopy);
	evbuffer_remove(input, line, ncopy);

	Buf buf;
	buf.set(line, ncopy);
	doMsg(&buf);
}


int BevStream::newpacket(uint size)
{
	uint tsize = (size & 0x00ffffff)|((size%255)<<24);
	write(&tsize, sizeof(size));
	return packetWriteSN_++;
}

void BevStream::write(void* data, uint len)
{
	if (seed_!=0)
	{
	}
	innerWrite(data, len);
}

void BevStream::write(const Buf& buf)
{
	if (seed_!=0)
	{
		uint size = buf.getLength();
		LocalBuf buffer(size);
		buf.read(buffer, size);
		innerWrite(buffer, size);
	}
	else
	{
		innerWrite(buf);
	}

}

void BevStream::copyOnWrite(void* data, uint len)
{
	if (seed_!=0)
	{
		LocalBuf xdata(len);
		innerWrite(xdata, len);
	}
	else
	{
		innerWrite(data, len);
	}
}

void BevStream::setSeed(int seed)
{
	seed_ = seed;
}



