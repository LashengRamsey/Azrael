#include <stdio.h>
#include "zmq.h"
//#include "tclap/CmdLine.h"
#include "arch.h"
#include <string.h>
#include "Config.h"

#define PT_SERVER_START 100050
#define PT_SERVER_OFFLINE 100051
#define PT_SERVER_STOP 100052


int digi = 0;

int main(int argc, char *argv[])
{
#ifdef WIN32
#	ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#	endif
#endif
	signalIgn();

	if (argc<2)
	{
		printf("argc = %d,not Config\n", argc);
		return false;
	}
	printf("router server Config path:%s\n",argv[1]);
	//�����ļ�·��
	Config::SetConfigName(argv[1]);

	//myName_ = Config::GetValue("ServerID");

	//TCLAP::CmdLine cmdLine_("Command description message", ' ', "1.2.1");
	//TCLAP::ValueArg<std::string> nameArg("n", "name", "Name of server", true, "router", "string");
	//TCLAP::ValueArg<std::string> sockArg("s", "sock", "sock path", false, "tcp://*", "string");
	//TCLAP::ValueArg<std::string> wpathArg("w", "workpath", "work pathr", true, "config/router", "string");
	//TCLAP::ValueArg<std::string> portArg("p", "port", "bind port", false, "10001", "string");
	//TCLAP::SwitchArg daemonArg("d", "daemon", "daemon mode", false);
	//cmdLine_.add(nameArg);
	//cmdLine_.add(sockArg);
	//cmdLine_.add(wpathArg);
	//cmdLine_.add(portArg);
	//cmdLine_.add(daemonArg);
	//cmdLine_.parse(argc, argv);
	//std::string name = nameArg.getValue();
	//std::string sockpath = sockArg.getValue();
	//std::string workpth = wpathArg.getValue();
	//std::string port = portArg.getValue();

	std::string name = Config::GetValue("ServerID");
	std::string sockpath = Config::GetValue("SocketPath");
	std::string workpth = Config::GetValue("PidFile");
	std::string port = Config::GetValue("BindPort");
	
	//��̨����
	int dameon = Config::GetIntValue("Dameon");
	if (dameon)
	{
		be_daemon();
	}
	//����pid
	write_pid(workpth.c_str());

	char path[MAX_PATH] = {0};
	if (strcmp(sockpath.c_str(), "tcp://") >= 0)
	{
		snprintf(path, MAX_PATH, "%s:%s", sockpath.c_str(), port.c_str());
	}
	else
	{
		snprintf(path, MAX_PATH, "%s/router_%s", sockpath.c_str(), name.c_str());
	}

	//============================================
	void *context = zmq_init(1);

	//ZMQ_ROUTER socket�յ���Ϣʱ������Ϣջ�ϼ�һ�������Ϣ��Դ��ַ����Ϣ��
	//������Ϣʱ���Ὣ��һ����Ϣȡ����������Ϊ���͵�Ŀ�ĵ�
	void *router = zmq_socket(context, ZMQ_ROUTER);

	int rc = 0;
	if ((rc = zmq_bind(router, path)) != 0)
	{
		printf("router bind %s failed:%d!\n", path, rc);
		return 1;
	}
	printf("router bind %s ok!\n", path);
	//============================================


	while(1)
	{
		int source;	//���ͷ�����ID
		zmq_recv(router, &source, 4, 0);

		zmq_msg_t message;
		zmq_msg_init(&message);
		if ((rc = zmq_recvmsg(router, &message, 0)) < 0)
		{
			printf("recv message error:%d\n", rc);
			continue;
		}

		int fid, target;
		size_t len = zmq_msg_size(&message);
		if (len < 8)
		{
			printf("recv message less then 8\n");
			continue;
		}

		memcpy(&target, (char*)zmq_msg_data(&message), 4);	//Ŀ�������ID
		memcpy(&fid, (char*)zmq_msg_data(&message)+4, 4);	//��Ϣ����

		if (fid == PT_SERVER_START)
		{
			if (digi == 0 && len == 12)
			{
				digi = *((int*)zmq_msg_data(&message)+2);
			}

			//int sendLen = 0;
			//��֡��Ϣ
			zmq_send(router, &source, sizeof(source), ZMQ_SNDMORE);
			int data[3] = {source, fid, digi};
			zmq_send(router, &data, sizeof(data), 0);
			//printf("PT_SERVER_START send len = %d\n", sendLen);
		}
		else if (fid == PT_SERVER_STOP)
		{
			printf("server %d stop\n", source);
		}
		else
		{
			//ָ�����ڱ����͵���Ϣ�Ǹ���֡��Ϣ�����Һ��滹�и������Ϣ����з���
			zmq_send(router, &target, 4, ZMQ_SNDMORE);

			zmq_msg_t data;
			zmq_msg_init_size(&data, len);

			memcpy(zmq_msg_data(&data), &source, 4);
			memcpy((char*)zmq_msg_data(&data)+4, (char*)zmq_msg_data(&message)+4, len-4);

			//ָ�����β���ʹ�÷�������ʽִ�С�
			//��������Ϣ���ܱ���ӵ�Ŷ��Ϣ�������棬zmq_msg_send()������ִ��ʧ�ܣ�
			//������errno��ֵΪEAGAIN.��

			zmq_sendmsg(router, &data, ZMQ_DONTWAIT);
			zmq_msg_close(&data);
		}
		zmq_msg_close(&message);
	}

	zmq_close(router);
	zmq_term(context);

	return 0;
}


