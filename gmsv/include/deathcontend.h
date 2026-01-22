#include "version.h"

#ifdef _DEATH_CONTEND

//芶勦�冼屪�等
typedef struct 
{
	int use;
	char cdkey[64];
	char name[64];
}PkTeamMans;
//桵須槨翹
typedef struct 
{
	int use;
	int teamnum;
	int flg;	//0 1
}BattleHistorys;

#define MAXTEAMMANNUM 5		//勦斪郔詢�侕�
#define MAXBATTLENUM 100	//勦斪郔詢桵須槨翹
#define MAXTEAMNUM 1000

#define DEFMAXBATTLENUM 50 //郔詢樵須部棒
#define DEFWINSCORE 90
#define DEFLOSERATE 0.4
//樵須統��勦斪info
typedef struct _tagPkTeamLists
{
	int use;					//flg
	int teamnum;				//勦斪唗瘍
	char teamname[64];			//勦斪靡備
	char pathdir[64];			//勦斪訧蹋醴翹
	char leadercdkey[64];		//勦酗CDKEY
	int win;					//吨
	int lost;					//蛹
	int battleplay;				//軞部棒
	int score;
	int inside;					//翹�﹉麇�
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
	PKTYPE_NONE=0,		//拸
	PKTYPE_WAIT,		//脹渾む坻勦斪樓��
	PKTYPE_STANDBY,		//袧掘勤桵脹渾奀潔
	PKTYPE_PK,			//勤桵笢
};

void del_rn( char *s );
void PKLIST_ResetOneTeamMan( int ti ); //笭离勦埜靡等
void PKLIST_ResetOneBHistory( int ti ); //笭离勤桵靡等
void PKLIST_ResetOnePkTeamList( int ti ); //笭离統��勦斪訧蹋
int PKLIST_InitPkTeamList( int teamnum ); //笭离

int PKLIST_GetPkTeamListArray( int teamnum, char *cdkey);
int PKLIST_GetPkTeamListArrayFromNum( int teamnum);
//�溜珋婺椒槳�
BOOL PKLIST_CHECKPkTeamSame( int teamnum, int charaindex, char *cdkey, int toteamnum );
//�溜炩婭蔔﹋�
int PKLIST_CHECKPkTeamNew( int teamnum, int charaindex, char *cdkey );

//LOAD DATA
BOOL PKLIST_LoadPkTeamListDataSub( int ti, char *data);
BOOL PKLIST_LoadPkTeamListDataMyTeamMans( int ti, char *data);
BOOL PKLIST_LoadPkTeamListDataBHistory( int ti, char *data);
//BOOL PKLIST_LoadPkTeamListData( char *data); //揭燴ac冞懂腔 pklist
BOOL PKLIST_LoadPkTeamListData(void); // 蜊傖黍紫
void PKLIST_SavePkTeamListData(void);
void PKLIST_LoadInitPkTeamListData(void);	// 黍�＝鍘倚撐觸恛�靡等
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

//��最
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

//淏宒��
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


BOOL ABATTLE_InsertBattle( ArrangeBattleC *aB); //齬�躽�最
void ABATTLE_EnterBattle( ArrangeBattleC *aB); //�輷�
void ABATTLE_EliminateBattlefromFl( ArrangeBattleC *aB);//枌壺

BOOL ABATTLE_CheckInABattle( int ti);//�溜珂�最桵須袨怓 婦漪奀潔
int ABATTLE_FindBattlefromFl( int ti, int fl); //梑扆褫樓�躽�最勦斪郪磁

void ABATTLE_CheckBattlefromFl(int charindex, int ti,int battleindex); //�溜炰蒮恀Й鮹窸� й 齬离��最
int ABATTLE_CheckBattlefromFl_sub(int charindex, int ti, int fl,int battleindex); //�溜炰蒮恀Й鮹窸�


ArrangeBattleC *ABATTLE_getInBattle( int teamnum); //�△蠍�最head form teamnum

void ABATTLE_MakeInABattleString( void); //秶釬��最趼揹

BOOL PKLIST_GetABattlelistDataString( int ti, int *tindex, int *stime,
									 char *buf1, char *buf2, char *buf3, int flg);//�△蠍�最趼揹

ArrangeBattleC *ArrangeBattleC_getInBattleArray( int ti);




void ABATTLE_RecordBattle( int ti, char *buf1, char *tstr1,char *buf2, char *tstr2);
void remove_r( char *s );
void ABATTLE_GetRecordBattle( void);

#endif
