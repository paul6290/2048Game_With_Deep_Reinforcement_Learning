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
    FILE *fp;
    if((fp = fopen("HumanSamples.txt","a")) == NULL){
        printf("File Open Fail!\n");
        return 0;
    }
    
    initializeBoard(4);
    while(1){
        printBoard();
        int* possibleAction = getPossibleActions(board2048);
        
        /*
        int i;
        for(i=0; i<boardSize; i++){
            printf("%d ", possibleAction[i]);
        }
        */
        
        int action = 0;
        while(1){
            printf("action : ");
            scanf("%d", &action);
            if(possibleAction[action-1] == 0){
                printf("Error!: Impossible Postion Select other actions\n");
            }else{
                break;
            }
        }

        free(possibleAction);
        
        float *currentState = getCurrentState();
        int reward = getReward(action);
        float *nextState = getCurrentState();
        
        int i,j;
        
        //prev state save
        for(i=0; i<boardSize*boardSize; i++){
            fprintf(fp,"%f ", currentState[i]);
        }
        fprintf(fp,"\n");
        //action save
        fprintf(fp,"%d\n",action);
        //reward save
        fprintf(fp,"%d\n",reward);
        //next state save
        for(i=0; i<boardSize*boardSize; i++){
            fprintf(fp,"%f ", nextState[i]);
        }
        fprintf(fp,"\n");
        
        free(currentState);
        free(nextState);
        
        if(reward >= 2048){
            printf("성공 끝\n");
            break;
        }else if(reward == gameFailReward){
            printf("실패 끝\n");
            break;
        }
    }
    fclose(fp);
    
    /*
    //Deep Q Learning
    const float MIN_Q_VAL = -1*FLT_MAX; // Q_VALUE 최솟값 의미.
    const int replayMemorySize = 2000;
    const int sampleNum = 10;
    const float discounted_factor = 0.9;
    const float learningRate = 0.01;
    const float targetReward = 2048;
    
	srand(time(NULL)); // time seed 초기화
    
    RP *rp = (RP*)malloc(sizeof(RP));
    init_RP(rp, replayMemorySize);
    NN *nn = (NN*)malloc(sizeof(NN));
    int layer[4];
    layer[0] = 16;
    layer[1] = 100;
    layer[2] = 80;
    layer[3] = 4;
    init_NN(nn, layer, sizeof(layer)/sizeof(int));
    
    int gameTurn = 1000;
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
            int action; //action 값이 1~4 여야 함.
            
            float *prevState = getCurrentState();
            int* possActions = getPossibleActions(board2048); // 가능한 액션들 받아옴.
            
            if((float)ranAction*0.1 > e_val){ // exploitation
                exploitation_num++;
                float* output = feedForward(nn, prevState, boardSize*boardSize);
                //printf("%f %f %f %f\n", output[0], output[1], output[2], output[3]);
                float max = MIN_Q_VAL;
                for(j=0; j<4; j++){
                    if(possActions[j] == 1 && output[j] > max){ //가능한 액션이면서 value 가 높은.
                        max = output[j];
                        action = j+1;
                    }
                }
                free(output);
            }else{ // exploration 탐험
                exploration_num++;
                while(1){
                    int ranNum = getRandomNumber(0,3);
                    if(possActions[ranNum] == 1){
                        action = ranNum+1;
                        break;
                    }
                }
            }
            free(possActions); // 메모리 해제
            
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
    */
    
    
    /*
    // TEST
    int testNum = 10;
    int failNum = 0;
    int successNum = 0;
    
    int score_2048 = 0;
    int score_1024 = 0;
    int score_512 = 0;
    int score_256 = 0;
    
    for(i=0; i<testNum; i++){
        initializeBoard(4);
        while(1){
            int action;
            float *prevState = getCurrentState();
            int* possActions = getPossibleActions(board2048); // 가능한 액션들 받아옴.
            float* output = feedForward(nn, prevState, boardSize*boardSize);
            float max = MIN_Q_VAL;
            for(j=0; j<4; j++){
                if(possActions[j] == 1 && output[j] > max){
                    max = output[j];
                    action = j+1;
                }
            }
            free(possActions);
            free(output);
            free(prevState);
            
            int reward = getReward(action); // action 수행!
            
            if(reward >= 256) score_256++;
            if(reward >= 512) score_512++;
            if(reward >= 1024) score_1024++;
            if(reward >= 2048) score_2048++;
            
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
    printf("256 횟수 : %d \n512 횟수 : %d\n1024 횟수 : %d\n2048 횟수 : %d\n", score_256, score_512,score_1024, score_2048);
    */
	return 0;
}
