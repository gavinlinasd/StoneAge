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

	int updata;					//載陔奀潔

	PkTeamMans MyTeamMans[MAXTEAMMANNUM];
	BattleHistorys BHistory[MAXBATTLENUM];
}PkTeamLists;

//昦雄-------------------------------------------------------
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
void PKLIST_SortTheCharts( int ti);//齬俴埤揭燴
void PKLIST_TheCharts( int fd, int type, int flg);

int PKLIST_LoadUserfromFile( char *dirpath, char *listfilename );
int PKLIST_CutStartUserfromFile( char *dirpath, char *listfilename );

void PKLIST_CheckRepeatBattleHistory( void);
int PKLIST_alteUserfromFile( char *dirpath, char *listfilename );
void PKLIST_Sort_PKListSort( void);

void PKLIST_CheckAllUser( void);

#endif
