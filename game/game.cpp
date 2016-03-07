#include <stdio.h>
#include "app.h"
#include "mqnet.h"
#include "buf.h"
#include "arch.h"
#include "net.h"
#include "log.h"
#include "luasvr.h"
#include "lua_export.h"
#include "timer.h"
#include "Config.h"



class Game : public ServerApp
{
public:
	Game() {}
	~Game() {}

	void onInited()
	{
		if (1)
		{
			std::string bindPort = Config::GetValue("BindPort");
			if (bindPort.empty())
			{
				ERROR("get bind port failed");
				return;
			}
			net_ = new Net(LM_CHECKOVERTIME);
		}
		connect();
	}

	void onUpdate(unsigned int dtime)
	{
		const char* line = getInput();
		if (line && lua_)
		{
			lua_State *L = lua_->L();
			if (luaL_loadbuffer(L, line, strlen(line), "TempDebug"))
			{
				LOG("err = %s\n", lua_tostring(L, -1));
			}
			LuaSvr::scriptCall(L, 0, 0);
		}
	}

	/*void doNetMsg(int sn, Buf *buf)
	{
		if (buf == NULL)
		{
			ERROR("doNetMsg method buf is NULL error");
			return;
		}
		int len = buf->getLength();
		if (len < 2)
		{
			ERROR("doNetMsg method len less then 2 error");
			return;
		}
		uint16 fid = 0;
		*buf >> fid;
		std::string data;
		buf->readText(data);
		LuaSvr::call("CHandlerNetMsg", "iiSii", sn, fid, &data, 0, data.size());
	}*/

};


int main(int argc, char *argv[])
{
#ifdef WIN32
#	ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#	endif
#endif
	Game game;
	if ( !game.init(argc, argv))
	{
		ERROR("init failed!");
	}
	else
	{
		game.start();
	}


	return 0;
}