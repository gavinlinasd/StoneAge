#include "version.h"

#ifdef _DEATH_CONTEND
//团队人物名单
typedef struct 
{
	int use;
	char cdkey[64];
	char name[64];
}PkTeamMans;
//战斗纪录
typedef struct 
{
	int use;
	int teamnum;
	int flg;	//0 1
}BattleHistorys;

#define MAXTEAMMANNUM 5		//队伍最高人数
#define MAXBATTLENUM 100	//队伍最高战斗纪录

#define DEFMAXBATTLENUM 50 //最高决斗场次
#define DEFWINSCORE 90
#define DEFLOSERATE 0.4
//决斗参赛队伍info
typedef struct _tagPkTeamLists
{
	int use;					//flg
	int teamnum;				//队伍序号
	char teamname[64];			//队伍名称
	char pathdir[64];			//队伍资料目录
	char leadercdkey[64];		//队长CDKEY
	int win;					//胜
	int lost;					//负
	int battleplay;				//总场次
	int score;
	int inside;					//录取旗标

	int updata;					//更新时间

	PkTeamMans MyTeamMans[MAXTEAMMANNUM];
	BattleHistorys BHistory[MAXBATTLENUM];
}PkTeamLists;

//勿动-------------------------------------------------------
int PKLIST_getStringPkTeamData( int ti, char *data, int sizes);
int PKLIST_SetOneBHistory( int ti, int hi, int use, int teamnum, int flg );
int PKLIST_GetOneBHistory( int ti );
void PKLIST_ResetOneTeamMan( int ti );
void PKLIST_ResetOneBHistory( int ti );
void PKLIST_ResetOnePkTeamList( int ti );
int PKLIST_UpdateOnePkTeamData( int ti, int forti, int winerflg);
//-----------------------------------------------------------
int PKLIST_InitPkTeamList( int maxteam );
void PKLIST_UpdatePkTeamList( int fd, int menum, char *mecdkey, int tonum, char *tocdkey, int winer, int flg);
void PKLIST_GetPkTeamListData( int fd, int startTi, int count);
void PKLIST_GetPkTeamListDataOne( int fd, int ti);
//-----------------------------------------------------------
int PKLIST_LoadPkTeamListfromFile( char *dirpath, char *listfilename );
int PKLIST_BackupPkTeamListfromFile( char *dirpath, char *listfilename );
void PKLIST_SortTheCharts( int ti);//排行榜处理
void PKLIST_TheCharts( int fd, int type, int flg);

int PKLIST_LoadUserfromFile( char *dirpath, char *listfilename );
int PKLIST_CutStartUserfromFile( char *dirpath, char *listfilename );

void PKLIST_CheckRepeatBattleHistory( void);
int PKLIST_alteUserfromFile( char *dirpath, char *listfilename );
void PKLIST_Sort_PKListSort( void);

void PKLIST_CheckAllUser( void);

#endif
