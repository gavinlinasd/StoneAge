#ifndef __BATTLE_EVENT_H__
#define __BATTLE_EVENT_H__

#define BCC_HIT		'H'
#define BCC_FIRE	'F'

#define BCF_DEATH		(1 << 0)
#define BCF_NORMAL		(1 << 1)
#define BCF_KAISHIN		(1 << 2)
#define BCF_GUARD		(1 << 3)
#define BCF_COUNTER		(1 << 4)
#define BCF_DODGE		(1 << 5)
#define BCF_ULTIMATE_1	(1 << 6)
#define BCF_ULTIMATE_2	(1 << 7)
#define BCF_GBREAK		(1 << 8)
#define BCF_GUARDIAN	(1 << 9)
#define BCF_REFRECT		(1 << 10)
#define BCF_ABSORB		(1 << 11)
#define BCF_VANISH		(1 << 12)
#define BCF_CRUSH		(1 << 13)
#define BCF_FALL		(1 << 14)	//落马术
#ifdef _SKILL_TOOTH
#define BCF_TOOTH		(1 << 15)	//  齿
#endif
#ifdef _PSKILL_MODIFY
#define BCF_ATTDOUBLE	(1 << 16)	//属性强化
#endif
#ifdef _SKILL_ROAR  
#define BCF_ROAR		(1 << 17)	//宠技:大吼(克年兽)
#endif 

#ifdef _MAGIC_DEFMAGICATT
#define BCF_DEFMAGICATT	(1 << 19)
#endif
#ifdef  _MAGIC_SUPERWALL
#define BCF_SUPERWALL	(1 << 20)
#endif
#ifdef _PSKILL_MDFYATTACK
#define BCF_MODIFY		(1 << 21)
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#define BCF_F_SKILLACT	(1 << 22)	//击中前秀图
#define	BCF_TRAP		(1 << 25)	//陷阱
#define BCF_NO_DAMAGE   (1 << 26)   //双重攻击
#endif
#define BCF_B_SKILLACT	(1 << 23)	//击中後秀图



#ifdef _EQUIT_ARRANGE
#define BCF_B_ARRANGE	(1 << 24)	//格挡秀图
#endif

#ifdef _PETSKILL_ACUPUNCTURE
#define BCF_ACUPUNCTURE (1 << 27)
#endif

#ifdef _PETSKILL_ANTINTER
#define BCF_ANTINTER    (1 << 28)
#endif
#ifdef _PETSKILL_EXPLODE
#define BCF_EXPLODE    (1 << 29)
#endif

#ifdef _OTHER_MAGICSTAUTS
#ifdef _MAGICSTAUTS_RESIST
#define MAXSTATUSTYPE 6
#else
#define MAXSTATUSTYPE 3
#endif //_MAGICSTAUTS_RESIST
extern char MagicStatus[MAXSTATUSTYPE][36];
extern int MagicTbl[];
extern int MagicTypeTbl[];
#endif

extern char *aszStatus[];
extern char *aszStatusFull[];
extern int StatusTbl[];
extern int RegTbl[];
extern int MagicDefTbl[];
extern char *aszMagicDefFull[];
extern char *aszMagicDef[];

extern char *aszParamChange[];
extern char *aszParamChangeFull[];
extern int aParamChangeTbl[];

extern float gBattleDamageModyfy;
extern int   gBattleDuckModyfy;
extern int	 gBattleStausChange;
extern int	 gBattleStausTurn;

enum{
	BATTLE_ST_NONE,		 //  0 "正常",
	BATTLE_ST_POISON,	 //  1 "毒",
	BATTLE_ST_PARALYSIS, //  2 "麻",
	BATTLE_ST_SLEEP,	 //  3 "眠",
	BATTLE_ST_STONE,	 //  4 "石",
	BATTLE_ST_DRUNK,	 //  5 "醉",
	BATTLE_ST_CONFUSION, //  6 "乱",
    BATTLE_ST_WEAKEN,    //  7 "虚"
	BATTLE_ST_DEEPPOISON,//  8 "剧毒"
	BATTLE_ST_BARRIER,   //  9 "魔障"
	BATTLE_ST_NOCAST,    // 10 "沉默"
#ifdef _PET_SKILL_SARS				// WON ADD 毒煞蔓延
	BATTLE_ST_SARS,		 // 11 "煞"
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	BATTLE_ST_DIZZY,	 // 12 "晕"
	BATTLE_ST_ENTWINE,	 // 13 "缠"
	BATTLE_ST_DRAGNET,	 // 14 "天罗地网"
	BATTLE_ST_ICECRACK,  // 15 "冰爆术"
	BATTLE_ST_OBLIVION,	 // 16 "遗忘"
	BATTLE_ST_ICEARROW,	 // 17 "冰箭"
	BATTLE_ST_BLOODWORMS,// 18 "嗜血蛊"
	BATTLE_ST_SIGN,		 // 19 "一针见血"
	BATTLE_ST_INSTIGATE, // 20 "挑拨"
	BATTLE_ST_F_ENCLOSE, // 21 "火附体"
	BATTLE_ST_I_ENCLOSE, // 22 "冰附体"
	BATTLE_ST_T_ENCLOSE, // 23 "雷附体"
	BATTLE_ST_FOCUS,	 // 24 "专注战斗"	
	BATTLE_ST_RESIST_F,	 // 25 "火抗"
	BATTLE_ST_RESIST_I,	 // 26 "冰抗"
	BATTLE_ST_RESIST_T,	 // 27 "雷抗"
	BATTLE_ST_F_ENCLOSE2, // 28 "火附"
	BATTLE_ST_I_ENCLOSE2, // 29 "冰附"
	BATTLE_ST_T_ENCLOSE2, // 30 "雷附"
#ifdef _PROFESSION_ADDSKILL
    BATTLE_ST_RESIST_F_I_T, //31 "火冰雷抗"
    //BATTLE_ST_BOUNDARY_F, //32 "火结界
	BATTLE_ST_WATER, //32 "水附体"
	BATTLE_ST_WORKANNEX, //33 "附身"
	BATTLE_ST_FEAR, //34 "恐惧"
	BATTLE_ST_ICECRACK2,  // 35 "冰爆术"
	BATTLE_ST_ICECRACK3,  // 36
	BATTLE_ST_ICECRACK4,  // 37
	BATTLE_ST_ICECRACK5,  // 38
	BATTLE_ST_ICECRACK6,  // 39
	BATTLE_ST_ICECRACK7,  // 40
	BATTLE_ST_ICECRACK8,  // 41
	BATTLE_ST_ICECRACK9,  // 42
	BATTLE_ST_ICECRACK10,  // 43
#endif

#endif

	BATTLE_ST_END
};

#ifdef _OTHER_MAGICSTAUTS
enum{
	BATTLE_MST_NONE,
	BATTLE_MST_DEFMAGIC,
	BATTLE_MST_SUPERWALL,
#ifdef _MAGICSTAUTS_RESIST
	BATTLE_MST_MAGICFIRE, 
	BATTLE_MST_MAGICTHUNDER, 
	BATTLE_MST_MAGICICE,
#endif
	BATTLE_MST_END
};
#endif

enum{
	BATTLE_MD_NONE,
	BATTLE_MD_ABSROB,
	BATTLE_MD_REFLEC,
	BATTLE_MD_VANISH,
	BATTLE_MD_TRAP,
#ifdef _PETSKILL_ACUPUNCTURE
    BATTLE_MD_ACUPUNCTURE, //针刺外皮
#endif
	BATTLE_MD_END 
};

#ifdef _PETSKILL_RETRACE
    //存放BATTLE_Attack函式执行後的攻击模式   
typedef struct tagBattle_Attack_ReturnData_x1
{
    int Battle_Attack_ReturnData;
}Battle_Attack_ReturnData_x1;
Battle_Attack_ReturnData_x1 Battle_Attack_ReturnData_x;

#endif

int BATTLE_DamageCalc( int attackindex, int defindex );

BOOL BATTLE_Attack( int battleindex, int attackNo, int defNo );

BOOL BATTLE_Counter( int battleindex, int attackNo, int defNo );

BOOL BATTLE_Capture( int battleindex, int attackNo, int defNo );

void BATTLE_Guard( int battleindex, int attackNo );

// Nuke 20040112 fix for performance
#define BATTLE_BroadCast(A,B,C) 0
#define BATTLE_BroadCastOn(A,B,C) 0
//int  BATTLE_BroadCast( int battleindex, char *pszBuffer, int color );
//int  BATTLE_BroadCastOn( int battleindex, char *pszBuffer, int color );

BOOL BATTLE_Escape( int battleindex, int attackNo, int flag);

int BATTLE_NoAction( int battleindex, int attackNo );

int BATTLE_PetIn( int battleindex, int attackNo );

int BATTLE_PetOut( int battleindex, int attackNo );

int BATTLE_SurpriseCheck( int battleindex );

int BATTLE_Magic( int battleindex, int attackNo );

int BATTLE_S_GBreak( int battleindex, int attackNo, int defNo );

#ifdef _SKILL_GUARDBREAK2//破除防御2 vincent add 2002/05/20
int BATTLE_S_GBreak2( int battleindex, int attackNo, int defNo );
#endif

#ifdef _SKILL_SACRIFICE
int BATTLE_S_Sacrifice( int battleindex, int attackNo, int defNo );
#endif

#ifdef _SKILL_REFRESH
int BATTLE_S_Refresh( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_WEAKEN  //vincent宠技:虚弱
int BATTLE_S_Weaken( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_DEEPPOISON  //vincent宠技:剧毒 
int BATTLE_S_Deeppoison( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_BARRIER  //vincent宠技:魔障
int BATTLE_S_Barrier( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_NOCAST  //vincent宠技:沉默
int BATTLE_S_Nocast( int battleindex, int attackNo, int defNo, int marray );
#endif

#ifdef _SKILL_ROAR //vincent宠技:大吼
int BATTLE_S_Roar( int battleindex, int attackNo, int defNo, int marray );
#endif

int BATTLE_Charge( int battleindex, int attackNo );


int BATTLE_StatusAttackCheck(

	int attackindex,
	int defindex,
	int status,
	//BATTLE_ST_POISON,	    1 "毒",
	//BATTLE_ST_PARALYSIS,  2 "麻",
	//BATTLE_ST_SLEEP,	    3 "眠",
	//BATTLE_ST_STONE,	    4 "石",
	//BATTLE_ST_DRUNK,	    5 "醉",
	//BATTLE_ST_CONFUSION,  6 "乱"
	int PerOffset,
	int Range,
	float Bai,
	int *pPer
);

int BATTLE_Combo( int battleindex, int *pAttackList, int defNo );

int BATTLE_EarthRoundHide( int battleindex, int attackNo );

int BATTLE_GetDamageReact( int charaindex );

void  BATTLE_talkToCli( int charaindex, char *pszBuffer, int color );

#ifdef _PETSKILL_SETDUCK
BOOL BATTLE_CheckMySkillDuck( int charaindex );
#endif

BOOL BATTLE_LostEscape( int battleindex, int attackNo );

BOOL BATTLE_Abduct( int battleindex, int attackNo, int defNo, int array );

void BATTLE_Steal( int battleindex, int attackNo, int defNo );

int BATTLE_getReactFlg( int index, int react);

#ifdef _BATTLESTEAL_FIX
void BATTLE_StealMoney( int battleindex, int attackNo, int defNo) ;
#endif

#ifdef _PETSKILL_LER
void BATTLE_BatFly(int battleindex,int attackNo,int myside);
void BATTLE_DivideAttack(int battleindex,int attackNo,int myside);
void BATTLE_LerChange(int battleindex,int charaindex,int no);
#endif

#ifdef _PETSKILL_BATTLE_MODEL
void BATTLE_BattleModel(int battleindex,int attackNo,int myside);
#endif

// Robin 0727 Ride Pet
#define	ATTACKSIDE	1
#define DEFFENCESIDE	2

float BATTLE_adjustRidePet3A( int charaindex, int petindex, int workindex, int action );

int BATTLE_getRidePet( int charaindex );

#ifdef _PSKILL_FALLGROUND	//落马术
int BATTLE_S_FallGround( int battleindex, int attackNo, int defNo, int skill_type );
#endif
#ifdef _PETSKILL_EXPLODE
int BATTLE_S_Explode( int battleindex, int attackNo, int defNo, int skill_type );
#endif

#ifdef _PETSKILL_PROPERTY
int BATTLE_S_PetSkillProperty( int battleindex, int attackNo, int skill_type, int skill);
#endif

int BATTLE_S_AttackDamage( int battleindex, int attackNo, int defNo, int skill_type, int skill);

#ifdef _MAGIC_SUPERWALL
int	PETSKILL_MagicStatusChange_Battle( int battleindex, int attackNo, int toNo, int marray);
#endif

#ifdef _PETSKILL_SETDUCK
int	PETSKILL_SetDuckChange_Battle( int battleindex, int attackNo, int toNo, int marray);
#endif

#ifdef _MAGICPET_SKILL
int PETSKILL_SetMagicPet_Battle( int battleindex, int attackNo, int toNo, int marray);
#endif

#ifdef _TAKE_ITEMDAMAGE
int BATTLE_ItemCrushCheck( int charaindex , int flg);
int BATTLE_ItemCrush( int charaindex, int ItemEquip, int Damages, int flg);
#endif

#ifdef _PRO_BATTLEENEMYSKILL
int BATTLE_E_ENEMYREFILE( int battleindex, int attackNo, int defNo, int skill_type );

int BATTLE_E_ENEMYREHP( int battleindex, int attackNo, int defNo, int skill_type );

int BATTLE_E_ENEMYHELP( int battleindex, int attackNo, int defNo, int skill_type );
#endif


#ifdef _PREVENT_TEAMATTACK
int BATTLE_CheckSameSide( int charaindex, int toNo);
#endif

#ifdef _USER_CHARLOOPS
int CHAR_BattleStayLoop( int charaindex);//原地遇敌
#endif

#ifdef _PETSKILL_PROPERTY
int PET_PetskillPropertyEvent( int Myindex, int defindex, int *damage, int *T_Pow, int size);
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
int battle_profession_attack_fun(int battleindex, int attackNo, int defNo, int charaindex);
int battle_profession_attack_magic_fun(int battleindex, int attackNo, int defNo, int charaindex);
int battle_profession_assist_fun(int battleindex, int attackNo, int defNo, int charaindex);
int battle_profession_status_chang_fun(int battleindex, int attackNo, int defNo, int charaindex);
int BATTLE_check_profession_duck( int charaindex, int per );
int BATTLE_PROFESSION_ATK_PET_DamageSub( int attackindex, int defindex, int *pDamage, int *pPetDamage, int *pRefrect, int skill_level );
int PROFESSION_BATTLE_StatusAttackCheck( int charaindex, int toindex, int status, int Success );
int BATTLE_PROFESSION_CONVOLUTE_GET_DAMAGE( int attackindex, int defindex, int skill_level ); 
int BATTLE_PROFESSION_THROUGH_ATTACK_GET_DAMAGE( int attackindex, int defindex );
int BATTLE_PROFESSION_RANG_ATTACK_DAMAGE( int charaindex, int attackNo, int defNo, int skill_type, int status, int turn, int perStatus, int effect, int img1, int img2 );

#endif

#ifdef _PETSKILL_FIREKILL //火线猎杀专用
int BATTLE_Attack_FIREKILL( int battleindex, int attackNo, int defNo );
#endif
#ifdef _PROFESSION_ADDSKILL
BOOL BATTLE_BattleUltimate( int battleindex, int bid ); //检查此位  上是否被打飞
#endif

#endif
