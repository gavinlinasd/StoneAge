#ifndef __NPC_SCHEDULEMAN_H__
#define __NPC_SCHEDULEMAN_H__

#define CHARNAME_MAX	32

// 郔湮 scheduleman 杅講
#ifdef _FAMILY_MANORNUM_CHANGE
#define MAX_SCHEDULEMAN	20
#else
#define MAX_SCHEDULEMAN	12
#endif

// 藩珨跺 scheduleman 諷奪腔 schedule 杅講
#define MAX_SCHEDULE	24

// schedule 蹈桶腔珨珜垀夔蹈堤腔郔湮杅講
#define MAXSCHEDULEINONEWINDOW	10

// family 蹈桶腔珨珜垀夔蹈堤腔郔湮杅講
#define MAXFAMILYINONEWINDOW	10

// 藩珨跺 scheduleman 垀妏蚚腔 schedule 揣湔弇离
// 勤茼婓 fmpks 腔 ID*MAX_SCHEDULE 奻 (ID=0~MAX_SCHEDULEMAN-1)
// ID 岆迡婓 data/npc 笢硌隅跤 "id:" 腔硉

// dueltime = (路毞?10000:0) + (苤奀*100)

typedef struct tagFamilyPKSchedule {
	int	dueltime;	// 掀��奀覦
	int	host_index;	// 翋勦模逜
	char host_name[CHARNAME_MAX];	// 翋勦模逜靡備
	int	guest_index;	// 諦勦模逜
	char guest_name[CHARNAME_MAX];	// 諦勦模逜靡備
	int	prepare_time;	// 袧掘奀潔 (1~40 煦笘)
	int	max_player;	// 郔湮堤部�侕� (1~模逜�侕�奻癹)
	int	flag;		// 袨怓
	int	win;		// 吨瞳沭璃扢隅
	int	challenge_timeout;	// 泔桵奀癹
	int	setting_timeout;	// 扢隅袨怓腔 timeout
	char gmsv_name[256];
} FamilyPKSchedule;

// 槨翹婓 FamilyPKSchedule (fmpks) 腔 flag 硉
#define FMPKS_FLAG_NONE		-1	// 羶衄�庥恓鑒�
#define FMPKS_FLAG_CHALLENGE	0	// 脹渾諦勦肮砩笢
#define FMPKS_FLAG_SETTING	1	// 翋勦淏婓扢隅齬最 (�＋�奀曹傖 NONE)
#define FMPKS_FLAG_CONFIRMING	2	// 諦勦淏婓肮砩笢
#define FMPKS_FLAG_SCHEDULED	3	// 眒冪齬疑齬最ㄛ奾帤羲湖
#define FMPKS_FLAG_DUEL		4	// 羲湖笢
#define FMPKS_FLAG_HOSTWIN	5	// 翋勦吨
#define FMPKS_FLAG_GUESTWIN	6	// 諦勦吨
#define FMPKS_FLAG_MANOR_BATTLEBEGIN	7	// 蚽埶泔桵 桵須笢
#define FMPKS_FLAG_MANOR_PREPARE	8	// 蚽埶泔桵 袧掘笢
#define FMPKS_FLAG_MANOR_PEACE	9	// 蚽埶泔桵 倎桵笢
#define FMPKS_FLAG_MANOR_OTHERPLANET	10	// 蚽埶泔桵 婓梗腔陎⑩樵桵
#define FMPKS_FLAG_MANOR_BATTLEEND	11	// 蚽埶泔桵 桵須賦旰
#define FMPKS_FLAG_MANOR_PEACE_SAVE	12	// 蔚蚽埶泔桵湔紫
#define FMPKS_FLAG_MANOR_READYTOFIGHT	13	// (GM: manorpk) �峒ж絞銙刐尕槳論棧�
#define FMPKS_FLAG_MANOR_CLEANFLAG	14	// (GM: manorpk) 掛陎⑩ь壺袨怓
#ifdef _NEW_MANOR_LAW
#define FMPKS_FLAG_WAIT		15	// 輛�輲譭複�,眒暮翹模逜ァ岊,脹渾泔桵齬最
#endif

// 換跤 client 腔 flag
#define FLAG_NONE	-1
#define FLAG_MODIFY	0
#define FLAG_ACCEPT	1
#define FLAG_SETTING	2
#define FLAG_CONFIRM	3
#define FLAG_SCHEDULED	4

extern FamilyPKSchedule fmpks[MAX_SCHEDULE*MAX_SCHEDULEMAN];

BOOL NPC_SchedulemanInit(int meindex);
void NPC_SchedulemanTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_SchedulemanWindowTalked(int meindex, int talkerindex,
				 int seqno, int select, char *data);
void NPC_SchedulemanLoop(int meindex);

#endif 

/*__NPC_SCHEDULEMAN_H__*/
