#pragma once
//#include <winsock2.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Object_struct.h"

class Sock_manager {
private:
	SOCKET client[4];
	HANDLE client_thread[4];//�������� �ڵ�
	HANDLE event_thread[4];//�������� �̺�Ʈ �ڵ�
	HANDLE update_handle[4];//���ν������� ��� ���� ������ �˸��� �̺�Ʈ �ڵ�
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
