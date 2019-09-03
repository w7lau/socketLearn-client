// socketLearn.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//



#include "pch.h"
#include <iostream>
//#include<WinSock2.h>
//#include<Windows.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>
#include<string.h>
#include<thread>
#include<fcntl.h>
#include<vector>
#include<fstream>
#include <sys/epoll.h>
#include <sys/time.h>
#define CONNECT_NUM_MAX 50
#define CHAR_MAX 40
#define STRUCT_MAX 44
//#pragma comment(lib,"ws2_32.lib")
using namespace std;

struct lData
{
	char name[CHAR_MAX]={0};
	int time=2;
};

struct sData
{
	char name[CHAR_MAX] = {0};
	int num = 0;
};

vector<lData> Read_list()
{
	lData Temp_ldata;
	vector<lData> ldata;
	ifstream read_l("list.xml");
	char list_name[CHAR_MAX];
	int list_time;
	string temps;
	while(getline(read_l,temps))
	{
		//string temps;
		//read_l>>temps;
		/* for(int i=0;i<temps.length();i++)
		{
			list_name[i]=temps[i];
		}
		 */
		 int whe=temps.find(' ',0);
		 cout<<"whe: "<<whe<<endl;
		 cout<<"temps: "<<temps<<endl;
		 for(int i=0;i<whe;i++)
		 {
			 list_name[i]=temps[i];
			 //cout<<" list_name[i]: "<< list_name[i]<<endl;
		 }
		 list_name[whe]='\0';
		 char* end;
		 list_time=static_cast<int>(strtol(temps.substr(whe).c_str(),&end,10));
		//read_l>>list_name;
		//read_l>>list_time;
		//Temp_ldata.name=list_name;
		memcpy(Temp_ldata.name,list_name,sizeof(list_name));
		Temp_ldata.time=list_time;
		ldata.push_back(Temp_ldata);
		cout<<"ID："<<list_name<<"  犯罪次数："<<list_time<<endl;
	};
	read_l.close();
	return ldata;
}

void Write_zuixing(string name,string zuixing)
{
	string filename=name+".xml";
	cout<<"filename:"<<filename<<endl;
	ofstream infile(filename,ios::app);
	time_t tt;
    time( &tt );
    tt = tt + 8*3600;  // transform the time zone
    tm* t= gmtime( &tt );
	infile<<t->tm_year + 1900<<"-"<<t->tm_mon + 1<<"-"<<t->tm_mday<<"_"<<t->tm_hour<<":"<<t->tm_min<<" "<<zuixing<<endl;
	infile.close();
	
	
	vector<lData> ldata;
	vector<lData>::iterator it;
	ldata=Read_list();
			//cout<<"test::"<<ldata.size()<<endl;
	ofstream linfile("list.xml");
	int is_new=0;
	for(it=ldata.begin();it!=ldata.end();it++)
	{
		if(it->name==name)
		{
			it->time++;
			is_new=1;
		}
		linfile<<it->name<<" "<<it->time<<endl;
	}
	if(is_new==0)
	{
		linfile<<name<<" "<<1<<endl;
	}
	linfile.close();
	
}

void T_x(int connSocket)
{

		char buff[CHAR_MAX]={0};
	    sData sendd;
        strcpy(sendd.name,"连接服务器成功………………");
        sendd.num=99;
		//cout << "发送：" << sendd.name << "死亡次数：" << sendd.num << endl;
        memset(buff,0,CHAR_MAX);
        memcpy(buff,&sendd,sizeof(sendd));
		//printf("%s",buff);
		//int i=send(connSocket, buff, sizeof(buff), 0);
        //cout<<i<<endl;
		//while(1)
		//{
		//cout<<"in while"<<endl;
		char recv_buf[STRUCT_MAX];
        memset(recv_buf,0,STRUCT_MAX);
		sData recd;
	while(1)
	{   
        //cout<<"in while"<<endl;
        //int i=recv(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
		int i=recv(connSocket, recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
 		if(i>0)
		{
		//cout<<i<<endl;
		memcpy(&recd,recv_buf,sizeof(recd));
		int b=strcmp(recd.name,"_connect_");
		//cout<<b<<endl;
		if(strcmp(recd.name,"_connect_")==0)
		{
			cout << "接收到消息：" << recd.name << " num: "<<recd.num<< endl;
			//cout<<"1111111111111"<<endl;
			//send(connSocket, buff, sizeof(buff), MSG_DONTWAIT);
			send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
			//cout<<"333333333333"<<endl;
		}
		else if(strcmp(recd.name,"list")==0)
		{
			
			//lData sendl;
			cout << "接收到消息list：" << recd.name << " num: "<<recd.num<< endl;
			strcpy(sendd.name,"List_begining");
			send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
			vector<lData> ldata;
			vector<lData>::iterator it;
			ldata=Read_list();
			//cout<<"test::"<<ldata.size()<<endl;
			for(it=ldata.begin();it!=ldata.end();it++)
			{
				//cout<<"test:"<<it->name<<" "<<it->time<<endl;
				memcpy(sendd.name,it->name,sizeof(ldata[0].name));
			    sendd.num=it->time;
				
				//printf("buff:%s\n",buff);
			    //send(connSocket, buff, sizeof(buff), MSG_DONTWAIT);
				send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
			}
			strcpy(sendd.name,"List_ending");
			send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
			//cout << "接收到消息：" << recd.name <<  endl;
			//send(connSocket, buff, sizeof(buff), MSG_DONTWAIT);
		}
		else if(strcmp(recd.name,"read")==0)
		{
			cout << "接收到消息read：" << recd.name << " num: "<<recd.num<< endl;
			strcpy(sendd.name,"Name_begining");
			send(connSocket, (char*)&sendd, sizeof(sendd), 0);
			memset(recv_buf,0,STRUCT_MAX);
			int k=1;
			while(k)
			{
				int kk=recv(connSocket, recv_buf, sizeof(recv_buf), 0);
				if(kk>0)
				{
					k=0;
				}
			}
			
			memcpy(&recd,recv_buf,sizeof(recd));
			cout<<"收到ID: "<<recd.name<<" num:"<<recd.num<<endl;
			vector<lData> ldata;
			vector<lData>::iterator it;
			ldata=Read_list();
			for(it=ldata.begin();it!=ldata.end();it++)
			{
				cout<<"it.name:"<<it->name<<endl;
				if((it->name==recd.name)==0)
				{
					//cout<<"it.name:"<<it->name<<endl;
					char xmln[]=".xml";
					//char file_name[44]=strcat(recd.name,xmln);
					ifstream Read_name(strcat(recd.name,xmln));
					cout<<"recd.name:"<<recd.name<<endl;
					//char temps[CHAR_MAX];
					string temps;
					while(getline(Read_name,temps))
					{
					//Read_name>>temps;
					int whe=temps.find(' ',0);
					cout<<"temps:"<<temps<<endl;
					int i=0;
					for(i;i<whe;i++)
					{
						sendd.name[i]=temps[i];
					}
					sendd.name[whe]='\0';
			        send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
					memset(sendd.name,0,CHAR_MAX);
					for(i=whe;i<temps.length();i++)
					{
						sendd.name[i-whe]=temps[i];
					}
					sendd.name[temps.length()-whe]='\0';
			        send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
					}
					Read_name.close();
				}
			}
			strcpy(sendd.name,"Name_ending");
			send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
			
		}
		else if(strcmp(recd.name,"write")==0)
		{
			cout << "接收到消息write：" << recd.name << " num: "<<recd.num<< endl;
			strcpy(sendd.name,"Write_begining");
			send(connSocket, (char*)&sendd, sizeof(sendd), 0);
			memset(recv_buf,0,STRUCT_MAX);
			
			string N_ame;
			string Z_uixing;
			
			int i = 0;
			cout<<"i: "<<i<<endl;
			while(i<=0)
			{
				//cout<<"i1: "<<i<<endl;
				i =recv(connSocket, recv_buf, sizeof(recv_buf), 0);
			}
			memcpy(&recd,recv_buf,sizeof(recd));
			cout<<"ID:"<<recd.name<<endl;
			N_ame=recd.name;//记录ID

			
			strcpy(sendd.name,"Write_midding");
			send(connSocket, (char*)&sendd, sizeof(sendd), 0);
			//memset(recv_buf,0,STRUCT_MAX);
			//cout<<"i3: "<<i<<endl;
			//i =recv(connSocket, recv_buf, sizeof(recv_buf), 0);
			//memset(recv_buf,0,STRUCT_MAX);
			cout<<"i2: "<<i<<endl;
			i=0;
			while(i<=0)
			{
				//cout<<"i2: "<<i<<endl;
				i =recv(connSocket, recv_buf, sizeof(recv_buf), 0);
			}
			memcpy(&recd,recv_buf,sizeof(recd));
			cout<<"i: "<<i<<endl;
			//recv(connSocket, recv_buf, sizeof(recv_buf), 0);
			cout<<"罪行:"<<recd.name<<endl;
			Z_uixing=recd.name;//罪行
			Write_zuixing(N_ame,Z_uixing);
			
		}
		else
		{
			cout << "接收到消息：" << recd.name << "num: "<<recd.num<< endl;
			strcpy(sendd.name,"acc");
			sendd.num=5;
			//cout<<"222222222222"<<endl;
			send(connSocket, (char*)&sendd, sizeof(sendd), MSG_DONTWAIT);
			//cout<<"44444444444"<<endl;
		}
		 memset(buff,0,CHAR_MAX);
		 memset(recv_buf,0,STRUCT_MAX);
		 i=0;
		//send(connSocket, buff, sizeof(buff), 0);
		   
	}
        

	
}
	
}

void T_c(struct epoll_event	events[],struct epoll_event ev,int sock_s,int epoll_fd,socklen_t sin_size)
{
	int nfds=0;
	int connSocket;
	struct sockaddr_in addrClinet;
	int if_T =1;
	while(1)
		{
			//cout<<"-------------"<<endl;
			nfds=epoll_wait(epoll_fd,events,CONNECT_NUM_MAX,5000);
			//cout<<"nfds:"<<nfds<<endl;
			if(nfds==-1)
			{
				perror("epoll启动失败");
				exit(EXIT_FAILURE);
			}
			//cout<<"nfds:"<<nfds<<endl;
			for(int i=0;i<nfds;i++)
			{
				//cout<<"nfds:"<<nfds<<endl;
				//cout<<"========"<<endl;
				if(events[i].data.fd==sock_s)
					
				{
					cout<<"sock_s:"<<sock_s<<endl;
					if(connSocket=accept4(sock_s,(struct sockaddr*)&addrClinet,&sin_size,SOCK_NONBLOCK))
					{if(connSocket<0)
						{
						perror("接收客户端socket连接失败");
						exit(EXIT_FAILURE);
						}
						else
						{
							cout<<"connSocket:"<<connSocket<<endl;
						}
					}
					//cout<<"+++++++++++"<<endl;
					ev.events=EPOLLIN|EPOLLET;
					ev.data.fd=connSocket;//原sock_s，暂时替换看效果
					//int flags = fcntl(connSocket, F_GETFL, 0); 
					//cout<<"flag:"<<flags<<endl;
					//fcntl(connSocket, F_SETFL, flags | O_NONBLOCK);
					//cout<<"!flag:"<<flags<<endl;
					//int flags1= fcntl(connSocket, F_GETFL, 0);
					if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connSocket,&ev)==-1)
					{
						perror("客户端socket池子注册失败");
						exit(EXIT_FAILURE);
					}
					cout<<"收到连接："<<inet_ntoa(addrClinet.sin_addr)<<endl;
					//T_x(connSocket);
					if(if_T==1)
					{
					thread t(T_x,connSocket);
					thread tc(T_c,events,ev,sock_s,epoll_fd,sin_size);
		            t.join();
					//cout<<"in thread1"<<endl;
					tc.join();
		            tc.detach();
		            t.detach(); 
					if_T=0;
					}

					
				}
			    else
			    {
				//cout<<"else111111"<<endl;
				
			    }
		    }
		}
		close(connSocket);//测试关闭
}

int main()
{
        
	//WORD ver = MAKEWORD(2, 2);
	//WSADATA dat;
	//WSAStartup(ver, &dat);
    struct sockaddr_in addrSrv;
	
	int sock_s = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
	//SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);

	//绑定
	if (bind(sock_s, (struct sockaddr*)&addrSrv, sizeof(addrSrv)))
	{
		std::cout << "绑定失败...." << std::endl;
		return 0;
	}
	else
	{
		std::cout << "绑定成功...." << std::endl;
	}


	//监听
	if (listen(sock_s, CONNECT_NUM_MAX))
	{
		std::cout << "监听失败...." << std::endl;
	}
	else
	{
		std::cout << "监听成功...." << std::endl;
	}
	struct sockaddr_in clientAddr;
	int len = sizeof(clientAddr);
/* 
		int connSocket = accept(sock_s, (struct sockaddr*)NULL, NULL);
		if (connSocket == -1)
		{
			cout << "连接失败！" << endl;
			
		}
		else
		{
			cout << "连接成功！" << endl;
		
		} */
		int epoll_fd=epoll_create(CONNECT_NUM_MAX);
		socklen_t sin_size=sizeof(sockaddr_in);
        
        if(epoll_fd==-1)
		{
			perror("epoll创建失败");
			exit(EXIT_FAILURE);
		}
        struct epoll_event ev;
        struct epoll_event	events[CONNECT_NUM_MAX];
        ev.events=EPOLLIN|EPOLLET;
        ev.data.fd=sock_s;
		if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sock_s,&ev)==-1)
		{
			perror("sock_s注册失败");
			exit(EXIT_FAILURE);
			
		}
		//thread tc(T_c,events,ev,sock_s,epoll_fd,sin_size);
		//tc.join();
		//tc.detach();
		T_c(events,ev,sock_s,epoll_fd,sin_size);
		/* thread t(T_x,connSocket);
		t.join();
		t.detach(); */
		

    cout<<"now endding"<<endl;
    getchar();
	close(sock_s);
	getchar();
}
