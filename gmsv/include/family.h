#ifndef __FAMILY_H__
#define __FAMILY_H__

#include "version.h"
#include "common.h"
#include "util.h"
#include "net.h"
#include "time.h"

#define FAMILY_MAXNUM			1000	// 模逜杅講
#ifdef _FMVER21
#define FAMILY_MAXMEMBER		100	// 模逜�侕�
#define FAMILY_MAXCHANNELMEMBER		50	// け耋�侕�
#else
#define FAMILY_MAXMEMBER		50	// 模逜�侕�
#define FAMILY_MAXCHANNELMEMBER		10	// け耋�侕�
#endif
#define FAMILY_MAXCHANNEL 		5	// 模逜け耋

#define CHAR_MAXNAME			20
#define CHAR_MAXID			20
#define MINFMLEVLEFORPOINT      	3       // 3 扠③蚽埶郔腴脹撰
#define FMLEADERLV			30	// 逜酗脹撰

#ifdef _FAMILY_MANORNUM_CHANGE
#define FAMILY_FMPKFLOOR		15	// 模逜�苺冞撞�
#else
#define FAMILY_FMPKFLOOR		9	// 模逜�苺冞撞�
#define FMPOINTNUM			4       // 衄擂萸模逜腔郔湮杅講
#define MANORNUM                	4
#define FAMILY_MAXHOME			4	// 模逜擂萸
#endif

enum
{
    FM_TOP_INTEGRATE = 1,    // DPTOP 軘磁
    FM_TOP_ADV,              // DPTOP 簸玸
    FM_TOP_FEED,             // DPTOP 侞郤
    FM_TOP_SYNTHESIZE,       // DPTOP 磁傖
    FM_TOP_DEALFOOD,         // DPTOP 蹋燴
    FM_TOP_PK,               // DPTOP �苺�
#ifdef _NEW_MANOR_LAW
		FM_TOP_MOMENTUM = 8,		 // DPTOP ァ岊
#endif
    FM_TOP_NUM,              // DPTOP 杅講
};

enum
{
	FM_FIX_ACCEPTFLAG = 1,
	FM_FIX_FMPK,
	FM_FIX_FMPET,
	FM_FIX_FMRULE,
	FM_FIX_DELFMTIME,
	FM_FIX_FMGOLD,
	FM_FIX_FMADV,
	FM_FIX_FMFEED,
	FM_FIX_FMSYNTHESIZE,
	FM_FIX_FMDEALFOOD,
	FM_FIX_FMLEADERCHANGE,
#ifdef _NEW_MANOR_LAW
	FM_FIX_FMMOMENTUM,
	FM_FIX_FAME,
#endif
};

/*
 * �荂懽�    摯眥摯鳹鐎敁摯 cdkey charname 摯荇踏禱譴鞠堎倜誧勗
 * CHEKEYLEN, CHARNAMELEN
 * 禱窅竣源竣勗  脾��
 */
    
void CHAR_Family(int fd, int index, char* message);
void ACAddFM(int fd, int result, int fmindex, int index);
void ACJoinFM(int fd, int result, int recv);
void ACLeaveFM(int fd, int result, int resultflag);
void ACDelFM(int fd, int result);
void ACShowFMList(int ret, int fmnum, char *data);
void ACFMDetail(int ret, char *data, int charfdid);
void ACShowMemberList(int result, int index, int fmnumm,
int fmacceptflag, int fmjoinnum, char *data);
void ACShowDpTop(int result,int num, char *data, int kindflag);
void ACShowPointList(int result, char *data);
void ACShowFMMemo(int result, int index, int num, int dataindex, char *data);

#ifdef _PERSONAL_FAME   // Arminius: 模逜跺�侂驫�
void ACFMCharLogin(int fd, int ret, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex, int charfame
	#ifdef _NEW_MANOR_LAW
	,int momentum
	#endif
	);
#else
void ACFMCharLogin(int fd, int ret, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex);
#endif

void ACFMPointList(int ret, char *data);
void ACSetFMPoint(int ret, int r, int clifd);
void ACFMAnnounce(int ret, char *fmname, int fmindex, int index, int kindflag,
	char *data, int color);
void ACFixFMPK(int winindex, int loseindex, int data);
void ACFMJob( int fd, int ret, char* data1, char* data2 );

void FAMILY_Add(int fd, int meindex, char *message);
void FAMILY_Join(int fd, int meindex, char *message);
void FAMILY_Leave(int fd, int meindex, char *message);
void FAMILY_Detail(int fd, int meindex, char *message);
void FAMILY_CheckMember(int fd, int meindex, char *message);
void FAMILY_Channel(int fd, int meindex, char *message);
void FAMILY_Bank(int fd, int meindex, char *message);
void FAMILY_SetPoint(int fd, int meindex, char *message);
void FAMILY_Init(void);
void FAMILY_SetAcceptFlag(int fd, int meindex, char *message);
void FAMILY_FixRule( int fd, int meindex, char* message );
void FAMILY_RidePet( int fd, int meindex, char* message );
void FAMILY_LeaderFunc( int fd, int meindex, char* message );



#ifdef _CK_ONLINE_PLAYER_COUNT    // WON ADD 數呾盄奻�侕�
void GS_SEND_PLAYER_COUNT(void);
#endif

void SetFMPetVarInit(int meindex);
void SetFMVarInit(int meindex);


int CheckFMLeader(int meindex);
int getFmLv(int playerindex);

void getNewFMList( void );
void checkFamilyIndex( void );

int CheckLeaderQ(int meindex);


// shan add Begin
struct FMMEMBER_LIST
{
//  int  fmindex;
    int  fmnum; 
    int  fmjoinnum;
//  BOOL use;                                       // 0->羶妏蚚   1->妏蚚
    int  memberindex[FAMILY_MAXMEMBER];             
    char numberlistarray[FAMILY_MAXMEMBER][64];
    char memo[35][220];                             // family dengon
    int  accept;                                    // 欸躁傖埜迵瘁
    int  memonum;
    int  memoindex;
};
// 模逜眳潔腔隱晟啣
struct FMS_MEMO
{
    char memo[140][220];
    int  memonum;
    int  memoindex;
};
// 模逜Ч氪桶
struct FMS_DPTOP
{
    int  num;															// 暮翹衄嗣屾跺模逜(軘磁)
    char topmemo[FAMILY_MAXNUM][128];
    int  fmtopid[FAMILY_MAXNUM];          // 模逜坰竘
#ifdef _FMVER21    
    int  fmtopdp[FAMILY_MAXNUM];          // 模逜軘磁汒咡
#endif    
#ifdef _NEW_MANOR_LAW
		int	fmMomentum[FAMILY_MAXNUM];		// 模逜ァ岊
		char	momentum_topmemo[30][96];			// 模逜ァ岊 top
		int	momentum_topid[FAMILY_MAXNUM];	// 模逜ァ岊 top id 坰竘
#endif
    int  adv_num;                // 簸玸
    char adv_topmemo[30][96];    
    int  feed_num;               // 侜郤
    char feed_topmemo[30][96];        
    int  syn_num;                // 磁傖
    char syn_topmemo[30][96];        
    int  food_num;               // 蹋燴
    char food_topmemo[30][96];        
    int  pk_num;                 // �苺�
    char pk_topmemo[30][96];        
};
// 模逜擂萸
struct FM_POINTLIST
{
  char pointlistarray[FAMILY_MAXHOME][1024];	// Arminius: 32->1024

#ifdef _NEW_MANOR_LAW
	int fm_momentum[FAMILY_MAXHOME];	// 暮翹泔桵奀ぶ羲宎奀腔忐蚽模逜ァ岊硉
	BOOL fm_inwar[FAMILY_MAXHOME];		// 森蚽埶岆瘁輛俴蚽埶齬最笢
#endif
};
// 模逜PK芞脯
struct FM_PKFLOOR
{
    int fl;
};
// End

#ifdef _NEW_MANOR_LAW
typedef struct _ManorSchedule_t{
	int iFmIndex[10];						// 齬�輲譭褐鑒拑躁眢樆壨�
	int iFmMomentum[10];				// 模逜ァ岊
	int iSort[10];							// 齬靡蚚
	char szMemo[10][256];				// 暮翹: 模逜靡備|埮桵奀潔|模逜ァ岊
	char szFmName[10][32];			// 模逜靡備
	struct tm tm1[10];							// 暮翹泔桵奀潔
}ManorSchedule_t;

extern ManorSchedule_t ManorSchedule[MANORNUM];
#endif

#define MAXFAMILYLIST 120000
extern char    familyListBuf[MAXFAMILYLIST];

void JoinMemberIndex( int charaindex, int fmindexi);


#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 模逜桵GM硌鍔


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
int get_fm_leader_index( int fm1 );


#endif



#endif
