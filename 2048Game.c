#include <stdio.h>
#include <stdlib.h>

extern int getRandomNumber(int s, int e);

int **board2048;
int boardSize = 4; //기본 4
int gameFailReward = -100;
int noDiffReward = -10;

void printBoard(){ //board 출력
    int i,j;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            printf("%d     ", board2048[i][j]);
        }
        printf("\n");
    }
}


void initializeBoard(int size){
    if(board2048 != NULL){
        int i;
        for(i=0; i<boardSize; i++){
            free(board2048[i]);
        }
        free(board2048);
    }
    boardSize = size;
    board2048 = (int **)malloc(sizeof(int*)*boardSize);
    int i,j,r;
    for(i=0; i<boardSize; i++){
        board2048[i] = (int *)malloc(sizeof(int)*boardSize);
        for(j=0; j<boardSize; j++){
            board2048[i][j] = 0;
        }
    }
    
    int ry,rx;
    int ry2,rx2;
    ry = getRandomNumber(0,boardSize-1);
    rx = getRandomNumber(0,boardSize-1);
    ry2 = getRandomNumber(0,boardSize-1);
    if(ry == ry2){
        rx2 = getRandomNumber(0,boardSize-1);
        while(rx2 == rx){
            rx2 = getRandomNumber(0, boardSize-1);
        }
    }else{
        rx2 = getRandomNumber(0,boardSize-1);
    }
    
    int s1 = getRandomNumber(0,1);
    int s2 = getRandomNumber(0,1);
    
    if(s1%2 == 0) board2048[ry][rx] = 2;
    else board2048[ry][rx] = 4;
    
    if(s2%2 == 0) board2048[ry2][rx2] = 2;
    else board2048[ry2][rx2] = 4;
}

int getReward(int action){ // 1 -> UP  2 -> DOWN  3 -> LEFT  4 -> RIGHT
    int prevState[boardSize][boardSize];
    int i,j,r;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            prevState[i][j] = board2048[i][j];
        }
    }
    
    //보상
    int reward = 0;
    
    if(action == 1){ //UP
        for(i=0; i<boardSize; i++){ // x축
            int arr[boardSize];
            int p = 0;
            for(j=0; j<boardSize; j++){ // y축
                if(board2048[j][i] == 0) continue;
                arr[p++] = board2048[j][i];
                board2048[j][i] = 0;
            }
            if(p == 0) continue;
            int brr[boardSize];
            int q = 0;
            r = 0;
            while(1){
                if(r+1 < p){
                    if(arr[r] == arr[r+1]){
                        brr[q++] = arr[r]*2;
                        reward += arr[r]*2;
                        r += 2;
                    }else{
                        brr[q++] = arr[r];
                        r += 1;
                    }
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
                if(r>=p) break;
            }
            for(j=0; j<q; j++){
                board2048[j][i] = brr[j];
            }
        }
    }else if(action == 2){ //DOWN
        for(i=0; i<boardSize; i++){ // x축
            int arr[boardSize];
            int p = 0;
            for(j=boardSize-1; j>=0; j--){ // y축
                if(board2048[j][i] == 0) continue;
                arr[p++] = board2048[j][i];
                board2048[j][i] = 0;
            }
            if(p == 0) continue;
            int brr[boardSize];
            int q = 0;
            r = 0;
            while(1){
                if(r+1 < p){
                    if(arr[r] == arr[r+1]){
                        brr[q++] = arr[r]*2;
                        reward += arr[r]*2;
                        r += 2;
                    }else{
                        brr[q++] = arr[r];
                        r += 1;
                    }
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
                if(r>=p) break;
            }
            for(j=0; j<q; j++){
                board2048[boardSize-j-1][i] = brr[j];
            }
        }
    }else if(action == 3){ //LEFT
        for(i=0; i<boardSize; i++){ // y축
            int arr[boardSize];
            int p = 0;
            for(j=0; j<boardSize; j++){ // x축
                if(board2048[i][j] == 0) continue;
                arr[p++] = board2048[i][j];
                board2048[i][j] = 0;
            }
            if(p == 0) continue;
            int brr[boardSize];
            int q = 0;
            r = 0;
            while(1){
                if(r+1 < p){
                    if(arr[r] == arr[r+1]){
                        brr[q++] = arr[r]*2;
                        reward += arr[r]*2;
                        r += 2;
                    }else{
                        brr[q++] = arr[r];
                        r += 1;
                    }
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
                if(r>=p) break;
            }
            for(j=0; j<q; j++){
                board2048[i][j] = brr[j];
            }
        }
    }else if(action == 4){ //RIGHT
        for(i=0; i<boardSize; i++){ // y축
            int arr[boardSize];
            int p = 0;
            for(j=boardSize-1; j>=0; j--){ // x축
                if(board2048[i][j] == 0) continue;
                arr[p++] = board2048[i][j];
                board2048[i][j] = 0;
            }
            if(p == 0) continue;
            int brr[boardSize];
            int q = 0;
            r = 0;
            while(1){
                if(r+1 < p){
                    if(arr[r] == arr[r+1]){
                        brr[q++] = arr[r]*2;
                        reward += arr[r]*2;
                        r += 2;
                    }else{
                        brr[q++] = arr[r];
                        r += 1;
                    }
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
                if(r>=p) break;
            }
            for(j=0; j<q; j++){
                board2048[i][boardSize-j-1] = brr[j];
            }
        }
    }else{
        printf("Error: action input Error!\n");
    }
    
    //이전 상황과 달라진게 있는지 체크한다. 없으면 reward 0 반환.
    int is_diff = 0;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            if(prevState[i][j] != board2048[i][j]){
                is_diff = 1;
            }
        }
    }
    if(is_diff==0){
        //printf("REWARD: 0 , No Diff\n");
        //return reward;
        return noDiffReward;
    }
    
    //액션을 취하면 새로운 수(2 or 4)가 빈 곳에 나타난다. 만약 더이상 줄일 것이 없다고 판단되면 게임 종료이다.
    //빈곳 찾기
    int emptyY[boardSize*boardSize];
    int emptyX[boardSize*boardSize];
    int p = 0; //빈 공간 갯수
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            if(board2048[i][j] == 0){
                emptyY[p] = i;
                emptyX[p] = j;
                p++;
            }
        }
    }
    int randomPos = getRandomNumber(0,p-1);
    int newNum; // 2 (7/8확률) or 4 (1/8확률)
    if((i=getRandomNumber(0,7))%8 == 4){
        newNum = 4;
    }else{
        newNum = 2;
    }
    board2048[emptyY[randomPos]][emptyX[randomPos]] = newNum;
    p--; //빈 공간 1개 감소
    
    if(p >= 1){
        return reward;
    }
    
    //빈 공간이 없다면
    //더 이상 줄일 수 없는지 판단. 줄일 수 없으면 게임 끝(패배)
    int shrink = 0;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            if(i == boardSize-1 && j == boardSize-1){
                continue;
            }else if(i == boardSize -1){
                if(board2048[i][j] == board2048[i][j+1]){
                    shrink = 1;
                    break;
                }
            }else if(j == boardSize -1){
                if(board2048[i][j] == board2048[i+1][j]){
                    shrink = 1;
                    break;
                }
            }else{
                if(board2048[i][j] == board2048[i][j+1] || board2048[i][j] == board2048[i+1][j]){
                    shrink = 1;
                    break;
                }
            }
        }
    }
    
    //더이상 줄일 부분이 없다면
    if(shrink == 0){
        printf("Game Fail !!\n");
        reward = gameFailReward;
        return reward;
    }else{
        return reward;
    }
}

float* getCurrentState(){
    float *state = (float *)malloc(sizeof(float)*boardSize*boardSize); //boardSize^2 만큼 할당
    int i,j;
    int count = 0;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            state[count++] = board2048[i][j];
        }
    }
    return state;
}


int* getPossibleActions(int** board){ //board에서 가능한 action들을 return 하는 함수. boardSize만큼 리턴한다. 가능하지 않은 action은 0 이고, 가능한 action 은 1 이다.
    int tmpBoard[boardSize][boardSize]; // 임시 보드
    int* result = (int*)malloc(sizeof(int)*boardSize); // 결과 값.
    int i,j,r;
    
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            tmpBoard[i][j] = board[i][j];
        }
    }
    
    //action == 1 일때
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            tmpBoard[i][j] = board[i][j];
        }
    }
    
    for(i=0; i<boardSize; i++){ // x축
        int arr[boardSize];
        int p = 0;
        for(j=0; j<boardSize; j++){ // y축
            if(tmpBoard[j][i] == 0) continue;
            arr[p++] = tmpBoard[j][i];
            tmpBoard[j][i] = 0;
        }
        if(p == 0) continue;
        int brr[boardSize];
        int q = 0;
        r = 0;
        while(1){
            if(r+1 < p){
                if(arr[r] == arr[r+1]){
                    brr[q++] = arr[r]*2;
                    r += 2;
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
            }else{
                brr[q++] = arr[r];
                r += 1;
            }
            if(r>=p) break;
        }
        for(j=0; j<q; j++){
            tmpBoard[j][i] = brr[j];
        }
    }
    int is_diff = 0;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            if(tmpBoard[i][j] != board[i][j]){
                is_diff = 1;
            }
        }
    }
    result[0] = is_diff;
    
    //action == 2일 때
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            tmpBoard[i][j] = board[i][j];
        }
    }
    for(i=0; i<boardSize; i++){ // x축
        int arr[boardSize];
        int p = 0;
        for(j=boardSize-1; j>=0; j--){ // y축
            if(tmpBoard[j][i] == 0) continue;
            arr[p++] = tmpBoard[j][i];
            tmpBoard[j][i] = 0;
        }
        if(p == 0) continue;
        int brr[boardSize];
        int q = 0;
        r = 0;
        while(1){
            if(r+1 < p){
                if(arr[r] == arr[r+1]){
                    brr[q++] = arr[r]*2;
                    r += 2;
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
            }else{
                brr[q++] = arr[r];
                r += 1;
            }
            if(r>=p) break;
        }
        for(j=0; j<q; j++){
            tmpBoard[boardSize-j-1][i] = brr[j];
        }
    }
    is_diff = 0;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            if(tmpBoard[i][j] != board[i][j]){
                is_diff = 1;
            }
        }
    }
    result[1] = is_diff;
    
    //action == 3 일때
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            tmpBoard[i][j] = board[i][j];
        }
    }
    for(i=0; i<boardSize; i++){ // y축
        int arr[boardSize];
        int p = 0;
        for(j=0; j<boardSize; j++){ // x축
            if(tmpBoard[i][j] == 0) continue;
            arr[p++] = tmpBoard[i][j];
            tmpBoard[i][j] = 0;
        }
        if(p == 0) continue;
        int brr[boardSize];
        int q = 0;
        r = 0;
        while(1){
            if(r+1 < p){
                if(arr[r] == arr[r+1]){
                    brr[q++] = arr[r]*2;
                    r += 2;
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
            }else{
                brr[q++] = arr[r];
                r += 1;
            }
            if(r>=p) break;
        }
        for(j=0; j<q; j++){
            tmpBoard[i][j] = brr[j];
        }
    }
    is_diff = 0;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            if(tmpBoard[i][j] != board[i][j]){
                is_diff = 1;
            }
        }
    }
    result[2] = is_diff;
    
    //action == 4일 때
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            tmpBoard[i][j] = board[i][j];
        }
    }
    for(i=0; i<boardSize; i++){ // y축
        int arr[boardSize];
        int p = 0;
        for(j=boardSize-1; j>=0; j--){ // x축
            if(tmpBoard[i][j] == 0) continue;
            arr[p++] = tmpBoard[i][j];
            tmpBoard[i][j] = 0;
        }
        if(p == 0) continue;
        int brr[boardSize];
        int q = 0;
        r = 0;
        while(1){
            if(r+1 < p){
                if(arr[r] == arr[r+1]){
                    brr[q++] = arr[r]*2;
                    r += 2;
                }else{
                    brr[q++] = arr[r];
                    r += 1;
                }
            }else{
                brr[q++] = arr[r];
                r += 1;
            }
            if(r>=p) break;
        }
        for(j=0; j<q; j++){
            tmpBoard[i][boardSize-j-1] = brr[j];
        }
    }
    is_diff = 0;
    for(i=0; i<boardSize; i++){
        for(j=0; j<boardSize; j++){
            if(tmpBoard[i][j] != board[i][j]){
                is_diff = 1;
            }
        }
    }
    result[3] = is_diff;
    return result;
}



