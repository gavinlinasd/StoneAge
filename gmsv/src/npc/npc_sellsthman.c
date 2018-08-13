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
#include "npc_sellsthman.h"
#include "pet_skill.h"

#ifdef _NPC_SELLSTH
enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,

	NPC_WORK_PAGE = CHAR_NPCWORKINT2,
	NPC_WORK_NUM = CHAR_NPCWORKINT3,

	NPC_WORK_CALLPAGE = CHAR_NPCWORKINT4,
	NPC_WORK_CALLNUM = CHAR_NPCWORKINT5,

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
#define SELLSTHMESS_LOOP 2000
enum{
	WINDOW_TALK=0,
	WINDOW_START=10,
	WINDOW_MENU,
	WINDOW_LOOK,
	WINDOW_CALL,
	WINDOW_FIND,
	WINDOW_SELLON,
	WINDOW_SELLHANDLE,
	WINDOW_BUYMESS,
	WINDOW_BUYHANDLE,
	WINDOW_HELP,
	WINDOW_END,

	NPC_WINDOW_TALK=300,
	NPC_WINDOW_START,
	NPC_WINDOW_MENU,
	NPC_WINDOW_LOOK,
	NPC_WINDOW_FIND,
	NPC_WINDOW_NEXT,
	NPC_WINDOW_SELLON,
	NPC_WINDOW_SELLHANDLE,
	NPC_WINDOW_BUYMESS,
	NPC_WINDOW_BUYHANDLE,
	NPC_WINDOW_HELP,
	NPC_WINDOW_END,

};

#define MAX_LISTPAGE	20
#define MAX_LISTNUM		12
struct _tagNSellSthList{
	int use;
	int fd;
	int sellindex;
	int selltype;
	int uindex;
	int Iindex;
	char headmess[256];
	char sellmess[256];
}NSellSthList[MAX_LISTPAGE][MAX_LISTNUM];

static int StructBeInit=0;

static void NPC_SellsthMan_selectWindow( int meindex, int toindex, int num,int select, char *data);
BOOL NPC_getSellSth_ListString( int page, int num, char *ListString, int slen );
BOOL NPC_setSellSth_ListString( int meindex, int toindex, int *page, int *num, char *data );
BOOL NPC_getSellSth_ViewString( int page, int num, char *ListString, int slen );
int NPC_getSellSth_FreeTiCount( void);

void NPC_SellrePutList( int defpage, int ti)
{
	int i;
	if( NSellSthList[defpage][ti].use != 0 ) return;
	for( i=(ti+1); i<MAX_LISTNUM-1; i++){
		if( NSellSthList[defpage][i].use != 0 ) continue;
		NSellSthList[defpage][i].sellindex = NSellSthList[defpage][i+1].sellindex;
		NSellSthList[defpage][i].fd = NSellSthList[defpage][i+1].fd;
		NSellSthList[defpage][i].Iindex = NSellSthList[defpage][i+1].Iindex;
		NSellSthList[defpage][i].uindex = NSellSthList[defpage][i+1].uindex;
		NSellSthList[defpage][i].selltype = NSellSthList[defpage][i+1].selltype;
		sprintf( NSellSthList[defpage][i].headmess, "%s", NSellSthList[defpage][i+1].headmess);
		sprintf( NSellSthList[defpage][i].sellmess, "%s", NSellSthList[defpage][i+1].sellmess);
		NSellSthList[defpage][i].use = 1;

		NSellSthList[defpage][i+1].use = 0;
	}
}


BOOL NPC_SellsthManInit( int meindex )
{
	char npcarg[ NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg) )==NULL){
		print("SellsthMan: GetArgStrErr!!");
		return FALSE;
	}
   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_SELLSTHMAN);
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, SELLSTHMESS_LOOP);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

	if( StructBeInit == 0 ){
		int i, j;

		for( i=0; i<MAX_LISTPAGE; i++){
			for( j=0; j<MAX_LISTNUM; j++ )
				NSellSthList[i][j].use = 0;
		}
	}

	CHAR_setWorkInt( meindex, NPC_WORK_CALLPAGE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_CALLNUM, 0);

	CHAR_setWorkInt( meindex, NPC_WORK_PAGE, 0);
	CHAR_setWorkInt( meindex, NPC_WORK_NUM, 0);
	return TRUE;
}

void NPC_SellsthManTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;

	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}
/*
	if( CHAR_getWorkInt ( talkerindex , CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		CHAR_talkToCli( talkerindex, meindex, "！",CHAR_COLORYELLOW);
		return;
	}
*/
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_START, 0, NULL);
}

void NPC_SellsthMan_selectWindow( int meindex, int toindex, int num,int select, char *data)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[4096];

	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}
	print( "ANDY SellsthMan_selectWindow( %d, %d, %d, %d)\n", meindex, toindex, num, select);

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return;
	}

	memset( token, 0, sizeof( token));

	switch( num)	{
	case WINDOW_START:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "START_MSG", token, sizeof( token) ) == NULL) {
			print("SellSthMan err Can't Get START_MSG string\n");
			return;
		}
		buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
		windowno = NPC_WINDOW_START;
		break;
	case WINDOW_MENU:
		{
			int k=1;
			char buf[512], buf1[256];
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "MENU_SELECT", buf, sizeof( buf) ) == NULL) {
				print("SellSthMan err Can't Get MENU_SELECT string\n");
				return;
			}
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "MENU_HEAD", buf1, sizeof( buf1) ) == NULL) {
				print("SellSthMan err Can't Get MENU_HEAD string\n");
				return;
			}
			sprintf( token, "%s：\n", buf1);
			while( getStringFromIndexWithDelim( buf, ",", k, buf1, sizeof( buf1)) != FALSE ){
				k++;
				strcat( token, buf1);
				strcat( token, "\n");
			}
			windowtype = WINDOW_MESSAGETYPE_SELECT;
			buttontype = WINDOW_BUTTONTYPE_CANCEL;
			windowno = NPC_WINDOW_MENU;
		}
		break;
	case WINDOW_LOOK://看详细道具或宠物内容
		{
			int num;
			int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			num = atoi( data);
			if( num <0 || num >= MAX_LISTNUM ) return;
			if( NPC_getSellSth_ViewString( page, num, token, sizeof( token)) == FALSE ){
				return;
			}
			windowtype = WINDOWS_MESSAGETYPE_SELLSTHVIEW;
			windowno = NPC_WINDOW_NEXT;
		}
		break;
	case WINDOW_CALL:
		{
			int num;
			int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			num = atoi( data);
			if( num <0 || num >= MAX_LISTNUM ) return;

			if( NSellSthList[page][num].use == 0 )return;
			if( !CHAR_CHECKINDEX( NSellSthList[page][num].sellindex) ||
				NSellSthList[page][num].fd != getfdFromCharaIndex( NSellSthList[page][num].sellindex) )
				return;

			sprintf( token, "%s欲购买你拍卖(%d页,%d项)的物品，\n%s正在%s。", CHAR_getChar( toindex, CHAR_NAME),
				page, num, CHAR_getChar( toindex, CHAR_NAME), MAP_getFloorName( CHAR_getInt( toindex, CHAR_FLOOR)) );

			if( CHAR_getWorkInt( NSellSthList[page][num].sellindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ){
				CHAR_talkToCli( NSellSthList[page][num].sellindex, meindex, token, CHAR_COLORYELLOW);
			}else{
				int callfd = getfdFromCharaIndex( NSellSthList[page][num].sellindex);
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowno = NPC_WINDOW_END;
				lssproto_WN_send( callfd, windowtype, buttontype, windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
			}
			return;
		}
		break;
	case WINDOW_FIND:
		{
			int page, num;
			page = select >> 16;
			num = select & 0x0000ffff;

			page = (page<0||page>=MAX_LISTPAGE)?0:page;
			num = (num<0||num>=MAX_LISTNUM)?0:num;
			if( NPC_getSellSth_ListString( page, num, token, sizeof( token) ) == FALSE ) return;
			windowtype = WINDOWS_MESSAGETYPE_SELLSTHMENU;
			windowno = NPC_WINDOW_NEXT;
		}
		break;
	case WINDOW_SELLON:
		if( CHAR_getWorkInt( toindex, CHAR_WORKSELLSTHNUM) >= 3 ){
			CHAR_talkToCli( toindex, meindex, "最多只能登记叁次。", CHAR_COLORRED);
			return;
		}else if( CHAR_getInt( toindex, CHAR_GOLD) < 100 ){
			CHAR_talkToCli( toindex, meindex, "需付100石币才能登记。", CHAR_COLORRED);
			return;
		}

		if( NPC_getSellSth_FreeTiCount() < 1 ){
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FULL_MSG", token, sizeof( token) ) == NULL) {
				print("SellSthMan err Can't Get FULL_MSG string\n");
				return;
			}
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowno = NPC_WINDOW_END;
		}else{
			windowtype = WINDOWS_MESSAGETYPE_SELLSTHSELL;
			windowno = NPC_WINDOW_SELLON;
		}
		break;
	case WINDOW_SELLHANDLE:
		{
			int page, num;
			if( NPC_setSellSth_ListString( meindex, toindex, &page, &num, data ) == FALSE ) {
				if( NPC_Util_GetStrFromStrWithDelim( npcarg, "ERROR_MSG", token, sizeof( token) ) == NULL) {
					print("SellSthMan err Can't Get ERROR_MSG string\n");
					return;
				}
			}else if( NPC_Util_GetStrFromStrWithDelim( npcarg, "OK_MSG", token, sizeof( token) ) == NULL) {
				print("SellSthMan err Can't Get OK_MSG string\n");
				return;
			}
			{
				int sellnum = CHAR_getWorkInt( toindex, CHAR_WORKSELLSTHNUM);
				CHAR_setWorkInt( toindex, CHAR_WORKSELLSTHNUM, sellnum+1);
				CHAR_DelGold( toindex, 100);
			}
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowno = NPC_WINDOW_END;
		}
		break;
	case WINDOW_BUYMESS:
		break;
	case WINDOW_BUYHANDLE:
		break;
	case WINDOW_HELP:
		break;
	case WINDOW_END:
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_SellsthManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}
	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
		return;

	switch( seqno)	{
	case NPC_WINDOW_START:
		NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_MENU, select, NULL);
		break;
	case NPC_WINDOW_MENU:
		switch( atoi( data)) {
		  case 1://查看
			  CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			  NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_FIND, 0, NULL);
			  break;
		  case 2://登记
			  NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_SELLON, select, NULL);
			  break;
		}
		break;
	case NPC_WINDOW_LOOK:
		break;
	case NPC_WINDOW_FIND://包含买
		break;
	case NPC_WINDOW_NEXT:
		{
			int ti;
			int page = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);

			if( select == WINDOW_BUTTONTYPE_PREV ){
				page = ((page-1)<0)?0:page-1;
			}else if( select == WINDOW_BUTTONTYPE_NEXT ){
				page = ((page+1)>=MAX_LISTPAGE)?(MAX_LISTPAGE-1):(page+1);
			}else if( select == WINDOW_BUTTONTYPE_OK ){ //买
			}else if( select == WINDOW_BUTTONTYPE_YES ){ //买
				NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_CALL, select, data);
				return;
			}else if( select == 0 ){
				NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_LOOK, select, data);
				return;
			}
			ti = (page<<16);
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, page);
			NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_FIND, ti, data);
		}
		break;
	case NPC_WINDOW_SELLON:
		NPC_SellsthMan_selectWindow( meindex, talkerindex, WINDOW_SELLHANDLE, select, data);
		break;
	case NPC_WINDOW_SELLHANDLE:
		break;
	case NPC_WINDOW_BUYMESS:
		break;
	case NPC_WINDOW_BUYHANDLE:
		break;
	case NPC_WINDOW_HELP:
		break;
	case NPC_WINDOW_END:
		break;
	}
}
static int runType = 0;
void NPC_SellsthManLoop( int meindex)
{
	int i, j;
	char headmsg[256];
	char token[256];
	int playernum, page, num;
	int floor = CHAR_getInt( meindex, CHAR_FLOOR);
	playernum = CHAR_getPlayerMaxNum();

	runType = (++runType)%100;

	page = CHAR_getWorkInt( meindex, NPC_WORK_CALLPAGE );
	num = CHAR_getWorkInt( meindex, NPC_WORK_CALLNUM );

	for( i=0; i<MAX_LISTNUM; i++){
		if( NSellSthList[page][num].use == 1 ){
			break;
		}
		if( ++num >= MAX_LISTNUM ){
			num = 0;
			if( ++page >= MAX_LISTPAGE ){
				page=0;
			}
		}
	}
	CHAR_setWorkInt( meindex, NPC_WORK_CALLPAGE, page);
	CHAR_setWorkInt( meindex, NPC_WORK_CALLNUM, num);
	if( i >= MAX_LISTNUM )return;
	sprintf( headmsg, "拍卖频道(%d页%d项)：%s (拍卖者：%s)。",
		page, num, NSellSthList[page][num].headmess,
		CHAR_getChar( NSellSthList[page][num].sellindex, CHAR_NAME) );
	if( NSellSthList[page][num].selltype==1 ){
		sprintf( token, "拍卖道具：%s。",
			ITEM_getChar( NSellSthList[page][num].Iindex, ITEM_NAME) );
	}else{
		sprintf( token, "拍卖宠物：%s  LV:%d %s。",
			CHAR_getChar( NSellSthList[page][num].Iindex, CHAR_NAME),
			CHAR_getInt( NSellSthList[page][num].Iindex, CHAR_LV),
#ifdef _PET_2TRANS
			CHAR_getInt( NSellSthList[page][num].Iindex , CHAR_TRANSMIGRATION) == 2 )?"二转":(CHAR_getInt( NSellSthList[page][num].Iindex , CHAR_TRANSMIGRATION) == 1 )?"一转":"")
#else
			( CHAR_getInt( NSellSthList[page][num].Iindex , CHAR_TRANSMIGRATION) == 1 )?"转":""
#endif			
			);
	}

	for(i=0; i<playernum; i++){
		if( !CHAR_CHECKINDEX( i)) continue;
		if( CHAR_getInt( i, CHAR_FLOOR) != floor ) continue;
		if( CHAR_getWorkInt( i, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ) continue;
#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
		if ( CHAR_getFlg ( i , CHAR_ISAUC ) ){
#endif
			CHAR_talkToCli( i, meindex, headmsg, CHAR_COLORYELLOW);
			CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
#ifdef _AUCPROTOCOL				// (不可开) Syu ADD 拍卖频道开关Protocol
		}
#endif
	}

	if( ++num >= MAX_LISTNUM ){
		num = 0;
		if( ++page >= MAX_LISTPAGE ){
			page=0;
		}
	}
	CHAR_setWorkInt( meindex, NPC_WORK_CALLPAGE, page);
	CHAR_setWorkInt( meindex, NPC_WORK_CALLNUM, num);

	if( runType%10 == 0 ){
		for( i=0; i<MAX_LISTPAGE; i++){
			for( j=0; j<MAX_LISTNUM; j++ ){
				if( NSellSthList[i][j].use == 0 ) continue;
				if( !CHAR_CHECKINDEX( NSellSthList[i][j].sellindex)||
					NSellSthList[i][j].fd != getfdFromCharaIndex( NSellSthList[i][j].sellindex) ){
					NSellSthList[i][j].use = 0;
					continue;
				}

				if( NSellSthList[i][j].selltype == 1 ){//道具
					int itemindex = CHAR_getItemIndex( NSellSthList[i][j].sellindex , NSellSthList[i][j].uindex );
					if( !ITEM_CHECKINDEX( itemindex) ||
						NSellSthList[i][j].Iindex != itemindex ){
						int num;
						NSellSthList[i][j].use = 0;
						num = CHAR_getWorkInt( NSellSthList[i][j].sellindex, CHAR_WORKSELLSTHNUM);
						num = ((num-1)<0)?0:(num-1);
						CHAR_setWorkInt( NSellSthList[i][j].sellindex, CHAR_WORKSELLSTHNUM, num);
					}
				}else if( NSellSthList[i][j].selltype == 2 ){
					int petindex = CHAR_getCharPet( NSellSthList[i][j].sellindex, NSellSthList[i][j].uindex);
					if( !CHAR_CHECKINDEX( petindex) ||
						NSellSthList[i][j].Iindex != petindex ){
						int num;
						NSellSthList[i][j].use = 0;
						num = CHAR_getWorkInt( NSellSthList[i][j].sellindex, CHAR_WORKSELLSTHNUM);
						num = ((num-1)<0)?0:(num-1);
						CHAR_setWorkInt( NSellSthList[i][j].sellindex, CHAR_WORKSELLSTHNUM, num);
					}
				}
			}
		}
	}
}

BOOL NPC_getSellSth_ListString( int page, int num, char *ListString, int slen )
{
	char buff[4096];
	char buf[512];
	int listnum=0, i;

	memset( buff, 0, sizeof(buff));

	for( i=num; i<MAX_LISTNUM; i++)
		NPC_SellrePutList( page, i);

	for( i=num; i<MAX_LISTNUM; i++){
		if( NSellSthList[page][i].use == 0  )continue;
		if( !CHAR_CHECKINDEX( NSellSthList[page][i].sellindex ) ||
			NSellSthList[page][i].fd != getfdFromCharaIndex( NSellSthList[page][i].sellindex) ){
			NSellSthList[page][i].use = 0;
			continue;
		}
		if( NSellSthList[page][i].selltype == 1 ){//道具
			int itemindex = CHAR_getItemIndex( NSellSthList[page][i].sellindex , NSellSthList[page][i].uindex );
			if( !ITEM_CHECKINDEX( itemindex) ||
				NSellSthList[page][i].Iindex != itemindex ){
				
				int num;
				num = CHAR_getWorkInt( NSellSthList[page][i].sellindex, CHAR_WORKSELLSTHNUM);
				num = ((num-1)<0)?0:(num-1);
				CHAR_setWorkInt( NSellSthList[page][i].sellindex, CHAR_WORKSELLSTHNUM, num);
				NSellSthList[page][i].use = 0;
				continue;
			}
		}else if( NSellSthList[page][i].selltype == 2 ){
			int petindex = CHAR_getCharPet( NSellSthList[page][i].sellindex, NSellSthList[page][i].uindex);
			if( !CHAR_CHECKINDEX( petindex) ||
				NSellSthList[page][i].Iindex != petindex ){
				int num;
				num = CHAR_getWorkInt( NSellSthList[page][i].sellindex, CHAR_WORKSELLSTHNUM);
				num = ((num-1)<0)?0:(num-1);
				CHAR_setWorkInt( NSellSthList[page][i].sellindex, CHAR_WORKSELLSTHNUM, num);
				NSellSthList[page][i].use = 0;
				
				continue;
			}
		}

		listnum++;
		sprintf( buf, "%d,%s,%s,%s", i, 
			CHAR_getChar( NSellSthList[page][i].sellindex, CHAR_NAME ),
			NSellSthList[page][i].headmess,
			NSellSthList[page][i].sellmess );
		strcat( buff, buf);
		strcat( buff, "|");
	}

	if( strlen( ListString) + strlen( buff) > slen ) return FALSE;

	sprintf( ListString, "%d|%d|%s", listnum, page, buff);
	printf( "ANDY ListString:%s\nstrlen = %d \n", ListString, strlen( ListString) );

	if( strlen( ListString) <= 0 ) return FALSE;
	return TRUE;
}

BOOL NPC_setSellSth_ListString( int meindex, int toindex, int *page, int *num, char *data )
{
	char buf1[256];
	int i, j;
	int fd = getfdFromCharaIndex( toindex);

	for( i=0; i<MAX_LISTPAGE; i++){
		for( j=0; j<MAX_LISTNUM; j++ ){
			if( NSellSthList[i][j].use == 1 ){
				if( !CHAR_CHECKINDEX( NSellSthList[i][j].sellindex)||
					NSellSthList[i][j].fd != getfdFromCharaIndex( NSellSthList[i][j].sellindex) ){
					NSellSthList[i][j].use = 0;
				}
				continue;
			}
			NSellSthList[i][j].sellindex = toindex;
			NSellSthList[i][j].fd = fd;

			if( getStringFromIndexWithDelim( data, "|", 1, buf1, sizeof( buf1)) == FALSE ){
				print("ANDY err1.\n");
				return FALSE;
			}
			sprintf( NSellSthList[i][j].headmess, "%s", buf1);
			if( getStringFromIndexWithDelim( data, "|", 2, buf1, sizeof( buf1)) == FALSE ){
				print("ANDY err2.\n");
				return FALSE;
			}
			sprintf( NSellSthList[i][j].sellmess, "%s", buf1);


			if( getStringFromIndexWithDelim( data, "|", 3, buf1, sizeof( buf1)) == FALSE ){
				print("ANDY err3.\n");
				return FALSE;
			}
			NSellSthList[i][j].selltype = atoi( buf1);
			if( getStringFromIndexWithDelim( data, "|", 4, buf1, sizeof( buf1)) == FALSE ){
				print("ANDY err4.\n");
				return FALSE;
			}
			NSellSthList[i][j].uindex = atoi( buf1);

//Iindex

			if( NSellSthList[i][j].selltype == 1 ){//道具
				int itemindex = CHAR_getItemIndex( toindex , NSellSthList[i][j].uindex );
				if( !ITEM_CHECKINDEX( itemindex) ){
					print("ANDY err5. item:%d\n", NSellSthList[i][j].uindex);
					return FALSE;
				}
				if( ITEM_getInt( itemindex, ITEM_VANISHATDROP) == 1 ){
					char buf[256];
					sprintf( buf, "%s无法交易。", ITEM_getChar( itemindex, ITEM_NAME) );
					CHAR_talkToCli( toindex, meindex, buf, CHAR_COLORYELLOW);
					return FALSE;
				}
				NSellSthList[i][j].Iindex = itemindex;
			}else if( NSellSthList[i][j].selltype == 2 ){
				int petindex = CHAR_getCharPet( toindex, NSellSthList[i][j].uindex);
				if( !CHAR_CHECKINDEX( petindex) ){
					print("ANDY err6. pet:%d\n", NSellSthList[i][j].uindex);
					return FALSE;
				}
				NSellSthList[i][j].Iindex = petindex;
			}else{
				print("ANDY err8.\n");
				return FALSE;
			}

			NSellSthList[i][j].use = 1;
			*page = i;
			*num = j;
			return TRUE;
		}
	}
	print("ANDY err9.\n");
	return FALSE;
}


BOOL NPC_getSellSth_ViewString( int page, int num, char *ListString, int slen )
{
	char buff[256];
	char buf1[256], buf2[256];
	int ti, sellindex;

	if( num < 0 || num > MAX_LISTNUM ) return FALSE;
	if( NSellSthList[page][num].use == 0  )return FALSE;
	if( !CHAR_CHECKINDEX( NSellSthList[page][num].sellindex ) )return FALSE;
	sellindex = NSellSthList[page][num].sellindex;

	if( NSellSthList[page][num].selltype == 1 ){//道具
		int itemindex, crushe;
		ti = NSellSthList[page][num].uindex;
		itemindex = CHAR_getItemIndex( sellindex , ti);
		if( !ITEM_CHECKINDEX( itemindex) ) return FALSE;
		crushe	= ITEM_getItemDamageCrusheED( itemindex);
		sprintf( buff, "1|%s|%s|0|%d|%d|%d|0|0|0|%s|||||||",
			makeEscapeString( ITEM_getChar( itemindex, ITEM_SECRETNAME), buf2, sizeof( buf2)),
			makeEscapeString( ITEM_getChar( itemindex, ITEM_NAME), buf1, sizeof( buf1)),
			ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
			crushe,
			ITEM_getInt( itemindex, ITEM_USEPILENUMS),
			ITEM_getChar( itemindex, ITEM_EFFECTSTRING) );
	}else if( NSellSthList[page][num].selltype == 2 ){//宠物
		int petindex, i;
		ti = NSellSthList[page][num].uindex;
		petindex = CHAR_getCharPet( sellindex, ti);
		if( !CHAR_CHECKINDEX( petindex) ) return FALSE;
		//TYPE|名|  |图|等级|转|  |攻|防|敏|技1|技2|技3|技4|技5|技6|技7
		sprintf( buff, "2|%s|%s|%d|%d|%d|%d|%d|%d|%d|",
			makeEscapeString( CHAR_getChar( petindex, CHAR_NAME ), buf1, sizeof(buf1)),
			makeEscapeString( CHAR_getUseName( petindex), buf2, sizeof(buf2)),
			CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER),
			CHAR_getInt( petindex , CHAR_LV),
			CHAR_getInt( petindex , CHAR_TRANSMIGRATION),
			CHAR_getWorkInt(petindex , CHAR_WORKMAXHP),
			CHAR_getWorkInt( petindex, CHAR_WORKATTACKPOWER),
			CHAR_getWorkInt( petindex, CHAR_WORKDEFENCEPOWER),
			CHAR_getWorkInt( petindex, CHAR_WORKQUICK));

		for( i=0; i<7; i++){
			int skillarray, skillID;
			skillID = CHAR_getPetSkill( petindex, i);
			skillarray = PETSKILL_getPetskillArray( skillID);
			if( PETSKILL_CHECKINDEX( skillarray) ){
				strcat( buff, PETSKILL_getChar( skillarray, PETSKILL_NAME) );
			}
			strcat( buff, "|");
		}
	}
	sprintf( ListString, "%d|%s|%s|%s|%s",
		num, CHAR_getChar( sellindex, CHAR_NAME),
		NSellSthList[page][num].headmess,
		NSellSthList[page][num].sellmess, 
		buff );

	if( strlen( ListString) <= 0 ){
		print("err ListString:%s strlen <= %d\n", ListString, strlen( ListString));
		return FALSE;
	}
	return TRUE;
}


int NPC_getSellSth_FreeTiCount( void)
{
	int counts = 0, i, j;
	for( i=0; i<MAX_LISTPAGE; i++){
		for( j=0; j<MAX_LISTNUM; j++ ){
			if( NSellSthList[i][j].use != 1 )
				counts++;
		}
	}
	return counts;
}
#endif
