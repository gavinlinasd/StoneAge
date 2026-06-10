#include "version.h"
#include <string.h>
#include <math.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "enemy.h"
#include "log.h"
#include "battle.h"
#include "readmap.h"
#include "handletime.h"
#include "npc_eventaction.h"

#ifdef _MARKET_TRADE
#include "npc_mtradenpcman.h"
#include "item_trade.h"

#define MAPTRADE_CHECK	3000
#define MAPTRADE_CHECKMASTER 100
#define MAPTRADE_CHECKSELL 200

enum	{
	MAPTRADE_START = 1,
	MAPTRADE_MAN,
	MAPTRADE_END,
};
enum {
	NPC_WORK_INDEX = CHAR_NPCWORKINT1,
	NPC_WORK_WORKTYPE = CHAR_NPCWORKINT2,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT3,
	NPC_WORK_SELLINDEX = CHAR_NPCWORKINT4,
/*
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
*/
};

static void NPC_MapTrade_selectWindow( int meindex, int toindex, int num,int select);

BOOL MapTradeManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL)	{
		print("MapTradeManInit: GetArgStrErr");
		return FALSE;
	}
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_MAPTRADEMAN );
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, MAPTRADE_CHECK);

	CHAR_setWorkInt( meindex, NPC_WORK_SELLINDEX, 0 );
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	CHAR_setWorkInt( meindex, NPC_WORK_INDEX, TRADE_getMaxNumInt());
	InitMapTradeData( -1, 0);	//初始摊位资料
	return TRUE;
}

void MapTradeManLoop( int meindex)
{
	int masterindex=-1, i;
	int maxNum = TRADE_getMaxNumInt();
	int nums=0;

	if( CHAR_getWorkInt( meindex, NPC_WORK_CURRENTTIME) > NowTime.tv_sec)
		return;
	else
		CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec+MAPTRADE_CHECKMASTER);

	for( i=0; i<maxNum; i++)	{
		masterindex = TRADE_getMasterInt( i);
		if( masterindex == -1 )	continue;
		if( !CHAR_CHECKINDEX( masterindex) || 
			CHAR_getInt( masterindex, CHAR_FLOOR) != TRADEMAP ||
			CHAR_getWorkInt( masterindex, CHAR_MAPTRADETYPE) != (i+TRADESTARTNUM)	)	{
			TRADE_setMasterInt( i, -1);
			TRADE_setTimeInt( i, 0);
			MAPTRADE_setSellIndex( i, -1);
			MAPTRADE_setCharSellName( i, "NULL");
			MAPTRADE_CLEANGOLD( TRADEMAP, i);
		}else	{
			//showString( int meindex, char *showstr, 0);
			nums++;
		}
	}//MAPTRADE_getSellIndex
	{
		char buf1[256], *buf2;
		i = CHAR_getWorkInt( meindex, NPC_WORK_SELLINDEX);
		CHAR_setWorkInt( meindex, NPC_WORK_SELLINDEX, (i+1)%maxNum);
		masterindex = TRADE_getMasterInt( i);
		if( !CHAR_CHECKINDEX( masterindex))
			return;
		buf2 = MAPTRADE_getCharSellName( i);
		if( buf2 == NULL || !strcmp( buf2, "NULL"))
			return;
		sprintf( buf1, "%d号贩卖%s", i+TRADESTARTNUM, buf2);
		showString( meindex, buf1, 0);
	}
/*
	i = CHAR_getWorkInt( meindex, NPC_WORK_SELLINDEX);
	masterindex = TRADE_getMasterInt( i);
	if( !CHAR_CHECKINDEX( masterindex))
		return;
	{
		char *Goodname;
		int sellindex=-1, objindex;
		sellindex = MAPTRADE_getSellIndex( i);
		objindex = CHAR_getWorkInt( sellindex, CHAR_WORKOBJINDEX);
		if (OBJECT_getType( objindex) == OBJTYPE_CHARA) {
			if( CHAR_getInt( sellindex, CHAR_WHICHTYPE) == CHAR_TYPEPET )	{
				CHAR_getChar(
			}
		}
	}
*/
}

void MapTradeManTalked( int meindex , int talkerindex , char *msg , int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, MAPTRADE_START);
	NPC_MapTrade_selectWindow( meindex, talkerindex, MAPTRADE_START, -1);
}

void NPC_MapTrade_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];
	char message[][256]={ "STARTMSG", "YESMSG", "NOMSG", "ENDMSG", "NOTHING"};
	int Msg_Type=4;
	int fd = getfdFromCharaIndex( toindex );
	int buttontype = 0, windowtype = 0, windowno = 0;
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL)	{
		print("MapTradeManInit: GetArgStrErr");
		return;
	}
	memset( token, 0, sizeof( token));
	switch( num)	{
	case MAPTRADE_START:	//1
		Msg_Type = 0;
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, MAPTRADE_MAN);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = MAPTRADE_MAN;
		break;
	case MAPTRADE_MAN:
		Msg_Type = 3;
		if( select == WINDOW_BUTTONTYPE_YES )	{
			int num;
			Msg_Type = 2;
			if( CHAR_getWorkInt( toindex, CHAR_MAPTRADETYPE) > 0 )	{
				sprintf(token , "你已经承租了%d号摊位了呀！",
					CHAR_getWorkInt( toindex, CHAR_MAPTRADETYPE));
			}else	if( (num = TRADE_AddMasrerTrade( toindex)) != -1 )	{
				CHAR_setWorkInt( toindex, CHAR_MAPTRADETYPE, num);
				sprintf(token , "好了，%d号摊位暂时就租给你了。\n一旦下线离开後这摊位就必须归还了。", num);
				//CHAR_talkToCli( i, -1, buff, CHAR_COLORYELLOW);
				Msg_Type = 1;
			}
		}
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, MAPTRADE_END);
		buttontype = WINDOW_BUTTONTYPE_YES;
		windowno = MAPTRADE_END;
		break;
	case MAPTRADE_END:
		Msg_Type = 3;
		buttontype = WINDOW_BUTTONTYPE_YES;
		windowno = MAPTRADE_END;
		break;
	}
	if( !strcmp( token, "\0"))	{
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, message[Msg_Type], token, sizeof( token)) == NULL )
			return;
	}
	//送讯息给 CLI						
	lssproto_WN_send( fd, windowtype, buttontype, windowno,
			CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token	);																				
}

void MapTradeManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{

	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	if( select != WINDOW_BUTTONTYPE_YES )	{
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
		return;
	}

	switch( seqno)	{
	case MAPTRADE_MAN:
		if( CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT) != MAPTRADE_MAN )	{
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			return;
		}
		if( CHAR_getInt( talkerindex, CHAR_MAPTRADETYPE) > 0 )	{
			char buf1[256];
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);

			sprintf( buf1, "你已经是%d号摊位的主人了呀！", CHAR_getInt( talkerindex, CHAR_MAPTRADETYPE));
			CHAR_talkToCli( talkerindex, -1, buf1, CHAR_COLORYELLOW);
			return;
		}
		NPC_MapTrade_selectWindow( meindex, talkerindex, MAPTRADE_MAN, select);
		break;
	case MAPTRADE_END:
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
		return;
		break;
	}
	return;
}
/*
	while( getStringFromIndexWithDelim(buf1, ",", k, buf2, sizeof(buf2)) != FALSE )	{
		Re_Pet[k-1] = atoi( buf2);
		if( Re_Pet[k-1] < 0 )
			return FALSE;
		k++;
		if( (k-1) >= arraysizeof( Re_Pet) )
			break;
	}
*/
#endif
