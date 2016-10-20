

/*
消息总线服务器

负责将接收到的消息转发给消息的订阅者

发送失败后将此订阅者移除掉

*/



//#include "fe.h"
//#include "global.h"
#include "server.h"
#include "mcs.h"
//#include "fileSys.h"
//#include "dbSys.h"
//#include "./MySQL/DbMySQL.h"
//#include "SSOpcodes.h"
//#include "command.h"


/*
分析ip地址和端口号  -p 端口号 -ip 地址
*/
void proc_arg(int argc, const char* argv[])
{

}


int main(int argc, const char* argv[])
{

	proc_arg(argc, argv);

	ServerInit();

	if (0 != ServerListen("0",8989))
	{
		printf("服务器启动失败\n");
		exit(0);
	}


	//进入程序主循环
	while (true)
	{
		ServerStep();
//		theWorld.MainLoop();
//		global_timer_pass.Step();
//		global_tick_delta = global_timer_pass.Interval();
//		GameAppUpdate(global_tick_delta);
//		GxSleep(200);
	}

	ServerClean();
	subscriber_clear();

	return 0;
}
