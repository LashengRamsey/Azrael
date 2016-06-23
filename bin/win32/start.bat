cd router
start /b Router RouterConfig
cd ..

cd redis/Redis-win
del dump.rdb
start /b startup.bat
cd ../..

start /b db/DbServer db/DBConfig commConfig

start /b game/game game/GameConfig commConfig

start "gateServer" game/game gate/GateConfig commConfig
