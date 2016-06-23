module("gateService", package.seeall)


function GameClientConnection(src, packet)
	print("=======GameClientConnection======== sn=" .. src)
	print_r(packet)
	Session.newSession(packet.sn)
end

function GameClientDisConn(src, packet)
	print("=======GameClientDisConn======== sn=" .. src)
	print_r(packet)
	Session.delSession(packet.sn)	
end


