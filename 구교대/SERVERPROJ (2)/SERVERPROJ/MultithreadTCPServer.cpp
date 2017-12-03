#pragma comment(lib, "ws2_32")

#include <stdlib.h>
#include <stdio.h>
#include "time.h"
//#include "Object_struct.h"
#include "SockManager.h"
#define SERVERPORT 9000
#define BUFSIZE    512
#define WAITROOMBUFSIZE 4
int client_id = 0;
int error_id = -1;
ObjectGroup* server_ob;//서버의 오브젝트 그룹 
Sock_manager manager = Sock_manager(server_ob);//서버의 오브젝트 그룹의 포인터를 속 매니저에 등록
WaitRoom server_waitroom;//서버의 대기방 변수

bool acp = TRUE;//인원 초과시 FALSE
HANDLE GameThread;//게임 쓰레드의 시작여부
DWORD WINAPI SendRecvObjectInfo(LPVOID arg);
void AcceptClientSet(SOCKET s);
void AcceptClientGameObject(SOCKET s, ObjectGroup* GameObject);


//11.28 추가
int  globalvarObjPosNumberX = 0; //X좌표배열숫자 배열 0부터 넣어줌 객체가생성(res,obs)되면 전역변수값 증가.
int  globalvarObjPosNumberY = 0; //Y좌표배열숫자 배열 0부터 넣어줌 객체가생성(res,obs)되면 전역변수값 증가.
int  globalTime = 0; // 글로벌타임 food 생성자에 인자값필요해서. 아직 타임을어떻게할지 모르겠음. 글로벌 타임이 들어갈자리.
int x[1000];  //객체가 받을 좌표 배열.
int y[1000];  //객체가 받을 좌표 배열.
int globalfoodIndex;	//FOOD숫자
int globalResourceIndex; //OBJECT숫자
int globalCharacterIndex; //캐릭터숫자
void ObjectCreate(int res, int obs, ObjectGroup* server_ob); //자원과 장애물을 생성해야하는 개수
void InitObject(ObjectGroup* server_ob); //오브젝트 생성.
bool ObjStateCheck(ObjectGroup* server_ob); //오브젝트상태 오브젝트부족할시 생성.
void MakeRandomPos(void);	//랜덤좌표생성.
void err_quit(char *msg);
void err_display(char *msg);
//11.28 추가
void InitObject(ObjectGroup* server_ob)
{
	//캐릭터한개 초기화
	POINT pt;
	pt.x = x[globalvarObjPosNumberX++];
	pt.y = y[globalvarObjPosNumberY++];
	//포인트는 그냥 원하는위치잡고 넣어도됨 캐릭터는 
	Character* pCharacter = new Character(globalCharacterIndex, pt);
	ObjectCreate(15, 15, server_ob);// 원하는개수넣기.일단 99,99넣음
	server_ob->player[globalCharacterIndex++] = *pCharacter; //4개배열에 넣어준다.
	server_ob->playerCnt = globalCharacterIndex;
}


//11.28 추가
void ObjectCreate(int res, int obs, ObjectGroup* server_ob)
{
	MakeRandomPos();//포지션랜덤으로
					//객제좌표(중복되지않는 랜덤값을) point(좌표)에 집어넣어줌 
	POINT pt;
	for (int i = 0; i < res; i++)
	{
		pt.x = x[globalvarObjPosNumberX++];
		pt.y = y[globalvarObjPosNumberY++];
		POINT PointInsertres;
		PointInsertres.x = pt.x;
		PointInsertres.y = pt.y;
		//Food* pRes = new Food(globalTime, PointInsertres);
		server_ob->food[globalfoodIndex++] = *new Food(globalTime, PointInsertres); //500개배열에 넣어준다.
		server_ob->foodCnt = globalfoodIndex;
	}
	for (int i = 0; i < obs; i++)
	{
		pt.x = x[globalvarObjPosNumberX++];
		pt.y = y[globalvarObjPosNumberY++];
		POINT PointInsertObs;
		PointInsertObs.x = pt.x;
		PointInsertObs.y = pt.y;
		//Obstacle* pObs = new Obstacle(globalTime, PointInsertObs);
		server_ob->obs[globalResourceIndex++] = *new Obstacle(globalTime, PointInsertObs);//500개배열에 넣어준다.
		server_ob->obsCnt = globalResourceIndex;
	}
}


//11.28 추가
bool ObjStateCheck(ObjectGroup* server_ob)
{
	int foodCnt = 10;//설정한 개수보다작으면  ObjectCreate부른다.
	int fObstacleCnt = 10; //설정한 개수보다작으면  ObjectCreate부른다.
	if (server_ob->foodCnt < foodCnt)
	{
		//원래있던 랜덤값 배열개수에서 cnt개수만큼 빼줌
		//100개 50개사라짐 50개만큼 呼 난수 0으로 초기화 
		for (int i = globalvarObjPosNumberX; i < globalfoodIndex - foodCnt; i--)
		{
			globalvarObjPosNumberX--;
			globalvarObjPosNumberY--;
			globalfoodIndex--;
			x[globalvarObjPosNumberX] = 0; //랜덤값 0으로 초기화
			y[globalvarObjPosNumberX] = 0;
			MakeRandomPos(); // 0초기화된거 다시 랜덤값으로 채움
		}
		ObjectCreate(10, 0, server_ob); //개수만큼늘림.
	}
	if (server_ob->obsCnt < fObstacleCnt)
	{
		//원래있던 랜덤값 배열개수에서 cnt개수만큼 빼줌
		//100개 50개사라짐 50개만큼 呼 난수 0으로 초기화 
		for (int i = globalvarObjPosNumberX; i < globalResourceIndex - fObstacleCnt; i--)
		{
			globalvarObjPosNumberX--;
			globalvarObjPosNumberY--;
			globalResourceIndex--;
			x[globalvarObjPosNumberX] = 0; //랜덤값 0으로 초기화
			y[globalvarObjPosNumberX] = 0; //랜덤값 0으로 초기화
			MakeRandomPos(); // 0초기화된거 다시 랜덤값으로 채움
		}
		ObjectCreate(0, 10, server_ob); //개수만큼늘림.
	}

	//각각의 객체의 스테이트는 배열로 루프돌려서 찾음 아직잘모르겠음
	//3개이상의 캐릭터몸체의 길이가 0 이되면 리턴false한다
	//0,1,2가 살아남을경우를 넣어서.
	for (int i = 0; i < globalCharacterIndex; i++)
	{
		int iTempCnt = 0;
		if ((server_ob->player[i].getbodylength() == 0))
			iTempCnt++;//3명이0이면 
		if (iTempCnt == 3)
		{
			return false;
		}
	}
}



//11.28 추가
void MakeRandomPos(void)
{
	int Xarea = 800; // x영역 범위지정 
	int Yarea = 800; // y영역 범위지정

	int i, j;
	int bFound;
	int ObjPosNumber = 100; //랜덤좌표 100개 배열에 넣음 
	srand((unsigned int)time(NULL));
	for (i = 0; i < ObjPosNumber; ++i)
	{
		while (1)
		{
			x[i] = rand() % Xarea + 1 -400;
			y[i] = rand() % Yarea + 1 -400;
			bFound = 0;
			// 같은값이있는지확인한다
			for (j = 0; j < i; ++j)
			{
				// 같은값이있으면
				if (x[j] == x[i] || y[j] == y[i])
				{
					bFound = 1;
					break;
				}
			}
			// 같은값이없으면while문탈출
			if (!bFound)
			{
				break;
			}
		}
	}
}


// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
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
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
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
	GameThread = CreateEvent(NULL, TRUE, FALSE, NULL);//비신호 상태로 시작, 수동 리셋

	server_waitroom.room_state = 0;
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (acp) {//acp 현재 인원이 다 차있는 상태라면 FALSE
				 // accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		/*printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));*/

		AcceptClientSet(client_sock);// 클라이언트 접속시 id 부여, 클래스에 셋
		manager.SendGameState(server_waitroom);// 게임 상태 전송

		//11.28추가
		if (acp) {
			//계속 오브젝트그룹 주고받기.
			AcceptClientGameObject(client_sock, server_ob);
			ObjStateCheck(server_ob);

		}
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void AcceptClientSet(SOCKET s) {

	int retval;
	acp = manager.ClientsockSet(s);//아직 인원이 덜 찼을때는 acp -> TRUE

	if (!acp) {//인원초과
		retval = send(s, (char*)&error_id, sizeof(int), 0);//인원초과 시 아이디에 -1 전송
		closesocket(s);
		return;
	}

	retval = send(s, (char*)&client_id, sizeof(int), 0);//아이디 전송
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
	printf("방 설정을 입력 받았습니다. 최대 인원 %d\n", server_waitroom.max_player);
}

//11.28추가 전체데이터를 받는다.
void AcceptClientGameObject(SOCKET s, ObjectGroup* GameObject)
{
	while (1) {
		int retval;
		retval = send(s, (char*)&GameObject, sizeof(GameObject), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;

		}
		retval = recv(s, (char*)GameObject, sizeof(GameObject), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		//클라로부터  GameObject구조체정보받고 구조체형변환 
		GameObject = (ObjectGroup*)GameObject;
	}


}
// 클라이언트와 데이터 통신
DWORD WINAPI SendRecvObjectInfo(LPVOID arg)
{

	SOCKET s = (SOCKET)arg;
	int retval;
	int cur_id;
	Character rec_char = Character();//캐릭터를 받아올 버퍼용 변수 선언
	CharacterBody* char_body;
	//printf("대기중\n");

	WaitForSingleObject(GameThread, INFINITE);

	//event 핸들 
	//printf("시작");
	while (1) {
		retval = recvn(s, (char *)&rec_char, sizeof(rec_char), 0);//recv
		cur_id = rec_char.get_id();
		char_body = rec_char.get_body();

		manager.RecvClientCaracter(cur_id, char_body);

		//다른 클라리언트로 부터 다 받았을때 까지 wait 할 필요있음 

		retval = send(s, (char*)&server_ob, sizeof(server_ob), 0);//send
	}


	return 0;
}

