#include "DBServer.h"
#include "timer.h"
#include "lua-hiredis.h"


DBServer::DBServer()
{
	socket_ = NULL;
}

DBServer::~DBServer()
{
	int zero = 0;
	if (socket_)
	{
		zmq_setsockopt(socket_, ZMQ_LINGER, &zero, sizeof(zero));
		zmq_close(socket_);
		socket_ = NULL;
	}

	LOG("finalized zmq objects");
}

void DBServer::createContext()
{
	lua_ = new LuaSvr();
	mqnet_ = new MQNet();
	timer_ = new Timer();
}

void DBServer::run()
{
	onInited();
	if(lua_)
		lua_->init();
	if(timer_)
		timer_->init();
	if(net_)
		net_->init();

	if(lua_)
	{
		lua_State *L_ = lua_->L();
		luaL_register(L_, "_G", LuaMysql::functions);
		luaopen_hiredis(L_);
		lua_->run();
	}

	if (mqnet_)
	{
		connectRouter();
	}
}

void DBServer::onInited()
{
	init_log();

	if (mqnet_)
	{
		socket_ = zmq_socket(mqnet_->context(), ZMQ_ROUTER);
	}
	else
	{
		ERRLOG("dbserver mqnet init failed!\n");
		return;
	}

	int rc = 0;
	char path[100] = {0};
	std::string sockpath = Config::GetValue("SocketPath");
	std::string port = Config::GetValue("BindPort");
	if (strcmp(sockpath.c_str(), "tcp://") >= 0)
	{
		snprintf(path, MAX_PATH, "%s:%s", sockpath.c_str(), port.c_str());
	}
	else
	{
		snprintf(path, MAX_PATH, "%s/router_%s", sockpath.c_str(), port.c_str());
	}

	if ((rc = zmq_bind(socket_, path)) != 0)
	{
		ERRLOG("dbserver bind %s failed %d!\n", path, rc);
		return;
	}
	INFO("bind to path %s ok!", path);
}

void DBServer::readMqMsg()
{
	if (!socket_)
		return;

	while(true)
	{
		int source;
		if (zmq_recv(socket_, &source, 4, ZMQ_DONTWAIT) <= 0)
		{
			break;
		}
		
		zmq_msg_t message;
		zmq_msg_init(&message);
		int rc = 0;
		if ((rc = zmq_recvmsg(socket_, &message, ZMQ_DONTWAIT)) < 0 )
		{
			LOG("recv message error:%d", rc);
			return;
		}
		int fid, target;
		size_t len = zmq_msg_size(&message);
		if (len < 0)
		{
			LOG("recv message less than 8");
			zmq_msg_close(&message);
			return;
		}

		memcpy(&target, (char*)zmq_msg_data(&message), 4);
		memcpy(&fid, (char*)zmq_msg_data(&message)+4, 4);

		std::string data;
		data.append((char*)zmq_msg_data(&message)+8, len-8);
		zmq_msg_close(&message);
		uint sn = 0;
		LuaSvr::call("CHandlerMsg", "iiiSii", source, sn, fid, &data, 0, data.size());
	}
}


void DBServer::onUpdate(unsigned int dtime)
{
	readMqMsg();
}

int DBServer::SendToGameServer(int target, int fid, int sn, const Buf& buf)
{
	int source  = 0;
	std::string str;

	const_cast<Buf&>(buf).readText(str);

	zmq_send(socket_, &target, 4, ZMQ_SNDMORE);
	zmq_msg_t data;
	zmq_msg_init_size(&data, str.size() + sizeof(int) *3);// + sizeof(int64));

	memcpy(zmq_msg_data(&data), &source, 4);
	memcpy(((char*)zmq_msg_data(&data)) + sizeof(int), &fid, sizeof(int));
	memcpy(((char*)zmq_msg_data(&data)) + sizeof(int)*2, &sn, sizeof(int));
	//memcpy(((char*)zmq_msg_data(&data)) + sizeof(int)*3, &uid, sizeof(int64));
	memcpy((char*)zmq_msg_data(&data) + sizeof(int)*3, str.c_str(), str.size());
	
	zmq_sendmsg(socket_, &data, ZMQ_DONTWAIT);
	zmq_msg_close(&data);
	return 0;
}









