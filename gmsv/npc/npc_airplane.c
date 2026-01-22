#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_airplane.h"
#include "handletime.h"

/* 
 * 樓藝瑤諾 (Made from Bus)
 */
 
enum {
	NPC_WORK_ROUTETOX = CHAR_NPCWORKINT1,		/* 汔喫喜ㄑ  淢   */
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,		/* 汔喫喜ㄑ  淢   */
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,		/* ぽ笰    凝 */
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,		/* 菜拻凝諸堇凝  兔※菜拻 兒※諸堇  */
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
	NPC_WORK_ONEWAYFLG = CHAR_NPCWORKINT11,
	NPC_WORK_RUNWAVE = CHAR_NPCWORKINT13,
};

/* 藸  隍蚗掛↓喇摯enum */
enum {
	NPC_AIR_MSG_GETTINGON,
	NPC_AIR_MSG_NOTPARTY,
	NPC_AIR_MSG_OVERPARTY,
	NPC_AIR_MSG_DENIEDITEM,
	NPC_AIR_MSG_ALLOWITEM,
	NPC_AIR_MSG_LEVEL,
	NPC_AIR_MSG_GOLD,
	NPC_AIR_MSG_EVENT,
	NPC_AIR_MSG_START,
	NPC_AIR_MSG_END,
#ifdef _NPC_AIRDELITEM
	NPC_AIR_MSG_DELITEM,
#endif
#ifdef _NPC_AIRLEVEL
	NPC_AIR_MSG_MAXLEVEL,
#endif
};
typedef struct {
	char	option[32];
	char	defaultmsg[128];
}NPC_AIR_MSG;
NPC_AIR_MSG		airmsg[] = {
	{ "msg_gettingon",	"PAONㄐㄗ斕拸楊黺笢芴樓�輷疰ч﹝﹝�"},
	{ "msg_notparty",	"PAPAONㄐㄐ拸楊眕芶勦樓�錟﹝�"},
	{ "msg_overparty",	"PAONㄐㄐ�侕�眒雛﹝"},
	{ "msg_denieditem",		"PAPAONㄐㄐ扂褫祥猁涴跺耋撿ㄐ"},
	{ "msg_allowitem",		"阺鉊~(砑猁饒跺耋撿陛!)"},
	{ "msg_level",		"PAPAONㄐㄐ斕腔脹撰遜祥劂遄ㄐ"},
	{ "msg_stone",		"PAPAONㄐㄐ踢ヴ祥逋遄ㄐ"},
	{ "msg_event",		"PAONㄐㄐ斕拸楊樓�錟﹝�"},
	{ "msg_start",		"阺鉊~(堤楷輛俴)"},
	{ "msg_end",		"阺鉊~(善蹕)"}
#ifdef _NPC_AIRDELITEM
	,{ "msg_delitem",  "斕羶衄減傚腔耋撿"}
#endif
#ifdef _NPC_AIRLEVEL
    ,{ "msg_maxlevel",  "斕腔脹撰徹詢韃"}
#endif
};

static int NPC_AirSetPoint( int meindex, char *argstr);
static void NPC_AirSetDestPoint( int meindex, char *argstr);
static BOOL NPC_AirCheckDeniedItem( int meindex, int charaindex, char *argstr);
static BOOL NPC_AirCheckLevel( int meindex, int charaindex, char *argstr);
static int NPC_AirCheckStone( int meindex, int charaindex, char *argstr);
static void NPC_AirSendMsg( int meindex, int talkerindex, int tablenum);
static int NPC_AirGetRoutePointNum( int meindex, char *argstr );
static void NPC_Air_walk( int meindex);
#ifdef _NPC_AIRLEVEL
static BOOL NPC_AirCheckMaxLevel( int meindex, int charaindex, char *argstr);
#endif

#define		NPC_AIR_LOOPTIME		100
#define		NPC_AIR_WAITTIME_DEFAULT	180
#define		NPC_AIR_WAITINGMODE_WAITTIME	5000

/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_AirInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int	i;
	char	buf[256],buf1[256];
	int	routenum;
	int	waittime;
	int	seflg;
	int	onewayflg;
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	/* 埵崹躂�屼倠縋聿陔羽捊冕鯆慪濛� */
	routenum = NPC_Util_GetNumFromStrWithDelim( argstr, "routenum");
	if( routenum == -1 ) {
		print( "npcair:nothing routenum \n");
		return FALSE;
	}
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEMAX, routenum);
	
	for( i = 1; i <= routenum; i ++ ) {
		char routetostring[64];
		snprintf( routetostring, sizeof( routetostring), "routeto%d", i);
		if( NPC_Util_GetStrFromStrWithDelim( argstr, routetostring,buf, sizeof(buf))
			== NULL ) 
		{
			print( "npcair:nothing route to \n");
			return FALSE;
		}
	}
	//ANDY_ADD	NPC_WORK_RUNWAVE
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "WAVE",buf1, sizeof(buf1)) == NULL )	{
		CHAR_setWorkInt( meindex, NPC_WORK_RUNWAVE, 77);
	}else	{
		CHAR_setWorkInt( meindex, NPC_WORK_RUNWAVE, atoi( buf1) );
	}

	waittime = NPC_Util_GetNumFromStrWithDelim( argstr, "waittime");
	if( waittime == -1 ) waittime = NPC_AIR_WAITTIME_DEFAULT;
	CHAR_setWorkInt( meindex, NPC_WORK_WAITTIME, waittime);

	seflg = NPC_Util_GetNumFromStrWithDelim( argstr, "seflg");
	if( seflg == -1 ) seflg = TRUE;
	CHAR_setWorkInt( meindex, NPC_WORK_SEFLG, seflg);
	
	onewayflg = NPC_Util_GetNumFromStrWithDelim( argstr, "oneway");
	if( onewayflg == -1 ) onewayflg = FALSE;	// default
	CHAR_setWorkInt( meindex, NPC_WORK_ONEWAYFLG, onewayflg);
	
	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEBUS );
	
	CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 2);
	CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, 0);
			
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 
		NPC_AIR_WAITINGMODE_WAITTIME);
    
    /* 藯凜摯鄹蹬禱掛蚗哱 */
    CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

    for( i = 0; i < CHAR_PARTYMAX; i ++) {
    	CHAR_setWorkInt( meindex, CHAR_WORKPARTYINDEX1 + i, -1);
    }
	
	/* 鳴↓哱鋆擭埰堎 */
{
	int rev;
	int r = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEMAX);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, RAND( 1, r));
	//print( "route:%d\n",CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));

	/*   Й筒淏↓哱 */
	rev = NPC_Util_GetNumFromStrWithDelim( argstr, "reverse");
	if( rev == 1 ) {
		int num = NPC_AirGetRoutePointNum( meindex, argstr);
		if( num <= 0 ) {
			print( "npcairplane:淩も墅ㄐ\n");
			return FALSE;
		}
		CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, num-1);
		CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, 1);
	}
	/* 鳴↓哱禱掛蚗哱埰堎 */
	NPC_AirSetPoint( meindex, argstr);
	/* 菜拻袸禱  崚埰堎 */
	NPC_AirSetDestPoint( meindex, argstr);
}

    return TRUE;
}


/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_AirTalked( int meindex , int talkerindex , char *szMes ,
                     int color )
{
    int i;
    int	partyflg = FALSE;
	int npc_wave = CHAR_getWorkInt( meindex, NPC_WORK_RUNWAVE);
	
    /* 鏤畛騷媕↓勗葡媃趙煦崹  伀埰堎 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
    /* 猷覕摯蚕↓  贖  篥楱  凝汔竣凝ぅ迋堎 */
    for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
	int index = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
	if( CHAR_CHECKINDEX(index)){
		if( index == talkerindex) {
			partyflg = TRUE;
		}
	}
    }
	if( !partyflg ) {
		//NPC_AirCheckJoinParty( meindex, talkerindex, TRUE);
	}
	else {
		if( CHAR_getWorkInt( meindex, NPC_WORK_MODE) == 0 ) {
			int i;
	//		#define NPC_AIR_DEBUGROUTINTG	"routingtable:"
			if( strstr( szMes, "堤楷" )  ||
				strstr( szMes, "堤楷" )  ||
				strstr( szMes, "Go" )  ||
				strstr( szMes, "go" ))
			{
				CHAR_setWorkInt( meindex, NPC_WORK_MODE,1);
				
				/* 鳴↓鏤匴倳摯蠙③媃禱倵�妡尥滷槬彸� */
	 			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_AIR_LOOPTIME);
				
				/* SE   �梊�  悃璃さ筒摯絖怮   */
				if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG )) {
					//andy_reEdit	NPC_WORK_RUNWAVE
					CHAR_sendSEoArroundCharacter( 
									CHAR_getInt( meindex, CHAR_FLOOR),
									CHAR_getInt( meindex, CHAR_X),
									CHAR_getInt( meindex, CHAR_Y),
									npc_wave,
									TRUE);
				}
				/* ③  埰堎鄹摯隍蚗掛↓喇*/
				for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
					int partyindex = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
					if( CHAR_CHECKINDEX( partyindex)) {
						NPC_AirSendMsg( meindex, partyindex, NPC_AIR_MSG_START);
					}
				}
			}
		}
#if 0
		else if( strstr( szMes, "礿砦" )  ||
			strstr( szMes, "礿砦" )  ||
			strstr( szMes, "stop" )  ||
			strstr( szMes, "Stop" ))
		{
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,2);

			/* 鳴↓鏤匴倳摯騷璃淏↓泬鳴禱蘗�彸彸�  */
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 
						NPC_AIR_WAITINGMODE_WAITTIME);
		    /* 藯凜摯鄹蹬禱掛蚗哱 */
		    CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
		}
		else if( strstr( szMes, NPC_AIR_DEBUGROUTINTG )) {
			/* 溢泬蚗精暵 */
			char *p = strstr( szMes,NPC_AIR_DEBUGROUTINTG);
			char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

			NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
			if( p) {
				int a = atoi( p+strlen(NPC_AIR_DEBUGROUTINTG));
				if( a <0 ) a = 1;
				CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, a);
			}
			//print( "route:%d\n",CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));
			/* 鳴↓哱禱掛蚗哱埰堎 */
			NPC_AirSetPoint( meindex, argstr);
		}
#endif
	}
}
/**************************************
 * 鳴↓鏤匴倳
 **************************************/
void NPC_AirLoop( int meindex)
{
	int	i;
	int npc_wave = CHAR_getWorkInt( meindex, NPC_WORK_RUNWAVE);
	switch( CHAR_getWorkInt( meindex, NPC_WORK_MODE )) {
	  case 0:
	    /* 輝зさ↓迶摯鄹�暕暋甝姻鯆慪濛邴彸� */
		/* 鄹蹬誑歲埱倜摯ぁ�晸�  埰堎 */
		if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) 
			+ CHAR_getWorkInt( meindex, NPC_WORK_WAITTIME) 
			< NowTime.tv_sec)
		{
			/* SE   �梊�  悃璃さ筒摯絖怮   */
			if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG )) {
				//ANDY_reEdit
				CHAR_sendSEoArroundCharacter( 
								CHAR_getInt( meindex, CHAR_FLOOR),
								CHAR_getInt( meindex, CHAR_X),
								CHAR_getInt( meindex, CHAR_Y),
								npc_wave,
								TRUE);
			}
			/* ③  埰堎鄹摯隍蚗掛↓喇*/
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int partyindex = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
				if( CHAR_CHECKINDEX( partyindex)) {
					NPC_AirSendMsg( meindex, partyindex, NPC_AIR_MSG_START);
				}
			}
			
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,1);
			/* 鳴↓鏤匴倳摯蠙③媃禱倵�妡尥滷槬彸� */
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_AIR_LOOPTIME);
		}
		return;
	  case 1:
	  	/* 倵�� */
	  	NPC_Air_walk( meindex);
	  case 2:
		/* 鰼竘埱趙笢堎さ↓迶 */
		/* 鄹蹬誑歲埱倜摯ぁ�晸�  埰堎 */
		if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) 
			+ (CHAR_getWorkInt( meindex, NPC_WORK_WAITTIME) /3)
			< NowTime.tv_sec)
		{
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,1);
			/* 鳴↓鏤匴倳摯蠙③媃禱倵�妡尥滷槬彸� */
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_AIR_LOOPTIME);
		
		}
		return;
	  case 3:
		/* 莒槫媃趙忒�椳芛薩昃尬�哱摯  桷輝з摯諱勗��
		 * 撙媃喫喫ぁ坳攝騷哱禱笢躂趙盓堎
		 */
		if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) + 3	< NowTime.tv_sec){
			char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
			NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_AIR_WAITINGMODE_WAITTIME);
			{
				int r = CHAR_getWorkInt( meindex, NPC_WORK_ROUTEMAX);
				CHAR_setWorkInt( meindex, NPC_WORK_CURRENTROUTE, RAND( 1, r));
			}
			CHAR_setWorkInt( meindex, NPC_WORK_ROUNDTRIP, 
				CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP)^1);
			if( CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP) == 1)  {
				int num = NPC_AirGetRoutePointNum( meindex, argstr);
				CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, num-1);
			}else {
				CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 
					CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT) +1);
			}
			NPC_AirSetPoint( meindex, argstr);
			NPC_AirSetDestPoint( meindex, argstr);
			CHAR_DischargeParty( meindex, 0);
		    CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
			if ((CHAR_getWorkInt(meindex, NPC_WORK_ONEWAYFLG) == 1) &&
			    (CHAR_getWorkInt(meindex, NPC_WORK_ROUNDTRIP) == 1) ){
			  CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NPC_AIR_LOOPTIME);
			  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 1);
			} else
			  CHAR_setWorkInt( meindex, NPC_WORK_MODE, 0);
		}
		return;
	  default:
	    break;
	}
}
/**************************************
 * 倵�呁�
 **************************************/
static void NPC_Air_walk( int meindex)
{
	POINT	start, end;
	int dir;
	int ret;
	int i;
	int npc_wave = CHAR_getWorkInt( meindex, NPC_WORK_RUNWAVE );

	/* 倵�抌�祛 */
	/* 莒槫媃倜鄹摯窐   */
	start.x = CHAR_getInt( meindex, CHAR_X);
	start.y = CHAR_getInt( meindex, CHAR_Y);
	end.x = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOX);
	end.y = CHAR_getWorkInt( meindex, NPC_WORK_ROUTETOY);

	/* 莒槫媃倜摯ぁべ摯睽騷璃哱勗 */
	if( start.x == end.x && start.y == end.y ) {
		int add = 1;
		char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

		NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

		if( CHAR_getWorkInt( meindex, NPC_WORK_ROUNDTRIP ) == 1 ) {
			add *= -1;
		}
		CHAR_setWorkInt( meindex, NPC_WORK_ROUTEPOINT, 
			CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT) +add);
		if( NPC_AirSetPoint( meindex, argstr) == FALSE ) {
			/*     勗莒槫*/
			/* 輝зさ↓迶勗埰堎 */
			CHAR_setWorkInt( meindex, NPC_WORK_MODE,3);
			
			/* SE   �梊�  悃璃さ筒摯絖怮   */
			if( CHAR_getWorkInt( meindex, NPC_WORK_SEFLG )) {
				//ANDY_reEdit
				CHAR_sendSEoArroundCharacter( 
					CHAR_getInt( meindex, CHAR_FLOOR),
					CHAR_getInt( meindex, CHAR_X),
					CHAR_getInt( meindex, CHAR_Y),
					npc_wave,
					TRUE);
			}
			/* 槫笢倜鄹摯隍蚗掛↓喇*/
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int partyindex = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1+i);
				if( CHAR_CHECKINDEX( partyindex)) {
					NPC_AirSendMsg( meindex, partyindex, NPC_AIR_MSG_END);
				}
			}
			/* 藯凜摯鄹蹬禱掛蚗哱 */
			CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
			return;
		}
		else {
			return;
		}
	}
	/*-------------------------------------------------------*/
	/* 倵凝鞠堎窐   */
	
	/*   澺禱滑誧堎 */
	dir = NPC_Util_getDirFromTwoPoint( &start,&end );

	/* ぽ笢堎鳹鐎摯輝    蚕↓  贖倵拻ぁ窅竣   */
	end.x = CHAR_getInt( meindex, CHAR_X);
	end.y = CHAR_getInt( meindex, CHAR_Y);

#if 0
	/* 痲埱凝凝埱倜鄹摯諱摯窐   */
	for( i = 0; i < 100; i ++ ) {	
		if( dir < 0 ) {
			dir = RAND( 0,7);
		}	
		dir = NPC_Util_SuberiWalk( meindex, dir);
		if( dir >= 0 && dir <= 7) break;
	}
#endif
	
	if( dir >= 0 && dir <= 7 ) {
		/* 倵�� */
		ret = CHAR_walk( meindex, dir, 0);

		if( ret == CHAR_WALKSUCCESSED ) {
			/* 猷覕誑虮埵�淴棺甝峒睅挨齉� */
			int	i;
			int	mefl=CHAR_getInt( meindex, CHAR_FLOOR);
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int toindex = CHAR_getWorkInt( meindex, i + CHAR_WORKPARTYINDEX1);
				int fl = CHAR_getInt( toindex, CHAR_FLOOR);
				int xx = CHAR_getInt( toindex, CHAR_X);
				int yy = CHAR_getInt( toindex, CHAR_Y);
				if( CHAR_CHECKINDEX(toindex) &&
				    (mefl==fl) && (abs(xx-end.x)+abs(yy-end.y)<10) ) {
					int	parent_dir;
					/* 碳摯痾  敁�楢宎匾硩殢□啕�  凝��  澺禱滑誧堎 */
					/* 倵�� */
					start.x = xx;
					start.y = yy;
					parent_dir = NPC_Util_getDirFromTwoPoint( &start,&end );
					/* 精溘溢贖坳筒酘鏤で砫璃倵拻禱  藯埰堎諱勗��
					 * べ摯碳毀鞳摯碳摯  禱黹竣源竣勗埰堎
					 */
					end = start;
					if( parent_dir != -1 ) {
						CHAR_walk( toindex, parent_dir, 0);
					}
				}
			}
		}
	}
}
/**************************************
 * べ摯鳹鐎禱掛蚗哱埰堎
 **************************************/
static int NPC_AirSetPoint( int meindex, char *argstr)
{
	char	buf[4096];
	char	buf2[256];
	char	buf3[256];
	int floor,warpx,warpy;
	int ret;
	char routetostring[64];
	
	snprintf( routetostring, sizeof( routetostring), "routeto%d", 
				CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, routetostring,buf, sizeof(buf))
		== NULL ) 
	{
		print( "npcair:nothing route \n");
		return FALSE;
	}

	ret = getStringFromIndexWithDelim( buf, ";", 
		CHAR_getWorkInt( meindex, NPC_WORK_ROUTEPOINT),
		buf2, sizeof(buf2));
	if( ret == FALSE ) return FALSE;

	// Arminius: add floor
	
	ret = getStringFromIndexWithDelim( buf2, ",", 1,
		buf3, sizeof(buf3));
	if( ret == FALSE) return FALSE;
	floor = atoi(buf3);

	ret = getStringFromIndexWithDelim( buf2, ",", 2,
		buf3, sizeof(buf3));
	if( ret == FALSE) return FALSE;
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOX, atoi( buf3));
	warpx = atoi(buf3);
	
	ret = getStringFromIndexWithDelim( buf2, ",", 3,
		buf3, sizeof(buf3));
	if( ret == FALSE) return FALSE;
	CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOY, atoi( buf3));
	warpy = atoi(buf3);

	if (floor!=CHAR_getInt(meindex, CHAR_FLOOR)) {
		int	i;
		CHAR_warpToSpecificPoint(meindex, floor, warpx, warpy);

		for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
			int toindex = CHAR_getWorkInt( meindex, i + CHAR_WORKPARTYINDEX1);
			if( CHAR_CHECKINDEX(toindex) ) {
				CHAR_warpToSpecificPoint(toindex, floor, warpx, warpy);
			}
		}
		CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOX, warpx);
		CHAR_setWorkInt( meindex, NPC_WORK_ROUTETOY, warpy);
	}

	return TRUE;
}
/**************************************
 * route  蠕凝�掁�  鞳誑堣埱倜�桯孩壅�
 * 措蠕摯敁喫勗掛蚗哱埰堎��
 **************************************/
static void NPC_AirSetDestPoint( int meindex, char *argstr)
{
	char 	buf[256];
	char	routename[256];

	snprintf( routename, sizeof( routename), "routename%d", 
				CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));

	if( NPC_Util_GetStrFromStrWithDelim( argstr, routename, buf, sizeof( buf))
		!= NULL ) 
	{
		CHAR_setChar( meindex, CHAR_OWNTITLE, buf);
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
	}
}
/**************************************
 * 炩擭踏躂倜囮騷  堜禱  埱趙笢堎凝鏍攝蚗袲埰堎
 *   埱趙笢倜�梫硐�
 **************************************/
static BOOL NPC_AirCheckDeniedItem( int meindex, int charaindex, char *argstr)
{
	char	buf[1024];
	BOOL	found = TRUE;

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "denieditem", buf, sizeof( buf))
		!= NULL ) 
	{
		int	i;
		int ret;
		for( i = 1; ; i ++) {
			int itemid;
			char buf2[64];
			int j;
			ret = getStringFromIndexWithDelim( buf, ",", i, buf2, sizeof(buf2));
			if( ret == FALSE ) break;
			itemid = atoi( buf2);
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++) {
				int itemindex = CHAR_getItemIndex( charaindex, j);
				if( ITEM_CHECKINDEX( itemindex)) {
					if( ITEM_getInt( itemindex, ITEM_ID) == itemid) {
						found = FALSE;
						break;
					}
				}
			}
		}
	}
	return found;
}
/**************************************
 * 炩擭踏躂倜囮騷  堜禱  埱趙笢堎凝鏍攝蚗袲埰堎
 *   埱趙笢埵笢敁煦誧
 **************************************/
BOOL NPC_AirCheckAllowItem( int meindex, int charaindex, BOOL pickupmode)
{
	char	buf[1024];
	BOOL	found = TRUE;
	BOOL	pickup = FALSE;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "pickupitem", buf, sizeof( buf))
		!= NULL ) 
	{
		pickup = TRUE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "allowitem", buf, sizeof( buf))
		!= NULL ) 
	{
		int	i;
		int ret;
		for( i = 1; ; i ++) {
			int itemid;
			char buf2[64];
			int j;
			BOOL	getflg;
			ret = getStringFromIndexWithDelim( buf, ",", i, buf2, sizeof(buf2));
			if( ret == FALSE ) break;
			itemid = atoi( buf2);
			getflg = FALSE;
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++) {
				int itemindex = CHAR_getItemIndex( charaindex, j);
				if( ITEM_CHECKINDEX( itemindex)) {
					if( ITEM_getInt( itemindex, ITEM_ID) == itemid) {
						/* 邳贀誑諏埱趙笢堎凝�掁搮姨動岌�  堜禱噁堎 */
						if( pickupmode && pickup && !getflg) {
							CHAR_DelItem( charaindex, j);
							getflg = TRUE;
						}
						break;
					}
				}
			}
			if( j == CHAR_MAXITEMHAVE) {
				found = FALSE;
				break;
			}
		}
	}
	return found;
}

/**************************************
 * 炩擭踏躂倜畛穫鳴雄窀凝鏍攝蚗袲埰堎
 **************************************/
static BOOL NPC_AirCheckLevel( int meindex, int charaindex, char *argstr)
{
	int		level;
	
	/* 埵崹躂�屼倠縋聿陔羽捊冕鯆慪濛� */
	level = NPC_Util_GetNumFromStrWithDelim( argstr, "needlevel");
	if( level == -1 ) {
		return TRUE;
	}
	if( CHAR_getInt( charaindex, CHAR_LV) >= level ) return TRUE;
	
	return FALSE;
}

#ifdef _NPC_AIRLEVEL
static BOOL NPC_AirCheckMaxLevel( int meindex, int charaindex, char *argstr)
{
	int		level;
	
	/* 埵崹躂�屼倠縋聿陔羽捊冕鯆慪濛� */
	level = NPC_Util_GetNumFromStrWithDelim( argstr, "maxlevel");
	if( level == -1 ) {
		return TRUE;
	}
	if( CHAR_getInt( charaindex, CHAR_LV) < level ) return TRUE;
	
	return FALSE;
}
#endif

/**************************************
 * 遛鉣禱鏍攝蚗袲埰堎
 * -1 藗   0雄窀§    �旓捆�  菺Stone
 **************************************/
static int NPC_AirCheckStone( int meindex, int charaindex, char *argstr)
{
	int		gold;
	
	/* 埵崹躂�屼倠縋聿陔羽捊冕鯆慪濛� */
	gold = NPC_Util_GetNumFromStrWithDelim( argstr, "needstone");
	if( gold == -1 ) {
		return 0;
	}
	if( CHAR_getInt( charaindex, CHAR_GOLD) >= gold ) return gold;
	
	return -1;
}
/**************************************
 * 隍蚗掛↓喇禱邞堎
 * 礎倳摯隍蚗掛↓喇誑埵崹躂�伢萼觙伒懩�隍蚗掛↓喇禱邞堎
 **************************************/
static void NPC_AirSendMsg( int meindex, int talkerindex, int tablenum)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[256];
	char	msg[256];
	if( tablenum < 0 || tablenum >= arraysizeof( airmsg)) return;
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, airmsg[tablenum].option, buf, sizeof( buf))
		!= NULL ) 
	{
		strcpy( msg, buf);
	}
	else {
		snprintf( msg, sizeof(msg),airmsg[tablenum].defaultmsg);
	}
	CHAR_talkToCli( talkerindex, meindex, msg, CHAR_COLORYELLOW);
}
/**************************************
 * 鳴↓哱  ↓々鳴摯睽騷璃哱摯倳禱噁  埰堎
 **************************************/
static int NPC_AirGetRoutePointNum( int meindex, char *argstr )
{
	int		i;
	char	buf[4096];
	char	buf2[256];
	int ret;
	char routetostring[64];
	
	snprintf( routetostring, sizeof( routetostring), "routeto%d", 
				CHAR_getWorkInt( meindex, NPC_WORK_CURRENTROUTE));
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, routetostring,buf, sizeof(buf))
		== NULL ) 
	{
		print( "npcair:nothing route \n");
		return -1;
	}
	for( i = 1; ; i ++ ) {
		ret = getStringFromIndexWithDelim( buf, ";", i, buf2, sizeof(buf2));
		if( ret == FALSE) break;
	}
	return( i -1);
}
BOOL NPC_AirCheckJoinParty( int meindex, int charaindex, BOOL msgflg)
{
    //int		fd;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int		ret;
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	/* 兒精惉蚗迶雄  摯陑 */
	if( !NPC_Util_charIsInFrontOfChar( charaindex, meindex, 1 )) return FALSE; 
	/*     篥氈毀藸  埰堎 */
	if( CHAR_getWorkInt( meindex, NPC_WORK_MODE) != 0 ) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_GETTINGON);
		return FALSE;
	}
	/* 毞↓趙祥煦埱倜�梫硐� */
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) != CHAR_PARTY_NONE) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_NOTPARTY);
		return FALSE;
	}
	/* 蚕↓  贖摯硞倳禱鏍攝蚗袲埰堎 */
	if( CHAR_getEmptyPartyArray( meindex) == -1 ) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_OVERPARTY);
		return FALSE;
	}
	/* 囮騷  堜摯鏍攝蚗袲禱埰堎(鉞鰼囮騷  堜) */
	if( !NPC_AirCheckDeniedItem( meindex, charaindex, argstr)) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_DENIEDITEM);
		return FALSE;
	}
#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE )	{
		CHAR_talkToCli( charaindex, -1, "拸楊觓湍億昜奻儂﹝", CHAR_COLORYELLOW);
		return FALSE;
	}
#endif

	/* 囮騷  堜摯鏍攝蚗袲禱埰堎(  菺囮騷  堜) */
	if( !NPC_AirCheckAllowItem( meindex, charaindex, FALSE)) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_ALLOWITEM);
		return FALSE;
	}
#ifdef _NPC_AIRDELITEM
	if( !NPC_AirCheckDelItem( meindex, charaindex, FALSE) ){ //�襣Ы遛蛦�賸耋撿
		if( msgflg ) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_DELITEM);
	    return FALSE;
	}
#endif
	/* 畛穫鳴摯鏍攝蚗袲禱埰堎 */
	if( !NPC_AirCheckLevel( meindex, charaindex, argstr)) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_LEVEL);
		return FALSE;
	}
#ifdef _NPC_AIRLEVEL
    if( !NPC_AirCheckMaxLevel( meindex, charaindex, argstr)) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_MAXLEVEL);
		return FALSE;
	}
#endif
	/* 騷穫璃哱  凝鏍攝蚗袲埰堎 */
//	if( CHAR_getInt( charaindex, CHAR_NOWEVENT) != 0 ||
//		CHAR_getInt( charaindex, CHAR_NOWEVENT2) != 0 ||
//		CHAR_getInt( charaindex, CHAR_NOWEVENT3) != 0 )
//	{
//		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_EVENT);
//		return FALSE;
//	}
	/* 遛鉣摯鏍攝蚗袲禱埰堎  堁鉣禱噁堎摯ぁ��  敖鏍攝蚗袲勗埰堎喫敁∪   */
	ret = NPC_AirCheckStone( meindex, charaindex, argstr);
	if( ret == -1 ) {
		if( msgflg) NPC_AirSendMsg( meindex, charaindex, NPC_AIR_MSG_GOLD);
		return FALSE;
	}
	if( ret != 0 ) {
		char msgbuf[128];
		/* 遛鉣禱敁堎 */
		CHAR_setInt( charaindex, CHAR_GOLD, 
					CHAR_getInt( charaindex, CHAR_GOLD) - ret);
		/* 邞嚭 */
		CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
		snprintf( msgbuf, sizeof( msgbuf), "盓葆賸%d Stoneㄐ", ret);
		CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
	}
	/* 蚕↓  贖勗  堎 */
	//CHAR_JoinParty_Main( charaindex, meindex);
	
	//fd = getfdFromCharaIndex( charaindex );
	
	//lssproto_PR_send( fd, 1, 1);
	
	return TRUE;
}

#ifdef _NPC_AIRDELITEM //奻滄儂奀,潰脤岆瘁猁諶壺耋撿
BOOL NPC_AirCheckDelItem( int meindex, int charaindex, BOOL pickupmode)
{
	char	buf[1024];
	BOOL	found = TRUE;
	BOOL	pickup = FALSE;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "delitem", buf, sizeof( buf))
		!= NULL ) 
	{
		int	i;
		int ret;
		for( i = 1; ; i ++) {
			int itemid;
			char buf2[64];
			int j;
			ret = getStringFromIndexWithDelim( buf, ",", i, buf2, sizeof(buf2));
			if( ret == FALSE ) break;
			itemid = atoi( buf2);
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++) {
				int itemindex = CHAR_getItemIndex( charaindex, j);
				if( ITEM_CHECKINDEX( itemindex)) {
					if( ITEM_getInt( itemindex, ITEM_ID) == itemid) {
						CHAR_DelItem( charaindex, j);
						break;
					}
				}
			}
			if( j == CHAR_MAXITEMHAVE) {
				found = FALSE;
				break;
			}
		}
	}
	return found;
}
#endif
