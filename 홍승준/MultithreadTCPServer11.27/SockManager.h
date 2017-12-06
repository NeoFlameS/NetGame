#pragma once
//#include <winsock2.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Object_struct.h"

class Sock_manager {
private:
	SOCKET client[4];
	HANDLE client_thread[4];//스레드의 핸들
	HANDLE event_thread[4];//스레드의 이벤트 핸들
	HANDLE update_handle[4];//메인스레드의 계산 시작 시점을 알리는 이벤트 핸들
	int current_player;
	WaitRoom *wa = NULL;

	ObjectGroup* GameObject;
public :
	HANDLE Recv_wait;

	Sock_manager(ObjectGroup* ob);
	bool ClientsockSet(SOCKET s, HANDLE handle);
	int get_current_player();
	void SetWaitRoom(WaitRoom *w);
	int SendGameState(WaitRoom r);

	void RecvClientCaracter(int id, CharacterBody* ch);

	void SendWait();
	void MainWait();
	void ResetEventthis(int id);
	void SetEventthis(int id);
	void SetUpdateHandle(int id);
};
