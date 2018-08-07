#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skill.h"
#include "util.h"
#include "char.h"

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
#include "battle.h"
#include "profession_skill.h"
#endif


static SKILL_intDataSetting SKILL_setint[SKILL_DATAINTNUM]={
    {"lv"},           /*  SKILL_LEVEL	*/
    {"id"},           /*  SKILL_ID		*/
};

static SKILL_charDataSetting SKILL_setchar[SKILL_DATACHARNUM]={
};




static char    SKILL_dataString[STRINGBUFSIZ];

char* SKILL_makeStringFromSkillData( Skill* sk )
{
    int     i;
    int     strlength=0;

    for( i = 0 ; i < SKILL_DATAINTNUM ; i ++ ){
        char    linedata[128];
        snprintf( linedata , sizeof(linedata),
                  "%s=%d" NONCHAR_DELIMITER,
                  SKILL_setint[i].dumpskill, sk->data[i] );

        strcpysafe( &SKILL_dataString[strlength],
                    sizeof( SKILL_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( SKILL_dataString ) )goto RETURN;
    }

    for( i = 0 ; i < SKILL_DATACHARNUM ; i ++ ){
        char    linedata[128];
        char    escapebuffer[128];
        snprintf( linedata, sizeof(linedata),
                  "%s=%s" NONCHAR_DELIMITER,
                  SKILL_setchar[i].dumpskill,
                  makeEscapeString(sk->string[i].string,escapebuffer,
                                   sizeof(escapebuffer)));

        strcpysafe( &SKILL_dataString[strlength],
                    sizeof( SKILL_dataString ) - strlength,
                    linedata );
        strlength += strlen( linedata );
        if( strlength > sizeof( SKILL_dataString ) )goto RETURN;
    }

RETURN:
    dchop( SKILL_dataString , NONCHAR_DELIMITER );

    return SKILL_dataString;
}


BOOL SKILL_makeSkillFromStringToArg( char* src, Skill* sk )
{
    int     readindex=1;
    while( 1 ){
        BOOL    ret;
        char    linebuf[512];
        char    first[256];
        char    second[256];
        int     i;


        ret = getStringFromIndexWithDelim( src ,NONCHAR_DELIMITER ,
                                           readindex,
                                           linebuf,  sizeof( linebuf ) );
        if( ret == FALSE )
            break;

        ret = getStringFromIndexWithDelim( linebuf ,"=", 1,
                                           first,  sizeof( first ) );
        if( ret == FALSE )  return FALSE;
        strcpysafe( second , sizeof( second ),
                    linebuf + strlen(first) + strlen("=") );

        for( i = 0 ; i < SKILL_DATAINTNUM ; i ++ ){
            if( strcmp(first ,SKILL_setint[i].dumpskill) == 0 ){
                sk->data[i] = atoi( second );
                goto NEXT;
            }
        }

        for( i = 0 ; i < SKILL_DATACHARNUM ; i ++ ){
            if( strcmp(first ,SKILL_setchar[i].dumpskill) == 0 ){
                strcpysafe( sk->string[i].string,
                            sizeof(sk->string[i].string),
                            makeStringFromEscaped(second) );
                goto NEXT;
            }
        }

        fprint( "??? : %s[%s]\n" , linebuf, first );
        
    NEXT:
        readindex++;
    }

    return TRUE;
}

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
INLINE int SKILL_getRealInt( Skill* skill, int element)
{
    return skill->data[element];
}
#endif

INLINE int SKILL_getInt( Skill* skill, int element)
{
	int value = skill->data[element];

	if( element == SKILL_LEVEL )	value /= 100;

    return value;
}


INLINE int SKILL_setInt( Skill* skill, int element, int new)
{
    int     buf = SKILL_getInt( skill, element );
    skill->data[element] = new;
    return buf;
}

#ifndef _PROFESSION_SKILL			// WON ADD 人物职业技能
static void SKILL_setitemlimit( int charaindex, Skill* sk );
static void SKILL_setmerchant( int charaindex, Skill* sk );
static void SKILL_setlevel( int charaindex, Skill* sk );
#endif

static SKILL_table  SKILL_tbl[]={

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	{ 100,	NULL},		// 1
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},
	{ 100,	NULL},		// 16
#else
    {8,SKILL_setlevel},			/*  SKILL_FIRE				*/
    {4,NULL},					/*  SKILL_MAGICIAN			*/
    {4,NULL},					/*  SKILL_PREIST			*/
    {8,SKILL_setitemlimit},		/*  SKILL_ALOTOFTHINGS		*/
    {8,NULL},					/*  SKILL_AVOIDRATEUP		*/
    {4,SKILL_setlevel},			/*  SKILL_DETERMINEITEM		*/
    {5,SKILL_setlevel},			/*  SKILL_DETERMINEOTHERS	*/
    {8,SKILL_setmerchant},		/*  SKILL_MERCHANT			*/
    {8,SKILL_setlevel},			/*  SKILL_HEALER			*/
    {8,SKILL_setlevel},			/*  SKILL_LARGEVOICE		*/
#endif
};


#ifndef _PROFESSION_SKILL			// WON ADD 人物职业技能
static void SKILL_setitemlimit( int charaindex, Skill* sk )
{
    static int itemlimit[9]={
        7 + 8 + 4*0,
        7 + 8 + 4*1,
        7 + 8 + 4*2,
        7 + 8 + 4*3,
        7 + 8 + 4*4,
        7 + 8 + 4*5,
        7 + 8 + 4*6,
        7 + 8 + 4*7,
        7 + 8 + 4*8,
    };
    int     level;
    if( !CHAR_CHECKINDEX(charaindex) )return;
    if( sk->data[SKILL_IDENTITY] != SKILL_ALOTOFTHINGS )return;
    level = sk->data[SKILL_LEVEL];
    if( level < 0 )level = 0;
    if( level>=arraysizeof(itemlimit) ) level=arraysizeof(itemlimit) - 1;
}


static void SKILL_setmerchant( int charaindex, Skill* sk )
{
    int     level;
    int     merchantlevel=0;

    if( !CHAR_CHECKINDEX(charaindex) )return;
    if( sk->data[SKILL_IDENTITY] != SKILL_MERCHANT )return;

    level = sk->data[SKILL_LEVEL];
    if( level < 0 )level = 0;
    if( level >= SKILL_tbl[sk->data[SKILL_IDENTITY]].maxlevel )
        level  = SKILL_tbl[sk->data[SKILL_IDENTITY]].maxlevel;

    SETHIGHVALUE(merchantlevel,(100 - level*5));
    SETLOWVALUE(merchantlevel,(20 + level*5));
    
    CHAR_setInt( charaindex,CHAR_MERCHANTLEVEL,merchantlevel );
}


static void SKILL_setlevel( int charaindex, Skill* sk )
{
    static struct skillvalset
    {
        SKILL_ID        id;
        CHAR_DATAINT    charadataindex;
    } skvalset[] = {
        { SKILL_DETERMINEITEM,      CHAR_DETERMINEITEM },
        { SKILL_DETERMINEOTHERS,    CHAR_RADARSTRENGTH },
        { SKILL_HEALER,             CHAR_HEALERLEVEL },
        { SKILL_LARGEVOICE,         CHAR_CHATVOLUME },
    };
    int id;
    int i;
    int index=-1;
    int level;

    if( !CHAR_CHECKINDEX(charaindex) )return;
    id = sk->data[SKILL_IDENTITY];
    for( i=0 ; i<arraysizeof(skvalset) ; i ++ )
        if( skvalset[i].id == id )
            index = i;

    if( index == -1 )return;

    level = sk->data[SKILL_LEVEL];
    if( level < 0 )level = 0;
    if( level >= SKILL_tbl[id].maxlevel )
        level  = SKILL_tbl[id].maxlevel;

    CHAR_setInt(charaindex,skvalset[index].charadataindex,level );
}
#endif





#define SKILLSTRINGBUFSIZ   256

static char    ITEM_statusStringBuffer[SKILLSTRINGBUFSIZ];


#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
char* SKILL_makeSkillStatusString( Skill* skill, int charaindex, int skill_num )
{
	  int skillindex=SKILL_getInt( skill,SKILL_IDENTITY);
	  int Pskill=PROFESSION_SKILL_getskillArray( skillindex);
	  int skill_level=0, cost_mp=0;
		
	  // 人物技能等级
	  skill_level = SKILL_getInt( skill, SKILL_LEVEL);
	
	  // 耗费MP
	  if( (cost_mp = PROFESSION_MAGIC_COST_MP( charaindex, skill_num )) == -1 )
		cost_mp = PROFESSION_SKILL_getInt( Pskill, PROFESSION_SKILL_COST_MP);

	  snprintf( ITEM_statusStringBuffer, sizeof( ITEM_statusStringBuffer ),
			"%d|%d|%d|%d|%d|%d|%d|%s|%s",
			PROFESSION_SKILL_getInt( Pskill, PROFESSION_SKILL_USE_FLAG),
            SKILL_getInt(skill,SKILL_IDENTITY),
			PROFESSION_SKILL_getInt( Pskill, PROFESSION_SKILL_TARGET),
		    PROFESSION_SKILL_getInt( Pskill, PROFESSION_SKILL_KIND),
		    PROFESSION_SKILL_getInt( Pskill, PROFESSION_SKILL_ICON),
			cost_mp,
			skill_level,
			PROFESSION_SKILL_getChar( Pskill, PROFESSION_SKILL_NAME),
		    PROFESSION_SKILL_getChar( Pskill, PROFESSION_SKILL_TXT) );


    return ITEM_statusStringBuffer;
}
#else
char* SKILL_makeSkillStatusString( Skill* skill )
{
    snprintf( ITEM_statusStringBuffer,
              sizeof( ITEM_statusStringBuffer ),
              "%d|%d",
              SKILL_getInt(skill,SKILL_IDENTITY),
              SKILL_getInt(skill,SKILL_LEVEL) );


    return ITEM_statusStringBuffer;
}
#endif



char* SKILL_makeSkillFalseString( void )
{
    snprintf( ITEM_statusStringBuffer,
              sizeof( ITEM_statusStringBuffer ),
              "|" );
    return ITEM_statusStringBuffer;
}


BOOL  SKILL_CHECKID( int skillid )
{
    if( SKILL_NUM <= skillid && skillid > 0 )return FALSE;
    return TRUE;
}

BOOL SKILL_makeSkillData( Skill* sk ,int skid, int lev )
{
    sk->data[SKILL_LEVEL] = lev;
    sk->data[SKILL_IDENTITY] = skid;

    return TRUE;
}

int SKILL_levelup( Skill* sk )
{
    int     id = sk->data[SKILL_IDENTITY];
    if( !SKILL_CHECKID(id) )return -1;
    sk->data[SKILL_LEVEL] ++;
    sk->data[SKILL_LEVEL] = min( sk->data[SKILL_LEVEL],
                                 SKILL_tbl[id].maxlevel );
    return TRUE;
}

int SKILL_getLevelFromSkillID( int charaindex, SKILL_ID id )
{
    int     i;
    if( !CHAR_CHECKINDEX(charaindex) )return -1;
    for( i=0 ; i<CHAR_SKILLMAXHAVE ; i++ ){
        CHAR_HaveSkill* sk;
        sk = CHAR_getCharHaveSkill(charaindex,i);
        if( sk && sk->use && sk->skill.data[SKILL_IDENTITY] == id )
            return sk->skill.data[SKILL_LEVEL];
        
    }
    return -1;
}

BOOL SKILL_getUpableSkillID( int charaindex,char* buf, int buflen )
{
    int     i;
    if( !CHAR_CHECKINDEX(charaindex)) return FALSE;
    if( buflen <= 0 )return FALSE;
    buf[0] = '\0';
    for( i=0 ; i<CHAR_SKILLMAXHAVE ; i++ ){
        CHAR_HaveSkill* chsk;
        chsk = CHAR_getCharHaveSkill(charaindex,i);
        if( chsk && chsk->use
            && SKILL_CHECKID(chsk->skill.data[SKILL_IDENTITY])
            && chsk->skill.data[SKILL_LEVEL]
            < SKILL_tbl[chsk->skill.data[SKILL_IDENTITY]].maxlevel ){
            char    tmpbuf[512];
            snprintf( tmpbuf,sizeof(tmpbuf),"%d|",
                      chsk->skill.data[SKILL_IDENTITY] );
            strcatsafe( buf, buflen,tmpbuf );
        }
    }
    dchop(buf,"|");
    return TRUE;
}

void SKILL_skillEffect( int charaindex )
{
    int i;
    if( !CHAR_CHECKINDEX(charaindex) )return;

    if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)
        return;
        
    for( i=0 ; i<CHAR_SKILLMAXHAVE ; i ++ ){
        typedef void (*SKILLEFFECTFUNC)(int,Skill*);
        SKILLEFFECTFUNC skfunc;
        CHAR_HaveSkill* cskill;
        Skill*          skill;
        int             id;

        cskill = CHAR_getCharHaveSkill(charaindex,i);
        if( cskill == NULL  || cskill->use == FALSE )continue;

        skill = &cskill->skill;
        id = skill->data[SKILL_IDENTITY];
        if( !SKILL_CHECKID(id) )continue;
        
        skfunc = (SKILLEFFECTFUNC)SKILL_tbl[id].effectfunc;
        if( skfunc )skfunc(charaindex,skill);
    }    
}



#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
// 取使用魔法耗费MP
int PROFESSION_MAGIC_COST_MP( int charaindex, int skill_num )
{
	int skill_level=0, dec_mp=0;
	int	Pskillid, skillid;
	char *skill_name;
	CHAR_HaveSkill* hskill;
		
	// 人物技能
	skillid = CHAR_getCharSkill( charaindex, skill_num);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	if( Pskillid == -1 )	return FALSE;

	// 技能名称
	skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);

	// 技能等级
	hskill = CHAR_getCharHaveSkill( charaindex, skill_num );
	skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );	

	if( (strcmp( skill_name, "PROFESSION_VOLCANO_SPRINGS" )) == 0 ){	// 火山泉
				if( skill_level >= 10 )		dec_mp = 35;
				else if( skill_level >= 7 )	dec_mp = 30;
				else if( skill_level >= 5 )	dec_mp = 20;
				else if( skill_level >= 3 )	dec_mp = 15;
				else						dec_mp = 10;
	}else
	if( (strcmp( skill_name, "PROFESSION_FIRE_BALL" )) == 0 ){			// 火星球
				if( skill_level >= 9 )		dec_mp = 50;
				else if( skill_level >= 7 )	dec_mp = 45;
				else if( skill_level >= 5 )	dec_mp = 40;
				else if( skill_level >= 3 )	dec_mp = 35;
				else						dec_mp = 30;
	}else 
	if( (strcmp( skill_name, "PROFESSION_SUMMON_THUNDER" )) == 0 ){		// 召雷术
				if( skill_level >= 8 )		dec_mp = 30;
				else if( skill_level >= 5 )	dec_mp = 25;
				else if( skill_level >= 3)	dec_mp = 20;	
				else						dec_mp = 10;
	}else
	if( (strcmp( skill_name, "PROFESSION_CURRENT" )) == 0 ){			// 电流术
#ifdef _PROFESSION_ADDSKILL
                if( skill_level >= 10 )		dec_mp = 100;
				else if( skill_level > 9 )	dec_mp = 90;
				else if( skill_level > 8 )	dec_mp = 80;
				else if( skill_level > 7 )	dec_mp = 70;
				else if( skill_level > 6 )	dec_mp = 60;
				else if( skill_level > 4 )	dec_mp = 50;
				else if( skill_level > 2 )	dec_mp = 40;
				else						dec_mp = 30;
#else
				if( skill_level >= 9 )		dec_mp = 80;
				else if( skill_level >= 7 )	dec_mp = 60;
				else if( skill_level >= 5 )	dec_mp = 50;
				else if( skill_level >= 3 )	dec_mp = 40;
				else						dec_mp = 30;
#endif
	}else
	if( (strcmp( skill_name, "PROFESSION_STORM" )) == 0 ){				// 暴风雨
#ifdef _PROFESSION_ADDSKILL
                if( skill_level > 8 )		dec_mp = 50;
				else if( skill_level > 6 )	dec_mp = 45;
				else if( skill_level > 4 )	dec_mp = 40;
				else if( skill_level > 2 )	dec_mp = 35;
				else						dec_mp = 30;
#else
				if( skill_level >= 10 )		dec_mp = 80;
				else if( skill_level >= 6 )	dec_mp = 70;
				else if( skill_level >= 5 )	dec_mp = 60;
				else						dec_mp = 50;
#endif
	}else
	if( (strcmp( skill_name, "PROFESSION_ICE_ARROW" )) == 0 ){			// 冰箭术
				if( skill_level >= 8)		dec_mp = 20;
				else if( skill_level >= 4 )	dec_mp = 15;
				else						dec_mp = 10;
	}else
	if( (strcmp( skill_name, "PROFESSION_ICE_CRACK" )) == 0 ){			// 冰爆术
#ifdef _PROFESSION_ADDSKILL
                if( skill_level >= 10 )		dec_mp = 80;
				else if( skill_level > 8 )  dec_mp = 70;
				else if( skill_level > 6 )  dec_mp = 60;
				else if( skill_level > 4 )  dec_mp = 50;
				else if( skill_level > 2 )  dec_mp = 40;
				else						dec_mp = 30;
#else
		        if( skill_level >= 9 )		dec_mp = 70;
				else if( skill_level >= 7 ) dec_mp = 60;
				else if( skill_level >= 5 ) dec_mp = 50;
				else if( skill_level >= 3 ) dec_mp = 40;
				else						dec_mp = 30;
#endif
	}else
	if( (strcmp( skill_name, "PROFESSION_DOOM" )) == 0 ){				// 世界末日	
				if( skill_level > 8 )		dec_mp = 150;
				else if( skill_level > 4 )  dec_mp = 100;
				else						dec_mp = 50;
	}else
	if( (strcmp( skill_name, "PROFESSION_FIRE_SPEAR" )) == 0 ){			// 火龙枪
				if( skill_level > 8 )		dec_mp = 80;
				else if( skill_level > 6 )	dec_mp = 70;			
				else if( skill_level > 4 )	dec_mp = 60;
				else if( skill_level > 2 )	dec_mp = 40;
				else						dec_mp = 30;
	}else
	if( (strcmp( skill_name, "PROFESSION_BLOOD_WORMS" )) == 0 ){		// 嗜血蛊
				if( skill_level >= 10 )		dec_mp = 15;
				else if( skill_level >= 5 ) dec_mp = 10;
				else						dec_mp = 5;
	}else
	if( (strcmp( skill_name, "PROFESSION_SIGN" )) == 0 ){				// 一针见血

				if( skill_level >= 8 )		dec_mp = 10;
				else						dec_mp = 5;

	}else
	if( (strcmp( skill_name, "PROFESSION_ENCLOSE" )) == 0 ){			// 附身术
				if( skill_level >= 10 )		dec_mp = 80;
				else if( skill_level >= 8 ) dec_mp = 70;
				else if( skill_level >= 5)  dec_mp = 60;
				else						dec_mp = 50;
	}else
	if( (strcmp( skill_name, "PROFESSION_ICE_MIRROR" )) == 0 ){			// 冰镜术
				if( skill_level >= 9 )		dec_mp = 40;
				else if( skill_level >= 7 )	dec_mp = 35;			
				else if( skill_level >= 5 )	dec_mp = 30;
				else if( skill_level >= 3 )	dec_mp = 25;
				else						dec_mp = 20;
	}else
	if( (strcmp( skill_name, "PROFESSION_FIRE_ENCLOSE" )) == 0 ){		// 火附体		
				if( skill_level >= 10 )		dec_mp = 50;
				else if( skill_level >= 7 )	dec_mp = 40;			
				else if( skill_level >= 4 )	dec_mp = 30;
				else						dec_mp = 20;
	}else	
	if( (strcmp( skill_name, "PROFESSION_ICE_ENCLOSE" )) == 0 ){		// 冰附体		
				if( skill_level >= 10 )		dec_mp = 50;
				else if( skill_level >= 7 )	dec_mp = 40;			
				else if( skill_level >= 4 )	dec_mp = 30;
				else						dec_mp = 20;
	}else
	if( (strcmp( skill_name, "PROFESSION_THUNDER_ENCLOSE" )) == 0 ){	// 雷附体
				if( skill_level >= 10 )		dec_mp = 50;
				else if( skill_level >= 7 )	dec_mp = 40;			
				else if( skill_level >= 4 )	dec_mp = 30;
				else						dec_mp = 20;
	}else	
	if( (strcmp( skill_name, "PROFESSION_TRANSPOSE" )) == 0 ){			// 移形换位		
				if( skill_level >= 10 )		dec_mp = 50;
				else if( skill_level >= 9 )	dec_mp = 40;			
				else if( skill_level >= 7 )	dec_mp = 30;
				else if( skill_level >= 4 )	dec_mp = 20;				
				else						dec_mp = 10;
	}else
#ifdef _PROFESSION_ADDSKILL
	if( (strcmp( skill_name, "PROFESSION_RESIST_F_I_T" )) == 0 ){  // 自然威能
	            if( skill_level >= 10 )		dec_mp = 20;
				else if( skill_level >= 9 )	dec_mp = 15;			
				else if( skill_level >= 6 )	dec_mp = 10;		
				else						dec_mp = 5;
	}else
	if( (strcmp( skill_name, "PROFESSION_CALL_NATURE" )) == 0 ){  // 号召自然
		        
		/*skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);
	            if( skill_level >= 100 )	dec_mp = 50;
				else if( skill_level > 95 )	dec_mp = 50;			
				else if( skill_level > 90 )	dec_mp = 50;		
				else if( skill_level > 85 )	dec_mp = 50;
				else if( skill_level > 80 )	dec_mp = 50;
				else if( skill_level > 60 )	dec_mp = 50;
				else if( skill_level > 40 )	dec_mp = 50;
				else if( skill_level > 20 )	dec_mp = 50;
				else dec_mp = 50;*/
		dec_mp = 50;
	}else
	if( (strcmp( skill_name, "PROFESSION_BOUNDARY" )) == 0 ){  // 四属性结界
	            char *pszP=NULL; 
		        if( skill_level > 9 )	dec_mp = 20;
				else if( skill_level > 6 )	dec_mp = 15;			
				else dec_mp = 10;
				//破除结界耗损mp与其他结界不同
				if( (pszP = strstr( PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_OPTION), "破结界" ) ) != NULL ){// 技能的参数
                    if( skill_level >= 9 )	dec_mp = 20;
				    else if( skill_level > 4 )	dec_mp = 15;	
				    else if( skill_level > 2 )  dec_mp = 10;
				    else dec_mp = 5;
				}
	}else
#endif
	{
		return -1;
	}

	return dec_mp;
}


INLINE int PROFESSION_CHANGE_SKILL_LEVEL_M( int skill_level )
{
	if( skill_level > 90 )skill_level = 10;
	else if( skill_level > 80 ) skill_level = 9;
	else if( skill_level > 70 ) skill_level = 8;
	else if( skill_level > 60 ) skill_level = 7;
	else if( skill_level > 50 ) skill_level = 6;
	else if( skill_level > 40 ) skill_level = 5;
	else if( skill_level > 30 ) skill_level = 4;
	else if( skill_level > 20 ) skill_level = 3;
	else if( skill_level > 10 ) skill_level = 2;
	else						skill_level = 1;

	return skill_level;
}


INLINE int PROFESSION_CHANGE_SKILL_LEVEL_A( int skill_level )
{
	if( skill_level >= 100 )	skill_level = 10;
	else if( skill_level > 90 )	skill_level = 9;
	else if( skill_level > 80 ) skill_level = 8;
	else if( skill_level > 70 ) skill_level = 7;
	else if( skill_level > 60 ) skill_level = 6;
	else if( skill_level > 50 ) skill_level = 5;
	else if( skill_level > 40 ) skill_level = 4;
	else if( skill_level > 30 ) skill_level = 3;
	else if( skill_level > 20 ) skill_level = 2;
	else if( skill_level > 10 ) skill_level = 1;
	else						skill_level = 0;

	return skill_level;
}
#endif

