#ifndef __DEFEND_H__
#define __DEFEND_H__

#include "version.h"

#ifdef _DEFEND_NEWUSERS
#define DEFEND_MAXLISTS 20
#define DEFEND_SCOREMAXLISTS 200

typedef struct _tagDefendArrayList
{
	char cdkey[256]; //帐号
	char name[256];	//人物名
	int defnums;	//次数
	int type;		//状态
	int score;		//分数
	int use;		//使用
}DefendArrayList;

typedef struct _tagScoreArrayList
{
	char cdkey[256]; //帐号
	char name[256];	//人物名
	char fromname[256];
	int score;		//分数
	int use;		//使用
}ScoreArrayList;


void DEFEND_InitList();
int DEFEND_getUse( int si);
int DEFEND_setUse( int si, int num);
void DEFEND_resetUserForList();
int DEFEND_addUserForList( char *cdkey, char *name, int score, int nums, int type);
void DEFEND_LoadUserForList( char *listarg);
void DEFEND_ReadUserForList( char *filename);
void DEFEND_ShowUserLists();
void DEFEND_SendToUserLists();
void DEFEND_backupUseLists( char *filename);

void DEFEND_resetScoreForList( void);
void DEFEND_LoadScoreForList( char *listarg);
int DEFEND_addScoreForList( char *cdkey, char *name, int score, char *fromname );
int DEFEND_getScoreUse( int si);
void DEFEND_ReadScoreForList( char *filename);
void DEFEND_backupScoreLists( char *filename);
void DEFEND_ShowScoreLists( void);
void DEFEND_CheckScoreForList( int fd, int userindex, char *cdkey, char *name);
#endif

#endif


