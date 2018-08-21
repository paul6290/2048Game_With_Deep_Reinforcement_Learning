#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct NeuralNetwork{
	int inputNum; // input 갯수
	int outputNum; // output 갯수
	int layerNum; // layer 갯수 (input layer, output layer 포함)
	int *layerInfo; // layer마다의 node 갯수
	
	float ***weight; // weight[a][b][c]는 a번째 layer의 b번째 node의 a-1번째layer의 c번째 node와 연결된 weight값
	float **bias; // bias[a][b]는 a번째 layer의 b번째 node의 bias 값
	float **z_val; // z_val[a][b]는 a번째 layer의 b번째 node에서 z값 역전파에서 gradient descent 구할 때 이용
	float **a_val; // a_val[a][b]는 a번째 layer의 b번째 node에서 z값에 activation 함수를 씌운것. 역시 역전파에서 이용
	float ***gd_w; // gd_w[a][b][c]는 a번째 layer의 b번째 node에서 a-1번째 layer의 c번째로 가는 weight의 gradient descent 값. batch에서 차곡차곡 쌓여진다. 
	float **gd_b; // gd_b[a][b][c] a번째 layer의 b번째 node에서 bias의 gradient descent 값. batch에서 차곡차곡 쌓여진다.
};
typedef struct NeuralNetwork NN;

float randn (float mu, float sigma);
void parameterZeroSet(NN *nn);

void printNNInfo(NN *nn){
	int *layerInfo = nn->layerInfo;
	int i,j,r;
	printf("==== weight ====\n");
	for(i=1; i<nn->layerNum; i++){
		for(j=0; j<layerInfo[i]; j++){
			for(r=0; r<layerInfo[i-1]; r++){
				printf("%f ", nn->weight[i][j][r]);
			}
			printf("  ");
		}
		printf("\n");
	}

	printf("==== bias ====\n");
	for(i=0; i<nn->layerNum; i++){
		for(j=0; j<layerInfo[i]; j++){
			printf("%f ", nn->bias[i][j]);
		}
		printf(" ");
	}
}

//신경망 초기화
void init_NN(NN *nn, int *layerInfo, int layerSize){
	
	nn->inputNum = layerInfo[0];
	nn->outputNum = layerInfo[layerSize-1];
	nn->layerNum = layerSize;
	nn->layerInfo = (int *)malloc(sizeof(int)*layerSize);
	
	int i;
	// layerInfo 저장
	for(i=0; i<layerSize; i++){
		nn->layerInfo[i] = layerInfo[i];
	}

	//weight 초기화
	nn->weight = (float***)malloc(sizeof(float**)*layerSize);
	for(i=0; i<layerSize; i++){
		nn->weight[i] = (float**)malloc(sizeof(float*)*layerInfo[i]);
		if(i==0){
			int j;
			for(j=0; j<layerInfo[i]; j++){
				nn->weight[i][j] = (float*)malloc(sizeof(float)*1);
				nn->weight[i][j][0] = 1;
			}
		}else{
			int j;
			for(j=0; j<layerInfo[i]; j++){
				nn->weight[i][j] = (float*)malloc(sizeof(float)*layerInfo[i-1]);
				int r;
				for(r=0; r<layerInfo[i-1]; r++){
					nn->weight[i][j][r] = randn(0,1); //가우시안 분포 랜덤 변수 적용
				}
			}
		}
	}

	//bias 초기화
	nn->bias = (float**)malloc(sizeof(float*)*layerSize);
	for(i=0; i<layerSize; i++){
		nn->bias[i] = (float*)malloc(sizeof(float)*layerInfo[i]);
		if(i==0){
			int j;
			for(j=0; j<layerInfo[i]; j++){
				nn->bias[i][j] = 0;
			}
		}else{
			int j;
			for(j=0; j<layerInfo[i]; j++){
				nn->bias[i][j] = randn(0,1); //가우시안 분포 랜덤 변수 적용
			}
		}
	}

	//z_val, a_val, gd_w, gd_b 초기화
	nn->z_val = (float**)malloc(sizeof(float*)*layerSize);
	nn->a_val = (float**)malloc(sizeof(float*)*layerSize);
	nn->gd_w = (float***)malloc(sizeof(float**)*layerSize);
	nn->gd_b = (float**)malloc(sizeof(float*)*layerSize);

	for(i=0; i<layerSize; i++){
		nn->z_val[i] = (float*)malloc(sizeof(float)*layerInfo[i]);
		nn->a_val[i] = (float*)malloc(sizeof(float)*layerInfo[i]);
		nn->gd_b[i] = (float*)malloc(sizeof(float)*layerInfo[i]);
		nn->gd_w[i] = (float**)malloc(sizeof(float*)*layerInfo[i]);
		
		if(i==0) continue; //input layer 는 gd 구할 필요 없음
		int j;
		for(j=0; j<nn->layerInfo[i]; j++){
			nn->gd_w[i][j] = (float*)malloc(sizeof(float)*layerInfo[i-1]);
		}
	}
	parameterZeroSet(nn);
}

float activation(float val){
	//sigmoid
	return 1/(1+exp(-1*val));
}

//activation 미분 함수
float d_activation(float val){
	//sigmoid
	return activation(val)*(1-activation(val));
}

float* feedForward(NN *nn, float *inputVal, int inputSize){
	int i,j,r;
	for(i=0; i<nn->layerNum; i++){
		if(i==0){
			for(j=0; j<nn->inputNum; j++){
				nn->z_val[i][j] = inputVal[j]*nn->weight[i][j][0] + nn->bias[i][j];
				nn->a_val[i][j] = activation(nn->z_val[i][j]);
			}
		}else{
			for(j=0; j<nn->layerInfo[i]; j++){
				float z_sum = 0;
				for(r=0; r<nn->layerInfo[i-1]; r++){
					z_sum += nn->a_val[i-1][r] * nn->weight[i][j][r];
				}
				z_sum += nn->bias[i][j];
				nn->z_val[i][j] = z_sum;
				nn->a_val[i][j] = activation(z_sum);
			}
		}
	}
	float *output = (float*)malloc(sizeof(float)*nn->outputNum);
	for(i=0; i<nn->outputNum; i++){
		output[i] = nn->z_val[nn->layerNum-1][i];
	}
	return output;
}

//역전파
void backPropagation(NN *nn, float *answer, int inputSize, int batchSize){
	float **nodeError = (float**)malloc(sizeof(float*)*nn->layerNum);
	int i,j,r;
	for(i=0; i<nn->layerNum; i++){
		nodeError[i] = (float*)malloc(sizeof(float)*nn->layerInfo[i]);
	}
	
	//Output Layer
	//mean square error
	for(i=0; i<nn->layerInfo[nn->layerNum-1]; i++){
		nodeError[nn->layerNum-1][i] = (nn->z_val[nn->layerNum-1][i] - answer[i]); //z_val 을 사용
		for(j=0; j<nn->layerInfo[nn->layerNum-2]; j++){ //gd_w output layer update
			nn->gd_w[nn->layerNum-1][i][j] += nodeError[nn->layerNum-1][i] * nn->a_val[nn->layerNum-2][j] / batchSize;
		}
		nn->gd_b[nn->layerNum-1][i] += nodeError[nn->layerNum-1][i] / batchSize; //gd_b output layer update
	}
	
	//inner Layer
	for(i=nn->layerNum-2; i>=1; i--){
		for(j=0; j<nn->layerInfo[i]; j++){
			float errSum = 0;
			for(r=0; r<nn->layerInfo[i+1]; r++){ 
				errSum += nn->weight[i+1][r][j]*nodeError[i+1][r]*d_activation(nn->z_val[i][j]);
			}
			nodeError[i][j] = errSum;
			nn->gd_b[i][j] += nodeError[i][j] / batchSize;//gd_b update
			for(r=0; r<nn->layerInfo[i-1]; r++){
				nn->gd_w[i][j][r] += nodeError[i][j] * nn->a_val[i-1][r] / batchSize; //gd_w update
			}
		}
	}

	free(nodeError); // 메모리 해제
}

void update(NN *nn, float learningRate){ // gd를 이용하여 가중치 업데이트
	int i,j,r;
	// update
	for(i=1; i<nn->layerNum; i++){
		for(j=0; j<nn->layerInfo[i]; j++){
			// weight update
			for(r=0; r<nn->layerInfo[i-1]; r++){
				nn->weight[i][j][r] = nn->weight[i][j][r] - (learningRate * nn->gd_w[i][j][r]);
			}

			//bias update
			nn->bias[i][j] = nn->bias[i][j] - (learningRate * nn->gd_b[i][j]);
		}
	}
}
void parameterZeroSet(NN *nn){ // z_val, a_val, gd_w, gd_b 초기화
	int i,j,k;
	for(i=0; i<nn->layerNum; i++){
		for(j=0; j<nn->layerInfo[i]; j++){
			nn->z_val[i][j] = 0;
			nn->a_val[i][j] = 0;
			nn->gd_b[i][j] = 0;
		}
	}
	for(i=1; i<nn->layerNum; i++){
		for(j=0; j<nn->layerInfo[i]; j++){
			for(k=0; k<nn->layerInfo[i-1]; k++){
				nn->gd_w[i][j][k] = 0;
			}
		}
	}
}

float randn (float mu, float sigma){
	float U1, U2, W, mult;
	static float X1, X2;
	static int call = 0;
	if (call == 1){
		call = !call;
		return (mu + sigma * (float) X2);
	}
 	do{
		U1 = -1 + ((float) rand () / RAND_MAX) * 2;
		U2 = -1 + ((float) rand () / RAND_MAX) * 2;
		W = pow (U1, 2) + pow (U2, 2);
	}while (W >= 1 || W == 0);
 
	mult = sqrt ((-2 * log (W)) / W);
	X1 = U1 * mult;
	X2 = U2 * mult;
	call = !call;
	return (mu + sigma * (float) X1);
}



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

int main(){
	srand(time(NULL));
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
	/*
	NN *nn = (NN*)malloc(sizeof(NN));
	int arr[3];
	arr[0] = 6;
	arr[1] = 10;
	arr[2] = 3;

	init_NN(nn,arr,sizeof(arr)/sizeof(int));
	
	int i=0;
	float inputVal[6];
	inputVal[0] = -100;
	inputVal[1] = 100;
	inputVal[2] = 1;
	inputVal[3] = 4;
	inputVal[4] = 10;
	inputVal[5] = -5;

	float answer[3];
	answer[0] = 1;
	answer[1] = 2;
	answer[2] = 3;

	for(i=1; i<=5000; i++){
		float *output = feedForward(nn,inputVal, sizeof(inputVal)/sizeof(float));
		printf("%f %f %f\n",output[0],output[1], output[2]);
		backPropagation(nn,answer,sizeof(answer)/sizeof(float),20);
		if(i%20 == 0){
			update(nn,0.02);
			parameterZeroSet(nn);
		}
		free(output);
	}
	*/
	return 0;
}
