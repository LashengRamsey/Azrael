cd router
start /b Router RouterConfig
cd ..

cd redis/Redis-win
del dump.rdb
start /b startup.bat
cd ../..

start /b db/DbServer db/DBConfig

start /b game/game game/GameConfig

