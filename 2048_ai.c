#include "NeuralNet.h"
#include "ReplayMemory.h"
#include "2048Game.h"

int getRandomNumber(int s, int e){ //범위start <= 난수 <= end
	return rand()%(e-s+1)+s;
}

void sort(float *values, int size){ //insertion sort
    int i,j,r;
    if(size <= 1) return;
    for(i=1; i<size; i++){
        float val = values[i];
        int j = i-1;
        while(j >= 0 && values[j] < values[i]){
            j--;
        }
        j++;
        for(r=i; r>j; r--){
            values[r] = values[r-1];
        }
        values[j] = val;
    }
}

int main(){
    
    /*게임 테스트 부분*/
    /*
    initializeBoard(4);
    while(1){

        printBoard();
        int* possibleAction = getPossibleActions(board2048);
        int i;
        for(i=0; i<boardSize; i++){
            printf("%d ", possibleAction[i]);
        }
        printf("\n");
        free(possibleAction);
        int action = 0;
        printf("action : ");
        scanf("%d", &action);
        int reward = getReward(action);
        if(reward >= 2048){
            printf("성공 끝\n");
            break;
        }else if(reward == gameFailReward){
            printf("실패 끝\n");
            break;
        }
    }
    */
    
    
    //Deep Q Learning
    const float MIN_Q_VAL = -1*FLT_MAX; // Q_VALUE 최솟값 의미.
    const int replayMemorySize = 2000;
    const int sampleNum = 10;
    const float discounted_factor = 0.9;
    const float learningRate = 0.1;
    const float targetReward = 2048;
    
	srand(time(NULL)); // time seed 초기화
    
    RP *rp = (RP*)malloc(sizeof(RP));
    init_RP(rp, replayMemorySize);
    NN *nn = (NN*)malloc(sizeof(NN));
    int layer[4];
    layer[0] = 16;
    layer[1] = 100;
    layer[2] = 20;
    layer[3] = 4;
    init_NN(nn, layer, sizeof(layer)/sizeof(int));
    
    int gameTurn = 500;
    int i,j,r;
    
    float e_val = 0.9; // 0.1 ~ 0.9
    
    for(i=0; i<=gameTurn; i++){
        e_val = 0.9 - 0.9*(float)(i)/gameTurn; //
        printf("e_val = %f\n",e_val);
        initializeBoard(4); // 4*4보드 생성
        
        int exploitation_num = 0;
        int exploration_num = 0;
        
        while(1){
            //action 선택 : e-greedy 기법 사용
            int ranAction = getRandomNumber(1,10);
            int action;
            
            float *prevState = getCurrentState();
            
            if((float)ranAction*0.1 > e_val){ // exploitation
                exploitation_num++;
                float* output = feedForward(nn, prevState, boardSize*boardSize);
                //printf("%f %f %f %f\n", output[0], output[1], output[2], output[3]);
                float max = MIN_Q_VAL;
                for(j=0; j<4; j++){
                    if(output[j] > max){
                        max = output[j];
                        action = j+1;
                    }
                }
                free(output);
            }else{ // exploration 탐험
                exploration_num++;
                action = getRandomNumber(1,4);
            }
            
            int reward = getReward(action); // action 수행!
            
            float *nextState = getCurrentState();
            TR *tr = (TR*)malloc(sizeof(TR));
            init_TR(tr,prevState,action,reward,nextState); // transition 생성
            insertTransition(rp, tr); // transition 삽입

            //신경망 update
            TR** samples = getTransitionSamples(rp, sampleNum); // sampleNum개 추출
            if(samples != NULL){ // sampleNum만큼을 받아왔다면
                for(j=0; j<sampleNum; j++){ // 각각 처리
                    TR *sample = samples[j];
                    // nextState FeedForward
                    float *output_nextState = feedForward(nn, sample->nextState, boardSize*boardSize);
                    float max = MIN_Q_VAL;
                    for(r=0; r<4; r++){
                        if(output_nextState[r] > max){
                            max = output_nextState[r];
                        }
                    }
                    free(output_nextState); // 메모리 해제
    
                    // prevState FeedForward
                    float *output_prevState = feedForward(nn, sample->prevState, boardSize*boardSize);
                    output_prevState[sample->action-1] = sample->reward + discounted_factor*max;
  
                    // backPropagation
                    backPropagation(nn, output_prevState, 4, sampleNum);
                    free(output_prevState); // 메모리 해제
                }
                update(nn, learningRate); // weight bias update.
                free(samples); // 메모리 해제
                parameterZeroSet(nn); // parameter 초기화
            }
            // Game Turn 처리
            if(reward == gameFailReward){ // 실패 점수일 경우
                printf("%d번째 판, 탐험 수 : %d , 착취 수 : %d \n", i+1, exploration_num, exploitation_num);
                printBoard();
                break;
            }else if(reward >= targetReward){ // 점수가 2048을 넘을 경우
                printf("%d번째 판 \n", i+1);
                printBoard();
                break;
            }else{
                continue;
            }
        }
    }
    printf("Traning Finish\n");
    
    // TEST
    int testNum = 1;
    int failNum = 0;
    int successNum = 0;
    for(i=0; i<testNum; i++){
        initializeBoard(4);
        while(1){
            int action;
            float *prevState = getCurrentState();
            float* output = feedForward(nn, prevState, boardSize*boardSize);
            float max = MIN_Q_VAL;
            for(j=0; j<4; j++){
                if(output[j] > max){
                    max = output[j];
                    action = j+1;
                }
            }
            free(output);
            free(prevState);
            
            int reward = getReward(action); // action 수행!
            printf("reward = %d\n",reward);
            // Game Turn 처리
            if(reward == gameFailReward){ // 실패 점수일 경우
                failNum++;
                printBoard();
                break;
            }else if(reward >= targetReward){ // 점수가 2048을 넘을 경우
                successNum++;
                printBoard();
                break;
            }else{
                continue;
            }
        }
    }
    
    printf("TEST %d번 시행 결과. 성공 : %d  실패 : %d \n", testNum, successNum, failNum);
    
     
	return 0;
}
