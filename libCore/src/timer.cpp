#include "timer.h"
#include "arch.h"

Timer* Timer::timer_ = NULL;

LUA_IMPLE(Timer, Timer);
LUA_METD(Timer)
LUA_METD_END
LUA_FUNC(Timer)
L_METHOD(Timer, addTimer)
L_METHOD(Timer, delTimer)
LUA_FUNC_END



Timer* Timer::get()
{
	return timer_;
}

Timer::Timer()
{
	m_L = NULL;
	timer_ = this;
}

Timer::~Timer()
{

}


bool Timer::init()
{
	m_L = LuaSvr::get()->L();
	return true;
}

//���Ӷ�ʱ����
int Timer::addTimer(lua_State *L)
{
	Timer *timer = Timer::get();
	if (!timer)
		return 0;
	
	int exp,cycle,id;
	Lua::argParse(L, "iii", &exp, &cycle, &id);
	
	stTime timeInfo;
	timeInfo.index = id;	//id
	timeInfo.lasttime = timer_get_time();	//��һ��ִ��ʱ��
	timeInfo.cycle = cycle;					//ʱ������
	timeInfo.expires = exp;					//��Ч��

	timer->timerMap_.insert(std::make_pair(id, timeInfo));
	return 0;
}

//ɾ����ʱ����
int Timer::delTimer(lua_State *L)
{
	Timer *timer = Timer::get();
	if(!timer)
		return 0;
	
	int id;
	Lua::argParse(L, "i", &id);
	if (id > 0)
	{
		TimerMap::iterator iter = timer->timerMap_.find(id);
		if (iter != timer->timerMap_.end())
		{
			timer->timerMap_.erase(iter);
			lua_pushboolean(L, true);
			return 1;
		}
	}
	lua_pushboolean(L, false);
	return 1;
}

//ִ�ж�ʱ������
int Timer::update(unsigned int dtime)
{
	if (timerMap_.empty())
	{
		return 0;
	}

	//��ǰʱ��
	unsigned int curTime = timer_get_time();
	uint64 timeDiff = 0;
	
	int ret = 0;
	TimerMap::iterator iter = timerMap_.begin();
	for(; iter != timerMap_.end() ;)
	{
		ret = 1;
		stTime &timer = iter->second;
		//ʱ����
		timeDiff = GetTimeDiff(curTime, timer.lasttime);
		
		//ʱ�䵽��ִ�����ɾ��
		if(timer.expires <= 0 && timeDiff >= timer.cycle)
		{
			ret = callTimer(iter, timer.index, 0);
			timer.lasttime = curTime;
			//ret = 1;
		}
		//��Ч��ʱ�䵽
		else if( timer.expires > 0 && timeDiff >= timer.expires)
		{
			ret = callTimer(iter, timer.index, 0);
			timer.expires = 0;
			timer.lasttime = curTime;
		}

		if (ret != 1)
		{
			timerMap_.erase(iter++);
		}
		else
		{
			iter++;
		}
	}
	return 1;
}

//����lua�ű�
int Timer::callTimer(TimerMap::iterator& iter, unsigned int id, unsigned int diff)
{
	int res = 0;
	LuaSvr::get()->getRef("CHandlerTimer");
	lua_pushinteger(m_L, id);

	if (LuaSvr::scriptCall(m_L, 1, 1))
	{
		res = lua_tointeger(m_L, -1);
		lua_pop(m_L, 1);
	}
	return res;
}