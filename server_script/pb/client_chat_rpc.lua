module(..., package.seeall)
	
client2chat = {
	rpcChatUp = {client_chat_pb.sendMsg, public_pb.fake},
	rpcBanChannelReq = {public_pb.fake, public_pb.fake},
	rpcBanChannelSet = {client_chat_pb.banChannelMsg, public_pb.fake},
	rpcHeartbeat = {public_pb.fake, base_pb.bool_}
}

chat2client = {
	rpcChatDown = {client_chat_pb.receiveMsg, public_pb.fake},
	rpcBanChannelRes = {client_chat_pb.banChannelMsg, public_pb.fake},
	rpcModFastChat = {client_chat_pb.fastChatInfo, public_pb.fake},
	rpcDelFastChat = {base_pb.int32_, public_pb.fake}
}
