//#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include "stdafx.h"
#include "ObjectManager.h"

int recvn(SOCKET s, char *buf, int len, int flags) {
	int received;
	char *ptr = buf;
	int left = len;

	
	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		else if (received == 0) {
			break;
		}
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void Object_Manager::ConnectWaitRoom(SOCKET server) {

	int retval = recvn(server,(char*)&Client_ID,sizeof(int),0);
	if (retval == SOCKET_ERROR) {
		err_display("recvn()");
	}
	else if (Client_ID < 0) {
		printf("������ ������ �� �����ϴ�. �ο��� �ʰ� �Ǿ����ϴ�.");
	}

	
	printf("%d\n", Client_ID);

	if (Client_ID == 0) { // ó�� ���ӽ� �漳��  
		printf("���� �ο����� ���� �Ͻʽÿ�.\n");
		scanf_s("%d", &wr.max_player);
		wr.current_player = 1;
		wr.room_state = 1;

		retval = send(server, (char*)&wr, sizeof(wr), 0);//������ �� ���� ����
		if (retval == SOCKET_ERROR) err_quit("send()");
	}

}

Object_Manager::Object_Manager()
{
	int i = 0;
	for (i = 0; i < 4; i++) {
		this->player[i] = Character();
	}
	//all_object.player;
	this->gamewait =  TRUE;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) exit(1);

	// socket()
	this->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	else this->ConnectWaitRoom(sock);
}



void Object_Manager::Update(float time)
{
	//printf("%f \n", time);
}
void Object_Manager::DrawAll()
{
	return;
}

void Object_Manager::RecvGameData()
{
	int i = 0;
	for(i = 0; i<wr.max_player; i++){
	retval = recvn(this->sock, (char *)&player[i], sizeof(Character), 0);
	if (retval == SOCKET_ERROR) {
		err_quit("ĳ���� ���� ����");
	}
	}
	printf("ĳ���� ���� ���� �Ϸ�\n");
	for (i = 0; i < wr.max_player; i++) {
		printf("%d %d %d %d\n", player[i].get_body()->locate.x, player[i].get_body()->locate.y, player[i].get_id(), Client_ID);
	}
	
	return;
}
void Object_Manager::SendGameData()
{
	retval = send(this->sock, (char *)&player[this->Client_ID], sizeof(Character),0);
	if (retval == SOCKET_ERROR) {
		err_quit("ĳ���� ���� ����");
	}
	return;
}

void Object_Manager::RecvGameState() {
	while(1){
	retval = recvn(this->sock,(char *)&wr,sizeof(wr),0);
	if (retval == SOCKET_ERROR) {
		err_quit("recv()");
	}
	if (wr.current_player == wr.max_player) {
		this->gamewait = FALSE;
		printf("����");
		break;
	}
	}
}

void Object_Manager::err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void Object_Manager::err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}