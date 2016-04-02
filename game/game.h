#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <stdio.h>
#include "app.h"
#include "mqnet.h"
#include "buf.h"
#include "arch.h"
#include "net.h"
#include "log.h"
#include "luasvr.h"


class Game : public ServerApp
{
public:
	Game();
	~Game();

	void onInited();
	void onUpdate(unsigned int dtime);

};




#endif //GAME_SERVER_H
