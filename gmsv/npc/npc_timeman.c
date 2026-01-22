#include "version.h"
#include <string.h>
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "npcutil.h"
#include "npc_oldman.h"
#include "title.h"
#include "char_data.h"
#include "readmap.h"
#include "lssproto_serv.h"
#include "log.h"
#include "handletime.h"





enum {
	E_INT_GRAPHIC = CHAR_NPCWORKINT1,		/*   褥蟆及  飓 */
	E_INT_GRAPHIC2 = CHAR_NPCWORKINT2,		/*   褥  及  飓 */
	E_INT_BORN = CHAR_NPCWORKINT3,			/* 藕戏凛棉 */
	E_INT_DEAD = CHAR_NPCWORKINT4,			/* 壅尹月凛棉*/ 
	E_INT_MODE = CHAR_NPCWORKINT5,			/* 蜇箕及乒□玉 */
	E_INT_NOWGRAPHIC = CHAR_NPCWORKINT6,	/* 蜇箕及弘仿白奴永弁瓜件田□ */

};


typedef struct {
	char	arg[32];
	int		born;
	int 	dead;
}NPC_TimeMan;


#define YOAKE 700
#define NICHIBOTU 300
#define SHOUGO 125
#define YONAKA 500

static NPC_TimeMan	TimeTble[] = {

	{"ALLNIGHT",	NICHIBOTU+1,	YOAKE}, 	/* 脐  互请化中卅中凛 */
	{"ALLNOON",		YOAKE+1,		NICHIBOTU},/* 脐  互请化中月凛 */
	{"AM",			YONAKA+1,		SHOUGO}, 	/* AM 凛及心 */
	{"PM",			SHOUGO+1,		YONAKA}, 	/* PM 凛及心 */
	{"FORE",		YOAKE+1,		SHOUGO},	/* 脐  互匹化井日恳诟引匹 */
	{"AFTER",		SHOUGO+1,		NICHIBOTU},/* 恳诟井日脐  互释戈引匹 */
	{"EVNING",		NICHIBOTU+1,	YONAKA}, 	/* 脐  互释氏匹井日蕞    引匹 */
	{"MORNING",		YONAKA+1,		YOAKE}, 	/* 蕞    井日脐  互哝月引匹 */
	{"FREE",		0,				1024},
};




BOOL NPC_TimeManInit( int meindex )
{

	int i=0;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[32];
	
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}

	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "change_no", buf, sizeof( buf)) != NULL ){
		if(strstr(buf,"CLS")!=NULL){
			CHAR_setWorkInt( meindex,E_INT_GRAPHIC2, 9999 );
		}else{
			CHAR_setWorkInt( meindex,E_INT_GRAPHIC2, atoi(buf) );
		}
	}else{
		CHAR_setWorkInt( meindex,E_INT_GRAPHIC2, 9999 );
	}

	CHAR_setWorkInt( meindex, E_INT_GRAPHIC, CHAR_getInt( meindex, CHAR_BASEIMAGENUMBER) );

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "time", buf, sizeof( buf)) != NULL ){
		for(i=0 ; i<9 ; i++){
			if(strstr(buf,TimeTble[i].arg) != NULL){
				CHAR_setWorkInt( meindex, E_INT_BORN, TimeTble[i].born);
				CHAR_setWorkInt( meindex, E_INT_DEAD, TimeTble[i].dead);
				return TRUE;
			}
		}
	}
	
	return FALSE;

}


void NPC_TimeManTalked( int meindex , int talkerindex , char *msg ,int color )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[512];
	char buf[512];
	int tokennum=0;
	int i;


	/*--  及蟆卞中月井升丹井＂--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex ,2)==FALSE){
		/* ㄠ弘伉永玉动  及心 */
		if( NPC_Util_CharDistance( talkerindex ,meindex ) > 1) return;
	}

	if(CHAR_getInt(meindex,CHAR_BASEIMAGENUMBER)==9999){
		/*--壅尹化中月乒□玉--*/
		return;
	}

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");	
		return;
	}
	

	if( CHAR_getInt(talkerindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER )return;
	
	if( NPC_Util_charIsInFrontOfChar( talkerindex, meindex, 3 ) ==FALSE) return;




	if(CHAR_getWorkInt(meindex,E_INT_MODE)==0){
		/*--丢永本□斥毛忒允 */
		NPC_Util_GetStrFromStrWithDelim(argstr,"main_msg",buf,sizeof( buf) );
 	
 	}else{
 		/*弘仿白奴永弁    毛  尹凶分仃卅及匹｝啜丹丢永本□斥毛忒允 */
		NPC_Util_GetStrFromStrWithDelim(argstr,"change_msg",buf,sizeof( buf) );
 	}

   	tokennum = 1;
   	/* 戊件穴匹嗉濠日木凶玄□弁件互窒仇丐月井醒尹月 */
	 for( i=0;buf[i]!='\0';i++ ){
          if( buf[i] == ',' ) tokennum++;
     }
 
    /* 仿件母丞匹升木毛韵月井瑁户化｝公及玄□弁件毛潸曰分允 */
    getStringFromIndexWithDelim( buf,",", rand()%tokennum+1,token, sizeof(token));
	/*--仄扎屯月--*/
	CHAR_talkToCli( talkerindex, meindex, token, CHAR_COLORWHITE );


}

/** 引歹曰卞窒井互粟仇匀凶午五卞裟壬木月*/
void NPC_TimeManWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen )
{
	int meindex=0;
	int index;
	LSTIME	nowlstime;
	int born;
	int dead;
	
	if( OBJECT_getType( objindex) != OBJTYPE_CHARA) return;
	index = OBJECT_getIndex( objindex);

	/* 皿伊奶乩□卞及心  杀允月 */
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;

    meindex = OBJECT_getIndex( meobjindex);
	born = CHAR_getWorkInt( meindex , E_INT_BORN );
	dead = CHAR_getWorkInt( meindex , E_INT_DEAD );

//	print("now_hour=%d",nowlstime.hour);
	
	/* 凛棉毛苇化橇谪毛瑁户月 */
	if(born < dead){
		RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
		if( (born < nowlstime.hour) && (dead > nowlstime.hour) ){
			if(CHAR_getWorkInt(meindex,E_INT_NOWGRAPHIC)
				==CHAR_getWorkInt( meindex, E_INT_GRAPHIC)) return;

			CHAR_setInt(meindex,CHAR_BASEIMAGENUMBER,
							CHAR_getWorkInt( meindex, E_INT_GRAPHIC));
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
			CHAR_setWorkInt(meindex,E_INT_MODE,0);		
			CHAR_setWorkInt(meindex,E_INT_NOWGRAPHIC,
								CHAR_getInt(meindex,CHAR_BASEIMAGENUMBER));
		}else{
			/*-壅尹化中月--*/
			if(CHAR_getWorkInt(meindex,E_INT_NOWGRAPHIC)
				==CHAR_getWorkInt( meindex, E_INT_GRAPHIC2)) return;

			CHAR_setInt(meindex,CHAR_BASEIMAGENUMBER,
								CHAR_getWorkInt( meindex, E_INT_GRAPHIC2));
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
			CHAR_setWorkInt(meindex,E_INT_MODE,1);
			CHAR_setWorkInt(meindex,E_INT_NOWGRAPHIC,
								CHAR_getInt(meindex,CHAR_BASEIMAGENUMBER));		

		}
	}else{
		RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
		if( (born < nowlstime.hour && 1024 > nowlstime.hour) 
			|| ( 0 < nowlstime.hour && dead > nowlstime.hour) 
		){
			if(CHAR_getWorkInt(meindex,E_INT_NOWGRAPHIC)
				==CHAR_getWorkInt( meindex, E_INT_GRAPHIC)) return;
	
			CHAR_setInt(meindex,CHAR_BASEIMAGENUMBER,
							CHAR_getWorkInt( meindex, E_INT_GRAPHIC));
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
			CHAR_setWorkInt(meindex,E_INT_MODE,0);
			CHAR_setWorkInt(meindex,E_INT_NOWGRAPHIC,
							CHAR_getInt(meindex,CHAR_BASEIMAGENUMBER));		
		}else{
			if(CHAR_getWorkInt(meindex,E_INT_NOWGRAPHIC)
				==CHAR_getWorkInt( meindex, E_INT_GRAPHIC2)) return;
			
			CHAR_setInt(meindex,CHAR_BASEIMAGENUMBER,
							CHAR_getWorkInt( meindex, E_INT_GRAPHIC2));
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
			CHAR_setWorkInt(meindex,E_INT_MODE,1);
			CHAR_setWorkInt(meindex,E_INT_NOWGRAPHIC,
							CHAR_getInt(meindex,CHAR_BASEIMAGENUMBER));		
		}
	}

}

