#include "luanetwork.h"
#include "luaglobal.h"
#include "luasvr.h"
#include "app.h"
#include "zmq.h"
#include "mqnet.h"
#include "connection.h"



LUA_IMPLE(LuaNetwork, LuaNetwork);
LUA_METD(LuaNetwork)
LUA_METD_END

LUA_FUNC(LuaNetwork)
L_METHOD(LuaNetwork, ConnectRouter)
L_METHOD(LuaNetwork, ConnectDB)
L_METHOD(LuaNetwork, GetSessionIP)
L_METHOD(LuaNetwork, SetSessionUserData)
L_METHOD(LuaNetwork, CloseSession)
L_METHOD(LuaNetwork, SendToServer)
L_METHOD(LuaNetwork, SendToNet)
L_METHOD(LuaNetwork, SendToDB)
L_METHOD(LuaNetwork, SendToGameServer)
LUA_FUNC_END



int LuaNetwork::GetSessionIP(lua_State* L)
{
	int sn;
	Lua::argParse(L, "i", &sn);
	Net *pNet = ServerApp::get()->getNet();
	if (!pNet)
	{
		luaL_error(L, "Net have'nt been created!");
		return 0;
	}
	return Lua::returnValue(L, "s", pNet->sessionHostIP(sn));
}

int LuaNetwork::SetSessionUserData(lua_State* L)
{
	int sn;
	char *ud;
	Lua::argParse(L, "is", &sn, &ud);
	Net *pNet = ServerApp::get()->getNet();
	if (!pNet)
	{
		luaL_error(L, "Net have'nt been created!");
		return 0;
	}
	Session *s = pNet->getSession(sn);
	if (s)
	{
		s->setUserData(ud);
	}
	else
	{
		luaL_error(L, "Can't find session %d", sn);
	}
		
	return 0;
}

int LuaNetwork::CloseSession(lua_State* L)
{
	int sn;
	Lua::argParse(L, "i", &sn);
	Net *pNet = ServerApp::get()->getNet();
	if (!pNet)
	{
		luaL_error(L, "Net have'nt been created!");
		return 0;
	}
	Session *s = pNet->getSession(sn);
	if (s)
	{
		s->close();
		LOG("Close connection %d sn by script interface", sn);
	}
	return 0;
}

int LuaNetwork::ConnectRouter(lua_State* L)
{
	ServerApp::get()->connectRouter();
	return 0;
}

int LuaNetwork::ConnectDB(lua_State* L)
{
	ServerApp::get()->connectDB();
	return 0;
}

int LuaNetwork::SendToDB(lua_State* L)
{
	int channel, target, t;
	int fid = 0;
	Lua::argParse(L, "iit", &channel, &target, &t);
	Buf buf;
	int len = lua_objlen(L, t);
	for (int i = 1;i <= len; ++i)
	{
		lua_rawgeti(L, t, i);
		if(!lua_istable(L,-1))
		{
			luaL_error(L, "except table at index:%d", i);
		}
		resolvePacketTableItem(L, &buf);
		lua_pop(L, 1);
	}
	MQNet *mqnet = ServerApp::get()->getMQNet();
	if(mqnet)
	{
		INFO("[lua proto]SendToDB sned msg, target:%d, fid:%d, data size:%d", target, fid, buf.getLength());
		mqnet->methodToDB(channel, target, fid, buf);
	}
	return 0;

}

int LuaNetwork::SendToNet(lua_State* L)
{
	int sn, t;
	Lua::argParse(L, "it", &sn, &t);

	Buf buf;
	int len = lua_objlen(L, t);
	for (int i = 1;i <= len; ++i)
	{
		lua_rawgeti(L, t, i);
		if(!lua_istable(L,-1))
		{
			luaL_error(L, "except table at index:%d", i);
		}
		resolvePacketTableItem(L, &buf);
		lua_pop(L, 1);
	}
	Net *net = ServerApp::get()->getNet();
	if (net)
	{
		net->sendPacket(sn, 0, buf);
	}
	return 0;
}

int LuaNetwork::SendToServer(lua_State* L)
{
	int target, sn, t;

	Lua::argParse(L, "iit", &target, &sn, &t);
	Buf buf;
	int len = lua_objlen(L, t);
	for (int i = 1;i <= len; ++i)
	{
		lua_rawgeti(L, t, i);
		if(!lua_istable(L,-1))
		{
			luaL_error(L, "except table at index:%d", i);
		}
		resolvePacketTableItem(L, &buf);
		lua_pop(L, 1);
	}
	ServerApp::get()->SendPacket(target, 0, sn, buf);
	return 0;
}

int LuaNetwork::SendToGameServer(lua_State* L)
{
	int target, sn, t;
	int fid = 0;
	Lua::argParse(L, "iit", &target, &sn, &t);
	
	Buf buf;
	//返回指定的索引处的值的长度。对于 string ，那就是字符串的长度；对于 table ，
	//是取长度操作符 ('#') 的结果；对于 userdata ，就是为其分配的内存块的尺寸；对于其它值，为 0 。
	int len = lua_objlen(L, t);
	for (int i = 1;i <= len; ++i)
	{
		//把 t[n] 的值压栈，这里的 t 是指给定索引 index 处的一个值
		lua_rawgeti(L, t, i);
		if(!lua_istable(L,-1))
		{
			luaL_error(L, "except table at index:%d", i);
		}
		resolvePacketTableItem(L, &buf);
		lua_pop(L, 1);			//弹出栈
	}
	ServerApp::get()->SendToGameServer(target, fid, sn, buf);
	return 0;
}

void LuaNetwork::resolvePacketTableItem(lua_State* L, class Buf* buf)
{
	if (!buf)
	{
		return;
	}

	//lua_rawgeti(L, -1, -1);
	lua_rawgeti(L, -1, 1);
	int type = luaL_checkinteger(L, -1);
	lua_rawgeti(L, -2, 2);

	switch(type)
	{
	case 0:
		{
			size_t len = 0;
			const char *v = luaL_checklstring(L, -1, &len);
			buf->writeLitteString(v, (short)len);
			break;
		}
	case 1:
		{
			int v = luaL_checkinteger(L, -1);
			buf->writeByte(v);
			break;
		}
	case 2:
		{
			int v = luaL_checkinteger(L, -1);
			buf->writeShort(v);
			break;
		}
	case 4:
		{
			int v = luaL_checkinteger(L, -1);
			buf->writeInt(v);
			break;
		}
	case 8:
		{
			int64 v = (int64)luaL_checknumber(L, -1);
			buf->writeInt64(v);
			break;
		}
	default:
		luaL_error(L, "undefine type:%d", type);
	}
	//从堆栈中弹出 n 个元素
	lua_pop(L, 2);
}




