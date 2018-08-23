#include "NeuralNet.h"

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
    for(i=0; i<nn->layerNum; i++){
        free(nodeError[i]);
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

