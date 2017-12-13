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
	Renderer *m_renderer;

	int Client_ID;
	ObjectGroup all_object;
	int connected_player;
	int cur_obstacle;
	int cur_resuorce;

	int retval;
	WSADATA wsa;
	SOCKADDR_IN serveraddr;
	SOCKET sock;
	WaitRoom wr;
	bool gamewait; // 게임 시작 신호시 FALSE

	//12.7 5:27
	CharacterBody *body;

	float prex;
	float prey;
	float targetx;
	float targety;
	float speedx;
	float speedy;
	float moveTime;

	int bodyLen;
	float bodyPrex[MAX_BODY_LENGTH], bodyPrey[MAX_BODY_LENGTH];
	
public:
	Object_Manager();
	void Update();
	void DrawAll();
	void RecvGameData(SOCKET s);
	void SendGameData(SOCKET s);
	void RecvGameState();

	
	//추가한 부분은 밑으로

	//12.7 오전 2시
	void MouseSet(float x, float y);
private:
	WaitRoom r;
	int ConnectWaitRoom(SOCKET server);
};