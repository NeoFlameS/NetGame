#pragma once
//#include <winsock2.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Object_struct.h"

class Sock_manager {
private:
	SOCKET client[4];
	HANDLE client_thread[4];
	int current_player;
	WaitRoom *wa = NULL;

	ObjectGroup GameObject;
public :
	HANDLE Recv_wait;

	Sock_manager();
	bool ClientsockSet(SOCKET s);
	int get_current_player();
	void SetWaitRoom(WaitRoom *w);
	int SendGameState(WaitRoom r);

	void RecvClientCaracter(int id, CharacterBody* ch);
};
