#include <mutex>
#include <string>
#include <fstream>
#include <vector>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")


#include "protocol.h"


//全局变量定义
std::string g_str;
std::mutex g_mutex;	//全局互斥锁

char* str_text;	//文本内容

//套接字列表
std::vector<SOCKET> socket_list;
//房间号
std::vector<std::vector<int>> room_list = { {0,0},{0,0},{0,0},{0,0},{0,0},{0,0} };
//玩家列表
std::vector<std::vector<std::string>> player_list = { {"",""},{"",""},{"",""},{"",""},{"",""},{"",""}};


//获取房间信息
void getRoomList(SOCKET client_socket, PDU& uiPDU)
{

	for (auto it : socket_list)
	{
		uiPDU.uiPDUType = GET_ROOM_LIST_SUCCESS;
		for (int i = 0; i < sizeof(uiPDU.roomList) / sizeof(uiPDU.roomList[0]); i++)
		{
			if (i >= room_list.size() * 2) break;
			uiPDU.roomList[i] = room_list[i / 2][i % 2];
		}
		send(it, (char*)&uiPDU, uiPDU.uiPDULen, 0);
	}
}

//获取游戏中信息
void getGameInfo(SOCKET client_socket, PDU& uiPDU)
{
	if (room_list[uiPDU.room_id][0] == 0 || room_list[uiPDU.room_id][1] == 0)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(g_mutex);		//互斥锁

	uiPDU.uiPDUType = GET_GAME_INFO_SUCCESS;
	strcpy_s(uiPDU.caData, sizeof(uiPDU.caData), "get game info success");
	if (room_list[uiPDU.room_id][0] == (int)client_socket)
	{
        send(room_list[uiPDU.room_id][1], (char*)&uiPDU, uiPDU.uiPDULen, 0);
	}
	else if (room_list[uiPDU.room_id][1] == (int)client_socket)
	{
		send(room_list[uiPDU.room_id][0], (char*)&uiPDU, uiPDU.uiPDULen, 0);
	}

}

//创建房间
void createRoom(SOCKET client_socket, PDU& uiPDU)
{
	std::lock_guard<std::mutex> lock(g_mutex);		//互斥锁

	if (room_list.size() > 6)
	{
		uiPDU.uiPDUType = CREATE_ROOM_FAIL;
		strcpy_s(uiPDU.caData, sizeof(uiPDU.caData), "create room fail: all room is full");
	}
	else
	{
		for (int i = 0; i < room_list.size(); i++)
		{
			if (room_list[i][0] == (int)client_socket)
			{
				std::cout << "create room fail: room is have" << room_list[i][0] << " " << (int)client_socket << std::endl;
				uiPDU.uiPDUType = CREATE_ROOM_FAIL;
				strcpy_s(uiPDU.caData, sizeof(uiPDU.caData), "create room fail: room is have");
				send(client_socket, (char*)&uiPDU, uiPDU.uiPDULen, 0);
				return;
			}
		}

		for (int i = 0; i < room_list.size(); i++)
		{
			if (room_list[i][0] == 0 && room_list[i][1] == 0)
			{
				room_list[i][0] = (int)client_socket;
				uiPDU.room_id = i;
				player_list[i][0] = uiPDU.caData;
				std::cout << "create room: " << room_list[i][0] << " " << client_socket << std::endl;

				//将房间创建消息发送给所有人
				getRoomList(client_socket, uiPDU);
				break;
			}
		}


		std::cout << "create room: " << client_socket << std::endl;
		std::cout << "player_name: " << uiPDU.caData << std::endl;

		uiPDU.uiPDUType = CREATE_ROOM_SUCCESS;
		strcpy_s(uiPDU.caData, sizeof(uiPDU.caData), "create room success");
	}
	
	for (auto it : room_list)
	{
        std::cout << it[0] << " " << it[1] << std::endl;
	}
	for (auto it : player_list)
	{
        std::cout << it[0] << " " << it[1] << std::endl;
	}

	send(client_socket, (char*)&uiPDU, uiPDU.uiPDULen, 0);
}

//取消房间
void cancelRoom(SOCKET client_socket, PDU& uiPDU)
{
	std::lock_guard<std::mutex> lock(g_mutex);		//互斥锁

	for (int i = 0; i < room_list.size(); i++)
	{
		if (room_list[i][0] == (int)client_socket)
		{
			std::cout << "delete room: " << room_list[i][0] << std::endl;
			room_list[i][0] = 0;
			player_list[i][0] = "";

			uiPDU.uiPDUType = CANCEL_ROOM_SUCCESS;
			strcpy_s(uiPDU.caData, sizeof(uiPDU.caData), "cancel room success");
			send(client_socket, (char*)&uiPDU, uiPDU.uiPDULen, 0);

			//将房间创建消息发送给所有人
			getRoomList(client_socket, uiPDU);

			break;

		}
	}

}

//更新房间列表
void updateRoomList(SOCKET client_socket, PDU& uiPDU)
{

	uiPDU.uiPDUType = UPDATE_ROOM_LIST_SUCCESS;
	for (int i = 0; i < sizeof(uiPDU.roomList) / sizeof(uiPDU.roomList[0]); i++)
	{
		if (i >= room_list.size() * 2) break;
		uiPDU.roomList[i] = room_list[i / 2][i % 2];
	}
	send(client_socket, (char*)&uiPDU, uiPDU.uiPDULen, 0);

}

//加入房间
void joinRoom(SOCKET client_socket, PDU& uiPDU)
{
	std::lock_guard<std::mutex> lock(g_mutex);		//互斥锁

	if (room_list[uiPDU.room_id][0] == 0) return;

	for (int i = 0; i < room_list.size(); i++)
	{
		if (room_list[i][0] == (int)client_socket)
		{
			room_list[i][0] = 0;
			player_list[i][0] = "";

			//将房间更新消息发送给所有人
			getRoomList(client_socket, uiPDU);
		}
	}
	if (room_list[uiPDU.room_id][0] != 0 && room_list[uiPDU.room_id][1] == 0)
	{
		std::cout << client_socket << "join room: " << room_list[uiPDU.room_id][0] << std::endl;
        room_list[uiPDU.room_id][1] = (int)client_socket;
        player_list[uiPDU.room_id][1] = uiPDU.caData;

		uiPDU.uiPDUType = JOIN_ROOM_SUCCESS;
		uiPDU.player_game_id = 2;
		uiPDU.opposing_player = 0;
		strcpy_s(uiPDU.caData, sizeof(uiPDU.caData), player_list[uiPDU.room_id][0].c_str());
		send(client_socket, (char*)&uiPDU, uiPDU.uiPDULen, 0);

		//发给房间内的人
		uiPDU.player_game_id = 1;
		strcpy_s(uiPDU.caData, sizeof(uiPDU.caData), player_list[uiPDU.room_id][1].c_str());
		send(room_list[uiPDU.room_id][0], (char*)&uiPDU, uiPDU.uiPDULen, 0);

		//将房间更新消息发送给所有人
		getRoomList(client_socket, uiPDU);
	}
	else
	{
		std::cout << client_socket << "join room fail" << std::endl;
	}
	
}

//退出选择界面
void exitRoom(SOCKET client_socket, PDU& uiPDU)
{
	std::lock_guard<std::mutex> lock(g_mutex);		//互斥锁
	//在套接字列表中移除
	for (int i = 0; i < socket_list.size(); i++)
	{
		if (socket_list[i] == client_socket)
		{
			for (int i = 0; i < room_list.size(); i++)
			{
				if (room_list[i][0] == (int)client_socket)
				{
					std::cout << "delete room: " << room_list[i][0] << std::endl;
					room_list[i][0] = 0;
					player_list[i][0] = "";

				}
			}

			std::cout << "delete socket: " << client_socket << std::endl;
			socket_list.erase(socket_list.begin() + i);

			//6 关闭socket
			closesocket(client_socket);
			std::cout << "client_socket: " << client_socket << " quit" << std::endl;

			break;
		}
	}
}

//退出游戏lan界面
void exitGameLan(SOCKET client_socket, PDU& uiPDU)
{
	if (room_list[uiPDU.room_id][0] == 0 || room_list[uiPDU.room_id][1] == 0)
	{
		return;
	}
	std::lock_guard<std::mutex> lock(g_mutex);		//互斥锁

	uiPDU.uiPDUType = EXIT_GAME_LAN_SUCCESS;
	if (room_list[uiPDU.room_id][0] == (int)client_socket)
	{
		send(room_list[uiPDU.room_id][1], (char*)&uiPDU, uiPDU.uiPDULen, 0);
	}
	else if (room_list[uiPDU.room_id][1] == (int)client_socket)
	{
		send(room_list[uiPDU.room_id][0], (char*)&uiPDU, uiPDU.uiPDULen, 0);
	}

	//在房间列表中移除
	room_list[uiPDU.room_id][0] = 0;
	room_list[uiPDU.room_id][1] = 0;

	player_list[uiPDU.room_id][0] = "";
    player_list[uiPDU.room_id][1] = "";
	
	//将房间更新消息发送给所有人
	getRoomList(client_socket, uiPDU);

}


//多线程
DWORD WINAPI threadFunc(LPVOID lpParam)
{
	SOCKET client_socket = *(SOCKET*)lpParam;
	free(lpParam);		//释放创建的socket内存

	socket_list.push_back(client_socket);
	for (auto it : socket_list)
		std::cout << it << std::endl;

	std::ifstream file("text.txt");		//打开文本文件
	if (!file.good())
	{
		MessageBox(nullptr, L"无法打开文本文件 text.txt", L"启动失败", MB_OK | MB_ICONERROR);
		return -1;
	}
	std::stringstream str_stream;	//创建字符串流
	str_stream << file.rdbuf();		//将文件内容读入流中
	std::string str = str_stream.str();	//将流中的内容转换为字符串
    str_text = (char*)str.c_str();

	//std::cout << "str: " << str << " " << str.length() << std::endl;
	//std::cout << "char* : " << str_text << " " << strlen(str_text) << std::endl;

	file.close();		//关闭文件

	PDU* uiPDU = mkPDU((uint)strlen(str_text) + 1);
	//std::cout << strlen(str_text) << std::endl;
	uiPDU->room_id = (int)client_socket;
	uiPDU->uiPDUType = LOGIN_SUCCESS;
	strcpy_s(uiPDU->caData,"login success");
	memcpy(uiPDU->strMsg, str_text, strlen(str_text));
	
	std::cout << uiPDU->uiMSGLen << " " << uiPDU->strMsg << " " << strlen(uiPDU->strMsg) << std::endl;

	//登录成功
	send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);		//将文本内容发给客户端
	free(uiPDU);
	uiPDU = nullptr;

	
	//5 通信(B/S)
	while (1)
	{
		uint pduLen = 0;
		int ret = recv(client_socket, (char*)&pduLen, sizeof(uint), 0);		//读取PDU长度
		if (ret <= 0) break;

		uiPDU = mkPDU(pduLen - sizeof(PDU));
		ret = recv(client_socket, (char*)uiPDU + sizeof(uint), pduLen - sizeof(uint), 0);		//读取剩余PDU内容
		if (ret <= 0) break;

		switch (uiPDU->uiPDUType)
		{
		case GET_GAME_INFO:
            getGameInfo(client_socket, *uiPDU);
			break;
		case CREATE_ROOM:
			createRoom(client_socket, *uiPDU);
			break;
		case EXIT_ROOM:
			exitRoom(client_socket, *uiPDU);
			break;
		case CANCEL_ROOM:
			cancelRoom(client_socket, *uiPDU);
			break;
		case UPDATE_ROOM_LIST:
			updateRoomList(client_socket, *uiPDU);
			break;
        case JOIN_ROOM:
			joinRoom(client_socket, *uiPDU);
			break;
		case EXIT_GAME_LAN:
            exitGameLan(client_socket, *uiPDU);
			break;
		default:
			break;
		}

		Sleep(50);
	}

	//在套接字列表中移除
	for (int i = 0; i < socket_list.size(); i++)
	{
		if (socket_list[i] == client_socket)
		{
			std::cout << "delete socket: " << client_socket << std::endl;
			socket_list.erase(socket_list.begin() + i);
		}
		if (room_list[i][0] == (int)client_socket)
		{
            std::cout << "delete room: " << room_list[i][0] << player_list[i][0] << std::endl;
			room_list[i][0] = 0;
			player_list[i][0] = "";
		}
		if (room_list[i][1] == (int)client_socket)
		{
			std::cout << "delete room: " << room_list[i][1] << player_list[i][0] << std::endl;
			room_list[i][1] = 0;
			player_list[i][0] = "";
		}
	}
	//6 关闭socket
	closesocket(client_socket);
	std::cout << "client_socket: " << client_socket << " quit" << std::endl;

	free(uiPDU);

	for (auto it : socket_list)
		std::cout << it << std::endl;

	return 0;
}


int main()
{
	std::cout << "Server started" << std::endl;
	//windows上使用网络需要开始网络权限
	WSADATA wsaData;
	int iResual = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResual != 0)
	{
        std::cout << "WSAStartup error!!!" << "errcode:" << GetLastError() << std::endl;
		return -1;
	}

	//1 创建socket套接字
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		std::cout << "create listen socket error!!!" << "errcode:" << GetLastError() << std::endl;
	}
	std::cout << "listen_socket: " << (int)listen_socket << std::endl;

	//2 给socket绑定地址和端口
	struct sockaddr_in local = { 0 };
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);	//htons 将主机字节序转为网络字节序，大小端序不同，需要转换
	//INADDR_ANY 表示任意地址
	//local.sin_addr.s_addr = htonl(INADDR_ANY);	
	local.sin_addr.s_addr = inet_addr("0.0.0.0");	//inet_addr 将点分十进制IP地址转为网络字节

	//绑定套接字
	if (-1 == bind(listen_socket, (struct sockaddr*)&local, sizeof(local)))
	{
		std::cout << "bind error!!!" << "errcode:" << GetLastError() << std::endl;
		return -1;
	}

	//3 监听
	if (-1 == listen(listen_socket, 10))	//10表示最大连接数
	{
		std::cout << "listen error!!!" << "errcode:" << GetLastError() << std::endl;
		return -1;
	}

	//4 接受客户端连接
	//返回的client_socket是客户端的socket,直接与客户端通信
	//阻塞函数，没有客户端连接会一直等待
	while (1)
	{
		SOCKET client_socket = accept(listen_socket, NULL, NULL);
		if (INVALID_SOCKET == client_socket)
		{
			std::cout << "accept error!!!" << "errcode:" << GetLastError() << std::endl;
			continue;
		}
		std::cout << "client_socket: " << client_socket << " jion" << std::endl;



		//创建线程
		SOCKET* sockfd = (SOCKET*)malloc(sizeof(SOCKET));
		if (sockfd == NULL)
		{
            std::cout << "malloc error!!!" << "errcode:" << GetLastError() << std::endl;
			return -1;
		}
		*sockfd = client_socket;		
		//将客户端socket传递给线程
		CreateThread(NULL, 0, threadFunc, sockfd, 0, NULL);


	}


	return 0;
}
