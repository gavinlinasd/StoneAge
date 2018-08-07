#include "version.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "handletime.h"
#include "anim_tbl.h"
#include "npc_door.h"
#include "npcutil.h"
#include "readmap.h"
#include "npc_roomadminnew.h"

#include <string.h>
#include <ctype.h>


static int NPC_Door_isOpen( int meindex );

/*
 * ÓñÊ§Ã«  òØÔÊÔÂÐ×»§¼°NPC±å  Û¢Ø¦èúÐÑ¼°ÀÃñø£Û
 * ÕýÄÌÃó  ¡° Door
 *
 * NPCARGUMENT¼°°À·´£ý
 *
 * og|cg|idname|swcount|closetime_sec|soonflg0/1|passflg0/1/2
 *
 * passflg ·´£ý1·ÖÔÈÐ×ÈÕÌìÔÊ´õ¡õÉý¹´ÎåÆ¥Ø¤ÔÂÀ²  ±åØ¦ÔÂ£Û2·ÖÔÈÐ×ÈÕ
 * å§¸²·òÈÕ¾®Ø¦ÖÐ£Û 0·ÖÔÈÐ×ÈÕÓÉµ©·¥¡õÓñÆ¥Ø¦ÖÐ£Û
 *
 * Ã«Ï¶ÀÃÔÊÔÂ£Û  ÒýÔÂÒýÆ¥¼°ÁÝÃÞ·´·¸°×ÇÉ»ïÐþÆ¥·´1ÁÝÃÞ£Û
 * É¬ÀÃÃ«Ì¤ÈÊÎçÎå±å·´  ì«  Ä¯»¥ºÖÛ¢Ø¦¶ã°íÃ«ÊÖ¹´¾®ÈÕïÛÀ²»¥  Û¢Æ¥Ø¤ÔÂ£Û
 *
 * ·ßÛÐ¼°Òý´õÔ»±åCÃ«ËªññÔÊÔÂèúÐÑ»¥Ø¤ÔÂ¾®ÈÕ£ý1µæinfo,  ì«  Ä¯
 * Ã«  µÊØÆ»¯¾®ÈÕ¹«¼°èúÐÑÃ«·½ÍÍÈÉÛÍ·ÂÄÌÊ§¼þÐþ¼°    ±å  Ô÷½ñÄ¾»¯ÖÐÔÂ
 * ÇÁÚØ»¥  Þ¥±åØ¦ÔÂ£Û
 */

#define NPC_DOOR_OPENMSG "ÓñÊ§»¥£ý·òÈÕÖÐÐ×!"
#define NPC_DOOR_CLOSEMSG "ÓñÊ§Ã«£ýØÆ»§Ð×!"
#define NPC_DOOR_CANTOPENMSG "¾®¿º»¥Ø¤´õØ¦ÖÐ¼°Æ¥£ýÓñÊ§Ã«·òÈÕÈÊ³ðÎç»¥\
Æ¥ÎåØ¦ÖÐ!"

#define ROPEGRAPHIC 9259        /* Ç¾  ±åØ¤ÔÂÉ§Ä¾Ø¦ÖÐ·ò¡õÃó */

enum{
	NPC_DOOR_FLG_SOONCLOSE = 1,
	NPC_DOOR_FLG_WATCHCLOSE = 2
};


/*
 * ÓñÊ§ÈÕØÆÖÐÉ¬ÀÃÃ«ÔÊÔÂ±å·´£ý³ð³ðÆ¥
 * maxhp , maxmp , level , str , tough Ã«0±åÔÊÔÂÎç·½ÖÐ£Û
 *
 */
BOOL NPC_DoorInit( int meindex )
{
	char arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    char settingdir[1024];
    char buf[256];

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE,CHAR_EVENT_NPC);

    getStringFromIndexWithDelim(arg, "|" , 1 , buf,sizeof(buf) );
    CHAR_setWorkInt( meindex , CHAR_WORKDOOROPENG , atoi( buf ));
    getStringFromIndexWithDelim(arg, "|" , 2 , buf,sizeof(buf) );
    CHAR_setWorkInt( meindex , CHAR_WORKDOORCLOSEG , atoi( buf ));
    getStringFromIndexWithDelim(arg, "|" , 3 , buf,sizeof(buf) );
    CHAR_setWorkChar( meindex , CHAR_WORKDOORNAME , buf );
    strcpysafe( settingdir,sizeof( settingdir) , buf );
    getStringFromIndexWithDelim(arg, "|" , 4 , buf,sizeof(buf) );
    CHAR_setWorkInt( meindex , CHAR_WORKDOORSWITCHCOUNT , atoi( buf ));
    getStringFromIndexWithDelim(arg, "|" , 5 , buf,sizeof(buf) );
    CHAR_setWorkInt( meindex , CHAR_WORKDOORCLOSETIME , atoi( buf ));

    CHAR_setInt( meindex , CHAR_MERCHANTLEVEL , atoi( buf ));
    getStringFromIndexWithDelim(arg, "|" , 6 , buf,sizeof(buf) );
    CHAR_setWorkInt( meindex , CHAR_WORKDOORSOONFLG , atoi( buf ));

    getStringFromIndexWithDelim(arg, "|" , 7 , buf,sizeof(buf) );
    CHAR_setWorkInt( meindex , CHAR_WORKDOORPASSFLG , atoi( buf ));
    CHAR_setWorkInt( meindex , CHAR_WORKDOORORIGINALGRAPHIC,
                     CHAR_getInt( meindex,CHAR_BASEIMAGENUMBER ));
	CHAR_setInt( meindex, CHAR_LV, 0 );
    if( getStringFromIndexWithDelim( arg , "|" , 8 , buf ,sizeof( buf ))){
        if( strncmp( buf , "title" ,5 ) == 0 ){
            print( "This is a title door:'%s'\n" , buf );
             CHAR_setWorkInt( meindex , CHAR_WORKDOORPASSFLG ,0 );
            CHAR_setWorkChar( meindex , CHAR_WORKDOORTITLE , buf );
            CHAR_setInt( meindex, CHAR_LV, 1 );
        } else if( isdigit( buf[0] )){
            if( getStringFromIndexWithDelim(arg, "|" , 8 , buf,sizeof(buf) ))
                CHAR_setWorkChar( meindex, CHAR_WORKDOORGOLDLIMIT, buf);
            if( getStringFromIndexWithDelim(arg, "|" , 10 , buf,sizeof(buf) ))
                CHAR_setWorkChar( meindex, CHAR_WORKDOORWEEK, buf);
            if( getStringFromIndexWithDelim(arg, "|" , 11 , buf,sizeof(buf) ))
                CHAR_setWorkChar( meindex, CHAR_WORKDOORHOUR, buf);
            if( getStringFromIndexWithDelim(arg, "|" , 12 , buf,sizeof(buf) ))
                CHAR_setWorkChar( meindex, CHAR_WORKDOORMINUTE, buf);
        }
    }

    CHAR_setInt( meindex , CHAR_HP , 0 );
    CHAR_setInt( meindex , CHAR_MP , 0 );
    CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    CHAR_setInt( meindex , CHAR_STR , 0 );
    CHAR_setInt( meindex , CHAR_TOUGH, 0 );
     {
        int g = CHAR_getWorkInt( meindex , CHAR_WORKDOOROPENG );
        if( g >= 11900 && g <= 11915 ){
            CHAR_setWorkInt( meindex , CHAR_WORKDOORKEYITEMID , 15 );
        } else if( g >= 11916 && g <= 11931 ){

            CHAR_setWorkInt( meindex , CHAR_WORKDOORKEYITEMID , 16 );
        } else {
            CHAR_setWorkInt( meindex , CHAR_WORKDOORKEYITEMID , -1 );
        }
        if(CHAR_getWorkInt( meindex , CHAR_WORKDOORPASSFLG )==2){
            CHAR_setWorkInt( meindex , CHAR_WORKDOORKEYITEMID , -2 );
        }
        if( CHAR_getInt( meindex , CHAR_BASEIMAGENUMBER ) ==
            ROPEGRAPHIC ){
            CHAR_setWorkInt( meindex , CHAR_WORKDOORKEYITEMID,-2 );
        }
    }

    {
        int closeg = CHAR_getWorkInt( meindex , CHAR_WORKDOORCLOSEG );
        int dir = -1;
        switch( closeg ){
        case 11900: dir = 6; break;  /* ö«¼°ÓñÊ§ ·ò·ÖÔ»ØÆÐ× */
        case 11902: dir = 0; break;  /*          ·ò·ÖÔ»µ¤Òü */
        case 11904: dir = 2; break;  /*          ÐÄ¿ºµ¤Òü */
        case 11906: dir = 4; break;  /*          ÐÄ¿ºØÆÐ× */

        case 11908: dir = 6; break;  /* Ó®ÖÐö«¼°ÓñÊ§ ·ò·ÖÔ»ØÆÐ× */
        case 11910: dir = 0; break;  /*              ·ò·ÖÔ»µ¤Òü */
        case 11912: dir = 2; break;  /*              ÐÄ¿ºµ¤Òü */
        case 11914: dir = 4; break;  /*              ÐÄ¿ºØÆÐ× */

        case 11916: dir = 6; break;  /* ÎË¼°ÓñÊ§ ·ò·ÖÔ»ØÆÐ× */
        case 11918: dir = 0; break;  /*          ·ò·ÖÔ»µ¤Òü */
        case 11920: dir = 2; break;  /*          ÐÄ¿ºµ¤Òü */
        case 11922: dir = 4; break;  /*          ÐÄ¿ºØÆÐ× */

        case 11924: dir = 6; break;  /* Ó®ÖÐÎË¼°ÓñÊ§ ·ò·ÖÔ»ØÆÐ× */
        case 11926: dir = 0; break;  /*              ·ò·ÖÔ»µ¤Òü */
        case 11928: dir = 2; break;  /*              ÐÄ¿ºµ¤Òü */
        case 11930: dir = 4; break;  /*              ÐÄ¿ºØÆÐ× */

        case 11958: dir = 2; break;  /* Ó®ÖÐ  ¼°ÓñÊ§ ÐÄ¿ºµ¤Òü */
        case 11960: dir = 4; break;  /*              ÐÄ¿ºØÆÐ× */
        case 11962: dir = 6; break;  /*              ·ò·ÖÔ»ØÆÐ× */
        case 11964: dir = 0; break;  /*              ·ò·ÖÔ»µ¤Òü */

        case 11966: dir = 2; break;  /*   ¼°ÓñÊ§ ÐÄ¿ºµ¤Òü */
        case 11968: dir = 4; break;  /*          ÐÄ¿ºØÆÐ× */
        case 11970: dir = 6; break;  /*          ·ò·ÖÔ»ØÆÐ× */
        case 11972: dir = 0; break;  /*          ·ò·ÖÔ»µ¤Òü */

        case 11978: dir = 2; break;  /* Ó®ÖÐàÅ¼°ÓñÊ§ ÐÄ¿ºµ¤Òü */
        case 11980: dir = 4; break;  /*              ÐÄ¿ºØÆÐ× */
        case 11982: dir = 6; break;  /*              ·ò·ÖÔ»ØÆÐ× */
        case 11984: dir = 0; break;  /*              ·ò·ÖÔ»µ¤Òü */

        case 11986: dir = 2; break;  /* àÅ¼°ÓñÊ§ ÐÄ¿ºµ¤Òü */
        case 11988: dir = 4; break;  /*          ÐÄ¿ºØÆÐ× */
        case 11990: dir = 6; break;  /*          ·ò·ÖÔ»ØÆÐ× */
        case 11992: dir = 0; break;  /*          ·ò·ÖÔ»µ¤Òü */

        default: break;
        }
        if( dir >= 0 )CHAR_setInt(meindex,CHAR_DIR,dir);
    }

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEDOOR );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    CHAR_setInt( meindex , CHAR_WORKDOORSWITCHCURRENT , 0 );
	CHAR_setWorkInt( meindex, CHAR_WORKDOOREXPIRETIME, 0xffffffff);
    if( CHAR_getWorkInt( meindex , CHAR_WORKDOORPASSFLG ) == 1){
		NPC_ROOMINFO roominfo;
		if( NPC_RoomAdminNew_ReadFile( CHAR_getWorkChar( meindex, CHAR_WORKDOORNAME), &roominfo) == TRUE ){
			NPC_DoorSetPasswd( meindex, roominfo.passwd);
			CHAR_setWorkInt( meindex, CHAR_WORKDOOREXPIRETIME, roominfo.expire);
		}else {
	        NPC_DoorSetPasswd( meindex, "8hda8iauia90494jasd9asodfasdf89" );
		    if( getStringFromIndexWithDelim(arg, "|" , 13 , buf,sizeof(buf) ))
		    	if( strlen( buf) != 0 )
			        NPC_DoorSetPasswd( meindex, buf );
    	}
    }

    CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER , CHAR_getWorkInt(meindex, CHAR_WORKDOORCLOSEG));
    NPC_DoorRefreshOverFlg( meindex );
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_NPC);
    return TRUE;
}
void NPC_DoorAddSwitchCounter( int meindex , int i , int doindex )
{
    if( i != 0  && CHAR_getWorkInt( meindex, CHAR_WORKDOORSWITCHCOUNT)
        != 0 ){
        CHAR_setWorkInt( meindex, CHAR_WORKDOORSWITCHCURRENT ,
                         CHAR_getWorkInt(meindex,
                                         CHAR_WORKDOORSWITCHCURRENT)
                         + i );
        if( CHAR_getWorkInt( meindex, CHAR_WORKDOORSWITCHCURRENT ) ==
            CHAR_getWorkInt( meindex, CHAR_WORKDOORSWITCHCOUNT) ){
            NPC_DoorOpen( meindex , doindex );
        } else {
            NPC_DoorClose( meindex , doindex );
        }
    }
}

void NPC_DoorSetSwitchCounter( int meindex, int i , int doindex )
{
    if( !CHAR_CHECKINDEX(meindex))return;
    CHAR_setWorkInt( meindex , CHAR_WORKDOORSWITCHCURRENT , i );
    NPC_DoorAddSwitchCounter( meindex, 0 , doindex );
}

static int NPC_DoorSamePos( int meindex ){
	int iWork;
	OBJECT object;
    int floor = CHAR_getInt( meindex, CHAR_FLOOR );
    int x  = CHAR_getInt( meindex, CHAR_X );
    int y  = CHAR_getInt( meindex, CHAR_Y );

	for( object=MAP_getTopObj( floor, x,y ); object ; object = NEXT_OBJECT(object) ){
		int objindex = GET_OBJINDEX(object);
   		if( OBJECT_getType(objindex) != OBJTYPE_CHARA )continue;
   		iWork = OBJECT_getIndex(objindex);
   		if( CHAR_getInt(iWork,CHAR_HP) < 0 )continue;
		if( CHAR_getInt( iWork,CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
			return iWork;
		}
	}
	return -1;
}

void NPC_DoorWatch( int myobjindex, int moveobjindex, CHAR_ACTION act,
                    int x, int y, int dir, int* opt, int optlen )
{
    unsigned long closetime;
	int meindex, moveindex;

	if( OBJECT_getType( moveobjindex) != OBJTYPE_CHARA ) {
		return;
	}
	meindex = OBJECT_getIndex(myobjindex);
	moveindex = OBJECT_getIndex(moveobjindex);
	if( NPC_Door_isOpen( meindex ) == 0 )return;
    if( CHAR_getInt( moveindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
        return;
    }
    if( CHAR_getWorkInt( meindex , CHAR_WORKDOORSOONFLG )){
		if( CHAR_getInt( meindex , CHAR_TALKCOUNT ) > 0 ){
			if( NPC_DoorSamePos( meindex ) == -1 ){
	    	    NPC_DoorClose(meindex , -1 );
				CHAR_setInt( meindex , CHAR_TALKCOUNT, 0 );
			}
		}
	}else
		if( CHAR_getInt( meindex, CHAR_MERCHANTLEVEL ) >= 0 ){
    		closetime = (unsigned long )CHAR_getWorkInt( meindex , CHAR_WORKDOORCLOSETIME );
    		if( (unsigned long) NowTime.tv_sec > (unsigned long) closetime ){
				NPC_DoorClose(meindex , -1 );
			}
		}
}


#if 1
void NPC_DoorOff( int meindex , int movedindex )
{
    if( CHAR_getWorkInt( meindex , CHAR_WORKDOORSOONFLG )){
        NPC_DoorClose(meindex , -1 );
    } else {
    }
}
#endif

void NPC_DoorPostOver( int meindex , int movedindex )
{
    if( CHAR_getWorkInt( meindex , CHAR_WORKDOORSOONFLG )){
		CHAR_setInt( meindex , CHAR_TALKCOUNT, 1 );
        CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );	/* É§Ä¾Ø¦ÈÊÔÊÔÂ */
    }
}

void NPC_DoorLooked( int meindex , int lookedindex )
{
    int needkey;

    if( CHAR_getInt( meindex , CHAR_LV )){
        char token[32];
        char *p = CHAR_getWorkChar( meindex , CHAR_WORKDOORTITLE );
        int i,j,ok=FALSE;
        for(i = 0 ; ; i++ ){
            int r;
            r = getStringFromIndexWithDelim( p,":",2+i,token ,sizeof(token));
            if(r){
                int titleind = atoi( token );
                for(j=0;j<CHAR_TITLEMAXHAVE;j++){
                    if(CHAR_getCharHaveTitle( lookedindex , j ) == titleind ){
                        ok = TRUE;
                        break;
                    }
                    if( ok )break;
                }
            } else {
                break;
            }
            if( ok ) break;
        }
        if( ok ){
            NPC_DoorFlip( meindex , lookedindex );
        } else {
            char arg[512];
            char buf[512];
            NPC_Util_GetArgStr( meindex , arg,sizeof(arg));
            getStringFromIndexWithDelim( arg,"|" , 9 , buf,sizeof(buf));
            CHAR_talkToCli( lookedindex,-1,buf,CHAR_COLORWHITE);
        }
        return;
    }

    if( CHAR_getWorkInt( meindex, CHAR_WORKDOORORIGINALGRAPHIC)==
        ROPEGRAPHIC ){
        return;
    }
    if( CHAR_getInt( meindex , CHAR_DIR ) == -1 ){
        NPC_DoorFlip( meindex , lookedindex );
        return;
    }

    if( CHAR_getWorkInt( meindex , CHAR_WORKDOORSOONFLG ) == 0 &&
		NPC_Util_isBackContact( meindex , lookedindex ) == TRUE ){
        NPC_DoorFlip( meindex , lookedindex);
        return;
    }

    if( CHAR_getWorkInt( meindex, CHAR_WORKDOORPASSFLG)>=1 ){
        if( NPC_Door_isOpen( meindex ) ){
            NPC_DoorFlip( meindex , lookedindex );
            return;
        }
#define		NPC_DOOR_EXPIRE_MSG		"%s µÄ·¿¼äÓÐÐ§ÆÚÏÞÖÁ %d/%d %d:%d"
        else {
			if( CHAR_getWorkInt( meindex, CHAR_WORKDOOREXPIRETIME)
				!= 0xffffffff ){
				struct tm tmwk;
				time_t	expire = CHAR_getWorkInt( meindex, CHAR_WORKDOOREXPIRETIME);
				char	msgbuf[128];

				NPC_ROOMINFO roominfo;
				/* ÓÉµ©Îç  ¶ãÓåò×Ã«±¾ÓÀÐþÔÊÔÂ */
				NPC_RoomAdminNew_ReadFile(
								CHAR_getWorkChar( meindex, CHAR_WORKDOORNAME),
										&roominfo);
				memcpy( &tmwk, localtime( &expire), sizeof( tmwk));
				snprintf( msgbuf, sizeof( msgbuf), NPC_DOOR_EXPIRE_MSG,
						  roominfo.charaname,
						  tmwk.tm_mon+1, tmwk.tm_mday,
						  tmwk.tm_hour, tmwk.tm_min);
				CHAR_talkToCli( lookedindex, meindex, msgbuf, CHAR_COLORWHITE);
        	}
        }
        #undef NPC_DOOR_EXPIRE_MSG
        return;
    }
    needkey = CHAR_getWorkInt( meindex , CHAR_WORKDOORKEYITEMID );
    if( needkey >= 0 ){
        if( NPC_Util_countHaveItem( lookedindex , needkey ) > 0 ){
            NPC_DoorFlip( meindex , lookedindex);
        } else {
            CHAR_talkToCli( lookedindex , -1 , NPC_DOOR_CANTOPENMSG , CHAR_COLORWHITE );
        }
    } else if( needkey == -1 ){
        NPC_DoorFlip(meindex , lookedindex );
    } else {
        CHAR_talkToCli( lookedindex , -1 , "ÕâÉÈÃÅËÆºõÔõ÷áÒ²´ò²»¿ª£¡" , CHAR_COLORWHITE);
    }
}
void NPC_DoorTalked( int meindex , int talkerindex , char *msg , int color )
{
    char *m = CHAR_getWorkChar( meindex , CHAR_WORKDOORPASSWD );
	if(NPC_Util_CharDistance( talkerindex, meindex ) > 1)return;
    if( CHAR_getWorkInt( meindex , CHAR_WORKDOORPASSFLG ) != 1 ){
        return;
    }
    if( CHAR_getWorkInt( meindex , CHAR_WORKDOORPASSFLG ) >= 1 &&
        ((unsigned long)NowTime.tv_sec) > (unsigned long)
        CHAR_getWorkInt( meindex , CHAR_WORKDOOREXPIRETIME )){
        print( "RINGO: passdoor expired ( talked )\n" );
        return;
    }
    if( m[0] == '\0' )return;
    cutDotsTail( msg );
    if( strcmp( m , msg ) == 0 ){
        NPC_DoorFlip( meindex , talkerindex );
    }else {
        CHAR_talkToCli( talkerindex , -1 , "ÃÜÂë²»ÕýÈ·À²£¡", CHAR_COLORWHITE );
    }
}

int NPC_DoorSearchByName( char *name )
{
    int i;
    int charnum = CHAR_getCharNum();

    for(i= CHAR_getPlayerMaxNum()  + CHAR_getPetMaxNum();
    	i<charnum;
    	i++){

        if( CHAR_getCharUse(i) &&
            CHAR_getInt( i ,CHAR_WHICHTYPE) == CHAR_TYPEDOOR ){
            if( strcmp( CHAR_getWorkChar(i,CHAR_WORKDOORNAME),name)==0 ){
                return i;
            }
        }
    }
    return -1;
}

int NPC_DoorSearchByPosition( int fl , int x, int y )
{
    int i;
    int charnum = CHAR_getCharNum();

    for(i=CHAR_getPlayerMaxNum() + CHAR_getPetMaxNum();
    	i<charnum;
    	i++)
    {
        if( CHAR_getCharUse(i) &&
            CHAR_getInt( i , CHAR_WHICHTYPE) == CHAR_TYPEDOOR &&
            CHAR_getInt( i , CHAR_FLOOR ) == fl &&
            CHAR_getInt( i , CHAR_X ) == x &&
            CHAR_getInt( i , CHAR_Y ) == y          ){
            return i;
        }
    }
    return -1;
}

int NPC_DoorSetPasswd( int meindex, char *pas )
{
    if( CHAR_setWorkChar( meindex, CHAR_WORKDOORPASSWD , pas ) == FALSE){
        return -1;
    } else {
        return 0;
    }
}

int NPC_DoorGetPasswd( int meindex, char *out , int len )
{
    char *m = CHAR_getWorkChar( meindex , CHAR_WORKDOORPASSWD );
    if( m == NULL )return -1;
    strcpysafe( out ,len , m );
    return 0;
}

void NPC_DoorFlip(int meindex , int doindex)
{
    if( NPC_Door_isOpen( meindex ) == 1 ){
        NPC_DoorClose( meindex , doindex);
    } else {
        NPC_DoorOpen( meindex , doindex);
    }
}
void NPC_DoorClose(int meindex , int doindex)
{

    CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER ,
                 CHAR_getWorkInt( meindex,CHAR_WORKDOORCLOSEG ));
    NPC_DoorRefreshOverFlg( meindex );
    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex , CHAR_WORKOBJINDEX ));
    if( doindex >= 0 ) CHAR_talkToCli( doindex , -1 , NPC_DOOR_CLOSEMSG, CHAR_COLORWHITE );
	CHAR_setInt( meindex , CHAR_TALKCOUNT, 0 );
}
void NPC_DoorOpen(int meindex , int doindex )
{
    CHAR_setWorkInt( meindex , CHAR_WORKDOORCLOSETIME,
                     NowTime.tv_sec +
                     CHAR_getInt( meindex , CHAR_MERCHANTLEVEL));
    CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER ,
                 CHAR_getWorkInt( meindex,CHAR_WORKDOOROPENG ));
    NPC_DoorRefreshOverFlg( meindex );
    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex,
                                                   CHAR_WORKOBJINDEX ));
    if( doindex >= 0 )
        CHAR_talkToCli( doindex , -1, NPC_DOOR_OPENMSG ,CHAR_COLORWHITE);
}

static int NPC_Door_isOpen( int meindex )
{
    int now_g = CHAR_getInt( meindex , CHAR_BASEIMAGENUMBER );
    int open_g = CHAR_getWorkInt( meindex ,CHAR_WORKDOOROPENG );
    if( now_g == open_g ) return 1; else return 0;
}

BOOL NPC_DoorRefreshOverFlg( int meind )
{
    if( !CHAR_CHECKINDEX(meind))return FALSE;
    if( NPC_Door_isOpen( meind ) ==1 ){
        CHAR_setFlg( meind , CHAR_ISOVERED , 1 );
    } else {
        CHAR_setFlg( meind , CHAR_ISOVERED , 0 );
    }
    return TRUE;
}

BOOL NPC_DoorSetRopeFlag(int fl,int x,int y, int flag )
{
    int ind = NPC_DoorSearchByPosition( fl,x,y );
    if( ind < 0 ) return FALSE;
    if( flag ){
        CHAR_setInt( ind , CHAR_BASEIMAGENUMBER , ROPEGRAPHIC );
    } else {
        CHAR_setInt(ind , CHAR_BASEIMAGENUMBER , 0 );
    }
    NPC_DoorRefreshOverFlg( ind );
    CHAR_sendCToArroundCharacter(
        CHAR_getWorkInt( ind , CHAR_WORKOBJINDEX ));

    return TRUE;
}

