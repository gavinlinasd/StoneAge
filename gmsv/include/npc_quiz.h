#ifndef __NPC_QUIZ_H__
#define __NPC_QUIZ_H__

typedef struct NPC_Quiz{

	int 	no;				//  坶摯  蠕
	int 	type;			//  坶摯淏騷鏤  斛↓堜    ��筒睽↓眴)	
	int		level;  		//  坶摯畛穫鳴
	int 	answertype;		//紮窇摯撒怮凝倜(兕  ��兩  ��ワ鞅悃蚗鏍  
	int 	answerNo;		//  坶摯紮窇
	char	question[512]; 	//  坶
	char	select1[128]; 	//撒  鱁1
	char	select2[128]; 	//撒  鱁2
	char	select3[128]; 	//撒  鱁3

}NPC_QUIZ;


void NPC_QuizTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_QuizInit( int meindex );
void NPC_QuizWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

BOOL QUIZ_initQuiz( char *filename);


#endif 

/*__NPC_QUIZ_H__*/
