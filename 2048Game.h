#ifndef _2048GAME_H_
#define _2048GAME_H_

extern int **board2048;
extern int boardSize; //기본 4
extern int gameFailReward;
extern int noDiffReward;

extern void printBoard(); //board 출력
extern void initializeBoard(int size);
extern int getReward(int action); // 1 -> UP  2 -> DOWN  3 -> LEFT  4 -> RIGHT
extern float* getCurrentState(); // 현재 보드 상태 반환
extern int* getPossibleActions(int** board); //board에서 가능한 action들을 return 하는 함수. boardSize만큼 리턴한다. 가능하지 않은 action은 0 이고, 가능한 action 은 1 이다.
#endif
