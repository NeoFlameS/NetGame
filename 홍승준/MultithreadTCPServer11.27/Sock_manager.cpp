#include "SockManager.h"

Sock_manager::Sock_manager(ObjectGroup* ob) {
	this->current_player = 0;
	this->GameObject = ob;
}

bool Sock_manager::ClientsockSet(SOCKET s) {
	
	if (this->wa == NULL && this->current_player > 3) { 
		return FALSE; // 4명 이상이 접속할 때는 FALSE 리턴
	} 
	else if (this->wa != NULL) {
		if(this->wa->max_player < this->current_player){//방에 설정된 최대 인원보다 많은 플레이어가 접속할때도 FALSE 리턴
			return FALSE;
		}
	}

	this->client[this->current_player] = s;
	this->current_player++;

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