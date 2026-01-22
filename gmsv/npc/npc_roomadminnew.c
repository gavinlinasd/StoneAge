#include "version.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "object.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "net.h"
#include "npcutil.h"
#include "handletime.h"
#include "readmap.h"
#include "npc_roomadminnew.h"
#include "npc_door.h"


/* 
 * 箾  Е盓踏庌 By Kawata
 *
 * 蹄    蜬勗  脾摯諱怳拻  媃倜  誑傲笢敁  竣摯ぁ怳拻埵堁媃
 */


enum{
	/* 酘↓袲で砫璃  ⑹ */
	CHAR_WORKINT_RANGE     = CHAR_NPCWORKINT1,
	/* べ摯騠憍鄹蹬 */
	CHAR_WORKINT_NEXTTIME  = CHAR_NPCWORKINT2,
	/*   崚暵ˇ騠憍ㄑ覕鞳ˊ */
	CHAR_WORKINT_PRINTCLOSETIME = CHAR_NPCWORKINT3,
	/*   碟號喙媃倜  痹 */
	CHAR_WORKINT_SEARCHROOMTIME = CHAR_NPCWORKINT4,
	
};
enum {
	/* 猷覕摯  鞳 */
	CHAR_WORKCHAR_MYNAME   = CHAR_NPCWORKCHAR1,
	/* べ摯  贀 */
	CHAR_WORKCHAR_NEXTROOM = CHAR_NPCWORKCHAR2,
	
};

/* 溘璃す璃精  崚      暵極↓袲*/
enum {
	/* 集  硞摯  鞳 */
	CHAR_WORKCHAR_ADMNAME = CHAR_NPCWORKCHAR1,
};


/* 溢↓淏啞栝騷鳴禱氶  埰堎溢贖畛袲哱惉�莿s2/src/lss 凝�梩勒繡紮黃� */
#define     NPC_ROOMADMINNEW_DATADIR        "roomadmin/"
/* 溢↓淏啞栝騷鳴摯錚耦碳 */
#define     NPC_ROOMADMINNEW_EXTENSION      ".room"
/* 溘璃す璃精溢↓淏啞栝騷鳴摯錚耦碳    璃睽溘惉   */
#define     NPC_ROOMADMINNEW_RANKING_EXTENSION      ".score"
#define     NPC_ROOMADMINNEW_RANKING_EXTENSION_TMP  ".score.tmp"
/*   禜  睊榆溘璃す璃精啞栝騷鳴 */
#define     NPC_ROOMADMINNEW_SCOREFILE      ".score"
/* 騠誧憍堇ㄑ覕鞳凝�梮肩彌鴃麚� */
#define     NPC_ROOMADMINNEW_PRINTCLOSETIME_DEFAULT     5

#define     NPC_ROOMADMINNEW_MSG_CLOSE_BEFORE       \
"%s %s %s腔魂雄ㄛ諍砦��%d煦ヶ﹝"
#define     NPC_ROOMADMINNEW_MSG_CLOSE              \
"﹝﹝﹝眒諍砦﹝"
#define     NPC_ROOMADMINNEW_MSG_NONE               \
"竭褫洇掛棒羶衄�侄騊繩蕉鉾鼯萍倞芋�"
#define     NPC_ROOMADMINNEW_MSG_CONGRATULATION     \
"鳩炰蠟ㄐ%s鳳腕滇潔腔垀衄�見�%s腔踢ヴ�垓膨頗褖桷芊�%s③砃奪燴埜戙恀滇潔做瘍﹝"
#define     NPC_ROOMADMINNEW_MSG_PRINTPASSWD_NONE    \
"蚕黺斕帤厥衄滇潔秪森羶衄做瘍﹝"
#define     NPC_ROOMADMINNEW_MSG_DSPPASSWD1          \
"斕腔滇潔岆 %s %s %s﹝"
#define     NPC_ROOMADMINNEW_MSG_DSPPASSWD2          \
"滇潔腔做瘍岆 ☆%s ★﹝硐猁婓勤覂藷佽趕奀婓迵帣樓奻☆...★ㄛ梗�侗芨散遞�賸遄ㄐ"
#define     NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION_NONE        \
"掛�桮躉貕紗慲齱�"
#define     NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION1     \
"諉狟懂腔滇潔岆 %s %s %s﹝"
#define     NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION2     \
"諍砦�梛� %d:%d ﹝善諍砦�梉肯劂墓�%2d苤奀%02d煦﹝"
#define     NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION3     \
"郔腴腕梓踢塗岆 %s ﹝"

#define     NPC_ROOMADMINNEW_MSG_RANKING_INFO       \
"涴岆詢塗腕梓氪齬俴埤﹝"
#define     NPC_ROOMADMINNEW_MSG_RANKING            \
"Rank %2d:%4d/%02d/%02d %8d GOLD %s%s "

/* 喫摯NPC摯隍蚗掛↓喇褗 */
#define     NPC_ROOMADMINNEW_MSGCOLOR           CHAR_COLORWHITE
#define     NPC_RANKING_MSGCOLOR                CHAR_COLORWHITE
#define		NPC_ROOMADMINNEW_2WEEK				60*60*24*14

static void NPC_RoomAdminNew_DspNextAuction( int meindex, int talkerindex);
static void NPC_RoomAdminNew_DspRoomPasswd( int meindex, int adminindex, int talkerindex);
static BOOL NPC_RoomAdminNew_SetNextAuction( int meindex);
static BOOL NPC_RoomAdminNew_WriteDataFile( int meindex, int pindex, char *roomname,
											char *passwd, BOOL flg);
static void NPC_RoomAdminNew_CreatePasswd( char *passwd, int passlen);
static void NPC_RoomAdminNew_SendMsgThisFloor( int index, char *buff);
static int NPC_RoomAdminNew_SearchPlayerHasMostMoney( int meindex);
static void NPC_RoomAdminNew_CloseProc( int meindex);
static int NPC_RoomAdminNew_WriteScoreFile( int meindex, int pindex);
static void NPC_RoomAdminNew_ReadScoreFile( int meindex, int talkerindex, 
											char *admname);

/************************************************
 * 疐趵窐  
 ************************************************/
BOOL NPC_RoomAdminNewInit( int meindex )
{
	int     i;
	char    buf[256];
	char    argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	/* 猷覕摯集酮摯蠈  摯迶囮誑③  窀誑堎竘ぁ喫摯NPC毀軘埬踏躂埵笢 */
	for( i = 3; ; i ++ ) {
		if( getStringFromIndexWithDelim(argstr, "|" , i , buf,sizeof(buf) )) {
			if( NPC_DoorSearchByName( buf) < 0 ) {
				print( "admin not created.婓躇鎢藷酕疑眳ヶ珂脹渾﹝\n");
				return FALSE;
			}
		}
		else{ 
			break;
		}
	}
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	/* 疐趵撬窐   */
	
	/* type掛蚗哱 */
	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEROOMADMIN);
	/*   嗚  ⑹噁   */
	getStringFromIndexWithDelim(argstr, "|" , 1 , buf,sizeof(buf) );
	CHAR_setWorkInt( meindex , CHAR_WORKINT_RANGE , atoi( buf ));
	/* 猷覕摯  鞳噁   */
	if( !getStringFromIndexWithDelim(argstr, "|" , 2 , buf,sizeof(buf) )) {
		print( "arg err.admin not created.\n");
		return( FALSE);
	}
	CHAR_setWorkChar( meindex , CHAR_WORKCHAR_MYNAME , buf);
	
	/*   崚暵ˇ騠憍ㄑ覕鞳ˊ疐趵撬 */
	CHAR_setWorkInt( meindex, CHAR_WORKINT_PRINTCLOSETIME, 0);
	
	/* べ摯  贀敁酘↓袲で砫璃楣桍鄹蹬禱掛蚗哱 */
	NPC_RoomAdminNew_SetNextAuction( meindex);
	
	return TRUE;
}
/************************************************
 * 鳴↓鏤窐  
 * 蠙怮③媃  梣毀鼠庌埵勗    ぁ埵�妡缺笱倗苺�
 * ぁ忒憪葡勗兒覕    勗埰堎痀
 ************************************************/
void NPC_RoomAdminNewLoop( int meindex )
{
	int     hun;
	/* 忒媃忒楣桍鄹蹬誑鋆竘埱趙笢埵凝埱倜�湴�  媃埵堁埰 */
	if( CHAR_getWorkInt( meindex, CHAR_WORKINT_NEXTTIME) == 0 ) {
		/* 鞳樊號喙媃倜  誑鄖窇�伓像鵗ぴ耒ば� */
		struct  tm      tm1,tm2;
		time_t          t;
		memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
		t = CHAR_getWorkInt( meindex, CHAR_WORKINT_SEARCHROOMTIME);
		memcpy( &tm2, localtime( &t), 
								sizeof( tm2));
		if( tm1.tm_year != tm2.tm_year ||
			tm1.tm_mday != tm2.tm_mday ||
			tm1.tm_mon != tm2.tm_mon )
		{
			NPC_RoomAdminNew_SetNextAuction( meindex);
		}
		return;
	}
	
	hun = ceil((double)(CHAR_getWorkInt( meindex, CHAR_WORKINT_NEXTTIME) - 
		   NowTime.tv_sec) / 60.0);
	/* 忒媃忒摯鄹摯諱勗hun誑悃騷圖筒摯鄹毀兔勗  埰 */
	if( hun < 0 ) hun = 0;
	
	if( hun < NPC_ROOMADMINNEW_PRINTCLOSETIME_DEFAULT ) {
		/* 騠誧憍堇閟�奀槾耒�倜�掁橡遙批拉誨Ｍ唒﹉疋呁諄褉伝灄� */
		if( hun != CHAR_getWorkInt( meindex, CHAR_WORKINT_PRINTCLOSETIME)) {
			char    buff[256];
			if( hun != 0 ) {
				char    buf2[1024];
				char	msgbuff[16];
				char    argstr2[NPC_UTIL_GETARGSTR_BUFSIZE];
				int		doorindex;

				/*   碟攷  噁   */
				doorindex = NPC_DoorSearchByName( CHAR_getWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM));
				NPC_Util_GetArgStr( doorindex, argstr2, sizeof( argstr2));
				getStringFromIndexWithDelim( argstr2, "|" , 9 , 
											 buf2,sizeof(buf2) );
				/*   碟攷  */
				if( strlen(CHAR_getChar( doorindex, CHAR_NAME) ) != 0 ) {
					strcpy( msgbuff, "瘍弅");
				}
				else {
					msgbuff[0] = '\0';
				}
				/* ˇ騠誧憍堇ㄑ覕鞳ぁ埰ˊ */
				snprintf( buff, sizeof(buff), 
							NPC_ROOMADMINNEW_MSG_CLOSE_BEFORE, 
							buf2,
							CHAR_getChar( doorindex, CHAR_NAME),
							msgbuff,
							hun);
				
				CHAR_setWorkInt( meindex, CHAR_WORKINT_PRINTCLOSETIME, hun);
			}
			else {
				/* ˇ騠誧憍堇竘媃倜ˊ */
				strcpy( buff, NPC_ROOMADMINNEW_MSG_CLOSE);
				CHAR_setWorkInt( meindex, CHAR_WORKINT_PRINTCLOSETIME, 0);
			}
			/* 喫摯啞痲囮勗笢堎硞蠈劂勗邞嚭 */
			NPC_RoomAdminNew_SendMsgThisFloor( meindex, buff);
		}
	}
	/* 騠誧憍埱倜鄹摯窐   */
	if( hun == 0 ) {
		NPC_RoomAdminNew_CloseProc( meindex);
	}
}

/************************************************
 *   媃凝崹�桫憶袬敯啄�  
 ************************************************/
void NPC_RoomAdminNewTalked( int meindex , int talkerindex , char *msg ,
							 int color )
{
	
	/* 鏤畛騷媕↓誑迶囮悃璃摯1精惉蚗迶雄  埵��  伀埰堎�� */
	if(NPC_Util_CharDistance( talkerindex, meindex ) > 1)return;
	
	/*   媃凝崹�桫憶跁郅�  碟禱韁堇趙笢倜�梌伂弧央灃鯇�  崚埰堎 */
	/*NPC_RoomAdminNew_DspRoomPasswd( meindex, talkerindex);*/
	
	/* べ樊楣桍鄹蹬��  碟禱虭竣 */
	NPC_RoomAdminNew_DspNextAuction( meindex, talkerindex);
}
/*-----------------------------------------------
 * 騠誧憍�桫憶袬敯啄�  
 *----------------------------------------------*/
static void NPC_RoomAdminNew_CloseProc( int meindex)
{
	int     pindex;
	char    buff[256];
	char    pname[32];
	int     failed = TRUE;
	
	/* 郖  堁鉣禱蘗��  埱趙笢堎鏤畛騷媕↓禱潺埰 */
	pindex = NPC_RoomAdminNew_SearchPlayerHasMostMoney( meindex);
	/* 器騍榆囮惉 */
	if( pindex != -1) {
		/* 兒樊ぁ  崹堎鳴↓鏤ぁ埰 */
		while( 1) {
			char    passwd[9];
			int		doorindex;
			doorindex = NPC_DoorSearchByName( 
							CHAR_getWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM));
			/* 蚕筒極↓迶禱牁埬埰堎 */
			NPC_RoomAdminNew_CreatePasswd( passwd, sizeof( passwd));
			/* 啞栝騷鳴勗喫摯硞摯    す↓�暔褐珝�  ��  碟  �楂伂怔屏及�  資 */
			if( NPC_RoomAdminNew_WriteDataFile( meindex, pindex,
							CHAR_getWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM),
												passwd, 1)
				== FALSE) 
			{
				break;
			}
			/* 迶囮勗蚕筒禱掛蚗哱 */
			if( NPC_DoorSetPasswd( doorindex, passwd) == -1 ) {
				break;
			}
			/*   嗚趵藦禱掛蚗哱 */
			CHAR_setWorkInt( doorindex, CHAR_WORKDOOREXPIRETIME, 
							CHAR_getWorkInt( meindex, CHAR_WORKINT_NEXTTIME)
							+ NPC_ROOMADMINNEW_2WEEK);
			/*   睊榆溘璃す璃精鏍攝蚗袲∣啞栝騷鳴怳拻  陑 */
			NPC_RoomAdminNew_WriteScoreFile( meindex, pindex);

			CHAR_setInt( pindex, CHAR_GOLD, 0);

			CHAR_send_P_StatusString( pindex , CHAR_P_STRING_GOLD);
			
			/* 堁誧ぁ敁竣∪隍蚗掛↓喇 */
			strcpy( pname, CHAR_getChar( pindex, CHAR_NAME));
			snprintf( buff, sizeof(buff), 
						NPC_ROOMADMINNEW_MSG_CONGRATULATION, pname, pname, pname);
			failed = FALSE;
			break;
		}
	}
	
	/* 器騍榆圖で竘倜毀窐  謄  摯鄹 */
	if( failed) {
		/* 啞栝騷鳴凝�梜蟦�  碟摯攷  禱觛埰 */
		NPC_RoomAdminNew_WriteDataFile( meindex, -1,
					CHAR_getWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM),
										NULL, 0);
		/* 器騍榆毀笢竘鞠庌隍蚗掛↓喇 */
		snprintf( buff, sizeof(buff), 
				NPC_ROOMADMINNEW_MSG_NONE);
	}
	/* 喫摯啞痲囮勗笢堎硞蠈劂勗邞嚭 */
	NPC_RoomAdminNew_SendMsgThisFloor( meindex, buff);
	
	/* べ摯  贀敁酘↓袲で砫璃楣桍鄹蹬禱掛蚗哱 */
	NPC_RoomAdminNew_SetNextAuction( meindex);
	
}
/*-----------------------------------------------
 * 郖  堁鉣禱蘗��  埱趙笢堎鏤畛騷媕↓禱潺埰
 *----------------------------------------------*/
static int NPC_RoomAdminNew_SearchPlayerHasMostMoney( int meindex)
{
	int     i,j, range;
	int     fl,x,y, gold;
	int     doorindex;
	int     topindex;
	
	fl = CHAR_getInt( meindex, CHAR_FLOOR);
	x  = CHAR_getInt( meindex, CHAR_X);
	y  = CHAR_getInt( meindex, CHAR_Y);
	range = CHAR_getWorkInt( meindex , CHAR_WORKINT_RANGE);
	topindex = -1;
	
	/*   碟摯  鞳凝��  黖  睊鉣滌禱ぅ迋堎 */
	doorindex = NPC_DoorSearchByName( CHAR_getWorkChar( meindex, 
														CHAR_WORKCHAR_NEXTROOM));
	gold = atoi( CHAR_getWorkChar( doorindex, CHAR_WORKDOORGOLDLIMIT));
	/*   ⑹  勗笢堎す籤溘摯倳禱ぅ迋堎 */
	for( i = x-range ; i <= x+range ; i++ ){
		for( j = y-range ; j <= y+range ; j ++ ){
			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j); object; 
				 object = NEXT_OBJECT(object) ) 
			{
				int objindex = GET_OBJINDEX( object);
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
					int     index;
					index = OBJECT_getIndex(objindex);
					if( CHAR_getInt( index,
								 CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )
					{
						if( CHAR_getInt( index, CHAR_GOLD) >= gold ) {
							if( topindex == -1 ) {
								topindex =  index;
							}
							else {
								/* 哱蚗鏤摯硞敁堁鉣  吨 */
								int nowgold,topgold;
								nowgold = CHAR_getInt( index, CHAR_GOLD );
								topgold = CHAR_getInt( topindex, CHAR_GOLD);
								if( nowgold > topgold ) {
									topindex = index;
								}
								else if( nowgold == topgold ) {
									/* 郖灉埵�梑謄牴嚜伊萋倵� */
									int nowlv, toplv;
									nowlv = CHAR_getInt( index, CHAR_LV );
									toplv = CHAR_getInt( topindex, CHAR_LV);
									if( nowlv < toplv ) {
										topindex = index;
									}
									/* 鼠躂ぁ忒搣鋆媃埵笢庌埵�梫翹�譫堜啋崨*/
									/* index誑袸摯硞誑з踝埱敁箾媋煦崹汔 */
									else if( nowlv == toplv ) {
										if( RAND( 0,1)) {
											topindex = index;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return( topindex);
}
/*-----------------------------------------------
 * 炩擭踏躂倜index摯啞痲囮勗笢堎鏤畛騷媕↓蠈劂勗
 * 隍蚗掛↓喇禱邞嚭埰堎��
 *----------------------------------------------*/
static void NPC_RoomAdminNew_SendMsgThisFloor( int index, char *buff)
{
	int     i, fl;
	int     playernum = CHAR_getPlayerMaxNum();
	
	fl = CHAR_getInt( index, CHAR_FLOOR);
	for( i = 0; i < playernum; i ++ ) {
		if( CHAR_getInt( i, CHAR_FLOOR) == fl ) {
			/*   摯倜誧鏤畛騷媕↓凝鏍攝蚗袲 */
			if( CHAR_getInt( i, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
				CHAR_talkToCli( i, index, buff, NPC_ROOMADMINNEW_MSGCOLOR);
			}
		}
	}
}
/*-----------------------------------------------
 * 蚕筒極↓迶禱牁埬埰堎�菘asslen摯-1  棬誑軘�桫戰瞿�
 * makecdn.c摯毞�彸誨�
 *----------------------------------------------*/
static void NPC_RoomAdminNew_CreatePasswd( char *passwd, int passlen)
{
#define NPC_ROOMADMINNEW_RAND()    ( (rand() /256) % 32768 )       /* 256ぁ鄎堎摯毀崷倳摯  艉摯倜誧 */
	int     i;
	char *candidates[]= {
		"A","B","D",   "E","F","G","H",   "J","K","L","M",   "N","P","Q","R",
		"T","W","X","Y",   "a","b","d","e",   "f","g","m","n",   "q","r","t",
		"0","1","2",   "3","4","5","6",   "7","8","9"
	};
	strcpy( passwd , "");
	for(i = 0;i < passlen -1 ; i++){
		strcat( passwd , candidates[ NPC_ROOMADMINNEW_RAND() % (sizeof( candidates)/sizeof(candidates[0])) ] );
	}
#undef NPC_ROOMADMINNEW_RAND
}
/*-----------------------------------------------
 * 集  硞暵摯啞栝騷鳴勗溢↓淏禱怳拻  資
 *
 * 啞栝騷鳴  毀"滇潔腔靡趼.room"
 * 禱窅竣��
 *
 * 啞栝騷鳴摯啞б↓悃蚗哱毀��
 * cdkey|charaname|passwd|expiretime
 *
 *
 * 礎倳摯flg 誑兒煦埱倜�梋及�  陑�搢葽笪�倜�桹豽踽抳鼢舊�
 *   堇偯※TRUE※埬  
 *         FALSE:笰�桴挺偭�  
 *----------------------------------------------*/
static BOOL NPC_RoomAdminNew_WriteDataFile( int meindex, int pindex, 
											char *roomname,
											char *passwd, int flg)
{
	char    *cdkey;
	char    charaname[32*2];
	char    expiretime[64];
	char    filename[128];
	char    outbuf[1024];
	FILE    *fp_src;
	
	/* 啞栝騷鳴  禱牁埬 */
	strcpy( filename, NPC_ROOMADMINNEW_DATADIR);
	/*strcat( filename, CHAR_getWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM));*/
	strcat( filename, roomname);
	strcat( filename, NPC_ROOMADMINNEW_EXTENSION);

	if( flg) {
		/*   睊媃倜硞摯    す↓ */
        //getcdkeyFromCharaIndex( pindex, cdkey, sizeof(cdkey ));
		cdkey = CHAR_getChar( pindex, CHAR_CDKEY);
		/*   睊媃倜硞摯す籤溘   */
		makeEscapeString(  CHAR_getChar( pindex, CHAR_NAME),
							charaname, sizeof( charaname));
		/* 趵藦憍躂鄹蹬 */
		snprintf( expiretime, sizeof( expiretime), "%d", 
					CHAR_getWorkInt( meindex, CHAR_WORKINT_NEXTTIME)
					+ NPC_ROOMADMINNEW_2WEEK   /* 兕棣蹬   */
					);

		/* 溢惉蕾淏ぁ�岓彸� */
		snprintf( outbuf, sizeof( outbuf), "%s|%s|%s|%s|",
					cdkey, charaname, passwd, expiretime);
		fp_src = fopen( filename, "w");
		if( !fp_src) {
			print( "[%s:%d] File Open Error\n", __FILE__, __LINE__);
			return FALSE;
		}
		fputs( outbuf, fp_src);
		fclose( fp_src);
	}
	else {
		unlink( filename);
	}
	
	return TRUE;
	
}
/*-----------------------------------------------
 * 溢↓淏啞栝騷鳴禱  庌ぁ器騍埰堎  碟誑堣躂��
 * 溢↓淏禱氶  媃趙蒍埰��
 *----------------------------------------------*/
BOOL NPC_RoomAdminNew_ReadFile( char *roomname, NPC_ROOMINFO *data)
{
	FILE    *fp;
	char    line[1024];
	char    buf2[128];
	char    filename[128];
	
	/* 啞栝騷鳴  禱牁埬 */
	strcpy( filename, NPC_ROOMADMINNEW_DATADIR);
	strcat( filename, roomname);
	strcat( filename, NPC_ROOMADMINNEW_EXTENSION);
	
	fp = fopen( filename, "r");
	if( !fp) return FALSE;
	fgets( line, sizeof( line), fp);
	/* 溢↓淏掛蚗哱 */
	getStringFromIndexWithDelim( line, "|" , 1 , buf2,sizeof(buf2) );
	strcpy( data->cdkey, buf2);
	getStringFromIndexWithDelim( line, "|" , 2 , buf2,sizeof(buf2) );
	strcpy( data->charaname, makeStringFromEscaped( buf2));
	getStringFromIndexWithDelim( line, "|" , 3 , buf2,sizeof(buf2) );
	strcpy( data->passwd, buf2);
	getStringFromIndexWithDelim( line, "|" , 4 , buf2,sizeof(buf2) );
	if( strcmp( buf2, "max") == 0 ) {
		data->expire = 0xffffffff;
	}
	else {
		data->expire = atoi( buf2);
	}
	fclose( fp);
	return TRUE;
}
/*-----------------------------------------------
 * べ摯酘↓袲で砫璃摯鄹蹬敁  碟  禱極↓袲勗掛蚗哱埰堎
 *----------------------------------------------*/
static BOOL NPC_RoomAdminNew_SetNextAuction( int meindex)
{
	int     i, j;
	int     hitindex = -1;
	int     charnum = CHAR_getCharNum();
	char    argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	struct  tm  tmwk;
	
	/*     埵汔禱噁  埰堎*/
	memcpy( &tmwk, localtime( ( time_t *)&NowTime.tv_sec), sizeof( tmwk));
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	/* 迶囮禱號喙埰堎�腇PC_DoorSearchByName()禱窅埱倜�梋銩笭糗�
	 *    藗誑蘗�忐耒遜奏尺汝副戊薱襐鵷瓥漡牰�
	 */
	for( i = CHAR_getPlayerMaxNum()+ CHAR_getPetMaxNum(); 
		 i < charnum;
		 i ++ )
	{
		if( CHAR_getInt( i, CHAR_WHICHTYPE) != CHAR_TYPEDOOR) {
			continue;
		}
		if( CHAR_getWorkInt( i, CHAR_WORKDOORPASSFLG) == 0 ) {
			continue;
		}
		/*   鞳誑集酮摯忒摯凝鏍攝蚗袲埰堎 */
		for( j = 3; ; j ++ ) {
			int rc;
			char    *doorname;
			char    buf[256];
			int     dataexist;
			NPC_ROOMINFO data;
			
			rc = getStringFromIndexWithDelim( argstr, "|" , j,
												buf,sizeof(buf) );
			if( rc != TRUE ) {
				break;
			}
			doorname = CHAR_getWorkChar( i, CHAR_WORKDOORNAME);
			/* 迶囮  誑郖窙媃倜 */
			if( strcmp( buf, doorname ) != 0 ) {
				continue;
			}
			/*     誑郖窙埰堎 */
			if( atoi( CHAR_getWorkChar( i, CHAR_WORKDOORWEEK)) != tmwk.tm_wday) {
				break;
			}
			/* 郖伀疐趵撬 */
			memset( &data, 0, sizeof( data));
			/* 溢↓淏啞栝騷鳴凝�梫腹灊�禱  資 */
			if( NPC_RoomAdminNew_ReadFile( doorname, &data) ) {
				struct tm   tmdoor;
				/* 喫喫ぁ趵藦憍躂鏍攝蚗袲禱盓埱趙盓堎��
				 * 趵藦誑憍躂趙笢倜�桹豽踽抳屪契舊笪彸�
				 * 蹄  黍勗堣竘堇喫摯鳴↓鏍璃玊堎痀毀埵笢敁  竣��
				 */
				if( data.expire < NowTime.tv_sec) {
					NPC_RoomAdminNew_WriteDataFile( meindex, -1,doorname, NULL, 0);
					/* 迶囮忒拻躂笢勗*/
					NPC_DoorSetPasswd( i, "8hda8iauia90494jasd9asodfasdf89");
					CHAR_setWorkInt( i, CHAR_WORKDOOREXPIRETIME, 0xffffffff);
					
					break;
				}
				/* ぽ  ぁ憍躂堎  贀埵��     */
				memcpy( &tmdoor, localtime( ( time_t *)&data.expire), sizeof( tmdoor));
				if( tmdoor.tm_mon != tmwk.tm_mon ||
					tmdoor.tm_mday != tmwk.tm_mday)
				{
					break;
				}
				dataexist = TRUE;
			}
			else {
				dataexist = FALSE;
			}
			/* 藯凜摯鄹蹬源堇  ぁ堣堎痀�� */
			if( tmwk.tm_hour > atoi( CHAR_getWorkChar( i, CHAR_WORKDOORHOUR))){
				break;
			}
			if( tmwk.tm_hour == atoi( CHAR_getWorkChar( i, CHAR_WORKDOORHOUR))) {
				if( tmwk.tm_min >= atoi( CHAR_getWorkChar( i, CHAR_WORKDOORMINUTE))) {
					break;
				}
			}
			/* 喫喫竘ぁ拻倜�梉諏隙夫�囀喫摯迶囮毀べ楣桍勗③堎瘀氶誑
			 * 堣堎敁笢竣痀�蛦蟤噢旅欶側商鯓形褌鯆慪濛邴彸瞿�
			 */
			if( hitindex == -1 ) {
				hitindex = i;
			}
			else {
				/* 楣桍鄹蹬誑傲�呁椯婸�韁堇�桫噱胱俶蟲誸迮兮偵伏嗦ぐ俅� */
				if( atoi( CHAR_getWorkChar( hitindex, CHAR_WORKDOORHOUR)) 
					> atoi( CHAR_getWorkChar( i, CHAR_WORKDOORHOUR)))
				{
					hitindex = i;
				}
				else if( atoi( CHAR_getWorkChar( hitindex, CHAR_WORKDOORHOUR)) 
						== atoi( CHAR_getWorkChar( i, CHAR_WORKDOORHOUR)))
				{
					if( atoi( CHAR_getWorkChar( hitindex, CHAR_WORKDOORMINUTE)) 
						> atoi( CHAR_getWorkChar( i, CHAR_WORKDOORMINUTE)))
					{
						hitindex = i;
					}
					else if( atoi( CHAR_getWorkChar( hitindex, CHAR_WORKDOORMINUTE)) 
						== atoi( CHAR_getWorkChar( i, CHAR_WORKDOORMINUTE)))
					{
						NPC_ROOMINFO    datawk;
						int     hitindexdataexist;
						hitindexdataexist = NPC_RoomAdminNew_ReadFile( 
									CHAR_getWorkChar( hitindex, CHAR_WORKDOORNAME),
									&datawk);
						if( hitindexdataexist < dataexist ) {
							hitindex = i;
						}
						/* 喫喫竘ぁ  堎源竣埵�殏じ迺ndex摯竘竘敁埰堎��*/
						
					}
				}
			}
		}
	}
	if( hitindex > 0 ) {
		/* 楣桍鄹蹬禱掛蚗哱 */
		tmwk.tm_sec = 0;
		tmwk.tm_min = atoi( CHAR_getWorkChar( hitindex, CHAR_WORKDOORMINUTE));
		tmwk.tm_hour = atoi( CHAR_getWorkChar( hitindex, CHAR_WORKDOORHOUR));
		CHAR_setWorkInt( meindex, CHAR_WORKINT_NEXTTIME, mktime( &tmwk));
		/*   碟  禱掛蚗哱 */
		CHAR_setWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM,
						 CHAR_getWorkChar( hitindex, CHAR_WORKDOORNAME));
		
	}
	else {
		CHAR_setWorkInt( meindex, CHAR_WORKINT_NEXTTIME, 0);
		/*   碟  禱掛蚗哱 */
		CHAR_setWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM,"" );
	
	}
	/* 號喙鄹蹬禱掛蚗哱 */
	CHAR_setWorkInt( meindex, CHAR_WORKINT_SEARCHROOMTIME, NowTime.tv_sec);
	return( TRUE);
}
/*-----------------------------------------------
 *   媃凝崹�桫憶跁郅�  碟禱韁堇趙笢倜�梌伂弧央灃鯇�  崚埰堎
 *----------------------------------------------*/
static void NPC_RoomAdminNew_DspRoomPasswd( int index, int adminindex, int talkerindex)
{
	int     i, rc, flg = FALSE;
	char    argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char    argstr2[NPC_UTIL_GETARGSTR_BUFSIZE];
	char    *cdkey;
	char    buf[128];
	char	buff[16];
	char    buf2[1024];

	NPC_ROOMINFO    info;

    //getcdkeyFromCharaIndex( talkerindex, cdkey, sizeof(cdkey));
	cdkey = CHAR_getChar( talkerindex, CHAR_CDKEY);
	
	NPC_Util_GetArgStr( adminindex, argstr, sizeof( argstr));
	for( i = 3; ; i ++ ) {
		if( getStringFromIndexWithDelim(argstr, "|" , i , buf,sizeof(buf) )) {
			rc = NPC_RoomAdminNew_ReadFile( buf, &info);
			if( rc == TRUE ) {
				/* CDKEY 敁す籤溘摯  鞳誑郖窙媃倜�� */
				if( strcmp( info.cdkey, cdkey) == 0 &&
					strcmp( info.charaname, 
							CHAR_getChar( talkerindex, CHAR_NAME)) == 0 )
				{
					int doorindex;
					char    msgbuf[256];
					/*   碟攷  噁   */
					doorindex = NPC_DoorSearchByName( buf);
					NPC_Util_GetArgStr( doorindex, argstr2, sizeof( argstr2));
					getStringFromIndexWithDelim( argstr2, "|" , 9 , 
												 buf2,sizeof(buf2) );
					/*   碟攷  */
					if( strlen(CHAR_getChar( doorindex, CHAR_NAME) ) != 0 ) {
						strcpy( buff, "瘍弅");
					}
					else {
						buff[0] = '\0';
					}
					
					/* 隍蚗掛↓喇掛蚗哱 */
					snprintf( msgbuf, sizeof( msgbuf), 
								NPC_ROOMADMINNEW_MSG_DSPPASSWD1,
								buf2,
								CHAR_getChar( doorindex, CHAR_NAME),
								buff
							);
					/* 隍蚗掛↓喇邞嚭 */
					CHAR_talkToCli( talkerindex, index,msgbuf, 
									NPC_ROOMADMINNEW_MSGCOLOR);

					/* 隍蚗掛↓喇掛蚗哱 */
					snprintf( msgbuf, sizeof( msgbuf), 
								NPC_ROOMADMINNEW_MSG_DSPPASSWD2,
								CHAR_getWorkChar(doorindex, CHAR_WORKDOORPASSWD)
							);
					/* 隍蚗掛↓喇邞嚭 */
					CHAR_talkToCli( talkerindex, index,msgbuf, 
									NPC_ROOMADMINNEW_MSGCOLOR);
					flg = TRUE;
				}
			}
		}
		else {
			break;
		}
	}
	if( !flg) {
		CHAR_talkToCli( talkerindex, index,
						NPC_ROOMADMINNEW_MSG_PRINTPASSWD_NONE,
						NPC_ROOMADMINNEW_MSGCOLOR);
	}
}
/*-----------------------------------------------
 * べ樊楣桍鄹蹬��  碟禱虭竣
 *----------------------------------------------*/
static void NPC_RoomAdminNew_DspNextAuction( int meindex, int talkerindex)
{
	char    msgbuf[2048];
	time_t  closetime;
	
	closetime = CHAR_getWorkInt( meindex, CHAR_WORKINT_NEXTTIME);
	/* べ摯楣桍鄹蹬誑鋆竘埱趙笢埵崹躂�氿曇�竘埱趙竘鞠庌�菇蓿肩彌鴃麚�*/
	if( closetime == 0 ) {
		snprintf( msgbuf, sizeof( msgbuf),
					NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION_NONE
				);
		CHAR_talkToCli( talkerindex, meindex,msgbuf, NPC_ROOMADMINNEW_MSGCOLOR);
	}
	else {
		int     doorindex;
		char    argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
		char    explain[1024];      /*    */
		char	buff[16];
		struct  tm tmwk;
		int     difftime;
		/*   碟攷  禱噁   */
		doorindex = NPC_DoorSearchByName( 
					CHAR_getWorkChar( meindex, CHAR_WORKCHAR_NEXTROOM));
		NPC_Util_GetArgStr( doorindex, argstr, sizeof( argstr));
		getStringFromIndexWithDelim(argstr, "|" , 9 , explain,sizeof(explain));
		/*   碟攷  */
		if( strlen(CHAR_getChar( doorindex, CHAR_NAME) ) != 0 ) {
			strcpy( buff, "瘍弅");
		}
		else {
			buff[0] = '\0';
		}
		snprintf( msgbuf, sizeof( msgbuf),
					NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION1,
					explain,
					CHAR_getChar( doorindex, CHAR_NAME),
					buff
				);
		CHAR_talkToCli( talkerindex, meindex,msgbuf, NPC_ROOMADMINNEW_MSGCOLOR);
		/* 騠誧憍堇鄹蹬 */
		memcpy( &tmwk, localtime( &closetime), sizeof( tmwk));
		difftime = closetime - NowTime.tv_sec;
		snprintf( msgbuf, sizeof( msgbuf),
					NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION2,
					tmwk.tm_hour,
					tmwk.tm_min,
					difftime / 3600,
					(difftime % 3600) / 60
				);
		CHAR_talkToCli( talkerindex, meindex,msgbuf, NPC_ROOMADMINNEW_MSGCOLOR);
		
		/* き藦鉣滌 */
		snprintf( msgbuf, sizeof( msgbuf),
					NPC_ROOMADMINNEW_MSG_DSPNEXTAUCTION3,
					CHAR_getWorkChar( doorindex, CHAR_WORKDOORGOLDLIMIT)
				);
		CHAR_talkToCli( talkerindex, meindex,msgbuf, NPC_ROOMADMINNEW_MSGCOLOR);
	}
	
}
/*-----------------------------------------------
 *   睊榆溘璃す璃精鏍攝蚗袲∣啞栝騷鳴怳拻  陑
 *
 * 啞栝騷鳴  毀"奪燴氪腔靡趼.score"
 *   璃睽溘惉啞栝騷鳴  毀"奪燴氪腔靡趼.score.tmp"
 *
 * 啞栝騷鳴啞б↓悃蚗哱毀��
 * gold|time|cdkey|charaname|owntitle
 * ぁ兒兔菜竘ぁ��
 *
 *   堇偯※ 埬  ※溘璃す璃精笰痾凝蒍埰�蛪翹�す璃精勗  �欷肋篫寰禸舝�
 *          謄  ※-1
 *----------------------------------------------*/
static int NPC_RoomAdminNew_WriteScoreFile( int meindex, int pindex)
{
#define NPC_RANKING_DEFAULT     10
	int     ret = 0;
	char    *cdkey;
	char    charaname[32*2],owntitle[32*2];
	char    filename_dest[128],filename[128];
	char    outbuf[1024], line[1024];
	FILE    *fp_src, *fp_dest;

	/* 啞栝騷鳴  禱牁埬 */
	strcpy( filename, NPC_ROOMADMINNEW_DATADIR);
	strcat( filename, CHAR_getWorkChar( meindex, CHAR_WORKCHAR_MYNAME));
	strcpy( filename_dest, filename);
	strcat( filename, NPC_ROOMADMINNEW_RANKING_EXTENSION);
	strcat( filename_dest, NPC_ROOMADMINNEW_RANKING_EXTENSION_TMP);
	
	/*   睊媃倜硞摯    す↓ */
	//getcdkeyFromCharaIndex( pindex , cdkey , sizeof(cdkey ));
	cdkey = CHAR_getChar( pindex, CHAR_CDKEY);
	
	/*   睊媃倜硞摯す籤溘   */
	makeEscapeString(  CHAR_getChar( pindex, CHAR_NAME),
						charaname, sizeof( charaname));
	
	/*   睊媃倜硞摯措蠕 */
	makeEscapeString(  CHAR_getChar( pindex, CHAR_OWNTITLE),
						owntitle, sizeof( owntitle));

	/* 溢惉蕾淏ぁ�岓彸� */
	snprintf( outbuf, sizeof( outbuf), "%d|%d|%s|%s|%s|\n",
				CHAR_getInt( pindex, CHAR_GOLD),
				CHAR_getWorkInt( meindex,CHAR_WORKINT_NEXTTIME),
				cdkey, charaname, owntitle );


	fp_dest = fopen( filename_dest, "w");
	if( !fp_dest) return -1;
	
	fp_src = fopen( filename, "r");
	if( !fp_src ) {
		/*   疐摯郖贀埵摯ぁ鼠摯竘竘怳笢趙敖渦堇 */
		fclose( fp_dest);
		fp_src = fopen( filename, "w");
		if( !fp_src) return -1;
		fputs( outbuf, fp_src);
		fclose( fp_src);
		return 1;
	}
	else {
		int cnt = 1;
		char buf[128];
		while( fgets( line, sizeof( line), fp_src)) {
			getStringFromIndexWithDelim( line, "|" , 1 , buf,sizeof(buf) );
			if( ret == 0 ) {
				/*   睊鉣滌  吨 */
				if( atoi( buf) < CHAR_getInt( pindex, CHAR_GOLD) ) {
					/* 鉣滌誑  埱趙笢堎摯ぁ麙   */
					fputs( outbuf, fp_dest);
					fputs( line, fp_dest);
					ret = cnt;
					cnt ++;
				}
				else {
					/* 鼠躂雄翻毀鼠摯竘竘怳�� */
					fputs( line, fp_dest);
				}
			}
			else {
				/* 鼠躂雄翻毀鼠摯竘竘怳�� */
				fputs( line, fp_dest);
			}
			cnt ++;
			if( cnt > NPC_RANKING_DEFAULT ) break;
		}
		/* 溘璃す璃精誑兒兔蠀鼠Й埱趙笢埵笢鄹 */
		if( ret == 0 && cnt <= NPC_RANKING_DEFAULT ) {
			fputs( outbuf, fp_dest);
			ret = cnt;
		}
	}
	fclose( fp_src);
	fclose( fp_dest);
	/*   ��  躂儒窇 */
	/* 移凝啞栝騷鳴昡鼀↓埰堎螢ワ埵  傮蠐埱趙倜��  窇趙き踏笢�� */
	fp_src = fopen( filename_dest, "r");
	if( !fp_src) return -1;
	fp_dest = fopen( filename, "w");
	if( !fp_dest) {
		fclose( fp_src);
		return -1;
	}
	while( fgets( line, sizeof( line), fp_src)) {
		fputs( line, fp_dest);
	}
	fclose( fp_dest);
	fclose( fp_src);
	return( ret);
#undef  NPC_RANKING_DEFAULT
}
/*-----------------------------------------------
 *   睊榆溘璃す璃精啞栝騷鳴  陑  陑∣  崚
 *
 * 礎倳 meindex     す籤溘騷璃溢蚗袲筒�蛪翹�す璃精NPC摯騷璃溢蚗袲筒��
 *      talkerindex   媃凝崹�桫憶跁邾迺ndex�蛦蟤勳廞alk禱邞嚭埰堎��
 *      admname     箾  Е碟踏庌摯  鞳
 *      
 *----------------------------------------------*/
static void NPC_RoomAdminNew_ReadScoreFile( int meindex, int talkerindex, 
											char *admname)
{
	int     cnt;
	char    filename[128];
	char    line[1024];
	FILE    *fp;
	
	/* 啞栝騷鳴  禱牁埬 */
	strcpy( filename, NPC_ROOMADMINNEW_DATADIR);
	strcat( filename, admname);
	strcat( filename, NPC_ROOMADMINNEW_RANKING_EXTENSION);
	
	fp = fopen( filename, "r");
	if( !fp) return;
	cnt = 1;
	while( fgets( line, sizeof( line), fp)) {
		int     gold,bidtime;
		char    charaname[32],owntitle[32];
		char    tmp[64];
		char    outbuf[1024], buf[256];
		struct  tm tmwk;
		
		getStringFromIndexWithDelim( line, "|" , 1 , buf,sizeof(buf) );
		gold = atoi(buf);
		getStringFromIndexWithDelim( line, "|" , 2 , buf,sizeof(buf) );
		bidtime = atoi(buf);
		getStringFromIndexWithDelim( line, "|" , 4 , buf,sizeof(buf) );
		strcpy( charaname, makeStringFromEscaped( buf));
		getStringFromIndexWithDelim( line, "|" , 5 , buf,sizeof(buf) );
		strcpy( owntitle, makeStringFromEscaped( buf));
		if( strlen( owntitle) != 0 ) {
			strcpy( tmp, "[");
			strcat( tmp, owntitle);
			strcat( tmp, "] ");
			strcpy( owntitle, tmp);
		}
		else {
			strcpy( owntitle, " ");
		}
		memcpy( &tmwk, localtime( (time_t *)&bidtime), sizeof( tmwk));
		
		snprintf( outbuf, sizeof( outbuf),
				NPC_ROOMADMINNEW_MSG_RANKING,
				cnt, 
				tmwk.tm_year + 1900,
				tmwk.tm_mon+1,
				tmwk.tm_mday,
				gold,charaname,owntitle
				);
		CHAR_talkToCli( talkerindex, meindex,outbuf, 
						NPC_RANKING_MSGCOLOR);
		cnt ++;
	}
	fclose( fp);
	
}
/************************************************
 * 疐趵窐  
 ************************************************/
BOOL NPC_RankingInit( int meindex )
{
	char    buf[256];
	char    argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	/* 疐趵撬窐   */
	
	/* type掛蚗哱 */
	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPERANKING);
	/* 集  硞摯  鞳噁   */
	if( !getStringFromIndexWithDelim(argstr, "|" , 1 , buf,sizeof(buf) )) {
		print( "arg err.rankingNPC not created.\n");
		return( FALSE);
	}
	CHAR_setWorkChar( meindex , CHAR_WORKCHAR_ADMNAME , buf);
	
	return TRUE;
}
/************************************************
 *   媃凝崹�桫憶袬敯啄�  
 ************************************************/
void NPC_RankingTalked( int meindex , int talkerindex , char *msg ,
							 int color )
{
	/* 鏤畛騷媕↓誑迶囮悃璃摯1精惉蚗迶雄  埵��  伀埰堎�� */
	if(NPC_Util_CharDistance( talkerindex, meindex ) > 1)return;
	
	CHAR_talkToCli( talkerindex, meindex,
					NPC_ROOMADMINNEW_MSG_RANKING_INFO, 
					NPC_RANKING_MSGCOLOR);
	
	NPC_RoomAdminNew_ReadScoreFile( meindex, talkerindex, 
						CHAR_getWorkChar( meindex, CHAR_WORKCHAR_ADMNAME));
	
}
/************************************************
 * 疐趵窐  
 ************************************************/
BOOL NPC_PrintpassmanInit( int meindex )
{
	char    buf[256];
	char    argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	/* 疐趵撬窐   */
	
	/* type掛蚗哱 */
	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEPRINTPASSMAN);
	/* 集  硞摯  鞳噁   */
	if( !getStringFromIndexWithDelim(argstr, "|" , 1 , buf,sizeof(buf) )) {
		print( "arg err.passmanNPC not created.\n");
		return( FALSE);
	}
	CHAR_setWorkChar( meindex , CHAR_WORKCHAR_ADMNAME , buf);
	
	return TRUE;
}
/************************************************
 *   媃凝崹�桫憶袬敯啄�  
 ************************************************/
void NPC_PrintpassmanTalked( int meindex , int talkerindex , char *msg ,
							 int color )
{
	int i;
	int charnum;
	/* 鏤畛騷媕↓誑迶囮悃璃摯2精惉蚗迶雄  埵��  伀埰堎�� */
	if(NPC_Util_CharDistance( talkerindex, meindex ) > 2)return;
	
	/* 猷覕暵摯集  硞摯  鞳禱號喙埰堎*/
	charnum  = CHAR_getCharNum();
	for(i= CHAR_getPlayerMaxNum() +CHAR_getPetMaxNum();
		i<charnum;
		i++){

		if( CHAR_getCharUse(i) &&
			CHAR_getInt( i ,CHAR_WHICHTYPE) == CHAR_TYPEROOMADMIN ){
			if( strcmp( CHAR_getWorkChar(i,CHAR_WORKCHAR_MYNAME),
						CHAR_getWorkChar( meindex, CHAR_WORKCHAR_ADMNAME))
				==0 )
			{
				break;
			}
		}
	}
	if( i < charnum ) {
		NPC_RoomAdminNew_DspRoomPasswd( meindex, i, talkerindex);
	}
	else {
		CHAR_talkToCli( talkerindex, meindex, "﹝﹝﹝", NPC_RANKING_MSGCOLOR);
	}
}
/* end of roomadminnew */
