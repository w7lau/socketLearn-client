#include "pch.h"
#include <iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<string>
#include<thread>
#include <codecvt>
#define CONNECT_NUM_MAX 10
#define CHAR_MAX 40
#define STRUCT_MAX 40

#pragma comment(lib,"ws2_32.lib")
using namespace std;

string utf8_to_gbk(const string &str)
{
	wstring_convert<codecvt_utf8<wchar_t>> utf8_cvt; // utf8-》unicode转换器
	wstring_convert<codecvt<wchar_t, char, mbstate_t>> gbk_cvt(new codecvt<wchar_t, char, mbstate_t>("chs")); // unicode-》gbk转换器
	wstring t = utf8_cvt.from_bytes(str);
	return gbk_cvt.to_bytes(t);
}

// gbk转utf8
string gbk_to_utf8(const string &str)
{
	wstring_convert<codecvt_utf8<wchar_t>> utf8_cvt; // utf8-》unicode转换器
	wstring_convert<codecvt<wchar_t, char, mbstate_t>> gbk_cvt(new codecvt<wchar_t, char, mbstate_t>("chs")); // unicode-》gbk转换器
	wstring t = gbk_cvt.from_bytes(str);
	return utf8_cvt.to_bytes(t);
}

struct lData
{
	char name[CHAR_MAX] = { 0 };
	int time=0;
};

struct sData
{
	char name[CHAR_MAX] = { 0 };
	int num = 0;
};
void Writemsg(sData sendd, SOCKET sock_c,string zl="请输入指令：")
{
	string ta;
	cout << zl;
	cin >> ta;
	//cout << gbk_to_utf8(ta) << endl;
	strcpy_s(sendd.name, gbk_to_utf8(ta).c_str());
	//cout << "发送：" << ta << endl;
	send(sock_c, (char*)&sendd, sizeof(sendd), 0);
}
void T_x()
{

	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	SOCKET sock_c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("45.76.5.164");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);

	//绑定
	if (0 != connect(sock_c, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{
		std::cout << "连接失败……" << std::endl;
		cout << "可能卡了，重新开一下，可能被墙了，可能服务器没话费了" << endl;
		return;
	}
	else
	{
		std::cout << "连接成功……" << std::endl;
		sData sendd = { "_connect_",9 };
		send(sock_c, (char*)&sendd, sizeof(sendd), 0);
		
	}
	int i=0;
	while (1)
	{
		sData recd{};
		//cout << "i0:" << i << endl;
		i = recv(sock_c, (char*)&recd, sizeof(recd), 0);
		while (i>0)
		{
			//cout <<"i1:"<< i<<endl;
			//cout << "ID：" << utf8_to_gbk(recd.name) << endl;
			//cout << "ID：" << utf8_to_gbk(recd.name) << "  死亡次数：" << recd.num << endl;
			sData sendd = { " ",3 };
			
			if (utf8_to_gbk(recd.name) == "连接服务器成功………………")
			{
				cout << "当前可以使用指令：list、read、write" << endl;
				cout  << utf8_to_gbk(recd.name) <<endl;
				Writemsg(sendd, sock_c);
				i = 0;
			}
			else if(utf8_to_gbk(recd.name) =="List_begining")
			{
				//char recv_buf[1024] = "";

				i = recv(sock_c, (char*)&recd, sizeof(recd), 0);
				//cout << "text：" << utf8_to_gbk(recd.name) << "  number：" << recd.num << endl;
				//i = recv(sock_c, recv_buf, sizeof(recv_buf), 0);
				while (i>0)
				{
					if (utf8_to_gbk(recd.name) == "List_ending")
					{
						//cout << "ID：" << utf8_to_gbk(recd.name) << "  死亡次数：" << recd.num << endl;
						i = 0;
						Writemsg(sendd, sock_c);
						//cout << "here" << endl;
						break;
					}
					else
					{
						cout << "ID：" << utf8_to_gbk(recd.name) << "  犯罪次数：" << recd.num << endl;
						i = recv(sock_c, (char*)&recd, sizeof(recd), 0);
					}
				}
				
				
			}
			else if (utf8_to_gbk(recd.name) == "Name_begining")
			{
				
				Writemsg(sendd, sock_c,"请输入要查看的ID：");
				i = recv(sock_c, (char*)&recd, sizeof(recd), 0);
				while (i>0)
				{
					if (utf8_to_gbk(recd.name) == "Name_ending")
					{
						i = 0;
						Writemsg(sendd, sock_c);
						//cout << "here111" << endl;
						break;
					}
					else
					{
						//char t1=''
						string sss= utf8_to_gbk(recd.name);
						for (int i = 0; i < sss.length(); i++)
						{
							if (sss[i]=='_')
							{
								sss[i] = ' ';
							}
						}
						cout << "时间：" << sss;
						i = recv(sock_c, (char*)&recd, sizeof(recd), 0);
						cout << "\t罪行：" << utf8_to_gbk(recd.name) << endl;
						i = recv(sock_c, (char*)&recd, sizeof(recd), 0);
					}
				}

			}
			else if (utf8_to_gbk(recd.name) == "Write_begining")
			{
				//cout << "text：" << utf8_to_gbk(recd.name) << "  number：" << recd.num << endl;
				Writemsg(sendd, sock_c, "请输入要记录的ID：");
				recv(sock_c, (char*)&recd, sizeof(recd), 0);
				while (utf8_to_gbk(recd.name) != "Write_midding")
				{
					recv(sock_c, (char*)&recd, sizeof(recd), 0);
				}
				//cout << "text：" << utf8_to_gbk(recd.name) << "  number：" << recd.num << endl;
				Writemsg(sendd, sock_c, "请输入要记录的罪行：");
				cout << "记录成功" << endl;
				//i = recv(sock_c, (char*)&recd, sizeof(recd), 0);
			}
			else
			{
				//cout << "name：" << utf8_to_gbk(recd.name) << "  num：" << recd.num << endl;
				Writemsg(sendd, sock_c);
				i = 0;
			}
			
			//cout << "接收到消息：" << recd.name << endl;
			
			//cout << "i2:" << i << endl;
		}
	}
	closesocket(sock_c);
}
int main()
{

	//char recvBuf[100];
	//recv(sock_c, recvBuf, sizeof(recvBuf), 0);
	//cout << "接收到消息："<<recvBuf << endl;

	//char sendBuf[100];
	//strcpy_s(sendBuf, "aaaaaa");
	//cout << "发送：" << sendBuf << endl;
	//send(sock_c, sendBuf, strlen(sendBuf) + 1, 0);

	cout << "自动化的功能算法问题好麻烦，先当一个云公用记事本用着吧，其他的以后再说吧……" << endl;
	cout << "还有好多细节暂时懒得弄了，例如如果发一半发现卡着不动了，可能卡住了，没写重连，重启吧，重连以后再说……" << endl;

	thread *T = new thread(T_x);
	T->join();
	T->detach();

		
	system("pause");

		//recv(sock_c, (char*)&recd, sizeof(recd), 0);
		//cout << "接收到消息：" << utf8_to_gbk(recd.name) << endl;

	
	WSACleanup();
	getchar();
	getchar();
}

