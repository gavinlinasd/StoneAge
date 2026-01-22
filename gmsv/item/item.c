#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include "common.h"
#include "char.h"
#include "char_data.h"
#include "battle.h"
#include "item.h"
#include "buf.h"
#include "function.h"
#include "magic_base.h"

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#endif

static int ITEM_tblen = 0;
static ITEM_exists* ITEM_item;
static int          ITEM_itemnum;
static int			ITEM_UseItemnum=0;
static char* ITEM_checkString( char* string );
static int ITEM_getRandomValue( char*  string,int* randomwidth, int num );
static int ITEM_isstring1or0( char*  string,int* randomwidth, int num );

#ifdef _IMPOROVE_ITEMTABLE
typedef struct _tag_ITEM_transformList
{
	int Si;
	int use;
}sITEM_transformList;
sITEM_transformList *ITEM_TransformList = NULL;
static int defitemtbls=0;
static int totalitemtbls=0;
#endif

#ifdef _SIMPLIFY_ITEMSTRING
typedef struct ITEM_tag_intDataSetting
{
	char*   dumpchar;
	int     defaults;
	int table;
}ITEM_intDataSetting;

typedef struct ITEM_tag_charDataSetting
{
	char*   dumpchar;
	char*   defaults;
	int table;
}ITEM_charDataSetting;

static ITEM_intDataSetting ITEM_setintdata[]={
	{"id",		0,			ITEM_ID },
	{"bi",		0,			ITEM_BASEIMAGENUMBER },
	{"cs",		0,			ITEM_COST },
	{"ep",		ITEM_OTHER,	ITEM_TYPE },
	{"ft",		0,			ITEM_ABLEUSEFIELD },
	{"tg",		0,			ITEM_TARGET },
	{"lv",		0,			ITEM_LEVEL },
#ifdef _ITEM_MAXUSERNUM
	{"dib",		-1,			ITEM_DAMAGEBREAK },
#endif
#ifdef _ITEMSET4_TXT
	{"upin",	1,			ITEM_USEPILENUMS },
	{"canpile",	0,			ITEM_CANBEPILE },
	{"nestr",	0,			ITEM_NEEDSTR },
	{"nedex",	0,			ITEM_NEEDDEX },
	{"netra",	0,			ITEM_NEEDTRANS },
	{"neprof",	0,			ITEM_NEEDPROFESSION },
#endif
#ifdef _TAKE_ITEMDAMAGE
	{"dmce",	0,			ITEM_DAMAGECRUSHE },
	{"mdmce",	0,			ITEM_MAXDAMAGECRUSHE },
#endif
#ifdef _ADD_DEAMGEDEFC
	{"otdmag",	0,			ITEM_OTHERDAMAGE },
	{"otdefc",	0,			ITEM_OTHERDEFC },
#endif
#ifdef _SUIT_ITEM
	{"nsuit",	0,			ITEM_SUITCODE },
#endif
	{"ann",		0,			ITEM_ATTACKNUM_MIN },
	{"anx",		0,			ITEM_ATTACKNUM_MAX },
	{"ma",		0,			ITEM_MODIFYATTACK },
	{"md",		0,			ITEM_MODIFYDEFENCE },

	{"mh",		0,			ITEM_MODIFYQUICK}, //ITEM_MODIFYHP }, ITEM_MODIFYQUICK
	{"mm",		0,			ITEM_MODIFYHP}, //ITEM_MODIFYMP }, ITEM_MODIFYHP
	{"mq",		0,			ITEM_MODIFYMP}, //ITEM_MODIFYQUICK }, ITEM_MODIFYMP
	{"ml",		0,			ITEM_MODIFYLUCK },
	{"mc",		0,			ITEM_MODIFYCHARM },
	{"mv",		0,			ITEM_MODIFYAVOID },
	{"mat",		0,			ITEM_MODIFYATTRIB },
	{"mav",		0,			ITEM_MODIFYATTRIBVALUE },
	{"mid",		-1,			ITEM_MAGICID },
	{"mpr",		0,			ITEM_MAGICPROB },
	{"mu",		0,			ITEM_MAGICUSEMP },
#ifdef _ITEMSET5_TXT
	{"arr",		0,			ITEM_MODIFYARRANGE },
	{"seqce",	0,			ITEM_MODIFYSEQUENCE },
	{"iapi",	0,			ITEM_ATTACHPILE },
	{"hirt",	0,			ITEM_HITRIGHT },
#endif

#ifdef _ITEMSET6_TXT
	{"neguard",	0,			ITEM_NEGLECTGUARD},
#endif
	{"mrf",		0,			ITEM_MERGEFLG},

};

static ITEM_charDataSetting     ITEM_setchardata[]={
	{"na",			"",	ITEM_NAME },        /*  ITEM_NAME   */
	{"sn",			"",	ITEM_SECRETNAME },        /*  ITEM_SCRETNAME  */
	{"en",			"",	ITEM_EFFECTSTRING },        /*  ITEM_EFFECTSTRING  */
	{"ar",			"",	ITEM_ARGUMENT },        /*  ITEM_ARGUMENT   */
#ifdef _ITEM_INSLAY
	{"acode",		"",	ITEM_TYPECODE }, /*ITEM_TYPECODE,*/
	{"inlaycode",	"",	ITEM_INLAYCODE}, /*ITEM_INLAYCODE,*/
#endif
	{"cdk",			"",	ITEM_CDKEY },       /*  ITEM_CDKEY   */
#ifdef _ITEM_FORUSERNAMES
	{"forname",		"",	ITEM_FORUSERNAME },		//ITEM_FORUSERNAME,
	{"forcdkey",	"",	ITEM_FORUSERCDKEY },		//ITEM_FORUSERCDKEY
#endif

#ifdef _ANGEL_SUMMON
	{"mindex",		"",	ITEM_ANGELMISSION},
	{"angel",		"",	ITEM_ANGELINFO},
	{"hero",		"",	ITEM_HEROINFO},
#endif

#ifdef _CONTRACT
	{"ctt",			"",	ITEM_CONTRACTTIME},
	{"cta",			"",	ITEM_CONTRACTARG},
#endif

#ifdef _UNIQUE_P_I
	{"ucode",		"",	ITEM_UNIQUECODE},    /*  ITEM_UNIQUECODE  */
#endif	
};

static ITEM_charDataSetting     ITEM_setmemdata={
	"",  "", -1         /*  ITEM_MEMO  */
};


#else
typedef struct ITEM_tag_intDataSetting
{
	char*   dumpchar;
	int     defaults;
}ITEM_intDataSetting;

typedef struct ITEM_tag_charDataSetting
{
	char*   dumpchar;
	char*   defaults;
}ITEM_charDataSetting;

static ITEM_intDataSetting ITEM_setintdata[ITEM_DATAINTNUM]={
	{"id",  0},                 /*  ITEM_ID */
	{"bi",  0},                 /*  ITEM_ITEMBASEIMAGENUMBER */
	{"cs",  0},                 /*  ITEM_COST    */
	{"ep",  ITEM_OTHER},        /*  ITEM_TYPE   */
	{"ft",  0},					/*  ITEM_ABLEUSEFIELD */
	{"tg",  0},					/*  ITEM_TARGET */
	{"lv",  0},                 /*  ITEM_LEVEL  */

#ifdef _ITEM_MAXUSERNUM
	{"dib", -1},					/*ITEM_DAMAGEBREAK*/
#endif

#ifdef _ITEMSET4_TXT
	{"upin", 1},				//ITEM_USEPILENUMS,
	{"canpile", 0},				//ITEM_CANBEPILE
	{"nestr", 0},				//ITEM_NEEDSTR,
	{"nedex", 0},				//ITEM_NEEDDEX,
	{"netra", 0},				//ITEM_NEEDTRANS,
	{"neprof", 0},				//ITEM_NEEDPROFESSION,
#endif

#ifdef _TAKE_ITEMDAMAGE
	{"dmce", 0},				/*ITEM_DAMAGECRUSHE*/
	{"mdmce",0},				//ITEM_MAXDAMAGECRUSHE
#endif

#ifdef _ADD_DEAMGEDEFC
	{"otdmag", 0},				//ITEM_OTHERDAMAGE,
	{"otdefc", 0},				//ITEM_OTHERDEFC,
#endif

#ifdef _SUIT_ITEM
	{"nsuit", 0},				//ITEM_SUITCODE,
#endif
	{"ann",  0},                /*  ITEM_ATTACKNUM_MIN  */
	{"anx",  0},                /*  ITEM_ATTACKNUM_MAX  */
	{"ma",  0},                 /*  ITEM_MODIFYATTACK    */
	{"md",  0},                 /*  ITEM_MODIFYDEFENCE  */
	{"mh",  0},                 /*  ITEM_MODIFYHP    */
	{"mm",  0},                 /*  ITEM_MODIFYMP    */

	{"mq",  0},                 /*  ITEM_MODIFYQUICK */

	{"ml",  0},                 /*  ITEM_MODIFYLUCK */
	{"mc",  0},                 /*  ITEM_MODIFYCHARM */
	{"mv",  0},                 /*  ITEM_MODIFYAVOID */
	{"mat",  0},				/*  ITEM_MODIFYATTRIB */
	{"mav",  0},				/*  ITEM_MODIFYATTRIBVALUE */

	{"mid", -1},				/*  ITEM_MAGICID	*/
	{"mpr", 0},					/*  ITEM_MAGICPROB  */
	{"mu",  0},					/*  ITEM_MAGICUSEMP  */
#ifdef _ITEMSET5_TXT
	{"arr",  0},					/*ITEM_MODIFYARRANGE*/
	{"seqce",  0},					//ITEM_MODIFYSEQUENCE,
	{"iapi", 0},					//ITEM_ATTACHPILE
	{"hirt", 0},					//ITEM_HITRIGHT		//额外命中
#endif

#ifdef _ITEMSET6_TXT
	{"neguard", 0},					//ITEM_NEGLECTGUARD
//	{"bemerge", 0},					//ITEM_BEMERGE
#endif
	{"mpo",  0},					/*  ITEM_POISON  */
	{"mpa",  0},					/*  ITEM_PARALYSIS  */
	{"msl",  0},					/*  ITEM_SLEEP  */
	{"mst",  0},					/*  ITEM_STONE  */
	{"mdr",  0},					/*  ITEM_DRUNK  */
	{"mco",  0},					/*  ITEM_CONFUSION  */

	{"mcr",  0},					/*  ITEM_CRITICAL  */

	{"ua",  -1},					/*  ITEM_USEACTION */
	{"drlo",FALSE},             /*  ITEM_DROPATLOGOUT   */
	{"valo",FALSE},             /*  ITEM_VANISHATLOGOUT */
	{"ioed",TRUE},              /*  ITEM_ISOVERED   */
	{"cpm", TRUE},              /*  ITEM_CANPETMAIL   */
	{"cmf", TRUE},              /*  ITEM_CANMERGEFROM   */
	{"cmt", TRUE},              /*  ITEM_CANMERGETO   */

    {"ingv0",0},                /*  ITEM_INGVALUE0 */
    {"ingv1",0},
    {"ingv2",0},
    {"ingv3",0},
    {"ingv4",0},

	{"ipt",  0},				/*  ITEM_PUTTIME    */
	{"ll",  0},					/*  ITEM_LEAKLEVEL  */
	{"mrf",  0},				/*  ITEM_MERGEFLG  */
	{"clv", 0},					/*	ITEM_CRUSHLEVEL */

	{"v1",  0},                 /*  ITEM_VAR1  */
	{"v2",  0},                 /*  ITEM_VAR2  */
	{"v3",  0},                 /*  ITEM_VAR3  */
	{"v4",  0},                 /*  ITEM_VAR4  */
};

static ITEM_charDataSetting     ITEM_setchardata[ITEM_DATACHARNUM]={
	{"na",  ""},        /*  ITEM_NAME   */
	{"sn",  ""},        /*  ITEM_SCRETNAME  */
	{"en",  ""},        /*  ITEM_EFFECTSTRING  */
	{"ar",  ""},        /*  ITEM_ARGUMENT   */
#ifdef _ITEM_INSLAY
	{"acode",   ""}, /*ITEM_TYPECODE,*/
	{"inlaycode",   ""}, /*ITEM_INLAYCODE,*/
#endif
	{"cdk",  ""},       /*  ITEM_CDKEY   */
#ifdef _ITEM_FORUSERNAMES
	{"forname",  ""},		//ITEM_FORUSERNAME,
	{"forcdkey",  ""},		//ITEM_FORUSERCDKEY
#endif
// CoolFish: 2001/10/11
#ifdef _UNIQUE_P_I
	{"ucode",   ""},    /*  ITEM_UNIQUECODE  */
#endif	
    {"ingn0", ""},      /*  ITEM_INGNAME0 */
    {"ingn1", ""},
    {"ingn2", ""},
    {"ingn3", ""},
    {"ingn4", ""},

	{"inif",    ""},    /*  ITEM_PREOVERFUNC    */
	{"preof",   ""},    /*  ITEM_PREOVERFUNC    */
	{"posof",   ""},    /*  ITEM_POSTOVERFUNC    */
	{"wf",      "ITEM_DeleteTimeWatched"},    /*  ITEM_WATCHFUNC  */
	{"usef",    ""},    /*  ITEM_USEFUNC  */
	{"atchf",   ""},     /*  ITEM_ATTACH  */
	{"dtchf",   ""},     /*  ITEM_DETACH  */
	{"attaf",   ""},     /*  ITEM_ATTACKFUNC */
	{"pickupf",   ""},   /*  ITEM_PICKUPFUNC  */
#ifdef _Item_ReLifeAct
	{"relifes",   ""},	/*ITEM_DIERELIFEFUNC*/
#endif

};

static ITEM_charDataSetting     ITEM_setmemdata={
	"",  ""          /*  ITEM_MEMO  */
};
#endif

typedef enum
{
	ITEM_INTENTRY=0,
	ITEM_CHARENTRY=1,
	ITEM_INTFUNC=2,
	ITEM_CHARFUNC=3,
}ITEM_DATATYPECATEGORY;

static struct ITEM_itemconfentry
{
	char*   entryname;
	ITEM_DATATYPECATEGORY   type;
	int     index;
	void*   func;
}ITEM_itemconfentries[]={
	{"name",        ITEM_CHARFUNC, 	ITEM_NAME,          	ITEM_checkString},
	{"secretname",  ITEM_CHARFUNC, 	ITEM_SECRETNAME,    	ITEM_checkString},
	{"effectstring",ITEM_CHARFUNC, 	ITEM_EFFECTSTRING,  	ITEM_checkString},
	{"argument",    ITEM_CHARENTRY, ITEM_ARGUMENT,          NULL},
#ifdef _ITEM_INSLAY
	{"acode",		ITEM_CHARENTRY, ITEM_TYPECODE,		NULL},
	{"inlaycode",	ITEM_CHARENTRY, ITEM_INLAYCODE,		NULL},
#endif
	{"initfunc",    ITEM_CHARENTRY, ITEM_INITFUNC,          NULL},
	{"preoverfunc", ITEM_CHARENTRY, ITEM_PREOVERFUNC,       NULL},
	{"postoverfunc",ITEM_CHARENTRY, ITEM_POSTOVERFUNC,      NULL},
	{"watchfunc",   ITEM_CHARENTRY, ITEM_WATCHFUNC,         NULL},
	{"usefunc",     ITEM_CHARENTRY, ITEM_USEFUNC,           NULL},
	{"attachfunc",  ITEM_CHARENTRY, ITEM_ATTACHFUNC,        NULL},
	{"detachfunc",  ITEM_CHARENTRY, ITEM_DETACHFUNC,        NULL},
	{"dropfunc",  	ITEM_CHARENTRY, ITEM_DROPFUNC,  	    NULL},
	{"pickupfunc",  ITEM_CHARENTRY, ITEM_PICKUPFUNC,        NULL},
#ifdef _Item_ReLifeAct
	{"relifefunc",  ITEM_CHARENTRY, ITEM_DIERELIFEFUNC,        NULL},
#endif
	{"id",			ITEM_INTENTRY,	ITEM_ID,				NULL},
	{"imagenumber",	ITEM_INTENTRY,	ITEM_BASEIMAGENUMBER,	NULL},
	{"cost",        ITEM_INTENTRY,	ITEM_COST,  			NULL},
	{"type",        ITEM_INTENTRY,	ITEM_TYPE,  			NULL},
	{"fieldtype",   ITEM_INTENTRY,	ITEM_ABLEUSEFIELD,  		NULL},
	{"target",   	ITEM_INTENTRY,	ITEM_TARGET,  			NULL},

	{"level",       ITEM_INTENTRY,	ITEM_LEVEL, 			NULL},
#ifdef _ITEM_MAXUSERNUM
	{"dambreak",       ITEM_INTENTRY,	ITEM_DAMAGEBREAK, 			NULL},
#endif

#ifdef _ITEMSET4_TXT
	{"upinums",     ITEM_INTENTRY,	ITEM_USEPILENUMS, 			NULL},
	{"campile",     ITEM_INTENTRY,	ITEM_CANBEPILE, 			NULL},
	{"nestr",		ITEM_INTENTRY,	ITEM_NEEDSTR, 			NULL},
	{"nedex",		ITEM_INTENTRY,	ITEM_NEEDDEX, 			NULL},
	{"netra",		ITEM_INTENTRY,	ITEM_NEEDTRANS, 			NULL},
	{"neprof",      ITEM_INTENTRY,	ITEM_NEEDPROFESSION, 			NULL},
#endif

#ifdef _TAKE_ITEMDAMAGE
	{"damcrushe",	ITEM_INTENTRY,	ITEM_DAMAGECRUSHE,	NULL},
	{"maxdmce",	ITEM_INTENTRY,	ITEM_MAXDAMAGECRUSHE,	NULL},
#endif

#ifdef _ADD_DEAMGEDEFC
	{"otdmags",	ITEM_INTENTRY,	ITEM_OTHERDAMAGE,	NULL},
	{"otdefcs",	ITEM_INTENTRY,	ITEM_OTHERDEFC,	NULL},
#endif
#ifdef _SUIT_ITEM
	{"nsuit",	ITEM_INTENTRY,	ITEM_SUITCODE,	NULL},				//ITEM_SUITCODE,
#endif
	{"attacknum_min",ITEM_INTENTRY,	ITEM_ATTACKNUM_MIN, 	NULL},
	{"attacknum_max",ITEM_INTENTRY,	ITEM_ATTACKNUM_MAX, 	NULL},
	{"attack",      ITEM_INTFUNC,	ITEM_MODIFYATTACK,		ITEM_getRandomValue},
	{"defence",  	ITEM_INTFUNC,	ITEM_MODIFYDEFENCE,		ITEM_getRandomValue},
	{"quick",       ITEM_INTFUNC,	ITEM_MODIFYQUICK,		ITEM_getRandomValue},

	{"hp",       	ITEM_INTFUNC,	ITEM_MODIFYHP,			ITEM_getRandomValue},
	{"mp",       	ITEM_INTFUNC,	ITEM_MODIFYMP,			ITEM_getRandomValue},
	{"luck",        ITEM_INTFUNC,	ITEM_MODIFYLUCK,		ITEM_getRandomValue},
	{"charm",       ITEM_INTFUNC,	ITEM_MODIFYCHARM,		ITEM_getRandomValue},
	{"avoid",       ITEM_INTFUNC,	ITEM_MODIFYAVOID,		ITEM_getRandomValue},
	{"attrib",      ITEM_INTENTRY,	ITEM_MODIFYATTRIB,		NULL},
	{"attribvalue", ITEM_INTENTRY,	ITEM_MODIFYATTRIBVALUE,NULL},

	{"magicid",     ITEM_INTENTRY,  ITEM_MAGICID,           NULL},
	{"magicprob",   ITEM_INTENTRY,  ITEM_MAGICPROB,         NULL},
	{"magicusemp",  ITEM_INTENTRY,  ITEM_MAGICUSEMP,        NULL},
#ifdef _ITEMSET5_TXT
	{"arr",			ITEM_INTENTRY,	ITEM_MODIFYARRANGE,		NULL},
	{"seqce",			ITEM_INTENTRY,	ITEM_MODIFYSEQUENCE,		NULL},
	{"iapi",		ITEM_INTENTRY,	ITEM_ATTACHPILE,		NULL},
	{"hirt",		ITEM_INTENTRY,	ITEM_HITRIGHT,		NULL},
#endif
#ifdef _ITEMSET6_TXT
	{"neguard",		ITEM_INTENTRY,	ITEM_NEGLECTGUARD,		NULL},
//	{"bemerge",		ITEM_INTENTRY,	ITEM_BEMERGE,		NULL},
#endif
	{"poison",      ITEM_INTFUNC,	ITEM_POISON,		ITEM_getRandomValue},
	{"paralysis",   ITEM_INTFUNC,	ITEM_PARALYSIS,		ITEM_getRandomValue},
	{"sleep",   	ITEM_INTFUNC,	ITEM_SLEEP,			ITEM_getRandomValue},
	{"stone",   	ITEM_INTFUNC,	ITEM_STONE,			ITEM_getRandomValue},
	{"drunk",   	ITEM_INTFUNC,	ITEM_DRUNK,			ITEM_getRandomValue},
	{"confusion",   ITEM_INTFUNC,	ITEM_CONFUSION,		ITEM_getRandomValue},

	{"critical",   	ITEM_INTFUNC,	ITEM_CRITICAL,		ITEM_getRandomValue},

	{"useaction",	ITEM_INTENTRY,   ITEM_USEACTION,   		NULL},

	{"dropatlogout",ITEM_INTFUNC,   ITEM_DROPATLOGOUT,   	ITEM_isstring1or0},
	{"vanishatdrop",ITEM_INTFUNC,   ITEM_VANISHATDROP,   	ITEM_isstring1or0},
	{"isovered",    ITEM_INTFUNC,   ITEM_ISOVERED,   	ITEM_isstring1or0},
	{"canpetmail",  ITEM_INTFUNC,   ITEM_CANPETMAIL,   	ITEM_isstring1or0},
	{"canmergefrom",ITEM_INTFUNC,   ITEM_CANMERGEFROM,   	ITEM_isstring1or0},
	{"canmergeto",	ITEM_INTFUNC,   ITEM_CANMERGETO,   	ITEM_isstring1or0},

    {"ingname0",    ITEM_CHARENTRY,  ITEM_INGNAME0,       NULL},
    {"ingvalue0",   ITEM_INTENTRY, ITEM_INGVALUE0,      NULL},
    {"ingname1",    ITEM_CHARENTRY,  ITEM_INGNAME1,       NULL},
    {"ingvalue1",   ITEM_INTENTRY, ITEM_INGVALUE1,      NULL},
    {"ingname2",    ITEM_CHARENTRY,  ITEM_INGNAME2,       NULL},
    {"ingvalue2",   ITEM_INTENTRY, ITEM_INGVALUE2,      NULL},
    {"ingname3",    ITEM_CHARENTRY,  ITEM_INGNAME3,       NULL},
    {"ingvalue3",   ITEM_INTENTRY, ITEM_INGVALUE3,      NULL},
    {"ingname4",    ITEM_CHARENTRY,  ITEM_INGNAME4,       NULL},
    {"ingvalue4",   ITEM_INTENTRY, ITEM_INGVALUE4,      NULL},
};


#ifdef _ITEMTBL_STAIC
ITEM_table  ITEM_tbl[28000];
#else
ITEM_table*  ITEM_tbl=NULL;
#endif

static INLINE BOOL ITEM_CHECKARRAYINDEX(int index)
{
	if( ITEM_itemnum<=(index) || (index)<0 )return FALSE;
	return TRUE;
}

INLINE BOOL _ITEM_CHECKINDEX( char *file, int line, int index)
{
	if( !ITEM_CHECKARRAYINDEX(index) ) {
		if( index != -1 ) {
			print( "err %s:%d \n", file, line);
		}
		return FALSE;
	}
	if( ITEM_item[index].use == FALSE ) {
		return FALSE;
	}
	return TRUE;
}

static INLINE int ITEM_CHECKINTDATAINDEX(int index)
{
	if( ITEM_DATAINTNUM <= index || index < 0 )return FALSE;
	return TRUE;
}

static INLINE int ITEM_CHECKCHARDATAINDEX(int index)
{
	if( ITEM_DATACHARNUM<=index || index<0 )return FALSE;
	return TRUE;
}

BOOL ITEM_initExistItemsArray( int num )
{
	int     i;

	BOOL ITEM_checksetdata( void );
	if( ITEM_checksetdata() == FALSE )return FALSE;

	ITEM_itemnum = num;

	ITEM_item = allocateMemory( sizeof( ITEM_exists ) * num );
	if( ITEM_item == NULL ) return FALSE;

	for( i = 0 ; i < num ; i ++ ){
		memset( &ITEM_item[i], 0 , sizeof( ITEM_exists ));
		ITEM_item[i].use = FALSE;
	}

	return TRUE;
}

BOOL ITEM_endExistItemsArray( void )
{
	freeMemory( ITEM_item );
	return TRUE;
}

int _ITEM_initExistItemsOne( char *file, int line, ITEM_Item* itm )
{
	int i;
	int itemID=-1;
	static int Sindex=1;
	itemID = itm->data[ITEM_ID];
	if( ITEM_CHECKITEMTABLE(itm->data[ITEM_ID] ) == FALSE){
		print("CHAR itemData err Item_id:%d=%s%d!!\n", itm->data[ITEM_ID], file, line);
		return -1;
	}
	for( i = 0 ; i < ITEM_itemnum ; i ++ ) {
		Sindex++;
		if( Sindex >= ITEM_itemnum ) Sindex = 1;
		if( Sindex < 1 ) Sindex = 1;
		if( !ITEM_item[Sindex].use){
			int	charaindex;
			charaindex = ITEM_item[Sindex].itm.workint[ITEM_WORKCHARAINDEX];
			if( CHAR_CHECKINDEX( charaindex) &&
				CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
				int	j;
				for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
					if( CHAR_getItemIndex( charaindex, j) == Sindex) {
						print( "item.c: error! chara have this item charaindex[%d] "
								"itemindex[%d] Name(%s)POS(%d)NAME(%s)\n",
								charaindex, Sindex,
								CHAR_getUseName( charaindex ), j,
								ITEM_item[Sindex].itm.string[ITEM_NAME].string );
						print( "from %s:%d\n", file, line);
						ITEM_item[Sindex].use = TRUE;
						break;
					}
				}
				if( j != CHAR_MAXITEMHAVE ) continue;
			}
			memcpy( &ITEM_item[Sindex].itm , itm , sizeof( ITEM_Item ) );
			ITEM_item[Sindex].use = TRUE;
			{
				BOOL    (*initfunc)(ITEM_Item*)=NULL;
				initfunc = (BOOL(*)(ITEM_Item*)) getFunctionPointerFromName( itm->string[ITEM_INITFUNC].string );
				if( initfunc ){
					if( initfunc( &ITEM_item[Sindex].itm ) == FALSE ){
						ITEM_item[Sindex].use = FALSE;
						fprint( "Error:Can't init item\n" );
						return -1;
					}
				}
			}
			ITEM_UseItemnum++;
			ITEM_constructFunctable( Sindex);
			return Sindex;
		}
	}
	fprint( "错误:物品已满\n" );
	return -1;
}

void _ITEM_endExistItemsOne( int index , char *file, int line)
{
	int	i,j;
	int  hitcnt = 0;
	int     playernum = CHAR_getPlayerMaxNum();

	if( !ITEM_CHECKINDEX(index)) return;

	for( i = 0; i < playernum; i ++ ) {
		if( CHAR_CHECKINDEX( i)) {
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
				if( CHAR_getItemIndex( i, j) == index) {
					print( "warning !! player have this item:%d call from [%s:%d](%s)(%s)\n",
							index, file,line, CHAR_getUseName(i),ITEM_item[index].itm.string[ITEM_NAME].string );
					hitcnt ++;
					if( hitcnt > 1 ) {
						print( "ITEM_INDEX(%d) duplicate!!\n", index);
					}
				}
			}
		}
	}

	if( hitcnt < 1 ){
		ITEM_item[index].use = FALSE;
		ITEM_item[index].itm.workint[ITEM_WORKCHARAINDEX] = -1;
		ITEM_UseItemnum--;
	}
}

INLINE int ITEM_getIntStrict( int index ,ITEM_DATAINT element,int* error)
{
	if(!ITEM_CHECKINDEX(index))return *error=FALSE;
	if(!ITEM_CHECKINTDATAINDEX(element))return *error=FALSE;
	return ITEM_item[index].itm.data[element];
}

INLINE int _ITEM_getInt( char *file, int line, int index ,ITEM_DATAINT element)
{
	if(!ITEM_CHECKINDEX(index)){
		return -1;
	}
	if(!ITEM_CHECKINTDATAINDEX(element))	{
		return -1;
	}
	return ITEM_item[index].itm.data[element];
}

INLINE int ITEM_setIntStrict( int index ,ITEM_DATAINT element,int data,
							  int* error)
{
	int buf;
	if(!ITEM_CHECKINDEX(index))
		return *error=FALSE;
	if(!ITEM_CHECKINTDATAINDEX(element))
		return *error=FALSE;
	buf = ITEM_item[index].itm.data[element];
	ITEM_item[index].itm.data[element] = data;
	return buf;
}

INLINE int _ITEM_setInt( char *file, int line, int index ,ITEM_DATAINT element, int data)
{
	int buf;
	if( !ITEM_CHECKINDEX( index) ){
		return -1;
	}

	if( !ITEM_CHECKINTDATAINDEX( element) ){
		return -1;
	}

	buf = ITEM_item[ index].itm.data[ element];
	ITEM_item[ index].itm.data[ element] = data;
	return buf;
}

INLINE char* ITEM_getChar( int index ,ITEM_DATACHAR element )
{
	if(!ITEM_CHECKINDEX(index))return NULL;
	if(!ITEM_CHECKCHARDATAINDEX(element))return NULL;
	return ITEM_item[index].itm.string[element].string;
}

INLINE BOOL ITEM_setChar( int index ,ITEM_DATACHAR element , char* new)
{
	if(!ITEM_CHECKINDEX(index))return FALSE;
	if(!ITEM_CHECKCHARDATAINDEX(element))return FALSE;
	strcpysafe( ITEM_item[index].itm.string[element].string,
				sizeof( ITEM_item[index].itm.string[element].string ),
				new );
	return TRUE;
}

INLINE int ITEM_getWorkInt( int index ,ITEM_WORKDATAINT element)
{
	if( !ITEM_CHECKINDEX(index) ) return -1;
	if( element >= ITEM_WORKDATAINTNUM || element < 0 ) return -1;
	return ITEM_item[index].itm.workint[element];
}

INLINE int ITEM_setWorkInt( int index ,ITEM_WORKDATAINT element, int data)
{
	int buf;
	if( !ITEM_CHECKINDEX( index) ) return -1;
	if( element >= ITEM_WORKDATAINTNUM || element < 0 ) return -1;

	buf = ITEM_item[index].itm.workint[element];
	ITEM_item[index].itm.workint[element] = data;
	return buf;
}

INLINE int ITEM_getITEM_itemnum( void )
{
	return ITEM_itemnum;
}

INLINE int ITEM_getITEM_UseItemnum( void )
{
	return ITEM_UseItemnum;
}


INLINE BOOL ITEM_getITEM_use( int index )
{
	if( !ITEM_CHECKINDEX(index))return FALSE;
	return ITEM_item[index].use;
}

void ITEM_constructFunctable( int itemindex )
{
	int     i;
	if( !ITEM_CHECKINDEX(itemindex))return;

	for( i=ITEM_FIRSTFUNCTION; i < ITEM_LASTFUNCTION ; i++ )	{
		ITEM_item[itemindex].itm.functable[i-ITEM_FIRSTFUNCTION]
			= getFunctionPointerFromName( ITEM_getChar( itemindex,i ) );
	}
}

void* ITEM_getFunctionPointer( int itemindex, int functype )
{
	if( !ITEM_CHECKINDEX(itemindex) )return NULL;
	if( functype < ITEM_FIRSTFUNCTION  || functype >= ITEM_LASTFUNCTION ){
		print( "类型错误:%d\n", functype);
		return NULL;
	}
	return ITEM_item[itemindex].itm.
		functable[functype-ITEM_FIRSTFUNCTION];
}

int ITEM_getItemMaxIdNum( void)
{
	return ITEM_tblen;
}

BOOL ITEM_checksetdata( void )
{
	int i;
	char*   strings[ITEM_DATAINTNUM + ITEM_DATACHARNUM + 1];
#ifdef _SIMPLIFY_ITEMSTRING
	int num=0;
	for(i = 0; i < arraysizeof( ITEM_setintdata) ; i ++ )
		strings[ num++ ] = ITEM_setintdata[i].dumpchar;
	for( i = 0 ; i < arraysizeof( ITEM_setchardata) ; i ++ )
		strings[ num++ ] = ITEM_setchardata[i].dumpchar;
	strings[ITEM_DATAINTNUM+ITEM_DATACHARNUM] = ITEM_setmemdata.dumpchar;

	if( ! checkStringsUnique( strings, num,1 ) ){
		fprint( "ITEM_set????data is overlapped.\nIt is not allowed\n");
		return FALSE;
	}
#else
	for(i = 0; i < ITEM_DATAINTNUM ; i ++ )
		strings[i] = ITEM_setintdata[i].dumpchar;
	for( i = 0 ; i < ITEM_DATACHARNUM ; i ++ )
		strings[ITEM_DATAINTNUM+i] = ITEM_setchardata[i].dumpchar;
	

	strings[ITEM_DATAINTNUM+ITEM_DATACHARNUM] = ITEM_setmemdata.dumpchar;

	if( ! checkStringsUnique( strings, arraysizeof( strings ),1 ) ){
		fprint( "ITEM_set????data is overlapped.\nIt is not allowed\n");
		return FALSE;
	}
#endif
	return TRUE;
}

static char    ITEM_dataString[STRINGBUFSIZ];
char* ITEM_makeStringFromItemIndex( int index, int mode )
{
	if( 0 <= index && index < ITEM_itemnum && ITEM_item[index].use == TRUE)
		;
	else
		return NULL;
	return ITEM_makeStringFromItemData( &ITEM_item[index].itm, mode );
}

#ifdef _SIMPLIFY_ITEMSTRING2
BOOL CHECK_HaveBeSave( int itemID, int oneNum, int type )
{
	if( ITEM_ID == type ) return TRUE;
	if( ITEMTBL_getInt( itemID, type) == oneNum ) return FALSE;
	return TRUE;
}
#endif

char*   ITEM_makeStringFromItemData( ITEM_Item* one, int mode )
{
	int     i;
	char	delim1 = '\0';
	char	delim2 = '\0';
	int     strlength=0;

#ifdef _SIMPLIFY_ITEMSTRING2
	int itemID = one->data[ ITEM_ID];
	if( !ITEM_CHECKITEMTABLE( itemID) ) return NULL;
#endif
	if( mode == 0 ) {
		delim1 = '=';	delim2 = '|';
	}else {
		delim1 = ':';	delim2 = ',';
	}

#ifdef _SIMPLIFY_ITEMSTRING
	for(i = 0; i < arraysizeof( ITEM_setintdata) ; i ++ ){
		char    linedata[128];
#ifdef _SIMPLIFY_ITEMSTRING2
		if( !CHECK_HaveBeSave( itemID, one->data[ ITEM_setintdata[i].table], ITEM_setintdata[i].table) ) continue;
#endif
		snprintf( linedata , sizeof(linedata),
				  "%s%c%d%c", ITEM_setintdata[i].dumpchar, delim1,
				  one->data[ ITEM_setintdata[i].table], delim2 );
		strcpysafe( &ITEM_dataString[strlength], sizeof( ITEM_dataString ) - strlength, linedata );
		strlength += strlen( linedata );
	}

	for( i = 0 ; i < arraysizeof( ITEM_setchardata) ; i ++ ){
		char    linedata[128];
		char    escapebuffer[128];
		if( strlen( one->string[ ITEM_setchardata[i].table].string) == 0 ) continue;

		snprintf( linedata , sizeof(linedata),
				"%s%c%s%c", ITEM_setchardata[i].dumpchar,delim1,
				makeEscapeString( one->string[ITEM_setchardata[i].table].string, escapebuffer,
				sizeof(escapebuffer)), delim2);
		strcpysafe( &ITEM_dataString[strlength], sizeof( ITEM_dataString ) - strlength, linedata );
		strlength += strlen( linedata );
	}
#else
	for( i = 0 ; i < ITEM_DATAINTNUM ; i ++ ) {
		char    linedata[128];
		snprintf( linedata , sizeof(linedata),
				  "%s%c%d%c", ITEM_setintdata[i].dumpchar, delim1, one->data[i], delim2 );
		strcpysafe( &ITEM_dataString[strlength], sizeof( ITEM_dataString ) - strlength, linedata );
		strlength += strlen( linedata );
	}

	for( i = 0 ; i < ITEM_DATACHARNUM ; i ++ ){
		char    linedata[128];
		char    escapebuffer[128];
		if( strlen( one->string[i].string) != 0 ) {
			snprintf( linedata , sizeof(linedata),
					"%s%c%s%c", ITEM_setchardata[i].dumpchar,delim1,
					makeEscapeString(one->string[i].string,escapebuffer,
					sizeof(escapebuffer)), delim2);
			strcpysafe( &ITEM_dataString[strlength], sizeof( ITEM_dataString ) - strlength, linedata );
			strlength += strlen( linedata );
		}
	}
#endif
	dchop( ITEM_dataString, (mode == 0 ) ? "|" : ",");
	return ITEM_dataString;
}

BOOL ITEM_makeExistItemsFromStringToArg( char* src , ITEM_Item* item, int mode )
{
	int     readindex=1;
	char	delim1[2];
	char	delim2[2];

	if( mode == 0 ) {
		strcpy( delim1, "=");
		strcpy( delim2, "|");
	}
	else {
		strcpy( delim1, ":");
		strcpy( delim2, ",");
	}
#ifdef _SIMPLIFY_ITEMSTRING
	{

		int itemID=-1;
		while( 1 ){
			char    linebuf[512];
			char firstToken[256];
			char secondToken[256];
			if( getStringFromIndexWithDelim( src ,delim2 , readindex, linebuf, sizeof( linebuf)) == FALSE ) break;

			readindex++;
			if( getStringFromIndexWithDelim( linebuf, delim1, 1, firstToken, sizeof( firstToken )) == FALSE )return FALSE;
			if( strcmp( firstToken, "id" ) ) continue;
			strcpysafe( secondToken , sizeof( secondToken ), linebuf + strlen( firstToken) + strlen( delim1) );
			itemID = atoi( secondToken);
			if( !ITEM_CHECKITEMTABLE( itemID) ) return FALSE;
			ITEM_getDefaultItemData( itemID, item);
			break;
		}
		readindex =1;
	}
#else
	ITEM_getDefaultItemSetting(item);
#endif

	while( 1 ){
		BOOL    ret;
		char    linebuf[512];

		char firstToken[256];
		char secondToken[256];
		int     i;

		ret = getStringFromIndexWithDelim( src ,delim2 , readindex,linebuf, sizeof(linebuf) );
		if( ret == FALSE )
			break;
		ret = getStringFromIndexWithDelim( linebuf, delim1,  1, firstToken, sizeof( firstToken ) );
		if( ret == FALSE )  return FALSE;
		strcpysafe( secondToken , sizeof( secondToken ),
					linebuf + strlen(firstToken) + strlen(delim1) );
#ifdef _SIMPLIFY_ITEMSTRING
		for(i = 0; i < arraysizeof( ITEM_setintdata) ; i ++ ){
			if( strcmp( firstToken , ITEM_setintdata[i].dumpchar) ) continue;
			item->data[ITEM_setintdata[i].table] = atoi( secondToken );
			goto NEXT;
		}

		for(i = 0; i < arraysizeof( ITEM_setchardata) ; i ++ ){
			if( strcmp(firstToken ,ITEM_setchardata[i].dumpchar ) ) continue;
			strcpysafe( item->string[ ITEM_setchardata[i].table].string,
						sizeof(item->string[ITEM_setchardata[i].table].string),
						makeStringFromEscaped(secondToken));
			goto NEXT;
		}
#else
		for( i = 0 ; i < ITEM_DATAINTNUM ; i ++ ){
			if( strcmp(firstToken ,ITEM_setintdata[i].dumpchar) == 0 ){
				item->data[i] = atoi( secondToken );
				goto NEXT;
			}
		}

		for( i = 0 ; i < ITEM_DATACHARNUM ; i ++ ){
			if( strcmp(firstToken ,ITEM_setchardata[i].dumpchar ) == 0 ){
				strcpysafe( item->string[i].string,
							sizeof(item->string[i].string),
							makeStringFromEscaped(secondToken));
				goto NEXT;
			}
		}
#endif
		fprint( "??? : %s[%s]\n" , src , firstToken );

	NEXT:
		readindex++;
	}

	return TRUE;
}

void ITEM_getDefaultItemSetting(ITEM_Item* itm)
{
	int     i;
	memset( itm, 0 , sizeof( ITEM_Item ));
#ifdef _SIMPLIFY_ITEMSTRING
	for(i = 0; i < arraysizeof( ITEM_setintdata) ; i ++ ){
		itm->data[ITEM_setintdata[i].table] = ITEM_setintdata[i].defaults;
	}
	for(i = 0; i < arraysizeof( ITEM_setchardata) ; i ++ ){
		strcpysafe( itm->string[ITEM_setchardata[i].table].string,
				   sizeof( itm->string[ITEM_setchardata[i].table].string),
				   ITEM_setchardata[i].defaults);
	}
#else
	for(i=0;i<ITEM_DATAINTNUM;i++)
		itm->data[i] = ITEM_setintdata[i].defaults;
	for(i=0;i<ITEM_DATACHARNUM;i++)
		strcpysafe(itm->string[i].string,
				   sizeof(itm->string[i].string),
				   ITEM_setchardata[i].defaults);
#endif

	strcpysafe( itm->string[ITEM_WATCHFUNC].string,
		sizeof(itm->string[ITEM_WATCHFUNC].string), "ITEM_DeleteTimeWatched" );


	for(i=0;i<ITEM_WORKDATAINTNUM;i++){
		itm->workint[i] = -1;
	}

}

#ifdef _SIMPLIFY_ITEMSTRING
void ITEM_getDefaultItemData( int itemID, ITEM_Item* itm)
{
	int     i;
//	memset( itm, 0 , sizeof( ITEM_Item ));

	for(i=0;i<ITEM_DATAINTNUM;i++){
		itm->data[i] = ITEMTBL_getInt( itemID, i);
	}
	for(i=0;i<ITEM_DATACHARNUM;i++){
		strcpysafe( itm->string[i].string, sizeof( itm->string[i].string),
				   ITEMTBL_getChar( itemID, i) );
	}
	for(i=0;i<ITEM_WORKDATAINTNUM;i++){
		itm->workint[i] = -1;
	}

	strcpysafe( itm->string[ITEM_WATCHFUNC].string, sizeof( itm->string[ITEM_WATCHFUNC].string),
				   "ITEM_DeleteTimeWatched" );

}
#endif

static int ITEM_getRandomValue( char*  string,int* randomwidth, int num )
{
	int   minvalue;
	int   maxvalue;
	char	token[64];
	int		ret;
#if 1
	*randomwidth = 0;

	ret = getStringFromIndexWithDelim( string,",",num -1,token, sizeof( token));
	if( ret == FALSE ) {
		return 0;
	}
	minvalue = atoi( token);
	ret = getStringFromIndexWithDelim( string,",",num ,token, sizeof( token));
	if( ret == FALSE ) {
		return minvalue;
	}
	maxvalue = atoi( token);
	*randomwidth = ABS( maxvalue - minvalue);
	return min( minvalue, maxvalue);

#else
	minvalue = atoi( string );
	startmax = index( string,',' );
	if( startmax != NULL ){
		maxvalue = atoi(startmax+1);
		*randomwidth = ABS(maxvalue - minvalue);
		return min(minvalue,maxvalue);
	}
	*randomwidth = 0;
	return minvalue;
#endif
}
static int ITEM_isstring1or0( char*  string,int* randomwidth, int num )
{
	char	token[64];
	int		ret;

	ret = getStringFromIndexWithDelim( string,",",num -1,token, sizeof( token));
	if( ret == FALSE ) {
		return FALSE;
	}

	return isstring1or0( token);
}

static char* ITEM_checkString( char* string )
{
#define ITEM_STRINGLEN 64
	if( strlen(string) >= ITEM_STRINGLEN )
		print( "Over %d:[%s]\n",ITEM_STRINGLEN ,string );
	return string;
#undef ITEM_STRINGLEN
}

BOOL ITEM_readItemConfFile( char* filename )
{
	FILE*   f;
	char    line[512];
	char	token[64];
	int     linenum=0;
	int		i;
	int		maxid=0, itemid;
	int		ret;
	int     intdata[ITEM_DATAINTNUM];

	f = fopen(filename,"r");
	if( f == NULL ){
		print( "不能打开文件\n");
		return FALSE;
	}
#ifdef _ITEMSET2_ITEM

#define ITEM_ID_TOKEN_INDEX 17

#else

#ifdef _ITEM_MAXUSERNUM
#define ITEM_ID_TOKEN_INDEX 15
#endif

#endif
	while( fgets( line, sizeof( line ), f ) ){
		linenum ++;
		if( line[0] == '#' )continue;        /* comment */
		if( line[0] == '\n' )continue;       /* none    */
		chomp( line );

		ret = getStringFromIndexWithDelim( line, ",", ITEM_ID_TOKEN_INDEX, token, sizeof(token));
		if( ret == FALSE ){
			fprint("文件秩序错误:%s 第:%d行\n",filename,linenum);
			continue;
		}
		itemid = atoi( token);
		if( itemid > maxid ){
            maxid = itemid;
        }
	}

	if( maxid <=0 ) {
		print( "最大ID错误\n");
		fclose(f);
		return FALSE;
	}
	
	if( fseek( f, 0, SEEK_SET ) == -1 ){
		fprint( "搜索错误\n" );
		fclose(f);
		return FALSE;
	}
#ifdef _IMPOROVE_ITEMTABLE
	ITEM_tblen = linenum+1;
	print( "物品数 %d - 大小:%d\n", linenum, sizeof(ITEM_table));
	if( ITEM_TransformList != NULL )
		freeMemory( ITEM_TransformList);
	defitemtbls = (maxid+1);
	ITEM_TransformList = allocateMemory( sizeof(sITEM_transformList) * defitemtbls );
	for( i = 0 ; i < (maxid+1); i ++ ) {
		ITEM_TransformList[i].use = FALSE;
	}
#else
	print( "物品最大ID %d", maxid);
	ITEM_tblen = maxid + 1;
#endif

#ifdef _ITEMTBL_STAIC
	for( i = 0 ; i < 28000 ; i ++ ) {
		ITEM_tbl[i].use = FALSE;
	}
#else
	if( ITEM_tbl != NULL )
		freeMemory( ITEM_tbl);
	ITEM_tbl = allocateMemory( sizeof(ITEM_table) * ITEM_tblen );

	if( ITEM_tbl == NULL ){
		fprint( "无法分配内存 %d\n" , sizeof(ITEM_table)*ITEM_tblen );
		fclose( f );
		return FALSE;
	}
	for( i = 0 ; i < ITEM_tblen ; i ++ ) {
		ITEM_tbl[i].use = FALSE;
	}
#endif
	linenum = 0;
	while( fgets( line, sizeof( line ), f ) ){
		linenum ++;
		if( line[0] == '#' )continue;        /* comment */
		if( line[0] == '\n' )continue;       /* none    */
		chomp( line );
		replaceString( line, '\t' , ' ' );

{
		char    buf[256];
		for( i = 0; i < strlen( line); i ++) {
			if( line[i] != ' ' ) {
				break;
			}
			strcpy( buf, &line[i]);
		}
		if( i != 0 ) {
			strcpy( line, buf);
		}
}
{
		char    token[256];
		int     ret;
		int		readpos = 1;
		BOOL	dataerror = FALSE;
		ITEM_Item	itm;

		ITEM_getDefaultItemSetting(&itm);

		for( i=0 ; i< ITEM_DATAINTNUM ; i++ ) {
			intdata[i] = 0;
		}
		itemid = 0;
		for( i = 0 ; i < arraysizeof( ITEM_itemconfentries) ; i ++ ){
			ret = getStringFromIndexWithDelim( line,",",readpos,token, sizeof(token));
			if( ret == FALSE ) {
#ifdef _Item_ReLifeAct
				{
					char buf[256];
					char buf1[256];
					sscanf( line,"%s,%s", buf, buf1);
					print("[ITEM data Error] tbl=%d:%s line:%d[%s]\n",
						readpos, ITEM_itemconfentries[i].entryname, linenum, line);
				}
#else
				fprint("Syntax Error tki:%d file:%s line:%d[%s]\n",
                       i , filename,linenum,line);
#endif
				dataerror = TRUE;
				break;
			}

			if( readpos == ITEM_ID_TOKEN_INDEX )
				itemid = atoi( token);

			readpos ++;

			if( strlen( token ) != 0 ) {
				switch(ITEM_itemconfentries[i].type){
				  case ITEM_INTENTRY:
					itm.data[ITEM_itemconfentries[i].index] = atoi( token );
					break;
				  case ITEM_CHARENTRY:
					strcpysafe( itm.string[ITEM_itemconfentries[i].index].string,
								sizeof(itm.string[ITEM_itemconfentries[i].index].string),
								token);
					break;
				  case ITEM_INTFUNC:
				  {
					int (*intfunction)(char*,int*, int );
					intfunction = ITEM_itemconfentries[i].func;
					itm.data[ITEM_itemconfentries[i].index]
								= intfunction(line,
									  		&intdata[ITEM_itemconfentries[i].index],readpos);
					if( intfunction == ITEM_getRandomValue) readpos ++;

				  }
					break;
				  case ITEM_CHARFUNC:
				  {
					char* (*charfunction)(char* );
					charfunction = ITEM_itemconfentries[i].func;
					strcpysafe( itm.string[ITEM_itemconfentries[i].index].string,
								sizeof(itm.string[ITEM_itemconfentries[i].index].string),
								charfunction( token));
					break;
				  }
				  default:
					break;
				}
			}
		}
		if( !dataerror) {
#ifdef _IMPOROVE_ITEMTABLE
			int ListNum = itemid;
			itemid = totalitemtbls;
#endif
			if( itemid >= ITEM_tblen ){
				print( "ITEM_tbl full:%d err !!\n" , itemid );
			}else if( ITEM_tbl[itemid].use == TRUE ) {
				fprint( "Duplicate Itemid %d.ignore\n" , itemid );
			}else{
				if( itm.string[ITEM_SECRETNAME].string[0] == '\0') {
					fprint( "ERROR: ID %d item doesn't have secretname\n"
							,itemid );
					memcpy( &itm.string[ITEM_SECRETNAME].string,
							&itm.string[ITEM_NAME].string,
							sizeof(itm.string[ITEM_NAME].string));
				}
{
				int	attacknum_min, attacknum_max;
				attacknum_min = itm.data[ITEM_ATTACKNUM_MIN];
				attacknum_max = itm.data[ITEM_ATTACKNUM_MAX];
				if( attacknum_min == 0 ) attacknum_min = attacknum_max;
				itm.data[ITEM_ATTACKNUM_MIN] = min( attacknum_min,
													attacknum_max);
				itm.data[ITEM_ATTACKNUM_MAX] = max( attacknum_min,
													attacknum_max);
}
				memcpy( &ITEM_tbl[itemid].itm, &itm, sizeof(ITEM_Item));				
				ITEM_tbl[itemid].use = TRUE;

				for( i=0 ; i< ITEM_DATAINTNUM ; i++ ){
					ITEM_tbl[itemid].randomdata[i] = intdata[i];
				}
#ifdef _IMPOROVE_ITEMTABLE

				if( ITEM_TransformList[ ListNum].use != 0 ){
					ITEM_tbl[itemid].use = TRUE;
					ITEM_TransformList[ ListNum].use = 0;
				}else {
					ITEM_TransformList[ ListNum].Si = totalitemtbls;
					ITEM_TransformList[ ListNum].use = TRUE;
				}
				totalitemtbls++;
#endif
			}
		}
}
	}
	fclose(f);
	return TRUE;
}

CHAR_EquipPlace ITEM_getEquipPlace( int charaindex, int itmid )
{
	ITEM_CATEGORY   cat;
	cat = ITEM_getInt( itmid , ITEM_TYPE );
	switch( cat ){
	case ITEM_FIST:
	case ITEM_SPEAR:
	case ITEM_AXE:
	case ITEM_CLUB:
//	case ITEM_SHIELD:
	case ITEM_BOOMERANG:
	case ITEM_BREAKTHROW:
	case ITEM_BOUNDTHROW:

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
		{	
			int i=0;

			// 勇士职业技能二刀流
			for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
				char *skill_name;
				// 技能ID
				int skillid = CHAR_getCharSkill( charaindex, i);
				int Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
				if( Pskillid <= 0 ) continue;

				// 技能名称
				skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
				if( skill_name == NULL ) continue;
	
				// 二刀流
				if( (strcmp( skill_name , "PROFESSION_DUAL_WEAPON" )) == 0 ){
					int get_item = BATTLE_GetWepon( charaindex );

					if( get_item != ITEM_BOW ){
						int right_hand = CHAR_getItemIndex( charaindex, CHAR_ARM );
						int left_hand  = CHAR_getItemIndex( charaindex, CHAR_EQSHIELD );

						if( right_hand < 0 )	return CHAR_ARM;
						else{
							if( left_hand < 0 )	return CHAR_EQSHIELD;
							else				return CHAR_ARM;	
						}
					}
				} 
			}
		}
#endif
		return CHAR_ARM;
		break;
	case ITEM_HELM:
		return CHAR_HEAD;
		break;
	case ITEM_ARMOUR:
		return CHAR_BODY;
		break;
	case ITEM_BRACELET:
	case ITEM_MUSIC:
	case ITEM_NECKLACE:
	case ITEM_RING:
	case ITEM_BELT:
	case ITEM_EARRING:
	case ITEM_NOSERING:
	case ITEM_AMULET:
		return CHAR_DECORATION1;
		break;
	case ITEM_BOW:
#ifdef _ITEM_EQUITSPACE
		if( CHAR_getItemIndex( charaindex, CHAR_EQSHIELD ) > 0 )	return -1;
#endif
		return CHAR_ARM;
		break;

#ifdef _ITEM_EQUITSPACE
	case ITEM_WBELT:
		return CHAR_EQBELT;
		break;
	case ITEM_WSHIELD:
		if( BATTLE_GetWepon( charaindex ) != ITEM_BOW )
			return CHAR_EQSHIELD;
		break;
	case ITEM_WSHOES:
		return CHAR_EQSHOES;
		break;
#endif

#ifdef _EQUIT_NEWGLOVE
	case ITEM_WGLOVE:
		return CHAR_EQGLOVE;
		break;
#endif
	default:
		break;
	}
	return -1;
}
#define ITEMSTRINGBUFSIZ    512
static char ITEM_itemStatusStringBuffer[ITEMSTRINGBUFSIZ];
char*  ITEM_makeItemStatusString( int haveitemindex, int itemindex )
{
	char    escapename[256];
	char    escapeeffectstring[256];
	char    paramshow[256];
	int     leaklevel;
	int		itemcolor = 0;
	int 	flg;
	char	INGNAME0[4];
	char	INGNAME1[4];

	if( !ITEM_CHECKINDEX(itemindex) ) return NULL;
	leaklevel = ITEM_getInt(itemindex,ITEM_LEAKLEVEL);
	 
	//if( leaklevel >= 1 ) 	// Nuke debug
	{
		makeEscapeString( ITEM_getChar(itemindex, ITEM_SECRETNAME),
						  escapename, sizeof(escapename) );
		/*if( leaklevel >= 2  )
		{
			static struct Showparamint{
				char*   name;
				int     intindex;
			}showparamint[]={
				{ "ATK" , ITEM_MODIFYATTACK },
				{ "DEF" , ITEM_MODIFYDEFENCE },
				{ "HP"  , ITEM_MODIFYHP },
				{ "MP"  , ITEM_MODIFYMP },
				{ "QUICK" , ITEM_MODIFYQUICK },
				{ "LUCK" , ITEM_MODIFYLUCK },
				{ "CHARM" , ITEM_MODIFYCHARM },
			};
			int     i;
			int     stringlen=0;

			paramshow[0] = '\0';

			for( i = 0 ; i < arraysizeof( showparamint ); i ++ ){
				int     value;
				char    sign;
				char    tmpbuf[128];
				value = ITEM_getInt(itemindex,showparamint[i].intindex);

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
				
				

#endif

				if( value == 0 )continue;
				else if( value > 0 ) sign = '+';
				else sign = '-';

				snprintf(  tmpbuf,sizeof( tmpbuf ), "%s%c%d ",
						   showparamint[i].name,sign,ABS(value) );

				strcpysafe( paramshow + stringlen ,
							sizeof(paramshow) - stringlen, tmpbuf );
				stringlen +=strlen(tmpbuf);
				if( stringlen >= arraysizeof( paramshow ))
					break;
			}
			dchop( paramshow , " " );
		}
		else*/
			paramshow[0] = '\0';

	}
	/*else{
		if(strlen(ITEM_getChar(itemindex,ITEM_SECRETNAME)) > 0){
			makeEscapeString(ITEM_getChar(itemindex,ITEM_SECRETNAME),escapename,sizeof(escapename));
		}
		else makeEscapeString( ITEM_getChar(itemindex,ITEM_NAME),escapename,sizeof(escapename));
		paramshow[0] = '\0';
	}*/

	makeEscapeString( ITEM_getChar( itemindex, ITEM_EFFECTSTRING),
						escapeeffectstring,
						sizeof(escapeeffectstring));
	itemcolor = CHAR_COLORWHITE;
	if( strlen( ITEM_getChar( itemindex, ITEM_CDKEY)) != 0 ) {
		itemcolor = CHAR_COLORGREEN;
	}else {
		if( ITEM_getInt( itemindex, ITEM_MERGEFLG) ) {
			itemcolor = CHAR_COLORYELLOW;
		}
	}
	flg = 0;
	if( ITEM_getInt( itemindex, ITEM_CANPETMAIL)) {
		flg |= 1 << 0;
	}
	if( ITEM_getInt( itemindex, ITEM_CANMERGEFROM)) {
		flg |= 1 << 1;
	}
	if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_DISH ) {
		flg |= 1 << 2;
	}
#ifdef _ITEM_INSLAY
	if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_METAL ) {
		flg |= 1 << 3;
	}else if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_JEWEL ) {
		flg |= 1 << 4;
	}
#endif
#ifdef _PETSKILL_FIXITEM
	if( ITEM_getInt( itemindex, ITEM_DAMAGECRUSHE) 
		!= ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE) ){
		flg |= 1 << 5;
	}
#endif

#ifdef _ALCHEMIST
	strcpy( INGNAME0, ITEM_getChar( itemindex, ITEM_INGNAME0) );
	strcpy( INGNAME1, ITEM_getChar( itemindex, ITEM_INGNAME1) );
	if( (INGNAME0[0] != NULL && INGNAME1[0] != NULL) || (INGNAME0[0] == NULL) ) {
		strcpy( INGNAME0, "杂" );
	}
#endif

	if( haveitemindex == -1 ) {
#ifdef _ADD_SHOW_ITEMDAMAGE  // WON ADD 显示物品耐久度
		char buff1[256];
		int crushe	= ITEM_getInt( itemindex, ITEM_DAMAGECRUSHE);
		int maxcrushe = ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE);
		if(crushe < 1) crushe = 1;
		if(maxcrushe < 1){
			sprintf(buff1, "不会损坏");
		}else{
			maxcrushe = maxcrushe/1000;
			crushe = crushe/1000;
			if( maxcrushe <= 0 ) maxcrushe = 1;
			snprintf(buff1, sizeof(buff1), "%d%%", (int)((crushe*100)/maxcrushe) );
		}

		snprintf(ITEM_itemStatusStringBuffer,
				 sizeof( ITEM_itemStatusStringBuffer),
#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
				"%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d|%s",
#else
				"%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d",
#endif
#else
				 "%s|%s|%d|%s|%d|%d|%d|%d|%d|%s",
#endif
				 escapename, paramshow,
				 itemcolor, escapeeffectstring ,
				 ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
				 ITEM_getInt( itemindex, ITEM_ABLEUSEFIELD),
				 ITEM_getInt( itemindex, ITEM_TARGET),
				 ITEM_getInt( itemindex, ITEM_LEVEL),
				  flg,
				  buff1
#ifdef _ITEM_PILENUMS
				 ,ITEM_getInt( itemindex, ITEM_USEPILENUMS)
#ifdef _ALCHEMIST
				 ,INGNAME0 //,ITEM_getInt( itemindex, ITEM_ALCHEMIST)
#endif
#endif
				  );
#else
		snprintf(ITEM_itemStatusStringBuffer,
				 sizeof( ITEM_itemStatusStringBuffer),
				 "%s|%s|%d|%s|%d|%d|%d|%d|%d",
				 escapename, paramshow,
				 itemcolor, escapeeffectstring ,
				 ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
				 ITEM_getInt( itemindex, ITEM_ABLEUSEFIELD),
				 ITEM_getInt( itemindex, ITEM_TARGET),
				 ITEM_getInt( itemindex, ITEM_LEVEL),
				  flg
				  );
#endif
	}else {
#ifdef _ADD_SHOW_ITEMDAMAGE  // WON ADD 显示物品耐久度
		char buff1[256];
		int crushe	= ITEM_getInt( itemindex, ITEM_DAMAGECRUSHE);
		int maxcrushe = ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE);
		if(crushe < 1) crushe = 1;
		if(maxcrushe < 1){
			sprintf(buff1, "不会损坏");
		}else{
			maxcrushe = maxcrushe/1000;
			crushe = crushe/1000;
			if( maxcrushe <= 0 ) maxcrushe = 1;
			snprintf(buff1, sizeof(buff1), "%d%%", (int)((crushe*100)/maxcrushe) );
		}
		snprintf(ITEM_itemStatusStringBuffer, sizeof( ITEM_itemStatusStringBuffer),
				 
#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
				"%d|%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d|%s",
#else
				"%d|%s|%s|%d|%s|%d|%d|%d|%d|%d|%s|%d",
#endif
#else
				"%d|%s|%s|%d|%s|%d|%d|%d|%d|%d|%s",
#endif
				 haveitemindex,
				 escapename, paramshow,
				 itemcolor, escapeeffectstring ,
				 ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
				 ITEM_getInt( itemindex, ITEM_ABLEUSEFIELD),
				 ITEM_getInt( itemindex, ITEM_TARGET),
				 ITEM_getInt( itemindex, ITEM_LEVEL),
				 flg,
				 buff1
#ifdef _ITEM_PILENUMS
				,ITEM_getInt( itemindex, ITEM_USEPILENUMS)
#ifdef _ALCHEMIST
				,INGNAME0 //,ITEM_getInt( itemindex, ITEM_ALCHEMIST)
#endif
#endif
				  );
#else
		snprintf(ITEM_itemStatusStringBuffer,
				 sizeof( ITEM_itemStatusStringBuffer),
				 "%d|%s|%s|%d|%s|%d|%d|%d|%d|%d",
				 haveitemindex,
				 escapename, paramshow,
				 itemcolor, escapeeffectstring ,
				 ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
				 ITEM_getInt( itemindex, ITEM_ABLEUSEFIELD),
				 ITEM_getInt( itemindex, ITEM_TARGET),
				 ITEM_getInt( itemindex, ITEM_LEVEL),
				 flg
				  );
#endif
	}

	return ITEM_itemStatusStringBuffer;
}

char*   ITEM_makeItemFalseString( void )
{

#ifdef _ADD_SHOW_ITEMDAMAGE  // WON ADD 显示物品耐久度
	strcpysafe( ITEM_itemStatusStringBuffer,
				sizeof( ITEM_itemStatusStringBuffer),
#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
				"|||||||||||"
#else
				"||||||||||"
#endif
#else
				"|||||||||"
#endif
			);

#else
	strcpysafe( ITEM_itemStatusStringBuffer,
				sizeof( ITEM_itemStatusStringBuffer),
				"||||||||" );
#endif
	return ITEM_itemStatusStringBuffer;
}

char*   ITEM_makeItemFalseStringWithNum( int haveitemindex )
{
#ifdef _ADD_SHOW_ITEMDAMAGE  // WON ADD 显示物品耐久度
	snprintf(  ITEM_itemStatusStringBuffer, sizeof( ITEM_itemStatusStringBuffer),

#ifdef _ITEM_PILENUMS
#ifdef _ALCHEMIST
				"%d||||||||||||",
#else
				"%d|||||||||||",
#endif
#else
				"%d||||||||||",
#endif
				haveitemindex );
#else
	snprintf(  ITEM_itemStatusStringBuffer, sizeof( ITEM_itemStatusStringBuffer),
				"%d|||||||||", haveitemindex);
#endif
	return ITEM_itemStatusStringBuffer;
}

BOOL ITEM_makeItem( ITEM_Item* itm, int number )
{
	int i;
#ifdef _IMPOROVE_ITEMTABLE
	int Itemsi;
#endif
	if( ITEM_CHECKITEMTABLE(number) == FALSE ){
		print(" Can't makeItem for itemid:%d!!\n", number);
		return FALSE;
	}
#ifdef _IMPOROVE_ITEMTABLE
	Itemsi = ITEM_getSIndexFromTransList( number);
	memcpy( itm, &ITEM_tbl[Itemsi].itm , sizeof( ITEM_Item ) ); //new
	for( i=0 ; i<ITEM_DATAINTNUM ; i++ ){
		int     randomvalue;
		randomvalue = RAND( 0, ITEM_tbl[Itemsi].randomdata[i] ); //new
		itm->data[i] = ITEM_tbl[Itemsi].itm.data[i] + randomvalue; //new
	}
#else
	memcpy( itm, &ITEM_tbl[number].itm , sizeof( ITEM_Item ) ); //new
	for( i=0 ; i<ITEM_DATAINTNUM ; i++ ){
		int     randomvalue;
		randomvalue = RAND(0,ITEM_tbl[number].randomdata[i]); //new
		itm->data[i] = ITEM_tbl[number].itm.data[i] + randomvalue; //new
	}
#endif
	itm->data[ITEM_LEAKLEVEL] = 1;
	return TRUE;
}

int ITEM_makeItemAndRegist( int number )
{
	ITEM_Item   itm;
	memset( &itm, 0, sizeof( itm));
	if( ITEM_makeItem( &itm, number ) == FALSE )
		return -1;
	return ITEM_initExistItemsOne( &itm );
}

#define EQUIP_FIX_MAX	10000000
void ITEM_equipEffect( int index )
{
	int     i;
	int		attribaccum[4] = { 0,0,0,0};
#ifdef _ANGEL_SUMMON
	int		angelmode, angelequip =0;
#endif
	static struct itmeffectstruct{
		int itemdataintindex;
		int charmodifyparamindex;
		int accumulation;
		int min;
		int max;
		int type;
	}itemEffect[]={
		{ ITEM_MODIFYATTACK,    CHAR_WORKATTACKPOWER,	0,	0,	EQUIP_FIX_MAX,	0},
		{ ITEM_MODIFYDEFENCE,	CHAR_WORKDEFENCEPOWER,	-100,	0,	EQUIP_FIX_MAX,	0},
		{ ITEM_MODIFYQUICK,		CHAR_WORKQUICK,			-100,	0,	EQUIP_FIX_MAX,	0},
		{ ITEM_MODIFYHP,		CHAR_WORKMAXHP,			0,	0,	EQUIP_FIX_MAX,	0},
		{ ITEM_MODIFYMP,        CHAR_WORKMAXMP,			0,	0,	1000,	0}, // MP 反100引匹

		{ ITEM_MODIFYLUCK,       CHAR_WORKFIXLUCK,	0,	1,	5,	0},
		{ ITEM_MODIFYCHARM,      CHAR_WORKFIXCHARM,	0,	0,	100,	0},
		{ ITEM_MODIFYAVOID,      CHAR_WORKFIXAVOID,	0,	0,	EQUIP_FIX_MAX,	0},
		{ ITEM_POISON,      	 CHAR_WORKMODPOISON,	0,	-100, 100,	0},
		{ ITEM_PARALYSIS,      	 CHAR_WORKMODPARALYSIS,	0,	-100, 100,	0},
		{ ITEM_SLEEP,      	 	 CHAR_WORKMODSLEEP,	0, -100,	100,	0},
		{ ITEM_STONE,      	 	 CHAR_WORKMODSTONE,	0, -100,	100,	0},
		{ ITEM_DRUNK,      	 	 CHAR_WORKMODDRUNK,	0, -100,	100,	0},
		{ ITEM_CONFUSION,      	 CHAR_WORKMODCONFUSION,	0,	-100,	100,	0},
		{ ITEM_CRITICAL,      	 CHAR_WORKMODCRITICAL,	0,	-100,	100,	0},
#ifdef _ADD_DEAMGEDEFC
		{ ITEM_OTHERDAMAGE,		CHAR_WORKOTHERDMAGE,	0,	-100,	100,	0},
		{ ITEM_OTHERDEFC,		CHAR_WORKOTHERDEFC,		0,	-100,	100,	0},
#endif

#ifdef _ITEMSET5_TXT
		{ ITEM_MODIFYARRANGE,	CHAR_WORKFIXARRANGE,	0,	0,	1000,	0},
		{ ITEM_MODIFYSEQUENCE,	CHAR_WORKFIXSEQUENCE,	0,	0,	100,	1},
		{ ITEM_ATTACHPILE,		CHAR_WORKATTACHPILE,	0,	0,	10,	1},
		{ ITEM_HITRIGHT,		CHAR_WORKHITRIGHT,		0,	0,	40,	1},
#endif
#ifdef _ITEMSET6_TXT
		{ ITEM_NEGLECTGUARD,	CHAR_WORKNEGLECTGUARD,	0,	0,	30,	1},
#endif
	};

	for( i = 0 ; i < arraysizeof( itemEffect ); i ++ )
		itemEffect[i].accumulation = 0;

	if( !CHAR_CHECKINDEX(index) )return;

#ifdef _ANGEL_SUMMON
	angelmode = CHAR_getWorkInt( index, CHAR_WORKANGELMODE);
#endif

	for( i = 0 ; i < CHAR_EQUIPPLACENUM ; i ++ ){
		int     id = CHAR_getItemIndex(index,i);
		if( ITEM_CHECKINDEX(id) ){
			int     j;
			int		attrib = 0;

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
			if( i == CHAR_ARM )
					CHAR_sendStatusString( index , "S");
#endif

			for( j=0; j < arraysizeof( itemEffect ); j ++ ) {
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能

				if( i == CHAR_EQSHIELD ){
					int item_type = ITEM_FIST;
					item_type = ITEM_getInt( id, ITEM_TYPE );

					// 当左手装备武器的时候									
					if( item_type != ITEM_WSHIELD ){
						int k;
						// 勇士职业技能二刀流
						for( k=0; k<CHAR_SKILLMAXHAVE; k++ ){
							char *skill_name;
							// 技能ID
							int skillid = CHAR_getCharSkill( index, k);
							int Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
							if( Pskillid <= 0 ) continue;

							// 技能名称
							skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
							if( skill_name == NULL ) continue;
	
							// 二刀流
							if( (strcmp( skill_name , "PROFESSION_DUAL_WEAPON" )) == 0 ){
								int skill=k, skill_level=0, rate=0, value=0;
								CHAR_HaveSkill* hskill;
								// 人物技能等级
								hskill = CHAR_getCharHaveSkill( index, skill );
								skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);
								skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );
								rate = (skill_level * 3 + 20);
								value = ITEM_getInt( id, itemEffect[j].itemdataintindex ) * rate / 100;
								itemEffect[j].accumulation += value;
							}
						}
					}else
						itemEffect[j].accumulation += ITEM_getInt( id, itemEffect[j].itemdataintindex );
				}else 
#endif
				itemEffect[j].accumulation += ITEM_getInt( id, itemEffect[j].itemdataintindex );			
			}

			attrib = ITEM_getInt( id, ITEM_MODIFYATTRIB);
			if(  attrib > 0 && attrib < 5) {
				attribaccum[attrib - 1] += ITEM_getInt( id, ITEM_MODIFYATTRIBVALUE);
			}

#ifdef _ANGEL_SUMMON
			//if( !strcmp( ITEM_getChar( id, ITEM_USEFUNC), "ITEM_AngelToken") ) {
			if( ITEM_getInt( id, ITEM_ID) == ANGELITEM ) {
				//print(" 使者信物装备中 ");
				angelequip = TRUE;
			}
#endif


		}
	}

#ifdef _ANGEL_SUMMON
	if( angelmode == TRUE && angelequip == FALSE ) {
		CHAR_setWorkInt( index, CHAR_WORKANGELMODE, FALSE);
		CHAR_sendAngelMark( CHAR_getWorkInt( index, CHAR_WORKOBJINDEX), 0);
		print(" 卸下使者信物 ");
	}
	if( angelmode == FALSE && angelequip == TRUE ) {
		CHAR_setWorkInt( index, CHAR_WORKANGELMODE, TRUE);
		CHAR_sendAngelMark( CHAR_getWorkInt( index, CHAR_WORKOBJINDEX), 1);
		CHAR_talkToCli( index, -1, "你受到了精灵保护，不会被敌人攻击。", CHAR_COLORYELLOW );
		print(" 装备使者信物 ");
	}
#endif

#ifdef _FIX_MAXCHARMP
	for(i = 3 ; i < arraysizeof( itemEffect ) ; i ++ ){
#else
	for(i = 5 ; i < arraysizeof( itemEffect ) ; i ++ ){
#endif
		int work=0;
		if( itemEffect[i].type == 1 ){
			work = itemEffect[i].accumulation;
		}else{
			work = CHAR_getWorkInt(index,itemEffect[i].charmodifyparamindex) + itemEffect[i].accumulation;
			work = min( itemEffect[i].max, work );
			work = max( itemEffect[i].min, work );
		}
		CHAR_setWorkInt( index, itemEffect[i].charmodifyparamindex, work );
	}


	for( i = 0; i < 4; i ++ ) {
		CHAR_setWorkInt( index, CHAR_WORKFIXEARTHAT + i,
						CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT + i) +
						attribaccum[i]);
	}
	for( i = 0; i < 4; i ++ ) {
		int		j;
		for( j = 0; j < 4; j ++ ) {
			if( i != j ) {
				CHAR_setWorkInt( index, CHAR_WORKFIXEARTHAT + j,
						CHAR_getWorkInt(index, CHAR_WORKFIXEARTHAT + j) - attribaccum[i]);
			}
		}
	}
	for( i = 0; i < 4; i ++ ) {
		if( CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT + i) > CHAR_MAXATTRIB) {
			CHAR_setWorkInt( index, CHAR_WORKFIXEARTHAT + i, CHAR_MAXATTRIB);
		}
	}

	{
		int work;
		work = CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+ itemEffect[0].accumulation;
		CHAR_setWorkInt( index, CHAR_WORKFIXSTR, max( 0, work ) );
		work = CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)	+ itemEffect[1].accumulation;
		CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH, max( -100, work ) );
		work = CHAR_getWorkInt( index, CHAR_WORKFIXDEX)	+ itemEffect[2].accumulation;
		CHAR_setWorkInt( index, CHAR_WORKFIXDEX, max( -100, work ) );
	}

}

void Other_DefcharWorkInt( int index)
{

#ifdef _SUIT_ITEM
	int mfix,mtgh,mdex,mods,maxhp;
	mfix = CHAR_getWorkInt( index, CHAR_WORKFIXSTR);
	mtgh = CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH);
	mdex = CHAR_getWorkInt( index, CHAR_WORKFIXDEX);
	maxhp = CHAR_getWorkInt( index, CHAR_WORKMAXHP);

	mods = CHAR_getWorkInt( index, CHAR_WORKSUITMODSTR);
	mfix = mfix+((mfix*mods)/100);
	CHAR_setWorkInt( index, CHAR_WORKFIXSTR, mfix+CHAR_getWorkInt( index, CHAR_WORKSUITSTR));
	CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH, mtgh+CHAR_getWorkInt( index, CHAR_WORKSUITTGH));
	CHAR_setWorkInt( index, CHAR_WORKFIXDEX, mdex+CHAR_getWorkInt( index, CHAR_WORKSUITDEX));
	CHAR_setWorkInt( index, CHAR_WORKMAXHP, maxhp+ CHAR_getWorkInt( index, CHAR_WORKSUITVIT));
#ifdef _SUIT_ADDPART3
	if( mfix > 0 )
		CHAR_setWorkInt( index, CHAR_WORKFIXSTR, CHAR_getWorkInt( index, CHAR_WORKFIXSTR)+mfix*CHAR_getWorkInt( index, CHAR_WORKSUITSTR_P)/100.0);
	if( mtgh > 0 )
		CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH, CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH)+mtgh*CHAR_getWorkInt( index, CHAR_WORKSUITTGH_P)/100.0);
	if( mdex > 0)
		CHAR_setWorkInt( index, CHAR_WORKFIXDEX, CHAR_getWorkInt( index, CHAR_WORKFIXDEX)+mdex*CHAR_getWorkInt( index, CHAR_WORKSUITDEX_P)/100.0);
#endif
#endif//_SUIT_ITEM

#ifdef _MAGIC_RESIST_EQUIT			// WON ADD 职业抗性装备
	/*{
		int f_res = -1, i_res = -1, t_res = -1;
		f_res = CHAR_getWorkInt( index, CHAR_WORK_F_RESIST );
		i_res = CHAR_getWorkInt( index, CHAR_WORK_I_RESIST );		
		t_res = CHAR_getWorkInt( index, CHAR_WORK_T_RESIST );

		CHAR_setWorkInt( index, CHAR_WORK_F_RESIST, f_res+CHAR_getWorkInt( index, CHAR_WORK_F_SUIT ) );
		CHAR_setWorkInt( index, CHAR_WORK_I_RESIST, i_res+CHAR_getWorkInt( index, CHAR_WORK_I_SUIT ) );
		CHAR_setWorkInt( index, CHAR_WORK_T_RESIST, t_res+CHAR_getWorkInt( index, CHAR_WORK_T_SUIT ) );
	}*/
#endif


#ifdef _PETSKILL_SETDUCK
	//profession fix
	//使用回避招式时,会将防值减去30%然後设定成回避值,奇怪的设定,也造成原本设定的回避值跟没设一样,因此我(Change)先把整段拿掉
	/*if( CHAR_getWorkInt( index, CHAR_MYSKILLDUCK) > 0 ){
		int mtgh = CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH);
		mtgh -= (mtgh*30)/100;
		if( mtgh < 0 ) mtgh=0;
		CHAR_setWorkInt( index, CHAR_MYSKILLDUCKPOWER, mtgh);
	}*/
#endif
#ifdef _MAGICPET_SKILL
	if( CHAR_getWorkInt( index, CHAR_MYSKILLSTR) > 0 ){
		int mpower, mdef;
		mpower = CHAR_getWorkInt( index, CHAR_WORKFIXSTR);
		mdef = CHAR_getWorkInt( index, CHAR_MYSKILLSTRPOWER);
		mpower += (mtgh*mdef)/100;
		if( mtgh < 0 ) mtgh=0;
		CHAR_setWorkInt( index, CHAR_WORKFIXSTR, mpower);
	}
	if( CHAR_getWorkInt( index, CHAR_MYSKILLTGH) > 0 ){
		int mpower, mdef;
		mpower = CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH);
		mdef = CHAR_getWorkInt( index, CHAR_MYSKILLTGHPOWER);
		mpower += (mtgh*mdef)/100;
		if( mtgh < 0 ) mtgh=0;
		CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH, mpower);
	}
	if( CHAR_getWorkInt( index, CHAR_MYSKILLDEX) > 0 ){
		int mpower, mdef;
		mpower = CHAR_getWorkInt( index, CHAR_WORKFIXDEX);
		mdef = CHAR_getWorkInt( index, CHAR_MYSKILLDEXPOWER);
		mpower += (mtgh*mdef)/100;
		if( mtgh < 0 ) mtgh=0;
		CHAR_setWorkInt( index, CHAR_WORKFIXDEX, mpower);
	}
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( CHAR_getWorkInt( index, CHAR_MYSKILLHIT) > 0 ){
		int mpower, mdef;
		mpower = CHAR_getWorkInt( index, CHAR_MYSKILLHIT);
		mdef = CHAR_getWorkInt( index, CHAR_WORKHITRIGHT);
		mpower += (mtgh*mdef)/100;
		if( mtgh < 0 ) mtgh=0;
		CHAR_setWorkInt( index, CHAR_MYSKILLHIT, mpower);
	}
	if( CHAR_getWorkInt( index, CHAR_WORK_WEAPON) > 0 ){	// 武器专精
		int mpower, mdef;
		mpower = CHAR_getWorkInt( index, CHAR_WORKFIXSTR);
		mdef = CHAR_getWorkInt( index, CHAR_WORKMOD_WEAPON);
		mpower = mpower * ( 100 + mdef ) /100;
		if( mtgh < 0 ) mtgh=0;
		CHAR_setWorkInt( index, CHAR_WORKFIXSTR, mpower);
	}
#endif

#ifdef _VARY_WOLF		
	if(CHAR_getInt( index, CHAR_BASEIMAGENUMBER)==101428){
		int Rands=0, power;
		Rands = CHAR_getWorkInt( index, CHAR_SKILLSTRPOWER);
		power = CHAR_getWorkInt( index, CHAR_WORKFIXSTR);
		power = power + (power*Rands)/100;
		CHAR_setWorkInt( index, CHAR_WORKFIXSTR, power);

		Rands = CHAR_getWorkInt( index, CHAR_SKILLDEXPOWER);
		power = CHAR_getWorkInt( index, CHAR_WORKFIXDEX);
		power = power + (power*Rands)/100;
		CHAR_setWorkInt( index, CHAR_WORKFIXDEX, power);
	}
#endif

#ifdef _PROFESSION_ADDSKILL
	if( CHAR_getWorkInt( index, CHAR_WORKFEAR ) > 0 ){
		CHAR_setWorkInt( index, CHAR_WORKFIXSTR, CHAR_getWorkInt( index, CHAR_WORKFIXSTR ) - mfix*0.1 );
		CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH, CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH ) - mtgh*0.1 );
		CHAR_setWorkInt( index, CHAR_WORKFIXDEX, CHAR_getWorkInt( index, CHAR_WORKFIXDEX ) - mdex*0.2 );
	}
#endif

#ifdef _MAGIC_WEAKEN
	if(CHAR_getWorkInt(index,CHAR_WORKWEAKEN)>0){
			CHAR_setWorkInt( index, CHAR_WORKFIXSTR, CHAR_getWorkInt( index, CHAR_WORKFIXSTR)*0.8);
			CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH, CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH)*0.8);
			CHAR_setWorkInt( index, CHAR_WORKFIXDEX, CHAR_getWorkInt( index, CHAR_WORKFIXDEX)*0.8);
            CHAR_setWorkInt( index, CHAR_WORKWEAKEN, CHAR_getWorkInt( index, CHAR_WORKWEAKEN)-1);
	}
#endif
#ifdef _MAGIC_BARRIER// vincent  精灵:魔障
	if( CHAR_getWorkInt( index, CHAR_WORKBARRIER) > 0){
            CHAR_setWorkInt( index, CHAR_WORKBARRIER, CHAR_getWorkInt( index, CHAR_WORKBARRIER) - 1);
	}
#endif

	CHAR_setWorkInt(index, CHAR_WORKATTACKPOWER,	CHAR_getWorkInt( index, CHAR_WORKFIXSTR ) );
	CHAR_setWorkInt(index, CHAR_WORKDEFENCEPOWER,	CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH ) );
	CHAR_setWorkInt(index, CHAR_WORKQUICK,			CHAR_getWorkInt( index, CHAR_WORKFIXDEX ) );
}

char* ITEM_getAppropriateName(int itemindex)
{
	int nameindex;
	if( !ITEM_CHECKINDEX(itemindex ) )return NULL;
	if( ITEM_getInt(itemindex,ITEM_LEAKLEVEL) <= 0 )
		nameindex = ITEM_NAME;
	else
		nameindex = ITEM_SECRETNAME;
	return ITEM_getChar(itemindex,nameindex);

}

char* ITEM_getEffectString( int itemindex )
{
	if( !ITEM_CHECKINDEX(itemindex))return NULL;
	return ITEM_item[itemindex].itm.string[ITEM_EFFECTSTRING].string;
}

char *ITEM_getItemInfoFromNumber( int itemid )
{
	return ITEMTBL_getChar( itemid, ITEM_EFFECTSTRING);
}

INLINE char* _ITEM_getNameFromNumber( char *file, int line, int itemid )
{
	if( ITEM_CHECKITEMTABLE(itemid) == FALSE ){
		print("Can't Get ItemTable for:%d! - %s:%d\n",itemid, file, line);
		return NULL;
	}
	return ITEMTBL_getChar( itemid, ITEM_NAME);
}

int ITEM_getcostFromITEMtabl( int itemid )
{
	return ITEMTBL_getInt( itemid, ITEM_COST);
}

int ITEM_getlevelFromITEMtabl( int itemid )
{
	return ITEMTBL_getInt( itemid, ITEM_LEVEL);
}

int ITEM_getgraNoFromITEMtabl( int itemid )
{
	return ITEMTBL_getInt( itemid, ITEM_BASEIMAGENUMBER);
}

int ITEM_getcanpetmailFromITEMtabl( int itemid )
{
	return ITEMTBL_getInt( itemid, ITEM_CANPETMAIL);
}

int ITEM_getvanishatdropFromITEMtabl( int itemid )
{
	return ITEMTBL_getInt( itemid, ITEM_VANISHATDROP);
}

int ITEM_getdropatlogoutFromITEMtabl( int itemid )
{
	return ITEMTBL_getInt( itemid, ITEM_DROPATLOGOUT);
}

int ITEM_getmergeItemFromFromITEMtabl( int itemid )
{
	return ITEMTBL_getInt( itemid, ITEM_CANMERGEFROM);
}

BOOL ITEM_canuseMagic( int itemindex)
{
	if( !ITEM_CHECKINDEX(itemindex))return FALSE;
	if( MAGIC_getMagicArray( ITEM_item[itemindex].itm.data[ITEM_MAGICID]) != -1 ) {
		return TRUE;
	}
	return FALSE;
}

INLINE ITEM_Item *ITEM_getItemPointer( int index )
{
	if(!ITEM_CHECKINDEX(index))return NULL;
	return &ITEM_item[index].itm;
}

int ITEM_isTargetValid( int charaindex, int itemindex, int toindex)
{
	int itemtarget;
	int Myside;
	itemtarget = ITEM_getInt( itemindex, ITEM_TARGET );

	Myside = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLESIDE );
	if ((toindex >= 0x0) && (toindex <= 0x13)) return 0;

	if (toindex == 0x14) {//20 右下全 0
		if (itemtarget == ITEM_TARGET_ALLMYSIDE){
			if( Myside == 0 )return 0;
		}else if(itemtarget == ITEM_TARGET_ALLOTHERSIDE ){
			if( Myside == 1 )return 0;
		}
	}else if (toindex == 0x15) {//21 左上全 1
		if (itemtarget == ITEM_TARGET_ALLMYSIDE){
			if( Myside == 1 )return 0;
		}else if( itemtarget == ITEM_TARGET_ALLOTHERSIDE ){
			if( Myside == 0 )return 0;
		}
	}else if (toindex == 0x16) {
		if (itemtarget == ITEM_TARGET_ALL)
			return 0;
	}
	return -1;
}
// Nuke end


#ifdef _ITEM_CHECKWARES
BOOL CHAR_CheckInItemForWares( int charaindex, int flg)
{
	int itemindex, i;
	char token[256];
	
	for( i=0; i<24; i++)	{
		itemindex = CHAR_getItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_WARES ) {
			if( flg == 0 ){
				return FALSE;
			}
			sprintf(token,"交出%s",ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);

			CHAR_setItemIndex( charaindex, i ,-1);
			ITEM_endExistItemsOne( itemindex);
			CHAR_sendItemDataOne( charaindex, i);
		}
	}
	return TRUE;
}
#endif


#ifdef _IMPOROVE_ITEMTABLE

BOOL ITEMTBL_CHECKINDEX( int ItemID)
{
	if( ItemID < 0 || ItemID >= defitemtbls )
		return FALSE;
	return ITEM_TransformList[ ItemID].use;
}

int ITEM_getSIndexFromTransList( int ItemID)
{
	if( !ITEMTBL_CHECKINDEX( ItemID) )
		return -1;
	return ITEM_TransformList[ ItemID].Si;
}

int ITEM_getTotalitemtblsFromTransList()
{
	return totalitemtbls;
}

int ITEM_getMaxitemtblsFromTransList()
{
	return defitemtbls;
}
#endif

int ITEMTBL_getInt( int ItemID, ITEM_DATAINT datatype)
{
#ifdef _IMPOROVE_ITEMTABLE
	int itemsi=0;
	if( !ITEM_CHECKITEMTABLE( ItemID) ) return -1;
	itemsi = ITEM_getSIndexFromTransList( ItemID);
	if( datatype >= ITEM_DATAINTNUM || datatype < 0 ) return -1;
	return ITEM_tbl[itemsi].itm.data[datatype]; //new
#else
	if( datatype >= ITEM_DATAINTNUM || datatype < 0 ) return -1;
	if( ITEM_CHECKITEMTABLE(ItemID) == FALSE )return -1;
		return ITEM_tbl[ItemID].itm.data[datatype]; //new
#endif
}

char *ITEMTBL_getChar( int ItemID, ITEM_DATACHAR datatype)
{
#ifdef _IMPOROVE_ITEMTABLE
	int itemsi=0;
	if( !ITEM_CHECKITEMTABLE( ItemID) ) return NULL;
	itemsi = ITEM_getSIndexFromTransList( ItemID);
	if( datatype >= ITEM_DATACHARNUM || datatype < 0 ) return NULL;
	return ITEM_tbl[itemsi].itm.string[datatype].string; //new
#else
	if( datatype >= ITEM_DATACHARNUM || datatype < 0 ) return NULL;
	if( ITEM_CHECKITEMTABLE(ItemID) == FALSE )return NULL;
	return ITEM_tbl[ItemID].itm.string[datatype].string; //new
#endif
}

INLINE BOOL ITEM_CHECKITEMTABLE( int number )
{
#ifdef _IMPOROVE_ITEMTABLE
	int itemsi=0;
	if( !ITEMTBL_CHECKINDEX( number) ) return FALSE;
	itemsi = ITEM_getSIndexFromTransList( number);
	if( itemsi < 0 || itemsi >= ITEM_tblen  )
		return FALSE;
	return ITEM_tbl[itemsi].use; //new
#else
	if( number < 0 || number >= ITEM_tblen  ){
		//andy_log
		print("ITEM_CHECKITEMTABLE() number:%d ITEM_tblen:%d !!\n", number, ITEM_tblen );
		return FALSE;
	}
	return ITEM_tbl[number].use; //new
#endif
}

int ITEM_getItemDamageCrusheED( int itemindex)
{
	int crushe	= ITEM_getInt( itemindex, ITEM_DAMAGECRUSHE);
	int maxcrushe = ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE);
	if(maxcrushe < 1)return -1;
	if( maxcrushe > 1000000) maxcrushe = 1000000;
	if( crushe > 1000000) crushe = 1000000;

	return ((int) (crushe*100)/maxcrushe);
}

void ITEM_RsetEquit( int charaindex)
{
	int i, itemindex, ti=-1;
	for( i=0; i<CHAR_STARTITEMARRAY; i++){
		itemindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex)) continue;
		if( i == ITEM_getEquipPlace( charaindex, itemindex ) ) continue;
		if( (ti = CHAR_findEmptyItemBox( charaindex )) == -1 ) return;
		CHAR_setItemIndex( charaindex , i, -1);
		CHAR_setItemIndex( charaindex , ti, itemindex);
	}
}

void ITEM_reChangeItemToPile( int itemindex)
{
	int itemID;
	if( !ITEM_CHECKINDEX(itemindex) ) return;
	
	if( ITEM_getInt( itemindex, ITEM_USEPILENUMS) <= 0 )
		ITEM_setInt( itemindex, ITEM_USEPILENUMS, 1);
	itemID = ITEM_getInt( itemindex, ITEM_ID);
	if( !ITEM_CHECKITEMTABLE( itemID) ) return;
	if( itemID == 20284 ) return;//铁枪叁另外处理
	if( ITEM_getInt( itemindex, ITEM_CANBEPILE) != ITEMTBL_getInt( itemID, ITEM_CANBEPILE) )
		ITEM_setInt( itemindex, ITEM_CANBEPILE, ITEMTBL_getInt( itemID, ITEM_CANBEPILE) );
}

void ITEM_reChangeItemName( int itemindex)
{//ITEM_NAME
/*
	int itemID;
	char *IDNAME;
	char *NAME;
	if( !ITEM_CHECKINDEX(itemindex) ) return;
	itemID = ITEM_getInt( itemindex, ITEM_ID);
	if( !ITEM_CHECKITEMTABLE( itemID) ) return;

	IDNAME = ITEMTBL_getChar( itemID, ITEM_NAME);
	NAME = ITEM_getChar( itemindex, ITEM_NAME);
	if( IDNAME==NULL || NAME==NULL ) return;
	if( !strcmp( IDNAME, NAME) ) return;

	ITEM_setChar( itemindex, ITEM_NAME, IDNAME);
*/
}

#ifdef _CHECK_ITEM_MODIFY
void ITEM_checkItemModify( int charaindex, int itemindex)
{
	//int itemindex;
	int item_ID;
	int nowData;
	int baseData;
	int randData;
	int i;

	int checkList[] = {	
			ITEM_MODIFYATTACK,
			ITEM_MODIFYDEFENCE,
			ITEM_MODIFYQUICK,
			ITEM_MODIFYHP,
			ITEM_MODIFYMP,
			ITEM_MODIFYLUCK,
			ITEM_MODIFYCHARM,
			ITEM_MODIFYAVOID
	};

	//itemindex = CHAR_getItemIndex( charaindex, equipindex);
	item_ID = ITEM_getInt( itemindex, ITEM_ID);

	for( i = 0; i < arraysizeof( checkList); i ++ ){

		nowData = ITEM_getInt( itemindex, checkList[i]);
		baseData = ITEMTBL_getInt( item_ID, checkList[i]);
		if( baseData == -1 )	continue;
		randData = ITEM_tbl[item_ID].randomdata[checkList[i]];
		
		if( nowData > baseData + randData ) {
			char szBuffer[1024];
			ITEM_setInt( itemindex, checkList[i], baseData); 
			sprintf( szBuffer, " 修正异常道具: id=%d a=%d now=%d base=%d rand=%d ",
				item_ID, checkList[i], nowData, baseData, randData );
			print( szBuffer);
			if( CHAR_CHECKINDEX(charaindex) ) {
				LogOther( 
					CHAR_getChar( charaindex, CHAR_CDKEY),
					CHAR_getChar( charaindex, CHAR_NAME),
					szBuffer );
			}

		}
	}
}
#endif
