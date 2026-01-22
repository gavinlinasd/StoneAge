#include "version.h"
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "buf.h"
#include "configfile.h"
#include "char_base.h"
#include "char_data.h"
#include "pet_skill.h"
#include "battle.h"
#include "battle_event.h"
#include "battle_magic.h"
#include "item.h"
#include "item_gen.h"
#include "char_talk.h"

/*========================================================================
 * 妐蚗哱摯  勗僑笢趙摯藺↓筒 唾昜腔跪宒撮夔
 *========================================================================*/


static Petskill	*PETSKILL_petskill;
static int		PETSKILL_petskillnum;

typedef struct tagPetskill_PetskillFunctionTable
{
	char			*functionname;		/*	优擭啞栝騷鳴勗怳�抌�倳摯  鞳 */
	PETSKILL_CALLFUNC	func;				/*    傷勗蠙怮③踏躂堎匴倳 */
	int				hash;				/*  hash */
	int				status;				/*  喫摯昡悃璃迶禱妐蚗哱誑炩擭媃倜硒 */
}PETSKILL_PetskillFunctionTable;

/*   禱  盓媃倜�梜薱藈樘�  埰堎痀 */
static PETSKILL_PetskillFunctionTable PETSKILL_functbl[] = {
	{ "PETSKILL_None",				PETSKILL_None,		0 },
	{ "PETSKILL_NormalAttack",		PETSKILL_NormalAttack,	0 },
	{ "PETSKILL_NormalGuard",		PETSKILL_NormalGuard,	0 },
	{ "PETSKILL_ContinuationAttack",PETSKILL_ContinuationAttack,	0 },
	{ "PETSKILL_ChargeAttack",		PETSKILL_ChargeAttack,			0 },
	{ "PETSKILL_Guardian",			PETSKILL_Guardian,		0 },
	{ "PETSKILL_PowerBalance",		PETSKILL_PowerBalance,		0 },
	{ "PETSKILL_Mighty",			PETSKILL_Mighty,		0 },
	{ "PETSKILL_StatusChange",		PETSKILL_StatusChange,	0 },
	{ "PETSKILL_EarthRound",		PETSKILL_EarthRound,	0 },
	{ "PETSKILL_GuardBreak",		PETSKILL_GuardBreak,	0 },
#ifdef _SKILL_GUARDBREAK2//ぢ壺滅郘2 vincent add 2002/05/20
	{ "PETSKILL_GuardBreak2",		PETSKILL_GuardBreak2,	0 },
#endif
	{ "PETSKILL_Abduct",			PETSKILL_Abduct,		0 },
	{ "PETSKILL_Steal",				PETSKILL_Steal,			0 },
	{ "PETSKILL_Merge",				PETSKILL_Merge,			0 },
#ifdef _ALCHEMIST
	{ "PETSKILL_Alchemist",			PETSKILL_Merge,			0 },
#endif
	{ "PETSKILL_NoGuard",			PETSKILL_NoGuard,		0 },
#ifdef _ITEM_INSLAY
	{ "PETSKILL_Inslay",			PETSKILL_Inslay,		0 },
#endif
#ifdef _PETSKILL_FIXITEM
	{ "PETSKILL_Fixitem",			PETSKILL_Fixitem,		0 },
#endif

#ifdef _MAGIC_SUPERWALL
	{ "PETSKILL_MagicStatusChange" , PETSKILL_MagicStatusChange, 0},
#endif

// Terry add 2001/11/05
#ifdef __ATTACK_MAGIC
	{ "PETSKILL_AttackMagic",	PETSKILL_AttackMagic,0},
#endif

#ifdef _PSKILL_FALLGROUND
	{ "PETSKILL_FallGround",	PETSKILL_FallGround, 0},
#endif
#ifdef _PETSKILL_EXPLODE
	{ "PETSKILL_Explode",	PETSKILL_Explode, 0},
#endif
#ifdef _BATTLESTEAL_FIX
	{ "PETSKILL_StealMoney",	PETSKILL_StealMoney,	0 },
#endif
#ifdef _PRO_BATTLEENEMYSKILL
	{ "ENEMYSKILL_ReLife",	ENEMYSKILL_ReLife, 0},
	{ "ENEMYSKILL_ReHP",	ENEMYSKILL_ReHP, 0},
	{ "ENEMYSKILL_EnemyHelp",	ENEMYSKILL_EnemyHelp, 0},
#endif
#ifdef _SKILL_DAMAGETOHP
	{ "PETSKILL_DamageToHp",	PETSKILL_DamageToHp, 0},
#endif

#ifdef _PETSKILL_TIMID
	{ "PETSKILL_BattleTimid",	PETSKILL_BattleTimid, 0},
#endif
#ifdef _PETSKILL_2TIMID
	{ "PETSKILL_2BattleTimid",	PETSKILL_2BattleTimid, 0},
#endif
#ifdef _PETSKILL_ANTINTER
	{ "PETSKILL_AntInter",	PETSKILL_AntInter, 0},
#endif

#ifdef _PETSKILL_PROPERTY
	{ "PETSKILL_BattleProperty",	PETSKILL_BattleProperty, 0},
#endif

#ifdef _PETSKILL_TEAR
	{ "PETSKILL_BattleTearDamage",	PETSKILL_BattleTearDamage, 0},
#endif

#ifdef _BATTLE_LIGHTTAKE
	{ "PETSKILL_Lighttakeed",	PETSKILL_Lighttakeed, 0},
#endif

#ifdef _BATTLE_ATTCRAZED
	{ "PETSKILL_AttackCrazed",	PETSKILL_AttackCrazed, 0},
#endif

#ifdef _SHOOTCHESTNUT	// Syu ADD 唾撮ㄩ隍璦赽
	{ "PETSKILL_AttackShoot",	PETSKILL_AttackShoot, 0},
#endif

#ifdef _Skill_MPDAMAGE
	{ "PETSKILL_MpDamage",	PETSKILL_MpDamage, 0},
#endif

#ifdef _SKILL_TOOTH
	{ "PETSKILL_ToothCrushe",	PETSKILL_ToothCrushe, 0},
#endif

#ifdef _PETSKILL_SETDUCK
	{ "PETSKILL_SetDuck",	PETSKILL_SetDuck, 0},
#endif
#ifdef _MAGICPET_SKILL
	{ "PETSKILL_SetMagicPet",	PETSKILL_SetMagicPet, 0},
#endif
#ifdef _PSKILL_MODIFY
	{ "PETSKILL_Modifyattack",	PETSKILL_Modifyattack, 0},
#endif

#ifdef _PSKILL_MDFYATTACK
	{ "PETSKILL_Mdfyattack",	PETSKILL_Mdfyattack, 0},
#endif
#ifdef _VARY_WOLF
	{ "PETSKILL_Vary", PETSKILL_Vary, 0},
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
    { "PETSKILL_WildViolentAttack",	PETSKILL_WildViolentAttack, 0},//vincent add 2002/05/16
#endif

#ifdef _SKILL_SPEEDY_ATT
    { "PETSKILL_SpeedyAttack",	PETSKILL_SpeedyAttack, 0},//vincent add 2002/05/20
#endif
#ifdef _SKILL_SACRIFICE  
    { "PETSKILL_Sacrifice",	PETSKILL_Sacrifice, 0},//vincent add 2002/05/30
#endif
#ifdef _SKILL_REFRESH
    { "PETSKILL_Refresh",	PETSKILL_Refresh, 0},//vincent add 2002/08/08
#endif
#ifdef _SKILL_WEAKEN  //vincent唾撮:剞��
    { "PETSKILL_Weaken",	PETSKILL_Weaken, 0},//vincent add 2002/07/11
#endif
#ifdef _SKILL_DEEPPOISON  //vincent唾撮:曄馮 
    { "PETSKILL_Deeppoison",	PETSKILL_Deeppoison, 0},//vincent add 2002/07/16
#endif
#ifdef _SKILL_BARRIER  //vincent唾撮:藹梤 
    { "PETSKILL_Barrier",	PETSKILL_Barrier, 0},//vincent add 2002/07/16
#endif
#ifdef _SKILL_NOCAST  //vincent唾撮:麥蘇 
    { "PETSKILL_Nocast",	PETSKILL_Nocast, 0},//vincent add 2002/07/16
#endif
#ifdef _SKILL_ROAR //vincent唾撮:湮綾
    { "PETSKILL_Roar",	PETSKILL_Roar, 0},//vincent add 2002/07/11
#endif	

#ifdef _PET_SKILL_SARS				// WON ADD 職業延長
	{ "PETSKILL_Sars",	PETSKILL_Sars, 0},
#endif

#ifdef _SONIC_ATTACK				// WON ADD 秞疏馴僻
	{ "PETSKILL_Sonic",	PETSKILL_Sonic, 0},
#endif
#ifdef _PETSKILL_REGRET
	{ "PETSKILL_Regret",	PETSKILL_Regret, 0},
#endif

#ifdef _PETSKILL_GYRATE
    { "PETSKILL_Gyrate", PETSKILL_Gyrate, 0},
#endif 
	
#ifdef _PETSKILL_ACUPUNCTURE //渀棧俋々
    { "PETSKILL_Acupuncture", PETSKILL_Acupuncture, 0},
#endif

#ifdef _PETSKILL_RETRACE
	{ "PETSKILL_Retrace", PETSKILL_Retrace, 0},
#endif

#ifdef _PETSKILL_HECTOR
    { "PETSKILL_Hector", PETSKILL_Hector, 0},
#endif

#ifdef _PETSKILL_FIREKILL
    { "PETSKILL_Firekill", PETSKILL_Firekill, 0},
#endif

#ifdef _PETSKILL_DAMAGETOHP
	{ "PETSKILL_DamageToHp2",	PETSKILL_DamageToHp2, 0},
#endif

#ifdef _PETSKILL_BECOMEFOX
	{ "PETSKILL_BecomeFox",	PETSKILL_BecomeFox, 0},
#endif

#ifdef _PETSKILL_BECOMEPIG
    { "PETSKILL_BecomePig",	PETSKILL_BecomePig, 0},
#endif

#ifdef _PETSKILL_SHOWMERCY
    { "PETSKILL_ShowMercy",	PETSKILL_ShowMercy, 0},
#endif

#ifdef _PETSKILL_COMBINED
    { "PETSKILL_Combined",	PETSKILL_Combined, 0},
#endif
#ifdef _PETSKILL_LER
		{ "PETSKILL_BatFly",	PETSKILL_BatFly, 0},
		{ "PETSKILL_DivideAttack",	PETSKILL_DivideAttack, 0},
#endif
#ifdef _PETSKILL_BATTLE_MODEL
		{ "PETSKILL_BattleModel",	PETSKILL_BattleModel, 0},
#endif

};

/*----------------------------------------------------------------------*/


/* 盻  鏍攝蚗袲�楟抄舠噩抵�祛 */
/*----------------------------------------------------------------------*/
INLINE BOOL PETSKILL_CHECKINDEX( int index )
{
    if( PETSKILL_petskillnum<=index || index<0 )return FALSE;
    return TRUE;
}
/*----------------------------------------------------------------------*/
static INLINE BOOL PETSKILL_CHECKINTDATAINDEX( int index)
{
	if( PETSKILL_DATAINTNUM <= index || index < 0 ) return FALSE;
	return TRUE;
}
/*----------------------------------------------------------------------*/
static INLINE BOOL PETSKILL_CHECKCHARDATAINDEX( int index)
{
	if( PETSKILL_DATACHARNUM <= index || index < 0 ) return FALSE;
	return TRUE;
}
/*----------------------------------------------------------------------*/
INLINE int PETSKILL_getInt( int index, PETSKILL_DATAINT element)
{
	return PETSKILL_petskill[index].data[element];
}
/*----------------------------------------------------------------------*/
INLINE int PETSKILL_setInt( int index, PETSKILL_DATAINT element, int data)
{
	int buf;
	buf = PETSKILL_petskill[index].data[element];
	PETSKILL_petskill[index].data[element]=data;
	return buf;
}
/*----------------------------------------------------------------------*/
INLINE char* PETSKILL_getChar( int index, PETSKILL_DATACHAR element)
{
	if( !PETSKILL_CHECKINDEX( index)) return NULL;
	if( !PETSKILL_CHECKCHARDATAINDEX( element)) return NULL;
	return PETSKILL_petskill[index].string[element].string;
}

/*----------------------------------------------------------------------*/
INLINE BOOL PETSKILL_setChar( int index ,PETSKILL_DATACHAR element, char* new )
{
    if(!PETSKILL_CHECKINDEX(index))return FALSE;
    if(!PETSKILL_CHECKCHARDATAINDEX(element))return FALSE;
    strcpysafe( PETSKILL_petskill[index].string[element].string,
                sizeof(PETSKILL_petskill[index].string[element].string),
                new );
    return TRUE;
}
/*----------------------------------------------------------------------
 * 妐蚗哱  摯倳禱蠐堎��
 *---------------------------------------------------------------------*/
int PETSKILL_getPetskillNum( void)
{
	return PETSKILL_petskillnum;
}

#ifdef _CFREE_petskill
#define PETSKILL_STARTINTNUM	7 //PETSKILL_DATACHARNUM+1
#else 
#define	PETSKILL_STARTINTNUM		5
#endif

/*----------------------------------------------------------------------
 * 妐蚗哱  摯优擭啞栝騷鳴禱  資//場宎唾撮
 *---------------------------------------------------------------------*/
BOOL PETSKILL_initPetskill( char *filename)
{
    FILE*   f;
    char    line[256];
    int     linenum=0;
    int     petskill_readlen=0;
	int		i,j;
	int		max_skillid =0;

    f = fopen(filename,"r");
    if( f == NULL ){
        print( "file open error\n");
        return FALSE;
    }

    PETSKILL_petskillnum=0;

    /*  竘囀  嗚埵菜誑笰菜堣堎凝汔竣凝ぅ迋堎    *///黍⑻唾撮軞杅
    while( fgets( line, sizeof( line ), f ) ){
		char    token[256];
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );
        PETSKILL_petskillnum++;
#ifdef _PETSKILL_OPTIMUM // Robin �○囆豱馧頛嘹D
		if( getStringFromIndexWithDelim( line, ",", PETSKILL_STARTINTNUM, token, sizeof(token)) == FALSE )
			continue;
		//print("\n 唾撮ID:%d %s ", atoi( token), line);
		max_skillid = max( atoi( token), max_skillid);
#endif
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "扆梑囮啖\n" );
        fclose(f);
        return FALSE;
    }

#ifdef _PETSKILL_OPTIMUM // 唾撮Table杅峈 郔湮撮夔ID +1
	PETSKILL_petskillnum = max_skillid +1;
//	print("\n 郔湮唾撮ID = %d\n", max_skillid);
#endif

    PETSKILL_petskill = allocateMemory( sizeof(struct tagPetskill)
                                   * PETSKILL_petskillnum );
    if( PETSKILL_petskill == NULL ){
        fprint( "拸楊煦饜囀湔 %d\n" ,
                sizeof(struct tagPetskill)*PETSKILL_petskillnum);
        fclose( f );
        return FALSE;
    }

	/* 參數結構 */
    for( i = 0; i < PETSKILL_petskillnum; i ++ ) {
    	for( j = 0; j < PETSKILL_DATAINTNUM; j ++ ) {
    		PETSKILL_setInt( i,j,-1);
    	}
    	for( j = 0; j < PETSKILL_DATACHARNUM; j ++ ) {
    		PETSKILL_setChar( i,j,"");
    	}
    }

    /*  竘倜  陑  埰    */
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */                        
                
        chomp( line );

        /* 實際調用的函數 */
        /* 將tab轉換為空格 */
        replaceString( line, '\t' , ' ' );
        /* 袸  摯筒妐↓筒禱噁堎��*/
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

#ifdef _PETSKILL_OPTIMUM // 黍�§憶迣頛撐鯧D, 眻諉眕唾撮ID絞Table index
		ret = getStringFromIndexWithDelim( line, ",", PETSKILL_STARTINTNUM, token, sizeof(token));
		if( ret==FALSE ){
			fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
			break;
		}
		petskill_readlen = atoi( token);
#endif

		for( i = 0; i < PETSKILL_DATACHARNUM; i ++ ) {
	        /*    棬  暵哱↓袲璃禱峟堎    */
	        ret = getStringFromIndexWithDelim( line,",", i + 1, token,sizeof(token));
	        if( ret==FALSE ){
	            fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
	            break;
	        }
	        PETSKILL_setChar( petskill_readlen, i, token);
		}

        /* 4僑  雄禢毀倳偯溢↓淏 */
        for( i = PETSKILL_STARTINTNUM; i < PETSKILL_DATAINTNUM+PETSKILL_STARTINTNUM; i ++ ) {
            ret = getStringFromIndexWithDelim( line,",",i,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("恅璃逄楊渣昫:%s 菴%d俴\n",filename,linenum);
                break;
            }
            if( strlen( token) != 0 ) {
	            PETSKILL_setInt( petskill_readlen, i - PETSKILL_STARTINTNUM, atoi( token));
			}
        }
        if( i < PETSKILL_DATAINTNUM+PETSKILL_STARTINTNUM ) continue;
        
#ifdef _CFREE_petskill
		if(line[0]=='E')	PETSKILL_setInt( petskill_readlen, PETSKILL_ILLEGAL, 1);
#else
		// Robin 2001/02/26 +1 set_illegal_Skill
		if(line[0]=='T')	PETSKILL_setInt( petskill_readlen, PETSKILL_ILLEGAL, 1);
#endif
        //print("Id:%d illegal:%d\n",PETSKILL_getInt(petskill_readlen,PETSKILL_ID),PETSKILL_getInt(petskill_readlen,PETSKILL_ILLEGAL));
		
		petskill_readlen ++;
}
		
    }
    fclose(f);

    PETSKILL_petskillnum = petskill_readlen;


    print( "衄虴唾昜撮夔杅岆 %d...", PETSKILL_petskillnum );

	/* hash 摯頞   */
	for( i = 0; i < arraysizeof( PETSKILL_functbl); i ++ ) {
		PETSKILL_functbl[i].hash = hashpjw( PETSKILL_functbl[i].functionname);
	}
/*
#ifdef _CFREE_petskill
    for( i=0; i <PETSKILL_petskillnum ; i++ ){
		for( j = PETSKILL_NAME; j < PETSKILL_DATACHARNUM; j ++ ){
	      print( "%s ", PETSKILL_getChar( i, j));
		}
		print( "\n");
		for( j = PETSKILL_ID; j < PETSKILL_DATAINTNUM; j ++ ){
			print( "%d ", PETSKILL_getInt( i, j));
		}
		print( "\n-------------------------------------------------\n");

	}
#endif
*/

    return TRUE;
}
/*------------------------------------------------------------------------
 * Petskill摯优擭啞栝騷鳴  陑  媃
 *-----------------------------------------------------------------------*/
BOOL PETSKILL_reinitPetskill( void )
{
	freeMemory( PETSKILL_petskill);
	return( PETSKILL_initPetskill( getPetskillfile()));
}
/*------------------------------------------------------------------------
 * PETSKILL_ID凝�桵撼紐拜籥鎔�倳
 * 蒍堇偯
 * 埬  : 蝨棬
 * 謄  : -1
 *-----------------------------------------------------------------------*/
int PETSKILL_getPetskillArray( int petskillid)
{
#ifdef _PETSKILL_OPTIMUM // 脤戙唾撮ID蜊峈淝蹈宒
	if( petskillid >= 0 && petskillid < PETSKILL_petskillnum )
		return PETSKILL_petskill[petskillid].data[PETSKILL_ID];
#else
	int		i;
	for( i = 0; i < PETSKILL_petskillnum; i ++ ) {
		if( PETSKILL_petskill[i].data[PETSKILL_ID] == petskillid ) {
			return i;
		}
	}
#endif
	return -1;
}
/*------------------------------------------------------------
 * 妐蚗哱  摯匴倳  凝�梣昉抯�淏↓禱蒍埰
 * 礎倳
 *  name        char*       ��絊摯  鞳
 * 蒍堇偯
 *  匴倳喜摯睽騷璃淏�訹聿剒踽�勗毀NULL
 ------------------------------------------------------------*/
PETSKILL_CALLFUNC PETSKILL_getPetskillFuncPointer(char* name )
{
    int i;
    int hash = hashpjw( name );
    for( i = 0 ; i< arraysizeof( PETSKILL_functbl) ; i++ ) {
        if( PETSKILL_functbl[i].hash == hash ) {
			if( strcmp( PETSKILL_functbl[i].functionname, name ) == 0 )  {
				return PETSKILL_functbl[i].func;
			}
		}
	}
    return NULL;
}

int PETSKILL_Use(
	int charaindex,
	int havepetskill,
	int toindex,
	char *data
	//BOOL isCLI	// Robin 2001/02/26 if owner is player
)
{
	int		array, petskillid;
	int		ret,i;
	PETSKILL_CALLFUNC func;
    char combined[32], *pszOption;

	petskillid = CHAR_getPetSkill( charaindex, havepetskill);
#ifdef _FIXWOLF	 // Syu ADD 党淏曖�丳韎檨ug
    if( petskillid == 600 ){
		if ( CHAR_getInt ( charaindex , CHAR_BASEIMAGENUMBER) == 101428 )
        petskillid = -1;
	}
#endif

	array = PETSKILL_getPetskillArray( petskillid);
	if( array == -1 ) return FALSE;

	if(PETSKILL_getInt(array, PETSKILL_ILLEGAL)){		
		if( (CHAR_getInt(charaindex, CHAR_WHICHTYPE)==CHAR_TYPEPET) ){
			print(" CHAR_TYPEPET:%d ", charaindex);
			return FALSE;
		}
	}
#ifdef _PETSKILL_CHECKTYPE
#define _SKILLTYPE_NONE 0x01
#define _SKILLTYPE_BATTLE 0x02
	if( (CHAR_getInt(charaindex, CHAR_WHICHTYPE)==CHAR_TYPEPET) &&
		(PETSKILL_getInt( array, PETSKILL_USETYPE) & _SKILLTYPE_BATTLE) > 0){
		int masterindex = CHAR_getWorkInt( charaindex, CHAR_WORKPLAYERINDEX);
		if( !CHAR_CHECKINDEX( masterindex) ) return FALSE;
		if( CHAR_getWorkInt( masterindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){
			return FALSE;
		}
	}
#endif
	func = PETSKILL_getPetskillFuncPointer( PETSKILL_getChar( array, PETSKILL_FUNCNAME) );
	if( func ) {
		ret = func( charaindex,  toindex,  array, data );
	}else {
		ret = FALSE;
	}
	return ret;
}


int _PETSKILL_GetArray( char *file, int line, int charaindex, int havepetskill )
{
	int		petskillid,array;

	petskillid = CHAR_getPetSkill( charaindex, havepetskill);
	if( petskillid == -1 ) {
		return -1;
	}

	array = PETSKILL_getPetskillArray( petskillid);
	
	return array;
}


//----------------------------------------------------------------------
int PETSKILL_ContinuationAttack(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
	char *pszOption;
	int N = 1;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_RENZOKU );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( sscanf( pszOption, "%d", &N ) == 1 ){
		if( N < 1 || N > 10 )N = 1;
	}else{
		N = 1;
	}

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, N );

	return TRUE;
}

int PETSKILL_ChargeAttack(
	int charaindex,
	int toindex,
	int array,
	char *data
)
{
	char *pszOption, *pszP;
	int N = 1, Per = 0;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_CHARGE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( sscanf( pszOption, "%d", &N ) == 1 ){
		if( N < 1 || N > 10 )N = 1;
	}else{
		N = 1;
	}
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%d", &Per );
	}
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, N );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, Per );
	return TRUE;
}

int PETSKILL_Guardian(
	int charaindex,
	int toNo,
	int array,
	char *data
)
{
	char *pszOption, *pszP;
	int
		battleindex, side, pos, ownerpos,
		strdef, flg;
	float fPer = 0.01;

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}

	if( (pszP = strstr( pszOption, "滅%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKDEFENCEPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH)+strdef));
	}

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_GUARDIAN_ATTACK );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	flg = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLG );
	flg |= CHAR_BATTLEFLG_GUARDIAN;
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEFLG, flg );

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	pos = BATTLE_Index2No( battleindex, charaindex );

	if( (pszP = strstr( pszOption, "COM:" ) ) != NULL
	&&  strstr( pszP+4, "滅郘" ) != NULL
	){
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD );

		side = ( toNo >= SIDE_OFFSET )?( 1 ):( 0 );
		ownerpos = toNo - side * SIDE_OFFSET;

		if( 0 <= ownerpos && ownerpos < SIDE_OFFSET ){
			BattleArray[battleindex].Side[side].Entry[ownerpos].guardian = pos;
		}

	}else{
		side = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLESIDE );
		ownerpos = pos - 5;
		ownerpos -= side * SIDE_OFFSET;

		if( ownerpos < 0 || ownerpos > 19 ){
		}else{
			BattleArray[battleindex].Side[side].Entry[ownerpos].guardian = pos;
		}
	}

	return TRUE;
}

int PETSKILL_Mighty(
	int charaindex,
	int toindex,
	int array,
	char *data
)
{
	char *pszOption, *pszP;
	int iBai = 0, iDuck = 0;
	float fBai = 2.00;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_MIGHTY );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( (pszP = strstr( pszOption, "捷" ) ) != NULL ){
		sscanf( pszP+2, "%f", &fBai );
		iBai = (int)(fBai * 100);
	}
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, iBai );
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( ( pszP = strstr( pszOption, "旌" ) ) != NULL){
		sscanf(	 pszP+2, "%d", &iDuck );
	}
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, iDuck );
	return TRUE;
}



int PETSKILL_PowerBalance(
	int charaindex,
	int toindex,
	int array,
	char *data
)
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_POWERBALANCE);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL )	{
		print("\n pszOption == NULL ");
		return FALSE;
	}
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}

	if( (pszP = strstr( pszOption, "滅%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKDEFENCEPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH)+strdef));
	}

	return TRUE;
}

//----------------------------------------------------------------------
int PETSKILL_StatusChange(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef;
	int status = -1, i = 0, turn = 3;
	char szTurn[] = "turn";

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_STATUSCHANGE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	pszP = pszOption;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, i );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, turn );

	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,
			(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef) );
	}

	if( (pszP = strstr( pszOption, "滅%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);

		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKDEFENCEPOWER,
			(CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH)+strdef));
	}


	return TRUE;

}

#ifdef _VARY_WOLF

int PETSKILL_Vary( int cindex, int tindex, int id, char* data)
{
	char  *pszOption, *pszP;
	float fPer = 0.01;
	int a_dep;
	int d_dep;
	int i;
	int petid[4] = {981,982,983,984};

	for( i=0; i<4; i++)	{
		if( CHAR_getInt( cindex, CHAR_PETID) == petid[i] ) break;
	}
	if( i>= 4 ) return FALSE;

	CHAR_setWorkInt( cindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_VARY);
	CHAR_setWorkInt( cindex, CHAR_WORKBATTLECOM2, tindex );
	CHAR_setWorkInt( cindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( id, PETSKILL_OPTION );


	if(pszOption==NULL){
		print("\n pszOption==NULL");
		return FALSE;
	}
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		CHAR_setWorkInt( cindex, CHAR_SKILLSTRPOWER, fPer);
		fPer = fPer / 100;
		a_dep = CHAR_getWorkInt( cindex, CHAR_WORKFIXSTR);
		a_dep = (int)(a_dep * fPer);		
		CHAR_setWorkInt( cindex, CHAR_WORKATTACKPOWER, CHAR_getWorkInt( cindex, CHAR_WORKFIXSTR) + a_dep);
		
	}
	if( (pszP = strstr( pszOption, "鏗%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		CHAR_setWorkInt( cindex, CHAR_SKILLDEXPOWER, fPer);
		fPer = fPer / 100;
		d_dep = CHAR_getWorkInt( cindex, CHAR_WORKFIXDEX);
		d_dep = (int)(d_dep * fPer);		
		CHAR_setWorkInt( cindex, CHAR_WORKQUICK, CHAR_getWorkInt( cindex, CHAR_WORKFIXDEX) + d_dep);
		
	}
	CHAR_setInt( cindex, CHAR_BASEIMAGENUMBER, 101428);	
	CHAR_setWorkInt( cindex, CHAR_WORKTURN, 0);

	return TRUE;
}
#endif

#ifdef _SKILL_WILDVIOLENT_ATT

int PETSKILL_WildViolentAttack(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef=0;
	int iDuck = 0;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_WILDVIOLENTATTACK);

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL )	{
		print("\n pszOption == NULL ");
		return FALSE;
	}
	//黍�★皮�
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);

		//扢隅党淏硉
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}
    //黍�◎氈�
	if( (pszP = strstr( pszOption, "滅%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);

		//扢隅党淏硉
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKDEFENCEPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH)+strdef));
	}
    //黍�§嗒�
	if( ( pszP = strstr( pszOption, "旌" ) ) != NULL){
		sscanf(	 pszP+2, "%d", &iDuck );
	}
	//       臚諜禱瞀凎
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, iDuck );

	return TRUE;
}
#endif

#ifdef _SKILL_SACRIFICE
int PETSKILL_Sacrifice(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
	char buf[256];

      if(CHAR_getInt(charaindex,CHAR_HP) > CHAR_getWorkInt(charaindex,CHAR_WORKMAXHP)*0.2 )
		{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SACRIFICE);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		}
	  else
		{
		        
			sprintf(buf, "%s赻旯騵壅薯祥逋 寰堔囮啖!!", CHAR_getChar( charaindex, CHAR_NAME ));
			CHAR_talkToCli( CHAR_getWorkInt(charaindex, CHAR_WORKPLAYERINDEX), -1, buf, CHAR_COLORYELLOW);
				return FALSE;
		}
				return TRUE;
}
#endif
#ifdef _SKILL_REFRESH
int PETSKILL_Refresh(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{

			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_REFRESH);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
				return TRUE;
}
#endif
#ifdef _SKILL_WEAKEN  //vincent唾撮:剞��
int PETSKILL_Weaken(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_WEAKEN);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
				return TRUE;
}
#endif
#ifdef _SKILL_DEEPPOISON  //vincent唾撮:曄馮 
int PETSKILL_Deeppoison(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_DEEPPOISON);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
				return TRUE;
}
#endif

#ifdef _SKILL_BARRIER  //vincent唾撮:藹梤
int PETSKILL_Barrier(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_BARRIER);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
				return TRUE;
}
#endif

#ifdef _SKILL_NOCAST  //vincent唾撮:麥蘇
int PETSKILL_Nocast(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_NOCAST);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
				return TRUE;
}
#endif

#ifdef _SKILL_ROAR //vincent唾撮:湮綾
int PETSKILL_Roar(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ROAR);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
				return TRUE;
}
#endif

#ifdef _SKILL_GUARDBREAK2//ぢ壺滅郘2 vincent add 2002/05/20
int PETSKILL_GuardBreak2(
	int charaindex,
	int toNo,
	int array,
	char *data

)
{

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_GBREAK2 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;

}
#endif 

//*******************************************************
int PETSKILL_NormalAttack(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;

}

int PETSKILL_NormalGuard(
	int charaindex,
	int toindex,
	int array,
	char *data

)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;

}


int PETSKILL_None(
	int charaindex,
	int toindex,
	int array,
	char *data
)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;
}


int PETSKILL_EarthRound(
	int charaindex,
	int toNo,
	int array,
	char *data
)
{
	char *pszOption, *pszP;
	float fPer = 0.01;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_EARTHROUND1 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM3, (int)fPer );
	}


	return TRUE;

}


int PETSKILL_GuardBreak(
	int charaindex,
	int toNo,
	int array,
	char *data

)
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_GBREAK );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);

		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}


	return TRUE;

}
#ifdef _SKILL_SPEEDY_ATT//撞厒馴僻 vincent add 2002/05/20
int PETSKILL_SpeedyAttack(
	int charaindex,
	int toNo,
	int array,
	char *data

)
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef=0;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SPEEDYATTACK );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL )	{
		print("\n pszOption == NULL ");
		return FALSE;
	}

    //黍�◎氈�
	if( (pszP = strstr( pszOption, "滅%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);

		//扢隅党淏硉
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKDEFENCEPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH)+strdef));
	}

	return TRUE;

}
#endif 

int PETSKILL_Abduct(
	int charaindex,
	int toNo,
	int array,
	char *data
)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ABDUCT );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);

	return TRUE;
}

int PETSKILL_Steal(
	int charaindex,
	int toNo,
	int array,
	char *data
)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_STEAL );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	return TRUE;
}

#ifdef _BATTLESTEAL_FIX
int PETSKILL_StealMoney( int charaindex, int toNo, int array, char *data)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_STEALMONEY );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	return TRUE;
}
#endif

//*******************************************************
// 歕  -- 囮騷  堜譴埬
//
int PETSKILL_Merge(
	int charaindex,
	int toNo,
	int array,
	char *data
)
{
	int ret, ownerindex;
	
	// Robin 0521 debug
	ownerindex = CHAR_getWorkInt( charaindex, CHAR_WORKPLAYERINDEX);	
	if( CHAR_getWorkInt( ownerindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ){
		print(" Merge_ErrB:fd=%d ", ownerindex);
		return FALSE;
	}
	ret = ITEM_mergeItem_merge( CHAR_getWorkInt( charaindex, CHAR_WORKPLAYERINDEX),
							CHAR_getInt( charaindex, CHAR_PETID), data, charaindex, 0);
	return ret;
}

#ifdef _ALCHEMIST
int PETSKILL_Alchemist(
	int charaindex,
	int toNo,
	int array,
	char *data
)
{
	int ret, ownerindex;
	
	// Robin 0521 debug
	ownerindex = CHAR_getWorkInt( charaindex, CHAR_WORKPLAYERINDEX);	
	if( CHAR_getWorkInt( ownerindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ){
		print(" Merge_Alchemist_ErrB:fd=%d ", ownerindex);
		return FALSE;
	}
	ret = ITEM_mergeItem_merge( CHAR_getWorkInt( charaindex, CHAR_WORKPLAYERINDEX),
							CHAR_getInt( charaindex, CHAR_PETID), data, charaindex, 1);
	return ret;
}
#endif

int PETSKILL_NoGuard(
	int charaindex,
	int toNo,
	int array,
	char *data

)
{
	char *pszOption, *pszP;
	int Duck = 0, Counter = 0, Critical = 0;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_NOGUARD );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( (pszP = strstr( pszOption, "旌%" ) ) != NULL ){
		sscanf( pszP+3, "%d", &Duck );
		CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, Duck );
	}
	if( (pszP = strstr( pszOption, "僻%" ) ) != NULL ){
		sscanf( pszP+3, "%d", &Counter );
	}
	if( (pszP = strstr( pszOption, "陑%" ) ) != NULL ){
		sscanf( pszP+3, "%d", &Critical );
	}
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3,
		(Counter << 8) + Critical );

	return TRUE;
}

#ifdef __ATTACK_MAGIC

int PETSKILL_AttackMagic(
	int charaindex,
	int toindex,
	int array,
	char *data
)
{
	char *pszP;
	char szMagic[] = "magic";
	char szItem[] = "item";
	int magic = 313,item = 19659;	// magic id啎扢峈鳶藹楊脹撰珨,item index啎扢峈19659

	// �△籀駘嚗敵傶�鍔
	pszP = PETSKILL_getChar( array, PETSKILL_OPTION );

	// �△蟾弧霾闡孚巡獻agic id
	if((pszP = strstr(pszP,szMagic)) != NULL)
	{
		pszP += sizeof(szMagic);
		sscanf(pszP,"%d",&magic);
	}
	// �△譚迡侐孚京譝埽瀾tem index
	if((pszP = strstr(pszP,szItem)) != NULL)
	{
		pszP += sizeof(szItem);
		sscanf(pszP,"%d",&item);
	}
	// 扢隅唾昜撮夔峈藹楊
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM1,BATTLE_COM_S_ATTACK_MAGIC );
	// 馴僻醴梓扢隅
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM2,toindex);
	// 扢隅垀妏蚚腔岆饒珨笱藹楊
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM3,magic);
	// 扢隅垀妏蚚腔藹楊腔耋撿
	CHAR_SETWORKINT_HIGH(charaindex,CHAR_WORKBATTLECOM3,item);
	// 韜鍔狟湛俇救
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLEMODE,BATTLE_CHARMODE_C_OK );

	return TRUE;
}

#endif

#ifdef _PSKILL_FALLGROUND	//邈鎮扲
int PETSKILL_FallGround( int charaindex, int toNo, int array, char *data )
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_FALLRIDE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}
	return TRUE;

}	//BATTLE_COM_S_DAMAGETOHP
#endif
#ifdef _PETSKILL_EXPLODE
int PETSKILL_Explode( int charaindex, int toNo, int array, char *data )
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	if( BattleArray[CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX )].type != BATTLE_TYPE_P_vs_P ){
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
		return TRUE;
	}
	else
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_EXPLODE );
	


	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( (pszP = strstr( pszOption, "韜%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKFIXDEX,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXDEX)+strdef));
	}
	if( (pszP = strstr( pszOption, "滅%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}
	return TRUE;

}
#endif

#ifdef _PRO_BATTLEENEMYSKILL
int ENEMYSKILL_ReLife( int enemyindex, int toNo, int array, char *data )
{
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ENEMYRELIFE );
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	return TRUE;
}

int ENEMYSKILL_ReHP( int enemyindex, int toNo, int array, char *data )
{
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ENEMYREHP );
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	return TRUE;
}

int ENEMYSKILL_EnemyHelp( int enemyindex, int toNo, int array, char *data )
{
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ENEMYHELP );
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( enemyindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	return TRUE;
}

#endif

#ifdef _SKILL_DAMAGETOHP	//岓悛撮 { "PETSKILL_DamageToHp",	PETSKILL_DamageToHp, 0},
int PETSKILL_DamageToHp( int charaindex, int toNo, int array, char *data )
{
	char *pszOption;
	char buf1[256];
	int strdef;
	float def=0.00;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_DAMAGETOHP );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM3, array);	//暮翹撮夔
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL ) return FALSE;
	if( getStringFromIndexWithDelim( pszOption, "|", 1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;

	strdef = CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
	def = (atoi( buf1)/100);
	strdef = strdef - (int)(strdef*def);
	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, strdef);

	return TRUE;

}
#endif

#ifdef _Skill_MPDAMAGE
int PETSKILL_MpDamage( int charaindex, int toNo, int array, char *data )
{
	char *pszOption;
	char buf1[256];
	int strdef;
	float def=0.00;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_MPDAMAGE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM3, array);	////暮翹撮夔

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == NULL ) return FALSE;
	if( getStringFromIndexWithDelim( pszOption, "|", 1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;

	def = (float)(atoi( buf1)/100);
	strdef = CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
	strdef = strdef - (int)( strdef*def);
	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, strdef);

	return TRUE;
}
#endif


#ifdef _SKILL_TOOTH
int PETSKILL_ToothCrushe( int charaindex, int toNo, int array, char *data )
{
	//char *pszOption;
	//char buf1[256];
	//int strdef;
	//float def=0.00;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_TOOTHCRUSHE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//暮翹撮夔
/*
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == NULL ) return FALSE;
	if( getStringFromIndexWithDelim( pszOption, "|", 1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;

	def = (float)(atoi( buf1)/100);
	strdef = CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
	strdef = strdef - (int)( strdef*def);
	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, strdef);
*/
	return TRUE;
}
#endif
#ifdef _PSKILL_MODIFY
int PETSKILL_Modifyattack( int charaindex, int toNo, int array, char *data )
{
	//char *pszOption;
	//char buf1[256];
	//int strdef;
	//float def=0.00;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_MODIFYATT );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//暮翹撮夔
/*
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == NULL ) return FALSE;
	if( getStringFromIndexWithDelim( pszOption, "|", 1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;

	def = (float)(atoi( buf1)/100);
	strdef = CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
	strdef = strdef - (int)( strdef*def);
	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, strdef);
*/
	return TRUE;
}
#endif

#ifdef _PSKILL_MDFYATTACK
int PETSKILL_Mdfyattack( int charaindex, int toNo, int array, char *data )
{
	char *pszOption;
	char buf1[256];
	char MNodify[][36]={"EA","WA","FI","WI"};
	int nums, i;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_MDFYATTACK);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == NULL ) return FALSE;
	if( getStringFromIndexWithDelim( pszOption, "|", 1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	for( i=0; i<4; i++)	{
		if( strcmp( buf1, MNodify[i]) ) continue;
		break;
	}
	if( i == 4 ) return FALSE;
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM4, i);//記錄屬性
	if( getStringFromIndexWithDelim( pszOption, "|", 2, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	nums=atoi( buf1);
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM4, nums);//暮翹杅硉
	return TRUE;
}
#endif

#ifdef _ITEM_INSLAY
int PETSKILL_Inslay( int index, int toNo, int array, char *data)
{
	int charaindex, petid;

	int inslayindex=-1, inslayno=-1;
	int i, j, itemno;
#define MAXINSLAY 4
	int		itemindexs[MAXINSLAY]={-1,-1,-1,-1};
	int		itemnos[MAXINSLAY]={-1,-1,-1,-1};

	if( !CHAR_CHECKINDEX( index) ) return FALSE;
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPET ) return FALSE;
	charaindex = CHAR_getWorkInt( index, CHAR_WORKPLAYERINDEX);
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;

	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE )
		return FALSE;
	petid = CHAR_getInt( charaindex, CHAR_PETID);


	for( i = 0; i < 15; i ++ ) {
		char buf[256];
		char *buf1;
		int itemindex;
		if( getStringFromIndexWithDelim( data , "|", i+1, buf,  sizeof( buf ) ) == FALSE ) break;
		itemno = atoi( buf);
		if( !CHAR_CHECKITEMINDEX( charaindex, itemno) ) continue;
		itemindex = CHAR_getItemIndex( charaindex, itemno);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;

		if( i >= MAXINSLAY )	{
			CHAR_talkToCli( charaindex, -1, "儕馱藩棒硐夔恁寁侐跺昜こ﹝", CHAR_COLORYELLOW);
			return FALSE;
		}
		for( j=0; j< MAXINSLAY; j++ ) {
			if( itemindexs[j] == itemindex )	{
				print("儕馱昜こindex眈肮 ERROR !!\n");
				return FALSE;
			}
		}
		if( (buf1 = ITEM_getChar( itemindex, ITEM_TYPECODE)) == NULL )	{
			print(" ITEM_TYPECODE == NULL 渣昫 !!\n");
			return FALSE;
		}
		if( !strcmp( buf1, "\0") || !strcmp( buf1, "NULL") )	{
			CHAR_talkToCli( charaindex, -1, "涴欴昜こ祥巠磁儕馱﹝", CHAR_COLORYELLOW);
			return FALSE;
		}
		if( strstr( buf1, "INSLAY") != 0 )	{
			if( !ITEM_CHECKINDEX( inslayindex))	{
				inslayindex = itemindex;
				inslayno = itemno;
			}else	{
				CHAR_talkToCli( charaindex, -1, "儕馱藩棒硐夔恁寁珨跺挕ん麼滅撿﹝", CHAR_COLORYELLOW);
				return FALSE;
			}
		}
		itemindexs[i] = itemindex;
		itemnos[i] = itemno;
	}

	if( !ITEM_CHECKINDEX( inslayindex) )	{
		CHAR_talkToCli( charaindex, -1, "儕馱斛剕恁寁珨跺挕ん麼滅撿﹝", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i=0; i< MAXINSLAY; i++) {
		if( !ITEM_CHECKINDEX( itemindexs[i]) )break;
		if( inslayindex == itemindexs[i] ) continue;
		if( PETSKILL_ITEM_inslay( charaindex, inslayindex, itemindexs[i] ) == FALSE ){
			return FALSE;
		}
		CHAR_DelItem( charaindex, itemnos[i]);
/*
		CHAR_setItemIndex( charaindex, itemnos[i] ,-1);
		CHAR_sendItemDataOne( charaindex, itemnos[i] );
		ITEM_endExistItemsOne( itemindexs[i]);
*/
	}

   	CHAR_sendItemDataOne( charaindex, inslayno );
	CHAR_complianceParameter( charaindex );
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_ATK | CHAR_P_STRING_DEF 
		| CHAR_P_STRING_QUICK | CHAR_P_STRING_CHARM );

	return TRUE;
}

#endif

#ifdef _MAGIC_SUPERWALL
int	PETSKILL_MagicStatusChange( int charaindex, int toindex, int array, char *data )
{
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SUPERWALL);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toindex );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性

	return TRUE;
}

#endif

#ifdef _PETSKILL_SETDUCK
int PETSKILL_SetDuck( int charaindex, int toNo, int array, char *data )
{
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )
		return FALSE;
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SETDUCK);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性


	CHAR_setWorkInt( charaindex, CHAR_MAGICPETMP, 0 );
	return TRUE;
}
#endif

#ifdef _MAGICPET_SKILL
int PETSKILL_SetMagicPet( int charaindex, int toNo, int array, char *data )
{
	int nums=0;
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )
		return FALSE;

	nums = CHAR_getWorkInt( charaindex, CHAR_MAGICPETMP);
	if( nums >= 3 ){
		int toindex = CHAR_getWorkInt( charaindex, CHAR_WORKPLAYERINDEX);
		CHAR_talkToCli( toindex, -1, "森撮夔等部癹蚚��棒﹝", CHAR_COLORYELLOW);
		return FALSE;
	}
	CHAR_setWorkInt( charaindex, CHAR_MAGICPETMP, nums );

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SETMAGICPET);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性
	return TRUE;
}
#endif

#ifdef _PETSKILL_FIXITEM
int PETSKILL_Fixitem( int index, int toNo, int array, char *data)
{
	int charaindex, petid;

	int fixindex=-1, fixno=-1;
	int i, j, itemno;
#define MAXFIXITEM 2
	int		itemindexs[MAXFIXITEM]={-1,-1};
	int		itemnos[MAXFIXITEM]={-1,-1};

	if( !CHAR_CHECKINDEX( index) ) return FALSE;
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPET ) return FALSE;
	charaindex = CHAR_getWorkInt( index, CHAR_WORKPLAYERINDEX);
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;

	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE )
		return FALSE;
	petid = CHAR_getInt( charaindex, CHAR_PETID);


	for( i = 0; i < 15; i ++ ) {
		char buf[256];
		int itemtype=-1;
		int itemindex;
		if( getStringFromIndexWithDelim( data , "|", i+1, buf,  sizeof( buf ) ) == FALSE ) break;
		itemno = atoi( buf);
		if( !CHAR_CHECKITEMINDEX( charaindex, itemno) ) continue;
		itemindex = CHAR_getItemIndex( charaindex, itemno);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;

		if( i >= MAXFIXITEM )	{
			CHAR_talkToCli( charaindex, -1, "党葩藩棒硐夔恁寁謗跺昜こ﹝", CHAR_COLORYELLOW);
			return FALSE;
		}
		for( j=0; j< MAXFIXITEM; j++ ) {
			if( itemindexs[j] == itemindex )	{
				print("党葩昜こindex眈肮 ERROR !!\n");
				return FALSE;
			}
		}
		itemtype = ITEM_getInt( itemindex, ITEM_TYPE);
		if( itemtype == ITEM_DISH )	{
			CHAR_talkToCli( charaindex, -1, "蹋燴祥夔酕党葩﹝", CHAR_COLORYELLOW);
			return FALSE;
		}

		if( (itemtype >= ITEM_FIST && itemtype <= ITEM_AMULET) || 
			itemtype == ITEM_BOOMERANG || itemtype == ITEM_BOUNDTHROW || itemtype == ITEM_BREAKTHROW){
			if( !ITEM_CHECKINDEX( fixindex))	{
				fixindex = itemindex;
				fixno = itemno;
			}else	{
				CHAR_talkToCli( charaindex, -1, "党葩藩棒硐夔恁寁珨跺挕ん麼滅撿﹝", CHAR_COLORYELLOW);
				return FALSE;
			}
		}

		itemindexs[i] = itemindex;
		itemnos[i] = itemno;
	}

	if( !ITEM_CHECKINDEX( fixindex) )	{
		CHAR_talkToCli( charaindex, -1, "党葩斛剕恁寁珨跺挕ん麼滅撿﹝", CHAR_COLORYELLOW);
		return FALSE;
	}

	if( PETSKILL_ITEM_FixItem( charaindex, fixindex, itemindexs ) == FALSE ){
		return FALSE;
	}else	{
		for( i=0; i< MAXFIXITEM; i++) {
			if( !ITEM_CHECKINDEX( itemindexs[i]) )break;
			if( fixindex == itemindexs[i] ) continue;
			CHAR_setItemIndex( charaindex, itemnos[i] ,-1);
			CHAR_sendItemDataOne( charaindex, itemnos[i] );
			ITEM_endExistItemsOne( itemindexs[i]);
		}
	}

   	CHAR_sendItemDataOne( charaindex, fixno );
	CHAR_complianceParameter( charaindex );
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_ATK | CHAR_P_STRING_DEF 
		| CHAR_P_STRING_QUICK | CHAR_P_STRING_CHARM );
	CHAR_talkToCli( charaindex, -1, "党葩俇傖﹝", CHAR_COLORYELLOW);
	return TRUE;
}
#endif

#ifdef _PETSKILL_TIMID
int PETSKILL_BattleTimid( int charaindex, int toNo, int array, char *data )
{
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_TIMID );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)*0.7) );
	CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)*0.4) );
	CHAR_setWorkInt( charaindex, CHAR_WORKQUICK, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX)*0.8) );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性

	return TRUE;
}
#endif
#ifdef _PETSKILL_2TIMID
int PETSKILL_2BattleTimid( int charaindex, int toNo, int array, char *data )
{
	char *pszOption=NULL;
	char *pszP;
	float fPer=0;
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_2TIMID );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == NULL ) return FALSE;
	if( (pszP = strstr( pszOption, "-馴%" ) ) != NULL ){
		sscanf( pszP+4, "%f", &fPer );
		fPer=(fPer/100);
		CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)*fPer) );
	}
	if( (pszP = strstr( pszOption, "+馴%" ) ) != NULL ){
		sscanf( pszP+4, "%f", &fPer );
		fPer=(fPer/100);
		CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)+(CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)*fPer) );
	}
	if( (pszP = strstr( pszOption, "-滅%" ) ) != NULL ){
		sscanf( pszP+4, "%f", &fPer );
		fPer=(fPer/100);
		CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)*fPer) );
	}
	if( (pszP = strstr( pszOption, "+滅%" ) ) != NULL ){
		sscanf( pszP+4, "%f", &fPer );
		fPer=(fPer/100);
		CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)+(CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)*fPer) );
	}
	if( (pszP = strstr( pszOption, "-鏗%" ) ) != NULL ){
		sscanf( pszP+4, "%f", &fPer );
		fPer=(fPer/100);
		CHAR_setWorkInt( charaindex, CHAR_WORKQUICK, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX)*fPer) );
	}
	if( (pszP = strstr( pszOption, "+鏗%" ) ) != NULL ){
		sscanf( pszP+4, "%f", &fPer );
		fPer=(fPer/100);
		CHAR_setWorkInt( charaindex, CHAR_WORKQUICK, CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX)+(CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX)*fPer) );
	}

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性

	return TRUE;
}
#endif
#ifdef _PETSKILL_ANTINTER
int PETSKILL_AntInter( int charaindex, int toNo, int array, char* data)
{
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;	

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ANTINTER);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性
	return TRUE;
}
#endif

#ifdef _PETSKILL_PROPERTY
int PETSKILL_BattleProperty( int charaindex, int toNo, int array, char *data )
{
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_PROPERTYSKILL );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

//	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)*0.7) );
//	CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)*0.5) );
//	CHAR_setWorkInt( charaindex, CHAR_WORKQUICK, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX)*0.95) );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性

	return TRUE;
}
#endif

#ifdef _PETSKILL_TEAR
int PETSKILL_BattleTearDamage( int charaindex, int toNo, int array, char *data )
{
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_PETSKILLTEAR );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)*0.9) );
	CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)*0.8) );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性
	return TRUE;
}
#endif


#ifdef _BATTLE_LIGHTTAKE
int PETSKILL_Lighttakeed( int charaindex, int toNo, int array, char *data )
{
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_LIGHTTAKE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)*0.7) );
	CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)*0.5) );
//	CHAR_setWorkInt( charaindex, CHAR_WORKQUICK, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX)*0.95) );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性
	return TRUE;
}
#endif

#ifdef _BATTLE_ATTCRAZED
int PETSKILL_AttackCrazed( int charaindex, int toNo, int array, char *data )
{
	char *pszOption=NULL;
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ATTCRAZED );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR)*0.8) );
	CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH)*0.7) );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == NULL ) return FALSE;

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, atoi(pszOption));
	return TRUE;
}
#endif

#ifdef _SHOOTCHESTNUT	// Syu ADD 唾撮ㄩ隍璦赽
int PETSKILL_AttackShoot( int charaindex, int toNo, int array, char *data )
{
	//int n = RAND ( 3 , 5 ) ; 
	int n ; 
	int loyal = CHAR_getWorkInt( charaindex, CHAR_WORKFIXAI );
	char bb1[16] , bb2[16] ; 
	char *buf = NULL ; 
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) return FALSE;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ATTSHOOT );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	buf = PETSKILL_getChar( array , PETSKILL_OPTION ) ; 
	if ( buf == NULL ) return FALSE;
	getStringFromIndexWithDelim( buf , "|", 1 , bb1 , sizeof( bb1 ) );
	getStringFromIndexWithDelim( buf , "|", 2 , bb2 , sizeof( bb2 ) );
	n = RAND ( atoi(bb1) , atoi(bb2) ) ; 
	if ( loyal >= 100 ) {
		if ( RAND ( 1 , 300 ) > 299 )
			n = 8 ;
		else if ( (CHAR_getInt( charaindex , CHAR_HP )) < 20 && ( RAND ( 1 , 50 ) > 49 ) )
			n = 8 ; 
	}
/*
	if ( n != 8 )
		CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR) * 1.2 / n ) );
	else 
		CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR) * 1.2 / 4 ) );
	CHAR_setWorkInt( charaindex, CHAR_WORKDEFENCEPOWER, (CHAR_getWorkInt( charaindex, CHAR_WORKFIXTOUGH) * 1) );
*/
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);//記錄屬性
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, n);
	return TRUE;
}
#endif

#ifdef _PET_SKILL_SARS				// WON ADD 職業延長
int PETSKILL_Sars( int charaindex, int toNo, int array, char *data )
{

	char *pszOption, *pszP;
	int status = -1, i = 0, turn = 3;
	char szTurn[] = "turn";

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SARS );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	pszP = pszOption;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, status );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, turn );

	return TRUE;


}
#endif

#ifdef _SONIC_ATTACK				// WON ADD 秞疏馴僻
int PETSKILL_Sonic( int charaindex, int toNo, int array, char *data )
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SONIC );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性
	return TRUE;

}
#endif
#ifdef _PETSKILL_REGRET
int PETSKILL_Regret( int charaindex, int toNo, int array, char *data )
{
	char *pszOption, *pszP;
	int strdef;
	float fPer = 0.01;
	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}

	if( (pszP = strstr( pszOption, "滅%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKDEFENCEPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH)+strdef));
	}

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_REGRET );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性
	return TRUE;

}
#endif

#ifdef _PETSKILL_GYRATE
int PETSKILL_Gyrate( int charaindex, int toNo, int array, char *data )
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef=0;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_GYRATE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo ); //馴僻勤砓  23,24,25,26 峈等齬
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL )	{
		print("\n pszOption == NULL ");
		return FALSE;
	}

	//黍�★皮�
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);

		//扢隅党淏硉
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性
	return TRUE;
}
#endif

#ifdef _PETSKILL_ACUPUNCTURE
int PETSKILL_Acupuncture( int charaindex, int toNo, int array, char *data )
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_ACUPUNCTURE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性
	return TRUE;
}
#endif

#ifdef _PETSKILL_RETRACE
int PETSKILL_Retrace( int charaindex, int toNo, int array, char *data )
{
	//char *pszOption, *pszP;
	//float fPer = 0.01;
	//int strdef=0;

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_RETRACE );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

/*	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL )	{
		print("\n pszOption == NULL ");
		return FALSE;
	}

	//黍�★皮�
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);
		//扢隅党淏硉
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}
*/
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性
	return TRUE;
}
#endif

#ifdef _PETSKILL_HECTOR
int PETSKILL_Hector( int charaindex, int toNo, int array, char *data )
{
	char *pszOption, *pszP;
	float fPer = 0.01;
	int strdef;
	int status = -1;//, i = 0, turn = 3;
	//char szTurn[] = "turn";
	int d_dep;
	
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_HECTOR );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL )	{
		print("\n pszOption == NULL ");
		return FALSE;
	}

	//黍�★皮�
	if( (pszP = strstr( pszOption, "馴%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		fPer=(fPer/100);

		//扢隅党淏硉
		strdef=CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
		strdef=(int)(strdef * fPer);
		CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER,(CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR)+strdef));
	}
    
	if( (pszP = strstr( pszOption, "鏗%" ) ) != NULL ){
		sscanf( pszP+3, "%f", &fPer );
		CHAR_setWorkInt( charaindex, CHAR_SKILLDEXPOWER, fPer);
		fPer = fPer / 100;
		d_dep = CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX);
		d_dep = (int)(d_dep * fPer);		
		CHAR_setWorkInt( charaindex, CHAR_WORKQUICK, CHAR_getWorkInt( charaindex, CHAR_WORKFIXDEX) + d_dep);	
	}

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性

	return TRUE;
}
#endif

#ifdef _PETSKILL_FIREKILL
int PETSKILL_Firekill( int charaindex, int toNo, int array, char *data )
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_FIREKILL );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性

	return TRUE;
}
#endif

#ifdef _PETSKILL_DAMAGETOHP
int PETSKILL_DamageToHp2( int charaindex, int toNo, int array, char *data )
{

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_DAMAGETOHP2 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM3, array);	//暮翹撮夔
/*	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );

	if( pszOption == NULL ) return FALSE;

	strdef = CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR);
	def = (atoi( pszOption)/100);
	strdef = strdef - (int)(strdef*def);
	CHAR_setWorkInt( charaindex, CHAR_WORKATTACKPOWER, strdef);
*/

	return TRUE;

}
#endif

#ifdef _PETSKILL_BECOMEFOX
int PETSKILL_BecomeFox( int charaindex, int toNo, int array, char* data)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_BECOMEFOX);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM3, array);	//暮翹撮夔

	return TRUE;
}
#endif

#ifdef _PETSKILL_BECOMEPIG
int PETSKILL_BecomePig( int charaindex, int toNo, int array, char* data)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_BECOMEPIG);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);	//記錄屬性
	return TRUE;
}
#endif

#ifdef _PETSKILL_LER
int PETSKILL_BatFly(int charaindex,int toNo,int array,char* data)
{
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM1,BATTLE_COM_S_BAT_FLY);
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM2,toNo);
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLEMODE,BATTLE_CHARMODE_C_OK);
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM3,array);
	return TRUE;
}

int PETSKILL_DivideAttack( int charaindex, int toNo, int array, char* data)
{
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM1,BATTLE_COM_S_DIVIDE_ATTACK);
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM2,toNo);
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLEMODE,BATTLE_CHARMODE_C_OK);
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM3,array);
	return TRUE;
}
#endif

#ifdef _PETSKILL_BATTLE_MODEL
int PETSKILL_BattleModel(int charaindex, int toindex, int array, char* data)
{
	float fPer;
	int i,iType,iObjectNum,iValue;
	int iAddPowerType[3] = {CHAR_WORKATTACKPOWER,CHAR_WORKDEFENCEPOWER,CHAR_WORKQUICK};
	char *pszOption,szData[32],szData2[32];
	char szWord[3][3] = {"馴","滅","鏗"};

  pszOption = PETSKILL_getChar(array,PETSKILL_OPTION);
	if(pszOption == NULL){
		printf("PETSKILL_BattleModel: 黍�� PETSKILL_OPTION 渣昫!!(杅郪:%d,恅璃:%s,菴%d俴)\n",array,__FILE__,__LINE__);
		return FALSE;
	}

	// �△羅本鷩閛�
	if(getStringFromIndexWithDelim(pszOption,"|",1,szData,sizeof(szData)) == FALSE){
		printf("PETSKILL_BattleModel: no type data!!(恅璃:%s,菴%d俴)\n",__FILE__,__LINE__);
		return FALSE;
	}
	iType = atoi(szData);
	// �△羅本龕嚗�杅講
	if(getStringFromIndexWithDelim(pszOption,"|",2,szData,sizeof(szData)) == FALSE){
		printf("PETSKILL_BattleModel: no object number data!!(恅璃:%s,菴%d俴)\n",__FILE__,__LINE__);
		return FALSE;
	}
	iObjectNum = atoi(szData);
	if(iObjectNum <= 0) iObjectNum = RAND(1,10);
	else if(iObjectNum > 10) iObjectNum = 10;
	// �△藥僋耒鶻�
	if(getStringFromIndexWithDelim(pszOption,"|",6,szData,sizeof(szData)) != FALSE){
		for(i=0;i<3;i++){
			if(getStringFromIndexWithDelim(szData," ",i+1,szData2,sizeof(szData2)) != FALSE){
				// 潰脤扢隅
				if(strstr(szData2,szWord[i]) != NULL){
					iValue = CHAR_getWorkInt(charaindex,CHAR_WORKATTACKPOWER);
					// 衄湍 "%" 腔扢隅 ex: "馴%"
					if(strstr(szData2,"%") != NULL){
						sscanf(szData2 + 3,"%f",&fPer);
						fPer = (fPer/100);
						iValue += (int)(iValue * fPer);
					}
					// 羶湍 "%" 腔扢隅 ex: "馴"
					else{
						sscanf(szData2 + 2,"%f",&fPer);
						iValue = (int)fPer;
					}
					CHAR_setWorkInt(charaindex,iAddPowerType[i],iValue);
				}
			}
			else break;
		}
	}

	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLEMODE,BATTLE_CHARMODE_C_OK);
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM1,BATTLE_COM_S_BATTLE_MODEL);
	CHAR_SETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM2,iType);				// 暮翹馴僻濬倰
	CHAR_SETWORKINT_HIGH(charaindex,CHAR_WORKBATTLECOM2,iObjectNum);	// 暮翹馴僻昜璃杅講
	CHAR_setWorkInt(charaindex,CHAR_WORKBATTLECOM3,array);
	return TRUE;
}
#endif

#ifdef _PETSKILL_SHOWMERCY
int PETSKILL_ShowMercy( int charaindex, int toNo, int array, char* data)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_SHOWMERCY);
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;
}
#endif

#ifdef _PETSKILL_COMBINED
int PETSKILL_Combined( int charaindex, int toNo, int array, char* data)
{
	char *pszOption;
	char combined[32];
	int kill[10],count,i;
	char killstr[32],countstr[32];

    pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == NULL ) return FALSE;

	strcpy( combined, " " );
	getStringFromIndexWithDelim( pszOption, "|", 1, combined, sizeof( combined));
    if( strcmp( combined, "軘磁楊" ) == 0 ){ 
		if( getStringFromIndexWithDelim( pszOption, "|", 2, countstr, sizeof( countstr)) == FALSE )
		    return FALSE;
        count = atoi(countstr);
	    if( count > 10 )
		    count = 10;
	    for(i=0;i<count;i++){
		    if( getStringFromIndexWithDelim( pszOption, "|", 3+i, killstr, sizeof( killstr)) )
		        kill[i] = atoi(killstr);
		}
	}
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_JYUJYUTU );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, kill[rand()%count] );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, 0 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;
}
#endif

