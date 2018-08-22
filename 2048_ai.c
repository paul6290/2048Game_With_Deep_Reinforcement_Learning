#include "NeuralNet.h"
#include "ReplayMemory.h"


//Game 부분
int **board2048;
int boardSize = 4; //기본 4
int gameFailReward = -1000;

void printBoard(){ //board 출력
	int i,j;
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			printf("%d     ", board2048[i][j]);
		}
		printf("\n");
	}
}

int getRandomNumber(int s, int e){ //범위start <= 난수 <= end
	return rand()%(e-s+1)+s;
}

void initializeBoard(int size){
	free(board2048);
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
		printf("REWARD: 0 , No Diff\n");
		return reward;
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

int main(){
    
    const float MIN_Q_VAL = FLT_MIN; // Q_VALUE 최솟값 의미.
    const int replayMemorySize = 1000;
	srand(time(NULL));
    
    RP *rp = (RP*)malloc(sizeof(RP));
    init_RP(rp, replayMemorySize);
    NN *nn = (NN*)malloc(sizeof(NN));
    int layer[4];
    layer[0] = 16;
    layer[1] = 50;
    layer[2] = 30;
    layer[3] = 4;
    init_NN(nn, layer, sizeof(layer)/sizeof(int));
    
    int gameTurn = 50;
    int i,j,r;
    
    float e_val = 0.9;
    
    for(i=0; i<=gameTurn; i++){
        printf("\ne_val : "); // e-greedy 에서 e값 받음 점차적으로 줄여줌.
        scanf("%f", &e_val);
        initializeBoard(4); // 4*4보드 생성
        
        //action 선택 : e-greedy 기법 사용
        int ranAction = getRandomNumber(1,10);
        int action;
    
        float *prevState = getCurrentState();
        
        if(ranAction > e_val*10){ // exploitation
            float* output = feedForward(nn, prevState, boardSize*boardSize);
            float max = MIN_Q_VAL;
            for(j=0; j<4; j++){
                if(output[j] > max){
                    max = output[j];
                    action = j+1;
                }
            }
            free(output);
        }else{ // exploration
            action = getRandomNumber(1,4);
        }
        
        int reward = getReward(action);
        
        if(reward == gameFailReward){ // 게임 성공 여부 판단해야함
            
        }
        
        float *nextState = getCurrentState();
        TR *tr = (TR*)malloc(sizeof(TR));
        
        tr->prevState = prevState;
        tr->action = action;
        tr->reward = reward;
        tr->nextState = nextState;
        
        insertTransition(rp, tr); // transition 삽입
        
    }
    
    /*
	initializeBoard(4);

	while(1){
		printBoard();
		int action;
		printf("\n\nNextAction :  ");
		scanf("%d", &action);
		int reward = getReward(action);
		printf("Reward : %d\n\n", reward);
		if(reward == gameFailReward) break;
	}
     */
	
	
    /*
	NN *nn = (NN*)malloc(sizeof(NN));
	int arr[4];
	arr[0] = 3;
	arr[1] = 3;
	arr[2] = 4;
	arr[3] = 1;

	init_NN(nn,arr,sizeof(arr)/sizeof(int));
	
	int i=0;
	float inputVal[6];
	inputVal[0] = -100;
	inputVal[1] = 100;
	inputVal[5] = -5;

	float answer[1];
	answer[0] = 1000;

	for(i=1; i<=500; i++){
		float *output = feedForward(nn, inputVal, sizeof(inputVal)/sizeof(float));
		backPropagation(nn,answer,sizeof(answer)/sizeof(float),10);
		if(i%10 == 0){
			printf("%f \n",output[0]);
			update(nn,0.01);
			parameterZeroSet(nn);
		}
		free(output);
	}
	*/
	return 0;
}
