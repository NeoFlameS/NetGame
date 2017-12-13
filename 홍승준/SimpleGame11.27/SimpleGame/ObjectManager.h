#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <winsock2.h>

#include <stdlib.h>
#include <stdio.h>
#include "Renderer.h"
#include "Object_struct.h"
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    50


class Object_Manager {
	int Client_ID;
	//ObjectGroup all_object;
	int connected_player;
	int cur_obstacle;
	int cur_resuorce;

	Character player[4];

	int retval;
	WSADATA wsa;
	SOCKADDR_IN serveraddr;
	SOCKET sock;
	WaitRoom wr;
	bool gamewait; // 게임 시작 신호시 FALSE
public:
	Object_Manager();
	void Update(float time);
	void DrawAll();
	void RecvGameData();
	void SendGameData();
	void err_quit(char *msg);
	void err_display(char *msg);
	void RecvGameState();
	//추가한 부분은 밑으로
private:
	//WaitRoom r;
	void ConnectWaitRoom(SOCKET server);
};