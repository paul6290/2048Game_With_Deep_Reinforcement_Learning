#ifndef _NEURALNET_H_
#define _NEURALNET_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

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

extern void printNNInfo(NN *nn);
extern void init_NN(NN *nn, int *layerInfo, int layerSize);
extern float activation(float val);
extern float d_activation(float val);
extern float* feedForward(NN *nn, float *inputVal, int inputSize);
extern void backPropagation(NN *nn, float *answer, int inputSize, int batchSize);
extern void update(NN *nn, float learningRate);
extern void parameterZeroSet(NN *nn);

#endif
