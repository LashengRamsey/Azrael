module(..., package.seeall)

--上行的
tUp={
	client_chat_rpc.client2chat,
	client_main_rpc.client2main,
	client_scene_rpc.client2scene
}


--下行的
tDown={
	client_chat_rpc.chat2client,
	client_main_rpc.main2client,
	client_scene_rpc.scene2client
}