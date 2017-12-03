#pragma comment(lib, "ws2_32")

#include <stdlib.h>
#include <stdio.h>
//#include "Object_struct.h"
#include "SockManager.h"
#define SERVERPORT 9000
#define BUFSIZE    512

int client_id = 0;
int error_id = -1;
ObjectGroup server_ob;//서버의 오브젝트 그룹
Sock_manager manager = Sock_manager(&server_ob);//서버의 오브젝트 그룹의 포인터를 속 매니저에 등록
WaitRoom server_waitroom;//서버의 대기방 변수

bool acp = TRUE;//인원 초과시 FALSE
HANDLE GameThread;//게임 쓰레드의 시작여부
DWORD WINAPI SendRecvObjectInfo(LPVOID arg);
void AcceptClientSet(SOCKET s,HANDLE handle);
// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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
	GameThread = CreateEvent(NULL,TRUE,FALSE,NULL);//비신호 상태로 시작, 수동 리셋
	
	server_waitroom.room_state = 0;
	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while(acp){//acp 현재 인원이 다 차있는 상태라면 FALSE
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		/*printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));*/

		hThread = CreateThread(NULL, 0, SendRecvObjectInfo, (LPVOID)client_sock, 0, NULL);

		AcceptClientSet(client_sock,hThread);// 클라이언트 접속시 id 부여, 클래스에 셋
		manager.SendGameState(server_waitroom);// 게임 상태 전송

		if (client_id == server_waitroom.max_player) {
			acp = FALSE;
		}
		/*if (acp) {
			 
		}*/
		
	}
	printf("신호상태\n");
	SetEvent(GameThread);//게임 시작, 데이터 송수신 쓰레드를 시작한다

	bool running = TRUE;
	while (TRUE) {
		//게임 루프
	}


	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void AcceptClientSet(SOCKET s,HANDLE handle) {
	
	int retval;
	acp = manager.ClientsockSet(s, handle);//아직 인원이 덜 찼을때는 acp -> TRUE
	
	if (!acp) {//인원초과
		retval = send(s, (char*)&error_id, sizeof(int), 0);//인원초과 시 아이디에 -1 전송
		closesocket(s);
		return;
	}

	retval = send(s,(char*)&client_id,sizeof(int),0);//아이디 전송
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	if (client_id == 0 && server_waitroom.room_state == 0) {//첫번째 클라이언트가 들어왔을때
		retval = recvn(s, (char*)&server_waitroom, sizeof(server_waitroom), 0);//set waitroom
		//server_waitroom.current_player = 1;
		//server_waitroom.max_player = server_waitroom.max_player;
		//server_waitroom.room_state = 1;//0: 방이 없을 때 1: 방이 존재함 2: 풀방일때
		manager.SetWaitRoom(&server_waitroom);
	}
	
	client_id++;
	printf("client ID : %d\n", client_id);
	printf("방 설정을 입력 받았습니다. 최대 인원 %d\n",server_waitroom.max_player);
}


// 클라이언트와 데이터 통신
DWORD WINAPI SendRecvObjectInfo(LPVOID arg)
{

	SOCKET s = (SOCKET)arg;
	int retval;
	int cur_id;
	Character rec_char = Character();//캐릭터를 받아올 버퍼용 변수 선언
	CharacterBody* char_body;
	printf("대기중\n");

	WaitForSingleObject(GameThread,INFINITE);
	
	//event 핸들 
	
	while(1){
	printf("시작");
	retval = recvn(s,(char *)&rec_char,sizeof(rec_char),0);//recv
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	cur_id = rec_char.get_id();
	char_body = rec_char.get_body();

	manager.RecvClientCaracter(cur_id,char_body);
	manager.SetEventthis(cur_id);

	//다른 클라리언트로 부터 다 받았을때 까지 wait 할 필요있음 
	manager.SendWait();//WaitForMultipleObjects() 를 실행하는 메소드 호출

	retval = send(s, (char*)&server_ob, sizeof(server_ob), 0);//send
	if (retval == SOCKET_ERROR) {
		err_display("send()");}
	//send 이후에 리셋
	manager.ResetEventthis(cur_id);

	}


	return 0;
}

