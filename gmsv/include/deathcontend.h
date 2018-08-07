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
#define MAXTEAMNUM 1000

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
	int read;
	PkTeamMans MyTeamMans[MAXTEAMMANNUM];
	BattleHistorys BHistory[MAXBATTLENUM];
}PkTeamLists;

#define MAXJOINTEAM 40
#define MAXWATCHMAP 16
typedef struct 
{
	int use;
	int teamnum;
	char cdkey[256];
	char name[256];
	int toindex;
	int fd;
}JoinTeamList;

typedef struct _tagPKProcedureRow
{
	int use;
	int time;
	int type;
	JoinTeamList Team[2];
}PKProcedures;

enum{
	PKTYPE_NONE=0,		//无
	PKTYPE_WAIT,		//等待其他队伍加入
	PKTYPE_STANDBY,		//准备对战等待时间
	PKTYPE_PK,			//对战中
};

void del_rn( char *s );
void PKLIST_ResetOneTeamMan( int ti ); //重置队员名单
void PKLIST_ResetOneBHistory( int ti ); //重置对战名单
void PKLIST_ResetOnePkTeamList( int ti ); //重置参赛队伍资料
int PKLIST_InitPkTeamList( int teamnum ); //重置

int PKLIST_GetPkTeamListArray( int teamnum, char *cdkey);
int PKLIST_GetPkTeamListArrayFromNum( int teamnum);
//确认重复约战
BOOL PKLIST_CHECKPkTeamSame( int teamnum, int charaindex, char *cdkey, int toteamnum );
//确认对战场数
int PKLIST_CHECKPkTeamNew( int teamnum, int charaindex, char *cdkey );

//LOAD DATA
BOOL PKLIST_LoadPkTeamListDataSub( int ti, char *data);
BOOL PKLIST_LoadPkTeamListDataMyTeamMans( int ti, char *data);
BOOL PKLIST_LoadPkTeamListDataBHistory( int ti, char *data);
//BOOL PKLIST_LoadPkTeamListData( char *data); //处理ac送来的 pklist
BOOL PKLIST_LoadPkTeamListData(void); // 改成读档
void PKLIST_SavePkTeamListData(void);
void PKLIST_LoadInitPkTeamListData(void);	// 读取最原始的参赛名单
void PKLIST_UpData(char *mycdkey,char *tocdkey,int menum,int tonum,int winer,int flg);
int PKLIST_GetOneBHistory( int ti );
int PKLIST_SetOneBHistory( int ti, int hi, int use, int teamnum, int flg );
int PKLIST_UpdateOnePkTeamData( int ti, int forti, int winerflg);

void PKLIST_InsertTeamNum( int charaindex );
BOOL PKLIST_GetTeamLeaderCdkey( int teamnum, char *buf);
void PKLIST_ShowPkListTeamData( void);

int NPC_PKLIST_Finish_Exit( int menum, int tonum, int winside, int battlemap);


BOOL PKLIST_HandleChartsMess( int fd, char *data, int type, int flg);
BOOL PKLIST_GetChartsListData( int ti, char *data, int sizes );
BOOL PKLIST_GetMyPKListTeamData( int teamnum, char *data, int sizes );


/*
BOOL PKLIST_CheckPKSameTeam( int charaindex );
BOOL PKLIST_JoinPKProcedures( int charaindex );
void PKLIST_DelPKProcedures( int ti, int side );
void PKLIST_CheckPKProcedures( void );

void PKLIST_CheckPKProcedures_PKTYPEWAIT( int ti);
void PKLIST_CheckPKProcedures_PKTYPESTANDBY( int ti);
*/

//赛程
void PKLIST_DelPKProcedures( int ti, int side, int type);
BOOL PKLIST_CheckPklistInServerMap( int ti, int side);
BOOL PKLIST_CheckPKSameTeam( int charaindex );
BOOL PKLIST_CheckPKReapetTeam( int menum, int tonum);
BOOL PKLIST_JoinPKProcedures( int charaindex );
void PKLIST_CheckTeamBeEnable( void);
void PKLIST_warp( int ti, int side, int fl, int x, int y );
void NPC_PKLIST_PlayerLogout_Exit( int charaindex );
int PKLIST_GetPKProcedureArray( int menum );

BOOL PKLIST_CheckLOCKTeam( int menum);
void PKLIST_LOCKTeam( int menum);
void PKLIST_UNLOCKTeam( int menum);
void PKLIST_Sort_PKListSort( void);

//正式赛
typedef struct _tagArrangeBattle
{
	int use;
	int fl;
	int code;
	int teamnum;
	int type; //0 NULL // 1 in battle
	int time;
	char teamname[256];
	int toindex;

	struct _tagArrangeBattle *next[2];
	struct _tagArrangeBattle *top;
}ArrangeBattleC;

#define MAXBAFLOOR 20
#define MAXNOWBATTLE 128

#define MAXBAHEAD 16
#define MAXBATTLEPAGE MAXJOINTEAM

void ABATTLE_InitABattle( int maxnums );
int ABATTLE_CreateNet( ArrangeBattleC *now, int ti, int fl, int maxfl);
void ABATTLE_ShowNet( ArrangeBattleC *now, int fl);
void ABATTLE_ShowBattlefromFl( int ti, int fl);
ArrangeBattleC *ArrangeBattleC_getNew( void);


BOOL ABATTLE_InsertBattle( ArrangeBattleC *aB); //排入赛程
void ABATTLE_EnterBattle( ArrangeBattleC *aB); //入围
void ABATTLE_EliminateBattlefromFl( ArrangeBattleC *aB);//剔除

BOOL ABATTLE_CheckInABattle( int ti);//确认赛程战斗状态 包含时间
int ABATTLE_FindBattlefromFl( int ti, int fl); //找寻可加入赛程队伍组合

void ABATTLE_CheckBattlefromFl(int charindex, int ti,int battleindex); //确认层次是否完成 且 排置赛程
int ABATTLE_CheckBattlefromFl_sub(int charindex, int ti, int fl,int battleindex); //确认层次是否完成


ArrangeBattleC *ABATTLE_getInBattle( int teamnum); //取得赛程head form teamnum

void ABATTLE_MakeInABattleString( void); //制作赛程字串

BOOL PKLIST_GetABattlelistDataString( int ti, int *tindex, int *stime,
									 char *buf1, char *buf2, char *buf3, int flg);//取得赛程字串

ArrangeBattleC *ArrangeBattleC_getInBattleArray( int ti);




void ABATTLE_RecordBattle( int ti, char *buf1, char *tstr1,char *buf2, char *tstr2);
void remove_r( char *s );
void ABATTLE_GetRecordBattle( void);

#endif
