#include "version.h"
#include <string.h>
#include "char.h"
#include "char_event.h"
#include "object.h"
#include "handletime.h"
#include "magic.h"
#include "npcutil.h"
#include "char_data.h"
#include "chatmagic.h"
#include "net.h"
#include "configfile.h"
#include "npcgen.h"

/*
---------------------------------------------------------------------
sysinfo By Kawata
---------------------------------------------------------------------
*/

#define		NPC_SYSINFO_MSGMAX				21

#define		NPC_SYSINFO_PASSWD_DEFAULT		"G00d1uck"

#define		NPC_SYSINFO_TIMELIMIT_DEFAULT		120

#define		NPC_SYSINFO_SHUTDOWNLIMIT_DEFAULT	5

#define		NPC_SYSINFO_SHUTDOWN_MSG		"婬徹 %d 煦笘摽ㄛ撈羲宎輛俴server炵苀峎誘﹝"

#define		NPC_SYSINFO_SHUTDOWN_MSG_COMP	"server眒壽敕﹝"
enum {
	CHAR_WORK_MODE        		= CHAR_NPCWORKINT1,
	CHAR_WORK_TIME	    		= CHAR_NPCWORKINT2,
	CHAR_WORK_TIMELIMIT    		= CHAR_NPCWORKINT3,
	CHAR_WORK_SHUTDOWNTIME		= CHAR_NPCWORKINT4,
	CHAR_WORK_SHUTDOWNLIMIT		= CHAR_NPCWORKINT5,
	CHAR_WORK_SHUTDOWNDSPTIME	= CHAR_NPCWORKINT6,
	CHAR_WORK_MSGCOLOR			= CHAR_NPCWORKINT7,
	CHAR_WORK_PLAYER			= CHAR_NPCWORKINT8,
};

enum {
	CHAR_WORKCHAR_PASSWD = CHAR_NPCWORKCHAR1,
};
typedef struct {
	char	msg[16];
}CMD_MSG;
CMD_MSG	cmd_msg[] = {
                        { "startinfo"}, {"endinfo"},	{"help"},
						{"player"},		{"list"},		{"sendno"},
						{"sendmsg"},	{"shutdown"},	{"msgcol"},
						{"sysinfo"},	{"info"},
                        { "allowaccept" } , { "denyaccept" } ,
                        { "closeallsockets" },
                        { "allnosee" },
                        { "allnobody" },
                        { "movenum" },
                        { "bornnum" },
                        { "enemystate" },
                        { "setwalktime"},{ "setcatime" },{"setcdtime"},
                        { "setoneloop"},
                        { "titlerestart"},
                        { "setsaacwrite"}, { "setsaacread"},
};
enum{
	NPC_SYSINFO_MSG_STARTINFO,
	NPC_SYSINFO_MSG_ENDINFO,
	NPC_SYSINFO_MSG_HELP,
	NPC_SYSINFO_MSG_PLAYER,
	NPC_SYSINFO_MSG_LIST,
	NPC_SYSINFO_MSG_SENDNO,
	NPC_SYSINFO_MSG_SENDMSG,
	NPC_SYSINFO_MSG_SHUTDOWN,
	NPC_SYSINFO_MSG_MSGCOL,
	NPC_SYSINFO_MSG_SYSINFO,
	NPC_SYSINFO_MSG_INFO,
	NPC_SYSINFO_MSG_ALLOWACCEPT,
	NPC_SYSINFO_MSG_DENYACCEPT,
	NPC_SYSINFO_MSG_CLOSEALLSOCKETS,

	/* 渣�眉做� */
	NPC_SYSINFO_ALL_NOSEE,
	NPC_SYSINFO_ALL_NOBODY,
	NPC_SYSINFO_MOVE_NUM,
	NPC_SYSINFO_BORN_NUM,
	NPC_SYSINFO_ENEMY_STATE,
	NPC_SYSINFO_SETWALKTIME,
	NPC_SYSINFO_SETCATIME,
	NPC_SYSINFO_SETCDTIME,
	NPC_SYSINFO_SETONELOOP,
	NPC_SYSINFO_TITLERESTART,
	NPC_SYSINFO_SETSAACWRITE,
	NPC_SYSINFO_SETSAACREAD,

	NPC_SYSINFO_MSG_NUM,
};

typedef struct {
	char	arg[32];
	char	defMsg[128];
}NPC_Sysinfo_Msg;

static NPC_Sysinfo_Msg		msgtable[] = {
	{ "def_msg",			"扂茧衄睿�封擠蝯饑佫絰做闡僋式�" },
	{ "shutdown_msg",		"珋婓羲宎猁輛俴server炵苀峎誘③鴃厒logoutㄐㄐ"},
};
enum{
	NPC_SYSINFO_ARG_DEF_MSG,
	NPC_SYSINFO_ARG_SHUTDOWN_MSG,
};


static void NPC_Sysinfo_SendMsg( int meindex, int pindex, int tblnum);
static char *NPC_Sysinfo_GetMsg( int meindex, char *msgindexstr,
								char *out,int outlen, int num );
static void NPC_Sysinfo_SendMsgToAll( int meindex, char *msg);
static void NPC_Sysinfo_Msg_EndInfo( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Help( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Player( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_List( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_SendNo( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_SendMsg( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Shutdown( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Msg_Msgcol( int meindex, int tindex, char *msg);

/* 渣�眉做� */
static void NPC_Sysinfo_All_NoBody( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_All_NoSee( int meindex, int tindex, char *msg);

static void NPC_Sysinfo_Move_Num( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Born_Num( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_Enemy_State( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetWalkTime( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetCATime( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetCDTime( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetOneloop( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_TitleRestart( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetSaacwrite( int meindex, int tindex, char *msg);
static void NPC_Sysinfo_SetSaacread( int meindex, int tindex, char *msg);

static void NPC_Sysinfo_Msg_allowAccept(int meindex, int tindex, char*msg)
{
    SERVSTATE_SetAcceptMore( 1 );
	CHAR_talkToCli( tindex, meindex, "allowaccept OK.", CHAR_COLORYELLOW);
}
static void NPC_Sysinfo_Msg_denyAccept(int meindex, int tindex, char*msg)
{
    SERVSTATE_SetAcceptMore( 0 );
	CHAR_talkToCli( tindex, meindex, "denyaccept OK.", CHAR_COLORYELLOW);
}
static void NPC_Sysinfo_Msg_closeallsockets(int meindex, int tindex,
                                            char*msg)
{
	CHAR_talkToCli( tindex, meindex, "closeallsockets OK.", CHAR_COLORYELLOW);
    closeAllConnectionandSaveData();
}
static void NPC_Sysinfo_Msg_Sysinfo(int meindex, int tindex, char*msg)
{
	CHAR_CHAT_DEBUG_sysinfo( tindex, msg);
}
static void NPC_Sysinfo_Msg_Info(int meindex, int tindex, char*msg)
{
	CHAR_CHAT_DEBUG_info( tindex, msg);
}


typedef void	(*FUNC)(int,int,char *);
static FUNC		functbl[] = {
	NULL,
	NPC_Sysinfo_Msg_EndInfo,
	NPC_Sysinfo_Msg_Help,
	NPC_Sysinfo_Msg_Player,
	NPC_Sysinfo_Msg_List,
	NPC_Sysinfo_Msg_SendNo,
	NPC_Sysinfo_Msg_SendMsg,
	NPC_Sysinfo_Msg_Shutdown,
	NPC_Sysinfo_Msg_Msgcol,
	NPC_Sysinfo_Msg_Sysinfo,
	NPC_Sysinfo_Msg_Info,
    NPC_Sysinfo_Msg_allowAccept,
    NPC_Sysinfo_Msg_denyAccept,
    NPC_Sysinfo_Msg_closeallsockets,
	/* 渣�眉做� */
    NPC_Sysinfo_All_NoSee,
    NPC_Sysinfo_All_NoBody,
    NPC_Sysinfo_Move_Num,
    NPC_Sysinfo_Born_Num,
    NPC_Sysinfo_Enemy_State,
    NPC_Sysinfo_SetWalkTime,
    NPC_Sysinfo_SetCATime,
    NPC_Sysinfo_SetCDTime,
    NPC_Sysinfo_SetOneloop,
    NPC_Sysinfo_TitleRestart,
    NPC_Sysinfo_SetSaacwrite,
    NPC_Sysinfo_SetSaacread,
    
};

/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_SysinfoInit( int meindex )
{
	int tmp;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
    char	buff[128];

    NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	/* 疐趵撬窐   */

	CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 0);

	/* 蚕筒極↓迶掛蚗哱 */
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "passwd", buff, sizeof( buff))
		== NULL)
	{
		strcpy( buff, NPC_SYSINFO_PASSWD_DEFAULT);
	}
	CHAR_setWorkChar( meindex, CHAR_WORKCHAR_PASSWD, buff);


	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "time");
	if( tmp == -1 ) tmp = NPC_SYSINFO_TIMELIMIT_DEFAULT;
	CHAR_setWorkInt( meindex, CHAR_WORK_TIMELIMIT, tmp);

    /* 隍蚗掛↓喇摯褗噁   */
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "msg_col");
	if( tmp < CHAR_COLORRED || tmp > CHAR_COLORWHITE )
		tmp = CHAR_COLORYELLOW;
	CHAR_setWorkInt( meindex, CHAR_WORK_MSGCOLOR, tmp);

	CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, -1);
	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME, 0);
	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME, 0);
    CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEOTHERNPC);
	CHAR_setFlg( meindex, CHAR_ISATTACKED, 0);

	return TRUE;
}

/*********************************
* Loop窐  
**********************************/
void NPC_SysinfoLoop( int meindex )
{
	int		shuttime;
	if( CHAR_getWorkInt( meindex, CHAR_WORK_MODE) == 1) {
		int		oldtime;
		/* 鄹蹬禱鵌誧堎 */
		oldtime = CHAR_getWorkInt( meindex, CHAR_WORK_TIME);
		/* �羬袬暋盝鞳懽鵅嚾玉折棐飽灃騄� */
		if( NowTime.tv_sec - oldtime >
			CHAR_getWorkInt( meindex, CHAR_WORK_TIMELIMIT))
		{
			CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 0);
			CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, -1);
		}
	}
	shuttime = CHAR_getWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME);
	/* shutdown鄹蹬鏍攝蚗袲 */
	if( shuttime > 0 ) {
		int diff,limit,hun;

		diff = NowTime.tv_sec - shuttime;
		limit = CHAR_getWorkInt( meindex, CHAR_WORK_SHUTDOWNLIMIT);
		hun = limit - (diff/60);
		/* 兒覕堁拻勗隍蚗掛↓喇 */
		if( hun != CHAR_getWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME)){
			char	buff[256];
			if( hun != 0 ) {
				snprintf( buff, sizeof(buff), NPC_SYSINFO_SHUTDOWN_MSG, hun);
			}
			else {
				strcpy( buff, NPC_SYSINFO_SHUTDOWN_MSG_COMP);
			}
			NPC_Sysinfo_SendMsgToAll( meindex, buff);
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME, hun);
		}
		if( hun == 0) {
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME,0);
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNLIMIT,0);
			CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME,0);
		}
	}
}
/*********************************
*   凝崹�桫憶袬敯啄�  
**********************************/
void NPC_SysinfoTalked( int meindex, int tindex, char *msg, int color)
{
	char	*msgwk;
	char	*buff;
	int		len;
	int		msgno;

	len = strlen( msg)+1;
	msgwk = calloc( 1, sizeof(char)*len);
	buff = calloc( 1, sizeof(char)*len);
	strcpy( msgwk, msg);
	/* 筒妐↓筒禱竘敁誧堎 */
	deleteSequentChar( msgwk, " ");
	if( CHAR_getWorkInt( meindex, CHAR_WORK_MODE) == 0 ) {
		msgno = 0;
		if( strncmp( msgwk, cmd_msg[NPC_SYSINFO_MSG_STARTINFO].msg,
					strlen( cmd_msg[NPC_SYSINFO_MSG_STARTINFO].msg)) == 0)
		{
			if( getStringFromIndexWithDelim( msgwk, " ", 2, buff, len)
				== TRUE )
			{
				if( strcmp( buff, CHAR_getWorkChar( meindex, CHAR_WORKCHAR_PASSWD))
					== 0 )
				{
					CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 1);
					CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, tindex);
					CHAR_setWorkInt( meindex, CHAR_WORK_TIME, NowTime.tv_sec);
					CHAR_talkToCli( tindex, meindex,
								"眒痄善硌鍔耀宒﹝偌狟help撈頗堤珋佽隴紫﹝",
								CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
					CHAR_talkToCli( tindex, meindex,
								"褫眕婓enemystate 笢賸賤菩�佽觸�汜睿俴雄杅﹝",
								CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

				}
			}
		}
		if( CHAR_getWorkInt( meindex, CHAR_WORK_MODE ) == 0 ) {
			NPC_Sysinfo_SendMsg( meindex, tindex, NPC_SYSINFO_ARG_DEF_MSG);
		}
	}
	else {
		/* 昡悃璃迶さ↓迶勗媃倜硞媃凝窐  ぁ拻埵笢 */
		if( tindex == CHAR_getWorkInt( meindex, CHAR_WORK_PLAYER)) {
			int		i;
			/* 鼠躂�葸擁�  �擸雌倠� */
			for( i = NPC_SYSINFO_MSG_ENDINFO; i < NPC_SYSINFO_MSG_NUM; i ++ ) {
				if( strncmp( msgwk, cmd_msg[i].msg, strlen( cmd_msg[i].msg)) == 0) {
					if( functbl[i] != NULL ) {
						functbl[i]( meindex, tindex, msgwk);
					}
					break;
				}
			}
			CHAR_setWorkInt( meindex, CHAR_WORK_TIME, NowTime.tv_sec);
		}
		else {
			NPC_Sysinfo_SendMsg( meindex, tindex, NPC_SYSINFO_ARG_DEF_MSG);
		}
	}
	free( msgwk);
	free( buff);
}
/*------------------------------------------------------------------------
 * 玊橖さ↓迶喜
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_EndInfo( int meindex, int tindex, char *msg)
{
	CHAR_setWorkInt( meindex, CHAR_WORK_MODE, 0);
	CHAR_setWorkInt( meindex, CHAR_WORK_PLAYER, -1);
	CHAR_talkToCli( tindex, meindex,
					"眒痄善珨啜耀宒",
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

}
/*------------------------------------------------------------------------
 * HELP隍蚗掛↓喇
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Help( int meindex, int tindex, char *msg)
{
	int		i;
	typedef struct {
		char	str[512];
	}SYSINFO_MSGWK;
	SYSINFO_MSGWK	msgwk[] = {
	{ "硌鍔HELP"},
	{ "help                  森砐捅洘﹝"},
	{ "player                桶尨珋婓LOGIN腔俙模�侕�﹝"},
	{ "list                  捅洘珨擬﹝"},
	{ "send number          ☆number★ 岆杅硉﹝硌隅捅洘腔瘍鎢摽蔚蜆捅洘囀�搥屎芵醽垓艙騵瞍牷�"},
	{ "sendmsg msgs          msgs峈砑換冞腔捅洘﹝婓send 眳摽諾珨跡ㄛ撈褫蔚眳摽腔恅梒冞陓跤�垓艙騵瞍牷�"},
	{ "shutdown min          min 測桶煦﹝"},
	{ "msgcol col            扢隅換冞跤俙模捅洘腔晇伎﹝晇伎腔硌隅衄red,green,yellow,blue,purple,cyan,white﹝"},
	{ "sysinfo               炵苀袨怓"},
	{ "info                  俙模袨怓"},
	{ "endinfo               隙葩ぱ籵腔袨怓﹝"},
	{ "denyaccept            拸楊諉忳CLIENT傷腔諉哿﹝"},
	{ "allowaccept           諉忳CLIENT傷腔諉哿﹝"},
	{ "closeallsockets       Чつ�顏LIENT傷LOGOUT﹝婓囀窒妗俴denyaccept﹝"},
	{ "titlerestart          婬棒黍�○ざ鷗�腔扢隅﹝"},
	{ "enemystate            佽隴NPC腔袨錶﹝"},
	{ ""},
	};

	for( i = 0; msgwk[i].str[0] != '\0'; i ++ ) {
		CHAR_talkToCli( tindex, meindex, msgwk[i].str,
						CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}
/*------------------------------------------------------------------------
 * 鏤畛騷媕↓摯倳禱ぅ迋堎
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Player( int meindex, int tindex, char *msg)
{
    int     i, cnt = 0;
    int     playernum=CHAR_getPlayerMaxNum();
    char	buff[64];
    for( i=0 ; i<playernum ; i++ ){
        if( CHAR_getCharUse(i) != FALSE ) {
        	if( CHAR_getInt( i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
        		cnt ++;
        	}
        }
	}
	snprintf( buff, sizeof( buff), "俙模衄%d�芊�", cnt);
	CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

}
/*------------------------------------------------------------------------
 * 隍蚗掛↓喇惉筒哱禱  崚埰堎
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_List( int meindex, int tindex, char *msg)
{
	char	buff[512];
	char	buff2[512];
	int		i;

	for( i = 1; i < NPC_SYSINFO_MSGMAX; i ++ ) {
		if( NPC_Sysinfo_GetMsg( meindex, "sendno", buff, sizeof( buff), i)
			!= NULL )
		{
			snprintf( buff2, sizeof(buff2), "No.%d:%s", i, buff);
			CHAR_talkToCli( tindex, meindex, buff2,
							CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

		}
	}
}
/*------------------------------------------------------------------------
 * 隍蚗掛↓喇  蠕凝�梮肩彌鴃麚熁屎近鵋彸�
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_SendNo( int meindex, int tindex, char *msg)
{
	char	buff[32];
	char	msgbuf[512];

	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		int	num;
		num = atoi( buff);
		if( num >=1 && num < NPC_SYSINFO_MSGMAX ) {
			if( NPC_Sysinfo_GetMsg( meindex, "sendno", msgbuf,
				sizeof( msgbuf), num) != NULL )
			{
				NPC_Sysinfo_SendMsgToAll( meindex, msgbuf);
			}
		}
	}
}
/*------------------------------------------------------------------------
 * 炩擭踏躂倜隍蚗掛↓喇  棬  禱邞嚭埰堎
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_SendMsg( int meindex, int tindex, char *msg)
{
	char	buff[512];

	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		NPC_Sysinfo_SendMsgToAll( meindex, buff);
	}
}
/*------------------------------------------------------------------------
 * 隍蚗掛↓喇庈溘↓禱  脾埰堎
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Msgcol( int meindex, int tindex, char *msg)
{
	char	buff[64];
	typedef struct {
		int		color;
		char	str[10];
	}SYSINFO_COLSET;
	SYSINFO_COLSET		colset[] = {
		{ 249, "red"},
		{ 250, "green"},
		{ 251, "yellow"},
		{ 252, "blue"},
		{ 253, "purple"},
		{ 254, "cyan"},
		{ 255, "white"},
	};

	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		int		i;
		for( i = 0; i < 7; i ++ ) {
			if( strstr( buff, colset[i].str) != NULL ) {
				CHAR_setWorkInt( meindex, CHAR_WORK_MSGCOLOR, colset[i].color);
				CHAR_talkToCli( tindex, meindex, "眒蜊曹捅洘腔晇伎﹝",
								CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
				break;
			}
		}
	}
}
/*------------------------------------------------------------------------
 * 礎倳凝�梮肩彌鴃麚熁思�  埰堎
 *----------------------------------------------------------------------*/
static char *NPC_Sysinfo_GetMsg( int meindex, char *msgindexstr,
								char *out,int outlen, int num )
{

	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	*work;
	char	*buff;
	char	*cret = NULL;
	int 	worklen;

	worklen = strlen( msgindexstr)+3;
	work = (char *)calloc( 1, worklen * sizeof( char));
	buff = (char *)calloc( 1, outlen  * sizeof( char));

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	snprintf( work, worklen, "%s%d", msgindexstr, num);
	if( NPC_Util_GetStrFromStrWithDelim( argstr, work,
									buff, outlen) != NULL )
	{
		strcpy( out, buff);
		cret = out;
	}

	free( work);
	free( buff);
	return( cret);
}
/*------------------------------------------------------------------------
 * shutdown隍蚗掛↓喇窐  
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_Msg_Shutdown( int meindex, int tindex, char *msg)
{
	char	buff[10];
	int		hun;

	/*   疐摯隍蚗掛↓喇邞嚭 */
	NPC_Sysinfo_SendMsg( meindex, tindex, NPC_SYSINFO_ARG_SHUTDOWN_MSG);
	/* 鄹蹬掛蚗哱 */
	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNTIME, NowTime.tv_sec);
	/* �羬袬暋碇�   */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		hun = atoi( buff);
		if( hun <= 0 ) {
			hun = NPC_SYSINFO_SHUTDOWNLIMIT_DEFAULT;
		}
	}
	else {
		hun = NPC_SYSINFO_SHUTDOWNLIMIT_DEFAULT;
	}

	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNLIMIT, hun);

	CHAR_setWorkInt( meindex, CHAR_WORK_SHUTDOWNDSPTIME,0);
}
/*------------------------------------------------------------------------
 * 隍蚗掛↓喇禱邞堎  礎倳凝�梮肩彌鴃麚熁思�    
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_SendMsg( int meindex, int pindex, int tblnum)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buff[128];

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	if( NPC_Util_GetStrFromStrWithDelim( argstr, msgtable[tblnum].arg,
		buff, sizeof( buff)) == NULL )
	{
		strcpy( buff, msgtable[tblnum].defMsg);
	}
	if( buff[0] != '\0' ) {
		CHAR_talkToCli( pindex, meindex, buff,
						CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}
/*------------------------------------------------------------------------
 * 蠈趙摯硞勗隍蚗掛↓喇禱邞堎��
 *----------------------------------------------------------------------*/
static void NPC_Sysinfo_SendMsgToAll( int meindex, char *msg)
{
    int     i;
    int     playernum = CHAR_getPlayerMaxNum();

    for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
			CHAR_talkToCli( i, meindex, msg,
						CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
        }
	}
}



/* 蠈傍す籤溘禱 NO_SEE 勗埰堎凝汔竣凝 */
static void NPC_Sysinfo_All_NoSee( int meindex, int tindex, char *msg ){
	char	buff[256];

	buff[0] = buff[1] = 0;
	/* �羬袬暋碇�   */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		/*     勗埰堎鳹譴 */
		if( strncmp( buff, "on", strlen( buff ) ) == 0 ){
			all_nosee = 1;
			snprintf( buff, sizeof( buff), "眒羲ゐall_nosee ﹝" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}else
		/*     勗埰堎鳹譴 */
		if( strncmp( buff, "off", strlen( buff ) ) == 0 ){
			all_nosee = 0;
			snprintf( buff, sizeof( buff), "眒壽敕all_nosee ﹝" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}
	}else
	if( all_nosee == 0 ){
		snprintf( buff, sizeof( buff), "all_nosee壽敕覂﹝" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "all_nosee羲ゐ覂﹝" );
			CHAR_talkToCli( tindex, meindex, buff,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}

/* 蠈傍す籤溘禱 NO_BODY 勗埰堎凝汔竣凝 */
static void NPC_Sysinfo_All_NoBody( int meindex, int tindex, char *msg ){
	char	buff[256];

	buff[0] = buff[1] = 0;
	/* �羬袬暋碇�   */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		/*     勗埰堎鳹譴 */
		if( strncmp( buff, "on", strlen( buff ) ) == 0 ){
			all_nobody = 1;
			snprintf( buff, sizeof( buff), "眒羲ゐall_nobody ﹝" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}else
		/*     勗埰堎鳹譴 */
		if( strncmp( buff, "off", strlen( buff ) ) == 0 ){
			all_nobody = 0;
			snprintf( buff, sizeof( buff), "眒壽敕all_nobody ﹝" );
			CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
		}
	}else
	if( all_nobody == 0 ){
		snprintf( buff, sizeof( buff), "all_nobody 壽敕覂﹝" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "all_nobody 羲ゐ覂﹝" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}


/* 蠈傍す籤溘禱 兒鳴↓鏤ぁ笰    軘踏鞠堎凝ㄑ */
static void NPC_Sysinfo_Move_Num( int meindex, int tindex, char *msg ){
	char	buff[256];
	int		work;
	buff[0] = buff[1] = 0;

	/* �羬袬暋碇�   */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		work = atoi( buff );
		if( work <= 0 ) {
			work = 1000; /* 媏騍 */
		}
		/*   鄹勗喫躂煦崹  凝媃竘埰 */
		EnemyMoveNum = work;
		snprintf( buff, sizeof( buff), "�繭倞冞畎�%d雄釬﹝",
			EnemyMoveNum );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "珋婓菩�冞畎敘睍靇�%d雄釬﹝",
			EnemyMoveNum );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}



/* 傍す籤溘禱兒鳴↓鏤ぁ笰    禜牁陑③埰凝 */
static void NPC_Sysinfo_Born_Num( int meindex, int tindex, char *msg ){
	char	buff[256];
	int		work;
	buff[0] = buff[1] = 0;

	/* �羬袬暋碇�   */
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE )
	{
		work = atoi( buff );
		if( work <= 0 ) {
			work = 2; /* 媏騍 */
		}
		/*   鄹勗喫躂煦崹牁陑③媃竘埰 */
		one_loop_born = work;
		snprintf( buff, sizeof( buff), "菩�冞畎掬伈囆賹萰�%d﹝",
			one_loop_born );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "珋婓菩�冞畎掬伈囆賹萰�%d﹝",
			one_loop_born );
		CHAR_talkToCli( tindex, meindex, buff,
					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
}



/* 藯凜摯  すす籤溘Щ稃禱峟堎 */
static void NPC_Sysinfo_Enemy_State( int meindex, int tindex, char *msg ){
	char	buff[256];

	if( all_nobody == 0 ){
		snprintf( buff, sizeof( buff), "allnobody 壽敕覂﹝" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "allnobody 羲ゐ覂﹝" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
	if( all_nosee == 0 ){
		snprintf( buff, sizeof( buff), "allnosee 壽敕覂﹝" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}else{
		snprintf( buff, sizeof( buff), "allnosee 羲ゐ覂﹝" );
		CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	}
	snprintf( buff, sizeof( buff),
              "bornnum 珋婓菩�冞畎掬伈囆賹萰�%d﹝",
              one_loop_born );
	CHAR_talkToCli( tindex, meindex, buff,
				CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));

	snprintf( buff, sizeof( buff),
              "movenum 珋婓菩�冞畎敘睍靇�%d雄釬﹝",
              EnemyMoveNum );
	CHAR_talkToCli( tindex, meindex, buff,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));


}
static void NPC_Sysinfo_SetWalkTime( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setWalksendinterval( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"WalkInterval = %d   鏃",getWalksendinterval());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
	
}
static void NPC_Sysinfo_SetCATime( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setCAsendinterval_ms( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"CAsendInterval = %d   鏃",
              getCAsendinterval_ms());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_SetCDTime( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setCDsendinterval_ms( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"CDsendInterval = %d   鏃",
              getCDsendinterval_ms());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_SetOneloop( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setOnelooptime_ms( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"Onelooptime = %d   鏃",
              getOnelooptime_ms());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_TitleRestart( int meindex, int tindex, char *msg)
{
	int		rc;
	char	msgbuf[256];
	
	rc = TITLE_reinitTitleName();
	if( rc) {
		snprintf( msgbuf, sizeof(msgbuf),"黍�﹠穇洁�" );
	}
	else {
		snprintf( msgbuf, sizeof(msgbuf),"楷汜ERROR﹝" );
	}
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}


static void NPC_Sysinfo_SetSaacwrite( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setSaacwritenum( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"Saacwritenum = %d ",getSaacwritenum());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
static void NPC_Sysinfo_SetSaacread( int meindex, int tindex, char *msg)
{
	int interval;
	char	msgbuf[256];
	char	buff[256];
	if( getStringFromIndexWithDelim( msg, " ", 2, buff, sizeof( buff)) == TRUE ) {
		interval = atoi( buff);
		if( interval > 0 ) {
			setSaacreadnum( (unsigned)interval);
		}
	}
	snprintf( msgbuf, sizeof(msgbuf),"Saacreadnum = %d ",getSaacreadnum());
	CHAR_talkToCli( tindex, meindex, msgbuf,
			CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR));
}
