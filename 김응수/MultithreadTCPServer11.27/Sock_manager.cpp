#include "SockManager.h"

Sock_manager::Sock_manager(ObjectGroup* ob) {
	this->current_player = 0;
	this->GameObject = ob;
	int i = 0;
	for (i = 0; i < 4; i++) {
		this->event_thread[i] = CreateEvent(NULL, TRUE, FALSE, NULL);//���� ����, ���ȣ ����
	}
	for (i = 0; i < 4; i++) {
		this->update_handle[i] = CreateEvent(NULL, FALSE, FALSE, NULL);//�ڵ� ����, ���ȣ ����
	}

}

bool Sock_manager::ClientsockSet(SOCKET s, HANDLE handle) {
	
	if (this->wa == NULL && this->current_player > 3) { 
		return FALSE; // 4�� �̻��� ������ ���� FALSE ����
	} 
	else if (this->wa != NULL) {
		if(this->wa->max_player < this->current_player){//�濡 ������ �ִ� �ο����� ���� �÷��̾ �����Ҷ��� FALSE ����
			return FALSE;
		}
	}

	this->client[this->current_player] = s;
	this->current_player++;
	this->client_thread[this->current_player] = handle;

	if (this->wa != NULL) {
		this->wa->current_player = this->current_player;
	}

	return TRUE;
}

int Sock_manager::get_current_player() {
	return this->current_player;
}

void Sock_manager::SetWaitRoom(WaitRoom *w) {
	this->wa = w;
}

int Sock_manager::SendGameState(WaitRoom r) {
	int retval,res;
	int i = 0;

	res = 0;

	for (i = 0; i < this->current_player; i++) {
		retval = send(this->client[i],(char*)&r,sizeof(r),0);
		if (retval == SOCKET_ERROR) {
			res = retval;
		}
	}
	return res;
}

void Sock_manager::RecvClientCaracter(int id, CharacterBody* ch) {
	this->GameObject->player[id].set_body(ch);
}

void Sock_manager::SendWait() {
	WaitForMultipleObjects(this->current_player,this->event_thread,TRUE,INFINITE);
}
void Sock_manager::MainWait() {
	WaitForMultipleObjects(this->current_player, this->update_handle, TRUE, INFINITE);
}
void Sock_manager::ResetEventthis(int id) {
	ResetEvent(this->event_thread[id]);
}

void Sock_manager::SetEventthis(int id) {
	SetEvent(this->event_thread[id]);
}

void Sock_manager::SetUpdateHandle(int id) {
	SetEvent(this->update_handle[id]);
}