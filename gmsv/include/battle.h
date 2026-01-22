#ifndef __BATTLE_H__
#define __BATTLE_H__

#define	BATTLE_ENTRY_MAX 10
#define	BATTLE_PLAYER_MAX 5
#define SIDE_OFFSET 	10

#define BATTLE_STRING_MAX	4096
#define BATTLE_TIME_LIMIT	(60*60)


#define DUELPOINT_RATE	(0.1)

enum{
	BATTLE_MODE_NONE = 0,	// 拸桵須袨怓
	BATTLE_MODE_INIT,		// 桵須場宎趙
	BATTLE_MODE_BATTLE,		// 桵須笢
	BATTLE_MODE_FINISH,		// 桵須賦旰
	BATTLE_MODE_STOP,		// 桵須賦旰(帤妏蚚)
	BATTLE_MODE_WATCHBC,	// 夤桵場宎趙
	BATTLE_MODE_WATCHPRE,	// 夤桵(羶釬蚚)
	BATTLE_MODE_WATCHWAIT,	// 夤桵(羶釬蚚)
	BATTLE_MODE_WATCHMOVIE, // 夤桵(羶釬蚚)
	BATTLE_MODE_WATCHAFTER, // 夤桵(羶釬蚚)
	BATTLE_MODE_END
};


enum{
	BATTLE_TYPE_NONE = 0,
	BATTLE_TYPE_P_vs_E = 1,
	BATTLE_TYPE_P_vs_P = 2,
	BATTLE_TYPE_E_vs_E = 3,
	BATTLE_TYPE_WATCH = 4,
	BATTLE_TYPE_DP_BATTLE = 5,
	BATTLE_TYPE_BOSS_BATTLE = 6,
	BATTLE_TYPE_END
};


enum{
	BATTLE_CHARMODE_NONE = 0,		// 拸桵須袨怓
	BATTLE_CHARMODE_INIT,			// 桵須場宎趙
	BATTLE_CHARMODE_C_WAIT,			// 脹渾桵須硌鍔
	BATTLE_CHARMODE_C_OK,			// 眒怀�鄳蓿煦蜂�
	BATTLE_CHARMODE_BATTLE,			// 帤妏蚚
	BATTLE_CHARMODE_RESCUE,			// 蚕help袨怓輛�賮�
	BATTLE_CHARMODE_FINAL,			// 桵須賦旰
	BATTLE_CHARMODE_WATCHINIT,		// 夤桵場宎趙
	BATTLE_CHARMODE_COMMAND,		// 帤妏蚚
	BATTLE_CHARMODE_END
};




typedef enum{
	BATTLE_ERR_NONE = 0,
	BATTLE_ERR_NOTASK,
	BATTLE_ERR_NOUSE,
	BATTLE_ERR_PARAM,
	BATTLE_ERR_ENTRYMAX,
	BATTLE_ERR_TYPE,
	BATTLE_ERR_CHARAINDEX,
	BATTLE_ERR_BATTLEINDEX,
	BATTLE_ERR_NOENEMY,
	BATTLE_ERR_ALREADYBATTLE,
	BATTLE_ERR_SAMEPARTY,
	BATTLE_ERR_END
}BATTLE_ERR;


enum{
	BATTLE_S_TYPE_PLAYER = 0,
	BATTLE_S_TYPE_ENEMY,
	BATTLE_S_TYPE_END
};


typedef enum{
	BATTLE_COM_NONE,
	BATTLE_COM_ATTACK,
	BATTLE_COM_GUARD,
	BATTLE_COM_CAPTURE,
	BATTLE_COM_ESCAPE,
	BATTLE_COM_PETIN,
	BATTLE_COM_PETOUT,
	BATTLE_COM_ITEM,
	BATTLE_COM_BOOMERANG,
	BATTLE_COM_COMBO,
	BATTLE_COM_COMBOEND,
	BATTLE_COM_WAIT,

	BATTLE_COM_SEKIBAN = 1000,
	BATTLE_COM_S_RENZOKU,
	BATTLE_COM_S_GBREAK,
	BATTLE_COM_S_GUARDIAN_ATTACK,
	BATTLE_COM_S_GUARDIAN_GUARD,
	BATTLE_COM_S_CHARGE,
	BATTLE_COM_S_MIGHTY,
	BATTLE_COM_S_POWERBALANCE,
	BATTLE_COM_S_STATUSCHANGE,
	BATTLE_COM_S_EARTHROUND0,
	BATTLE_COM_S_EARTHROUND1,
	BATTLE_COM_S_LOSTESCAPE,
	BATTLE_COM_S_ABDUCT,
	BATTLE_COM_S_STEAL,
	BATTLE_COM_S_NOGUARD,
	BATTLE_COM_S_CHARGE_OK,
	BATTLE_COM_JYUJYUTU = 2000,

	BATTLE_COM_COMPELESCAPE, //Ч秶燭羲

#ifdef __ATTACK_MAGIC
  BATTLE_COM_S_ATTACK_MAGIC,		// 唾昜藹楊
#endif

#ifdef _PSKILL_FALLGROUND
	BATTLE_COM_S_FALLRIDE,			//邈鎮扲
#endif
#ifdef _PETSKILL_EXPLODE
	BATTLE_COM_S_EXPLODE,
#endif
#ifdef _PETSKILL_TIMID
	BATTLE_COM_S_TIMID,
#endif
#ifdef _PETSKILL_2TIMID
	BATTLE_COM_S_2TIMID,
#endif
#ifdef _PETSKILL_ANTINTER
	BATTLE_COM_S_ANTINTER,
#endif
#ifdef _PETSKILL_PROPERTY
	BATTLE_COM_S_PROPERTYSKILL,
#endif
#ifdef _PETSKILL_TEAR
	BATTLE_COM_S_PETSKILLTEAR,
#endif
#ifdef _BATTLE_LIGHTTAKE
	BATTLE_COM_S_LIGHTTAKE,
#endif
#ifdef _BATTLE_ATTCRAZED		// ANDY 瑁遼惟軗
	BATTLE_COM_S_ATTCRAZED,
#endif
#ifdef _SHOOTCHESTNUT			// Syu ADD 唾撮ㄩ隍璦赽
	BATTLE_COM_S_ATTSHOOT,
#endif
#ifdef _BATTLESTEAL_FIX
	BATTLE_COM_S_STEALMONEY,
#endif
#ifdef _PRO_BATTLEENEMYSKILL
	BATTLE_COM_S_ENEMYRELIFE,		//NPC ENEMY 葩魂撮夔
	BATTLE_COM_S_ENEMYREHP,			//NPC ENEMY 硃悛撮夔
	BATTLE_COM_S_ENEMYHELP,			//NPC ENEMY 桸��
#endif
#ifdef _SKILL_DAMAGETOHP
	BATTLE_COM_S_DAMAGETOHP,		//岓悛撮
#endif
#ifdef _Skill_MPDAMAGE
	BATTLE_COM_S_MPDAMAGE,			//MP夼漲
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
    BATTLE_COM_S_WILDVIOLENTATTACK, //遼惟馴僻  vincent add 2002/05/16
#endif

#ifdef _SKILL_SPEEDY_ATT
    BATTLE_COM_S_SPEEDYATTACK,		//撞厒馴僻  vincent add 2002/05/20
#endif
#ifdef _SKILL_GUARDBREAK2
    BATTLE_COM_S_GBREAK2,			//ぢ壺滅郘2 vincent add 2002/05/20
#endif
#ifdef _SKILL_SACRIFICE
    BATTLE_COM_S_SACRIFICE,			//寰堔		vincent add 2002/05/30
#endif
#ifdef _SKILL_WEAKEN  
	    BATTLE_COM_S_WEAKEN,		//剞��		vincent add 2002/07/11
#endif
#ifdef _SKILL_DEEPPOISON  
	    BATTLE_COM_S_DEEPPOISON,	//曄馮		vincent add 2002/07/16
#endif
#ifdef _SKILL_BARRIER  
	    BATTLE_COM_S_BARRIER,		//藹梤		vincent add 2002/07/16
#endif
#ifdef _SKILL_NOCAST 
	    BATTLE_COM_S_NOCAST,		//麥蘇		vincent add 2002/07/16
#endif
#ifdef _SKILL_ROAR 
    BATTLE_COM_S_ROAR,			    //湮綾		vincent add 2002/07/11
#endif
#ifdef _BATTLENPC_WARP_PLAYER
	BATTLE_COM_WARP,                // npc warp player
#endif
#ifdef _SKILL_TOOTH
	BATTLE_COM_S_TOOTHCRUSHE,
#endif
#ifdef _PSKILL_MODIFY
	BATTLE_COM_S_MODIFYATT,
#endif
#ifdef _PSKILL_MDFYATTACK
	BATTLE_COM_S_MDFYATTACK,
#endif
#ifdef _MAGIC_SUPERWALL
	BATTLE_COM_S_SUPERWALL,
#endif
#ifdef _SKILL_REFRESH
    BATTLE_COM_S_REFRESH,
#endif
#ifdef _VARY_WOLF
	BATTLE_COM_S_VARY,
#endif
#ifdef _PETSKILL_SETDUCK
	BATTLE_COM_S_SETDUCK,
#endif
#ifdef _MAGICPET_SKILL
	BATTLE_COM_S_SETMAGICPET,
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 職業技能

// 拵呇
	BATTLE_COM_S_VOLCANO_SPRINGS,	// 鳶刓��	
	BATTLE_COM_S_FIRE_BALL,			// 鳶陎⑩
	BATTLE_COM_S_FIRE_SPEAR,		// 鳶韓Л
	BATTLE_COM_S_SUMMON_THUNDER,	// 欸濘扲
	BATTLE_COM_S_CURRENT,			// 萇霜扲
	BATTLE_COM_S_STORM,				// 惟瑞迾
	BATTLE_COM_S_ICE_ARROW,			// 梨璋扲
	BATTLE_COM_S_ICE_CRACK,			// 梨惇扲
	BATTLE_COM_S_ICE_MIRROR,		// 梨噩扲
	BATTLE_COM_S_DOOM,				// 岍賜藺��
	BATTLE_COM_S_BLOOD,				// 岓悛傖俶
	BATTLE_COM_S_BLOOD_WORMS,		// 岓悛嘍
	BATTLE_COM_S_SIGN,				// 珨渀獗悛
	BATTLE_COM_S_FIRE_ENCLOSE,		// 鳶蜇极	
	BATTLE_COM_S_ICE_ENCLOSE,		// 梨蜇极
	BATTLE_COM_S_THUNDER_ENCLOSE,	// 濘蜇极
	BATTLE_COM_S_ENCLOSE,			// 蜇旯扲
	BATTLE_COM_S_TRANSPOSE,			// 痄倛遙弇
// 蚋尪
	BATTLE_COM_S_BRUST,				// 惇僻			
	BATTLE_COM_S_CHAIN_ATK,			// 蟀遠馴僻
	BATTLE_COM_S_AVOID,				// 隙旌
	BATTLE_COM_S_RECOVERY,			// 硃悛
	BATTLE_COM_S_WEAPON_FOCUS,		// 挕ん蚳儕
	BATTLE_COM_S_REBACK,			// 袨怓隙葩
	BATTLE_COM_S_CHAIN_ATK_2,		// 邧笭馴僻
	BATTLE_COM_S_SCAPEGOAT,			// 忔眒峈衭
	BATTLE_COM_S_ENRAGE,			// 慾趙馴僻
	BATTLE_COM_S_COLLECT,			// 夔講擄摩
	BATTLE_COM_S_FOCUS,				// 蚳蛁桵須
	BATTLE_COM_S_SHIELD_ATTACK,		// 嗎僻
	BATTLE_COM_S_DUAL_WEAPON,		// 媼絮霜
	BATTLE_COM_S_DEFLECT,			// 跡紫
	BATTLE_COM_S_THROUGH_ATTACK,	// 嫗援馴僻
	BATTLE_COM_S_CAVALRY,			// 釱る馴僻
	BATTLE_COM_S_DEAD_ATTACK,		// 梭侚馴僻
	BATTLE_COM_S_CONVOLUTE,			// 隙唅馴僻
	BATTLE_COM_S_CHAOS,				// 髦觴馴僻
// 轂��
	BATTLE_COM_S_TRAP,				// 疪筘			
	BATTLE_COM_S_TRACK,				// 袚扆菩趿
	BATTLE_COM_S_DOCILE,			// 拲睦唾昜	
	BATTLE_COM_S_ENRAGE_PET,		// 慾躑唾昜	
	BATTLE_COM_S_DRAGNET,			// 毞蹕華厙
	BATTLE_COM_S_ENTWINE,			// 攷跦莊��
	BATTLE_COM_S_AUTARKY,			// 赻跤赻逋
	BATTLE_COM_S_PLUNDER,			//   极謨嗤
	BATTLE_COM_S_TOXIN_WEAPON,		// 馮匼挕ん
	BATTLE_COM_S_RESIST_FIRE,		// 鳶蕨俶枑汔
	BATTLE_COM_S_RESIST_ICE,		// 梨蕨俶枑汔
	BATTLE_COM_S_RESIST_THUNDER,	// 濘蕨俶枑汔
	BATTLE_COM_S_G_RESIST_FIRE,		// 芶极鳶蕨俶枑汔
	BATTLE_COM_S_G_RESIST_ICE,		// 芶极梨蕨俶枑汔
	BATTLE_COM_S_G_RESIST_THUNDER,	// 芶极濘蕨俶枑汔
	BATTLE_COM_S_ATTACK_WEAK,		// �黤蓍本�
	BATTLE_COM_S_INSTIGATE,			// 泔畢
	BATTLE_COM_S_OBLIVION,			// 疻咭
#ifdef _PROFESSION_ADDSKILL
	BATTLE_COM_S_RESIST_F_I_T,      // 赻�銓�夔
	BATTLE_COM_S_CALL_NATURE,       // 瘍欸赻��
    BATTLE_COM_S_BOUNDARY,          // 侐扽俶賦賜
#endif
#endif	

#ifdef _PET_SKILL_SARS				// WON ADD 職業延長
	BATTLE_COM_S_SARS,
#endif
#ifdef _SONIC_ATTACK				// WON ADD 秞疏馴僻
	BATTLE_COM_S_SONIC,
	BATTLE_COM_S_SONIC2,
#endif
#ifdef _PETSKILL_REGRET
	BATTLE_COM_S_REGRET,
	BATTLE_COM_S_REGRET2,
#endif
#ifdef _PETSKILL_GYRATE
    BATTLE_COM_S_GYRATE,
#endif
#ifdef _PETSKILL_ACUPUNCTURE
    BATTLE_COM_S_ACUPUNCTURE,
#endif
#ifdef _PETSKILL_RETRACE
	BATTLE_COM_S_RETRACE,
#endif
#ifdef _PETSKILL_HECTOR
	BATTLE_COM_S_HECTOR,
#endif
#ifdef _PETSKILL_FIREKILL
    BATTLE_COM_S_FIREKILL,
#endif
#ifdef _PETSKILL_DAMAGETOHP
	BATTLE_COM_S_DAMAGETOHP2, //做堎遼曖(岓悛撮腔曹极) 
#endif
#ifdef _PETSKILL_BECOMEFOX
    BATTLE_COM_S_BECOMEFOX,
#endif
#ifdef _PETSKILL_BECOMEPIG
    BATTLE_COM_S_BECOMEPIG,
#endif
#ifdef _PETSKILL_SHOWMERCY
    BATTLE_COM_S_SHOWMERCY,
#endif
#ifdef _PETSKILL_LER
		BATTLE_COM_S_BAT_FLY,					// 濘嫌撮 - �緊螤譬�
		BATTLE_COM_S_DIVIDE_ATTACK,		// 濘嫌撮 - 煦旯華蹊
#endif
#ifdef _PETSKILL_BATTLE_MODEL
		BATTLE_COM_S_BATTLE_MODEL,		// 唾昜撮夔桵須耀郪
#endif

	BATTLE_COM_END
}BATTLE_COM;


enum{
	BATTLE_RET_NORMAL,
	BATTLE_RET_CRITICAL,
	BATTLE_RET_MISS,
	BATTLE_RET_DODGE,
	BATTLE_RET_ALLGUARD,
#ifdef _EQUIT_ARRANGE
	BATTLE_RET_ARRANGE,
#endif
	BATTLE_RET_END
}BATTLE_RET;


#define BC_FLG_NEW			(1<<0)
#define BC_FLG_DEAD			(1<<1)
#define BC_FLG_PLAYER		(1<<2)
#define BC_FLG_POISON		(1<<3)
#define BC_FLG_PARALYSIS	(1<<4)
#define BC_FLG_SLEEP		(1<<5)
#define BC_FLG_STONE		(1<<6)
#define BC_FLG_DRUNK		(1<<7)
#define BC_FLG_CONFUSION	(1<<8)
#define BC_FLG_HIDE			(1<<9)
#define BC_FLG_REVERSE		(1<<10)
#ifdef _MAGIC_WEAKEN
#define BC_FLG_WEAKEN		(1<<11) // 剞��
#endif
#ifdef _MAGIC_DEEPPOISON
#define BC_FLG_DEEPPOISON	(1<<12) // 曄馮
#endif
#ifdef _MAGIC_BARRIER
#define BC_FLG_BARRIER		(1<<13) // 藹梤
#endif
#ifdef _MAGIC_NOCAST
#define BC_FLG_NOCAST		(1<<14) // 麥蘇
#endif

#ifdef _PET_SKILL_SARS			// WON ADD 職業延長
#define BC_FLG_SARS			(1<<15) // 馮伢
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 職業技能
#define BC_FLG_DIZZY		(1<<16)	// 婠悈	
#define BC_FLG_ENTWINE		(1<<17)	// 攷跦莊��
#define BC_FLG_DRAGNET		(1<<18)	// 毞蹕華厙	
#define BC_FLG_ICECRACK	    (1<<19)	// 梨惇扲	
#define BC_FLG_OBLIVION  	(1<<20)	// 疻咭
#define BC_FLG_ICEARROW		(1<<21)	// 梨璋
#define BC_FLG_BLOODWORMS 	(1<<22)	// 岓悛嘍
#define BC_FLG_SIGN 		(1<<23)	// 珨渀獗悛
#define BC_FLG_CARY			(1<<24) // 泔畢
#define BC_FLG_F_ENCLOSE	(1<<25) // 鳶蜇极
#define BC_FLG_I_ENCLOSE	(1<<26) // 梨蜇极
#define BC_FLG_T_ENCLOSE	(1<<27) // 濘蜇极
#ifdef _PROFESSION_ADDSKILL
#define BC_FLG_WATER	    (1<<28) // 阨蜇极
#define BC_FLG_FEAR			(1<<29) // 謁曉
#endif
#ifdef _PETSKILL_LER
#define BC_FLG_CHANGE			(1<<30) // 濘嫌曹旯
#endif
#endif




#define BP_FLG_JOIN					(1<<0)
#define BP_FLG_PLAYER_MENU_OFF		(1<<1)
#define BP_FLG_BOOMERANG			(1<<2)
#define BP_FLG_PET_MENU_OFF			(1<<3)
#define BP_FLG_ENEMY_SURPRISAL		(1<<4)
#define BP_FLG_PLAYER_SURPRISAL		(1<<5)


#define CHAR_BATTLEFLG_ULTIMATE		(1<<0)
#define CHAR_BATTLEFLG_AIBAD		(1<<1)
#define CHAR_BATTLEFLG_REVERSE		(1<<2)
#define CHAR_BATTLEFLG_GUARDIAN		(1<<3)
#define CHAR_BATTLEFLG_NORETURN		(1<<4)
#define CHAR_BATTLEFLG_RECOVERY		(1<<5)
#define CHAR_BATTLEFLG_ABIO			(1<<6)
#define CHAR_BATTLEFLG_NODUCK		(1<<7)

#define GETITEM_MAX	3
typedef struct
{
	int attacker;
}INVADER;

typedef struct
{
	int		charaindex;
	int		bid;
	int		escape;
	int		flg;
	int		guardian;
	int		duelpoint;
	int		getitem[GETITEM_MAX];
}BATTLE_ENTRY;
#define BENT_FLG_ULTIMATE	(1<<0)

typedef struct
{
	int		type;
	int		flg;
	int		common_dp;
	BATTLE_ENTRY	Entry[BATTLE_ENTRY_MAX];
}BATTLE_SIDE;

#define BSIDE_FLG_SURPRISE ( 1 << 0 )
#define BSIDE_FLG_HELP_OK ( 1 << 1 )



typedef struct _Battle
{
	BOOL	use;	/* 窅埱趙笢堎凝汔竣凝 */
	int		battleindex; /* 泬哱鳴  蠕 */
	int		mode;	/* 藯凜摯橋  さ↓迶 */
	int		type;	/* 橋  淏騷鏤 (0:玊橖)(1:DUEL)(2:尨筒橋) */
	int		dpbattle;	/* DP泬哱鳴凝ㄑ */
	int		norisk;	/* 屻庌ぁ忒惉筒袲摯  笢泬哱鳴凝ㄑ */
	int		turn;	/* 淏↓璃倳 */
	int		timer;	/* 坳操騷哱蹇勗窅竣淏騷悃 */
	int		leaderindex; /* 操璃庈坳璃哱禱厔喫媃倜す籤溘摯騷璃溢蚗袲筒 */
	int		winside; /*   媋媃倜�蚅枅� */
	int		field_att;	/* 啞贖↓鳴迶摯鶂鍛 */
	int		att_count;	/* 啞贖↓鳴迶摯鶂鍛  撬  摯淏騷悃 */
	int		att_pow;	/* 啞贖↓鳴迶摯鶂鍛  撬  摯蚕極↓ */
	int		field_no;
	int		flg;		/* 掘攣啞溘精 */
	BATTLE_SIDE		Side[2];
#ifdef _BATTLE_TIMESPEED
	unsigned int CreateTime;
	unsigned int EndTime;
	unsigned int PartTime;
	int flgTime;
	
#endif

#ifdef _PROFESSION_ADDSKILL
	//梨惇扲湔溫
	int ice_count;//婃湔杅講
	int ice_bout[20];//隙磁數杅
	BOOL ice_use[20];//岆瘁妏蚚
	int ice_toNo[20];//勤砓
	int ice_level[20];//馴僻薯
	int ice_array[20];
	int ice_charaindex[20];
	int ice_attackNo[20];
#endif

#ifdef _DEATH_CONTEND
	int menum;
	int tonum;
#endif

#ifdef _ACTION_BULLSCR
	int enemynum;
#endif
	int		iEntryBack[BATTLE_ENTRY_MAX*2];		// 鞳淏↓璃頞  踏躂趙笢倜隍璃泬↓
	int		iEntryBack2[BATTLE_ENTRY_MAX*2];		// 鞳淏↓璃頞  踏躂趙笢倜隍璃泬↓
	int		createindex;	/* 喫摯橋  禱軘堇③媃倜す籤溘騷璃溢蚗袲筒(NPC埵汔) */
	int    (*WinFunc)( int battleindex, int charaindex );
#ifdef _DEATH_CONTEND//數呾吨啖func
	int    (*PkFunc)( int menum, int tonum, int winside, int battlemap);
	int battlemap;
#endif
	struct _Battle *pNext;		// べ摯め橋精鳴↓鏤
	struct _Battle *pBefore;	// 鞳摯め橋精鳴↓鏤
}BATTLE;

#define BATTLE_FLG_FREEDP ( 1 << 0 )	//   ど趙忒    蚍�欷聿�
#define BATTLE_FLG_CHARALOST   ( 1 << 1 )	// す籤溘誑屻庌煦堇  崹倜堇媃趙笢堎



enum{	// 窀摯field_att 勗  躂堎偯
	BATTLE_ATTR_NONE = 0,	//   鶂鍛
	BATTLE_ATTR_EARTH,		// 貍
	BATTLE_ATTR_WATER,		//   
	BATTLE_ATTR_FIRE,		// 庄
	BATTLE_ATTR_WIND,		// 諿
	BATTLE_ATTR_END
};


//krynn 2001/12/28
enum
{
	BATTLE_SIDE_RIGHT,		// 桵須笢衵耜	right side of battle
	BATTLE_SIDE_LEFT,		// 桵須笢酘耜	left side of battle
	BATTLE_SIDE_WATCH,		// 夤桵			watch battle player
};
//krynn end

#define TARGET_SIDE_0	20	// 蒟插  �蚅枅顐�  炩擭
#define TARGET_SIDE_1	21	// 嫌插  �蚅枅顐�  炩擭
#define TARGET_ALL	22	// 蠈  炩擭

#ifdef __ATTACK_MAGIC

#define TARGET_SIDE_0_B_ROW     26      // 衵狟摽珨蹈
#define TARGET_SIDE_0_F_ROW     25      // 衵狟ヶ珨蹈
#define TARGET_SIDE_1_F_ROW     24      // 酘奻ヶ珨蹈
#define TARGET_SIDE_1_B_ROW     23      // 酘奻摽珨蹈

// won add
#define	TARGER_THROUGH			27

#endif

extern int	gItemCrushRate;		//   癟  溢啞б鳴哱
extern BATTLE *BattleArray; 	/* 橋  淏筒袲     */
extern int BATTLE_battlenum;	/*     摯倳 */
extern char szAllBattleString[BATTLE_STRING_MAX];	/* 橋  勗窅竣昡悃璃迶  棬   */
extern char *pszBattleTop, *pszBattleLast;	/* 橋  勗窅竣  棬  摯痾   */
extern char szBadStatusString[];	// 筒  ↓淏筒釭橖暵  棬  
extern int gWeponType;	// 藯凜摯  梲摯攣  
extern float gDamageDiv;	// 譫隍↓喇覕鄎

#define BATTLE_CHECKINDEX( a ) ( ((a)>=BATTLE_battlenum || (a)<0 )?(FALSE):(TRUE) )
#define BATTLE_CHECKSIDE( a ) ( ((a)>=2 || (a)<0)?(FALSE):( TRUE) )
#define BATTLE_CHECKNO( a ) ( ((a)>=20 || (a)<0 )?(FALSE):(TRUE) )
#define BATTLE_CHECKADDRESS( a )  ((&BattleArray[0])<=(a) && (a)<=(&BattleArray[BATTLE_battlenum-1] )?(TRUE):(FALSE) ) 

#define IsBATTLING( a ) (CHAR_getWorkInt((a),CHAR_WORKBATTLEMODE)?(TRUE):(FALSE))

#define STRCPY_TAIL( _pszTop, _pszLast, _szBuffer)	{ int _len = strlen( _szBuffer ); ( _pszTop + _len < (_pszLast)-1 )?( memcpy( _pszTop, _szBuffer, _len ),	_pszTop += _len, _pszTop[0] = 0):(0); }

#define BATTLESTR_ADD( _szBuffer ){ int _len = strlen( _szBuffer ); ( pszBattleTop + _len < ( pszBattleLast)-1 )?( memcpy( pszBattleTop, _szBuffer, _len ),	pszBattleTop += _len, pszBattleTop[0] = 0):(0); }

#define BATTLE_MAP_MAX 219

#define CH_FIX_PLAYERLEVELUP	(+2)	// 鏤畛騷媕↓摯畛穫鳴誑囮蚗鏤
#define CH_FIX_PLAYERDEAD		(-2)	// 鏤畛騷媕↓誑玊橖屻
#define CH_FIX_PLAYEULTIMATE	(-4)	// 鏤畛騷媕↓誑囮鳴  贖隍蚗哱屻
#define CH_FIX_PETESCAPE		(-1)	// 妐蚗哱誑  ど倜

#define AI_FIX_PETLEVELUP		(+5*100)	// 妐蚗哱誑畛穫鳴囮蚗鏤
#define AI_FIX_PETWIN			(+1)		// 妐蚗哱誑傍禱槸媃倜
#define AI_FIX_PETGOLDWIN		(+2*10)	// 妐蚗哱誑畛穫鳴摯禜笢傍禱槸媃倜
#define AI_FIX_PETRECOVERY		(+10)		// 橋    勗樊蜾媃趙忒�梊�倜
#define AI_FIX_PETRESSURECT		(+3*100)	// 橋    勗蜾阽媃趙忒�梊�倜
//#define AI_FIX_PETRECOVERY	(+50)		// 橋    勗樊蜾媃趙忒�梊�倜

#define AI_FIX_SEKKAN 			(-2*100)	// 猷覕摯妐蚗哱禱  賓
#define AI_FIX_PLAYERULTIMATE	(-10*100)	// 猷覕摯Д硞誑囮鳴  贖隍蚗哱屻
#define AI_FIX_PETULTIMATE		(-10*100)	// 妐蚗哱誑囮鳴  贖隍蚗哱屻
#define AI_FIX_PLAYERDEAD		(-1*100)	// 猷覕摯Д硞誑縈憪
#define AI_FIX_PETDEAD			(-5*100)	// 妐蚗哱誑縈憪


#ifdef _Item_ReLifeAct
int BATTLE_getBattleDieIndex( int battleindex, int bid );
#endif

int BATTLE_No2Index( int battleindex, int No);

int BATTLE_Index2No( int battleindex, int charaindex);

BOOL BATTLE_initBattleArray( int battlenum);

int BATTLE_CreateBattle( void );
int BATTLE_DeleteBattle( int battleindex);

int BATTLE_NewEntry( int charaindex, int battleindex, int side);

#define BATTLE_Exit( charaindex, battleindex) _BATTLE_Exit( __FILE__, __LINE__, charaindex, battleindex)
INLINE int _BATTLE_Exit( char *file, int line, int charaindex ,int battleindex);

#define BATTLE_ExitAll( battleindex) _BATTLE_ExitAll( __FILE__, __LINE__, battleindex)
INLINE void _BATTLE_ExitAll( char *file, int line, int battleindex);

int BATTLE_CreateVsPlayer( int charaindex0,	int charaindex1);

int BATTLE_CreateVsEnemy( int charaindex, int mode,	int	npcindex);

int BATTLE_CountEntry( int battleindex,	int side);

int BATTLE_Loop( void );

int BATTLE_FinishSet( int battleindex );
int BATTLE_StopSet( int battleindex );
int BATTLE_RescueEntry( int charaindex, int toindex);

int BATTLE_PetDefaultExit( int charaindex, int battleindex);

int	BATTLE_PetDefaultEntry(
	int charaindex,	// 妐蚗哱禱  埱趙笢堎鏤畛騷媕↓摯          
	int battleindex,// 泬哱鳴騷璃溢蚗袲筒
	int side
);

BOOL BATTLE_RescueTry( int charaindex);

BOOL BATTLE_RescueParentTry(
		int charaindex,
		int pindex
	);

int BATTLE_DefaultAttacker(	int battleindex, int side);

BOOL BATTLE_IsThrowWepon( int itemindex);

void BATTLE_BadStatusString( int defNo, int status );
int BATTLE_MultiList( int battleindex, int toNo, int ToList[] );
BOOL BATTLE_IsCharge( int com );
BOOL BATTLE_CanMoveCheck( int charaindex );
int BATTLE_TargetCheck( int battleindex, int defNo);
char *BATTLE_CharTitle( int charaindex );
void BATTLE_EscapeDpSend( int battleindex, int charaindex );
int BATTLE_GetDuelPoint( int battleindex, int side, int num);
int BATTLE_TargetCheckDead(	int battleindex, int defNo);

void BATTLE_MultiListDead( int battleindex, int toNo, int ToList[] );
BOOL BATTLE_WatchTry( int charaindex);
int BATTLE_WatchEntry( int charaindex, int toindex);
void BATTLE_WatchStop( int charaindex );
int BATTLE_WatchUnLink( int battleindex );
void BATTLE_BpSendToWatch( BATTLE *pBattle,	char *pszBcString);

int BATTLE_GetWepon( int charaindex );

#ifdef _ITEM_EQUITSPACE
int BATTLE_GetEqShield( int charaindex );
#endif

int BATTLE_GetAttackCount( int charaindex );
int DoujyouRandomWeponSet( int charaindex );
void BATTLE_AttReverse( int charaindex );
void BATTLE_BadStatusAllClr( int charaindex );
#define CHAR_GETWORKINT_HIGH( index, pos )	( CHAR_getWorkInt( (index), (pos) ) >> 16 )
#define CHAR_SETWORKINT_HIGH( index, pos, set )	{ int iTmp = CHAR_getWorkInt( (index), (pos) ) & 0xFFFF, work = (set);	CHAR_setWorkInt( (index), (pos), (work << 16)|iTmp ); }
#define CHAR_GETWORKINT_LOW( index, pos )	( CHAR_getWorkInt( (index), (pos) ) & 0xFFFF )
#define CHAR_SETWORKINT_LOW( index, pos, set )	{ int iTmp = CHAR_getWorkInt( index, pos ) & 0xFFFF0000, work = (set);	CHAR_setWorkInt( (index), (pos), (work & 0x0000FFFF) | iTmp ); }

int Battle_getTotalBattleNum();

#ifdef _TYPE_TOXICATION
void CHAR_ComToxicationHp( int charaindex);
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 職業技能
void BATTLE_ProfessionStatus_init( int battleindex, int charaindex );
void BATTLE_ProfessionStatusSeq( int battleindex, int charaindex);
#endif


#endif
