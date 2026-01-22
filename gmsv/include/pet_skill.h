#ifndef __PET_SKILL_H__
#define __PET_SKILL_H__

#include "util.h"

typedef enum
{
	PETSKILL_FIELD_ALL,			/* 埰迋趙摯鳹鐎ぁ窅窇堎 */
	PETSKILL_FIELD_BATTLE,				/* 橋    摯陑 */
	PETSKILL_FIELD_MAP,				/* 玊橖悃蚗鏤窀摯陑 */

}PETSKILL_FIELDTYPE;

typedef enum
{
	PETSKILL_TARGET_MYSELF,		/* 猷覕摯陑 */
	PETSKILL_TARGET_OTHER,			/* 眥摯硞  猷覕硈資) */
	PETSKILL_TARGET_ALLMYSIDE,		/*     蠈   */
	PETSKILL_TARGET_ALLOTHERSIDE,	/* Ъ鱗插蠈   */
	PETSKILL_TARGET_ALL,			/* 蠈趙 */
	PETSKILL_TARGET_NONE,			/* 移忒撒  ③  埵笢��  遛盓倜誧摯鄹 */
	PETSKILL_TARGET_OTHERWITHOUTMYSELF,/* 眥摯硞  猷覕硈竘埵笢) */
	PETSKILL_TARGET_WITHOUTMYSELFANDPET,  /* 猷覕敁妐蚗哱雄翻 */
}PETSKILL_TARGETTYPE;

typedef enum
{
	PETSKILL_ID,
	PETSKILL_FIELD,
	PETSKILL_TARGET,
#ifdef _PETSKILL2_TXT
	PETSKILL_USETYPE,
#endif
	PETSKILL_COST,
	PETSKILL_ILLEGAL,
	PETSKILL_DATAINTNUM,
}PETSKILL_DATAINT;

typedef enum
{
	PETSKILL_NAME,					/*      */
	PETSKILL_COMMENT,				/* 昡隍璃哱*/
	PETSKILL_FUNCNAME,				/* 匴倳   */
	PETSKILL_OPTION,				/* 酘鏤で砫璃 */
#ifdef _CFREE_petskill
	PETSKILL_FREE,					/*沭璃*/
	PETSKILL_KINDCODE,				/*笱濬鎢*/
#endif

	PETSKILL_DATACHARNUM,
}PETSKILL_DATACHAR;

typedef struct tagPetskill
{
	int			data[PETSKILL_DATAINTNUM];
	STRING64	string[PETSKILL_DATACHARNUM];

}Petskill;

typedef int (*PETSKILL_CALLFUNC)( int, int, int, char * );

INLINE BOOL PETSKILL_CHECKINDEX( int index );
INLINE int PETSKILL_getInt( int index, PETSKILL_DATAINT element);
INLINE int PETSKILL_setInt( int index, PETSKILL_DATAINT element, int data);
INLINE char* PETSKILL_getChar( int index, PETSKILL_DATACHAR element);
INLINE BOOL PETSKILL_setChar( int index ,PETSKILL_DATACHAR element, char* new );
int PETSKILL_getPetskillNum( void);

#define		PETSKILL_GetArray( charaindex, havepetskill)	_PETSKILL_GetArray( __FILE__, __LINE__, charaindex, havepetskill)
int _PETSKILL_GetArray( char *file, int line, int charaindex, int havepetskill );

BOOL PETSKILL_initPetskill( char *filename);
BOOL PETSKILL_reinitPetskill( void );
int PETSKILL_getPetskillArray( int petskillid);
PETSKILL_CALLFUNC PETSKILL_getPetskillFuncPointer(char* name);

int PETSKILL_Use(
	int charaindex,
	int toindex,
	int array,
	char *data
	//BOOL isCLI	// Robin 2001/02/26 if owner is player
);

int PETSKILL_ContinuationAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);
int PETSKILL_ChargeAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);


#define PETSKILL_ID_GBREAK		0	// 票↓迶々畛騷袲
#define PETSKILL_ID_RENZOKU		1	//   襄  賓
#define PETSKILL_ID_GUARDIAN	2	// 櫆憲辿鏍灃
#define PETSKILL_ID_CHARGE		3	// 鏍籤↓喇  賓
#define PETSKILL_ID_ICHIGEKI	100	// 郖賓  睍
#define PETSKILL_ID_POWERBALANCE 110	//     摯當

//**********************************************************************
//
//  歕  -- 眥硞禱陰堇埵誑��  賓
//
int PETSKILL_Guardian(
	int charaindex,
	int toindex,
	int array,
	char *data
);
//
//**********************************************************************

int PETSKILL_PowerBalance(
	int charaindex,
	int toindex,
	int array,
	char *data
);

//**********************************************************************
//
//  歕  -- 倯  煦誑樊  踏躂堎賃  誑禜笢
//
int PETSKILL_Mighty(
	int charaindex,
	int toindex,
	int array,
	char *data
);
//
//**********************************************************************


//**********************************************************************
//
//  歕  -- 筒  ↓淏筒釭橖  賓
//
int PETSKILL_StatusChange(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//**********************************************************************


//*******************************************************
//
// 歕  -- 玊橖  賓
//
int PETSKILL_NormalAttack(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
//
// 歕  -- 玊橖  遛
//
int PETSKILL_NormalGuard(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 歕  -- 輝敦(笰忒媃埵笢)
//
int PETSKILL_None(
	int charaindex,
	int toindex,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 歕  -- 貍曶郖棣(瞃躂趙兒淏↓璃  勗Ъ鱗摯    凝��  賓)
//
int PETSKILL_EarthRound(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 歕  -- 票↓迶々畛騷袲
//
int PETSKILL_GuardBreak(
	int charaindex,
	int toNo,
	int array,
	char *data

);
///////////////////////////////////////////
#ifdef _SKILL_GUARDBREAK2//ぢ壺滅郘2 vincent add 2002/05/20
int PETSKILL_GuardBreak2(
	int charaindex,
	int toNo,
	int array,
	char *data

);
#endif
//
//*******************************************************
//*******************************************************
// 歕  --     躂
//
int PETSKILL_Abduct(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************
//*******************************************************
// 歕  --   資
int PETSKILL_Steal(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//*******************************************************
#ifdef _BATTLESTEAL_FIX
int PETSKILL_StealMoney(
	int charaindex,
	int toNo,
	int array,
	char *data

);
#endif

#ifdef _ITEM_INSLAY
int PETSKILL_Inslay(
	int index,
	int toNo,
	int array,
	char *data

);
#endif

#ifdef _PETSKILL_FIXITEM
int PETSKILL_Fixitem(
	int index,
	int toNo,
	int array,
	char *data

);
#endif

// 歕  -- 囮騷  堜譴埬
//
int PETSKILL_Merge(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************

//*******************************************************
// 歕  -- 蚚↓票↓迶
//
int PETSKILL_NoGuard(
	int charaindex,
	int toNo,
	int array,
	char *data

);
//
//*******************************************************



// Terry add 2001/11/05
#ifdef __ATTACK_MAGIC
int PETSKILL_AttackMagic(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _VARY_WOLF
int PETSKILL_Vary( int cindex, int tindex, int id, char* data);
#endif

#ifdef _SKILL_WILDVIOLENT_ATT
//vincent add 2002/05/16
int PETSKILL_WildViolentAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_SPEEDY_ATT
//vincent add 2002/05/16
int PETSKILL_SpeedyAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_SACRIFICE
//vincent add 2002/05/30
int PETSKILL_Sacrifice(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_REFRESH
//vincent add 2002/08/08
int PETSKILL_Refresh(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_WEAKEN  //vincent唾撮:剞��
int PETSKILL_Weaken(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_DEEPPOISON  //vincent唾撮:曄馮 
int PETSKILL_Deeppoison(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_BARRIER  //vincent唾撮:藹梤
int PETSKILL_Barrier(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_NOCAST  //vincent唾撮:麥蘇
int PETSKILL_Nocast(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _SKILL_ROAR //vincent唾撮:湮綾
int PETSKILL_Roar(
	int charaindex,
	int toindex,
	int array,
	char *data
);
#endif

#ifdef _PSKILL_FALLGROUND
int PETSKILL_FallGround( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_EXPLODE
int PETSKILL_Explode( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PRO_BATTLEENEMYSKILL
int ENEMYSKILL_ReLife( int enemyindex, int toNo, int array, char *data );
int ENEMYSKILL_ReHP( int enemyindex, int toNo, int array, char *data );

int ENEMYSKILL_EnemyHelp( int enemyindex, int toNo, int array, char *data );
#endif

#ifdef _SKILL_DAMAGETOHP
int PETSKILL_DamageToHp( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_TIMID
int PETSKILL_BattleTimid( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_2TIMID
int PETSKILL_2BattleTimid( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_ANTINTER
int PETSKILL_AntInter(int charaindex, int toindex, int array, char* data);		// 唾昜撮夔桵須耀郪
#endif

#ifdef _PETSKILL_PROPERTY
int PETSKILL_BattleProperty( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_TEAR
int PETSKILL_BattleTearDamage( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _BATTLE_LIGHTTAKE
int PETSKILL_Lighttakeed( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _BATTLE_ATTCRAZED
int PETSKILL_AttackCrazed( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _SHOOTCHESTNUT	// Syu ADD 唾撮ㄩ隍璦赽
int PETSKILL_AttackShoot( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _Skill_MPDAMAGE
int PETSKILL_MpDamage( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_SETDUCK
int PETSKILL_SetDuck( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _MAGICPET_SKILL
int PETSKILL_SetMagicPet( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _SKILL_TOOTH
int PETSKILL_ToothCrushe( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PSKILL_MODIFY
int PETSKILL_Modifyattack( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PSKILL_MDFYATTACK
int PETSKILL_Mdfyattack( int charaindex, int toNo, int array, char *data );
#endif


#ifdef _MAGIC_SUPERWALL
int	PETSKILL_MagicStatusChange( int charaindex, int toindex, int array, char *data );
#endif

#ifdef _PET_SKILL_SARS				// WON ADD 馮伢雞晊
int PETSKILL_Sars( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _SONIC_ATTACK				// WON ADD 秞疏馴僻
int PETSKILL_Sonic( int charaindex, int toNo, int array, char *data );
#endif
#ifdef _PETSKILL_REGRET
int PETSKILL_Regret( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_GYRATE
int PETSKILL_Gyrate( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_ACUPUNCTURE //渀棧俋々
int PETSKILL_Acupuncture( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_RETRACE
int PETSKILL_Retrace( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_HECTOR
int PETSKILL_Hector( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_FIREKILL
int PETSKILL_Firekill( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_DAMAGETOHP   //做堎遼曖(岓悛撮腔曹极) 
int PETSKILL_DamageToHp2( int charaindex, int toNo, int array, char *data );
#endif

#ifdef _PETSKILL_BECOMEFOX
int PETSKILL_BecomeFox( int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_BECOMEPIG
int PETSKILL_BecomePig( int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_SHOWMERCY
int PETSKILL_ShowMercy(int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_COMBINED
int PETSKILL_Combined(int charaindex, int toNo, int array, char* data);
#endif

#ifdef _PETSKILL_LER
int PETSKILL_BatFly(int charaindex, int toNo, int array, char* data);					// 濘嫌撮 - �緊螤譬�
int PETSKILL_DivideAttack(int charaindex, int toNo, int array, char* data);		// 濘嫌撮 - 煦旯華蹊
#endif

#ifdef _PETSKILL_BATTLE_MODEL
int PETSKILL_BattleModel(int charaindex, int toindex, int array, char* data);		// 唾昜撮夔桵須耀郪
#endif

#endif
