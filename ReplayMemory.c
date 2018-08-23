#include "ReplayMemory.h"
#include <stdlib.h>
#include <stdio.h>

void memoryFreeTransition(TR *tr){ // Transition 메모리 해제 부분
    free(tr->prevState);
    free(tr->nextState);
    free(tr);
}

void init_RP(RP *rp, int size){ // ReplayMemory 초기화
    rp->size = size;
    rp->curNum = 0;
    rp->memory = (TR**)malloc(sizeof(TR*)*size); // 메모리 할당
}

void insertTransition(RP *rp, TR *tr){ // 새로운 transition 삽입. ranNum은 0~rp->size-1 까지
    if(rp->curNum == rp->size){ // 꽉 차있다면 교체
        int ranNum = getRandomNumber(0,rp->size-1); // 0부터 size-1 사이에 있는 수
        if(ranNum < 0 || ranNum >= rp->size) return; //안전장치
        memoryFreeTransition(rp->memory[ranNum]); //해당 transition 메모리 해제
        rp->memory[ranNum] = tr;
    }else{ // 빈 공간이 있다면 추가
        rp->memory[rp->curNum] = tr;
        rp->curNum += 1;
    }
}

TR** getTransitionSamples(RP *rp, int size){
    if(rp->curNum < size) return NULL; // 현재 쌓여있는 갯수보다 적으면 NULL값 return 더 샘플을 쌓아야 한다는 의미
    int i;
    TR **samples = (TR**)malloc(sizeof(TR*)*size); // 반환될 sample들을 담는 배열
    
    for(i=0; i<size; i++){ //앞단 siez만큼 주고 뒷단에 있는 샘플들과 랜덤하게 섞는다.
        samples[i] = rp->memory[i];
    }
    if(size*2 > rp->curNum){ // 일정 크기 이하면
        for(i=0; i<size; i++){
            int ranIndex = getRandomNumber(0,rp->curNum-1); // 전체에서 랜덤하게 교체
            TR* tmp_tr = rp->memory[i];
            rp->memory[i] = rp->memory[ranIndex];
            rp->memory[ranIndex] = tmp_tr;
        }
    }else{ // 일정 크기 이상이면
        for(i=0; i<size; i++){
            int ranIndex = getRandomNumber(size,rp->curNum-1); // 뒷단에서 랜덤하게 교체
            TR* tmp_tr = rp->memory[i];
            rp->memory[i] = rp->memory[ranIndex];
            rp->memory[ranIndex] = tmp_tr;
        }
    }
    return samples; //samples는 free해야함
}

void init_TR(TR* tr, float *prevState, int action, int reward, float *nextState){
    tr->prevState = prevState;
    tr->action = action;
    tr->reward = reward;
    tr->nextState = nextState;
}
