#ifndef _REPLAYMEMORY_H_
#define _REPLAYMEMORY_H_

struct Transition{
    float *prevState; // 이전 상태
    int action;
    int reward;
    float *nextState; // 새로운 상태
};
typedef struct Transition TR;

struct ReplayMemory{
    int size;
    int curNum;
    TR **memory;
};
typedef struct ReplayMemory RP;

extern void init_RP(RP *rp, int size); // 메모리 용량 설정 등 초기화
extern void insertTransition(RP *rp, TR *tr); // Replay memory에 transition 삽입
extern TR** getTransitionSamples(RP *rp, int size); // size만큼 샘플 transition 추출

extern int getRandomNumber(int s, int e);
#endif
