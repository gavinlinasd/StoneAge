#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "npc_pauctionman.h"
#include "saacproto_cli.h"

//拍卖王 andy
#ifdef _PAUCTION_MAN
enum {
	WINDOW_START=5,
	WINDOW_SELECT,
	WINDOW_BUY,
	WINDOW_SELL,
	WINDOW_BUYDO,
	WINDOW_SELLDO,
	WINDOW_WARP,
	WINDOW_END,
};
enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
	NPC_WORK_MULTIPLE = CHAR_NPCWORKINT2,
/*
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
typedef struct _tagAUCTIONITEM{
	int index;
	int ti;
	long Stime;
	char Name[256];
	int level;
	int use;
	struct _tagAUCTIONITEM *next;
}AUCTIONITEMs;
#define AUCTION_ADD 1
#define AUCTION_DEL 2
#define MAXAUCTIONs 20
AUCTIONITEMs *pAuctionItem;
#define STANDBYTIME 50

static void NPC_Pauctionman_selectWindow( int meindex, int toindex, int num,int select);
BOOL NPC_PAItemShop_Main(int meindex,int talker, char *argstr,char *token, int sizes);
BOOL NPC_PAItemShop_BuyMain(int meindex, int talker, char *argstr,char *token, int sizes);
BOOL NPC_PAItemShop_SellMain(int meindex,int talker, char *argstr,char *token, int sizes);
int NPC_PAGetMyLimtItem(int talker, char *argstr, char* token, int sizes);
BOOL NPC_PAItemShop_BuyDo( int meindex, int toindex, char *npcarg, int select);
BOOL NPC_PAItemShop_SellDo( int meindex, int toindex, char *npcarg, int select);

AUCTIONITEMs *LIST_getNew();
int NPC_PAItemShop_GetItem( int ti, int *ret);

void NPC_PAGetItemList( char *argstr, char *token, int sizes);
void NPC_AUCTIONITEMs_Init();

BOOL NPC_PauctionManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_PAUCTIONMAN);

	//CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	NPC_AUCTIONITEMs_Init();
	return TRUE;
}

void NPC_PauctionManTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}   
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_Pauctionman_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

void NPC_PauctionManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
		return;
	switch( seqno)	{
	case NPC_PAUCTION_START:
		break;
	case NPC_PAUCTION_SELECT:
		{
			int index=0;
			index = atoi( data);
			switch( index)	{
			case 1:
				NPC_Pauctionman_selectWindow( meindex, talkerindex, WINDOW_BUY, atoi( data));
				break;
			case 2:
				NPC_Pauctionman_selectWindow( meindex, talkerindex, WINDOW_SELL, atoi( data));
				break;
			}
		}
		break;
	case NPC_PAUCTION_BUY:
			NPC_Pauctionman_selectWindow( meindex, talkerindex, WINDOW_BUYDO, atoi( data));
		break;
	case NPC_PAUCTION_SELL:
			NPC_Pauctionman_selectWindow( meindex, talkerindex, WINDOW_SELLDO, atoi( data));
		break;
	case NPC_PAUCTION_WARP:
		if( select == WINDOW_BUTTONTYPE_CANCEL )
			return;
		NPC_Pauctionman_selectWindow( meindex, talkerindex, WINDOW_WARP, atoi( data));
		break;
	case NPC_PAUCTION_END:
		break;
	}

}
//while( getStringFromIndexWithDelim( npcarg,"}",talkNo, buf1, sizeof( buf1)) != FALSE )	{
static void NPC_Pauctionman_selectWindow( int meindex, int toindex, int num, int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[NPC_UTIL_GETARGSTR_BUFSIZE];

	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);

	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}

	if( Action_PartyCheck( meindex, toindex) == FALSE)	{
		CHAR_talkToCli( toindex, meindex, "请一个一个来！", CHAR_COLORYELLOW);
		return;
	}

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return;
	}
	memset( token, 0, sizeof( token));

	switch( num)	{
	case WINDOW_START:
		if( NPC_PAItemShop_Main( meindex, toindex, npcarg, token, sizeof( token)) == FALSE )	{
			return;
		}

		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		buttontype = WINDOW_BUTTONTYPE_NONE;
		windowtype = WINDOW_MESSAGETYPE_ITEMSHOPMENU;
		windowno = NPC_PAUCTION_SELECT;
		break;
	case WINDOW_SELECT:
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_PAUCTION_WARP;
		break;
// ================ Sell
	case WINDOW_SELLDO:
		if( select != 0 ){
			if( NPC_PAItemShop_SellDo( meindex, toindex, npcarg, select) == FALSE ){
				return;
			}
		}
		if( NPC_PAItemShop_Main( meindex, toindex, npcarg, token, sizeof( token)) == FALSE )	{
			return;
		}

		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		buttontype = WINDOW_BUTTONTYPE_NONE;
		windowtype = WINDOW_MESSAGETYPE_ITEMSHOPMENU;
		windowno = NPC_PAUCTION_SELECT;
		break;
	case WINDOW_SELL:
		if( NPC_PAItemShop_SellMain( meindex, toindex, npcarg, token, sizeof( token)) == FALSE ){
			return;
		}
		buttontype = WINDOW_BUTTONTYPE_NONE;
		windowtype = WINDOW_MESSAGETYPE_ITEMSHOPMAIN;
		windowno = NPC_PAUCTION_SELL;
		break;
// ================ BUY
	case WINDOW_BUYDO:
		if( select != 0 ){
			if( NPC_PAItemShop_BuyDo( meindex, toindex, npcarg, select-1) == FALSE ){
				return;
			}
		}
		if( NPC_PAItemShop_Main( meindex, toindex, npcarg, token, sizeof( token)) == FALSE )	{
			return;
		}

		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		buttontype = WINDOW_BUTTONTYPE_NONE;
		windowtype = WINDOW_MESSAGETYPE_ITEMSHOPMENU;
		windowno = NPC_PAUCTION_SELECT;
		break;
	case WINDOW_BUY:
		if( NPC_PAItemShop_BuyMain( meindex, toindex, npcarg, token, sizeof( token)) == FALSE ){
			return;
		}
		buttontype = WINDOW_BUTTONTYPE_NONE;
		windowtype = WINDOW_MESSAGETYPE_ITEMSHOPMAIN;
		windowno = NPC_PAUCTION_BUY;
		break;
// ================
	case WINDOW_WARP:
		ActionNpc_CheckMenuFree( meindex, toindex, npcarg, select);
		return;

	case WINDOW_END:
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}
void NPC_PauctionManLoop( int meindex)
{

}

BOOL NPC_PAItemShop_Main(int meindex,int talker, char *argstr,char *token, int sizes)
{
	char buff[256];
	//int fd = getfdFromCharaIndex( talker);
	memset( buff, 0, sizeof( buff));
    if( NPC_Util_GetStrFromStrWithDelim( argstr, "MAIN_MESS", buff, sizeof( buff)) == NULL )
		return FALSE;
	snprintf(token, sizes,"%s|%s", CHAR_getChar( meindex, CHAR_NAME), buff);
	return TRUE;
}

BOOL NPC_PAItemShop_BuyMain(int meindex, int talker, char *argstr,char *token, int sizes)
{
	//int fd = getfdFromCharaIndex( talker);
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
    char buff[256];

    if( NPC_Util_GetStrFromStrWithDelim( argstr, "MAIN_MESS", buff, sizeof( buff)) == NULL )
		return FALSE;
    if( NPC_Util_GetStrFromStrWithDelim( argstr, "BUY_MESS", buff2, sizeof( buff2)) == NULL )
		return FALSE;
	memset( token2, 0, sizeof( token2));
	sprintf(token,"0|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getChar( meindex, CHAR_NAME), buff, buff2);

    if( NPC_Util_GetStrFromStrWithDelim( argstr, "WHAT_MESS", buff, sizeof( buff)) == NULL )
		return FALSE;
    if( NPC_Util_GetStrFromStrWithDelim( argstr, "LEVEL_MESS", buff2, sizeof( buff2)) == NULL )
		return FALSE;

	snprintf( token2, sizeof( token2), "%s|%s", buff, buff2);
	strncat( token, token2, sizes);
    if( NPC_Util_GetStrFromStrWithDelim( argstr, "REALY_MESS", buff, sizeof( buff)) == NULL )
		return FALSE;
    if( NPC_Util_GetStrFromStrWithDelim( argstr, "ITEMFULL_MESS", buff2, sizeof( buff2)) == NULL )
		return FALSE;
	memset( token2, 0, sizeof( token2));
	sprintf( token2, "|%s|%s", buff, buff2);
	strncat(token , token2, sizeof( token2));
	strcpy(token2, "|");
	NPC_PAGetItemList( argstr, token2, sizeof( token2));
	strncat( token, token2, sizes);

	return TRUE;
}


void NPC_PAGetItemList( char *argstr, char *token, int sizes)
{
	int i=1, ret=-1;
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	for( i=0; i<MAXAUCTIONs; i++)	{
		int itemindex = -1, cost=0, graNo=0;
		itemindex = NPC_PAItemShop_GetItem( i, &ret);
		if( !ITEM_CHECKINDEX( itemindex) ){
			break;
		}
		cost = ITEM_getInt( itemindex, ITEM_COST);
		graNo = ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER);

		sprintf( token2, "%s|0|%d|%d|%d|%s|",
			ITEM_getChar( itemindex, ITEM_SECRETNAME), 0/*pAuctionItem[i].level*/, cost,
			graNo, ITEM_getChar(itemindex, ITEM_EFFECTSTRING));
		strncat( token, token2, sizeof(token2));
	}
}

void NPC_AUCTIONITEMs_Init()
{
	pAuctionItem = NULL;
}


BOOL NPC_PAItemShop_SellMain(int meindex,int talker, char *argstr,char *token, int sizes)
{
	//int fd = getfdFromCharaIndex( talker);
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
   	char buff[256];

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "MAIN_MESS", buff, sizeof( buff)) == NULL )
		return FALSE;
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "SELL_MESS", buff2, sizeof( buff)) == NULL )
		return FALSE;
	sprintf( token, "1|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
			CHAR_getChar( meindex, CHAR_NAME), buff, buff2);
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "STONE_MESS", buff, sizeof( buff)) == NULL )
		return FALSE;
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "REALY_MESS", buff2, sizeof( buff2)) == NULL )
		return FALSE;
	sprintf( token2,"%s|%s|", buff, buff2);
	NPC_PAGetMyLimtItem( talker, argstr, token2, sizeof( token2));
	strncat( token, token2, sizes);
	return TRUE;
}

int NPC_PAGetMyLimtItem(int talker, char *argstr, char* token, int sizes)
{
	char token2[NPC_UTIL_GETARGSTR_LINEMAX];
	int i=1;

	int itemindex;
	int flg=0;
	flg = 0;
	for( i=CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE ; i++ ){

		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) ){
//==
			char escapedname[256];
			char *eff;
			char name[256];
			int cost = ITEM_getInt( itemindex, ITEM_COST);
			//int rate = 1;
			//cost = (int)(cost * rate);
			strcpy( escapedname, ITEM_getChar( itemindex, ITEM_SECRETNAME));
			makeEscapeString( escapedname, name, sizeof( name));
			eff=ITEM_getChar(itemindex, ITEM_EFFECTSTRING);
			makeEscapeString( eff, escapedname, sizeof(escapedname));
			sprintf(token2,"%s|%d|%d|%d|%s|%d|",
					name, flg, cost, ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
					escapedname, i
			);
//==
			strncat( token, token2, sizeof( token2));
		}
	}
	return 1;
}

BOOL NPC_PAItemShop_BuyDo( int meindex, int toindex, char *npcarg, int select)
{
	int ret=-1, ti;
	int itemindex = NPC_PAItemShop_GetItem( select, &ret);
	int cost;
	char token[256];

	if( !ITEM_CHECKINDEX( itemindex) ){
		return FALSE;
	}

	cost=ITEM_getInt( itemindex, ITEM_COST);
	if( CHAR_DelGold( toindex, cost) == 0 ) return FALSE;

	//汇整资料
	saacproto_ACItemAuction_send( acfd, ITEM_getChar( itemindex, ITEM_NAME), "TEST嘿嘿嘿",
		ITEM_getInt( itemindex, ITEM_ID), ret,AUCTION_DEL);

	ti = CHAR_addItemSpecificItemIndex( toindex, itemindex);
	if( !CHAR_CHECKITEMINDEX( toindex, ti) ){
		ITEM_endExistItemsOne( itemindex);
		print ("\n ret error!!");
		return FALSE;
	}
	CHAR_sendItemDataOne( toindex, ti);
	sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);

	return TRUE;
}

BOOL NPC_PAItemShop_SellDo( int meindex, int toindex, char *npcarg, int select)
{
	int itemindex = CHAR_getItemIndex( toindex , select );
	if( ITEM_CHECKINDEX( itemindex) ){
		int cost = ITEM_getInt( itemindex, ITEM_COST);
		saacproto_ACItemAuction_send( acfd, ITEM_getChar( itemindex, ITEM_NAME), "TEST嘿嘿嘿",
			ITEM_getInt( itemindex, ITEM_ID), 0/*ret*/,AUCTION_ADD);
		CHAR_DelItem( toindex, select);
		CHAR_AddGold( toindex, cost);	
	}else	{
		CHAR_talkToCli( toindex, -1, "没有这样物品！", CHAR_COLORYELLOW);
		return FALSE;
	}
	return TRUE;
}

int NPC_PAItemShop_AddItem( int itemID, int ret)
{
	AUCTIONITEMs *works = NULL;
	AUCTIONITEMs *heads = NULL;

	int i, itemindex;
	works = pAuctionItem;
	heads = pAuctionItem;

	itemindex = ITEM_makeItemAndRegist( itemID);
	if( !ITEM_CHECKINDEX( itemindex) )
		return -1;

	i=0;
	while( i <= MAXAUCTIONs ){
		if( works == NULL )	{
			works = LIST_getNew();
			if( works == NULL )
				return -1;
			works->index = itemindex;
			works->use = 1;
			works->ti = ret;
			works->next = NULL;
			if( heads == NULL )	{
				pAuctionItem = works;
			}else	{
				heads->next = works;
			}
			works = pAuctionItem;
			return itemindex;
		}
		i++;
		heads = works;
		works = works->next;
		if( i >= MAXAUCTIONs )
			break;
	}
	return -1;
}

BOOL NPC_PAItemShop_DelItem( int ti, int index)
{
	AUCTIONITEMs *works = NULL;
	AUCTIONITEMs *heads = NULL;
	works = pAuctionItem;
	//heads = pAuctionItem;

	while( works != NULL )	{
		if( works->ti == ti )	{
			if( heads == NULL ){
				pAuctionItem = works->next;
			}else	{
				heads->next = works->next;
			}
			free( works);
			return TRUE;
		}
		heads = works;
		works = works->next;
	}
	return FALSE;
}

int NPC_PAItemShop_GetItem( int ti, int *ret)
{
	AUCTIONITEMs *works = NULL;
	//heads
	int i;
	works = pAuctionItem;

	for( i=0; i<ti; i++)	{
		if( works == NULL )
			return -1;
		works = works->next;
	}

	if( works == NULL ) return -1;
	if( works->use > 0 ){
		*ret = works->ti;
		return works->index;
	}
	return -1;
}

BOOL NPC_PAItemShop_reItem( int itemindex, char *Data)
{
	if( !ITEM_CHECKINDEX( itemindex) )
		return FALSE;

	return TRUE;
}

AUCTIONITEMs *LIST_getNew()
{
	AUCTIONITEMs *MBUF=NULL;
	MBUF = (AUCTIONITEMs *) calloc( 1, sizeof( struct _tagAUCTIONITEM) );
	if( MBUF == NULL )
		return NULL;
	MBUF->use = -1;
	MBUF->ti = -1;
	MBUF->next = NULL;
	return MBUF;
}
#endif

