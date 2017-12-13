//#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include "stdafx.h"
#include "ObjectManager.h"

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

int Object_Manager::ConnectWaitRoom(SOCKET server) {

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

	
	return 1;

}

Object_Manager::Object_Manager()
{
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
	else ConnectWaitRoom(sock);


	prex = 0;
	prey = 0;
	speedx = 0.0;
	speedy = 0.0;
	bodyLen = 0;

	moveTime = 0;
	
	m_renderer=new Renderer(800, 800);
	if (!m_renderer->IsInitialized())
	{
		std::cout << "SceneMgr::Renderer could not be initialized.. \n";
	}
}



void Object_Manager::Update()
{
	if (gamewait)
	{
		RecvGameState();
		return;
	}
	//12.7오전2:27 테스트기 때문에 지워도됨
	printf("sds");
	retval= send(this->sock, (char *)&Client_ID, sizeof(Client_ID), 0);
	if (retval == SOCKET_ERROR) err_display("objecterror");
	//12.7 오전 4:10
	printf("번번 %d\n",Client_ID);
	retval = recvn(this->sock, (char *)&all_object, sizeof(all_object), 0);
	if (retval == SOCKET_ERROR) err_display("allobject");

	all_object.player[Client_ID].Draw();
	/*Character aa[4];
	retval= recvn(this->sock, (char *)&aa, sizeof(aa), 0);
	if (retval == SOCKET_ERROR) err_display("allobject");
	
	body = aa[Client_ID].get_body();
	bodyLen =aa[Client_ID].get_bodyLength();
	printf("sds");
	if (moveTime > 0)
	{
		--moveTime;
		for (int i = bodyLen - 1; i > 0; --i)
		{
			body[i].locate.x = bodyPrex[i];
			body[i].locate.y = bodyPrey[i];
			bodyPrex[i] = bodyPrex[i - 1];
			bodyPrey[i] = bodyPrey[i - 1];
		}
		if (moveTime == 0)
		{
			bodyPrex[0] = targetx;
			bodyPrey[0] = targety;
			body[0].locate.x = targetx;
			body[0].locate.y = targety;
		}
		else
		{
			bodyPrex[0] = body[0].locate.x + speedx;
			bodyPrey[0] = body[0].locate.y + speedy;
			body[0].locate.x = bodyPrex[0];
			body[0].locate.y = bodyPrey[0];
		}

	}
	printf("sds22");
	retval = send(this->sock, (char *)&aa, sizeof(aa), 0);
	printf("sds");*/
}
void Object_Manager::DrawAll()
{
	for (int i = 0; i < bodyLen; ++i)
	{
		//m_renderer->DrawSolidRect(body[i].locate.x, body[i].locate.y, 0, 20, 0, 1, 1, 1, 0.1);
	}
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

void Object_Manager::RecvGameState() {
	printf("웨잇룸 받기");

	retval = recvn(this->sock,(char *)&wr,sizeof(wr),0);
	if (retval == SOCKET_ERROR) {
		err_quit("recv()");
	}
	if (wr.current_player == wr.max_player) {
		this->gamewait = FALSE;
		printf("시작");
	}
	
	printf("웨잇룸 받기 종료 죵료");
}


void Object_Manager::MouseSet(float x, float y)
{
	
	prex = body[0].x;//3.33
	prey = body[0].y;
	targetx = x;
	targety = y;

	float tx = x - prex;
	float ty = y - prey;

	moveTime = (int)(sqrt(tx*tx + ty*ty) / 3.33);
	printf("%d\n", moveTime);

	printf("x: %f y: %f\n", tx, ty);

	speedx = tx / moveTime;
	speedy = ty / moveTime;

}