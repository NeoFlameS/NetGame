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

WaitRoom Object_Manager::ConnectWaitRoom(SOCKET server) {

	WaitRoom r;

	int retval = recvn(server,(char*)&Client_ID,sizeof(int),0);
	if (retval == SOCKET_ERROR) {
		err_display("recvn()");
	}
	else if (Client_ID < 0) {
		printf("서버에 접속할 수 없습니다. 인원이 초과 되었습니다.");
	}

	

	printf("%d\n", Client_ID);

	if (Client_ID == 0) { // 처음 접속시 방설정  
		printf("방의 인원수를 설정 하십시오.\n");
		scanf_s("%d", &r.max_player);
		r.current_player = 1;
		r.room_state = 1;

		retval = send(server, (char*)&r, sizeof(r), 0);//설정한 방 정보 전송
		if (retval == SOCKET_ERROR) err_quit("send()");
	}

	return r;

}

Object_Manager::Object_Manager()
{
	//all_object.player;

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
	printf("%f \n", time);
}
void Object_Manager::DrawAll()
{
	return;
}

void Object_Manager::RecvGameData(SOCKET s)
{
	return;
}
void Object_Manager::SendGameData(SOCKET s)
{
	return;
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

// 소켓 함수 오류 출력
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