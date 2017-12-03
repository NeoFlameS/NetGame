#pragma comment(lib, "ws2_32")

#include <stdlib.h>
#include <stdio.h>
//#include "Object_struct.h"
#include "SockManager.h"
#define SERVERPORT 9000
#define BUFSIZE    512

int client_id = 0;
int error_id = -1;
ObjectGroup server_ob;//������ ������Ʈ �׷�
Sock_manager manager = Sock_manager(&server_ob);//������ ������Ʈ �׷��� �����͸� �� �Ŵ����� ���
WaitRoom server_waitroom;//������ ���� ����

bool acp = TRUE;//�ο� �ʰ��� FALSE
HANDLE GameThread;//���� �������� ���ۿ���
DWORD WINAPI SendRecvObjectInfo(LPVOID arg);
void AcceptClientSet(SOCKET s,HANDLE handle);
// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

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


int main(int argc, char *argv[])
{
	int retval;
	GameThread = CreateEvent(NULL,TRUE,FALSE,NULL);//���ȣ ���·� ����, ���� ����
	
	server_waitroom.room_state = 0;
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while(acp){//acp ���� �ο��� �� ���ִ� ���¶�� FALSE
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		/*printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));*/

		hThread = CreateThread(NULL, 0, SendRecvObjectInfo, (LPVOID)client_sock, 0, NULL);

		AcceptClientSet(client_sock,hThread);// Ŭ���̾�Ʈ ���ӽ� id �ο�, Ŭ������ ��
		manager.SendGameState(server_waitroom);// ���� ���� ����

		if (client_id == server_waitroom.max_player) {
			acp = FALSE;
		}
		/*if (acp) {
			 
		}*/
		
	}
	printf("��ȣ����\n");
	SetEvent(GameThread);//���� ����, ������ �ۼ��� �����带 �����Ѵ�

	bool running = TRUE;
	while (TRUE) {
		//���� ����
	}


	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

void AcceptClientSet(SOCKET s,HANDLE handle) {
	
	int retval;
	acp = manager.ClientsockSet(s, handle);//���� �ο��� �� á������ acp -> TRUE
	
	if (!acp) {//�ο��ʰ�
		retval = send(s, (char*)&error_id, sizeof(int), 0);//�ο��ʰ� �� ���̵� -1 ����
		closesocket(s);
		return;
	}

	retval = send(s,(char*)&client_id,sizeof(int),0);//���̵� ����
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	if (client_id == 0 && server_waitroom.room_state == 0) {//ù��° Ŭ���̾�Ʈ�� ��������
		retval = recvn(s, (char*)&server_waitroom, sizeof(server_waitroom), 0);//set waitroom
		//server_waitroom.current_player = 1;
		//server_waitroom.max_player = server_waitroom.max_player;
		//server_waitroom.room_state = 1;//0: ���� ���� �� 1: ���� ������ 2: Ǯ���϶�
		manager.SetWaitRoom(&server_waitroom);
	}
	
	client_id++;
	printf("client ID : %d\n", client_id);
	printf("�� ������ �Է� �޾ҽ��ϴ�. �ִ� �ο� %d\n",server_waitroom.max_player);
}


// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI SendRecvObjectInfo(LPVOID arg)
{

	SOCKET s = (SOCKET)arg;
	int retval;
	int cur_id;
	Character rec_char = Character();//ĳ���͸� �޾ƿ� ���ۿ� ���� ����
	CharacterBody* char_body;
	printf("�����\n");

	WaitForSingleObject(GameThread,INFINITE);
	
	//event �ڵ� 
	
	while(1){
	printf("����");
	retval = recvn(s,(char *)&rec_char,sizeof(rec_char),0);//recv
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	cur_id = rec_char.get_id();
	char_body = rec_char.get_body();

	manager.RecvClientCaracter(cur_id,char_body);
	manager.SetEventthis(cur_id);

	//�ٸ� Ŭ�󸮾�Ʈ�� ���� �� �޾����� ���� wait �� �ʿ����� 
	manager.SendWait();//WaitForMultipleObjects() �� �����ϴ� �޼ҵ� ȣ��

	retval = send(s, (char*)&server_ob, sizeof(server_ob), 0);//send
	if (retval == SOCKET_ERROR) {
		err_display("send()");}
	//send ���Ŀ� ����
	manager.ResetEventthis(cur_id);

	}


	return 0;
}

