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
ObjectGroup* server_ob;//������ ������Ʈ �׷� 
Sock_manager manager = Sock_manager(server_ob);//������ ������Ʈ �׷��� �����͸� �� �Ŵ����� ���
WaitRoom server_waitroom;//������ ���� ����

bool acp = TRUE;//�ο� �ʰ��� FALSE
HANDLE GameThread;//���� �������� ���ۿ���
DWORD WINAPI SendRecvObjectInfo(LPVOID arg);
void AcceptClientSet(SOCKET s);
void AcceptClientGameObject(SOCKET s, ObjectGroup* GameObject);


//11.28 �߰�
int  globalvarObjPosNumberX = 0; //X��ǥ�迭���� �迭 0���� �־��� ��ü������(res,obs)�Ǹ� ���������� ����.
int  globalvarObjPosNumberY = 0; //Y��ǥ�迭���� �迭 0���� �־��� ��ü������(res,obs)�Ǹ� ���������� ����.
int  globalTime = 0; // �۷ι�Ÿ�� food �����ڿ� ���ڰ��ʿ��ؼ�. ���� Ÿ����������� �𸣰���. �۷ι� Ÿ���� ���ڸ�.
int x[1000];  //��ü�� ���� ��ǥ �迭.
int y[1000];  //��ü�� ���� ��ǥ �迭.
int globalfoodIndex;	//FOOD����
int globalResourceIndex; //OBJECT����
int globalCharacterIndex; //ĳ���ͼ���
void ObjectCreate(int res, int obs, ObjectGroup* server_ob); //�ڿ��� ��ֹ��� �����ؾ��ϴ� ����
void InitObject(ObjectGroup* server_ob); //������Ʈ ����.
bool ObjStateCheck(ObjectGroup* server_ob); //������Ʈ���� ������Ʈ�����ҽ� ����.
void MakeRandomPos(void);	//������ǥ����.
void err_quit(char *msg);
void err_display(char *msg);
//11.28 �߰�
void InitObject(ObjectGroup* server_ob)
{
	//ĳ�����Ѱ� �ʱ�ȭ
	POINT pt;
	pt.x = x[globalvarObjPosNumberX++];
	pt.y = y[globalvarObjPosNumberY++];
	//����Ʈ�� �׳� ���ϴ���ġ��� �־�� ĳ���ʹ� 
	Character* pCharacter = new Character(globalCharacterIndex, pt);
	ObjectCreate(45, 45, server_ob);// ���ϴ°����ֱ�.�ϴ� 99,99����
	server_ob->player[globalCharacterIndex++] = *pCharacter; //4���迭�� �־��ش�.
	server_ob->playerCnt = globalCharacterIndex;
}


//11.28 �߰�
void ObjectCreate(int res, int obs, ObjectGroup* server_ob)
{
	MakeRandomPos();//�����Ƿ�������
					//������ǥ(�ߺ������ʴ� ��������) point(��ǥ)�� ����־��� 
	POINT pt;
	for (int i = 0; i < res; i++)
	{
		pt.x = x[globalvarObjPosNumberX++];
		pt.y = y[globalvarObjPosNumberY++];
		POINT PointInsertres;
		PointInsertres.x = pt.x;
		PointInsertres.y = pt.y;
		Food* pRes = new Food(globalTime, PointInsertres);
		server_ob->food[globalfoodIndex++] = *pRes; //500���迭�� �־��ش�.
		server_ob->foodCnt = globalfoodIndex;
	}
	for (int i = 0; i < obs; i++)
	{
		pt.x = x[globalvarObjPosNumberX++];
		pt.y = y[globalvarObjPosNumberY++];
		POINT PointInsertObs;
		PointInsertObs.x = pt.x;
		PointInsertObs.y = pt.y;
		Obstacle* pObs = new Obstacle(globalTime, PointInsertObs);
		server_ob->obs[globalResourceIndex++] = *pObs;//500���迭�� �־��ش�.
		server_ob->obsCnt = globalResourceIndex;
	}
}


//11.28 �߰�
bool ObjStateCheck(ObjectGroup* server_ob)
{
	int foodCnt = 10;//������ ��������������  ObjectCreate�θ���.
	int fObstacleCnt = 10; //������ ��������������  ObjectCreate�θ���.
	if (server_ob->foodCnt < foodCnt)
	{
		//�����ִ� ������ �迭�������� cnt������ŭ ����
		//100�� 50������� 50����ŭ ���� ���� 0���� �ʱ�ȭ 
		for (int i = globalvarObjPosNumberX; i < globalfoodIndex - foodCnt; i--)
		{
			globalvarObjPosNumberX--;
			globalvarObjPosNumberY--;
			globalfoodIndex--;
			x[globalvarObjPosNumberX] = 0; //������ 0���� �ʱ�ȭ
			y[globalvarObjPosNumberX] = 0;
			MakeRandomPos(); // 0�ʱ�ȭ�Ȱ� �ٽ� ���������� ä��
		}
		ObjectCreate(30, 0, server_ob); //������ŭ�ø�.
	}
	if (server_ob->obsCnt < fObstacleCnt)
	{
		//�����ִ� ������ �迭�������� cnt������ŭ ����
		//100�� 50������� 50����ŭ ���� ���� 0���� �ʱ�ȭ 
		for (int i = globalvarObjPosNumberX; i < globalResourceIndex - fObstacleCnt; i--)
		{
			globalvarObjPosNumberX--;
			globalvarObjPosNumberY--;
			globalResourceIndex--;
			x[globalvarObjPosNumberX] = 0; //������ 0���� �ʱ�ȭ
			y[globalvarObjPosNumberX] = 0; //������ 0���� �ʱ�ȭ
			MakeRandomPos(); // 0�ʱ�ȭ�Ȱ� �ٽ� ���������� ä��
		}
		ObjectCreate(0, 30, server_ob); //������ŭ�ø�.
	}

	//������ ��ü�� ������Ʈ�� �迭�� ���������� ã�� �����߸𸣰���
	//3���̻��� ĳ���͸�ü�� ���̰� 0 �̵Ǹ� ����false�Ѵ�
	//0,1,2�� ��Ƴ�����츦 �־.
	for (int i = 0; i < globalCharacterIndex; i++)
	{
		int iTempCnt = 0;
		if ((server_ob->player[i].getbodylength() == 0))
			iTempCnt++;//3����0�̸� 
		if (iTempCnt == 3)
		{
			return false;
		}
	}
}



//11.28 �߰�
void MakeRandomPos(void)
{
	int Xarea = 100; // x���� �������� 
	int Yarea = 100; // y���� ��������

	int i, j;
	int bFound;
	int ObjPosNumber = 100; //������ǥ 100�� �迭�� ���� 
	srand((unsigned int)time(NULL));
	for (i = 0; i < ObjPosNumber; ++i)
	{
		while (1)
		{
			x[i] = rand() % Xarea + 1;
			y[i] = rand() % Yarea + 1;
			bFound = 0;
			// ���������ִ���Ȯ���Ѵ�
			for (j = 0; j < i; ++j)
			{
				// ��������������
				if (x[j] == x[i] || y[j] == y[i])
				{
					bFound = 1;
					break;
				}
			}
			// �������̾�����while��Ż��
			if (!bFound)
			{
				break;
			}
		}
	}
}


// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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
	GameThread = CreateEvent(NULL, TRUE, FALSE, NULL);//���ȣ ���·� ����, ���� ����

	server_waitroom.room_state = 0;
	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (acp) {//acp ���� �ο��� �� ���ִ� ���¶�� FALSE
				 // accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		/*printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));*/

		AcceptClientSet(client_sock);// Ŭ���̾�Ʈ ���ӽ� id �ο�, Ŭ������ ��
		manager.SendGameState(server_waitroom);// ���� ���� ����

		//11.28�߰�
		if (acp) {
			//��� ������Ʈ�׷� �ְ�ޱ�.
			AcceptClientGameObject(client_sock, server_ob);
			ObjStateCheck(server_ob);

		}
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

void AcceptClientSet(SOCKET s) {

	int retval;
	acp = manager.ClientsockSet(s);//���� �ο��� �� á������ acp -> TRUE

	if (!acp) {//�ο��ʰ�
		retval = send(s, (char*)&error_id, sizeof(int), 0);//�ο��ʰ� �� ���̵� -1 ����
		closesocket(s);
		return;
	}

	retval = send(s, (char*)&client_id, sizeof(int), 0);//���̵� ����
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
	printf("�� ������ �Է� �޾ҽ��ϴ�. �ִ� �ο� %d\n", server_waitroom.max_player);
}

//11.28�߰� ��ü�����͸� �޴´�.
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
		//Ŭ��κ���  GameObject����ü�����ް� ����ü����ȯ 
		GameObject = (ObjectGroup*)GameObject;
	}


}
// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI SendRecvObjectInfo(LPVOID arg)
{

	SOCKET s = (SOCKET)arg;
	int retval;
	int cur_id;
	Character rec_char = Character();//ĳ���͸� �޾ƿ� ���ۿ� ���� ����
	CharacterBody* char_body;
	//printf("�����\n");

	WaitForSingleObject(GameThread, INFINITE);

	//event �ڵ� 
	//printf("����");
	while (1) {
		retval = recvn(s, (char *)&rec_char, sizeof(rec_char), 0);//recv
		cur_id = rec_char.get_id();
		char_body = rec_char.get_body();

		manager.RecvClientCaracter(cur_id, char_body);

		//�ٸ� Ŭ�󸮾�Ʈ�� ���� �� �޾����� ���� wait �� �ʿ����� 

		retval = send(s, (char*)&server_ob, sizeof(server_ob), 0);//send
	}


	return 0;
}

