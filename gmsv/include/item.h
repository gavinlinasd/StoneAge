
#ifndef __ITEM_H__
#define __ITEM_H__

#include "char.h"

#define NULLITEM    "0"

typedef enum
{
    ITEM_FIST =0,
    ITEM_AXE,
    ITEM_CLUB,
    ITEM_SPEAR,
    ITEM_BOW,
    ITEM_SHIELD,
    ITEM_HELM,
    ITEM_ARMOUR,

	ITEM_BRACELET =8,
	ITEM_MUSIC,
	ITEM_NECKLACE,
	ITEM_RING,
	ITEM_BELT,
	ITEM_EARRING,
	ITEM_NOSERING,
	ITEM_AMULET,
    /* ****** */
    ITEM_OTHER =16,
    ITEM_BOOMERANG,		// 隙唅梓
    ITEM_BOUNDTHROW,	// 芘祣葦芛
    ITEM_BREAKTHROW,	// 芘祣坒
    ITEM_DISH =20,
#ifdef _ITEM_INSLAY
	ITEM_METAL,
	ITEM_JEWEL,
#endif
#ifdef _ITEM_CHECKWARES
	ITEM_WARES,			//億昜
#endif

#ifdef _ITEM_EQUITSPACE
	ITEM_WBELT,			//殈湍
	ITEM_WSHIELD,		//嗎
	ITEM_WSHOES,		//衧赽
#endif
#ifdef _EQUIT_NEWGLOVE 
	ITEM_WGLOVE,		//忒杶
#endif

#ifdef _ALCHEMIST
	ITEM_ALCHEMIST =30,
#endif

#ifdef _ANGEL_SUMMON
	//ITEM_ANGELTOKEN,
	//ITEM_HEROTOKEN,
#endif

    ITEM_CATEGORYNUM,
    
}ITEM_CATEGORY;

typedef enum
{
	ITEM_FIELD_ALL,
	ITEM_FIELD_BATTLE,
	ITEM_FIELD_MAP,
}ITEM_FIELDTYPE;

typedef enum
{
	ITEM_TARGET_MYSELF,
	ITEM_TARGET_OTHER,
	ITEM_TARGET_ALLMYSIDE,
	ITEM_TARGET_ALLOTHERSIDE,
	ITEM_TARGET_ALL,
}ITEM_TARGETTYPE;

typedef enum
{
    ITEM_ID,
    ITEM_BASEIMAGENUMBER,
    ITEM_COST,
    ITEM_TYPE,
	ITEM_ABLEUSEFIELD,
	ITEM_TARGET,
    ITEM_LEVEL,                     /*  LEVEL  */
#ifdef _ITEM_MAXUSERNUM
	ITEM_DAMAGEBREAK,				//昜こ妏蚚棒杅
#endif

#ifdef _ITEMSET4_TXT
	ITEM_USEPILENUMS,				//昜こ剽詁棒杅
	ITEM_CANBEPILE,					//岆瘁褫剽詁

	ITEM_NEEDSTR,
	ITEM_NEEDDEX,
	ITEM_NEEDTRANS,
	ITEM_NEEDPROFESSION,
#endif

#ifdef _TAKE_ITEMDAMAGE
	ITEM_DAMAGECRUSHE,
	ITEM_MAXDAMAGECRUSHE,
#endif

#ifdef _ADD_DEAMGEDEFC
	ITEM_OTHERDAMAGE,
	ITEM_OTHERDEFC,
#endif

#ifdef _SUIT_ITEM
	ITEM_SUITCODE,
#endif

    ITEM_ATTACKNUM_MIN,             /*    黖  賓樊倳  */
    ITEM_ATTACKNUM_MAX,             /*    禜  賓樊倳  */
    ITEM_MODIFYATTACK,              /*    賓    撬倵  */
    ITEM_MODIFYDEFENCE,             /*    遛    撬倵  */
    ITEM_MODIFYQUICK,               /*  QUICK  撬倵  */

    ITEM_MODIFYHP,                  /*  HP  撬倵    */
    ITEM_MODIFYMP,                  /*  MP  撬倵    */
    ITEM_MODIFYLUCK,                /*  LUCK  撬倵    */
    ITEM_MODIFYCHARM,               /*  CHARM  撬倵    */
    ITEM_MODIFYAVOID,               /*  樊    臚諜    */
	ITEM_MODIFYATTRIB,				/*  鶂鍛臚諜 */
	ITEM_MODIFYATTRIBVALUE,			/*  鶂鍛臚諜偯 */
	ITEM_MAGICID,					/*  ��絊  蠕 */
	ITEM_MAGICPROB,					/*  ��絊       */
	ITEM_MAGICUSEMP,				/*  觛  MP */

#ifdef _ITEMSET5_TXT
	ITEM_MODIFYARRANGE,
	ITEM_MODIFYSEQUENCE,

	ITEM_ATTACHPILE,
	ITEM_HITRIGHT,	//塗俋韜笢
#endif
#ifdef _ITEMSET6_TXT
	ITEM_NEGLECTGUARD,
//	ITEM_BEMERGE,
#endif
    /*  筒  ↓淏筒臚諜偯��*/
    ITEM_POISON,					/*   倵�庈訞垮暰葆炕麚�          */
    ITEM_PARALYSIS,          		/* 媃怮躂��1  摯菜  誑ぁ拻埵笢�� */
    ITEM_SLEEP,              		/* 懨堇�蛣�  ぁ拻埵笢            */
    ITEM_STONE,              		/* 歕�蛣�  ぁ拻埵笢              */
    ITEM_DRUNK,              		/* 域竣��      誑き誑堎     */
    ITEM_CONFUSION,          		/* 趷崷��  賓    禱罃堎     */

	ITEM_CRITICAL,					/* 袲惉  贖庈鳴  臚諜 */

	ITEM_USEACTION,					/* 窅埱倜鄹摯囮袲で砫璃 */
    ITEM_DROPATLOGOUT,              /* 痲精囮坳哱埰堎鄹勗  埰凝汔竣凝  */
    ITEM_VANISHATDROP,              /*   媃倜鄹勗觛窇堎凝汔竣凝 */
    ITEM_ISOVERED,                  /*  窀勗篥埱凝�桱桫戰職時�竣凝��*/
	ITEM_CANPETMAIL,				/* 妐蚗哱隍↓鳴ぁ邞躂堎凝 */
	ITEM_CANMERGEFROM,				/* 譴埬楁勗埵躂堎凝 */
	ITEM_CANMERGETO,				/* 譴埬袸勗埵躂堎凝 */

    ITEM_INGVALUE0,                 /* 埬覕(5蠀覕) */
    ITEM_INGVALUE1,
    ITEM_INGVALUE2,
    ITEM_INGVALUE3,
    ITEM_INGVALUE4,
    
	ITEM_PUTTIME,					/*  囮騷  堜誑  凝躂倜鄹蹬 */
    ITEM_LEAKLEVEL,                 /*    碠誑汔躂煦崹�厊憶袛�  */
	ITEM_MERGEFLG,					/*  譴埬踏躂倜囮騷  堜凝汔竣凝 */
	ITEM_CRUSHLEVEL,				/*  癟躂梣譴笢 0  2 兔毀癟躂趙埵笢 2毀蠈癟 */

    ITEM_VAR1,              	/*    暵軘躺       */
    ITEM_VAR2,              	/*    暵軘躺       */
    ITEM_VAR3,              	/*    暵軘躺       */
    ITEM_VAR4,              	/*    暵軘躺       */

	ITEM_DATAINTNUM,

}ITEM_DATAINT;

typedef enum
{
    ITEM_NAME,                      /*    鞳     騍摯  鞳     */
    ITEM_SECRETNAME,                /*    鞳    脾踏躂堎菴  鍛  堇    */
    ITEM_EFFECTSTRING,              /*  嗚堅  棬    */
    ITEM_ARGUMENT,                  /*  囮騷  堜摯礎倳  */
#ifdef _ITEM_INSLAY
	ITEM_TYPECODE,
	ITEM_INLAYCODE,
#endif
	ITEM_CDKEY,						/*  囮騷  堜摯  鞳禱  疐勗  脾媃倜硞摯           */
#ifdef _ITEM_FORUSERNAMES
	ITEM_FORUSERNAME,
	ITEM_FORUSERCDKEY,
#endif
// CoolFish: 2001/10/11
#ifdef _UNIQUE_P_I
    ITEM_UNIQUECODE,		  /* 昜こ晤鎢 */
#endif

    ITEM_INGNAME0,                  /*  埬覕摯  鞳(5蠀覕) */
    ITEM_INGNAME1,
    ITEM_INGNAME2,
    ITEM_INGNAME3,
    ITEM_INGNAME4,


    ITEM_INITFUNC,                  /* 礎倳
                                     * ITEM_Item*
                                     * 蒍堇偯 BOOL
                                     * 蒍堇偯摯徽  毀 CHAR_INITFUNC
                                     * 敁  啋  */
    ITEM_FIRSTFUNCTION = ITEM_INITFUNC,
    ITEM_PREOVERFUNC,               /* CHAR_PREOVERFUNC 禱落敺 */
    ITEM_POSTOVERFUNC,              /* CHAR_POSTOVERFUNC 禱落敺*/
    ITEM_WATCHFUNC,                 /* CHAR_WATCHFUNC 禱落敺 */
    ITEM_USEFUNC,                   /* 礎倳毀��
                                     * int charaindex す籤溘騷璃溢蚗袲筒
                                     * int charitemindex 猷覕摯
                                     *              囮騷  堜    摯笰    
                                     *              禱窅埱倜凝
                                     */
    ITEM_ATTACHFUNC,                /* 礎倳毀��
                                     * int charaindex す籤溘騷璃溢蚗袲筒
                                     * int itemindex  囮騷  堜騷璃溢蚗袲筒
                                     *      す籤溘袲淏摯  埱趙笢堎囮騷  堜
                                     *      摯囮騷  堜  ぁ摯騷璃溢蚗袲筒
                                     *      ぁ毀埵笢痀勗曛徽��
                                     */
    ITEM_DETACHFUNC,                /* 礎倳毀��
                                     * int charaindex す籤溘騷璃溢蚗袲筒
                                     * int itemindex  囮騷  堜騷璃溢蚗袲筒
                                     *      す籤溘袲淏摯  埱趙笢堎囮騷  堜
                                     *      摯囮騷  堜  ぁ摯騷璃溢蚗袲筒
                                     *      ぁ毀埵笢痀勗曛徽��
                                     */
    ITEM_DROPFUNC, 		            /*   敁媃倜敁拻
                                     * 礎倳毀
                                     *  int charaindex   敁媃倜す籤溘
                                     *  int itemindex 囮騷  堜騷璃溢蚗袲筒
                                     */
    ITEM_PICKUPFUNC,              /* 囮騷  堜禱蝶埱倜鄹
                                     * 礎倳毀
                                     *  int charaindex  蝶埱倜す籤溘index
                                     *  int itemindex 囮騷  堜騷璃溢蚗袲筒
                                     */
#ifdef _Item_ReLifeAct
	ITEM_DIERELIFEFUNC,					/*ANDY_ADD
										葩魂耋撿	
									 */
#endif

#ifdef _CONTRACT
	ITEM_CONTRACTTIME,
	ITEM_CONTRACTARG,
#endif

    ITEM_LASTFUNCTION,

    ITEM_DATACHARNUM = ITEM_LASTFUNCTION,

#ifdef _ANGEL_SUMMON
	ITEM_ANGELMISSION = ITEM_INGNAME0,
	ITEM_ANGELINFO = ITEM_INGNAME1,
	ITEM_HEROINFO = ITEM_INGNAME2,
#endif

}ITEM_DATACHAR;

typedef enum
{
    ITEM_WORKOBJINDEX,
    ITEM_WORKCHARAINDEX,
#ifdef _MARKET_TRADE
	ITEM_WORKTRADEINDEX,
	ITEM_WORKTRADETYPE,
	ITEM_WORKTRADESELLINDEX,
#endif
#ifdef _ITEM_ORNAMENTS
	ITEM_CANPICKUP,
#endif
#ifdef _ITEM_TIME_LIMIT
	ITEM_WORKTIMELIMIT,
#endif
    ITEM_WORKDATAINTNUM,
}ITEM_WORKDATAINT;



typedef struct tagItem
{
    int         data[ITEM_DATAINTNUM];
    STRING64    string[ITEM_DATACHARNUM];
    int         workint[ITEM_WORKDATAINTNUM];

    void*       functable[ITEM_LASTFUNCTION-ITEM_FIRSTFUNCTION];
}ITEM_Item;

typedef struct tagITEM_table
{
    int         use; 
    ITEM_Item   itm;
    int         randomdata[ITEM_DATAINTNUM];
}ITEM_table;


typedef struct tagITEM_exists
{
    BOOL        use;
    ITEM_Item   itm;
}ITEM_exists;

#ifdef _CONTRACT
#define MAX_CONTRACTTABLE	10
typedef struct tagITEM_contract
{
    int         used;
    char		detail[2048];
    int			argnum;
}ITEM_contractTable;
#endif


#define		ITEM_CHECKINDEX(index)		\
	_ITEM_CHECKINDEX( __FILE__, __LINE__, index)
INLINE BOOL _ITEM_CHECKINDEX( char *file, int line, int index);


BOOL ITEM_initExistItemsArray( int num );
BOOL ITEM_endExistItemsArray( void );
#define		ITEM_initExistItemsOne( itm) \
	_ITEM_initExistItemsOne( __FILE__, __LINE__, itm)
int _ITEM_initExistItemsOne( char *file, int line, ITEM_Item* itm );

#define		ITEM_endExistItemsOne( index ) \
			_ITEM_endExistItemsOne( index, __FILE__, __LINE__)

void _ITEM_endExistItemsOne( int index , char *file, int line);

#define ITEM_getInt( Index, element) _ITEM_getInt( __FILE__, __LINE__, Index, element )
INLINE int _ITEM_getInt( char *file, int line, int index ,ITEM_DATAINT element);


#define ITEM_setInt( Index, element, data) _ITEM_setInt( __FILE__, __LINE__, Index, element, data)
INLINE int _ITEM_setInt( char *file, int line, int index ,ITEM_DATAINT element, int data);


INLINE char* ITEM_getChar( int index ,ITEM_DATACHAR element );
INLINE BOOL ITEM_setChar( int index ,ITEM_DATACHAR element , char* new);

INLINE int ITEM_getWorkInt( int index ,ITEM_WORKDATAINT element);
INLINE int ITEM_setWorkInt( int index ,ITEM_WORKDATAINT element, int data);
INLINE int ITEM_getITEM_itemnum( void );
INLINE int ITEM_getITEM_UseItemnum( void );
INLINE BOOL ITEM_getITEM_use( int index );
void ITEM_constructFunctable( int itemindex );
void* ITEM_getFunctionPointer( int itemindex, int functype );
INLINE ITEM_Item *ITEM_getItemPointer( int index );
int ITEM_getItemMaxIdNum( void);


char* ITEM_makeStringFromItemData( ITEM_Item* one, int mode );
char* ITEM_makeStringFromItemIndex( int index, int mode );

BOOL ITEM_makeExistItemsFromStringToArg( char* src , ITEM_Item* item, int mode );
void ITEM_getDefaultItemSetting( ITEM_Item* itm);


INLINE BOOL ITEM_CHECKITEMTABLE( int number );
BOOL    ITEM_readItemConfFile( char* filename );


CHAR_EquipPlace ITEM_getEquipPlace( int charaindex, int itmid );


char*  ITEM_makeItemStatusString( int haveitemindex, int itemindex );
char*   ITEM_makeItemFalseString( void );
char*   ITEM_makeItemFalseStringWithNum( int haveitemindex );


BOOL ITEM_makeItem( ITEM_Item* itm, int number );
int ITEM_makeItemAndRegist( int number );


void ITEM_equipEffect( int index );

void Other_DefcharWorkInt( int index);

char* ITEM_getAppropriateName(int itemindex);
char* ITEM_getEffectString( int itemindex );


int ITEM_getcostFromITEMtabl( int itemid );

#define ITEM_getNameFromNumber( id) _ITEM_getNameFromNumber( __FILE__, __LINE__, id)
INLINE char* _ITEM_getNameFromNumber( char *file, int line, int itemid );


int ITEM_getlevelFromITEMtabl( int itemid );
int ITEM_getgraNoFromITEMtabl( int itemid );
char *ITEM_getItemInfoFromNumber( int itemid );

int ITEM_getdropatlogoutFromITEMtabl( int itemid );
int ITEM_getvanishatdropFromITEMtabl( int itemid );
int ITEM_getcanpetmailFromITEMtabl( int itemid );
int ITEM_getmergeItemFromFromITEMtabl( int itemid );

#ifdef _ITEM_CHECKWARES
BOOL CHAR_CheckInItemForWares( int charaindex, int flg);
#endif

BOOL ITEM_canuseMagic( int itemindex);
// Nuke +1 08/23 : For checking the validity of item target
int ITEM_isTargetValid( int charaindex, int itemindex, int toindex);


#ifdef _IMPOROVE_ITEMTABLE
BOOL ITEMTBL_CHECKINDEX( int ItemID);
int ITEM_getSIndexFromTransList( int ItemID);
int ITEM_getMaxitemtblsFromTransList( void);
int ITEM_getTotalitemtblsFromTransList( void);
#endif

int ITEMTBL_getInt( int ItemID, ITEM_DATAINT datatype);
char *ITEMTBL_getChar( int ItemID, ITEM_DATACHAR datatype);

int ITEM_getItemDamageCrusheED( int itemindex);
void ITEM_RsetEquit( int charaindex);//赻雄迠壺蚾掘弇离渣昫眳昜こ
void ITEM_reChangeItemToPile( int itemindex);
void ITEM_reChangeItemName( int itemindex);


#ifdef _SIMPLIFY_ITEMSTRING
void ITEM_getDefaultItemData( int itemID, ITEM_Item* itm);
#endif

#ifdef _CONTRACT
BOOL ITEM_initContractTable( );
#endif

#endif
