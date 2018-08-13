#include "version.h"
#include<string.h>
#include"char.h"
#include"char_base.h"
#include"battle.h"
#include"battle_event.h"
#include"magic_base.h"
#include"battle_magic.h"
#include"item_event.h"
#include"anim_tbl.h"
#include"common.h"
#include"lssproto_serv.h"

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#include "skill.h"
#endif

#ifdef __ATTACK_MAGIC

extern AttMagic *ATTMAGIC_magic;
extern int       ATTMAGIC_magicnum;

#define AJ_SAME	(1.0)
#define AJ_UP	(1.5)
#define AJ_DOWN	(0.6)

#define ATTR_MAX 100
#define D_ATTR (1.0/(ATTR_MAX*ATTR_MAX))
#ifdef _FIX_MAGICDAMAGE
static int BATTLE_getMagicAdjustInt( int attackindex, int defindex, int MagicLv, int flg,int damage );
void Magic_ComputeAttExp( int charindex, int Mnum, int MagicLv, int Expoint);
void Magic_ComputeDefExp( int charindex, int Mnum, int MagicLv, int Damage);
#endif

#ifdef _MAGIC_TOCALL
/*
typedef struct tagToCallMagic
{
  unsigned int  uiSpriteNum;			// 此咒术在Spr_x.bin的编号
  unsigned int  uiAttackType;			// 攻击的方式：单人，整排( 轮流 ) ,  整排( 轮流 ) , 整排( 同时 ) , 全体( 轮流 ) , 全体( 同时 )
  unsigned int  uiSliceTime;			// 轮流攻击时的时间差
  unsigned int  uiShowType;             // 显示的位置方式：中央、指定
  int           siSx;                   // 显示的位置 - X轴
  int           siSy;                   // 显示的位置 - Y轴
  unsigned int  uiShowBehindChar;       // 显示在人物的前方或下方
  unsigned int  uiShakeScreen;          // 是否震动画面
  unsigned int  uiShakeFrom;            // 震动画面的起始时间( 毫秒 )
  unsigned int  uiShakeTo;              // 震动画面的结束时间( 毫秒 _
  unsigned int  uiPrevMagicNum;         // 前置咒术的索引号( 0XFFFFFFFFFF 表示无前置咒术 )
  int           siPrevMagicSx;          // 前置咒术的显示位置 - X轴
  int           siPrevMagicSy;          // 前置咒术的显示位置 - Y轴
  int           siPrevMagicOnChar;      // 前置咒术显示在人物的前方或下方
  unsigned int  uiPostMagicNum;         // 後置咒术的索引号( 0XFFFFFFFF 表示无後置咒术 )
  int           siPostMagicSx;          // 後置咒术的显示位置 - X轴
  int           siPostMagicSy;          // 後置咒术的显示位置 - Y轴
  int           siPostMagicOnChar;      // 後置咒术显示在人物的前方或下方
  int			isPostDisappear;		// 咒术一般攻击完时是否马上消失
  int			ToCallMagicNo;			// 召唤术的编号
}ToCallMagic;
*/
  
ToCallMagic TOCALL_magic[3] = { {100354,0,0,0,50,25,1,0,0,0,101120,65528,65485,0,-1,0,0,0,1,0},
								{100354,1,0,0,50,25,1,0,0,0,101120,65528,65485,0,-1,0,0,0,1,1},
								{100354,5,0,0,0,0,1,1,1000,4000,101120,65528,65485,0,-1,0,0,0,0,2},
								};

#endif

// 魔法熟练度及抗性升级所需经验值
int Magic_Level_Table[101] =
{
	   1,   3,   6,  10,  15,  21,  28,  36,  45,  55,	//lv  1~10
		66,  78,	91, 105, 120, 136, 153, 171, 190, 210,  //lv 11~20
	 231, 253, 276, 300, 325, 351, 378, 406, 435, 465,  //lv 21~30
	 496, 528, 561, 595, 630, 666, 703, 741, 780, 820,  //lv 31~40
	 861, 903, 946, 990,1035,1081,1128,1176,1225,1275,  //lv 41~50
	1326,1378,1431,1485,1540,1596,1653,1711,1770,1830,  //lv 51~60
	1891,1953,2016,2080,2145,2211,2278,2346,2415,2485,  //lv 61~70
	2556,2628,2701,2775,2850,2926,3003,3081,3160,3240,  //lv 71~80
	3321,3403,3486,3570,3655,3741,3828,3916,4005,4095,  //lv 81~90
	4186,4278,4371,4465,4560,4656,4753,4851,4950,5050,  //lv 91~100
	9999
};
#endif

#if 0

int	BATTLE_PointChange(
	int ToList[], 	//
	int kind,
	int point
)

{

	return 0;
}
#endif

int BATTLE_MagicEffect( int battleindex, int attackNo, int ToList[], int MyEffect, int ToEffect )
{
	int i;
	char szCommand[256];
	int attackindex;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )return 0;

	snprintf( szCommand, sizeof(szCommand),
		"BJ|a%X|m%X|e%X|e%X|",
		attackNo,
		CHAR_getInt( attackindex, CHAR_MP ),
		MyEffect,
		ToEffect
	);

	
	BATTLESTR_ADD( szCommand );

	for( i = 0; ToList[i] != -1; i ++ ){
		snprintf( szCommand, sizeof(szCommand), "r%X|",ToList[i]);
		BATTLESTR_ADD( szCommand );
	}
	BATTLESTR_ADD( "FF|" );

	return 0;
}

#ifdef __ATTACK_MAGIC

int BATTLE_AttMagicEffect( int battleindex , int attackNo , int ToList[] , int AttackMgcNo )
{
   int     i;
   char    szcommand[256];
   int     attackindex;

   attackindex = BATTLE_No2Index( battleindex , attackNo );
   if( FALSE == CHAR_CHECKINDEX( attackindex ) )
       return 0;

   if( attackNo >= 10 )
      i = AttackMgcNo * 2;
   else
      i = AttackMgcNo * 2 + 1;

   snprintf(
   szcommand , sizeof( szcommand ) , "BJ|a%X|i%X|m%X|%X|%X|%X|s%X|t%X|l%X|%X|%X|%X|%X|%X|%X|o%X|o%X|o%X|s%X|%X|%X|" ,
   attackNo , 12345678 , CHAR_getInt( attackindex , CHAR_MP ) ,
   ATTMAGIC_magic[i].uiPrevMagicNum ,
   ATTMAGIC_magic[i].uiSpriteNum ,
   ATTMAGIC_magic[i].uiPostMagicNum ,
   ATTMAGIC_magic[i].uiAttackType ,
   ATTMAGIC_magic[i].uiSliceTime ,
   ATTMAGIC_magic[i].uiShowType ,
   ATTMAGIC_magic[i].siSx ,
   ATTMAGIC_magic[i].siSy ,
   ATTMAGIC_magic[i].siPrevMagicSx ,
   ATTMAGIC_magic[i].siPrevMagicSy ,
   ATTMAGIC_magic[i].siPostMagicSx ,
   ATTMAGIC_magic[i].siPostMagicSy ,
   ATTMAGIC_magic[i].siPrevMagicOnChar ,
   ATTMAGIC_magic[i].uiShowBehindChar ,
   ATTMAGIC_magic[i].siPostMagicOnChar ,
   ATTMAGIC_magic[i].uiShakeScreen ,
   ATTMAGIC_magic[i].uiShakeFrom ,
   ATTMAGIC_magic[i].uiShakeTo
   );

   BATTLESTR_ADD( szcommand );

   for( i = 0 ; ToList[i] != -1 ; i++ )
   {
      snprintf( szcommand , sizeof( szcommand ) , "r%X|" , ToList[i] );
      BATTLESTR_ADD( szcommand );
   }

   BATTLESTR_ADD( "FF|" );

   return 0;
}

#endif


#ifdef _MAGIC_TOCALL

int BATTLE_ToCallDragonEffect( int battleindex , int attackNo , int ToList[] , int AttackMgcNo )
{
   int     i;
   char    szcommand[256];
   int     attackindex;

   attackindex = BATTLE_No2Index( battleindex , attackNo );
   if( FALSE == CHAR_CHECKINDEX( attackindex ) )
       return 0;
/*
   if( attackNo >= 10 )
      i = AttackMgcNo * 2;
   else
      i = AttackMgcNo * 2 + 1;
*/
   snprintf(
   szcommand , sizeof( szcommand ) , "B$|a%X|i%X|m%X|%X|%X|%X|s%X|t%X|l%X|%X|%X|%X|%X|%X|%X|o%X|o%X|o%X|s%X|%X|%X|%X|%X|" ,
   attackNo , 5711438 , CHAR_getInt( attackindex , CHAR_MP ) ,
	TOCALL_magic[AttackMgcNo].uiPrevMagicNum ,
	TOCALL_magic[AttackMgcNo].uiSpriteNum ,
	TOCALL_magic[AttackMgcNo].uiPostMagicNum ,
	TOCALL_magic[AttackMgcNo].uiAttackType ,
	TOCALL_magic[AttackMgcNo].uiSliceTime ,
	TOCALL_magic[AttackMgcNo].uiShowType ,
	TOCALL_magic[AttackMgcNo].siSx ,
	TOCALL_magic[AttackMgcNo].siSy ,
	TOCALL_magic[AttackMgcNo].siPrevMagicSx ,
	TOCALL_magic[AttackMgcNo].siPrevMagicSy ,
	TOCALL_magic[AttackMgcNo].siPostMagicSx ,
	TOCALL_magic[AttackMgcNo].siPostMagicSy ,
	TOCALL_magic[AttackMgcNo].siPrevMagicOnChar ,
	TOCALL_magic[AttackMgcNo].uiShowBehindChar ,
	TOCALL_magic[AttackMgcNo].siPostMagicOnChar ,
	TOCALL_magic[AttackMgcNo].uiShakeScreen ,
	TOCALL_magic[AttackMgcNo].uiShakeFrom ,
	TOCALL_magic[AttackMgcNo].uiShakeTo ,
	TOCALL_magic[AttackMgcNo].isPostDisappear ,
	TOCALL_magic[AttackMgcNo].ToCallMagicNo
   );

   BATTLESTR_ADD( szcommand );

   for( i = 0 ; ToList[i] != -1 ; i++ )
   {
      snprintf( szcommand , sizeof( szcommand ) , "r%X|" , ToList[i] );
      BATTLESTR_ADD( szcommand );
   }

   BATTLESTR_ADD( "FF|" );

   return 0;
}

#endif
#ifdef _IMPRECATE_ITEM
void BATTLE_ImprecateRecovery(
	int battleindex, int attackNo, int toNo, int kind, int powers,
	int rounds, int UseEffect, int RecevEffect )
{
	int i, toindex, flg;
	int ToList[SIDE_OFFSET*2+1];
	//char szBuffer[256]="";
	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, attackNo, ToList,
		UseEffect, RecevEffect );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		if( CHAR_CHECKINDEX( toindex ) == FALSE )continue;
		switch( kind ){
#ifdef _IMPRECATE_ITEM
		case BD_KIND_CURSE:
			if( CHAR_getWorkInt( toindex, CHAR_WORKIMPRECATENUM1 ) <= 0 && 
				CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
				CHAR_setWorkInt( toindex, CHAR_WORKHURTMP, powers);	//伤害 MP
				CHAR_setWorkInt( toindex, CHAR_WORKIMPRECATENUM1, rounds);
			}
			break;
		case BD_KIND_BESTOW:
			if( CHAR_getWorkInt( toindex, CHAR_WORKIMPRECATENUM2 ) <= 0 ){
#ifdef _TYPE_TOXICATION
				if( CHAR_CanCureFlg( toindex, "HP") == FALSE ) break;
#endif
				CHAR_setWorkInt( toindex, CHAR_WORKWISHESHP, powers);	//祝福 hp
				CHAR_setWorkInt( toindex, CHAR_WORKIMPRECATENUM2, rounds);
			}
			break;
		case BD_KIND_WISHES:
			if( CHAR_getWorkInt( toindex, CHAR_WORKIMPRECATENUM3 ) <= 0 &&
				CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
				CHAR_setWorkInt( toindex, CHAR_WORKWISHESMP, powers);	//祝福 MP
				CHAR_setWorkInt( toindex, CHAR_WORKIMPRECATENUM3, rounds);
			}
			break;
#endif
		}
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		if( BattleArray[battleindex].norisk == 0
		&&  CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET){
			flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
			if(  flg & CHAR_BATTLEFLG_RECOVERY ){
			}else{
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRECOVERY );
				CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg | CHAR_BATTLEFLG_RECOVERY );
			}
		}
	}
	return;
}
#endif

void BATTLE_MultiRecovery(
	int battleindex,
	int attackNo,
	int toNo,
	int kind,
	int power,
	int per,
	int UseEffect,
#ifdef _CHANGEITEMUSE	 // Syu ADD 调整战斗中使用料理设定
	int RecevEffect,
	int power1
#else
	int RecevEffect
#endif
)
{
	int i, toindex, UpPoint = 0, workhp, oldhp, flg;
#ifdef _CHANGEITEMUSE	 // Syu ADD 调整战斗中使用料理设定
	int UpPoint1 = 0;
#endif
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];
	
	int ridepet, petUpPoint =0;


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect( battleindex, attackNo, ToList, UseEffect, RecevEffect );

	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		if( CHAR_CHECKINDEX( toindex ) == FALSE )	continue;

		ridepet = BATTLE_getRidePet( toindex );

		switch( kind ){
#ifdef _ITEM_UNBECOMEPIG
		case BD_KIND_UNBECOMEPIG:
            if( CHAR_getInt( toindex, CHAR_BECOMEPIG ) > -1 ){
		        CHAR_setInt( toindex, CHAR_BECOMEPIG, -1 );
                CHAR_setInt( toindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( toindex, CHAR_BASEBASEIMAGENUMBER));
				CHAR_complianceParameter( toindex );
		        CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		        CHAR_send_P_StatusString( toindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		        CHAR_talkToCli( toindex,-1,"乌力化失效了。",CHAR_COLORWHITE);
			}
		break;
#endif 
#ifdef _ITEM_PROPERTY
		case BD_KIND_PROPERTY:
		{
			if (CHAR_getInt(toindex, CHAR_WHICHTYPE)!=CHAR_TYPEPLAYER)
            return;
		//print("地水火风:%d %d %d %d", CHAR_getInt( toindex, CHAR_EARTHAT ), CHAR_getInt( toindex, CHAR_WATERAT ), CHAR_getInt( toindex, CHAR_FIREAT ), CHAR_getInt( toindex, CHAR_WINDAT ) );
		if( power == 1 ){//旋转属性 正转
	        if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
			else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
		}
        if( power == 2 ){//旋转属性 反转
			if( CHAR_getInt( toindex, CHAR_EARTHAT ) == 100 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_WATERAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_FIREAT ) == 100 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) == 100 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 );
		    else if( CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 && CHAR_getInt( toindex, CHAR_WATERAT ) > 0 )
                CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WATERAT ) > 0 && CHAR_getInt( toindex, CHAR_FIREAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WATERAT, CHAR_getInt( toindex, CHAR_WATERAT )+10 ),
			    CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )-10 );
		    else if( CHAR_getInt( toindex, CHAR_FIREAT ) > 0 && CHAR_getInt( toindex, CHAR_WINDAT ) > 0 )
                CHAR_setInt( toindex, CHAR_FIREAT, CHAR_getInt( toindex, CHAR_FIREAT )+10 ),
			    CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )-10 );
            else if( CHAR_getInt( toindex, CHAR_WINDAT ) > 0 && CHAR_getInt( toindex, CHAR_EARTHAT ) > 0 )
                CHAR_setInt( toindex, CHAR_WINDAT, CHAR_getInt( toindex, CHAR_WINDAT )+10 ),
			    CHAR_setInt( toindex, CHAR_EARTHAT, CHAR_getInt( toindex, CHAR_EARTHAT )-10 );
		}
		}
#endif
		case BD_KIND_HP:
#ifdef _TYPE_TOXICATION
			if( CHAR_CanCureFlg( toindex, "HP") == FALSE ) continue;
#endif
		    // Robin 0728 ride Pet
		    if( ridepet == -1 ) {
				UpPoint = RAND( (power*0.9), (power*1.1) );
#ifdef _MAGIC_REHPAI
#else
				if( per ){
					UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
				}
				UpPoint *= GetRecoveryRate( toindex );
#endif
				workhp = CHAR_getInt( toindex, CHAR_HP );
				oldhp = workhp; 
				workhp += (int)UpPoint;
				CHAR_setInt( toindex, CHAR_HP,
				min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
				
				//snprintf( szBuffer, sizeof(szBuffer), "(%s)的耐久力回复(%d)", CHAR_getUseName( toindex ), UpPoint );

		    }else {
				int allUpPoint;
				UpPoint = RAND( (power*0.9), (power*1.1) );
#ifdef _MAGIC_REHPAI
				allUpPoint = UpPoint;
				UpPoint = (allUpPoint * Magic_RideGetHP( toindex, ridepet, 1 ))/100;
				petUpPoint = allUpPoint - UpPoint;
#else		
				petUpPoint = UpPoint;
				if( per ){
					UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
					petUpPoint *= CHAR_getWorkInt( ridepet, CHAR_WORKMAXHP ) * 0.01;
					
				}
				UpPoint *= GetRecoveryRate( toindex );
				petUpPoint *= GetRecoveryRate( ridepet );
#endif


				allUpPoint = UpPoint + petUpPoint;
#ifdef _MAGIC_REHPAI
#else
				UpPoint = (UpPoint * UpPoint) / allUpPoint;
				petUpPoint = (petUpPoint * petUpPoint) / allUpPoint;
#endif
				workhp = CHAR_getInt( toindex, CHAR_HP );
				oldhp = workhp;
				workhp += (int)UpPoint;
				CHAR_setInt( toindex, CHAR_HP,
					min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
				workhp = CHAR_getInt( ridepet, CHAR_HP );
				oldhp = workhp;
				workhp += (int)petUpPoint;
				CHAR_setInt( ridepet, CHAR_HP,
				min( workhp, CHAR_getWorkInt( ridepet, CHAR_WORKMAXHP ) ) );			
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)的耐久力回复(%d),(%s的骑宠)的耐久力回复(%d)",
				//		CHAR_getUseName( toindex ),
				//		UpPoint,
				//		CHAR_getUseName( toindex ),
				//		petUpPoint
				//	);
		    }
		    break;
		case BD_KIND_MP:
			UpPoint = RAND( (power*0.9), (power*1.1) );
			if( per ){
				UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXMP ) * 0.01;
			}
			workhp = CHAR_getInt( toindex, CHAR_MP );
			oldhp = workhp;
			workhp += (int)UpPoint;
			CHAR_setInt( toindex, CHAR_MP,
			min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXMP ) ) );
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)的气力回复(%d)",
			//		CHAR_getUseName( toindex ),
			//		UpPoint
			//	);
			break;
#ifdef _CHANGEITEMUSE	 // Syu ADD 调整战斗中使用料理设定
		case BD_KIND_HP_MP:
#ifdef _TYPE_TOXICATION
			if( CHAR_CanCureFlg( toindex, "HP") == FALSE ) 
				continue;
#endif
		    if( ridepet == -1 ) 
			{
				UpPoint = RAND( (power*0.9), (power*1.1) );
#ifdef _MAGIC_REHPAI
#else
				if( per )
				{
					UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
				}
				UpPoint *= GetRecoveryRate( toindex );
#endif
				workhp = CHAR_getInt( toindex, CHAR_HP );
				oldhp = workhp; 
				workhp += (int)UpPoint;
				CHAR_setInt( toindex, CHAR_HP,
				min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)的耐久力回复(%d)",
				//		CHAR_getUseName( toindex ),
				//		UpPoint
				//	);
		    }
			else 
			{
				int allUpPoint;
				UpPoint = RAND( (power*0.9), (power*1.1) );
#ifdef _MAGIC_REHPAI
				allUpPoint = UpPoint;
				UpPoint = (allUpPoint * Magic_RideGetHP( toindex, ridepet, 1 ))/100;
				petUpPoint = allUpPoint - UpPoint;
#else		
				petUpPoint = UpPoint;
				if( per )
				{
					UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
					petUpPoint *= CHAR_getWorkInt( ridepet, CHAR_WORKMAXHP ) * 0.01;
					
				}
				UpPoint *= GetRecoveryRate( toindex );
				petUpPoint *= GetRecoveryRate( ridepet );
#endif
				allUpPoint = UpPoint + petUpPoint;
#ifdef _MAGIC_REHPAI
#else
				UpPoint = (UpPoint * UpPoint) / allUpPoint;
				petUpPoint = (petUpPoint * petUpPoint) / allUpPoint;
#endif
				workhp = CHAR_getInt( toindex, CHAR_HP );
				oldhp = workhp;
				workhp += (int)UpPoint;
				CHAR_setInt( toindex, CHAR_HP,
					min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
				workhp = CHAR_getInt( ridepet, CHAR_HP );
				oldhp = workhp;
				workhp += (int)petUpPoint;
				CHAR_setInt( ridepet, CHAR_HP,
				min( workhp, CHAR_getWorkInt( ridepet, CHAR_WORKMAXHP ) ) );			
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)的耐久力回复(%d),(%s的骑宠)的耐久力回复(%d)",
				//		CHAR_getUseName( toindex ),
				//		UpPoint,
				//		CHAR_getUseName( toindex ),
				//		petUpPoint
				//	);
		    }
			UpPoint1 = RAND( (power1*0.9), (power1*1.1) );
			if( per )
			{
				UpPoint1 *= CHAR_getWorkInt( toindex, CHAR_WORKMAXMP ) * 0.01;
			}
			workhp = CHAR_getInt( toindex, CHAR_MP );
			oldhp = workhp;
			workhp += (int)UpPoint1;
			CHAR_setInt( toindex, CHAR_MP,
			min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXMP ) ) );
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)的气力回复(%d)",
			//		CHAR_getUseName( toindex ),
			//		UpPoint1);
			break;
#endif
		}

#ifdef _CHANGEITEMUSE	 // Syu ADD 调整战斗中使用料理设定
		if ( kind == BD_KIND_HP_MP)
		{
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
			{
				//补体的部分
				int Sign, Damage, petDamage;
				Sign = (UpPoint >= 0)?(1):(0);
				Damage = ABS( UpPoint );
				petDamage = ABS( petUpPoint );
#ifdef _NOTRIDE_
				snprintf( szCommand, sizeof(szCommand),
					"BD|r%X|%X|%X|d%X|", ToList[i], BD_KIND_HP, Sign, Damage );
#else
				snprintf( szCommand, sizeof(szCommand),
					"BD|r%X|%X|%X|d%X|p%X|", ToList[i], BD_KIND_HP, Sign, Damage, petDamage );
#endif //_NOTRIDE_
				BATTLESTR_ADD( szCommand );
			}

			{
				//补气的部分
				int Sign, Damage, petDamage;
				Sign = (UpPoint1 >= 0)?(1):(0);
				Damage = ABS( UpPoint1 );
				petDamage = ABS( petUpPoint );
#ifdef _NOTRIDE_
				snprintf( szCommand, sizeof(szCommand),
					"BD|r%X|%X|%X|d%X|", ToList[i], BD_KIND_MP, Sign, Damage );
#else
				snprintf( szCommand, sizeof(szCommand),
					"BD|r%X|%X|%X|d%X|p%X|", ToList[i], BD_KIND_MP, Sign, Damage, petDamage );
#endif //_NOTRIDE_
				BATTLESTR_ADD( szCommand );
			}

		}
		else
		{
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
			{
				int Sign, Damage, petDamage;
				Sign = (UpPoint >= 0)?(1):(0);
				Damage = ABS( UpPoint );
				petDamage = ABS( petUpPoint );
#ifdef _NOTRIDE_
				snprintf( szCommand, sizeof(szCommand),
					"BD|r%X|%X|%X|d%X|", ToList[i], kind, Sign, Damage );
#else
				snprintf( szCommand, sizeof(szCommand),
					"BD|r%X|%X|%X|d%X|p%X|", ToList[i], kind, Sign, Damage, petDamage );
#endif //_NOTRIDE_
				BATTLESTR_ADD( szCommand );
			}
		}
		if( BattleArray[battleindex].norisk == 0
			&&  CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET)
		{
			flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
			if(  flg & CHAR_BATTLEFLG_RECOVERY )
			{}
			else
			{
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRECOVERY );
				CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg | CHAR_BATTLEFLG_RECOVERY );
			}
		}
#else //_CHANGEITEMUSE
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		{
			int Sign, Damage, petDamage;
			Sign = (UpPoint >= 0)?(1):(0);
			Damage = ABS( UpPoint );
			petDamage = ABS( petUpPoint );
#ifdef _NOTRIDE_
			snprintf( szCommand, sizeof(szCommand),
				"BD|r%X|%X|%X|d%X|", ToList[i], kind, Sign, Damage );
#else
			snprintf( szCommand, sizeof(szCommand),
				"BD|r%X|%X|%X|d%X|p%X|", ToList[i], kind, Sign, Damage, petDamage );
#endif //_NOTRIDE_
			BATTLESTR_ADD( szCommand );
		}

		if( BattleArray[battleindex].norisk == 0
		&&  CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET){
			flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
			if(  flg & CHAR_BATTLEFLG_RECOVERY ){
			}else{
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRECOVERY );
				CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg | CHAR_BATTLEFLG_RECOVERY );
			}
		}
#endif //_CHANGEITEMUSE
	}
	return;
}

#ifdef _MAGIC_REHPAI
int Magic_RideGetHP( int toindex, int petindex, int flg)
{
	int allDHp=1;
	int petDHp=1;
	int CharDHp=1;
	int ISIZE = 100;

	CharDHp	= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) - CHAR_getInt( toindex, CHAR_HP);
	petDHp	= CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) - CHAR_getInt( petindex, CHAR_HP);
	allDHp = (CharDHp + petDHp);

	if( allDHp <= 0 ) allDHp=1;
	if( flg == 1)
		return (int)((CharDHp*ISIZE)/allDHp);
	else
		return (int)((petDHp*ISIZE)/allDHp);
}
#endif

int	MAGIC_Recovery_Battle( int charaindex, int toNo, int marray, int mp )
{
	char *magicarg;
	float power;
	int battleindex, attackNo, 	HealedEffect = 0, per = 0;
	int range;
	if( !CHAR_CHECKINDEX( charaindex) )	return FALSE;
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

#if 1	// Robin fix 防封包改Magic  围
	range = MAGIC_getInt( marray, MAGIC_TARGET );
	//print(" MAGIC_TARGET:%d toNo:%d ", range, toNo);
	if( range == 0 && toNo != attackNo ) { // 只能对自己使用
		print("\n 改封包!MAGIC只能对自己使用!!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
		return FALSE;
	}
	if( range == 1 && toNo >= 20 ) { // 只能对单体使用
		print("\n 改封包!MAGIC只能对单体使用!!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
		return FALSE;
	}
#endif

	// shan(对全体使用魔法的bug)，修改者jinchao+2001/12/07
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return FALSE;
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	// shan(对全体使用魔法的bug)，修改者jinchao+2001/12/07
	if (!magicarg) return FALSE;

	power =	atoi( magicarg );
	if( strstr( magicarg, "%" ) ){
		per = 1;
	}
	
	if( power <= 100 ){
		HealedEffect = SPR_heal;
	}else
	if( power <= 300 ){
		HealedEffect = SPR_heal2;
	}else{
		HealedEffect = SPR_heal3;
	}

#ifdef _CHANGEITEMUSE	 // Syu ADD 调整战斗中使用料理设定
	BATTLE_MultiRecovery( battleindex, attackNo, toNo,
		BD_KIND_HP, (int)power,  per, MAGIC_EFFECT_USER, HealedEffect , 0);
#else
	BATTLE_MultiRecovery( battleindex, attackNo, toNo,
		BD_KIND_HP, (int)power,  per, MAGIC_EFFECT_USER, HealedEffect );
#endif
	return TRUE;
}

void BATTLE_MultiResAndDef( 
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int power,			// 湘  荚汊袄
	int per,			// ⊙井＂
	int kind, 			//   芊  豢及潘  
	int count, 			// 窒荚  什井
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
{
	int i, toindex, charaindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];


	BATTLE_MultiListDead( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);
	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P
		&& CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			continue;
		}
		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == FALSE ){
			continue;
		}
		UpPoint = power;
#ifdef _MAGIC_REHPAI
#else
		if( per ){
			UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
		}
#endif
		if( power == 0 ){
			UpPoint = CHAR_getWorkInt( toindex, CHAR_WORKMAXHP );
		}else{
			UpPoint = RAND( (power*0.9), (power*1.1) );
		}
		UpPoint = max( 1, UpPoint );
		workhp = CHAR_getInt( toindex, CHAR_HP ) + UpPoint;
		CHAR_setInt( toindex, CHAR_HP,
			min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
		CHAR_setFlg( toindex, CHAR_ISDIE, 0 );
		CHAR_setWorkInt( toindex, MagicDefTbl[kind], count );

		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)恢复意识",
		//		CHAR_getUseName( toindex ) );
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)将(%s)遭受(损伤%s)",
		//	CHAR_getUseName( charaindex ),
		//	CHAR_getUseName( toindex ),
		//	aszMagicDefFull[kind]
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		snprintf( szCommand, sizeof(szCommand),
				"BL|r%X|h%X|", ToList[i], CHAR_getInt( toindex, CHAR_HP ) );
		BATTLESTR_ADD( szCommand );
		if( BattleArray[battleindex].norisk == 0
		 &&	BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E	 ){
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRESSURECT );
			}
		}
	}
}

#ifdef __ATTACK_MAGIC

static int CharTable[4][5] =
{
   { 13 , 11 , 10 , 12 , 14 } ,
   { 18 , 16 , 15 , 17 , 19 } ,
   {  8 ,  6 ,  5 ,  7 ,  9 } ,
   {  3 ,  1 ,  0 ,  2 ,  4 }
};

static int CharTableIdx[20][2] =
{
   { 3 , 2 } , { 3 , 1 } , { 3 , 3 } , { 3 , 0 } , { 3 , 4 } ,
   { 2 , 2 } , { 2 , 1 } , { 2 , 3 } , { 2 , 0 } , { 2 , 4 } ,
   { 0 , 2 } , { 0 , 1 } , { 0 , 3 } , { 0 , 0 } , { 0 , 4 } ,
   { 1 , 2 } , { 1 , 1 } , { 1 , 3 } , { 1 , 0 } , { 1 , 4 }
};



typedef int ( *FUNC )( const void* , const void* );

static int SortLoc( const int *pEle1 , const int *pEle2 )
{
   int ele1basex , ele1basey;
   int ele2basex , ele2basey;

   ele1basex = CharTableIdx[*pEle1][1];
   ele1basey = CharTableIdx[*pEle1][0];
   ele2basex = CharTableIdx[*pEle2][1];
   ele2basey = CharTableIdx[*pEle2][0];

   // 左上方
   if( *pEle1 >= 10 )
   {
      if( ele1basey != ele2basey )
         return ( ele1basey - ele2basey );

      return ( ele1basex - ele2basex );
   }
   // 右下方
   else
   {
      if( ele1basey != ele2basey )
         return ( ele2basey - ele1basey );

      return ( ele2basex - ele1basey );
   }

   return 0;
}


static void BATTLE_GetAttr( int charaindex , int *pAt_Fire , int *pAt_Water , int *pAt_Earth , int *pAt_Wind , int *pAt_None )
{

   int petindex = BATTLE_getRidePet( charaindex );

   if( petindex == -1 )
   {
        *pAt_Fire  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXFIREAT );
        *pAt_Water = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWATERAT );
        *pAt_Earth = CHAR_getWorkInt( charaindex , CHAR_WORKFIXEARTHAT );
        *pAt_Wind  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWINDAT );
   }
   else
   {
        *pAt_Fire  = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXFIREAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXFIREAT ) ) / 2;
        *pAt_Water = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXWATERAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXWATERAT ) ) / 2;
        *pAt_Earth = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXEARTHAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXEARTHAT ) ) / 2;
        *pAt_Wind  = ( CHAR_getWorkInt( charaindex , CHAR_WORKFIXWINDAT )
                   + CHAR_getWorkInt( petindex , CHAR_WORKFIXWINDAT ) ) / 2;
   }

   if( *pAt_Fire < 0 )  *pAt_Fire  = 0;
   if( *pAt_Water < 0 ) *pAt_Water = 0;
   if( *pAt_Earth < 0 ) *pAt_Earth = 0;
   if( *pAt_Wind < 0 )  *pAt_Wind  = 0;

   *pAt_None = 100 - *pAt_Fire - *pAt_Water - *pAt_Earth - *pAt_Wind;

   if( *pAt_None < 0 )
      *pAt_None = 0;
}

static void BATTLE_GetPureAttr( int charaindex , int *pAt_Fire , int *pAt_Water, int *pAt_Earth , int *pAt_Wind )
{
   *pAt_Fire  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXFIREAT );
   *pAt_Water = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWATERAT );
   *pAt_Earth = CHAR_getWorkInt( charaindex , CHAR_WORKFIXEARTHAT );
   *pAt_Wind  = CHAR_getWorkInt( charaindex , CHAR_WORKFIXWINDAT );

   if( *pAt_Fire < 0 )  *pAt_Fire  = 0;
   if( *pAt_Water < 0 ) *pAt_Water = 0;
   if( *pAt_Earth < 0 ) *pAt_Earth = 0;
   if( *pAt_Wind < 0 )  *pAt_Wind  = 0;
}



int BATTLE_CalAttMagicEffect( int AttEle , int DefPosEleValue , int DefNegEleValue )
{
    int value;

    value = ( 100 * DefPosEleValue - 100 * DefNegEleValue ) / 1000;

    if( value > 10 )
        return 10;
    else if( value < -10 )
        return -10;

    return value;
}



int BATTLE_CalcCharaRatio( int AttEle , int charaidx )
{
   int petidx , charahurt , pethurt;
   int charaattr[4] , petattr[4];

   // 没有宠物
   if( -1 == ( petidx = BATTLE_getRidePet( charaidx ) ) )
      return 10;

   // Nuke 20040330: Fix bug
   if ( -1 == AttEle )
       return 5;

   BATTLE_GetPureAttr( charaidx , &charaattr[2] , &charaattr[1] , &charaattr[0], &charaattr[3] );
   BATTLE_GetPureAttr( petidx , &petattr[2] , &petattr[1] , &petattr[0] , &petattr[3] );

   //print("\nAttEle: %d %d %d %d %d ",AttEle, charaattr[( AttEle + 1 ) % 4], charaattr[AttEle],petattr[( AttEle + 1 ) %4] , petattr[AttEle]);

   charahurt = 20 + BATTLE_CalAttMagicEffect( AttEle , charaattr[( AttEle + 1 ) % 4] , charaattr[AttEle] );
   pethurt   = 20 + BATTLE_CalAttMagicEffect( AttEle , petattr[( AttEle + 1 ) %4] , petattr[AttEle] );

   charahurt = 10 * charahurt / ( charahurt + pethurt );

   if( charahurt < 2 )
      return 2;
   else if( charahurt > 8 )
      return 8;

   return charahurt;
}

static int	BATTLE_AttrCalc(
		int My_Fire,	// 愤坌础(膜恳熬仃月幻丹)
		int My_Water,
		int My_Earth,
		int My_Wind,
		int My_None,
		int Vs_Fire,	// 锹澎础
		int Vs_Water,
		int Vs_Earth,
		int Vs_Wind,
		int Vs_None
)
{
	int iRet = 0;

	// 绍及  猾  宁煌允月［
	My_Fire = My_Fire * Vs_None * AJ_UP		// 绍     雄中
			+ My_Fire * Vs_Fire * AJ_SAME   // 绍  绍   元
			+ My_Fire * Vs_Water * AJ_DOWN  // 绍     浇中
			+ My_Fire * Vs_Earth * AJ_SAME  // 绍  萝   元
			+ My_Fire * Vs_Wind * AJ_UP;    // 绍  氘 雄中

	//   及  猾  宁煌允月［
	My_Water = My_Water * Vs_None * AJ_UP	//        雄中
			+ My_Water * Vs_Fire * AJ_UP    //     绍 雄中
			+ My_Water * Vs_Water * AJ_SAME  //          元
			+ My_Water * Vs_Earth * AJ_DOWN  //     萝 浇中
			+ My_Water * Vs_Wind * AJ_SAME;  //     氘   元

	// 萝
	My_Earth = My_Earth * Vs_None * AJ_UP	 // 萝     雄中
			+ My_Earth  * Vs_Fire * AJ_SAME  // 萝  绍   元
			+ My_Earth * Vs_Water * AJ_UP    // 萝     雄中
			+ My_Earth * Vs_Earth * AJ_SAME  // 萝  萝   元
			+ My_Earth * Vs_Wind * AJ_DOWN;  // 萝  氘 浇中

	// 氘
	My_Wind = My_Wind * Vs_None * AJ_UP		 // 氘     雄中
			+ My_Wind  * Vs_Fire * AJ_DOWN   // 氘  绍  浇中
			+ My_Wind * Vs_Water * AJ_SAME   // 氘        元
			+ My_Wind * Vs_Earth * AJ_UP     // 氘  萝  雄中
			+ My_Wind * Vs_Wind * AJ_SAME;   // 氘  氘    元

	//   箪岭
	My_None = My_None * Vs_None * AJ_SAME	//          元
			+ My_None * Vs_Fire * AJ_DOWN   //     绍  浇中
			+ My_None * Vs_Water * AJ_DOWN  //         浇中
			+ My_None * Vs_Earth * AJ_DOWN  //     萝  浇中
			+ My_None * Vs_Wind * AJ_DOWN;  //     氘  浇中

	// 蝈  宁煌允月
	iRet = (My_Fire + My_Water + My_Earth + My_Wind + My_None) ;

	// 喘仃遥壬匀井曰匹  五仁卅匀凶及匹公及坌尺日允
	return (iRet * D_ATTR);
}

extern float BATTLE_FieldAttAdjust(int battleindex,int pAt_Fire,int pAt_Water,int pAt_Earth,int pAt_Wind);
#ifndef _FIX_MAGICDAMAGE
static int BATTLE_AttrAdjust(
	int attackindex,
	int defindex,
	int damage
)
{
	int At_Fire, At_Earth, At_Water, At_Wind, At_none;
	int Df_Fire, Df_Earth, Df_Water, Df_Wind, Df_none;
	float At_FieldPow, Df_FieldPow;

	BATTLE_GetAttr(attackindex,&At_Fire,&At_Water,&At_Earth,&At_Wind,&At_none);

	At_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX ),
		At_Fire, At_Water, At_Earth, At_Wind );

	At_Fire *= damage;	At_Water *= damage;	At_Earth *= damage;
	At_Wind *= damage;	At_none *= damage;
	BATTLE_GetAttr( defindex, &Df_Fire, &Df_Water,
		&Df_Earth,&Df_Wind, &Df_none );
	Df_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX ),
		Df_Fire, Df_Water, Df_Earth, Df_Wind );
	damage = BATTLE_AttrCalc(
		At_Fire, At_Water, At_Earth, At_Wind, At_none,
		Df_Fire, Df_Water, Df_Earth, Df_Wind, Df_none );
	damage *= (At_FieldPow / Df_FieldPow);
	return damage;
}
#endif
// 计算闪避率
// return: 0:没闪过 1:闪过
int BATTLE_MagicDodge(int charindex,int nDefKind,int nFieldAttr)
{
	float fLuck = 0,fResist = 0;
#ifdef _EQUIT_DEFMAGIC
	float Dluck=0.0;
#endif
	int charType = CHAR_getInt( charindex, CHAR_WHICHTYPE);
	// 如果是防守者是玩家
	if( charType == CHAR_TYPEPLAYER ){
		fLuck = (float)CHAR_getInt( charindex, CHAR_LUCK) * 3;
		fResist = (float)CHAR_getInt( charindex, CHAR_EARTH_RESIST + nFieldAttr) * 0.15;
		fLuck += fResist;
#ifdef _EQUIT_DEFMAGIC
		Dluck = (float)(CHAR_getWorkInt( charindex, CHAR_EQUITQUIMAGIC)*0.9);
	    fLuck += Dluck;
#endif
	}else {// 如果是防守者是宠物
		fLuck = (float)CHAR_getInt( charindex, CHAR_LV) * 0.2;
		if(fLuck > 30) fLuck = 30;
	}
	if(rand()%100+1 > (int)fLuck)
		return 0;
	else
		return 1;
}


#define DEF_MAGIC_NUM	4

extern void BATTLE_changeRideImage( int index );

#ifdef _FIX_MAGICDAMAGE
void BATTLE_MultiAttMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int MagicLv)
#else
void BATTLE_MultiAttMagic( int battleindex, int attackNo, int toNo , int attIdx,
						  int FieldAttr , int Power)
#endif
{
    int  list[SIDE_OFFSET * 2 + 1];
    int  listidx,i,j,k,z;
    int  basex , basey , magicattidx,def_is_player[10],def_be_hit[10];
    int  att_magic_lv[4];
	int  def_magic_resist[DEF_MAGIC_NUM];
    int  attvalue , charahp , pethp , charaidx , petidx , charahurt,pet_att_lv = 1,pet_def_lv = 1,charahurt_temp;
    char szcommand[256];
	int  AttIsPlayer=0,DefIsPlayer=0;
	int getexp = 0;

#ifndef _FIX_MAGICDAMAGE
	int att_magic_exp_add,att_magic_exp_sub,def_magic_exp_add,def_magic_exp_sub;
	int lv_up_exp,DefFieldAttr = 0;
	float temp = 0.0f;
//	int attattr[5], defattr[5];
	char msgbuf[64];
	char kind[4][3] = {"地","水","火","风"};
#else
	BOOL TrueMagic=FALSE;
#endif

    // terry 


    if((z = BATTLE_MultiList(battleindex,toNo,list)) == -1)
		return;
	else
	{
		if(z != toNo) toNo = z;
	}

	BATTLE_AttMagicEffect( battleindex , attackNo , list , attIdx );

    // 计算攻击的人物清单
    if( attackNo < 10 )
        magicattidx = attIdx * 2 + 1;
    else
        magicattidx = attIdx * 2;

    listidx = 0;
		memset(def_is_player,-1,sizeof(def_is_player));
		memset(def_be_hit,-1,sizeof(def_be_hit));

    // 单人攻击
    if( toNo < 20 ){
		toNo = list[0];
       basex = CharTableIdx[toNo][1];
       basey = CharTableIdx[toNo][0];

       for( i = 0 , j = basey - 1 ; j <= basey + 1 ; i++ , j++ ){
          if( toNo < 10 && ( j < 2 || j > 3 ) )
             continue;
          else if( toNo >= 10 && ( j < 0 || j > 1 ) )
             continue;
          // 计算那些人需要加入清单
          for( k = 0 ; k < 5 ; k++ ){
             if( basex - 2 + k < 0 || basex - 2 + k > 4 )
                continue;

             if( ATTMAGIC_magic[magicattidx].siField[i][k] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[j][basex - 2 + k] ) )
             {
                list[listidx] = CharTable[j][basex - 2 + k];
                listidx++;
             }
          }
       }
    }else if( 20 == toNo ){	    // 右下全体
       for( i = 0 ; i < 2 ; i++ ){
          for( j = 0 ; j < 5 ; j++ ){
             if( ATTMAGIC_magic[magicattidx].siField[i][j] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[i + 2][j] ) ){
                list[listidx] = CharTable[i + 2][j];
                listidx++;
             }
          }
       }
    }else if( 21 == toNo ){	// 左上全体
       for( i = 0 ; i < 2 ; i++ ){
          for( j = 0 ; j < 5 ; j++ ){
             if( ATTMAGIC_magic[magicattidx].siField[i][j] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[i][j] ) ){
                list[listidx] = CharTable[i][j];
                listidx++;
             }
          }
       }
    }
    // 左上第一列 , 左上第二列 , 右下第一列 , 右下第二列
    else if( 23 == toNo || 24 == toNo || 25 == toNo || 26 == toNo ){
       basey = toNo - 23;

       for( i = 0 , j = basey - 1 ; j <= basey + 1 ; i++ , j++ ){
          if( ( 25 == toNo || 26 == toNo ) && ( j < 2 || j > 3 ) )
             continue;
          else if( ( 23 == toNo || 24 == toNo ) && ( j < 0 || j > 1 ) )
             continue;

          // 计算那些人需要加入清单
          for( k = 0 ; k < 5 ; k++ ){
             if( ATTMAGIC_magic[magicattidx].siField[i][k] && TRUE == BATTLE_TargetCheck( battleindex , CharTable[j][k] ) )
             {
                list[listidx] = CharTable[j][k];
                listidx++;
             }
          }
       }
    }

		
		qsort( list , listidx , sizeof( list[0] ) , ( FUNC )SortLoc );// 排序位置

		// 取得人物的属性(不论玩家或是宠物)
		//BATTLE_GetAttr( BATTLE_No2Index( battleindex , attackNo ) , &attattr[2] , &attattr[1] , &attattr[0] , &attattr[3] , &attattr[4] );
		// 如果攻击者是玩家,取得人物的魔法熟练度及魔法抗性
		{
			int attType = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_WHICHTYPE);
			int Check=0;
			AttIsPlayer=0;
			if( attType == CHAR_TYPEPLAYER)	{
				AttIsPlayer = 1;
				for(i=0;i<4;i++){	// att_magic_lv[i]: i = 0:地 1:水 2:火 3:风
					att_magic_lv[i] = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+i);
				}
			}else if( attType == CHAR_TYPEENEMY )	{
				for(i=0;i<4;i++){
					att_magic_lv[i] = (CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV)*0.9);
				}
			}else	{
#ifdef _FIX_MAGICDAMAGE
				AttIsPlayer = 1;
#endif
				for(i=0;i<4;i++){
#ifdef _FIX_MAGICDAMAGE
					att_magic_lv[i] = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+i);
#else
					att_magic_lv[i] = 0;
#endif
				}
			}
			Check = rand()%100;
#ifdef _FIX_MAGICDAMAGE
			if( Check > att_magic_lv[ FieldAttr] ) TrueMagic = FALSE;
			else TrueMagic = TRUE;
#endif
		}
		pet_att_lv = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV);

		z = 0;
    for(i=0;i<listidx;i++){
	  charaidx = BATTLE_No2Index( battleindex , list[i] );
      petidx   = BATTLE_getRidePet( charaidx );
      //BATTLE_GetAttr( charaidx , &defattr[2] , &defattr[1] , &defattr[0] , &defattr[3] , &defattr[4] );
		{//andy_fix
			int defType = CHAR_getInt(charaidx,CHAR_WHICHTYPE);
			DefIsPlayer = 0;
			if( defType == CHAR_TYPEPLAYER)	{
				DefIsPlayer = 1;
				for(j=0;j<DEF_MAGIC_NUM;j++){	// def_magic_resist[i]: i = 0:地 1:水 2:火 3:风
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
#ifdef _EQUIT_DEFMAGIC
					def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#endif
				}
			}else if( defType == CHAR_TYPEENEMY )	{
				for(j=0;j<DEF_MAGIC_NUM;j++){
					def_magic_resist[j] = (CHAR_getInt(charaidx,CHAR_LV)*0.5);
				}
			}else	{	//PET
#ifdef _FIX_MAGICDAMAGE
				DefIsPlayer = 1;
#endif
				for(j=0;j<DEF_MAGIC_NUM;j++){
#ifdef _FIX_MAGICDAMAGE
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
					//def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#else
					def_magic_resist[j] = 0;
#endif
				}
			}

#ifdef _MAGIC_DEFMAGICATT
			if( CHAR_getWorkInt( charaidx, CHAR_DEFMAGICSTATUS ) > 0 ){
				float def = (float)(CHAR_getWorkInt( charaidx, CHAR_OTHERSTATUSNUMS ));
				def = def/100;
				for(j=0;j<DEF_MAGIC_NUM;j++){
					if( def_magic_resist[j] <= 0 ) continue;
					def_magic_resist[j] += def_magic_resist[j] * def;
				}
			}
#endif

		}
		pet_def_lv = CHAR_getInt(charaidx,CHAR_LV);
			if(BATTLE_MagicDodge(charaidx,DefIsPlayer,FieldAttr)){	// 计算闪避率
				attvalue = pethp = 0;
				if( 0 == ATTMAGIC_magic[magicattidx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , attvalue , pethp );
				}else{	// 多人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , attvalue, pethp );
				}
				BATTLESTR_ADD( szcommand );
				continue;
			}else{	// 没闪过,计算攻击力
#ifdef _FIX_MAGICDAMAGE
				float Kmagic = ((float)att_magic_lv[FieldAttr]*1.4 - (float)def_magic_resist[FieldAttr]);
				float Mmagic = (float)att_magic_lv[FieldAttr];
				float Amagic=0.00;
				int APower=0;
				if( Kmagic < 0 ) Kmagic = 0;
				if( Mmagic < 1 ) Mmagic = 1;
				Amagic = (Kmagic*Kmagic) / (Mmagic*Mmagic);
				Amagic += ((float)(rand()%20))/100;
				APower = (int)(Power*(1+(float)MagicLv/10) * Amagic);	//MagicLv

				//andy_log
				//print("\natt_magic_lv:%d", att_magic_lv[FieldAttr]);
				//print( "Player:%s\n", CHAR_getUseName( charaidx));
				//print( "ANDY Magic Power:%d Amagic:%f APower:%d \n", Power, Amagic, APower);
 

				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index
				attvalue = BATTLE_getMagicAdjustInt( BATTLE_No2Index(battleindex,attackNo), charaidx,
					MagicLv, FieldAttr, APower);

				//andy_log
//				print("attvalue:%d \n", attvalue);

				if( TrueMagic == FALSE )	{//使用失败
					attvalue*=0.7;
				}
				if( DefIsPlayer )	{
					Magic_ComputeDefExp( charaidx, FieldAttr, MagicLv, attvalue);
				}
#else
				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index
				// 计算属性相克
				attvalue = BATTLE_AttrAdjust(BATTLE_No2Index(battleindex,attackNo),charaidx,Power);
				if(DefIsPlayer){	// 被打的是玩家
					def_is_player[z++] = list[i];
					if(AttIsPlayer){	// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)def_magic_resist[FieldAttr])/(float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}else{
						temp = ((float)pet_att_lv - (float)def_magic_resist[FieldAttr]) / (float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}else{// 被打的是宠物
					if(AttIsPlayer){// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}else	{
						temp = ((float)pet_att_lv - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}
				// 上下取10
				attvalue += rand()%2 ? (rand()%10)+1 : -(rand()%10)-1;
				if( attvalue <= 0 ) attvalue = 1;
#endif
			}

      charahp = CHAR_getInt( charaidx , CHAR_HP );

       // 没有宠物
       if( -1 == petidx || CHAR_getInt( petidx , CHAR_HP ) <= 0 )
       {
		   if( ( charahp -= attvalue ) < 0 )	{
             charahp = 0;
			 //ANDY_ADD
			 //CHAR_setFlg( charaidx, CHAR_ISDIE, TRUE);
		   }

          CHAR_setInt( charaidx , CHAR_HP , charahp );
          pethp = 0;

          // 单人攻击
          if( 0 == ATTMAGIC_magic[magicattidx].uiAttackType )
             sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , attvalue , pethp );
          // 多人攻击
          else
             sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , attvalue, pethp );
       }
       else
       {
          pethp     = CHAR_getInt( petidx , CHAR_HP ); 
          charahurt = attvalue * BATTLE_CalcCharaRatio( FieldAttr , charaidx ) / 10;

					charahurt_temp = charahurt;
          if( ( charahp -= charahurt ) < 0 )
          {
            charahurt = charahp;
            charahp   = 0;
			//ANDY_ADD
			//CHAR_setFlg( charaidx, CHAR_ISDIE, TRUE);
          }
					
          attvalue = attvalue - charahurt;
          if( ( pethp -= attvalue ) < 0 )
          {
            pethp = 0;
						// 宠物没血了退出战场
						CHAR_setInt(charaidx,CHAR_RIDEPET,-1);
						BATTLE_changeRideImage(charaidx);
						CHAR_setWorkInt(charaidx,CHAR_WORKPETFALL,1);
          }
          CHAR_setInt( charaidx , CHAR_HP , charahp );
          CHAR_setInt( petidx , CHAR_HP , pethp );

					charahurt = charahurt_temp;

          // 单人攻击
          if( 0 == ATTMAGIC_magic[magicattidx].uiAttackType )
             sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , charahurt , attvalue );
          // 多人攻击
          else
             sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , charahurt, attvalue );
       }

		{
				//Change fix 加上这个先拿到DP值
				int aAttackList[BATTLE_ENTRY_MAX*2+1];
				aAttackList[0] = attackNo;
				aAttackList[1] = -1;
				BATTLE_AddProfit( battleindex, aAttackList);
		}
		//change fix 让替身娃娃可正常运作
		if(	CHAR_getInt( charaidx , CHAR_HP ) <= 0 
			&& CHAR_getInt( charaidx, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER 
			/*&& !BattleArray[battleindex].dpbattle*/ )
			CHAR_setFlg( charaidx, CHAR_ISDIE, 1 );
       
		BATTLESTR_ADD( szcommand );
    }

    sprintf( szcommand , "%X|" , 0x12345678 );
    BATTLESTR_ADD( szcommand );
#ifdef _FIX_MAGICDAMAGE
	if( (TrueMagic == FALSE) && AttIsPlayer)	{	//计算攻击方经验值
		Magic_ComputeAttExp( BATTLE_No2Index(battleindex,attackNo), FieldAttr, MagicLv, getexp);
	}
	for(i=0; i<listidx; i++)	{
		// 如果被攻击者中了睡眠
		if(CHAR_getWorkInt(def_be_hit[i],CHAR_WORKSLEEP) > 0) {
			CHAR_setWorkInt(def_be_hit[i],CHAR_WORKSLEEP,0);
			z = BATTLE_Index2No(battleindex,def_be_hit[i]);
			sprintf(szcommand,"BM|%X|%X|",z,0);
			BATTLESTR_ADD(szcommand);
		}
	}
#else
		//TrueMagic == FALSE
		// 攻击方的经验值-----------------------------------------------------------------------------
		if(AttIsPlayer){
			// 被克的魔法
			DefFieldAttr = (FieldAttr+1)%4;
			// 取得玩家所用的魔法的经验值
			att_magic_exp_add = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+FieldAttr);
			// 取得玩家所用的魔法的被克魔法经验值
			att_magic_exp_sub = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+DefFieldAttr);
			// 依所使用的魔法加该属性的exp,打几只算几点exp
			att_magic_exp_add += getexp;									
			// 依所使用的魔法扣被克属性的exp,打几只扣几点exp
			att_magic_exp_sub -= getexp;
			lv_up_exp = Magic_Level_Table[att_magic_lv[FieldAttr]];
			while(att_magic_exp_add >= lv_up_exp || att_magic_exp_sub < 0){
				// 如果玩家所用的魔法的经验值现在可以升级了
				if(att_magic_exp_add >= lv_up_exp){
					// 如果超过了100级
					if(att_magic_lv[FieldAttr] + 1 > 100){
						att_magic_lv[FieldAttr] = 100;
						// 经验值清为0
						att_magic_exp_add = 0;
					}else{
						att_magic_lv[FieldAttr]++;
						// 把现在的经验值扣掉升了级所用的经验值
						att_magic_exp_add -= lv_up_exp;
						snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法熟练度升级为 %d。",kind[FieldAttr],att_magic_lv[FieldAttr]);
		        CHAR_talkToCli(BATTLE_No2Index(battleindex,attackNo),-1,msgbuf,CHAR_COLORRED);
					}
				}
				// 如果玩家所用的魔法的相克魔法的经验值现在可以降级了
				if(att_magic_exp_sub < 0 ){
					// 如果等级已经是1级
					if(att_magic_lv[DefFieldAttr] <= 1){
						att_magic_lv[DefFieldAttr] = 1;
						// 经验值清为0
						att_magic_exp_sub = 0;
					}else{
						att_magic_lv[DefFieldAttr]--;
						// 把下一个等级的经验值扣掉现在的经验值
						att_magic_exp_sub = Magic_Level_Table[att_magic_lv[DefFieldAttr]] + att_magic_exp_sub;
						snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法熟练度降级为 %d。",kind[DefFieldAttr],att_magic_lv[DefFieldAttr]);
		        CHAR_talkToCli(BATTLE_No2Index(battleindex,attackNo),-1,msgbuf,CHAR_COLORRED);
					}
				}
				lv_up_exp = Magic_Level_Table[att_magic_lv[FieldAttr]];
			}
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+FieldAttr,att_magic_lv[FieldAttr]);
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+FieldAttr,att_magic_exp_add);
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+DefFieldAttr,att_magic_lv[DefFieldAttr]);
			CHAR_setInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_ATTMAGIC_EXP+DefFieldAttr,att_magic_exp_sub);
		}

		// 防守方的经验值-----------------------------------------------------------------------------
		for(i=0;i<listidx;i++){
			if(def_is_player[i] != -1){
				charaidx = BATTLE_No2Index(battleindex,def_is_player[i]);
				// 取得防守方玩家对此魔法的抗性等级
				def_magic_resist[FieldAttr] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+FieldAttr);
				// 取得防守方玩家对此魔法的抗性经验值
				def_magic_exp_add = CHAR_getInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+FieldAttr);
				// 取得防守方玩家对此魔法被克的抗性等级
				def_magic_resist[DefFieldAttr] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+DefFieldAttr);
				// 取得防守方玩家对此魔法被克的抗性经验值
				def_magic_exp_sub = CHAR_getInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+DefFieldAttr);
				def_magic_exp_add++;
				def_magic_exp_sub--;
				
				lv_up_exp = Magic_Level_Table[def_magic_resist[FieldAttr]];
				
				while(def_magic_exp_add >= lv_up_exp || def_magic_exp_sub < 0){
					// 如果防守方玩家对此魔法的抗性的经验值现在可以升级了
					if(def_magic_exp_add >= lv_up_exp){
						// 如果超过了100级
						if(def_magic_resist[FieldAttr] + 1 > 100) def_magic_resist[FieldAttr] = 100;
						else {
							def_magic_resist[FieldAttr]++;
							snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法抗性升级为 %d。",kind[FieldAttr],def_magic_resist[FieldAttr]);
							CHAR_talkToCli(charaidx,-1,msgbuf,CHAR_COLORRED);
						}
						// 经验值清为0
						def_magic_exp_add = 0;
					}
					// 如果防守方玩家对此魔法的抗性的相克魔法的经验值现在可以降级了
					if(def_magic_exp_sub < 0 ) {
						// 如果等级已经是1级
						if(def_magic_resist[DefFieldAttr] <= 1) def_magic_exp_sub = 0; // 经验值清为0
						else {
							def_magic_resist[DefFieldAttr]--;
							// 经验值变成下一个等级的经验值
							def_magic_exp_sub = Magic_Level_Table[def_magic_resist[DefFieldAttr]];
							snprintf(msgbuf,sizeof(msgbuf),"你的 %s 魔法抗性降级为 %d。",kind[DefFieldAttr],def_magic_resist[DefFieldAttr]);
							CHAR_talkToCli(charaidx,-1,msgbuf,CHAR_COLORRED);
						}
					}
					lv_up_exp = Magic_Level_Table[def_magic_resist[FieldAttr]];
				}
				CHAR_setInt(charaidx,CHAR_EARTH_RESIST+FieldAttr,def_magic_resist[FieldAttr]);
				CHAR_setInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+FieldAttr,def_magic_exp_add);
				CHAR_setInt(charaidx,CHAR_EARTH_RESIST+DefFieldAttr,def_magic_resist[DefFieldAttr]);
				CHAR_setInt(charaidx,CHAR_EARTH_DEFMAGIC_EXP+DefFieldAttr,def_magic_exp_sub);
			}
			// 如果被攻击者中了睡眠
			if(CHAR_getWorkInt(def_be_hit[i],CHAR_WORKSLEEP) > 0) {
				CHAR_setWorkInt(def_be_hit[i],CHAR_WORKSLEEP,0);
				z = BATTLE_Index2No(battleindex,def_be_hit[i]);
				sprintf(szcommand,"BM|%X|%X|",z,0);
				BATTLESTR_ADD(szcommand);
			}
		}
#endif
}

#endif

#ifdef _MAGIC_TOCALL
void BATTLE_MultiToCallDragonMagic( int battleindex, int attackNo, int toNo,
						  int attIdx ,int FieldAttr ,int Power, int ImageNo)
{
    int  list[SIDE_OFFSET * 2 + 1];
    int  listidx,i,j,k,z;
    int  /*basex , */basey , magicattidx,def_is_player[10],def_be_hit[10];
    int  attvalue , charahp , pethp , charaidx , petidx , charahurt,charahurt_temp;
    char szcommand[256];
	int  /*AttIsPlayer = 0,*/DefIsPlayer = 0;

#ifndef _FIX_MAGICDAMAGE
	int att_magic_exp_add,att_magic_exp_sub,def_magic_exp_add,def_magic_exp_sub;
	int lv_up_exp,DefFieldAttr = 0;
	float temp = 0.0f;
	char msgbuf[64];
#else
#endif

	int icindex, ioindex;
    // terry 
	print("BATTLE_MultiToCallDragonMagic in.............\n");
    if((z = BATTLE_MultiList(battleindex,toNo,list)) == -1)
		return;
	else{
		if(z != toNo) toNo = z;
	}

	TOCALL_magic[attIdx].uiSpriteNum = ImageNo;

	BATTLE_ToCallDragonEffect( battleindex , attackNo , list , attIdx);

    // 计算攻击的人物清单
	if( attackNo < 10 )
		magicattidx = attIdx * 2 + 1;
	else
		magicattidx = attIdx * 2;


	listidx = 0;
	memset(def_is_player,-1,sizeof(def_is_player));
	memset(def_be_hit,-1,sizeof(def_be_hit));

    // 单人攻击
	if( toNo < 20 ){
		toNo = list[0];
		listidx++;

	} else if( 20 == toNo ){	    // 右下全体
		for( i = 0 ; i < 2 ; i++ ){
			for( j = 0 ; j < 5 ; j++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[i + 2][j] ) ){
					list[listidx] = CharTable[i + 2][j];
					listidx++;
				}
			}
		}
	} else if( 21 == toNo ){	// 左上全体
		for( i = 0 ; i < 2 ; i++ ){
			for( j = 0 ; j < 5 ; j++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[i][j] ) ){
					list[listidx] = CharTable[i][j];
					listidx++;
				}
			}
		}
	} // 左上第一列 , 左上第二列 , 右下第一列 , 右下第二列
		else if( 23 == toNo || 24 == toNo || 25 == toNo || 26 == toNo ){
		basey = toNo - 23;
		for( i = 0 , j = basey - 1 ; j <= basey + 1 ; i++ , j++ ){
			if( ( 25 == toNo || 26 == toNo ) && ( j < 2 || j > 3 ) )
				continue;
			else if( ( 23 == toNo || 24 == toNo ) && ( j < 0 || j > 1 ) )
				continue;
			// 计算那些人需要加入清单
			for( k = 0 ; k < 5 ; k++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[j][k] ) )
				{
					list[listidx] = CharTable[j][k];
					listidx++;
				}
			}
		}
	}
	print("listidx=%d............................\n", listidx);
	qsort( list , listidx , sizeof( list[0] ) , ( FUNC )SortLoc );// 排序位置
	for( i = 0 ; i < listidx ; i++ ) {
		int defType;
		charaidx = BATTLE_No2Index( battleindex , list[i] );
		petidx   = BATTLE_getRidePet( charaidx );
		defType = CHAR_getInt(charaidx, CHAR_WHICHTYPE);
		DefIsPlayer = 0;
		if (defType == CHAR_TYPEPLAYER) {
			DefIsPlayer = 1;
		}
		if (CHAR_createCharacter( 100354 /*image no*/, 100, 600, 370, 0,
			&icindex, &ioindex, 0) == TRUE) {

			print("char create success\n");
//		CHAR_setInt( icindex, CHAR_STR, 100);
			CHAR_setInt( icindex, CHAR_LUCK, 1);
			CHAR_setWorkInt( icindex, CHAR_WORKATTACKPOWER,
					Power );
			CHAR_setWorkInt( icindex, CHAR_WORKOTHERDMAGE, 
					0);

			if (BATTLE_MagicDodge(charaidx, DefIsPlayer, FieldAttr)) {
// 没击中
				attvalue = 0;
			} else {
				if( 0 == TOCALL_magic[attIdx].uiAttackType )	{// 单人攻击
					attvalue = BATTLE_DamageCalc(icindex, charaidx);
				}
				else {
					attvalue = BATTLE_DamageCalc(icindex, charaidx);
				}
			}
			print("char create success=%d  %d\n", icindex, attvalue);
			CHAR_CharaDelete(icindex);
			charahp = CHAR_getInt( charaidx , CHAR_HP );
			if (-1 == petidx || CHAR_getInt(petidx, CHAR_HP) <= 0) {
// 没骑宠物
				if( ( charahp -= attvalue ) < 0 )	{
					charahp = 0;
				}
				CHAR_setInt( charaidx , CHAR_HP , charahp );
				pethp = 0;
				if( 0 == TOCALL_magic[attIdx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , attvalue , pethp);
				} else
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , attvalue , pethp);
			}else{
				pethp     = CHAR_getInt( petidx , CHAR_HP ); 
				charahurt = attvalue * BATTLE_CalcCharaRatio( FieldAttr , charaidx ) / 10;

				charahurt_temp = charahurt;
				if( ( charahp -= charahurt ) < 0 )
				{
					charahurt = charahp;
					charahp   = 0;
				}
					
				attvalue = attvalue - charahurt;
				if( ( pethp -= attvalue ) < 0)
				{
					pethp = 0;
					// 宠物没血了退出战场
					CHAR_setInt(charaidx,CHAR_RIDEPET,-1);
					BATTLE_changeRideImage(charaidx);
					CHAR_setWorkInt(charaidx,CHAR_WORKPETFALL,1);
				}
				CHAR_setInt( charaidx , CHAR_HP , charahp );
				CHAR_setInt( petidx , CHAR_HP , pethp );

				charahurt = charahurt_temp;

				if( 0 == TOCALL_magic[attIdx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , charahurt , attvalue);
				} else
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , charahurt , attvalue);

			}

			BATTLESTR_ADD( szcommand );
		}
	}

	sprintf( szcommand , "%X|" , 0x5711438 );
	BATTLESTR_ADD( szcommand );

}

#endif

//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□毛汊唾今六月
//
void BATTLE_MultiRessurect(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int power,			// 湘  荚汊袄
	int per,			// ⊙井＂
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{
	int i, toindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];


	BATTLE_MultiListDead( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	// 汊唾今六月
	for( i = 0; ToList[i] != -1; i ++ ){
		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		//           反皿伊奶乩□及戏五忒曰  仄
		if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P
		&& CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
		){
			continue;
		}

		// 竣濮仄化中卅仃木壬汊唾今六卅中
		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == FALSE ){
			continue;
		}

		// 公及引引及煌遥
		UpPoint = power;

		if( per ){
			// ⊙煌遥
			UpPoint *= CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) * 0.01;
		}

		if( power == 0 ){
			// 仇及桦宁反敦蝈荚汊
			UpPoint = CHAR_getWorkInt( toindex, CHAR_WORKMAXHP );
		}else{
			// 荚汊汹卞汔毛  凶六月(漆反赝癫)
			UpPoint = RAND( (power*0.9), (power*1.1) );
		}
		//   斓匹手ㄠ
		UpPoint = max( 1, UpPoint );

		workhp = CHAR_getInt( toindex, CHAR_HP ) + UpPoint;

		//     毛译尹卅中方丹卞荚汊
		CHAR_setInt( toindex, CHAR_HP,
			min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );

		// 汊唾
		CHAR_setFlg( toindex, CHAR_ISDIE, 0 );

		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)恢复意识",
		//		CHAR_getUseName( toindex ) );

		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		// (汊唾)戊穴件玉毛中木月
		snprintf( szCommand, sizeof(szCommand),
				"BL|r%X|h%X|", ToList[i], CHAR_getInt( toindex, CHAR_HP ) );

		//   侬  馨笛
		BATTLESTR_ADD( szCommand );
/*
		{	int Sign, Damage;
			Sign = (UpPoint >= 0)?(1):(0);
			Damage = ABS( UpPoint );
			// (    荚汊)戊穴件玉毛中木月
			snprintf( szCommand, sizeof(szCommand),
				"BD|r%X|%X|%X|d%X|", ToList[i], 0, Sign, Damage );
		}
		//   侬  馨笛
		BATTLESTR_ADD( szCommand );
*/

		// 衬午及爵    匹伉旦弁互卅井匀凶日
		if( BattleArray[battleindex].norisk == 0
		 &&	BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E
		 ){
			// 仇仇匹矢永玄分匀凶日袄毛皿仿旦允月
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRESSURECT );
			}
		}
	}

	return;

}

#ifdef _Item_ReLifeAct
void BATTLE_MultiReLife( int battleindex, int attackNo, int toNo, 
						int power, int RecevEffect )
{
	int i, toindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];
	//char szBuffer[256]="";
	char szCommand[256];
	char buf1[256];

	memset(szCommand, 0, sizeof(szCommand));
	BATTLE_MultiListDead( battleindex, toNo, ToList );


	//snprintf( szCommand, sizeof(szCommand),	"Bj|");
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		//if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P
		//&& CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		//	continue;
		//}
		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == FALSE ){
			continue;
		}
		UpPoint = power;
		workhp = max( 1, UpPoint );

		CHAR_setInt( toindex, CHAR_HP, min( workhp, CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
		CHAR_setFlg( toindex, CHAR_ISDIE, 0 );
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)恢复意识", CHAR_getUseName( toindex ) );
#if 0	// Robin 修正多人同时替身娃娃复活会当机
		snprintf( buf1, sizeof(buf1), "Bj|r%X|l%X|e%X|h%X|FF|",
			ToList[i], 1, RecevEffect, CHAR_getInt( toindex, CHAR_HP ));
		strcat( szCommand, buf1);
#else
		snprintf( buf1, sizeof(buf1),
			"BJ|a%X|m%X|e%X|e%X|FF|",
			ToList[i],
			CHAR_getInt( toindex, CHAR_MP ),
			RecevEffect, //MyEffect,
			0  //ToEffect
		);
		strcat( szCommand, buf1);

		snprintf( buf1, sizeof(buf1),
			"BL|r%X|h%X|", ToList[i], CHAR_getInt( toindex, CHAR_HP ) );
		strcat( szCommand, buf1);

#endif
		if( BattleArray[battleindex].norisk == 0
		 &&	BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E ){
			if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				CHAR_PetAddVariableAi( toindex, AI_FIX_PETRESSURECT );
			}
		}
	}
	//strcat( szCommand, "FF|");
	BATTLESTR_ADD( szCommand );
	return;
}
#endif

void BATTLE_MultiStatusChange(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int status,			// 旦  □正旦潘  
	int turn,			// 正□件醒
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect,	// 井仃日木月谛及巨白尼弁玄
	int Success			// 岳    
)
{
	int i, toindex, charaindex;
	int perStatus;
	int ToList[SIDE_OFFSET*2+1];
	//char szBuffer[256]="";

	BATTLE_MultiList( battleindex, toNo, ToList );
	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		if( BATTLE_StatusAttackCheck( charaindex,
			toindex, status, Success, 30, 1.0, &perStatus ) == FALSE ){
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)无法将(%s)做成(%s)(%d%%)",
			//	CHAR_getUseName( charaindex ),
			//	CHAR_getUseName( toindex ),
			//	aszStatusFull[status],
			//	perStatus
			//);
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		}else{
			CHAR_setWorkInt( toindex, StatusTbl[status], turn );

			if( status == BATTLE_ST_PARALYSIS
			||  status == BATTLE_ST_SLEEP
			||  status == BATTLE_ST_STONE
			||  status == BATTLE_ST_BARRIER	){
				CHAR_setWorkInt( toindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
			}

			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)将(%s)做成(%s)(%d%%)",
			//	CHAR_getUseName( charaindex ),
			//	CHAR_getUseName( toindex ),
			//	aszStatusFull[status],
			//	perStatus
			//);
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
			BATTLE_BadStatusString( ToList[i], status );
		}
	}
	return;
}

#ifdef _OTHER_MAGICSTAUTS
void BATTLE_MultiMagicStatusChange( int battleindex, int attackNo, int toNo, int status, int turn,
	int UseEffect, int RecevEffect, int nums )
{
	int i, j, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];

	BATTLE_MultiList( battleindex, toNo, ToList );
	BATTLE_MagicEffect( battleindex, attackNo, ToList, UseEffect, RecevEffect );

	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		for( j=1; j<MAXSTATUSTYPE; j++)	{
			if( CHAR_getWorkInt( toindex, MagicTbl[j]) <= 0 ) continue;
			break;
		}
		if( j == MAXSTATUSTYPE )	{
//			char szBuffer[256]="";
			CHAR_setWorkInt( toindex, MagicTbl[status], turn );
			CHAR_setWorkInt( toindex, CHAR_OTHERSTATUSNUMS, nums );
//			sprintf( szBuffer, "Bm|%X|%X|", ToList[i], status );
//			BATTLESTR_ADD( szBuffer );
		}
	}
	return;
}
#endif



//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□毛旦  □正旦荚汊
//
void BATTLE_MultiStatusRecovery(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int status,			// 旦  □正旦潘  
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, j, toindex, charaindex, tostatus;
	int ToList[SIDE_OFFSET*2+1];


	//char szBuffer[256]="";


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );

	// 旦  □正旦唱橘荚汊今六月
	for( i = 0; ToList[i] != -1; i ++ ){
		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		// 锹澎互升及旦  □正旦井＂
		tostatus = 0;
		//检查异常状态
		for( j = 1; j < BATTLE_ST_END; j ++ ){
			if( CHAR_getWorkInt( toindex, StatusTbl[j] ) > 0 ){
				tostatus = j;
			}
		}
		//======== 仇仇匹岳  井升丹井毛  烂今六月 ==========
		if( tostatus == 0 ){//正常
			// 撩  凛
//	print("\n vincent--很健康");
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)很健康",
			//	CHAR_getUseName( toindex ) );
		}else
		// 蝈莒隙烂匹尕麻僖分匀凶日
		if( ( status == 0 && tostatus != 0 && tostatus <= CHAR_WORKCONFUSION ) // 蝈莒隙烂匹尕麻僖//找到异常
		||	( status == tostatus ) // 唱橘午荚汊椭互穴永民仄凶
		){
			// 荚汊允月
			CHAR_setWorkInt( toindex, StatusTbl[tostatus], 0 );
#ifdef _MAGIC_NOCAST
			if( StatusTbl[tostatus] == CHAR_WORKNOCAST ){
            //print("\nvincent-->lssproto_NC_send");
			lssproto_NC_send( getfdFromCharaIndex( toindex ), 0);//沉默解除
			}
#endif
//	print("\n vincent--解除了:%d",tostatus);
			// 岳  凛
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)解除了(%s)的(%s)",
			//	CHAR_getUseName( charaindex ),
			//	CHAR_getUseName( toindex ),
			//	aszStatusFull[tostatus]
			//);
			// 仇仇匹旦  □正旦唱橘迕  侬  戊穴件玉毛中木月
			BATTLE_BadStatusString( ToList[i], 0 );//解除异常

		}else{
//	print("\n vincent--不");
			// 撩  凛
/*			snprintf( szBuffer, sizeof(szBuffer),
				"(%s)不(%s)",
				CHAR_getUseName( toindex ),
				aszStatusFull[status]
			);
*/
		}
		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	}

	return;
}

//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□毛  芊  豢
//
void BATTLE_MultiMagicDef(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int kind,			// 升及  芊  豢井
	int count,			// 荚醒
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );
	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		CHAR_setWorkInt( toindex, MagicDefTbl[kind], count );
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)将(%s)遭受(损伤%s)",
		//	CHAR_getUseName( charaindex ),
		//	CHAR_getUseName( toindex ),
		//	aszMagicDefFull[kind]
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	}

	return;



}


#if 1

//***********************************************************
//
//    芊羁匹  醒及平乓仿弁正□及由仿丢□正毛晓票今六月
//
void BATTLE_MultiParamChange(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int kind,			// 升及由仿丢□正井
	int power,			// 禾奶件玄
	int par,			// ⊙煌遥允月
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];
	int res = 0, work;
	//char szBuffer[256]="";


	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );

	// 升及由仿丢□正卞允月井
	for( i = 0; ToList[i] != -1; i ++ ){

		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		switch( kind ){
		case PC_KIND_ATTACK:
			if( par ){
				// ⊙煌遥卅凶户｝ㄠㄟㄟ反井仃卅中
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXSTR );
				work = work * power;
			}else{
				// 公及引引及煌遥卅凶户ㄠㄟㄟ井仃月
				work = power * 100;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODATTACK,
				CHAR_getWorkInt( toindex, CHAR_WORKMODATTACK ) + work );
			res = work * 0.01;
			break;
		case PC_KIND_DEFENSE:
			if( par ){
				// ⊙煌遥卅凶户｝ㄠㄟㄟ反井仃卅中
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXTOUGH );
				work = work * power;
			}else{
				// 公及引引及煌遥卅凶户ㄠㄟㄟ井仃月
				work = power * 100;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODDEFENCE,
				CHAR_getWorkInt( toindex, CHAR_WORKMODDEFENCE ) + work );
			res = work * 0.01;
			break;
		case PC_KIND_QUICK:
			if( par ){
				// ⊙煌遥卅凶户｝ㄠㄟㄟ反井仃卅中
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXDEX );
				work = work * power;
			}else{
				// 公及引引及煌遥卅凶户ㄠㄟㄟ井仃月
				work = power * 100;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODQUICK,
				CHAR_getWorkInt( toindex, CHAR_WORKMODQUICK ) + work );
			res = work * 0.01;
			break;
		case PC_KIND_CHARM:
			if( par ){
				// CHARM 反 100  今木化卅中凶户｝100坌及ㄠ允月
				work = CHAR_getWorkInt( toindex, CHAR_WORKFIXCHARM );
				work = work * power * 0.01;
			}else{
				// 公及引引及煌遥
				work = power;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKMODCHARM,
				CHAR_getWorkInt( toindex, CHAR_WORKMODCHARM ) + work );
			res = work;
			break;
		case PC_KIND_CAPTURE:
			// CAPTURE 反 % 及心
			work = power;
			CHAR_setWorkInt( toindex, CHAR_WORKMODCAPTURE,
				CHAR_getWorkInt( toindex, CHAR_WORKMODCAPTURE ) + work );
			res = work;
			break;
		}

		// 仇仇引匹  凶日  内岳  
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)被(%s)(%+d)",
		//	CHAR_getUseName( toindex ),
		//	aszParamChangeFull[kind],
		//	res
		//);

		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	}

	return;
}

#endif


//***********************************************************
//
//    芊羁匹  醒及箪岭  鳖
//
void BATTLE_MultiAttReverse(
	int battleindex, 	// 田玄伙奶件犯永弁旦
	int attackNo,		// 井仃月谛  寞
	int toNo,			// 井仃日木月谛  寞
	int UseEffect,		// 银丹谛及巨白尼弁玄
	int RecevEffect		// 井仃日木月谛及巨白尼弁玄
)
//
//***********************************************************
{

	int i, toindex, charaindex;
	int ToList[SIDE_OFFSET*2+1];
	int flg, OnOff;
	char szBuffer[256]="";

	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex, 			// 田玄伙奶件犯永弁旦
		attackNo, 				// 井仃月谛  寞(ㄟ  ㄠㄨ)
		ToList, 				// 井仃日木月谛  寞伉旦玄(ㄟ  ㄠㄨ)
		UseEffect, 				// 井仃月谛巨白尼弁玄
		RecevEffect				// 井仃日木月谛巨白尼弁玄
	);

	charaindex = BATTLE_No2Index( battleindex, attackNo );

	// 升及由仿丢□正卞允月井
	for( i = 0; ToList[i] != -1; i ++ ){

		// 锹澎及奶件犯永弁旦
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
		flg ^= CHAR_BATTLEFLG_REVERSE;	//   鳖
		CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg );
		OnOff = ( flg & CHAR_BATTLEFLG_REVERSE )?(1):(0);

		// 仇仇匹允井今内  鳖今六月
		BATTLE_AttReverse( toindex );

		if( OnOff ){
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)的属性变成相反。",
			//	CHAR_getUseName( toindex ) );
		}else{
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)回复成原本的属性。",
			//	CHAR_getUseName( toindex ) );
		}

		//   须  煤
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		// 仇仇匹箪岭  鳖及  侬  毛霜月
		sprintf( szBuffer, "BR|%X|%X|", ToList[i], OnOff );
		BATTLESTR_ADD( szBuffer );
	}

	return;

}

void BATTLE_MultiCaptureUp(
	int battleindex,
	int attackNo,
	int toNo,
	int power,
	int UseEffect,
	int RecevEffect
)
{
	int i, toindex, UpPoint = 0, workhp;
	int ToList[SIDE_OFFSET*2+1];

	//char szBuffer[256]="";
	char szCommand[256];

	BATTLE_MultiList( battleindex, toNo, ToList );

	BATTLE_MagicEffect(
		battleindex,
		attackNo,
		ToList,
		UseEffect,
		RecevEffect
	);

	for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
			continue;
		}

		if( CHAR_getFlg( toindex, CHAR_ISDIE ) == TRUE ){
			continue;
		}

		UpPoint = power;

		UpPoint = RAND( (power*0.9), (power*1.1) );

		workhp = CHAR_getWorkInt( toindex, CHAR_WORKMODCAPTURE ) + UpPoint;

		CHAR_setWorkInt( toindex, CHAR_WORKMODCAPTURE, workhp );

		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)的捕获率变成(%d)",
		//		CHAR_getUseName( toindex ), UpPoint );

		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		snprintf( szCommand, sizeof( szCommand ),
			"B%%|%X|%X|", ToList[i], BFUKI_CAPTUREUP );

		BATTLESTR_ADD( szCommand );
	}
	return;
}





//****************************************************************
//
// 白奴□伙玉箪岭  凳热诸
//
int	BATTLE_FieldAttChange(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	char *pArg
)
//
// 岳  允木壬 TRUE
// 撩  卅日   FALSE
//
//****************************************************************

{
	
	int attr = -1, i, attackNo, turn = 3;
	int battleindex, power = 30;
	int FieldAttr[] = { BATTLE_ATTR_NONE, BATTLE_ATTR_EARTH, BATTLE_ATTR_WATER, BATTLE_ATTR_FIRE, BATTLE_ATTR_WIND };
	char szBuffer[256]="";
	char szCommand[256],
		// Robin 03/20 debug
		//*pszP, *aszAttr[] = { "无", "土", "水", "火", "风" };
		*pszP, *aszAttr[] = { "无", "地", "水", "火", "风" };		
	// Robin 03/21 debug
	//char szTurn[] = "回";	
	char szTurn[] = "turn";


	pszP = pArg;
	// 箪岭毛潸  
	for( ;attr == -1 && pszP[0] != 0; pszP++ ){
		for( i = 0; i < 5; i ++ ){
			// 箪岭疋永正伉井＂
			if( strncmp( pszP, aszAttr[i], 2 ) == 0 ){
				attr = i;
				pszP +=2;
				break;
			}
		}
	}
	// 箪岭  中及匹撩  
	if( attr == -1 ) 
	{
		return FALSE;
	}

	// 升木仁日中  凳今六月井＂
	sscanf( pszP, "%d", &power );
	if( power < 0 || power > 100 )power = 30;

	// 窒正□件  凳今六月井＂
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return FALSE;
	//   祭今六月箪岭
	BattleArray[battleindex].field_att = FieldAttr[attr];
	// 膜恳允月由伐□
	BattleArray[battleindex].att_pow = power;
	// 膜恳允月正□件醒
	BattleArray[battleindex].att_count = turn;

	// 仇仇引匹  凶日  内岳  
	if( attr >= 0 ){
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)将field的属性改为(%s)",
		//	CHAR_getUseName( charaindex ),
		//	aszAttr[attr]
		//);
	}

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	//   须  煤
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	// 仇仇匹  侬  戊穴件玉毛中木月
	sprintf( szCommand, "BV|%X|%X|",attackNo, attr );
	BATTLESTR_ADD( szCommand );

	return TRUE;

}




//****************************************************************
//
// 白奴□伙玉箪岭  凳热诸
//
int	MAGIC_FieldAttChange_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 	// 井仃日木月谛及奶件犯永弁旦(  银迕)
	int marray, 	// magicindex
	int mp 			// MP
)
// 岳  允木壬 TRUE
// 撩  卅日   FALSE
//
//****************************************************************

{
	char *pArg;

	// 由仿丢□正毛潸  
	pArg = MAGIC_getChar( marray, MAGIC_OPTION );
	// 由仿丢□正  中及匹撩  
	if( pArg == NULL )return FALSE;

	return BATTLE_FieldAttChange( charaindex, pArg );

}
//****************************************************************
//
// 旦  □正旦唱橘毛芨尹月热诸
//
int	MAGIC_StatusChange_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 	// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex, ReceveEffect, Success = 15;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";

	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	pszP = magicarg;
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	if( status == -1 ) return FALSE;
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );
	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;	//   月午五反仇木
	}else{
		ReceveEffect = SPR_hoshi;	// 井井月午五反仇木
	}

	BATTLE_MultiStatusChange( battleindex, attackNo, toNo,
		status, turn, MAGIC_EFFECT_USER, ReceveEffect, Success );


	return TRUE;

}

#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange_Battle2(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 	// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex, ReceveEffect, Success = 15;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	if( magicarg == NULL )	{
		print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;
	// 躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// ㄠ井日腹绸
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 躲绊卅中及匹撩  
	if( status == -1 ) return FALSE;

	// 窒正□件  凳今六月井＂
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	// 岳    涩烂
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}



	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;	//   月午五反仇木
	}else{
		ReceveEffect = SPR_hoshi;	// 井井月午五反仇木
	}
	// 蝈够
	BATTLE_MultiStatusChange( battleindex, attackNo, toNo,
		status, turn+2, MAGIC_EFFECT_USER, ReceveEffect, Success );
	return TRUE;
}
#endif
#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange_Battle( int charaindex, int toNo, int marray, int mp )
{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3, nums=0;
	int battleindex, ReceveEffect;
	char buf1[256];

	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	if( getStringFromIndexWithDelim( magicarg,"|",1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	for( i=0; i<MAXSTATUSTYPE; i++)	{
		if( strcmp( buf1, MagicStatus[i])) continue;
		status = i;
		break;
	}
	if( i == MAXSTATUSTYPE ) return FALSE;
	if( getStringFromIndexWithDelim( magicarg,"|",2, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	turn = atoi( buf1);
	//CHAR_OTHERSTATUSNUMS
	if( getStringFromIndexWithDelim( magicarg,"|",3, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	nums = atoi( buf1);
	if( getStringFromIndexWithDelim( magicarg,"|",4, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	if( strstr( buf1, "单") != 0 )	{
		if( toNo >= 20 )
			return FALSE;
	}
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );
  //动画图号决定
#define	SPR_mic_def			101411
	ReceveEffect = SPR_heal2;
	if( status == 1 )
		ReceveEffect = SPR_mic_def;
#ifdef _MAGICSTAUTS_RESIST
	else if( status == 3 )
		ReceveEffect = 101802;
	else if( status == 4 )
		ReceveEffect = 101804;	
	else if( status == 5 )
		ReceveEffect = 101803;
#endif
	//else{
	//	ReceveEffect = SPR_hoshi;
	//}

	BATTLE_MultiMagicStatusChange(
		battleindex, attackNo, toNo, status,
		turn, MAGIC_EFFECT_USER, ReceveEffect, nums );

	return TRUE;
}
#endif


int	MAGIC_MagicDef_Battle(
	int charaindex,
	int toNo,
	int marray,
	int mp 
)
{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex;
	char *pszP;

	char szTurn[] = "turn";

	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			if( strncmp( pszP, aszMagicDef[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return FALSE;

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	BATTLE_MultiMagicDef( battleindex, attackNo, toNo,
		status, turn, MAGIC_EFFECT_USER, SPR_difence );


	return TRUE;
}







#if 1

//****************************************************************
//
// 由仿丢□正  祭毛芨尹月热诸
//
int	MAGIC_ParamChange_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	char *magicarg;
	int kind = -1, i, attackNo;
	int battleindex, pow, par = 0;

	char *pszP;

	// 由仿丢□正毛潸  //魔法名称
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;
	// 躲绊毛潸  //取得更改参数(无,攻,防,早,魅,捕)
	for( ;kind == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszParamChange[i], 2 ) == 0 ){
				kind = i;
				pszP +=2;
				break;
			}
		}
	}
	// 躲绊卅中及匹撩  
	if( kind == -1 ) return FALSE;

	if( strstr( pszP, "%" ) ){	// 仇及桦宁反⊙煌遥
		par = 1;
	}

	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 窒禾奶件玄丐仆月井
		pow = 30;
	}

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiParamChange( battleindex, attackNo, toNo,
		kind, pow, par, MAGIC_EFFECT_USER, SPR_hoshi );


	return TRUE;
}



#endif







//****************************************************************
//
// 蜊谛及箪岭  鳖毛芨尹月热诸
//
int	MAGIC_AttReverse_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************

{
	int attackNo;
	int battleindex;

	// 由仿丢□正反漆及午仇欠  中
	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiAttReverse( battleindex, attackNo, toNo,
		MAGIC_EFFECT_USER, SPR_kyu );


	return TRUE;
}



//****************************************************************
//
// 旦  □正旦唱橘毛荚汊允月热诸
//
int	MAGIC_StatusRecovery_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************
{
	char *magicarg;
	int status = -1, i, attackNo;
	int battleindex, ReceveEffect;
	char *pszP;

	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;
	// 躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// 蝈莒手丐月及匹ㄟ井日腹绸
		for( i = 0; i < BATTLE_ST_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 躲绊卅中及匹撩  
	if( status == -1 ) return FALSE;


	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

	ReceveEffect = SPR_tyusya;	//   月午五反仇木

	// 蝈够
	BATTLE_MultiStatusRecovery( battleindex, attackNo, toNo,
		status, MAGIC_EFFECT_USER, ReceveEffect );


	return TRUE;

}


//****************************************************************
//
// 竣濮井日汊唾允月热诸
//
int	MAGIC_Ressurect_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************
{
	char *magicarg;
	int attackNo, ReceveEffect;
	int battleindex, pow = 0, par = 0;

	char *pszP;

	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;

	if( strstr( pszP, "%" ) ){	// 仇及桦宁反⊙煌遥
		par = 1;
	}
	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 窒禾奶件玄荚汊允月井＂
		pow = 0;	// ㄟ及桦宁反敦蝈荚汊
	}

	if( pow <= 0 ){
		ReceveEffect = SPR_fukkatu3;
	}else
	if( pow <= 100 ){
		ReceveEffect = SPR_fukkatu1;
	}else
	if( pow <= 300 ){
		ReceveEffect = SPR_fukkatu2;
	}else{
		ReceveEffect = SPR_fukkatu3;
	}

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiRessurect( battleindex, attackNo, toNo,
		pow, par, MAGIC_EFFECT_USER, ReceveEffect );

	return TRUE;
}


int	MAGIC_ResAndDef_Battle(
	int charaindex,
	int toNo,
	int marray,
	int mp
)
{
	char *magicarg;
	int attackNo, i;
	int battleindex, pow = 0, par = 0, turn = 3, status =  -1;
	// Robin 03/21 debug
	//char szTurn[] = "回";
	char szTurn[] = "turn";

	char *pszP;

// Terry add 2004/12/7
	// 复活光..不得使用敌方
	battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
	if(CHAR_getInt(charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
	{
		if(BATTLE_CheckSameSide(charaindex, toNo) == 0) //不同边
		{
			battleindex = CHAR_getWorkInt(charaindex, CHAR_WORKBATTLEINDEX);
			BATTLE_NoAction(battleindex, BATTLE_Index2No(battleindex, charaindex));
			CHAR_talkToCli(charaindex, -1, "复活光镜守..不得施予非玩家敌方。", CHAR_COLORYELLOW);
			return;
		}
	}
// end
	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	pszP = magicarg;

	if( strstr( pszP, "%" ) ){	// 仇及桦宁反⊙煌遥
		par = 1;
	}
	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 窒禾奶件玄荚汊允月井＂
		pow = 0;	// ㄟ及桦宁反敦蝈荚汊
	}

	// 馨笛允月  芊  豢躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			char *p;
			// 躲绊疋永正伉井＂
			p = strstr( pszP, aszMagicDef[i] );
			if( p != NULL ){
				status = i;
				pszP = p;
				break;
			}
		}
	}

	// 躲绊卅中及匹撩  
	if( status == -1 ) return FALSE;

	// 窒荚  凳今六月井＂
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiResAndDef( battleindex, attackNo, toNo,
		pow, par, status, turn, MAGIC_EFFECT_USER, SPR_difence );

	return TRUE;
}

#ifdef __ATTACK_MAGIC

int MAGIC_AttMagic_Battle( int charaindex , int toNo , int marray , int mp )
{
	int  attno , attidx , battleindex;
	int  attr = -1, i , power;
	char aszattr[][32] = { "地", "水", "火", "风" };
	char buf1[256];
	char *magicarg;
	int magiclv=0;
	magicarg = MAGIC_getChar( marray , MAGIC_OPTION );
	if( magicarg == NULL )	{
		print("\n Magic magicarg == NULL ");
		return FALSE;
	}

	for( i=0; i<4; i++)	{
		if( strstr( magicarg, aszattr[i]) != NULL )	{
			attr = i;
			break;
		}
	}
	if( i == 4 )	{
		print("\n not find aszattr !!");
		return FALSE;
	}
	memset( buf1, 0, sizeof( buf1));
	if( getStringFromIndexWithDelim( magicarg, "|", 2, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	power = atoi( buf1);

	if( getStringFromIndexWithDelim( magicarg, "|", 3, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	magiclv = atoi( buf1);

	battleindex   = CHAR_getWorkInt( charaindex , CHAR_WORKBATTLEINDEX );
	attno         = BATTLE_Index2No( battleindex , charaindex );
	attidx        = MAGIC_getInt( marray , MAGIC_IDX );
	if( -1 == attidx || attidx >= ATTMAGIC_magicnum )
		return FALSE;
#ifdef _FIX_MAGICDAMAGE
	BATTLE_MultiAttMagic( battleindex , attno , toNo , attidx , attr , power, magiclv);
#else
	BATTLE_MultiAttMagic( battleindex , attno , toNo , attidx , attr , power );
#endif
  return TRUE;
}

#endif


#ifdef _MAGIC_TOCALL

int MAGIC_ToCallDragon_Battle( int charaindex , int toNo , int marray , int mp )
{
	int  attno , attidx , battleindex;
	int  attr = -1,/* i , */power, imageno;
//	char aszattr[][32] = { "地", "水", "火", "风" };
	char buf1[256];
	char *magicarg;
	//int magiclv=0;
	magicarg = MAGIC_getChar( marray , MAGIC_OPTION );
	print("charaindex=%d, toNo=%d, marray=%d, mp=%d\n", charaindex, toNo, marray, mp);

	if( magicarg == NULL )	{
		print("\n Magic magicarg == NULL ");
		return FALSE;
	}
/*
	for( i=0; i<4; i++)	{
		if( strstr( magicarg, aszattr[i]) != NULL )	{
			attr = i;
			break;
		}
	}
	if( i == 4 )	{
		print("\n not find aszattr !!");
		return FALSE;
	}
*/
	memset( buf1, 0, sizeof( buf1));
	if( getStringFromIndexWithDelim( magicarg, "|", 1, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	imageno = atoi( buf1);
	if( getStringFromIndexWithDelim( magicarg, "|", 2, buf1, sizeof( buf1)) == FALSE )	return FALSE;
	power = atoi( buf1);

	battleindex   = CHAR_getWorkInt( charaindex , CHAR_WORKBATTLEINDEX );
	attno         = BATTLE_Index2No( battleindex , charaindex );
	attidx        = MAGIC_getInt( marray , MAGIC_IDX );
	print("attidx=%d....................\n", attidx);
	if( -1 == attidx || attidx >= ATTMAGIC_magicnum )
		return FALSE;
	print("battleindex=%d, attno=%d, attidx=%d, attr=%d, power=%d\n", battleindex, attno, attidx, attr, power);

	BATTLE_MultiToCallDragonMagic( battleindex , attno , toNo , attidx , attr , power, imageno);

  return TRUE;
}

#endif

//****************************************************************
//
//   凯    祭允月热诸
//
int	MAGIC_CaptureUp_Battle(
	int charaindex, // 井仃月谛及奶件犯永弁旦
	int toNo, 		// 井仃日木月谛及奶件犯永弁旦
	int marray, 	// magicindex
	int mp 			// MP
)
//  岳  仄凶日 TRUE
//  撩  仄凶日 FALSE
//****************************************************************
{
	char *magicarg;
	int attackNo, ReceveEffect;
	int battleindex, pow = 5;

	// 由仿丢□正毛潸  
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );

	if( sscanf( magicarg, "%d", &pow ) != 1 ){
		// 窒禾奶件玄荚汊允月井＂
		pow = 5;
	}

	ReceveEffect = SPR_hoshi;

	// 田玄伙  寞
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蝈够
	BATTLE_MultiCaptureUp( battleindex, attackNo, toNo,
		pow, MAGIC_EFFECT_USER, ReceveEffect );

	return TRUE;
}
#ifdef _FIX_MAGICDAMAGE
static int BATTLE_getMagicAdjustInt( int attackindex, int defindex, int MagicLv, int flg,int damage )
{
	int At_Fire, At_Earth, At_Water, At_Wind, At_none;
	int Df_Fire, Df_Earth, Df_Water, Df_Wind, Df_none;
	float At_FieldPow, Df_FieldPow;

	//取得施法者属性
	BATTLE_GetAttr(attackindex,&At_Fire,&At_Water,&At_Earth,&At_Wind,&At_none);
	MagicLv *=10;
	switch( flg)	{	//将属性置换为魔法牵引属性
	case 0:
		At_Earth = MagicLv+(MagicLv*(At_Earth/50));
		At_Water=0;	At_Fire=0; At_Wind=0;
		break;
	case 1:
		At_Water = MagicLv+(MagicLv*(At_Water/50));
		At_Earth=0;	At_Fire=0; At_Wind=0;
		break;
	case 2:
		At_Fire = MagicLv+(MagicLv*(At_Fire/50));
		At_Earth=0;	At_Water=0; At_Wind=0;
		break;
	case 3:
		At_Wind = MagicLv+(MagicLv*(At_Wind/50));
		At_Earth=0;	At_Water=0; At_Fire=0;
		break;
	}
	//取得该场属性加成
	At_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX ),
		At_Fire, At_Water,
		At_Earth, At_Wind );

	At_Fire *= damage;	At_Water *= damage;	At_Earth *= damage;
	At_Wind *= damage;	At_none *= damage;

	//取得防守方属性
	BATTLE_GetAttr( defindex, &Df_Fire, &Df_Water,
		&Df_Earth,&Df_Wind, &Df_none );
	//取得该场属性加成
	Df_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX ),
		Df_Fire, Df_Water, Df_Earth, Df_Wind );
	//四属相克
	damage = BATTLE_AttrCalc(
		At_Fire, At_Water, At_Earth, At_Wind, At_none,
		Df_Fire, Df_Water, Df_Earth, Df_Wind, Df_none );
	damage *= (At_FieldPow / Df_FieldPow);
	return damage;
}

void Magic_ComputeAttExp( int charindex, int Mnum, int MagicLv, int Expoint)
{
	int magiclv1, magicex1;
	int addEx = (MagicLv * 3) * Expoint;
	char MagicStr[][128]={"地","水","火","风"};
	char buf1[256];
	memset( buf1, 0, sizeof( buf1));
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_EXP+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum);
	magicex1 += addEx;
	if( magicex1 > 100 )	{
		magicex1 = 0;
		if( magiclv1 < 100 )	{
			magiclv1 += 1;
			sprintf( buf1,"%s魔法熟练度提升为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
		}
	}
	if( magicex1 < 0 ) magicex1 = 0;
	if( magiclv1 < 0 ) magiclv1 = 0;
	if( magiclv1 > 100 ) magiclv1 = 100;
	CHAR_setInt( charindex, CHAR_EARTH_EXP+Mnum, magiclv1);
	CHAR_setInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum, magicex1);

	Mnum = (Mnum+1)%4;	//相克的魔法
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_EXP+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum);
	if( magiclv1 > 1 )	{
		magicex1 -= addEx*0.5;
		if( magicex1 < 0 )	{
			magicex1 = 0;
			magiclv1 -= 1;
			if( magiclv1 < 0 ) magiclv1 = 0;
			CHAR_setInt( charindex, CHAR_EARTH_EXP+Mnum, magiclv1);
			sprintf( buf1,"%s魔法熟练度下降为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
		}
		CHAR_setInt( charindex, CHAR_EARTH_ATTMAGIC_EXP+Mnum, magicex1);
	}
	return;
}

void Magic_ComputeDefExp( int charindex, int Mnum, int MagicLv, int Damage)
{
	int magiclv1;
	int magicex1;
	int addEx;
	char buf1[256];
	char MagicStr[][128]={"地","水","火","风"};
	if( Damage < 200 )	return;
	memset( buf1, 0, sizeof( buf1));
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_RESIST+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum);
	addEx = (Damage/20)*(MagicLv*2);
	magicex1+=addEx;
	if( magiclv1 < 0 ) magiclv1 = 0;
	if( magicex1 > 100 )	{
		magicex1 = 0;
		if( magiclv1 < 100 )	{
			magiclv1 +=1;
			if( magiclv1 < 0 ) magiclv1 = 0;
			if( magiclv1 > 100 ) magiclv1 = 100;
			CHAR_setInt( charindex, CHAR_EARTH_RESIST+Mnum, magiclv1);
			sprintf( buf1,"%s抗魔值提升为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
		}
	}
	if( magicex1 < 0 ) magicex1 = 0;
	CHAR_setInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum, magicex1);
	Mnum = (Mnum+1)%4;	//相克的魔法
	magiclv1 = CHAR_getInt( charindex, CHAR_EARTH_RESIST+Mnum);
	magicex1 = CHAR_getInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum);
	if( magiclv1 > 1 )	{
		magicex1 -=2;
		if( magicex1 < 0 )	{
			magicex1 = 90;
			magiclv1 -=1;
			if( magiclv1 < 0 )
				magiclv1 = 0;
			sprintf( buf1,"%s抗魔值下降为%d。", MagicStr[Mnum], magiclv1);
			CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
			CHAR_setInt( charindex, CHAR_EARTH_RESIST+Mnum, magiclv1);
		}
		CHAR_setInt( charindex, CHAR_EARTH_DEFMAGIC_EXP+Mnum, magicex1);
	}
	return;
}

#endif

#ifdef _MAGIC_WEAKEN
int	MAGIC_ParamChange_Turn_Battle(
	int charaindex, 
	int toNo, 		
	int marray, 	
	int mp 			
)
{
	char *magicarg;
	int status = -1, i, attackNo, turn = 3;
	int battleindex, Success = 15;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
	magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
	if( magicarg == NULL )	{
		print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;
	// 躲绊毛潸  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// ㄠ井日腹绸
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			// 躲绊疋永正伉井＂
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return FALSE;

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );
	BATTLE_MultiParamChangeTurn( battleindex, attackNo, toNo, status, MAGIC_EFFECT_USER, SPR_hoshi, turn, Success );
	return TRUE;
}

void BATTLE_MultiParamChangeTurn(
	int battleindex, 	
	int attackNo,		
	int toNo,
	int status,
	int UseEffect,		
	int RecevEffect,		
	int turn,
	int Success
)
{

	int i, toindex,perStatus,charaindex;
	int ToList[SIDE_OFFSET*2+1];
    charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	BATTLE_MultiList( battleindex, toNo, ToList );
	
	//施魔法动画效果
	BATTLE_MagicEffect(battleindex, attackNo, ToList, UseEffect, RecevEffect);

	for( i = 0; ToList[i] != -1; i ++ )
	{
		toindex = BATTLE_No2Index( battleindex, ToList[i] );

		if( BATTLE_StatusAttackCheck( charaindex,
			toindex, status, Success, 30, 1.0, &perStatus ) == TRUE )
		{
			CHAR_setWorkInt( toindex, CHAR_WORKWEAKEN, turn+1 );
		}
	}
	return;

}
#endif


/*
typedef struct tagToCallMagic
{
  unsigned int  uiSpriteNum;			// 此咒术在Spr_x.bin的编号
  unsigned int  uiAttackType;			// 攻击的方式：单人，整排( 轮流 ) ,  整排( 轮流 ) , 整排( 同时 ) , 全体( 轮流 ) , 全体( 同时 )
  unsigned int  uiSliceTime;			// 轮流攻击时的时间差
  unsigned int  uiShowType;             // 显示的位置方式：中央、指定
  int           siSx;                   // 显示的位置 - X轴
  int           siSy;                   // 显示的位置 - Y轴
  unsigned int  uiShowBehindChar;       // 显示在人物的前方或下方
  unsigned int  uiShakeScreen;          // 是否震动画面
  unsigned int  uiShakeFrom;            // 震动画面的起始时间( 毫秒 )
  unsigned int  uiShakeTo;              // 震动画面的结束时间( 毫秒 _
  unsigned int  uiPrevMagicNum;         // 前置咒术的索引号( 0XFFFFFFFFFF 表示无前置咒术 )
  int           siPrevMagicSx;          // 前置咒术的显示位置 - X轴
  int           siPrevMagicSy;          // 前置咒术的显示位置 - Y轴
  int           siPrevMagicOnChar;      // 前置咒术显示在人物的前方或下方
  unsigned int  uiPostMagicNum;         // 後置咒术的索引号( 0XFFFFFFFF 表示无後置咒术 )
  int           siPostMagicSx;          // 後置咒术的显示位置 - X轴
  int           siPostMagicSy;          // 後置咒术的显示位置 - Y轴
  int           siPostMagicOnChar;      // 後置咒术显示在人物的前方或下方
  int			isPostDisappear;		// 咒术一般攻击完时是否马上消失
  int			ToCallMagicNo;			// 召唤术的编号
}ToCallMagic;
*/
  
ToCallMagic PROFESSION_magic[3] = { {0,0,0,1,0,0,1,1,0,0,0,65528,65485,0,-1,0,0,0,1,0},	// 单人
									{0,3,0,1,0,0,1,1,0,0,0,65528,65485,0,-1,0,0,0,1,1},	// 单排
									{0,5,0,1,0,0,1,1,0,0,0,65528,65485,0,-1,0,0,0,0,2},	// 全体
								};


#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
void PROFESSION_MAGIC_ATTAIC( int battleindex, int attackNo, int toNo, int attIdx ,int FieldAttr, int skill)
{
    int  list[SIDE_OFFSET * 2 + 1];
    int  listidx,i,j,k,z;
    int  basey, def_is_player[10],def_be_hit[10];
    int  attvalue , charahp , pethp , charaidx , petidx , charahurt,charahurt_temp;
    char szcommand[256];
	int  DefIsPlayer=0, power=0, skill_level;	
	float hp_power=0, mp_power=0, add_hp=0, add_mp=0, dec_hp=0, dec_mp=0;
	int icindex, ioindex;
	int charaindex, command, magic_type=0;
#ifdef _PROFESSION_ADDSKILL
	int decmplist[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, decmplistcount=0;
#endif
	charaindex = BATTLE_No2Index(battleindex, attackNo);
	if( !CHAR_CHECKINDEX(charaindex) )	return;

    if((z = BATTLE_MultiList(battleindex,toNo,list)) == -1)		return;
		else if(z != toNo){
			toNo = z;
// terry fix for 不设定为新的目标会有问题 2004/02/10
			CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM2,toNo);
// end
		}

	// 技能
	command = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1);

	// 技能等级
	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

	// 将魔法参数代入动画
	magic_type = analysis_profession_parameter( attIdx, skill, toNo, charaindex );
	PROFESSION_MAGIC_ATTAIC_Effect( battleindex, attackNo, list, attIdx);

	listidx = 0;
	memset(def_is_player,-1,sizeof(def_is_player));
	memset(def_be_hit,-1,sizeof(def_be_hit));
	
    // 单人攻击
	if( toNo < 20 ){
		toNo = list[0];
		listidx++;

	} else if( 20 == toNo ){	    // 右下全体
		for( i = 0 ; i < 2 ; i++ ){
			for( j = 0 ; j < 5 ; j++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[i + 2][j] ) ){
					list[listidx] = CharTable[i + 2][j];
					listidx++;
				}
			}
		}
	} else if( 21 == toNo ){	// 左上全体
		for( i = 0 ; i < 2 ; i++ ){
			for( j = 0 ; j < 5 ; j++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[i][j] ) ){
					list[listidx] = CharTable[i][j];
					listidx++;
				}
			}
		}
	} // 左上第一列 , 左上第二列 , 右下第一列 , 右下第二列
		else if( 23 == toNo || 24 == toNo || 25 == toNo || 26 == toNo ){
		basey = toNo - 23;
		for( i = 0 , j = basey - 1 ; j <= basey + 1 ; i++ , j++ ){
			if( ( 25 == toNo || 26 == toNo ) && ( j < 2 || j > 3 ) )
				continue;
			else if( ( 23 == toNo || 24 == toNo ) && ( j < 0 || j > 1 ) )
				continue;
			// 计算那些人需要加入清单
			for( k = 0 ; k < 5 ; k++ ){
				if(TRUE == BATTLE_TargetCheck( battleindex , CharTable[j][k] ) )
				{
					list[listidx] = CharTable[j][k];
					listidx++;
				}
			}
		}
	}
	qsort( list , listidx , sizeof( list[0] ) , ( FUNC )SortLoc );// 排序位置
	
	// 取熟练度
	PROFESSION_MAGIC_GET_PRACTICE( &hp_power, &mp_power, &dec_hp, &dec_mp, charaindex );


	power = hp_power;

	// 取出攻击 list
	PROFESSION_MAGIC_TOLIST_SORT( list, &listidx, charaindex );

	for( i = 0 ; i < listidx ; i++ ) {
		int defType;
		charaidx = BATTLE_No2Index( battleindex , list[i] );
		petidx   = BATTLE_getRidePet( charaidx );
		defType = CHAR_getInt(charaidx, CHAR_WHICHTYPE);
		DefIsPlayer = 0;

		if( !CHAR_CHECKINDEX(charaidx) )	continue;

		// 制作魔法特效
		if (CHAR_createCharacter( 100354, 777, 45, 45, 0, &icindex, &ioindex, 0) == TRUE){

			CHAR_setInt( icindex, CHAR_LUCK, 1);
			CHAR_setWorkInt( icindex, CHAR_WORKATTACKPOWER, power );
			CHAR_setWorkInt( icindex, CHAR_WORKOTHERDMAGE, 0 );
			
			
			// 魔法命中检定
			if( PROFESSION_MAGIC_DODGE( charaindex, charaidx, magic_type ) == 1 ){
				// MISS
				attvalue = 0;
			}else{
				// 取特殊魔法伤害值
				power = PROFESSION_MAGIC_GET_ICE_MIRROR_DAMAGE( charaindex, charaidx, command, power );
#ifdef _SUIT_ADDPART4
                if( CHAR_getWorkInt( charaidx, CHAR_WORKUNMPOWER ) > 0 ){//抵抗法师魔法的装备
					power -= power * (CHAR_getWorkInt( charaidx, CHAR_WORKUNMPOWER )/100.00);
					print("\nWORKUNMPOWER:%d", CHAR_getWorkInt( charaidx, CHAR_WORKUNMPOWER ));
				}
#endif
				// 取魔法伤害值
#ifdef _PROFESSION_ADDSKILL
				attvalue = PROFESSION_MAGIC_GET_DAMAGE( charaindex, charaidx, magic_type, power, command ); 
#else
				attvalue = PROFESSION_MAGIC_GET_DAMAGE( charaindex, charaidx, magic_type, power );
#endif
				// 改变自身状态魔法
				PROFESSION_MAGIC_CHANGE_STATUS( charaindex, attvalue, mp_power, &add_hp, &add_mp );

				// 特殊攻击
				attvalue = PROFESSION_MAGIC_CHANG_STATUS( command, battleindex, charaindex, charaidx, attvalue, i );
				def_be_hit[i] = charaidx;

#ifdef _PROFESSION_ADDSKILL
				if( command == BATTLE_COM_S_STORM ){// 暴风雨 水附体
					if( (PROFESSION_BATTLE_StatusAttackCheck( charaindex, charaidx, BATTLE_ST_WATER, 30 ) == 0 )// 状态命中检定
						|| (CHAR_getInt( charaidx, CHAR_HP ) <= 0 ) ){
	
					}else{
							int bid = BATTLE_Index2No( battleindex, charaidx );
							int watercount=0;
							if( skill_level > 8 ) watercount=5;
							else if( skill_level > 6 ) watercount=4;
							else if( skill_level > 4 ) watercount=3;
							else if( skill_level > 3 ) watercount=2;
							else watercount=1;
						    CHAR_setWorkInt( charaidx, CHAR_WORKWATER, watercount );
							BATTLE_BadStatusString( bid, BATTLE_ST_WATER );
					}
				}

				//记录被攻击方的idx
				decmplist[decmplistcount++] = list[i];
#endif
			}
			
			// 清除魔法特效
			CHAR_CharaDelete(icindex);
			
			
			charahp = CHAR_getInt( charaidx, CHAR_HP );

			// 没骑宠物
			if( -1 == petidx || CHAR_getInt(petidx, CHAR_HP) <= 0 ){
				if( ( charahp -= attvalue ) < 0 ){
					charahp = 0;
				}
				CHAR_setInt( charaidx , CHAR_HP , charahp );
				pethp = 0;
				if( 0 == PROFESSION_magic[attIdx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , attvalue , pethp);
				} else
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , attvalue , pethp);
				
			}else{
				pethp     = CHAR_getInt( petidx , CHAR_HP ); 
				charahurt = attvalue * BATTLE_CalcCharaRatio( FieldAttr , charaidx ) / 10;

				//以下的计算,若人物被打死,血不够扣的话,会将多出来的伤害再累加至宠物身上
				charahurt_temp = charahurt;
				if( ( charahp -= charahurt ) < 0 )
				{
					charahurt = charahp;
					charahp   = 0;
				}
					
				attvalue = attvalue - charahurt;
				if( ( pethp -= attvalue ) < 0)
				{
					pethp = 0;
					// 宠物没血了退出战场
					CHAR_setInt(charaidx,CHAR_RIDEPET,-1);
					BATTLE_changeRideImage(charaidx);
					CHAR_setWorkInt(charaidx,CHAR_WORKPETFALL,1);
				}
				CHAR_setInt( charaidx , CHAR_HP , charahp );
				CHAR_setInt( petidx , CHAR_HP , pethp );

				charahurt = charahurt_temp;

				if( 0 == PROFESSION_magic[attIdx].uiAttackType )	{// 单人攻击
					sprintf( szcommand , "%X|%X|%X|%X|" , toNo , list[i] , charahurt , attvalue);
				} else
					sprintf( szcommand , "%X|%X|%X|%X|" , list[i] , list[i] , charahurt , attvalue);
			}

			
			{
				//Change fix 加上这个先拿到DP值
				int aAttackList[BATTLE_ENTRY_MAX*2+1];
				aAttackList[0] = attackNo;
				aAttackList[1] = -1;
				BATTLE_AddProfit( battleindex, aAttackList);
			}
			//change fix 让替身娃娃可正常运作   (这样做所有法术类职技战斗後都拿不到DP值,不过上面先拿到,所以没关系)
			if( CHAR_getInt( charaidx, CHAR_HP ) <= 0 
				&& CHAR_getInt( charaidx, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER 
				/*&& !BattleArray[battleindex].dpbattle*/ )
				CHAR_setFlg( charaidx, CHAR_ISDIE, 1 );

			BATTLESTR_ADD( szcommand );
		} 
	}  

	sprintf( szcommand , "%X|" , 0x5711438 );
	BATTLESTR_ADD( szcommand );

#ifdef _PROFESSION_ADDSKILL
	//扣敌方mp
	if( (int)mp_power != 0 ){
		for( i=0;i<decmplistcount;i++ ){
			if( CHAR_getInt( BATTLE_No2Index( battleindex , decmplist[i] ), CHAR_WHICHTYPE ) != CHAR_TYPEPET ){
				if( CHAR_getInt( BATTLE_No2Index( battleindex , decmplist[i] ), CHAR_HP ) > 0 ){
					int mp = CHAR_getInt( BATTLE_No2Index( battleindex , decmplist[i] ), CHAR_MP);
					if( mp > 0 ){
						mp = mp_power>mp?mp:mp_power;
						sprintf( szcommand, "BD|r%X|1|0|m%X|", decmplist[i], mp );
						BATTLESTR_ADD( szcommand );
						CHAR_setInt(BATTLE_No2Index( battleindex , decmplist[i] ), CHAR_MP, CHAR_getInt( BATTLE_No2Index( battleindex , decmplist[i] ), CHAR_MP) - mp);
					}
				}
			}
		}
	}
	if( command == BATTLE_COM_S_DOOM ){ //世界末日 灭敏效果
		if( skill_level >= 10 ){
			int work=0;
			for( i=0;i<decmplistcount;i++ ){
				if( CHAR_getInt( BATTLE_No2Index( battleindex , decmplist[i] ), CHAR_HP ) > 0 ){
					CHAR_setWorkInt( BATTLE_No2Index( battleindex , decmplist[i] ), CHAR_WORKFEAR, 4 );
					BATTLE_BadStatusString( decmplist[i], BATTLE_ST_FEAR );
					sprintf( szcommand, "BD|r%X|0|6|%X|", decmplist[i], -20 );//敏降30%
					BATTLESTR_ADD( szcommand );
					// 减攻
					sprintf( szcommand, "BD|r%X|0|4|%X|", decmplist[i], -10 );
					BATTLESTR_ADD( szcommand );
					// 减防
					sprintf( szcommand, "BD|r%X|0|5|%X|", decmplist[i], -10 );
					BATTLESTR_ADD( szcommand );
				}
			}
		}
	}
#endif
	if( (int)add_hp != 0 ){
		int hp=0;

		hp = CHAR_getInt( charaindex, CHAR_HP) + add_hp;

		if( hp >= CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP) ){
			hp = CHAR_getWorkInt(charaindex, CHAR_WORKMAXHP);
			add_hp = hp - CHAR_getInt( charaindex, CHAR_HP);
		}else if ( hp <= 1 ){
			hp = 1;
			add_hp = 1 - CHAR_getInt( charaindex, CHAR_HP); 
		}
		CHAR_setInt(charaindex, CHAR_HP, hp);

		sprintf( szcommand, "BD|r%X|0|1|%X|", attackNo, (int)add_hp);
		BATTLESTR_ADD( szcommand );
	}


	if( (int)add_mp != 0 ){
		int mp=0;

		mp = CHAR_getInt( charaindex, CHAR_MP) + add_mp;

		if( mp >= CHAR_getWorkInt(charaindex, CHAR_WORKMAXMP) ){
			mp = CHAR_getWorkInt(charaindex, CHAR_WORKMAXMP);
			add_mp = mp - CHAR_getInt( charaindex, CHAR_MP);
		}else if ( mp <= 1 ){
			mp = 1;
			add_mp = 1 - CHAR_getInt( charaindex, CHAR_MP); 
		}

		CHAR_setInt(charaindex, CHAR_MP, mp);

		sprintf( szcommand, "BD|r%X|1|1|m%X|", attackNo, (int)add_mp);

		BATTLESTR_ADD( szcommand );
	}
// Terry fix 2003/11/17 修正睡眠者中了攻击性法术不会醒
	for(i=0; i<listidx; i++){
		// 如果被攻击者中了睡眠
		if(def_be_hit[i] < 0) continue;
		if(CHAR_getWorkInt(def_be_hit[i],CHAR_WORKSLEEP) > 0) {
			CHAR_setWorkInt(def_be_hit[i],CHAR_WORKSLEEP,0);
			sprintf(szcommand,"BM|%X|%X|",BATTLE_Index2No(battleindex,def_be_hit[i]),0);
			BATTLESTR_ADD(szcommand);
		}
	}
// Terry end
}


int PROFESSION_MAGIC_ATTAIC_Effect( int battleindex, int attackNo , int ToList[] , int AttackMgcNo )
{
   int     i;
   char    szcommand[256];
   int     attackindex;


   attackindex = BATTLE_No2Index( battleindex , attackNo );
   if( FALSE == CHAR_CHECKINDEX( attackindex ) )
       return 0;
#ifdef _PROFESSION_ADDSKILL
   //if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_ICE_CRACK ) //冰爆术不先秀特效
   //	   return 0;
#endif
   snprintf( szcommand , sizeof( szcommand ) , "B$|a%X|i%X|m%X|%X|%X|%X|s%X|t%X|l%X|%X|%X|%X|%X|%X|%X|o%X|o%X|o%X|s%X|%X|%X|%X|%X|" ,
			 attackNo , 5711438 , CHAR_getInt( attackindex , CHAR_MP ) ,
					PROFESSION_magic[AttackMgcNo].uiPrevMagicNum ,
					PROFESSION_magic[AttackMgcNo].uiSpriteNum ,
					PROFESSION_magic[AttackMgcNo].uiPostMagicNum ,
					PROFESSION_magic[AttackMgcNo].uiAttackType ,
					PROFESSION_magic[AttackMgcNo].uiSliceTime ,
					PROFESSION_magic[AttackMgcNo].uiShowType ,
					PROFESSION_magic[AttackMgcNo].siSx ,
					PROFESSION_magic[AttackMgcNo].siSy ,
					PROFESSION_magic[AttackMgcNo].siPrevMagicSx ,
					PROFESSION_magic[AttackMgcNo].siPrevMagicSy ,
					PROFESSION_magic[AttackMgcNo].siPostMagicSx ,
					PROFESSION_magic[AttackMgcNo].siPostMagicSy ,
					PROFESSION_magic[AttackMgcNo].siPrevMagicOnChar ,
					PROFESSION_magic[AttackMgcNo].uiShowBehindChar ,
					PROFESSION_magic[AttackMgcNo].siPostMagicOnChar ,
					PROFESSION_magic[AttackMgcNo].uiShakeScreen ,
					PROFESSION_magic[AttackMgcNo].uiShakeFrom ,
					PROFESSION_magic[AttackMgcNo].uiShakeTo ,
					PROFESSION_magic[AttackMgcNo].isPostDisappear ,
					PROFESSION_magic[AttackMgcNo].ToCallMagicNo
   );

   BATTLESTR_ADD( szcommand );

   for( i = 0 ; ToList[i] != -1 ; i++ ){
      snprintf( szcommand , sizeof( szcommand ) , "r%X|" , ToList[i] );
      BATTLESTR_ADD( szcommand );
   }

   BATTLESTR_ADD( "FF|" );

   return 0;
}

int analysis_profession_parameter( int attIdx, int skill, int toNo, int charaindex )
{
	int image_1, image_2, place, place2, shake_s_time;
	int shake_e_time, disappear, x, y, i;
	char temp[64];
	char *pszOption;
	// Robin fix profession magic_type
	//char magic[3][5]={"火","电","冰"};
	char magic[3][5]={"火","冰","电"};
	int magic_type=-1;
	

	// 取得技能参数
	if( (pszOption=PROFESSION_SKILL_getChar(skill,PROFESSION_SKILL_OPTION)) == NULL )	return -1;
	
	// 魔法属性
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",1,temp,sizeof(temp)) )	return -1;

	for( i=0; i<3; i++){
		if( (strcmp( magic[i], temp )) == 0 )
			magic_type = i+1;
	}

	switch( magic_type ){
	case 1:		
			// 加火熟练度经验值							
			PROFESSION_SKILL_LVEVEL_UP( charaindex, "PROFESSION_FIRE_PRACTICE" );			
			break;
	case 2:		
			// 加冰熟练度经验值							
			PROFESSION_SKILL_LVEVEL_UP( charaindex, "PROFESSION_ICE_PRACTICE" );			
			break;
	case 3:		
			// 加雷熟练度经验值							
			PROFESSION_SKILL_LVEVEL_UP( charaindex, "PROFESSION_THUNDER_PRACTICE" );			
			break;

	}

	// 显示的位置方式 0人物 1画面中央
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",2,temp,sizeof(temp)) )	return -1;
	place = atoi(temp);
	PROFESSION_magic[attIdx].uiShowType	= place;

	// 显示在人物的前方或下方 
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",3,temp,sizeof(temp)) )	return -1;
	place2 = atoi(temp);
	PROFESSION_magic[attIdx].uiShowBehindChar = place2;

	// 显示的位置X
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",4,temp,sizeof(temp)) )	return -1;
	x = atoi(temp);

	// 显示的位置Y
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",5,temp,sizeof(temp)) )	return -1;
	y = atoi(temp);

	// 震动画面的开始毫秒数
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",6,temp,sizeof(temp)) )	return -1;
	shake_s_time = atoi(temp);

	// 震动画面的结束毫秒数
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",7,temp,sizeof(temp)) )	return -1;
	shake_e_time = atoi(temp);
	
	// 咒术一般攻击完时是否马上消失
	memset( temp, 0, sizeof(temp) );
	if( !getStringFromIndexWithDelim(pszOption,"|",8,temp,sizeof(temp)) )	return -1;
	disappear = atoi(temp);

	// 准备动作的魔法图号
	image_1 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_1);
	PROFESSION_magic[attIdx].uiPrevMagicNum	= image_1;	// 前置动画
	
	// 战斗的魔法图号
	image_2 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_2);
	PROFESSION_magic[attIdx].uiSpriteNum	= image_2;	// 後置动画 

	// 显示在画面的 x,y座标
	PROFESSION_magic[attIdx].siSx = x;
	PROFESSION_magic[attIdx].siSy = y;
	
	// 取得攻击右边的魔法图号及座标
	PROFESSION_MAGIC_GET_IMG2( toNo, charaindex, attIdx, pszOption );

	// 地震的时间
	if( (shake_s_time>0) || (shake_e_time>0) ){
		PROFESSION_magic[attIdx].uiShakeScreen	= 1;
		PROFESSION_magic[attIdx].uiShakeFrom	= shake_s_time;
		PROFESSION_magic[attIdx].uiShakeTo		= shake_e_time;
	}else{
		PROFESSION_magic[attIdx].uiShakeScreen  = 0;
		PROFESSION_magic[attIdx].uiShakeFrom	= 0;
		PROFESSION_magic[attIdx].uiShakeTo		= 0;
	}


	return magic_type;
}


// 取熟练度得伤害力
void PROFESSION_MAGIC_GET_PRACTICE(
	float *hp_power, 
	float *mp_power, 
	float *dec_hp, 
	float *dec_mp,
	int charaindex
)
{
	int skill_level=0, critical=0, command=0;

	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

	command = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1);

	critical = RAND( 1,100);

	switch( command ){
		case BATTLE_COM_S_VOLCANO_SPRINGS:					// 火山泉
			{
				*hp_power = skill_level * 10 + 100;
				
				if( skill_level >= 10 ){ 	
					if( critical <= 25 )
						*hp_power *= 1.5;
				}else{
					if( critical <= (skill_level+12) )
						*hp_power *= 1.5;
				}

				break;
			}
		case BATTLE_COM_S_FIRE_BALL:						// 火星球
			{
				if( skill_level >= 10 )				
					*hp_power = 360;
				else if( skill_level >= 9 )
					*hp_power = 320;
				else if( skill_level >= 8 )
					*hp_power = 280;
				else if( skill_level >= 7)
					*hp_power = 260;
				else if( skill_level >= 5)
					*hp_power = 220;
				else if( skill_level >= 3)
					*hp_power = 180;
				else
					*hp_power = 160;

				break;
			}
		case BATTLE_COM_S_SUMMON_THUNDER:					// 召雷术
			{
				*hp_power = skill_level*10 + 200;
				break;
			}		
		case BATTLE_COM_S_CURRENT:							// 电流术
			{
#ifdef _PROFESSION_ADDSKILL
                if( skill_level >= 10 )				
					*hp_power = 300;
				else if( skill_level > 9 )
					*hp_power = 250;
				else if( skill_level > 7 )
					*hp_power = 200;
				else if( skill_level > 4 )
					*hp_power = 150;
				else if( skill_level > 1 )
					*hp_power = 10;
				else 
					*hp_power = 50;
#else
				*hp_power = skill_level*50 + 100;
#endif
				break;
			}
		case BATTLE_COM_S_STORM:							// 暴风雨
			{
#ifdef _PROFESSION_ADDSKILL
                if( skill_level > 9 )				
					*hp_power = 200;
				else if( skill_level > 7 )
					*hp_power = 180;
				else if( skill_level > 5 )
					*hp_power = 160;
				else if( skill_level > 3 )
					*hp_power = 140;
				else
					*hp_power = 120;
#else
				*hp_power = skill_level*10 + 100;
#endif
				break;
			}
		case BATTLE_COM_S_ICE_ARROW:						// 冰箭术
			{
				if( skill_level >= 10 )				
					*hp_power = 250;
				else
					*hp_power = skill_level*10 + 130;
				break;
			}
		case BATTLE_COM_S_ICE_CRACK:						// 冰爆术
			{
				if( skill_level >= 10 )			
					*hp_power = 400;
				else if ( skill_level == 9 )
					*hp_power = 300;
				else
					*hp_power = skill_level*10 + 210;

				break;
			}
		case BATTLE_COM_S_DOOM:								// 世界末日
			{
				if( skill_level >= 10 )				
					*hp_power = 550;
				else if( skill_level > 9 )
					*hp_power = 500;
				else if( skill_level > 8 )
					*hp_power = 450;
				else if( skill_level > 7 )
					*hp_power = 400;
				else if( skill_level > 6 )
					*hp_power = 350;
				else if( skill_level > 4 )
					*hp_power = 300;
				else if( skill_level > 2 )
					*hp_power = 250;
				else
					*hp_power = 200;
				break;
			}
		case BATTLE_COM_S_FIRE_SPEAR:						// 火龙枪
			{
				if( skill_level > 9 )				
					*hp_power = 800;
				else if( skill_level > 8 )
					*hp_power = 450;
				else if( skill_level > 7 )
					*hp_power = 400;
				else if( skill_level > 6 )
					*hp_power = 350;
				else if( skill_level > 5 )
					*hp_power = 300;
				else if( skill_level > 3 )
					*hp_power = 200;
				else
					*hp_power = 100;
				break;
			}

		case BATTLE_COM_S_BLOOD:							// 嗜血成性
			{
				int hp = CHAR_getInt( charaindex, CHAR_HP);
				if( hp > 1 )
					*hp_power = CHAR_getInt( charaindex, CHAR_HP) * (skill_level*5 +10) /100;
				else
					*hp_power = 0;

				break;
			}
		case BATTLE_COM_S_BLOOD_WORMS:						// 嗜血蛊
			{
				*hp_power = skill_level*10+20;
				break;
			}	
		case BATTLE_COM_S_SIGN:								// 一针见血
			{
#ifdef _PROFESSION_ADDSKILL
                if( skill_level >= 10 ){
					*hp_power = 200;
					*mp_power = 30;
				}else if( skill_level > 6 ){
					*hp_power = 150;
					*mp_power = 20;
				}else if( skill_level > 3 ){
					*hp_power = 100;
					*mp_power = 15;					
				}else{
					*hp_power = 50;
					*mp_power = 10;
				}
#else
				if( skill_level >= 10 ){
					*hp_power = 300;
					*mp_power = 70;
				}else if( skill_level >= 9 ){
					*hp_power = 200;
					*mp_power = 50;
				}else if( skill_level >= 7 ){
					*hp_power = 150;
					*mp_power = 30;	
				}else if( skill_level >= 4 ){				
					*hp_power = 100;
					*mp_power = 20;					
				}else{
					*hp_power = 50;
					*mp_power = 10;
				}
#endif
				break;
			}
		case BATTLE_COM_S_ENCLOSE:							// 附身术
			{
#ifdef _PROFESSION_ADDSKILL
				if( skill_level >= 10 )				
					*hp_power = 400;
				else if( skill_level > 9 )
					*hp_power = 300;
				else if( skill_level > 7 )
					*hp_power = 250;
				else if( skill_level > 4 )
					*hp_power = 200;
				else
					*hp_power = 150;
				break;
#else
				if( skill_level >= 10 )				
					*hp_power = 300;
				else if( skill_level >= 8 )
					*hp_power = 250;
				else if( skill_level >= 5 )
					*hp_power = 200;
				else
					*hp_power = 150;				
				break;
#endif
			}
		default:
			break;
	}

#ifdef _SUIT_ADDENDUM
    *hp_power += *hp_power * (CHAR_getWorkInt( charaindex, CHAR_WORKMPOWER )/100.00);
#endif
#ifdef _SUIT_ADDPART4
	if( rand()%100 < 30 )
		*hp_power += *hp_power * (CHAR_getWorkInt( charaindex, CHAR_WORKMPOWER2 )/100.00);//加强法师魔法的装备(限定30%机率)
#endif

	if( *hp_power > 0 ){
		float rand_num = RAND( 98, 102 );
		*hp_power *= rand_num / 100;
	}else{
		*hp_power = 0;
	}
}


// 选择改击数量
void PROFESSION_MAGIC_TOLIST_SORT( int *list, int *listidx, int charaindex )
{
	int j=0, get_num=0, temp[SIDE_OFFSET * 2 + 1];
	int skill_level=-1, command=-1;
	
	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

	command = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1);

	switch(command){
		case BATTLE_COM_S_CURRENT:				// 电流术
			{
#ifdef _PROFESSION_ADDSKILL
                if( skill_level > 9 )		get_num = 10;
				else if( skill_level > 8 )  get_num = 9;
				else if( skill_level > 7 )  get_num = 8;
				else if( skill_level > 6 )  get_num = 7;
				else if( skill_level > 5 )  get_num = 6;
				else if( skill_level > 4 )  get_num = 5;
				else if( skill_level > 3 )  get_num = 4;
				else if( skill_level > 2 )  get_num = 3;
				else if( skill_level > 1 )  get_num = 2;
				else						get_num = 1;
#else
				if( skill_level >= 9 )		get_num = 5;
				else if( skill_level >= 7 ) get_num = 4;
				else if( skill_level >= 5 ) get_num = 3;
				else if( skill_level >= 3 ) get_num = 2;
				else						get_num = 1;
#endif
				break;
			}
		case BATTLE_COM_S_STORM:				// 暴风雨
			{
				get_num = skill_level;
				break;
			}
		case BATTLE_COM_S_DOOM:					// 世界末日
			{
				if( skill_level >= 8 )		get_num = 10;
				else if( skill_level >= 7 ) get_num = 8;
				else if( skill_level >= 5 ) get_num = 6;
				else if( skill_level >= 3 ) get_num = 4;
				else						get_num = 2;
				break;
			}
		case BATTLE_COM_S_FIRE_SPEAR:			// 火龙枪
			{
				/*int miss_rate=0, rand_num=0;

				miss_rate = skill_level * 2 + 10;
				rand_num = RAND(0, 100);
				
				if( rand_num <= miss_rate ){
					memset(list, -1, sizeof(list));
					*listidx = 0;
					return;
				}else 
					get_num = skill_level;

				break;*/
				/*
				if( skill_level > 9 )		get_num = 10;
				else if( skill_level > 8)   get_num = 9;
				else if( skill_level > 7 )  get_num = 8;
				else if( skill_level > 6 )  get_num = 7;
				else if( skill_level > 5 )  get_num = 6;
				else if( skill_level > 4 )  get_num = 5;
				else if( skill_level > 3 )  get_num = 4;
				else if( skill_level > 2 )  get_num = 3;
				else if( skill_level > 1 )  get_num = 2;
				else						get_num = 1;
				*/
				break;
			}
		case BATTLE_COM_S_SIGN:					// 一针见血
			{
#ifdef _PROFESSION_ADDSKILL
               get_num = 10;
#else
				if( skill_level >= 10 )			get_num = 5;
				else if( skill_level >= 8 )		get_num = 4;
				else if( skill_level >= 6 )		get_num = 3;
				else if( skill_level >= 3 )		get_num = 2;
				else							get_num = 1;
#endif
				break;
			}
		case BATTLE_COM_S_CONVOLUTE:			// 回旋攻击
		case BATTLE_COM_S_FIRE_BALL:			// 火星球
			{
				int toNo=-1, toNo2=-1, battleindex=-1, i=0, count=0;
				battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

				if(battleindex == -1) return;

				toNo = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM2);
				memset( temp, -1, sizeof( temp ) );

				if( toNo == 24 )		toNo2 = 15;
				else if( toNo == 23 )	toNo2 = 10;
				else if( toNo == 25 )	toNo2 = 5;
				else if( toNo == 26 )	toNo2 = 0;
				else return;

				for( i=0; i<5; i++ ){
					if(BATTLE_TargetCheck(battleindex,toNo2+i) != FALSE){
						temp[count] = toNo2+i;
						count++;
					}	
				}

				memset(list, -1, sizeof(list));
				memcpy(	list, temp, sizeof(temp) );
	
				*listidx = count;

				return;
			}
		case BATTLE_COM_S_THROUGH_ATTACK:		// 贯穿攻击
			{
				int toNo = -1, toNo2 = -1, battleindex = -1;

				battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
				if(battleindex == -1) return;

				toNo = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM2);
				memset( temp, -1, sizeof( temp ) );
				temp[0] = toNo;

				if( toNo < (SIDE_OFFSET/2) )		toNo2= toNo + (SIDE_OFFSET/2);
				else if( toNo < (SIDE_OFFSET) )		toNo2= toNo - (SIDE_OFFSET/2);
				else if( toNo < (SIDE_OFFSET*1.5) )	toNo2= toNo + (SIDE_OFFSET/2);
				else if( toNo < (SIDE_OFFSET*2) )	toNo2= toNo - (SIDE_OFFSET/2);

				if(BATTLE_TargetCheck( battleindex, toNo2 ) != FALSE){
					if(	toNo2 > toNo ){
						temp[0] = toNo2;
						temp[1] = toNo;
					}else
						temp[1] = toNo2;

					*listidx = 2;
				}else	
					*listidx = 1;

				memset(list, -1, sizeof(list));
				memcpy(	list, temp, sizeof(temp) );
	
			}
		default: 
			return;
	}

	if( get_num == 0 ) return;

	if( get_num >= 10) get_num=10;

	if( get_num >= *listidx ) return;

	memset(temp, -1, sizeof(temp));

	while( j < get_num ){
		int rand_num=-1, out_num=-1;
		
		rand_num = RAND(0, *listidx-1);
		out_num	= list[rand_num];
		list[rand_num] = -1;

		if(out_num > 0){
			temp[j] = out_num;
			j++;
		}
	}

	memset(list, -1, sizeof(list));
	memcpy(	list, temp, sizeof(temp) );
	
	*listidx = j;

	return;
}


// 改变自身状态
void PROFESSION_MAGIC_CHANGE_STATUS( 
	int charaindex, 
	int hp_power, 
	float mp_power, 
	float *add_hp,
	float *add_mp
)
{
	int skill_level=0, command=0, critical=0;
	critical = RAND( 1,100);

	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

	command = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1);

	switch( command ){
		case BATTLE_COM_S_BLOOD:							// 嗜血成性
			{
				int rate = 0;

				if( skill_level >= 10 )		rate = 20;
				else if( skill_level >= 7 ) rate = 15;
				else if( skill_level >= 5 ) rate = 10;
				else if( skill_level >= 3 ) rate = 5;
				else						rate = 0;

				*add_mp += hp_power * ( rate + 40 ) / 100;	

				break;
			}	
		case BATTLE_COM_S_BLOOD_WORMS:						// 嗜血蛊
			{
				if( skill_level >= 10 ){
					*add_hp += hp_power * 0.2;
					break;
				}else{
					*add_hp += hp_power * ((skill_level/4)*5 + 5) / 100;
					break;
				}
				break;
			}
#ifdef _PROFESSION_ADDSKILL
		case BATTLE_COM_S_SIGN:								// 一针见血
			{
				int success=10;
                /*( skill_level >= 10 )		success = 30;//成功率
				else if( skill_level > 9 )  success = 25;
				else if( skill_level > 8 )  success = 20;
				else if( skill_level > 7 )  success = 15;
				else						success = 10;
				success = 30;*/
				if( RAND(0,100) < success ){
					if( skill_level > 8 ){
						*add_hp += hp_power;
						*add_mp += mp_power;
					}
					else if( skill_level > 7 ){
						*add_hp += hp_power/2;
						*add_mp += 0;
					}
				}
				else
				{
					*add_hp += 0;
					*add_mp += 0;
				}
				break;
			}
#endif
		default:
			break;
	}
}

// 取得位置不是在中央的多人魔法图号及座标
void PROFESSION_MAGIC_GET_IMG2( int toNo, int charaindex, int attIdx, char *pszOption )
{
	int command=0, img2=0, x=0, y=0;
	char temp[64];
	int skill_level=0; 
		
	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

	command = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1);

	switch( command ){	
		case BATTLE_COM_S_BLOOD:						// 嗜血成性
			{
				int img=0;	
				
				if( skill_level >= 10 )		img = 101689;
				else if( skill_level >= 7 ) img = 101690; 
				else if( skill_level >= 3 ) img = 101691; 
				else						img = 101692;

				PROFESSION_MAGIC_CHANG_IMG2( img, pszOption, attIdx );
				return;	
			}
		case BATTLE_COM_S_VOLCANO_SPRINGS:					// 火山泉
			{
				int img=0;
				
				if( skill_level >= 10 )		img = 101686;
				else if( skill_level >= 5 ) img = 101687; 
				else						img = 101688;

				if( toNo >= 0 && toNo < 10 )
					PROFESSION_MAGIC_CHANG_IMG2( img, pszOption, attIdx );
				else{
					int x=0, y=0;
					char temp[128];

					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",11,temp,sizeof(temp)) ) x = atoi(temp);

					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",12,temp,sizeof(temp)) ) y = atoi(temp);

					// 後置动画 
					PROFESSION_magic[attIdx].uiSpriteNum = img;
					
					// 後置动画座标
					PROFESSION_magic[attIdx].siSx = x;
					PROFESSION_magic[attIdx].siSy = y;
				}

				return;	
			}
		case BATTLE_COM_S_BLOOD_WORMS:						// 嗜血蛊
			{
				// 右方
				if( toNo == 20 || toNo == 25 || toNo == 26 ||
				  ( toNo >= 0 && toNo < SIDE_OFFSET ) )
					PROFESSION_MAGIC_CHANG_IMG2( 101623, pszOption, attIdx );
				return;
			}
		case BATTLE_COM_S_ICE_MIRROR:						// 冰镜术
			{
				if( toNo >= 0 && toNo < 10 )
					PROFESSION_MAGIC_CHANG_IMG2( 101652, pszOption, attIdx );
				else{
					int x=0, y=0;
					char temp[128];

					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",11,temp,sizeof(temp)) ) x = atoi(temp);

					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",12,temp,sizeof(temp)) ) y = atoi(temp);

					// 後置动画 
					PROFESSION_magic[attIdx].uiSpriteNum = 101652;
					
					// 後置动画座标
					PROFESSION_magic[attIdx].siSx = x;
					PROFESSION_magic[attIdx].siSy = y;
				}

				return;
			}
		case BATTLE_COM_S_ENCLOSE:							// 附身术
			{
				// 右方
				if( toNo == 20 || toNo == 25 || toNo == 26 ||
				  ( toNo >= 0 && toNo < SIDE_OFFSET ) )
					PROFESSION_MAGIC_CHANG_IMG2( 101643, pszOption, attIdx );
				return;
			}
		case BATTLE_COM_S_FIRE_SPEAR:						// 火龙枪
			{
				// 右方
				//if( toNo == 20 || toNo == 25 || toNo == 26 )
				if( toNo < 10 )
					PROFESSION_MAGIC_CHANG_IMG2( 101642, pszOption, attIdx );
				return;
			}
		case BATTLE_COM_S_DOOM:								// 世界末日
			{
				// 右方
				if( toNo == 20 || toNo == 25 || toNo == 26 )
					PROFESSION_MAGIC_CHANG_IMG2( 101639, pszOption, attIdx );
				return;				
			}
		case BATTLE_COM_S_ICE_CRACK:						// 冰爆术
			{
				// 右方
				if( toNo == 20 || toNo == 25 || toNo == 26 )
					PROFESSION_MAGIC_CHANG_IMG2( 101650, pszOption, attIdx );
				return;
			}
		case BATTLE_COM_S_ICE_ARROW:						// 冰箭术
			{
				// 右方
				if( toNo == 20 || toNo == 25 || toNo == 26 ||
				    ( toNo >= 0 && toNo < SIDE_OFFSET ) )
					PROFESSION_MAGIC_CHANG_IMG2( 101649, pszOption, attIdx );
				return;

			}
		case BATTLE_COM_S_STORM:							// 暴风雨
			{
				// 右方
				if( toNo == 20 || toNo == 25 || toNo == 26 )
					PROFESSION_MAGIC_CHANG_IMG2( 101677, pszOption, attIdx );
				return;
			}
#ifdef _PROFESSION_ADDSKILL
       case BATTLE_COM_S_BOUNDARY:							// 四属性结界
			{
				// 右方
				if( toNo == 20 || toNo == 25 || toNo == 26 ){
					if( getStringFromIndexWithDelim(pszOption,"|",1,temp,sizeof(temp)) )
					{
						if( strcmp( "地结界", temp ) == 0 )
					        PROFESSION_MAGIC_CHANG_IMG2( 101786, pszOption, attIdx );
					    else if( strcmp( "水结界", temp ) == 0 )
							PROFESSION_MAGIC_CHANG_IMG2( 101774, pszOption, attIdx );
					    else if( strcmp( "火结界", temp ) == 0 )
							PROFESSION_MAGIC_CHANG_IMG2( 101780, pszOption, attIdx );
					    else if( strcmp( "风结界", temp ) == 0 )
							PROFESSION_MAGIC_CHANG_IMG2( 101792, pszOption, attIdx );
						else
							PROFESSION_MAGIC_CHANG_IMG2( 101770, pszOption, attIdx );
					}
				}
				return;
			}
#endif
		case BATTLE_COM_S_CONVOLUTE:						// 回旋攻击
			{
				img2=101656;				
				if( toNo == 25 ){					// 右 1
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",9,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",10,temp,sizeof(temp)) ) y = atoi(temp);						
				}else if( toNo == 26 ){				// 右 2
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",11,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",12,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( toNo == 23 ){				// 左 1
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",13,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",14,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( toNo == 24 ){				// 左 2
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",15,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",16,temp,sizeof(temp)) ) y = atoi(temp);
				}else
					return;

				// 後置动画
				PROFESSION_magic[attIdx].uiSpriteNum = img2;

				// 後置动画座标
				PROFESSION_magic[attIdx].siSx = x;
				PROFESSION_magic[attIdx].siSy = y;

				break;
			}
		case BATTLE_COM_S_THROUGH_ATTACK:					// 贯穿攻击
			{	
				if( (toNo == 13) || (toNo == 18) ){			// 左下 1
					img2=101676;
					x = 50;
					y = 240;
				}else if( (toNo == 11) || (toNo == 16) ){	// 左 2
					img2=101675;
					x = 120;
					y = 200;
				}else if( (toNo == 10) || (toNo == 15) ){	// 左 3
					img2=101674;
					x = 140;
					y = 160;
				}else if( (toNo == 12) || (toNo == 17) ){	// 左 4
					img2=101673;
					x = 160;
					y = 120;
				}else if( (toNo == 14) || (toNo == 19) ){	// 左 5
					img2=101672;
					x = 180;
					y = 80;
				}else if( (toNo == 3) || (toNo == 8) ){		// 右 1
					img2=101665;
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",9,temp,sizeof(temp)) ) x = atoi(temp);
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",10,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( (toNo == 1) || (toNo == 6) ){		// 右 2
					img2=101664;
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",11,temp,sizeof(temp)) ) x = atoi(temp);
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",12,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( (toNo == 0) || (toNo == 5) ){		// 右 3
					img2=101663;
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",13,temp,sizeof(temp)) ) x = atoi(temp);
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",14,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( (toNo == 2) || (toNo == 7) ){		// 右 4
					img2=101662;
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",15,temp,sizeof(temp)) ) x = atoi(temp);
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",16,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( (toNo == 4) || (toNo == 9) ){		// 右 5
					img2=101661;
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",17,temp,sizeof(temp)) ) x = atoi(temp);
					memset( temp, 0, sizeof(temp) );
					if( getStringFromIndexWithDelim(pszOption,"|",18,temp,sizeof(temp)) ) y = atoi(temp);
				}else 
					return;

				// 後置动画
				PROFESSION_magic[attIdx].uiSpriteNum = img2;
				
				// 後置动画座标
				PROFESSION_magic[attIdx].siSx = x;
				PROFESSION_magic[attIdx].siSy = y;

				// 前置动画
				if( toNo >= 10 ){
					int img1 = 101671;
					PROFESSION_magic[attIdx].uiPrevMagicNum	= img1;	
				}
				break;
			}
		case BATTLE_COM_S_FIRE_BALL:						// 火星球
			{
				if( toNo == 25 ){					// 右 1
						img2=101694;

						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",9,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",10,temp,sizeof(temp)) ) y = atoi(temp);						
				}else if( toNo == 26 ){				// 右 2
						img2=101694;

						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",11,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",12,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( toNo == 23 ){				// 左 1
						img2=101693;

						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",13,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",14,temp,sizeof(temp)) ) y = atoi(temp);
				}else if( toNo == 24 ){				// 左 2
						img2=101693;

						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",15,temp,sizeof(temp)) ) x = atoi(temp);
						memset( temp, 0, sizeof(temp) );
						if( getStringFromIndexWithDelim(pszOption,"|",16,temp,sizeof(temp)) ) y = atoi(temp);
				}else
					return;

				// 後置动画
				PROFESSION_magic[attIdx].uiSpriteNum = img2;

				// 後置动画座标
				PROFESSION_magic[attIdx].siSx = x;
				PROFESSION_magic[attIdx].siSy = y;

				break;
			}
		default:	return;
	}

}


#define D_16	(1.0/16)
#define D_8		(1.0/8)
// 取得攻击伤害
#ifdef _PROFESSION_ADDSKILL
int PROFESSION_MAGIC_GET_DAMAGE( int attackindex, int defindex, int magic_type, int power, int command )
#else
int PROFESSION_MAGIC_GET_DAMAGE( int attackindex, int defindex, int magic_type, int power )
#endif
{
	int attack=0;
	int damage = 0, rand_num=0;
	int fire_proficiency = -1, electric_proficiency = -1, ice_proficiency = -1;//熟练度
	int fire_resist = -1, electric_resist = -1, ice_resist = -1;
	
#ifdef _FIX_MAGIC_RESIST			// WON ADD 修正魔法抗性	
    int proficiency=0;//熟练度
	int resist=0;//抗性
	int suit=0;//装备
	int spirit=0;//使用精灵造成的抗性

	if( command == BATTLE_COM_S_DOOM )
		magic_type = 1;

    if( magic_type == 1){	// 火属性
	    proficiency = CHAR_getWorkInt( attackindex, CHAR_WORK_F_PROFICIENCY);	// 火熟练度
	    resist = CHAR_getWorkInt( defindex, CHAR_WORK_F_RESIST);		// 火抗
#ifdef _EQUIT_RESIST
		suit = CHAR_getWorkInt( defindex, CHAR_WORK_F_SUIT)+CHAR_getWorkInt( defindex, CHAR_WORKEQUITFIRE );
#else
		suit = CHAR_getWorkInt( defindex, CHAR_WORK_F_SUIT);
#endif
#ifdef _MAGICSTAUTS_RESIST
		if( CHAR_getWorkInt( defindex, CHAR_MAGICFIRE) > 0 )
			spirit = CHAR_getWorkInt( defindex, CHAR_OTHERSTATUSNUMS );
#endif
	}
		
	if( command == BATTLE_COM_S_DOOM ){
		damage = power * (1+proficiency/100.0) * (1-(resist/100.0)) * (1-(suit/100.0)) * (1-(spirit/100.0));
		magic_type = 2;
	}

	if( magic_type == 2){	// 电属性
	    proficiency = CHAR_getWorkInt( attackindex, CHAR_WORK_T_PROFICIENCY);	// 电熟练度
	    resist = CHAR_getWorkInt( defindex, CHAR_WORK_T_RESIST);	// 电抗
#ifdef _EQUIT_RESIST
		suit = CHAR_getWorkInt( defindex, CHAR_WORK_I_SUIT)+CHAR_getWorkInt( defindex, CHAR_WORKEQUITTHUNDER );
#else
		suit = CHAR_getWorkInt( defindex, CHAR_WORK_I_SUIT);
#endif
#ifdef _MAGICSTAUTS_RESIST
		if( CHAR_getWorkInt( defindex, CHAR_MAGICTHUNDER) > 0 )
			spirit = CHAR_getWorkInt( defindex, CHAR_OTHERSTATUSNUMS );
#endif
	}
	
	if( command == BATTLE_COM_S_DOOM ){
		damage += power * (1+proficiency/100.0) * (1-(resist/100.0)) * (1-(suit/100.0)) * (1-(spirit/100.0));
		magic_type = 3;
	}

	if( magic_type == 3){	// 冰属性
	    proficiency = CHAR_getWorkInt( attackindex, CHAR_WORK_I_PROFICIENCY);	// 冰熟练度
	    resist = CHAR_getWorkInt( defindex, CHAR_WORK_I_RESIST);		// 冰抗
#ifdef _EQUIT_RESIST
		suit = CHAR_getWorkInt( defindex, CHAR_WORK_T_SUIT)+CHAR_getWorkInt( defindex, CHAR_WORKEQUITICE );
#else
		suit = CHAR_getWorkInt( defindex, CHAR_WORK_T_SUIT);
#endif
#ifdef _MAGICSTAUTS_RESIST
		if( CHAR_getWorkInt( defindex, CHAR_MAGICICE) > 0 )
			spirit = CHAR_getWorkInt( defindex, CHAR_OTHERSTATUSNUMS );
#endif
	}

	if( command == BATTLE_COM_S_DOOM ){
		damage += power * (1+proficiency/100.0) * (1-(resist/100.0)) * (1-(suit/100.0)) * (1-(spirit/100.0));
		damage /= 3.0;
	}
    else
		damage = power * (1+proficiency/100.0) * (1-(resist/100.0)) * (1-(suit/100.0)) * (1-(spirit/100.0));

#else //_FIX_MAGIC_RESIST

	fire_proficiency = CHAR_getWorkInt( attackindex, CHAR_WORK_F_PROFICIENCY);		// 火熟练度
	electric_proficiency = CHAR_getWorkInt( attackindex, CHAR_WORK_T_PROFICIENCY);	// 电熟练度
	ice_proficiency = CHAR_getWorkInt( attackindex, CHAR_WORK_I_PROFICIENCY);		// 冰熟练度
	fire_resist = CHAR_getWorkInt( defindex, CHAR_WORK_F_RESIST);		// 火抗
	electric_resist = CHAR_getWorkInt( defindex, CHAR_WORK_T_RESIST);	// 电抗
	ice_resist = CHAR_getWorkInt( defindex, CHAR_WORK_I_RESIST);		// 冰抗
	
//	print("\n won test 0.0 ==> f(%d) e(%d) i(%d)", fire_proficiency, electric_proficiency, ice_proficiency );
//	print("\n won test 0.1 ==> f(%d) e(%d) i(%d)", fire_resist, electric_resist, ice_resist );

	if( magic_type == 1){	// 火属性
	
		attack = power * ( 100 + fire_proficiency ) / 100;					// 攻击加成

		if( rand_num < 40){		// 原攻击伤害
			attack = attack * ( 100 - fire_resist ) / 100;
		}else{					// 乱数取伤害
			rand_num = RAND(-20, 20);
			attack = attack * ( 100 - (rand_num + fire_resist) ) / 100;		
		}
//print( "\nattack:%d ", attack ); 

	}else 
	if( magic_type == 2){	// 电属性

		attack = power * ( 100 + electric_proficiency ) / 100;					// 攻击加成

		if( rand_num < 40){		// 原攻击伤害
			attack = attack * ( 100 - electric_resist ) / 100;
		}else{					// 乱数取伤害
			rand_num = RAND(-20, 20);
			attack = attack * ( 100 - (rand_num + electric_resist) ) / 100;		
		}
	}else 
	if( magic_type == 3){	// 冰属性

		attack = power * ( 100 + ice_proficiency ) / 100;					// 攻击加成

		if( rand_num < 40){		// 原攻击伤害
			attack = attack * ( 100 - ice_resist ) / 100;
		}else{					// 乱数取伤害
			rand_num = RAND(-20, 20);
			attack = attack * ( 100 - (rand_num + ice_resist) ) / 100;		
		}
	}else{					// 无
		attack = power;
	}

	damage = attack;

#endif

	if( damage < 0 )
		damage = 0;

	return damage;
}


// 取得特殊攻击伤害
int PROFESSION_MAGIC_GET_ICE_MIRROR_DAMAGE( int attackindex, int defindex, int command, int power )
{
	int damage = power;

	if( command == BATTLE_COM_S_ICE_MIRROR ){	// 冰镜术
		float defense = -1;
		int rate=0;
		int defpet = BATTLE_getRidePet( defindex );
		int skill_level = CHAR_GETWORKINT_HIGH( attackindex, CHAR_WORKBATTLECOM3);
		int base_defense = CHAR_getInt( defindex, CHAR_TOUGH ) / 100;

		skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

		if( defpet == -1 )
			defense = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER );
		else
			defense = BATTLE_adjustRidePet3A( defindex, defpet, CHAR_WORKDEFENCEPOWER, DEFFENCESIDE );

		if( skill_level >= 10 ){
			rate = 60;
		}else{
			rate = skill_level * 5 + 5;
		}

		//andy_Edit
		damage = 120 + (int)( ( defense * rate / 100 ) + ( ( defense - base_defense ) * rate / 200 ) );
		//控制对npc的伤害
		if( CHAR_getInt( defense , CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )
			damage = damage > 800 ? 800 : damage;

	}else
	if( command == BATTLE_COM_S_CONVOLUTE ){	// 回旋攻击
		int skill_level = CHAR_GETWORKINT_HIGH( attackindex, CHAR_WORKBATTLECOM3);	
		int hit=0;
		int rand_num = RAND( 1, 100);

		skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

		hit = skill_level * 2 + 60;

		//if( rand_num <= hit ){
			// 取得回旋攻击伤害
			damage = BATTLE_PROFESSION_CONVOLUTE_GET_DAMAGE( attackindex, defindex, skill_level );
		//}else{
		//	damage = 0;
		//}
	}else
	if( command == BATTLE_COM_S_THROUGH_ATTACK ){	// 贯穿攻击
		int skill_level = CHAR_GETWORKINT_HIGH( attackindex, CHAR_WORKBATTLECOM3);	

		skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

		if( skill_level != 10 ){
			// 命中率下降
			CHAR_setWorkInt( attackindex, CHAR_MYSKILLHIT, 1);
			CHAR_setWorkInt( attackindex, CHAR_MYSKILLHIT_NUM, -70);
			CHAR_setWorkInt( attackindex, CHAR_WORKHITRIGHT, CHAR_getWorkInt(attackindex,CHAR_WORKHITRIGHT) -50 );
		}

		// 取得贯穿攻击伤害
		damage = BATTLE_PROFESSION_THROUGH_ATTACK_GET_DAMAGE( attackindex, defindex );
	}
#ifdef _PROFESSION_ADDSKILL
	else
	if( command == BATTLE_COM_S_CURRENT //电流术
		|| command == BATTLE_COM_S_SUMMON_THUNDER ){//召雷术
		if( CHAR_getWorkInt( defindex, CHAR_WORKWATER ) > 0 ){//水附体
			if( RAND(1, 100) < 75 ) 
				damage *= 3;
		}
	}
#endif
	return damage;
}



// 计算闪避率
// return: 0:没闪过 1:闪过
int PROFESSION_MAGIC_DODGE( int atk_index, int def_index, int magic_type )
{
	float fLuck = 0,fResist = 0, proficiency = 0;
	float Dluck=0.0;
	int charType = CHAR_getInt( def_index, CHAR_WHICHTYPE);
	int rand_num = RAND( 1, 100 );
    int command;


	// 目标地球一周，不动作
	if( CHAR_getWorkInt( def_index, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_EARTHROUND0 ){
		return 1;	// Miss
	}

	// 如果是防守者是玩家
	if( charType == CHAR_TYPEPLAYER ){

		// 幸运
		fLuck = (float)CHAR_getInt( def_index, CHAR_LUCK) * 3;

		// 抗性
		if( magic_type != 0 ){
			fResist = (float)(CHAR_getWorkInt( def_index, CHAR_WORK_F_RESIST + magic_type - 1 )) * 0.5;
		}
		fLuck += fResist;

		// 抗魔装备
		Dluck = (float)(CHAR_getWorkInt( def_index, CHAR_EQUITQUIMAGIC)*0.4);

	    fLuck += Dluck;

	}else {// 如果是防守者是宠物
		fLuck = (float)CHAR_getInt( def_index, CHAR_LV) * 0.15;
		if(fLuck > 20) fLuck = 20;
	}

	// 熟练度
	// Robin fix
	//if( magic_type != 0 ){
	if( magic_type > 0 ){
		proficiency = (float)CHAR_getInt( atk_index, (CHAR_WORK_F_PROFICIENCY + magic_type - 1) ) * 0.2;
		fLuck -= proficiency;
	}

/*	print("\n PROFESSION_MAGIC_DODGE atk=%s def=%s magic_type(%d)",
			CHAR_getChar( atk_index, CHAR_NAME),
			CHAR_getChar( def_index, CHAR_NAME),
			magic_type );
	print("\n won test 4 ==> rand_num(%d) lucky(%d) fResist(%f) Dluck(%f) proficiency(%f) fLuck(%f)", 
			rand_num,	
			CHAR_getInt( def_index, CHAR_LUCK) * 3, 
			fResist,
			Dluck,
			proficiency,
			fLuck );
*/
	if( rand_num > (int)fLuck){
#ifdef _PROFESSION_ADDSKILL
		command = CHAR_getWorkInt( atk_index, CHAR_WORKBATTLECOM1);
		switch( command ){
			case BATTLE_COM_S_CURRENT:
			case BATTLE_COM_S_STORM:	
				if( RAND( 1, 100 ) < 75 )
					return 0;
				else
					return 1;
				break;
			case BATTLE_COM_S_FIRE_SPEAR:
			case BATTLE_COM_S_DOOM:
				if( RAND( 1, 100 ) < 90 )
					return 0;
				else
					return 1;
				break;	
			case BATTLE_COM_S_SIGN:
				if( RAND( 1, 100 ) < 50 )
					return 0;
				else
					return 1;
				break;
		}
#endif
		return 0;	// hit
	}
	else						
		return 1;	// Miss
}

void PROFESSION_MAGIC_CHANG_IMG2( int img2, char *pszOption, int attIdx )
{
	int x=0, y=0;
	char temp[128];

	memset( temp, 0, sizeof(temp) );
	if( getStringFromIndexWithDelim(pszOption,"|",9,temp,sizeof(temp)) ) x = atoi(temp);

	memset( temp, 0, sizeof(temp) );
	if( getStringFromIndexWithDelim(pszOption,"|",10,temp,sizeof(temp)) ) y = atoi(temp);						

	// 後置动画 
	PROFESSION_magic[attIdx].uiSpriteNum = img2;
	
	// 後置动画座标
	PROFESSION_magic[attIdx].siSx = x;
	PROFESSION_magic[attIdx].siSy = y;
}



int PROFESSION_MAGIC_CHANG_STATUS(int command,int battleindex, int charaindex, int charaidx, int attvalue, int no )
{
	int i,j;
	int skill_level=0;

	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);	
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

	switch( command ){
		case BATTLE_COM_S_THROUGH_ATTACK:					// 贯穿攻击
			{
				int skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);	

				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

				if( no == 0 ){			// 攻击前排的
					attvalue = attvalue * (skill_level * 2 + 70) / 100;
				}else if( no == 1 ){	// 攻击後排的
					attvalue = attvalue * (skill_level * 2 + 50) / 100;
				}

				break;
			}
		case BATTLE_COM_S_ICE_CRACK:						// 冰爆术
			{
				int hit=0, rand_num = RAND( 0, 100 );

				//for( j = 1; j < BATTLE_ST_END; j++ ){		//若已有异常状态则return
    			//	if( CHAR_getWorkInt( charaidx, StatusTbl[j] ) > 0 ) return 0;
				//} 
				// 命中率
				if( skill_level >= 10 ) hit = 80;
				else if( skill_level >= 7 ) hit = 70;
				else if( skill_level >= 5 ) hit = 80;
				else if( skill_level >= 3 ) hit = 90;
				else						hit = 100;
				
				hit = 100;
				if(  rand_num < hit ){
					int damage=0;
					int bid = BATTLE_Index2No( battleindex, charaidx );
									
					if( skill_level >= 10 )		damage = attvalue * 1.3;
					else if( skill_level >= 7 ) damage = attvalue * 1.25;
					else if( skill_level >= 4 ) damage = attvalue * 1.2;
					else						damage = attvalue * 1.1;
									
					if( damage <= 0 ) break;

					for( i=0; i<10; i++ ){
						if( CHAR_getWorkInt( charaidx, CHAR_WORKICECRACK+i ) <= 0 ){
							CHAR_setWorkInt( charaidx, CHAR_WORKICECRACK+i, 3 );
							CHAR_setWorkInt( charaidx, CHAR_WORKMODICECRACK+i, damage );
							print("\niceidx:%d",charaidx);
							break;
						}
					}
					//BATTLE_BadStatusString( bid, BATTLE_ST_ICECRACK );

				}	

				// 冰爆术第一击无伤害
				attvalue = 0;
				
				break;
			} 
		case BATTLE_COM_S_ENCLOSE:							// 附身术
			{
				for( j = 1; j < BATTLE_ST_END; j++ )		//若已有异常状态则return
    				if( CHAR_getWorkInt( charaidx, StatusTbl[j] ) > 0 ) return attvalue;
				
				{
					int success=0, rand_num=0, round=1;
					int bid = BATTLE_Index2No( battleindex, charaidx );

					skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);

					if( skill_level >= 100 )		success = 50;
					else if( skill_level > 90 ) success = 40;
					else if( skill_level > 80 ) success = 30;
					else if( skill_level > 60 ) success = 25;
					else if( skill_level > 30 ) success = 20;
					else						success = 10;	
					
					if( skill_level > 80 ) round = 3;
					else if( skill_level > 50 ) round = 2;
					else round = 1;
					
					rand_num = RAND(0,100);
									
					if( rand_num <= success ){		//成功时目标混乱
#ifdef _PROFESSION_ADDSKILL
                        CHAR_setWorkInt( charaidx, CHAR_WORKANNEX, round );
						print("\n附身round:%d",round);
#else
						CHAR_setWorkInt( charaidx, CHAR_WORKCONFUSION, 4 );
						CHAR_setWorkInt( charaidx, CHAR_WORKMODCONFUSION, 100 );
#endif
						BATTLE_BadStatusString( bid, BATTLE_ST_CONFUSION );
					}							
				}
				break;
			}
		case BATTLE_COM_S_ICE_ARROW:						// 冰箭
			{
				for( j = 1; j < BATTLE_ST_END; j++ ){		//若已有异常状态则return
    				if( CHAR_getWorkInt( charaidx, StatusTbl[j] ) > 0 ) return attvalue;
				}
				{
					int success=0, rand_num=0;
					int bid = BATTLE_Index2No( battleindex, charaidx );

					if( skill_level >= 8 )		success = 25;
					else if( skill_level >= 5 ) success = 20;
					else if( skill_level >= 2 ) success = 15;					
					else						success = 10;	
					rand_num = RAND(0,100);
									
					if( rand_num <= success ){		//成功时目标降敏
						int dec_dex=0, turn=0;

						if( skill_level >= 8 )		dec_dex = 25;
						else if( skill_level >= 5 ) dec_dex = 20;
						else						dec_dex = 10;

						if( skill_level >= 10 )		turn = 3;
						else if( skill_level >= 6 ) turn = 2;
						else						turn = 1;

						CHAR_setWorkInt( charaidx, CHAR_WORKICEARROW, turn + 1 );
						CHAR_setWorkInt( charaidx, CHAR_WORKMODICEARROW, dec_dex );

						BATTLE_BadStatusString( bid, BATTLE_ST_ICEARROW );
					}	
				}
				break;
			}
		case BATTLE_COM_S_BLOOD_WORMS:						// 嗜血蛊
			{
				for( j = 1; j < BATTLE_ST_END; j++ ){		//若已有异常状态则return
    				if( CHAR_getWorkInt( charaidx, StatusTbl[j] ) > 0 ) return attvalue;
				}
				{
					int turn=0;
					int bid = BATTLE_Index2No( battleindex, charaidx );

					if( skill_level >= 10 )		turn = 5;
					else if( skill_level >= 8 ) turn = 4;
					else if( skill_level >= 5 ) turn = 3;
					else						turn = 2;	

					CHAR_setWorkInt( charaidx, CHAR_WORKBLOODWORMS, turn + 1 );
					CHAR_setWorkInt( charaidx, CHAR_WORKMODBLOODWORMS, skill_level );
					CHAR_setWorkInt( charaidx, CHAR_WORKBLOODWORMSID, charaindex );

					BATTLE_BadStatusString( bid, BATTLE_ST_BLOODWORMS );
							
				}
				break;
			}
		case BATTLE_COM_S_SIGN:								// 一针见血
			{
#ifdef _PROFESSION_ADDSKILL
#else
				for( j = 1; j < BATTLE_ST_END; j++ ){		//若已有异常状态则return
    				if( CHAR_getWorkInt( charaidx, StatusTbl[j] ) > 0 ) return attvalue;
				}
				{
					int turn=0, success=0, rand_num=0;
					int bid = BATTLE_Index2No( battleindex, charaidx );
		
					// 非玩家离开
//					if( CHAR_getInt( charaidx, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )  return attvalue;

					if( skill_level == 10 )		success = 35;
					else if( skill_level >= 8 ) success = (skill_level - 3) * 5;
					else if( skill_level >= 6 ) success = 20;
					else if( skill_level >= 4 ) success = 15;
					else if( skill_level >= 2 ) success = 10;
					else						success = 5;	

					rand_num = RAND(0,100);
									
					if( rand_num <= success ){		//成功时目标吸血
						if( skill_level >= 10 )		turn = 5;
						else if( skill_level >= 7 ) turn = 4;
						else if( skill_level >= 4 ) turn = 3;
						else						turn = 2;	

						CHAR_setWorkInt( charaidx, CHAR_WORKSIGN, turn + 1 );
						CHAR_setWorkInt( charaidx, CHAR_WORKMODSIGN, skill_level );
						CHAR_setWorkInt( charaidx, CHAR_WORKSIGNID, charaindex );

						BATTLE_BadStatusString( bid, BATTLE_ST_SIGN );
					}  
				}  
#endif
				break;
			}
		case BATTLE_COM_S_DOOM:				// 世界末日	
			{
/*				int rand_num = RAND( 1, 100 ), work, dex;
				char szCommand[64];
				int bid = BATTLE_Index2No( battleindex, charaidx );

				if( skill_level < 10 ) break;
#ifdef _PROFESSION_ADDSKILL
				CHAR_setWorkInt( charaidx, CHAR_WORKFEAR, 2 );
				BATTLE_BadStatusString( bid, BATTLE_ST_FEAR );
				work = CHAR_getWorkInt( charaindex, CHAR_WORKQUICK )+20;
				dex = work>>1;//敏降50%
				sprintf( szCommand, "BD|r%X|0|6|%X|", bid, (-1) * dex );
				BATTLESTR_ADD( szCommand );
#endif
				// 中麻痹
				/*if( rand_num <= 20 ){
					CHAR_setWorkInt( charaidx, CHAR_WORKPARALYSIS, 3 );
					BATTLE_BadStatusString( bid, BATTLE_ST_PARALYSIS );
				}else{
					rand_num = RAND( 1, 100 );	
					if( rand_num <= 10 ){
						CHAR_setWorkInt( charaidx, CHAR_WORKCONFUSION, 3 );
						BATTLE_BadStatusString( bid, BATTLE_ST_CONFUSION );
					}
				}*/

				break;
			}
	}
 	

	return attvalue;
}


#endif


#ifdef _PETSKILL_FIREKILL
/*
     . ..·. ..··. .○
     *. ..· ★.* ·. .
   ★  .  ·● ● ·. .★. 
     ..· /  \_/  \_ ·. 
       ??√√ √√??? 
        中秋节快乐 
*/
void BATTLE_MultiAttMagic_Fire( int battleindex, int attackNo, int defNo,
						  int FieldAttr , int Power)

{
    int  list[SIDE_OFFSET * 2 + 1];
    int  listidx=0,i,j,z,def_be_hit[10],att_magic_lv[4];
	int  def_magic_resist[DEF_MAGIC_NUM];
    int  attvalue , charahp , pethp , charaidx , petidx , charahurt,pet_att_lv = 1,pet_def_lv = 1,charahurt_temp;
    char szcommand[256];
	int  AttIsPlayer=0,DefIsPlayer=0;
	int getexp = 0;

#ifndef _FIX_MAGICDAMAGE
	int att_magic_exp_add,att_magic_exp_sub,def_magic_exp_add,def_magic_exp_sub;
	int lv_up_exp,DefFieldAttr = 0;
	float temp = 0.0f;
//	int attattr[5], defattr[5];
	char msgbuf[64];
	char kind[4][3] = {"地","水","火","风"};
#else
	BOOL TrueMagic=FALSE;
#endif
	int f_num=0;
	int MagicLv=4;
	
	memset(def_be_hit,-1,sizeof(def_be_hit));

	if(defNo < 5)
		f_num = 0;
	else if(defNo >=5 && defNo < 10)
		f_num = 5;
	else if(defNo >=10 && defNo < 15)
		f_num = 10;
	else 
		f_num = 15;

	// 取出战场上存活的人
	for( i=f_num; i<f_num+5; i++ ){			
		if( BATTLE_TargetCheck(battleindex, i) != FALSE ){
		    list[listidx]	= i;
			listidx++;
		}
	}
	
    sprintf( szcommand, "n%X|", listidx ); //被打的人数
	BATTLESTR_ADD( szcommand );			

		// 取得人物的属性(不论玩家或是宠物)
		//BATTLE_GetAttr( BATTLE_No2Index( battleindex , attackNo ) , &attattr[2] , &attattr[1] , &attattr[0] , &attattr[3] , &attattr[4] );
		// 如果攻击者是玩家,取得人物的魔法熟练度及魔法抗性
		{
			int attType = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_WHICHTYPE);
			int Check=0;
			AttIsPlayer=0;
			if( attType == CHAR_TYPEPLAYER)	{
				AttIsPlayer = 1;
				for(i=0;i<4;i++){	// att_magic_lv[i]: i = 0:地 1:水 2:火 3:风
					att_magic_lv[i] = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo),CHAR_EARTH_EXP+i);
				}
			}else if( attType == CHAR_TYPEENEMY )	{
				for(i=0;i<4;i++){
					att_magic_lv[i] = (CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV)*0.9);
				}
			}else	{ //pet
                for(i=0;i<4;i++){
					att_magic_lv[i] = 5;
				}

			}
			Check = rand()%100;
#ifdef _FIX_MAGICDAMAGE
			if( Check > att_magic_lv[ FieldAttr] ) TrueMagic = FALSE;
			else TrueMagic = TRUE;
#endif
		}
		pet_att_lv = CHAR_getInt(BATTLE_No2Index(battleindex,attackNo), CHAR_LV);

		z = 0;
    for(i=0;i<listidx;i++){
charaidx = BATTLE_No2Index( battleindex , list[i] );
      petidx   = BATTLE_getRidePet( charaidx );
      //BATTLE_GetAttr( charaidx , &defattr[2] , &defattr[1] , &defattr[0] , &defattr[3] , &defattr[4] );
		{//andy_fix
			int defType = CHAR_getInt(charaidx,CHAR_WHICHTYPE);
			DefIsPlayer = 0;
			if( defType == CHAR_TYPEPLAYER)	{
				DefIsPlayer = 1;
				for(j=0;j<DEF_MAGIC_NUM;j++){	// def_magic_resist[i]: i = 0:地 1:水 2:火 3:风
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
#ifdef _EQUIT_DEFMAGIC
					def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#endif
				}
			}else if( defType == CHAR_TYPEENEMY )	{
				for(j=0;j<DEF_MAGIC_NUM;j++){
					def_magic_resist[j] = (CHAR_getInt(charaidx,CHAR_LV)*0.5);
				}
			}else	{	//PET
#ifdef _FIX_MAGICDAMAGE
				DefIsPlayer = 1;
#endif
				for(j=0;j<DEF_MAGIC_NUM;j++){
#ifdef _FIX_MAGICDAMAGE
					def_magic_resist[j] = CHAR_getInt(charaidx,CHAR_EARTH_RESIST+j);
					//def_magic_resist[j] += CHAR_getWorkInt( charaidx, CHAR_EQUITDEFMAGIC_E+j);
#else
					def_magic_resist[j] = 0;
#endif
				}
			}

#ifdef _MAGIC_DEFMAGICATT
			if( CHAR_getWorkInt( charaidx, CHAR_DEFMAGICSTATUS ) > 0 ){
				float def = (float)(CHAR_getWorkInt( charaidx, CHAR_OTHERSTATUSNUMS ));
				def = def/100;
				for(j=0;j<DEF_MAGIC_NUM;j++){
					if( def_magic_resist[j] <= 0 ) continue;
					def_magic_resist[j] += def_magic_resist[j] * def;
				}
			}
#endif

		}
		pet_def_lv = CHAR_getInt(charaidx,CHAR_LV);
			if(BATTLE_MagicDodge(charaidx,DefIsPlayer,FieldAttr)){	// 计算闪避率
				attvalue = pethp = 0;
				sprintf( szcommand, 
			    "r%X|f%X|d%X|p%X|",
			    list[i],
			    0,
			    attvalue,
			    pethp );
				BATTLESTR_ADD( szcommand );
				continue;
			}else{	// 没闪过,计算攻击力
#ifdef _FIX_MAGICDAMAGE
				float Kmagic = ((float)att_magic_lv[FieldAttr]*1.4 - (float)def_magic_resist[FieldAttr]);
				float Mmagic = (float)att_magic_lv[FieldAttr];
				float Amagic=0.00;
				int APower=0;
				if( Kmagic < 0 ) Kmagic = 0;
				if( Mmagic < 1 ) Mmagic = 1;
				Amagic = (Kmagic*Kmagic) / (Mmagic*Mmagic);
				Amagic += ((float)(rand()%20))/100;
				APower = (int)(Power*(1+(float)MagicLv/10) * Amagic);	//MagicLv

				//andy_log
/*				print("att_magic_lv:%d",att_magic_lv[FieldAttr]);
				print( "Player:%s\n", CHAR_getUseName( charaidx));
				print( "ANDY Magic Power:%d Amagic:%f APower:%d \n", Power, Amagic, APower);
*/               
				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index 给後面 "被攻击者中了睡眠"那一段使用

				attvalue = BATTLE_getMagicAdjustInt( BATTLE_No2Index(battleindex,attackNo), charaidx,
					                                     MagicLv, FieldAttr, APower);
				//andy_log
//				print("attvalue:%d \n", attvalue);
				//if( TrueMagic == FALSE )//使用失败
				//	attvalue*=0.7;
				
				if( DefIsPlayer )	{
					Magic_ComputeDefExp( charaidx, FieldAttr, MagicLv, attvalue);
				}
				
#else
				def_be_hit[getexp++] = charaidx;	// 记下被打的人的index

				// 计算属性相克
				attvalue = BATTLE_AttrAdjust(BATTLE_No2Index(battleindex,attackNo),charaidx,Power);
				if(DefIsPlayer){	// 被打的是玩家
					if(AttIsPlayer){	// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)def_magic_resist[FieldAttr])/(float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}else{
						temp = ((float)pet_att_lv - (float)def_magic_resist[FieldAttr]) / (float)def_magic_resist[FieldAttr] / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}else{// 被打的是宠物
					if(AttIsPlayer){// 如果是玩家用魔法攻击
						temp = ((float)att_magic_lv[FieldAttr] - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}else	{
						temp = ((float)pet_att_lv - (float)pet_def_lv) / (float)pet_def_lv / 100;
						attvalue = Power + Power * temp + attvalue;
					}
				}
				// 上下取10
				attvalue += rand()%2 ? (rand()%10)+1 : -(rand()%10)-1;
				if( attvalue <= 0 ) attvalue = 1;
#endif
			}

      charahp = CHAR_getInt( charaidx , CHAR_HP );
       // 没有宠物
       if( -1 == petidx || CHAR_getInt( petidx , CHAR_HP ) <= 0 ){
		   if( ( charahp -= attvalue ) < 0 ){
             charahp = 0;
		   }
          CHAR_setInt( charaidx , CHAR_HP , charahp );
          pethp = 0;

          sprintf( szcommand, "r%X|f%X|d%X|p%X|", list[i], 0, attvalue, pethp );

       }else{
          pethp     = CHAR_getInt( petidx , CHAR_HP ); 
          charahurt = attvalue * BATTLE_CalcCharaRatio( FieldAttr , charaidx ) / 10;

		  charahurt_temp = charahurt;
          if( ( charahp -= charahurt ) < 0 )
          {
            charahurt = charahp;
            charahp   = 0;
			//ANDY_ADD
			//CHAR_setFlg( charaidx, CHAR_ISDIE, TRUE);
          }
					
          attvalue = attvalue - charahurt;

          if( ( pethp -= attvalue ) < 0 )
          {
            pethp = 0;
						// 宠物没血了退出战场
						CHAR_setInt(charaidx,CHAR_RIDEPET,-1);
						BATTLE_changeRideImage(charaidx);
						CHAR_setWorkInt(charaidx,CHAR_WORKPETFALL,1);
          }
          CHAR_setInt( charaidx , CHAR_HP , charahp );
          CHAR_setInt( petidx , CHAR_HP , pethp );

					charahurt = charahurt_temp;

          sprintf( szcommand, 
			    "r%X|f%X|d%X|p%X|",
			    list[i],
			    0,
			    charahurt,
			    attvalue );

       }
       BATTLESTR_ADD( szcommand );
    }

	if( (TrueMagic == FALSE) && AttIsPlayer)	{	//计算攻击方经验值
		Magic_ComputeAttExp( BATTLE_No2Index(battleindex,attackNo), FieldAttr, MagicLv, getexp);
	}
	for(i=0; i<listidx; i++)	{
		// 如果被攻击者中了睡眠
		if(def_be_hit[i] < 0) continue;
		if(CHAR_getWorkInt(def_be_hit[i],CHAR_WORKSLEEP) > 0) {
			CHAR_setWorkInt(def_be_hit[i],CHAR_WORKSLEEP,0);
			z = BATTLE_Index2No(battleindex,def_be_hit[i]);
			sprintf(szcommand,"BM|%X|%X|",z,0);
			BATTLESTR_ADD(szcommand);
		}
	}
}
#endif

#ifdef _PROFESSION_ADDSKILL
unsigned int GET_PROFESSION_magic_array(int idx)
{
    return PROFESSION_magic[idx].uiSpriteNum;
}
#endif




