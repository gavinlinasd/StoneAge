#ifndef _FAMILY_H_
#define _FAMILY_H_

#include "version.h"

#define MAX_FAMILY              1000            // 1000
#define TEXT_MAX                1024
#ifdef MEMBER_100
        #define MAX_MEMBERNUM           100
#else
	#define MAX_MEMBERNUM           50              // 50
#endif
#define MIN_MEMBERNUM           10              // 10

#ifdef _FIX_9_FMPOINT		// WON ADD 九大庄园
#define MAX_FMPOINT             10  
#else
#define MAX_FMPOINT             4               // 4
#endif

#define PREDEL_TIME             60*60*24*7      // 七天 60*60*24*3
#define PREDELMEMBER_TIME       60*60*24*30     // 30天 60*60*24*30
#define FAMILY_MEMONUM          35              // 家族留言板最大留言数量
#define FAMILY_MEMOLEN          256
#define FMSMEMOINDEX            10000           // 家族之间留言板 index
#define FMSMEMONUM              140
#define TOPFMLISTNUM            10              // 家族排行榜数量
#define MINFMPOPNUM             -20000000
// shan 20011207
#define MAXFMPOPNUM             100000000
//#define MAXFMPOPNUM           5000000
#define MINAPPLYPNUM            30              // 申请家族最低人数
#define FMMEMBERINDEX           "fmmember"
#define FMMEMOLIST              "memo"
#define FMELDERNUM		4		// 长老数量
#ifdef _EVERYONE_RIDE
	#define FMMAXGOLD			100000000	// 家族基金上限
#else
	#define FMMAXGOLD			10000000	// 家族基金上限
#endif
// shan 20011207
#define MAXRECVPOP				50000000	// 家族声望转移上限
// CoolFish: 2001/11/26
//#define MAXRECVPOP			2500000		// 家族声望转移上限
                
typedef enum
{
	fmindex,
	fmleadergrano,
	fmnum,
	fmjoinnum,
	fmacceptflag,
	fmsetupflag,
	fmsprite,
	fmpointindex,
	fmpopular,
	fmgold,
	fmmemberindexmaxnum,
	apply_time,
	setup_time,
	predel_time,
	memonowwritenum,
	memonum,
	fmadv,
	fmfeed,
	fmsynthesize,
	fmdealfood,
	fmpk,
#ifdef _NEW_MANOR_LAW
	fmmomentum,
#endif
	familymaxint,
} FAMILY_DATAINT;

typedef enum
{
	fmname,
	fmleadername,
	fmleaderid,
	petname,
	petattr,
	fmrule,
	familymaxchar,
} FAMILY_DATACHAR;

typedef enum
{
	charlv,
	charflag,
	onlineflag,
	charfdid,
	predeltime,
	popular,
#ifdef _FMVER21
	eventflag,
#endif
#ifdef _NEW_MANOR_LAW
	momentum,
#endif
	memberdatamaxint,
} MEMBER_DATAINT;

typedef enum
{
	charname,
	charid,
	memberdatamaxchar,
} MEMBER_DATACHAR;

typedef enum
{
	FMMEMBER_NONE = -1,
	FMMEMBER_MEMBER = 1,
	FMMEMBER_APPLY,
	FMMEMBER_LEADER,
	FMMEMBER_ELDER,
	FMMEMBER_KINDNUM,
} MEMBER_KIND;

// #define MEMBERMEM	memberdatamaxint * sizeof(int) + 50
// #define FAMILYMEM	familymaxint * sizeof(int) + 1000 + MAX_MEMBERNUM * MEMBERMEM
#ifdef _ACFMPK_LIST
void FMPK_InitList( void);
#endif
void sortFamily(void);	// Arminius: sort family
int getFMsortedlist(char *buf, int bufsize, int bp, int ep);

int readFamily(char *dir);

int readOneFamilyFromTi( int ti);
int readOneFamily( char *filename, int i);

int writeFamily(char *dir);
int readFMPoint(char *dir);
int writeFMPoint(char *dir);
int readFMSMemo(char *dir);
int writeFMSMemo(char *dir);
void delovertimeFMMem(int time);
#ifdef _PERSONAL_FAME
int ACAddFM(int fd, int *workindex, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petattr,
	char *fmrule, int fmsprite, int fmleadergrano, int fame, int charfdid);
#else
int ACAddFM(int fd, int *workindex, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petattr,
	char *fmrule, int fmsprite, int fmleadergrano, int charfdid);
#endif
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
int ACJoinFM(int fd, int index, char *fmname, int fmindex,
        char *charname, char *charid, int charlv, int fame, int charfdid);
#else
int ACJoinFM(int fd, int index, char *fmname, int fmindex,
        char *charname, char *charid, int charlv, int charfdid);
#endif
int ACLeaveFM(int index, char *fmname, int fmindex,
        char *charname, char* charid);
int ACDelFM(int index, char *fmname, int fmindex);
int ACShowFMList(char *data);
int ACShowFMMemberList(int index, int *acceptflag, int *fmjoinnum, char *data);
int ACFMDetail(int index, char *fmname, int fmindex, char *data);
#ifdef _FMVER21
int ACMemberJoinFM(int index, char *fmname, int fmindex,
        char *charname, int charindex, int result, int meindex);
#else
int ACMemberJoinFM(int index, char *fmname, int fmindex,
        char *charname, int charindex, int result);
#endif
#ifdef _FMVER21
int ACMemberLeaveFM(int index, char *fmname, int fmindex,
        char *charname, int flag, int charindex, int meindex);
#else
int ACMemberLeaveFM(int index, char *fmname, int fmindex,
        char *charname, int flag, int charindex);
#endif
int ACFMAssignOcp(int index, char *fmname, int fmindex,
        char *charname, int charindex, int result);

#ifdef _FM_MODIFY
int ACFMCharLogin(int fd, int index, char *fmname, int fmindex,
		char *charname, char *charid, int charlv, int *floor, int *fmpopular, 
		int *joinflag, int *fmsetupflag, int *charindex, int charfdid,
		int *charfame, int eventflag,int gsnum
	#ifdef _NEW_MANOR_LAW
		,int *momentum
	#endif
	);
#else
	#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
	int ACFMCharLogin(int fd, int index, char *fmname, int fmindex,
		char *charname, char *charid, int charlv, int *floor, int *fmpopular,
		int *joinflag, int *fmsetupflag, int *charindex, int charfdid,
		int *charfame, int eventflag);
	#else
	int ACFMCharLogin(int fd, int index, char *fmname, int fmindex,
		char *charname, char *charid, int charlv, int *floor, int *fmpopular,
		int *joinflag, int *fmsetupflag, int *charindex, int charfdid);
	#endif
#endif

int ACFMCharLogout(int index, char *fmname, int fmindex, char *charname,
        char  *charid, int charlv, int charfdid);
int delfmpointindex(int fmindex);
int ACgetFMFloor(int fmindex);
int ACgetFMInfoFromChar(char *fmname, int *fmindex,
	char *charname, char *charid, int *charindex);
int ACFMReadMemo(int index, int *dataindex, char *data);
int ACFMWriteMemo(int index, char *fmname, int fmindex, char *data);
int ACFMPointList(char *data);
int ACSetFMPoint(int index, char *fmanme, int fmindex, int fmpointindex,
	int fl, int x, int y);
int ACFixFMPoint(int winindex, char *winfmname, int winfmindex, int loseindex,
	char *losefmname, int losefmindex, int village);
int ACShowTopFMList(char *data, int datasize, int kindflag);
int ACFMAnnounce(char *fmname, int fmindex, int index, char *data, int color);
#ifdef _FAMILYBANKSTONELOG
int ACgetFMBankgold(int index, char *fmname,int fmindex,int charindex);
#endif
int ACFixFMData(int index, char *fmname, int fmindex, int kindflag, int charindex,
	char *data1, char *data2);
int ACGetFMData(int index, char *fmname, int fmindex, int kindflag, int *data);
int ACFixFMPK(int winindex, char *winfmname, int winfmindex, int loseindex,
	char *losefmname, int losefmindex);
int ACGMFixFMData(int index, char *fmname, char *charid, char *cmd, char *data);
int ChangeFMLeader(int index, char *fmname, int fmindex);
#ifdef _LEADERFUNCHECK
int CheckLeaderQ(int index, char *fmname, int fmindex, char *charname, char *charid);
#endif




#ifdef _ACFMPK_LIST
#define FMPKLIST_MAXNUM 10
typedef struct tagFamilyPKSchedule {
	int pkflg;
	char Data[256];
} FamilyPKSchedule;

void FMPK_LoadList();
void FMPK_BackUpList();
int FMPK_GetTypeFlg( int ti);
char *FMPK_GetData( int ti);
int FMPK_SetData( int ti, int Pkflg, char *Data, int sizes);
#endif



#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
void saacproto_ACRELOADFM_recv( int fd, int charindex );
char *get_fm_leader_index( int fm_index );
#endif




#ifdef _DEATH_FAMILY_STRUCT		// WON ADD 家族战存放胜负资料

#define fm_pk_max 200

typedef struct _fm_pk_struct
{
	int fm_index[fm_pk_max];
	int fm_win[fm_pk_max];
	int fm_lose[fm_pk_max];
	int fm_score[fm_pk_max];
	char fm_name[fm_pk_max][30];
}FM_PK_STRUCT;


enum{
	FM_INDEX =0,
	FM_WIN,
	FM_LOSE,
	FM_SCORE,
	FM_NAME,
};


void setInt_fm_pk_struct( int index, int type, int num );
void setChar_fm_pk_struct( int index, int type, char *msg );
int getInt_fm_pk_struct( int index, int type );
char *getChar_fm_pk_struct( int index, int type );


void set_fm_pk_score( char *msg );
void swap_fm_pk_struct( int no1, int no2 );
int get_fm_pk_index( int win_index );
void read_fm_pk_score();
//void save_fm_pk_score();


#endif


#endif

