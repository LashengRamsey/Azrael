#include "game.h"


Game::Game()
{

}

Game::~Game()
{

}

void Game::onInited()
{

	std::string bindPort = Config::GetValue("BindPort");
	if (bindPort.empty())
	{
		ERROR("get bind port failed");
		return;
	}
	net_ = new Net(LM_CHECKOVERTIME);
	
	connect();
}

void Game::onUpdate(unsigned int dtime)
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

/*void Game::doNetMsg(int sn, Buf *buf)
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



