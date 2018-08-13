#ifndef _RECV_H_
#define _RECV_H_

#ifdef _CK_ONLINE_PLAYER_COUNT    // WON ADD 计算线上人数
void PlayerCount(void);
#endif

#ifdef _SEND_EFFECT		   // WON ADD 送下雪、下雨等特效
void SendEffect(char *ffect);
#endif

#ifdef _BAD_PLAYER            // WON ADD 送坏玩家去关
void BadPlayer(void);
char BadPlayerList[500][20];
int MAX_BAD_PLAYER;
#endif

#ifdef _WORKER_AUTO_LOGOUT     // WON ADD GS断线自动送LOGOUT给WORKER
void Worker_send_logout( int ti ,char* id );
#endif


#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
void load_fm_pk_list(void);
void save_fm_pk_list(void);
char fm_pk_list[9][255];
#endif

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void LOAD_herolist(); 
void SAVE_herolist(int);
void Send_A_herolist(int);

#ifdef _FIX_ALLDOMAN
void Send_S_herolist( int ti );
#define MAX_HERO_LIST 50
typedef struct _tagHerolistBase
{
	char strings[3][64];
	int intdata[3];
	int use;
}HerolistBase;
HerolistBase Herolist[MAX_HERO_LIST];
#else
void Send_S_herolist( char *ocdkey , char *oname , char *ncdkey , char *nname , 
					 char *title , int level , int trns , int floor );
#define MAX_HERO_COLUMN 7
#define MAX_HERO_LIST 100
char Herolist[MAX_HERO_LIST][MAX_HERO_COLUMN][72];
#endif

#endif


#ifdef _RECAL_BACKUP_PLAYER			// WON 回溯功能

struct PLAYER_DATA {
	char name[20];
	char fl[10];
	char x[10];
	char y[10];
	char lv[20];
	char gold[20];
	char hp[20];
	char mp[20];
	char vi[20];
	char str[20];
	char tou[20];
	char dex[20];
	char exp[20];
	char trn[20];
	char fmi[20];
	char fmlf[20];
	char bankgld[20];
	char fame[20];
	char pgold[20];
	char fmname[20];
	char item_name[50][64];
	char item[50][255];
	char pet_name[50][64];
	char pet[25][255];
}player_data;


typedef struct tagSTRING64
{
    char string[64];
}STRING64;

struct ITEM_PET_CHANG{
	int equit_item[5];
	int have_item[15];
	int pool_item[30];
	int have_pet[5];
	int pool_pet[20];
}item_pet_chang;

#define	won_get(get, delime1, delime2)	_won_get(get, delime1, delime2, char_data)
void _won_get(char *get, char *delime1, char *delime2, char *char_data);
void saacproto_ACRecalPlayer_recv(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, int backup_flag);
void analysis_char(char *char_data);
int  get_delim(char *msg, char *temp, char *delime1, char *delime2);
void get_online_player_data(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date);
void send_backup_player_data();
void get_backup_player_data(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date);
void get_backup_date(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date);
void saacproto_ACRecalBackup_recv(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, char *char_data, int backup_flag);
void recal_part_backup(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date);
int  rgetFileName( char* dirname , STRING64* string, int size);
int  rrd( char* dirname , STRING64* buf ,int bufsize, int* index);


#define	BACKUP_DIR	"/home/backup/char"

void _won_get_player(char *char_data);
void _won_get_2(char *char_data, char *fun, char *check);


#endif // end RECAL_ASK_PLAYER

typedef struct tagUNLOCKMENUS{
	char PlayerId[256];
	int use;
	time_t time;
}UNLockMenus;

int UNlockM_isBe( char *id);
int UNlockM_addPlayer( char *id);
int UNlockM_UnlockPlayer( void);
int UNlockM_Init( void);


#ifdef _GM_BROADCAST					// WON ADD 客服公告系统
void broadcast( char *msg );
#endif


#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
void saacproto_ACShowMemberList_2_recv( int fd, int charindex, int fm1, int fm2, int time, int id );
#endif



#endif

