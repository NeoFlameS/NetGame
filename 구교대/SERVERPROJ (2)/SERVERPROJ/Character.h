

	//POINT vector; //방향
	//int state; //캐릭터 상태
	//bool isDead; //죽음 유무
	//int bodyLength; //몸의길이
	//CharacterBody char_Body[MAX_BODY_LENGTH];//몸의 길이 최대치로 바디 클래스 배열생성
	//int score, Total_Score; // 점수 ,총합점수 ; 랭킹에 반영됨 
	////2스코어당 몸체하나
	//
	//int id;//아이디로 플레이어 식별
	//int speed; // 이동속도
	//int size; // 원의크기
#pragma once
#include <Windows.h>
#include <windef.h>

#define MAX_BODY_LENGTH 20

struct CharacterBody {
	POINT locate;
	int type;// 0 : 없음, 1: 머리, 2: 몸통
};
class Character {
private:
	POINT vector; // 방향
	int state;
	bool isDead;
	int bodyLength;

	CharacterBody char_Body[MAX_BODY_LENGTH];
	int score, Total_Score;//점수 및 총점
	int id; //플레이어 식별 id
	int speed;
	int size; //원의 크기

public:
	Character(int id, POINT _locate);//생성자
									 //~Character();
	void Update(float time, POINT vector);
	void Draw();

	// 추가 사항 구현은 밑에 날짜와 같이 작성해 주세요.
	//11.25
public:
	Character();
	int get_id();
	CharacterBody* get_body();
	void set_body(CharacterBody* s);
	//11.28 추가
	int* getbodylength(void);
	bool GetDead(void);
	int* getTotalScore(void);
	
};