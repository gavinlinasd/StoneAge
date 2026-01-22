#ifndef __NPC_QUIZ_H__
#define __NPC_QUIZ_H__

typedef struct NPC_Quiz{

	int 	no;				//  锁及  寞
	int 	type;			//  锁及正奶皿  必□丞    ｝旦禾□汁)	
	int		level;  		//  锁及伊矛伙
	int 	answertype;		//蚕尹及蓟太井凶(ㄡ  ｝ㄢ  ｝签贿穴永民  
	int 	answerNo;		//  锁及蚕尹
	char	question[512]; 	//  锁
	char	select1[128]; 	//蓟  靼1
	char	select2[128]; 	//蓟  靼2
	char	select3[128]; 	//蓟  靼3

}NPC_QUIZ;


void NPC_QuizTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_QuizInit( int meindex );
void NPC_QuizWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

BOOL QUIZ_initQuiz( char *filename);


#endif 

/*__NPC_QUIZ_H__*/
