#include "version.h"
#include "correct_bug.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "trade.h"
#include "log.h"
#include "handletime.h"
#include "buf.h"
#include "net.h"
#include "char_base.h"
#include "battle.h"
#include "npc_bus.h"
#include "char_talk.h"
#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
#include "pet_skill.h"
#endif

#define TRADE_WAIT		"③尕緊ㄛ蟀釐%s笢﹞﹞﹞"
#define TRADE_NONE		"ヶ源妦欒�刳笛閨苺�"
#define	TRADE_OVERPLAYER	"ヶ源祥硐珨弇俙模鉊ㄐ"
#define	TRADE_NOWILL		"淩岆惕Кㄛ勤源祥堋砩躲斕蝠眢ㄐ"
#define	TRADE_TRADING		"%s淏婓迵む坻�佌閡蛂丑丑�"
#define	TRADE_VANISHITEM	"隍婓華奻頗秏囮腔昜こ拸楊蝠眢ㄐ"
#define TRADE_PETFULL		"%s唾昜戲眒雛ㄛ蝠眢�＋�ㄐ"
#define	TRADE_ITEMFULL		"%s昜こ戲眒雛ㄛ③淕燴摽婬蝠眢‵"
#define TRADE_CANCEL		"%s蔚蝠眢�＋�賸ㄐ"
#define TRADE_POOR		"%sヴ祥劂ㄛ拸楊蝠眢ㄐ"
#define	TRADE_RICH		"%s踢ヴ蝠眢③昦閉徹珨啃勀ㄐ"
#define TRADE_TOORICH		"%s踢ヴ蔚頗閉徹珨啃勀ㄐ"
#define TRADE_POORLV		"%s脹撰祥劂ㄛ拸楊桽嘈蝠眢摽腔唾昜ㄐ"
#define	TRADE_LOCK		"蝠眢坶隅﹞﹞﹞"
#define	TRADE_SUCCESS		"蝠眢�洷芄�"
#define	TRADE_FAILED		"蝠眢囮啖ㄐ"
#define TRADE_FMPET		"模逜忐誘忤拸楊蝠眢ㄐ"

#define TRADE_RDCANCEL	"蝠眢�＋�ㄐ"



#ifdef _ITEM_PILEFORTRADE
enum{
	TRADE_ITEM=0,
	TRADE_PET,
	TRADE_GOLD,
};

#define MAX_TRADELISTNUM 100
typedef struct _tagTradeList{
	int charaindex;
	int fd;
	int PetTi[5];
	int ItemTi[15];
	int ItemNum[15];
	int Golds;
	int use;
}STradeList;

STradeList TradeList[MAX_TRADELISTNUM][2];

void TRADE_ResetTradeList( int ti);
int TRADE_getTradeListIndex( void);
int TRADE_getMyTarget( int meindex );
BOOL TRADE_setTradeListIndex( int ti, int meindex, int toindex);
BOOL TRADE_addSomeTradeList( int meindex, int usTi, int *num, int type);
BOOL TRADE_getSomeTradeList( int meindex, STradeList *temp);

void TRADE_InitTradeList( void)
{
	int i;
	print("場宎趙蝠眢炵苀...");
	for( i=0; i<MAX_TRADELISTNUM; i++){
		TRADE_ResetTradeList( i);
	}
	print("俇傖\n");
}

void TRADE_ResetTradeList( int ti)
{
	int i, fd;
	if( ti<0 || ti >= MAX_TRADELISTNUM )return;
	for( i=0; i<5; i++){
		TradeList[ti][0].PetTi[i] = -1;
		TradeList[ti][1].PetTi[i] = -1;
	}
	for( i=0; i<15; i++){
		TradeList[ti][0].ItemTi[i] = -1;
		TradeList[ti][0].ItemNum[i] = -1;
		TradeList[ti][1].ItemTi[i] = -1;
		TradeList[ti][1].ItemNum[i] = -1;
	}

	TradeList[ti][0].Golds = 0;
	TradeList[ti][1].Golds = 0;

	
	if( CHAR_CHECKINDEX(TradeList[ti][0].charaindex)){
		fd = getfdFromCharaIndex( TradeList[ti][0].charaindex);
		CONNECT_setTradeList( fd, -1);
	}
	if( CHAR_CHECKINDEX(TradeList[ti][1].charaindex)){
		fd = getfdFromCharaIndex( TradeList[ti][1].charaindex);
		CONNECT_setTradeList( fd, -1);
	}

	TradeList[ti][0].charaindex = -1;
	TradeList[ti][1].charaindex = -1;
	TradeList[ti][0].fd = -1;
	TradeList[ti][1].fd = -1;
	
	TradeList[ti][0].use = 0;
	TradeList[ti][1].use = 0;

}

int TRADE_getTradeListIndex( void)
{
	int i;
	static int tradeTi=0;
	if( tradeTi < 0 ) tradeTi = 0;
	for( i=0; i<MAX_TRADELISTNUM; i++){
		if( TradeList[tradeTi][0].use == 0 && TradeList[tradeTi][1].use == 0 ){
			return tradeTi;
		}
		tradeTi++;
		if( tradeTi >= MAX_TRADELISTNUM ) tradeTi = 0;

	}
	return -1;
}

BOOL TRADE_setTradeListIndex( int ti, int meindex, int toindex)
{
	int fd;
	if( ti<0 || ti >= MAX_TRADELISTNUM ) return FALSE;
	if( TradeList[ti][0].use == 1 || TradeList[ti][1].use == 1 ) return FALSE;
	TRADE_ResetTradeList( ti);
	TradeList[ti][0].charaindex = meindex;
	TradeList[ti][1].charaindex = toindex;
	TradeList[ti][0].use = 1;
	TradeList[ti][1].use = 1;

	fd = getfdFromCharaIndex( meindex);
	TradeList[ti][0].fd = fd;
	CONNECT_setTradeList( fd, ti);

	fd = getfdFromCharaIndex( toindex);
	TradeList[ti][1].fd = fd;
	CONNECT_setTradeList( fd, ti);
	return TRUE;
}

void TRADE_CheckTradeListUser( void)
{
	int i, usenum=0, replace=0, freenum=0;
	for( i=0; i<MAX_TRADELISTNUM; i++){
		if( TradeList[i][0].use == 0 && TradeList[i][1].use == 0 ){
			freenum++;
			continue;
		}
		if( CHAR_CHECKINDEX( TradeList[i][0].charaindex ) &&
			CHAR_getWorkInt( TradeList[i][0].charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE &&
			CHAR_CHECKINDEX( TradeList[i][1].charaindex ) &&
			CHAR_getWorkInt( TradeList[i][1].charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE ){
			usenum++;
			continue;
		}else{
			TRADE_ResetTradeList( i);
			replace ++;
		}
	}
}

int TRADE_getMyTarget( int meindex )
{
	int i, fd, ti, side=-1;
	fd = getfdFromCharaIndex( meindex);
	ti = CONNECT_getTradeList( fd);

	if( ti<0 || ti >= MAX_TRADELISTNUM ) return -1;
	if( TradeList[ti][0].use == 0 || TradeList[ti][1].use == 0 ) return -1;

	for( i=0; i<2; i++){
		if( TradeList[ti][i].charaindex == meindex &&
			TradeList[ti][i].fd == fd ){
			side = (i+1)%2;
			if( !CHAR_CHECKINDEX( TradeList[ti][side].charaindex) ) return -1;
			if( TradeList[ti][side].charaindex == meindex ) return -1;
			if( TradeList[ti][side].fd != getfdFromCharaIndex( TradeList[ti][side].charaindex) ) return -1;

			return TradeList[ti][side].charaindex;
		}
	}
	return -1;
}

BOOL TRADE_addSomeTradeList( int meindex, int usTi, int *num, int type)
{
	int i, fd, ti, side=-1, toindex=-1;
	fd = getfdFromCharaIndex( meindex);
	ti = CONNECT_getTradeList( fd);
	if( ti <0 || ti >= MAX_TRADELISTNUM ) return FALSE;
	toindex =-1;
	if( TradeList[ti][0].use != 1 || TradeList[ti][1].use != 1 ) return FALSE;
	for( i=0; i<2; i++){
		if( TradeList[ti][i].charaindex != meindex ){
			continue;
		}
		side = i;
		toindex = TradeList[ti][((side+1)%2)].charaindex;
		if( !CHAR_CHECKINDEX( toindex) ) return FALSE;
		break;
	}
	if( side == -1 ) return FALSE;
	switch( type){
	case TRADE_ITEM:
		{
			int itemindex, MeMaxPile;
			MeMaxPile = CHAR_getMyMaxPilenum( meindex);//郔湮剽詁杅
			if( usTi < CHAR_STARTITEMARRAY || usTi >= CHAR_MAXITEMHAVE ) return FALSE;
			itemindex = CHAR_getItemIndex( meindex, usTi );
			if( !ITEM_CHECKINDEX( itemindex))	return FALSE;

			if( ITEM_getInt( itemindex, ITEM_VANISHATDROP) == 1 ){
				char buf[256];
				sprintf( buf, "%s拸楊蝠眢﹝", ITEM_getChar( itemindex, ITEM_NAME) );
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORYELLOW);
				return FALSE;
			}

			if( ITEM_getInt( itemindex, ITEM_CANBEPILE) == 1 ){//褫剽詁
				int itemMaxPile = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
				if( itemMaxPile < 0  ) return FALSE;
				//ITEM_USEPILENUMS
				for( i=0; i<15; i++){
					if( TradeList[ti][side].ItemTi[i] != usTi ) continue;
					if( TradeList[ti][side].ItemNum[i] >= MeMaxPile ||
						TradeList[ti][side].ItemNum[i] >= itemMaxPile ) return FALSE;

					TradeList[ti][side].ItemNum[i] += 1;
					if( num != NULL ) *num = TradeList[ti][side].ItemNum[i];
					return TRUE;
				}
			}
			for( i=0; i<15; i++){
				if( TradeList[ti][side].ItemTi[i] != -1 ) continue;
				TradeList[ti][side].ItemTi[i] = usTi;
				TradeList[ti][side].ItemNum[i] = 1;
				if( num != NULL ) *num = TradeList[ti][side].ItemNum[i];
				return TRUE;
			}
		}
		break;
	case TRADE_PET:
		{
			int petindex;
			if( usTi < 0 || usTi >= CHAR_MAXPETHAVE ) return FALSE;
			petindex = CHAR_getCharPet( meindex, usTi );
			if( !CHAR_CHECKINDEX(petindex))	return FALSE;
			if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 ){
				CHAR_talkToCli( meindex, -1, "模逜忐誘忤拸楊蝠眢﹝", CHAR_COLORYELLOW);
				return FALSE;//模逜忐誘忤
			}
			if( CHAR_getWorkInt( toindex, CHAR_PickAllPet) != TRUE ){
				if( CHAR_getInt( petindex, CHAR_LV) > (CHAR_getInt( toindex, CHAR_LV)+5) &&
					(CHAR_getInt( toindex, CHAR_TRANSMIGRATION ) <= 0 ) ){
					CHAR_talkToCli( meindex, -1, "勤源拸楊桽嘈蜆唾昜﹝", CHAR_COLORYELLOW);
					return FALSE;
				}
			}
			for( i=0; i<5; i++){
				if( TradeList[ti][side].PetTi[i] == usTi ) return FALSE;
			}
			for( i=0; i<5; i++){
				if( TradeList[ti][side].PetTi[i] != -1 ) continue;
				TradeList[ti][side].PetTi[i] = usTi;
				return TRUE;
			}
		}
		break;
	case TRADE_GOLD:
		TradeList[ti][side].Golds = usTi;
		return TRUE;
		break;
	}

	return FALSE;
}

BOOL TRADE_getSomeTradeList( int meindex, STradeList *temp)
{
	int i, fd, ti, side=-1;
	fd = getfdFromCharaIndex( meindex);
	ti = CONNECT_getTradeList( fd);
	if( ti <0 || ti >= MAX_TRADELISTNUM ) return FALSE;
	if( TradeList[ti][0].use != 1 || TradeList[ti][1].use != 1 ) return FALSE;
	for( i=0; i<2; i++){
		if( TradeList[ti][i].charaindex != meindex ) continue;
		side = i;
		break;
	}
	if( side == -1 ) return FALSE;
	
	memcpy( temp, &TradeList[ti][side], sizeof( STradeList) );
	return TRUE;
}
BOOL TRADE_CheckTradeList( int meindex, STradeList *temp1, int toindex, STradeList *temp2);
#endif

void TRADE_Will(int fd, int meindex, char* message);
void TRADE_ShowItem(int fd, int meindex, char* message);
void TRADE_Close(int fd, int meindex, char* message);

void TRADE_SwapItem(int meindex, int toindex, char* message, int fd, char* mycharaname, int tofd, char* tocharaname);

int TRADE_CheckItembuf(int fd, int meindex, int toindex, int tofd, char* mycharaname, char* tocharaname);
//BOOL TRADE_ChangeItem(int meindex, int toindex, char *a, char *b, int item1, int item2, int itemindex1, int itemindex2);
BOOL TRADE_HandleItem( int meindex, int showindex, char *message, char *outmess);
BOOL TRADE_HandleGold( int meindex, int showindex, char *message, char *outmess);
BOOL TRADE_HandlePet( int meindex, int showindex, char *message, char *outmess);



void CHAR_Trade(int fd, int index, char* message)
{
   char		firstToken[64];
   // shan 眕狟蛁裁岆祥斛猁腔雄釬  2002/03/05
   //char		messageeraseescape[512];
   char*	messagebody;
   
   {   		
   	if (*message == 0) 	return;
    if (!CHAR_CHECKINDEX(index))	return;

   	CHAR_getMessageBody( message, firstToken, sizeof(firstToken), &messagebody);

	switch( tolower( firstToken[0]) ){
	   case 'd':	// 扆梑ヶ源俙模
		
		TRADE_Search(fd, index, message);
	   	break;
/*	   case 'c':
	   	TRADE_Will(fd, index, message);
	   	break;
*/	   case 't':	// 珆尨蝠眢腔昜こ﹜踢ヴ﹜唾昜
	   	TRADE_ShowItem(fd, index, message);
	   	break;
	   case 'w':	// 壽敕蝠眢
		print(" TRADE_Close:%d ", index);
		TRADE_Close(fd, index, message);
	   	break;
	   default:
	   	break;
	}
   }
}

BOOL TRADE_Search(int fd, int meindex, char* message)
{
   int		objbuf[16];
   int		front_x, front_y, i, found_count;
   BOOL		found =  FALSE, searchflg = FALSE;
   int 		cnt = 0, tofd = -1, checkfd = -1;
   char		msgbuf[1024], mycharaname[256], tocharaname[256];
   char		token[256];

   if (!CHAR_CHECKINDEX(meindex))	return FALSE;

#if 0	// Robin fix
//#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族戰GM指令
	{
		char	msgbuf[512];
		snprintf( msgbuf, sizeof( msgbuf), "輻陎炵模逜PKㄛ拸楊蝠眢");
		CHAR_talkToCli( meindex, -1, msgbuf, CHAR_COLORWHITE);
		return FALSE;
	}
#endif

   // �觷瞍眢棧疥直閡趼郅羷閡訬禷併俶繰閨韐池�
   if (CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING
   	|| CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK)
   		return FALSE;

   // �觷瞍眢棧疥臥曊蚖藑蓿煦俶繰閨韐池�
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   	|| (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   		return FALSE;

#ifdef _STREET_VENDOR
	 // �觷瞍眕痚确胱赲輕池簀閡�
		if(CHAR_getWorkInt(meindex,CHAR_WORKSTREETVENDOR) > -1) return FALSE;
#endif

   strcpy(mycharaname, CHAR_getChar(meindex, CHAR_NAME));

   if (getStringFromIndexWithDelim(message, "|", 2, token,
	sizeof(token)) == FALSE)	return FALSE;
   // 扢隅袨怓蝠眢③⑴笢
//   CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_SENDING);
   
#ifndef _ITEM_PILEFORTRADE
	for (i = 0; i < CONNECT_WINDOWBUFSIZE; i++)
		CONNECT_setTradecharaindex(fd, i, -1);
#endif
   CHAR_getCoordinationDir(CHAR_getInt(meindex, CHAR_DIR),
   		CHAR_getInt(meindex, CHAR_X),
   		CHAR_getInt(meindex, CHAR_Y),
   		1, &front_x, &front_y);

	found_count = CHAR_getSameCoordinateObjects( objbuf, arraysizeof(objbuf),
							CHAR_getInt(meindex, CHAR_FLOOR),front_x, front_y);
	// ヶ源羶衄俙模麼勤源壽敕蝠眢恁砐
	if (found_count == 0){
		CHAR_talkToCli(meindex, -1, TRADE_NONE, CHAR_COLORYELLOW);
		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
		return FALSE;
	}
   
	for (i = 0; i < found_count; i++){
		int	objindex = objbuf[i];
		int	index = OBJECT_getIndex(objindex);

		if (OBJECT_getType(objindex) != OBJTYPE_CHARA)	continue;
		if (CHAR_getInt(index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER)	continue;
		if (index == meindex)	continue;
		found = TRUE;
		if (CHAR_getWorkInt(index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)	continue;
		if (CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)	continue;
		if (CHAR_getFlg(index, CHAR_ISTRADE) == FALSE)	continue;
		if (CHAR_getWorkInt(index, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)	continue;
		strcpy(tocharaname, CHAR_getChar(index, CHAR_NAME));
		tofd = getfdFromCharaIndex(index);
		if (tofd == -1){
			CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
			return	FALSE;
		}	
		if (searchflg){
			if (tofd != checkfd)	continue;
			if (strcmp(token, tocharaname) != 0)	continue;
		}
#ifndef _ITEM_PILEFORTRADE
		CONNECT_setTradecharaindex(fd, cnt, index);
#endif
		cnt ++;
		if (cnt == CONNECT_WINDOWBUFSIZE)	break;
	}

	if (cnt == 0){
		// ヶ源羶衄俙模麼勤源壽敕蝠眢恁砐
		if (found){
			CHAR_talkToCli(meindex, -1, TRADE_NOWILL, CHAR_COLORYELLOW);
		}else
			CHAR_talkToCli(meindex, -1, TRADE_NONE, CHAR_COLORYELLOW);

		sprintf(msgbuf, "C|%d|%s|0", tofd, tocharaname);
		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
		return FALSE;
	}
	// ヶ源衄珨弇俙模
	if (cnt == 1){
#ifdef _ITEM_PILEFORTRADE
		int ti;
#endif
		int toindex;
		toindex = CONNECT_getCharaindex(tofd);
		if( !CHAR_CHECKINDEX(toindex))       return FALSE;
#ifdef _ITEM_PILEFORTRADE
		
		if( (ti = TRADE_getTradeListIndex()) < 0 ||
			TRADE_setTradeListIndex( ti, meindex, toindex) == FALSE ){

			CHAR_talkToCli( meindex, -1, "炵苀疆繕笢﹝", CHAR_COLORYELLOW);
			CHAR_setWorkInt( meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
			TRADE_CheckTradeListUser();

   			return FALSE;
		}
		TRADE_setTradeListIndex( ti, meindex, toindex);
#endif
		snprintf(msgbuf, sizeof(msgbuf), TRADE_WAIT, tocharaname);
   		CHAR_talkToCli(meindex, -1, msgbuf, CHAR_COLORYELLOW);

   		sprintf(msgbuf, "C|%d|%s|1", fd, mycharaname);
   		lssproto_TD_send( tofd, -1, msgbuf);
   		sprintf(msgbuf, "C|%d|%s|1", tofd, tocharaname);
   		lssproto_TD_send(fd, -1, msgbuf);

   		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_TRADING);
   		CHAR_setWorkInt(toindex, CHAR_WORKTRADEMODE, CHAR_TRADE_TRADING);

		CONNECT_set_confirm(fd, FALSE);
		CONNECT_set_confirm(tofd, FALSE);

        CHAR_sendTradeEffect(meindex, 1);
        CHAR_sendTradeEffect(toindex, 1);
        return TRUE;
   }else if (cnt > 1){	// ヶ源祥硐珨弇俙模
		CHAR_talkToCli(meindex, -1, TRADE_OVERPLAYER, CHAR_COLORYELLOW);
//		sprintf(msgbuf, "C|%d|%s|0", tofd, tocharaname);
		CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
   		return FALSE;
    }

	CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	return FALSE;
}

void TRADE_Close(int fd, int meindex, char* message)
{
	char msg[128], mycharaname[256];
	int tofd=-1, toindex=-1;
	int j;
#ifndef _ITEM_PILEFORTRADE
	char token[256];
#endif
	if (!CHAR_CHECKINDEX(meindex))	return;
	if (*message == 0)	return;
	strcpy(mycharaname, CHAR_getChar(meindex, CHAR_NAME));
#ifdef _ITEM_PILEFORTRADE
	toindex = TRADE_getMyTarget( meindex);

	if( CHAR_CHECKINDEX( toindex) ){
		tofd = getfdFromCharaIndex( toindex);
		snprintf( msg, sizeof(msg), TRADE_CANCEL, mycharaname);
		CHAR_talkToCli(toindex, -1, msg, CHAR_COLORYELLOW);
		snprintf( msg, sizeof(msg), "W|%d|%s", fd, mycharaname);
		lssproto_TD_send( tofd, -1, msg);

		CHAR_setWorkInt( toindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
		CONNECT_setTradeTmp( tofd, "");
		CHAR_sendStatusString( toindex, "i");
		CHAR_sendStatusString( toindex, "P");
		CHAR_sendTradeEffect( toindex, 0);
		CONNECT_set_confirm( tofd, FALSE);

		for(j = 0; j < CHAR_MAXPETHAVE; j++){
			char msgbuf[256];
			int petindex = CHAR_getCharPet( toindex, j);
			if( !CHAR_CHECKINDEX( petindex) ) continue;
			CHAR_complianceParameter( petindex );
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( toindex, msgbuf );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( toindex, msgbuf );
		}
	}

#else
   if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)	return;
   tofd = atoi(token);
   toindex = CONNECT_getCharaindex(tofd);
   if (!CHAR_CHECKINDEX(toindex))       return;
   if (getStringFromIndexWithDelim(message, "|", 3, token, sizeof(token)) == FALSE)	return;

   
   snprintf(msg, sizeof(msg), TRADE_CANCEL, mycharaname);
   CHAR_talkToCli(toindex, -1, msg, CHAR_COLORYELLOW);
   snprintf( msg, sizeof(msg), "W|%d|%s", fd, mycharaname);
   lssproto_TD_send( tofd, -1, msg);

   CHAR_setWorkInt(toindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
   CONNECT_setTradeTmp(tofd, "");
   CHAR_sendStatusString( CONNECT_getCharaindex(tofd), "i");
   CHAR_sendStatusString( CONNECT_getCharaindex(tofd), "P");
   CHAR_sendTradeEffect( toindex, 0);
   CONNECT_set_confirm(tofd, FALSE);
#endif

	if( !CHAR_CHECKINDEX( toindex) ){
		char buf[256];
		if( getStringFromIndexWithDelim( message, "|", 2, buf, sizeof(buf)) == FALSE)	return;
		tofd = atoi( buf);
		toindex = CONNECT_getCharaindex( tofd);
		if (!CHAR_CHECKINDEX(toindex) )return;
	}


	snprintf( msg, sizeof(msg), TRADE_RDCANCEL);
	CHAR_talkToCli(meindex, -1, msg, CHAR_COLORYELLOW);
	snprintf( msg, sizeof(msg), "W|%d|%s", tofd, CHAR_getChar( toindex, CHAR_NAME) );
	lssproto_TD_send( fd, -1, msg);

	CHAR_setWorkInt( meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CONNECT_setTradeTmp(fd, "");
	CHAR_sendStatusString(meindex, "i");
	CHAR_sendStatusString(meindex, "P");
	CHAR_sendTradeEffect( meindex, 0);
	CONNECT_set_confirm(fd, FALSE);
// end
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		char msgbuf[256];
		int petindex = CHAR_getCharPet( meindex, j);
		if( !CHAR_CHECKINDEX( petindex) ) continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( meindex, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( meindex, msgbuf );
	}

#ifdef _ITEM_PILEFORTRADE
	{
		int ti=0;
		ti = CONNECT_getTradeList( fd);
		if( ti <0 || ti >= MAX_TRADELISTNUM ) return;
		TRADE_ResetTradeList( ti);
	}
#endif

}

void TRADE_ShowItem(int fd, int meindex, char* message)
{
   char token[128], mycharaname[256], tocharaname[256], msg[2048];
   int tofd, showindex, toindex;// itemindex, gold, tmpgold, item;

   int MaxGold;

   MaxGold = CHAR_getMaxHaveGold( meindex);
   strcpy( mycharaname, CHAR_getChar(meindex, CHAR_NAME));
#ifdef _ITEM_PILEFORTRADE
	toindex = TRADE_getMyTarget( meindex);
	if( !CHAR_CHECKINDEX( toindex) ){
		print(" TRADE_Close_Err1:%d ", toindex);
		TRADE_Close(fd, meindex, message);
		return;
	}
	if( meindex != TRADE_getMyTarget( toindex) ){
		print(" TRADE_Close_Err2:%d->%d ", meindex, toindex);
		TRADE_Close(fd, meindex, message);
		return;
	}
	tofd = getfdFromCharaIndex(toindex);
#else
   if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)	return;
   tofd = atoi(token);
   toindex = CONNECT_getCharaindex(tofd);
#endif

   if (!CHAR_CHECKINDEX(toindex))       return;
   // �觷瞍眢棧疥臥曊蚖藑蓿煦俶繰閨韐池�
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   	|| (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   		return;

   if ((CHAR_getWorkInt(toindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   	|| (CHAR_getWorkInt(toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   		return;

	if (getStringFromIndexWithDelim(message, "|", 3, tocharaname, sizeof(tocharaname)) == FALSE) return;
	if (getStringFromIndexWithDelim(message, "|", 5, token, sizeof(token)) == FALSE) return;
	showindex = atoi(token);
	if (getStringFromIndexWithDelim(message, "|", 4, token, sizeof(token)) == FALSE) return; 
	
	showindex = toindex;
	strcpy( tocharaname, CHAR_getChar(toindex, CHAR_NAME));

	switch(tolower(token[0])){
	case 'i':
		{
			char buf1[2048];
			if( TRADE_HandleItem( meindex, showindex, message, buf1) == FALSE ){
				print(" TRADE_Close_Err3:%d,%d,%s,%s ", meindex, showindex, message, buf1);
				TRADE_Close(fd, meindex, message);
				return;
			}else{
				snprintf(msg, sizeof(msg), "T|%d|%s|I|%d|%s", fd, mycharaname, showindex, buf1 );
				lssproto_TD_send(tofd, -1, msg);
			}
		}
		break;
   	case 'g':
		{
			char buf1[2048];
			if( TRADE_HandleGold( meindex, showindex, message, buf1) == FALSE ){
				print(" TRADE_Close_Err4:%d,%d,%s,%s ", meindex, showindex, message, buf1);
				TRADE_Close(fd, meindex, message);
				return;
			}else{// 蔚郗蝠眢眳踢ヴ換跤勤源
				snprintf(msg, sizeof(msg), "T|%d|%s|G|%d|%s", fd, mycharaname, showindex, buf1);
				lssproto_TD_send( tofd, -1, msg);
			}
		}
		break;
	case 'p':
		{
			char buf1[2048];
			if( TRADE_HandlePet( meindex, showindex, message, buf1) == FALSE ){
				print(" TRADE_Close_Err5:%d,%d,%s,%s ", meindex, showindex, message, buf1);
				TRADE_Close(fd, meindex, message);
				return;
			}else{
				snprintf(msg, sizeof(msg), "T|%d|%s|P|%d|%s",
					fd, mycharaname, showindex, buf1);
				lssproto_TD_send(tofd, -1, msg);
			}
		}
		break;
   	case 'k':
	    if((CONNECT_get_confirm(fd)==TRUE) &&
			(CONNECT_get_confirm(tofd)==TRUE) ){//絞邧源飲偌狟 lock
			if( CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK ){
				//andy_log
				print("ANDY err 滅砦菴媼棒輛��!!\n");
				return;//滅砦菴媼棒輛��
			}
			CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_LOCK);
			//潰脤勤源岆瘁偌狟 ok
			if( CHAR_getWorkInt( toindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_LOCK )return;

			snprintf(msg, sizeof(msg), "T|%d|%s|A", fd, mycharaname);
			lssproto_TD_send( tofd, -1, msg);
			TRADE_SwapItem(meindex, toindex, message, fd, mycharaname, tofd, tocharaname);
		}
   	break;
	case 'c':
        if( CONNECT_get_confirm( fd) == TRUE ) return;
		CONNECT_set_confirm(fd, TRUE);
		snprintf( msg, sizeof(msg), "T|%d|%s|C", fd, mycharaname);
		lssproto_TD_send( tofd, -1, msg);
	break;
   }
	return;
}

void TRADE_SwapItem( int meindex, int toindex, char* message, int fd, char* mycharaname, int tofd, char* tocharaname)
{
   int result;
   char msg[128], tmpmsg[128];

#ifndef _ITEM_PILEFORTRADE
   CONNECT_setTradeTmp(fd, message);
#endif

   result = TRADE_CheckItembuf(fd, meindex, toindex, tofd, mycharaname, tocharaname);
#ifndef _ITEM_PILEFORTRADE
   toindex = CONNECT_getCharaindex(tofd);
#endif

   if (!CHAR_CHECKINDEX(toindex))       return;
   if (result == -1)
   	strcpy(msg, TRADE_FAILED);
   else if (result == -2)
   	sprintf(msg, TRADE_ITEMFULL, mycharaname);
   else if (result == -3)
   	sprintf(msg, TRADE_ITEMFULL, tocharaname);
   else if (result == -4)
   	sprintf(msg, TRADE_RICH, mycharaname);
   else if (result == -5)
   	sprintf(msg, TRADE_POOR, mycharaname);
   else if (result == -6)
   	sprintf(msg, TRADE_RICH, tocharaname);
   else if (result == -7)
   	sprintf(msg, TRADE_POOR, tocharaname);
   else if (result == -8)
   	sprintf(msg, TRADE_TOORICH, mycharaname);
   else if (result == -9)
   	sprintf(msg, TRADE_TOORICH, tocharaname);
   else if (result == -10)
   	sprintf(msg, TRADE_PETFULL, mycharaname);
   else if (result == -11)
   	sprintf(msg, TRADE_PETFULL, tocharaname);
   else if (result == -12)
   	strcpy(msg, TRADE_VANISHITEM);
   else if (result == -13)
   	sprintf(msg, TRADE_POORLV, mycharaname);
   else if (result == -14)
   	sprintf(msg, TRADE_POORLV, tocharaname);
   else if (result == -15)
   	strcpy(msg, TRADE_FMPET);
   else if (result == 1)
   	strcpy(msg, TRADE_SUCCESS);
   else if (result == 2)
   	strcpy(msg, TRADE_LOCK);

   if (result != 2){
   	snprintf(tmpmsg, sizeof(tmpmsg), "W|%d|%s", fd, mycharaname);
   	lssproto_TD_send(tofd, -1, tmpmsg);
   	snprintf(tmpmsg, sizeof(tmpmsg), "W|%d|%s", tofd, tocharaname);
   	lssproto_TD_send(fd, -1, tmpmsg);
   	
	CHAR_setWorkInt(meindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CHAR_setWorkInt(toindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CONNECT_setTradeTmp(fd, "");
	CONNECT_setTradeTmp(tofd, "");
	CHAR_sendTradeEffect( meindex, 0);
	CHAR_sendTradeEffect( toindex, 0);   	
   }
	CHAR_sendStatusString(meindex, "i");
	CHAR_sendStatusString(toindex, "i");
	CHAR_sendStatusString(meindex, "P");
	CHAR_sendStatusString(toindex, "P");
	{
		int j, petindex;
		char msgbuf[256];
		for(j = 0; j < CHAR_MAXPETHAVE; j++){
			petindex = CHAR_getCharPet( meindex, j);
			if( !CHAR_CHECKINDEX( petindex) ) continue;
			CHAR_complianceParameter( petindex );
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( meindex, msgbuf );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( meindex, msgbuf );
		}
		for(j = 0; j < CHAR_MAXPETHAVE; j++){
			petindex = CHAR_getCharPet( toindex, j);
			if( !CHAR_CHECKINDEX( petindex) ) continue;
			CHAR_complianceParameter( petindex );
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( toindex, msgbuf );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( toindex, msgbuf );
		}
	}

   CHAR_talkToCli(meindex, -1, msg, CHAR_COLORYELLOW);
   CHAR_talkToCli(toindex, -1, msg, CHAR_COLORYELLOW);   
}

#ifdef _ITEM_PILEFORTRADE

int TRADE_HandleTrade_DelItem( int charaindex, int MyMaxPile, int MaxPile, int *Item, int *nums, int *indexlist)
{
	int i, k;
	
	for( i=0; i<50; i++){
		indexlist[i] = -1;
	}
	k=0;
	for( i=0; i<15; i++ ){
		int pilenum, itemindex, totalnums, newindex;
		if( Item[i] == -1 ) continue;
		itemindex = CHAR_getItemIndex( charaindex, Item[i] );
		if( !ITEM_CHECKINDEX( itemindex)) return -1;
		pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
		pilenum -= nums[i];
		if( pilenum < 0 ) return -1;
		if( pilenum == 0 ){
			CHAR_setItemIndex( charaindex, Item[i], -1);
			indexlist[k] = itemindex;
			k++;
			totalnums = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
			newindex = itemindex;
		}else{
			ITEM_setInt( itemindex, ITEM_USEPILENUMS, pilenum);
			newindex = ITEM_makeItemAndRegist( ITEM_getInt( itemindex, ITEM_ID));
			if( !ITEM_CHECKINDEX( newindex) ) return -1;
			indexlist[k] = newindex;
			k++;
			totalnums = nums[i];
		}
		if( totalnums > MaxPile ){
			totalnums -= MaxPile;
			ITEM_setInt( newindex, ITEM_USEPILENUMS, MaxPile);
		}else{
			ITEM_setInt( newindex, ITEM_USEPILENUMS, totalnums);
			totalnums = 0;
		}

		while( totalnums > 0 ){
			newindex = ITEM_makeItemAndRegist( ITEM_getInt( itemindex, ITEM_ID));
			if( !ITEM_CHECKINDEX( newindex) ) return -1;
			indexlist[k] = newindex;
			k++;
			if( totalnums > MaxPile ){
				totalnums -= MaxPile;
				ITEM_setInt( newindex, ITEM_USEPILENUMS, MaxPile);
			}else{
				ITEM_setInt( newindex, ITEM_USEPILENUMS, totalnums);
				totalnums = 0;
			}
		}
	}
	return k;
}

int TRADE_HandleTrade_DelPet( int charaindex, int *Pet, int *indexlist)
{
	int i, k, petindex;
	k=0;
	for( i=0; i<5; i++){
		indexlist[i] = -1;
	}
	for( i=0; i<5; i++ ){
		if( Pet[i] == -1 ) continue;
		petindex = CHAR_getCharPet( charaindex, Pet[i] );
		if( !CHAR_CHECKINDEX( petindex)) return -1;
		CHAR_setCharPet( charaindex, Pet[i], -1);
		indexlist[k] = petindex;
		k++;
	}
	return k;
}

BOOL TRADE_HandleTrade_DelGold( int charaindex, int sGold, int *Gold)
{
	int MyGold, MyMaxGold;
	
	MyGold = CHAR_getInt( charaindex, CHAR_GOLD);
	MyMaxGold = CHAR_getMaxHaveGold( charaindex);
	
	MyGold -= sGold;
	if( MyGold < 0 || MyGold > MyMaxGold ){
		MyGold = (MyGold<0)?0:MyGold;
		MyGold = (MyGold>MyMaxGold)?MyMaxGold:MyGold;
		CHAR_setInt( charaindex, CHAR_GOLD, MyGold);
		return FALSE;
	}
	CHAR_setInt( charaindex, CHAR_GOLD, MyGold);
	*Gold = sGold;
	return TRUE;
}

BOOL TRADE_HandleTrade_AddItem( int charaindex, int *Item)
{
	int ret, i;
	for( i=0; i<50; i++){
		if( Item[i] == -1 )break;
		ret = CHAR_addItemSpecificItemIndex( charaindex, Item[i]);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			int j;
			for( j=i; j< 50; j++)
				ITEM_endExistItemsOne( Item[j]);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL TRADE_HandleTrade_AddPet( int charaindex, int *Pet)
{
	int havepetelement, i;
	for( i=0; i<5; i++){
		havepetelement = CHAR_getCharPetElement( charaindex);
		if( Pet[i] == -1 ) break;
		if( havepetelement < 0 ){
			int j;
			for( j=i; j<5; j++){
				CHAR_endCharOneArray( Pet[j] );
			}
			return FALSE;
		
		}
		CHAR_setCharPet( charaindex, havepetelement, Pet[i]);
		CHAR_setWorkInt( Pet[i], CHAR_WORKPLAYERINDEX, charaindex);
		CHAR_setChar( Pet[i], CHAR_OWNERCDKEY, CHAR_getChar( charaindex, CHAR_CDKEY));
		CHAR_setChar( Pet[i], CHAR_OWNERCHARANAME, CHAR_getChar( charaindex, CHAR_NAME));
		CHAR_complianceParameter( Pet[i]);
	}
	return TRUE;
}

BOOL TRADE_HandleTrade_AddGold( int charaindex, int sGold)
{
	int MyGold, MyMaxGold;
	MyGold = CHAR_getInt( charaindex, CHAR_GOLD);
	MyMaxGold = CHAR_getMaxHaveGold( charaindex);
	
	MyGold += sGold;
	
	MyGold = (MyGold<0)?0:MyGold;
	MyGold = (MyGold>MyMaxGold)?MyMaxGold:MyGold;
	
	CHAR_setInt( charaindex, CHAR_GOLD, MyGold);
	return TRUE;
}

BOOL TRADE_HandleTrade( int meindex, STradeList *temp1, int toindex, STradeList *temp2 )
{
	int MeMaxPile, ToMaxPile, ret;
	int MeItem[50], ToItem[50];
	int MePet[5], ToPet[5];
	int MeGold=0, ToGold=0;
	
	MeMaxPile = CHAR_getMyMaxPilenum( meindex);//郔湮剽詁杅
	ToMaxPile = CHAR_getMyMaxPilenum( toindex);
	//痄壺meindex
	//痄壺toindex
	{
		char buf[256];

		memset( buf, 0, sizeof( buf));
		sprintf( buf, "A.%s[%s] trade B.%s[%s] FXY[%d,%d,%d]",
			CHAR_getChar( meindex, CHAR_NAME), CHAR_getChar(meindex, CHAR_CDKEY),
			CHAR_getChar( toindex, CHAR_NAME), CHAR_getChar(toindex, CHAR_CDKEY),
			CHAR_getInt( meindex, CHAR_FLOOR), CHAR_getInt( meindex, CHAR_X), CHAR_getInt( meindex, CHAR_Y) );
		LogTrade(buf);
	}

	//痄壺
	if( (ret=TRADE_HandleTrade_DelItem( meindex, MeMaxPile, ToMaxPile, temp1->ItemTi, temp1->ItemNum, ToItem ))== -1 ) return FALSE;
	if( (ret=TRADE_HandleTrade_DelItem( toindex, ToMaxPile, MeMaxPile, temp2->ItemTi, temp2->ItemNum, MeItem ))== -1 ) return FALSE;

	if( (ret=TRADE_HandleTrade_DelPet( meindex, temp1->PetTi, ToPet)) == -1 ) return FALSE;
	if( (ret=TRADE_HandleTrade_DelPet( toindex, temp2->PetTi, MePet)) == -1 ) return FALSE;
	if( TRADE_HandleTrade_DelGold( meindex, temp1->Golds, &ToGold) == FALSE ) return FALSE;
	if( TRADE_HandleTrade_DelGold( toindex, temp2->Golds, &MeGold) == FALSE ) return FALSE;
	//樓��
	if( TRADE_HandleTrade_AddItem( meindex, MeItem) == FALSE ) return FALSE;
	if( TRADE_HandleTrade_AddItem( toindex, ToItem) == FALSE ) return FALSE;
	if( TRADE_HandleTrade_AddPet( meindex, MePet) == FALSE) return FALSE;
	if( TRADE_HandleTrade_AddPet( toindex, ToPet) == FALSE) return FALSE;

	TRADE_HandleTrade_AddGold( meindex, MeGold);
	TRADE_HandleTrade_AddGold( toindex, ToGold);

	{
		char buf1[4096], buf2[4096], buf[256];
		int i;
		sprintf( buf1, "A.ITEM.");
		sprintf( buf2, "B.ITEM.");
		for( i=0; i<50; i++){
			if( ITEM_CHECKINDEX( ToItem[i]) ){
				sprintf( buf, "%s[%s]*%d,",
					ITEM_getChar( ToItem[i], ITEM_NAME), ITEM_getChar( ToItem[i], ITEM_UNIQUECODE),
					ITEM_getInt( ToItem[i], ITEM_USEPILENUMS) );
				strcat( buf1, buf);
			}
			if( ITEM_CHECKINDEX( MeItem[i]) ){
				sprintf( buf, "%s[%s]*%d,",
					ITEM_getChar( MeItem[i], ITEM_NAME), ITEM_getChar( MeItem[i], ITEM_UNIQUECODE),
					ITEM_getInt( MeItem[i], ITEM_USEPILENUMS) );
				strcat( buf2, buf);
			}
		}
		strcat( buf1, "PET.");
		strcat( buf2, "PET.");
		for( i=0; i<5; i++){
			if( CHAR_CHECKINDEX( ToPet[i]) ){
				sprintf( buf, "%s[%s]%s-%d",
					CHAR_getUseName( ToPet[i]), CHAR_getChar( ToPet[i], CHAR_UNIQUECODE),
					CHAR_getChar( ToPet[i], CHAR_NAME), CHAR_getInt( ToPet[i], CHAR_LV) );
				strcat( buf1, buf);
			}
			if( CHAR_CHECKINDEX( MePet[i]) ){
				sprintf( buf, "%s[%s]%s-%d",
					CHAR_getUseName( MePet[i]), CHAR_getChar( MePet[i], CHAR_UNIQUECODE),
					CHAR_getChar( MePet[i], CHAR_NAME), CHAR_getInt( MePet[i], CHAR_LV) );
				strcat( buf2, buf);
			}
		}
		sprintf( buf, "GOLD:%d", ToGold);
		strcat( buf1, buf);
		sprintf( buf, "GOLD:%d", MeGold);
		strcat( buf2, buf);
		LogTrade( buf1);
		LogTrade( buf2);
	}

	return TRUE;
}

int TRADE_CheckItembuf(int fd, int meindex, int toindex, int tofd, char* mycharaname, char* tocharaname)
{
	int Tradeti;
	STradeList TradeListTemp[2];
	if( TRADE_getMyTarget( meindex) != toindex ) return -1;
	Tradeti = CONNECT_getTradeList( fd);
	if( Tradeti <0 || Tradeti >= MAX_TRADELISTNUM ) return -1;
	if( TradeList[Tradeti][0].use != 1 || TradeList[Tradeti][1].use != 1 ) return -1;
	if( TRADE_getSomeTradeList( meindex, &TradeListTemp[0]) == FALSE ) return -1;
	if( TRADE_getSomeTradeList( toindex, &TradeListTemp[1]) == FALSE ) return -1;
	//�毓阮梉�
	if( TRADE_CheckTradeList( meindex, &TradeListTemp[0], toindex, &TradeListTemp[1]) == FALSE ) return -1;
	//痄壺�冼懭篽�
	//樓�躽冼懭篽�
	if( TRADE_HandleTrade( meindex, &TradeListTemp[0], toindex, &TradeListTemp[1] ) == FALSE )	return -1;
	//蝠眢賦旰雄釬
	return 1;
}

#else	
/*
int TRADE_CheckItembuf(int fd, int meindex, int toindex, int tofd, char* mycharaname, char* tocharaname)
{
#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	int myItem[42] = { -1 } ;			//扂源垀衄腔蝠眢囀�搊鉭�
	int oppItem[42] = { -1 };			//勤源垀衄腔蝠眢囀�搊鉭�
	int ii , jj; 
	int Iitem1[15] = { 0 } ;			//扂源耋撿腔index
	int Iitem2[15] = { 0 } ;			//勤源耋撿腔index
	int ppet1[5] = { 0 } ;				//扂源唾昜腔index
	int ppet2[5] = { 0 } ;				//勤源唾昜腔index
	BOOL tradeflag = TRUE ;
	int count = 0 ; 
#endif

   char a[16], b[16], c[16], d[16], e[16], f[16];
   char g[16], h[16], i[16], j[16], k[16], l[16], token[16];
   int itemindex1 = 0, itemindex2 = 0, itemindex3 = 0;
   int itemindex4 = 0, itemindex5 = 0, itemindex6 = 0;
   int toitemindex1 = 0, toitemindex2 = 0, toitemindex3 = 0;
   int toitemindex4 = 0, toitemindex5 = 0, toitemindex6 = 0;
   int item1 = 0, item2 = 0, item4 = 0, item5 = 0, pet3 = 0, pet6 = 0;
   int pet1 = 0, pet2 = 0;
   int swapitem1 = 0, swapitem2 = 0, gold1 = 0, gold2 = 0;
   char itembuf[256], toitembuf[256], buf[4048];
   
   CONNECT_getTradeTmp(fd, itembuf, sizeof(itembuf));
   CONNECT_getTradeTmp(tofd, toitembuf, sizeof(toitembuf));


#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
   //�△襞珝誼齾郇閡袎硜搊鉭�
   for ( ii = 0  ; ii < 42 ; ii ++ ) {
	   if (getStringFromIndexWithDelim(itembuf, "|", 6 + ii * 2 , token, sizeof(token)) == TRUE)
		   myItem[ii] = atoi(token);
   }
   //�△繹埸誼齾郇閡袎硜搊鉭�
   for ( ii = 0  ; ii < 42 ; ii ++ ) {
	   if (getStringFromIndexWithDelim(toitembuf, "|", 6 + ii * 2 , token, sizeof(token)) == TRUE)
		   oppItem[ii] = atoi(token);
   }
#else
   // 掀勤邧源眳郔摽蝠眢衪隅
   if (getStringFromIndexWithDelim(itembuf, "|", 5, token, sizeof(token)) == TRUE)
	   	strcpy(a, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 6, token, sizeof(token)) == TRUE)
		itemindex1 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 7, token, sizeof(token)) == TRUE)
	   	strcpy(b, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 8, token, sizeof(token)) == TRUE)
		itemindex2 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 9, token, sizeof(token)) == TRUE)
	   	strcpy(c, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 10, token, sizeof(token)) == TRUE)
		itemindex3 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 11, token, sizeof(token)) == TRUE)
	   	strcpy(d, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 12, token, sizeof(token)) == TRUE)
		itemindex4 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 13, token, sizeof(token)) == TRUE)
	   	strcpy(e, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 14, token, sizeof(token)) == TRUE)
		itemindex5 = atoi(token);
   if (getStringFromIndexWithDelim(itembuf, "|", 15, token, sizeof(token)) == TRUE)
		strcpy(f, token);
   if (getStringFromIndexWithDelim(itembuf, "|", 16, token, sizeof(token)) == TRUE)
		itemindex6 = atoi(token);

   if (getStringFromIndexWithDelim(toitembuf, "|", 5, token, sizeof(token)) == TRUE)
	   	strcpy(g, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 6, token, sizeof(token)) == TRUE)
		toitemindex1 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 7, token, sizeof(token)) == TRUE)
	   	strcpy(h, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 8, token, sizeof(token)) == TRUE)
		toitemindex2 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 9, token, sizeof(token)) == TRUE)
	   	strcpy(i, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 10, token, sizeof(token)) == TRUE)
		toitemindex3 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 11, token, sizeof(token)) == TRUE)
	   	strcpy(j, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 12, token, sizeof(token)) == TRUE)
		toitemindex4 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 13, token, sizeof(token)) == TRUE)
	   	strcpy(k, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 14, token, sizeof(token)) == TRUE)
		toitemindex5 = atoi(token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 15, token, sizeof(token)) == TRUE)
	   	strcpy(l, token);
   if (getStringFromIndexWithDelim(toitembuf, "|", 16, token, sizeof(token)) == TRUE)
		toitemindex6 = atoi(token);
#endif 

#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
   //潰脤衄拸笭葩蝠眢腔耋撿
   for ( ii = 0 ; ii < 15 ; ii ++ ) {
	   for ( jj = ii + 1 ; jj < 15 ; jj ++ ) {
		   if ( myItem[ii] == myItem[jj] )
			   if ( myItem[ii] != -1 )
				   return -1 ; 
	  }
   }
   for ( ii = 21 ; ii < 36 ; ii ++ ) {
	   for ( jj = ii + 1 ; jj < 36 ; jj++ ) {
		   if ( myItem[ii] == myItem[jj] )
			   if ( myItem[ii] != -1 )
				   return -1 ; 
	   }
   }
#else
   // shan hjj add 炴耋撿
   if (itemindex1==itemindex2){
        if (itemindex1!=-1){
            return -1;
        }
   }
   if (itemindex4==itemindex5){
        if (itemindex4!=-1){  
            return -1;
        }
   }
   // End
#endif

#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
   //掀勤邧源蝠眢腔昜こ岆瘁飲淏��
   for ( ii = 0 ; ii < 21 ; ii ++ ) {
	   if ( myItem[ii] != oppItem[ii+21] ) {
		   tradeflag = FALSE ; 
		   break;
	   }		   
   }
   for ( ii = 21 ; ii < 42 ; ii ++ ){
	   if ( myItem[ii] != oppItem[ii-21]) {
		   tradeflag = FALSE;
		   break;
	   }
   }
   //邧源囀�楙��滇羌播靇迡池�
   if ( tradeflag == TRUE ) {
   //數呾邧源耋撿腔樓熬
   for ( ii = 0 ; ii < 15 ; ii ++ ) {
	   if ( myItem[ii] != -1 ) {
		   swapitem1--;
		   swapitem2++;
	   }
   }
   for ( ii = 21 ; ii < 36 ; ii ++ ) {
	   if ( myItem[ii] != -1 ) {
		   swapitem1++;
		   swapitem2--;
	   }
   }
#else
   if ((strcmp(a, j) == 0) && (strcmp(b, k) == 0) && (strcmp(c,l) == 0)
   	&& (strcmp(d, g) == 0) && (strcmp(e, h) == 0) && (strcmp(f, i) == 0)
   	&& (itemindex1 == toitemindex4) && (itemindex2 == toitemindex5)
   	&& (itemindex3 == toitemindex6) && (itemindex4 == toitemindex1)
   	&& (itemindex5 == toitemindex2) && (itemindex6 == toitemindex3))
   {
   	// 潰桄邧源昜こ戲岆瘁衄諾弇
	if ((strcmp(a, "I") == 0) && (itemindex1 != -1)){
		swapitem1--;
		swapitem2++;
	}
	if ((strcmp(b, "I") == 0) && (itemindex2 != -1)){
		swapitem1--;
		swapitem2++;
	}
	if ((strcmp(d, "I") == 0) && (itemindex4 != -1)){
		swapitem1++;
		swapitem2--;
	}
	if ((strcmp(e, "I") == 0) && (itemindex5 != -1)){
		swapitem1++;
		swapitem2--;
	}
#endif		
	if (swapitem1 > CHAR_findTotalEmptyItem(meindex))
		return	-2;
	toindex = CONNECT_getCharaindex(tofd);
	if (!CHAR_CHECKINDEX(toindex))       return -1;
	if (swapitem2 > CHAR_findTotalEmptyItem(toindex))
		return	-3;
#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	//�△羸遙輓躅蟓�
	if ( myItem[20] != -1 ) gold1 += myItem[20] ;
	if ( myItem[41] != -1 ) gold2 += myItem[41] ; 
#else
	// 潰脤邧源踢ヴ
	if ((strcmp(a, "G") == 0) && (itemindex1 != -1))	gold1 += itemindex1;
	if ((strcmp(b, "G") == 0) && (itemindex2 != -1))	gold1 += itemindex2;
	if ((strcmp(d, "G") == 0) && (itemindex4 != -1))	gold2 += itemindex4;
	if ((strcmp(e, "G") == 0) && (itemindex5 != -1))	gold2 += itemindex5;
#endif
	if (gold1 > CHAR_getMaxHaveGold( meindex) )	return	-4;
	if (gold1 > CHAR_getInt(meindex, CHAR_GOLD))	return	-5;
	if (gold2 > CHAR_getMaxHaveGold( toindex))	return	-6;
	if (gold2 > CHAR_getInt(toindex, CHAR_GOLD))	return	-7;
	if ((gold2 + CHAR_getInt(meindex, CHAR_GOLD) - gold1) > CHAR_getMaxHaveGold( meindex) )	return	-8;
	if ((gold1 + CHAR_getInt(toindex, CHAR_GOLD) - gold2) > CHAR_getMaxHaveGold( toindex))	return	-9;

#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	//數呾邧源唾昜腔樓熬
	for ( ii = 15 ; ii < 20 ; ii ++ ) {
		if ( myItem[ii] != -1 ) {
			pet1--;	
			pet2++;
		}
	}
	for ( ii = 36 ; ii < 41 ; ii ++ ) {
		if ( myItem[ii] != -1 ) {
			pet1++;	
			pet2--;
		}
	}
#else
	// 潰脤邧源唾昜	
	if ((strcmp(c, "P") == 0) && (itemindex3 != -1))
	{
		pet1--;	pet2++;
	}
	if ((strcmp(f, "P") == 0) && (itemindex6 != -1))
	{
		pet1++;	pet2--;
	}
#endif

#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	if ((pet1 > 5) || (pet2 > 5))	return	-10;
	//�△襠奐褕篽狦傮霾議駘懮�講
	for( ii = 0; ii < CHAR_MAXPETHAVE; ii ++ ) {
		if( CHAR_getCharPet( meindex , ii ) == -1  ) count ++ ;
	}
	if ( pet1 > count )	return -10;
	count = 0 ; 
	for( ii = 0; ii < CHAR_MAXPETHAVE; ii ++ ) {
		if( CHAR_getCharPet( toindex , ii ) == -1  ) count ++ ;
	}
	if ( pet2 > count )	return -11;
#else
	if ((pet1 > 1) || (pet2 > 1))	return	-10;
	if ((pet1 == 1) && (CHAR_getCharPetElement( meindex) == -1))	return -10;
	if ((pet2 == 1) && (CHAR_getCharPetElement( toindex) == -1))	return -11;
#endif	
#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	//�△襠齾倬�蝠眢腔耋撿index
	for ( ii = 0 ; ii < 15 ; ii ++ ) {
		if ( myItem[ii] != -1 ) 
			Iitem1[ii] = CHAR_getItemIndex(meindex, myItem[ii]);
		else 
			Iitem1[ii] = -1 ;
		if ( myItem[ii + 21 ] != -1 ) 
			Iitem2[ii] = CHAR_getItemIndex(toindex, myItem[ii + 21]);
		else 
			Iitem2[ii] = -1 ;
	}
#else
	if ((strcmp(a, "I") == 0) && (itemindex1 != -1))
		item1 = CHAR_getItemIndex(meindex, itemindex1);
	else	item1 = -1;
	if ((strcmp(b, "I") == 0) && (itemindex2 != -1))
		item2 = CHAR_getItemIndex(meindex, itemindex2);
	else	item2 = -1;
	if ((strcmp(d, "I") == 0) && (itemindex4 != -1))
		item4 = CHAR_getItemIndex(toindex, itemindex4);
	else	item4 = -1;
	if ((strcmp(e, "I") == 0) && (itemindex5 != -1))
		item5 = CHAR_getItemIndex(toindex, itemindex5);
	else	item5 = -1;
#endif
#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	//�△譚�蝠眢唾昜腔index
	for ( ii = 15 ; ii < 20 ; ii ++ ) {
		if ( myItem[ii] != -1 ) {
			if ( CHAR_getInt( meindex , CHAR_RIDEPET) == myItem[ii] )
				return -1 ; 
			ppet1[ii - 15 ] = CHAR_getCharPet( meindex , myItem[ii] ) ;
		}
		else 
			ppet1[ii - 15 ] = -1 ; 
	}
	for ( ii = 36 ; ii < 41 ; ii ++ ) {
		if ( myItem[ii] != -1 ) {
			if ( CHAR_getInt( toindex , CHAR_RIDEPET) == myItem[ii] )
				return -1 ; 
			ppet2[ ii - 36 ] = CHAR_getCharPet( toindex , myItem[ii] ) ;
		}
		else 
			ppet2[ ii - 36 ] = -1 ; 
	}
#else
	if (itemindex3 != -1)
	{
		if( CHAR_getInt( meindex, CHAR_RIDEPET) == itemindex3 )
			return -1;
		pet3 = CHAR_getCharPet(meindex, itemindex3);
	}
	else 	pet3 = -1;
	if (itemindex6 != -1)
	{
		if( CHAR_getInt( toindex, CHAR_RIDEPET) == itemindex6 )
			return -1;
		pet6 = CHAR_getCharPet(toindex, itemindex6);
	}
	else	pet6 = -1;
#endif



#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	for ( ii = 0 ; ii < 15 ; ii ++ ) {
		if ( ITEM_getInt( Iitem1[ii] , ITEM_VANISHATDROP ) == 1 )
			return -12 ; 
		if ( ITEM_getInt( Iitem2[ii] , ITEM_VANISHATDROP ) == 1 )
			return -12 ; 
	}
#else
	if ((ITEM_getInt(item1, ITEM_VANISHATDROP) == 1)||
		(ITEM_getInt(item2, ITEM_VANISHATDROP) == 1)||
		(ITEM_getInt(item4, ITEM_VANISHATDROP) == 1)||
		(ITEM_getInt(item5, ITEM_VANISHATDROP) ==1))
			return	-12;
#endif


#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	//唾昜脹撰夔瘁蝠眢瓚剿
	if( CHAR_getWorkInt( meindex, CHAR_PickAllPet) != TRUE )	{
		for ( ii = 0 ; ii < 5 ; ii ++ ) {
			if ( (ppet2[ii] != -1 ) && ( CHAR_getInt( ppet2[ii] , CHAR_LV ) 
				- CHAR_getInt(meindex, CHAR_LV) > 5) 
				&& (CHAR_getInt(meindex,CHAR_TRANSMIGRATION) <= 0))
				return -13 ; 
		}
	}
	if( CHAR_getWorkInt( toindex, CHAR_PickAllPet) != TRUE )	{
		for ( ii = 0 ; ii < 5 ; ii ++ ) {
			if ( (ppet1[ii] != -1 ) && ( CHAR_getInt( ppet1[ii] , CHAR_LV ) 
				- CHAR_getInt(toindex, CHAR_LV) > 5) 
				&& (CHAR_getInt(toindex,CHAR_TRANSMIGRATION) <= 0))
				return -13 ; 
		}
	}
#else
	if( CHAR_getWorkInt( meindex, CHAR_PickAllPet) != TRUE )	{
		if ((pet6 != -1) && (CHAR_getInt(pet6, CHAR_LV)
			 - CHAR_getInt(meindex, CHAR_LV) > 5)
			 && (CHAR_getInt(meindex,CHAR_TRANSMIGRATION) <= 0))
		 		return -13;

	}
	if( CHAR_getWorkInt( toindex, CHAR_PickAllPet) != TRUE )	{

		if ((pet3 != -1) && (CHAR_getInt(pet3, CHAR_LV)
			 - CHAR_getInt(toindex, CHAR_LV) > 5)
			 && (CHAR_getInt(toindex,CHAR_TRANSMIGRATION) <= 0))
		 		return -14;
	}
#endif
#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	//岆瘁峈模逜忐誘忤
	for ( ii = 0 ; ii < 5 ; ii ++ ) {
		if ( ( ppet1[ii] != -1 ) && (CHAR_getInt(ppet1[ii], CHAR_PETFAMILY) == 1))
			return -15;
		if ( ( ppet2[ii] != -1 ) && (CHAR_getInt(ppet2[ii], CHAR_PETFAMILY) == 1))
			return -15;
	}
#else
	if ((pet6 != -1) && (CHAR_getInt(pet6, CHAR_PETFAMILY) == 1))
		 	return -15;
	if ((pet3 != -1) && (CHAR_getInt(pet3, CHAR_PETFAMILY) == 1))
		 	return -15;
#endif

#ifdef _TRADESYSTEM2	// (不可開) Syu ADD 新交易系統
	// 蝠遙俙模耋撿﹜踢ヴ﹜唾昜	
	for ( ii = 0 ; ii < 15 ; ii ++ ) {
		TRADE_ChangeItem(meindex, toindex, "I", "I", Iitem1[ii], Iitem2[ii],
			myItem[ii], oppItem[ii]);
	}
	for ( ii = 15 ; ii < 20 ; ii ++ ) {
		TRADE_ChangeItem(meindex, toindex, "P", "P", ppet1[ii - 15 ], ppet2[ii-15],
			myItem[ii], oppItem[ii]);
	}
	TRADE_ChangeItem(meindex, toindex, "G", "G", Iitem1[20], Iitem2[41],
		myItem[20], oppItem[20]);
#else
	TRADE_ChangeItem(meindex, toindex, a, d, item1, item4,
		itemindex1, itemindex4);
	TRADE_ChangeItem(meindex, toindex, b, e, item2, item5,
		itemindex2, itemindex5);
	TRADE_ChangeItem(meindex, toindex, c, f, pet3, pet6,
		itemindex3, itemindex6);
#endif

	// 迡�� trade.log
	{
		char petname1[256], petname2[256];
		int pet1lv, pet2lv;
		int logitem1 = 0, logitem2 = 0, logitem4 = 0, logitem5 = 0;

		if ((strcmp(a, "I") == 0) && (item1 != -1))	logitem1 = ITEM_getInt(item1, ITEM_ID);
		if ((strcmp(b, "I") == 0) && (item2 != -1))	logitem2 = ITEM_getInt(item2, ITEM_ID);
		if ((strcmp(a, "G") == 0) && (item1 == -1))	logitem1 = itemindex1;
		if ((strcmp(b, "G") == 0) && (item2 == -1))	logitem2 = itemindex2;
		if ((strcmp(c, "P") == 0) && (pet3 == -1))
		{
			sprintf(petname1, "NONE");
			pet1lv = 0;
		}
		else
		{
			sprintf(petname1, "%s", CHAR_getChar(pet3, CHAR_NAME));
			pet1lv = CHAR_getInt(pet3, CHAR_LV);
		}
		if ((strcmp(f, "P") == 0) && (pet6 == -1))
		{
			sprintf(petname2, "NONE");
			pet2lv = 0;
		}
		else
		{
			sprintf(petname2, "%s", CHAR_getChar(pet6, CHAR_NAME));
			pet2lv = CHAR_getInt(pet6, CHAR_LV);
		}
		if ((strcmp(d, "I") == 0) && (item4 != -1))	logitem4 = ITEM_getInt(item4, ITEM_ID);
		if ((strcmp(e, "I") == 0) && (item5 != -1))	logitem5 = ITEM_getInt(item5, ITEM_ID);
		if ((strcmp(d, "G") == 0) && (item4 == -1))	logitem4 = itemindex4;
		if ((strcmp(e, "G") == 0) && (item5 == -1))	logitem5 = itemindex5;
	        sprintf(buf, "%s\t%s\t(%s[%d,%s],%s[%d,%s],%s[%s,%d,%s]) <-> %s\t%s\t(%s[%d,%s],%s[%d,%s],%s[%s,%d,%s])",
                        CHAR_getChar(meindex, CHAR_CDKEY), mycharaname,
                        a, logitem1, ITEM_getChar( item1, ITEM_UNIQUECODE), b, logitem2, ITEM_getChar( item2, ITEM_UNIQUECODE),
						c, petname1, pet1lv, CHAR_getChar( pet3, CHAR_UNIQUECODE),
                        CHAR_getChar(toindex, CHAR_CDKEY), tocharaname,
                        d, logitem4, ITEM_getChar( item4, ITEM_UNIQUECODE), e, logitem5, ITEM_getChar( item5, ITEM_UNIQUECODE), 
						f, petname2, pet2lv, CHAR_getChar( pet6, CHAR_UNIQUECODE));
		LogTrade(buf);
	}
	
        return 1;
   }// if
   else
	return 2;
}
*/
#endif
/*
BOOL TRADE_ChangeItem(int meindex, int toindex, char *a, char *b,
	int item1, int item2, int itemindex1, int itemindex2)
{
   int gold1 = 0, gold2 = 0, eptitem = -1;
   if (CHAR_CHECKINDEX(meindex) == FALSE)	return FALSE;
   if (CHAR_CHECKINDEX(toindex) == FALSE)	return FALSE;
   // 耋撿 <-> 耋撿
   if ((strcmp(a, "I") == 0) && (strcmp(b, "I") == 0))
   {
   	// 拸 <-> 耋撿
   	if ((itemindex1 == -1) && (itemindex2 > 0))
   	{
   	   if (ITEM_CHECKINDEX(item2) == FALSE)	return FALSE;
   	   eptitem = CHAR_findEmptyItemBox(meindex);
   	   if (eptitem < 0)	return FALSE;
   	   CHAR_setItemIndex(meindex, eptitem, item2);
   	   ITEM_setWorkInt(item2, ITEM_WORKCHARAINDEX, meindex);
   	   ITEM_setWorkInt(item2, ITEM_WORKOBJINDEX, -1);
   	   CHAR_setItemIndex(toindex, itemindex2, -1);
   	}
   	// 耋撿 <-> 拸
   	else if ((itemindex1 > 0) && (itemindex2 == -1))
   	{
   	   if (ITEM_CHECKINDEX(item1) == FALSE)	return FALSE;
   	   eptitem = CHAR_findEmptyItemBox(toindex);
   	   if (eptitem < 0)	return FALSE;
   	   CHAR_setItemIndex(toindex, eptitem, item1);
   	   ITEM_setWorkInt(item1, ITEM_WORKCHARAINDEX, toindex);
   	   ITEM_setWorkInt(item1, ITEM_WORKOBJINDEX, -1);
   	   CHAR_setItemIndex(meindex, itemindex1, -1);
   	}
   	// 耋撿 <-> 耋撿
   	else if (itemindex1 > 0 && itemindex2 > 0)
   	{
   	   if (ITEM_CHECKINDEX(item1) == FALSE)	return FALSE;
   	   if (ITEM_CHECKINDEX(item2) == FALSE)	return FALSE;
   	   ITEM_setWorkInt(item1, ITEM_WORKCHARAINDEX, toindex);
   	   ITEM_setWorkInt(item1, ITEM_WORKOBJINDEX, -1);
   	   ITEM_setWorkInt(item2, ITEM_WORKCHARAINDEX, meindex);
   	   ITEM_setWorkInt(item2, ITEM_WORKOBJINDEX, -1);
   	   CHAR_setItemIndex(toindex, itemindex2, item1);
   	   CHAR_setItemIndex(meindex, itemindex1, item2);
   	}
   }
   // 耋撿 <-> 踢ヴ
   else if ((strcmp(a, "I") == 0) && (strcmp(b, "G") == 0))
   {
   	if (itemindex1 > 0)
   	{
      	   if (ITEM_CHECKINDEX(item1) == FALSE)	return FALSE;
      	   eptitem = CHAR_findEmptyItemBox(toindex);
      	   if (eptitem < 0)	return FALSE;
      	   ITEM_setWorkInt(item1, ITEM_WORKCHARAINDEX, toindex);
      	   ITEM_setWorkInt(item1, ITEM_WORKOBJINDEX, -1);
      	   CHAR_setItemIndex(toindex, eptitem, item1);
      	   CHAR_setItemIndex(meindex, itemindex1, -1);
   	}
	if (itemindex2 < 0)	itemindex2 = 0;
   	gold1 = CHAR_getInt(meindex, CHAR_GOLD);
   	gold1 += itemindex2;
   	CHAR_setInt(meindex, CHAR_GOLD, gold1);
   	gold2 = CHAR_getInt(toindex, CHAR_GOLD);
   	gold2 -= itemindex2;
   	CHAR_setInt(toindex, CHAR_GOLD, gold2);
   }
   // 踢ヴ <-> 耋撿
   else if ((strcmp(a, "G") == 0) && (strcmp(b, "I") == 0))
   {
   	if (itemindex2 > 0)
   	{
      	   if (ITEM_CHECKINDEX(item2) == FALSE)	return FALSE;
      	   eptitem = CHAR_findEmptyItemBox(meindex);
      	   if (eptitem < 0)	return FALSE;
      	   ITEM_setWorkInt(item2, ITEM_WORKCHARAINDEX, meindex);
      	   ITEM_setWorkInt(item2, ITEM_WORKOBJINDEX, -1);
      	   CHAR_setItemIndex(meindex, eptitem, item2);
      	   CHAR_setItemIndex(toindex, itemindex2, -1);
   	}
   	if (itemindex1 < 0)	itemindex1 = 0;
   	gold1 = CHAR_getInt(meindex, CHAR_GOLD);
   	gold1 -= itemindex1;
   	CHAR_setInt(meindex, CHAR_GOLD, gold1);
   	gold2 = CHAR_getInt(toindex, CHAR_GOLD);
   	gold2 += itemindex1;
   	CHAR_setInt(toindex, CHAR_GOLD, gold2);
   }
   // 踢ヴ <-> 踢ヴ
   else if ((strcmp(a, "G") == 0) && (strcmp(b, "G") == 0))
   {
   	if (itemindex1 < 0)	itemindex1 = 0;
   	if (itemindex2 < 0)	itemindex2 = 0;
   	gold1 = CHAR_getInt(toindex, CHAR_GOLD);
   	gold1 = gold1 + itemindex1 - itemindex2;
   	CHAR_setInt(toindex, CHAR_GOLD, gold1);
   	gold2 = CHAR_getInt(meindex, CHAR_GOLD);
   	gold2 = gold2 + itemindex2 - itemindex1;
   	CHAR_setInt(meindex, CHAR_GOLD, gold2);
   }
   // 唾昜 <-> 唾昜
   else if ((strcmp(a, "P") == 0) && (strcmp(b, "P") == 0))
   {
	char category[8];
   	if ((item1 != -1) && (CHAR_CHECKINDEX(item1) == FALSE))	return FALSE;
   	if ((item2 != -1) && (CHAR_CHECKINDEX(item2) == FALSE))	return FALSE;

   	if ((itemindex1 != -1) && (itemindex1 == CHAR_getInt(meindex, CHAR_DEFAULTPET)))
   	{
   		int fd;
   		CHAR_setInt(meindex, CHAR_DEFAULTPET, -1);
   		fd = getfdFromCharaIndex(meindex);
		if (fd != -1){
			CHAR_setWorkInt( meindex, CHAR_WORK_PET0_STAT+itemindex1-1, 0);
	   		lssproto_KS_send(fd, itemindex1, 0);
		}
   	}
   	if ((itemindex2 != -1)&& (itemindex2 == CHAR_getInt(toindex, CHAR_DEFAULTPET)))
   	{
   		int tofd;
   		CHAR_setInt(toindex, CHAR_DEFAULTPET, -1);
   		tofd = getfdFromCharaIndex(toindex);
		if (tofd != -1){
			CHAR_setWorkInt( toindex, CHAR_WORK_PET0_STAT+itemindex1-1, 0);			
	   		lssproto_KS_send(tofd, itemindex2, 0);
		}
   	}

	if (item1 != -1)
		CHAR_setWorkInt(item1, CHAR_WORKPLAYERINDEX, toindex);
	if (item2 != -1)
		CHAR_setWorkInt(item2, CHAR_WORKPLAYERINDEX, meindex);
	if (itemindex1 == -1)
		itemindex1 = CHAR_getCharPetElement( meindex);
	if (itemindex2 == -1)
		itemindex2 = CHAR_getCharPetElement( toindex);
	CHAR_setCharPet(meindex, itemindex1, item2);
	CHAR_setCharPet(toindex, itemindex2, item1);
	if ((itemindex1 != -1) && (item1 != -1))	{

			CHAR_setChar(item1, CHAR_OWNERCDKEY,
				CHAR_getChar(toindex, CHAR_CDKEY));
			CHAR_setChar(item1, CHAR_OWNERCHARANAME,
				CHAR_getChar(toindex, CHAR_NAME));

		CHAR_complianceParameter(item1);
		CHAR_setInt(item1, CHAR_PUTPETTIME, 0);
	}
	if ((itemindex2 != -1) && (item2 != -1))	{

		CHAR_setChar(item2, CHAR_OWNERCDKEY,
			CHAR_getChar(meindex, CHAR_CDKEY));
		CHAR_setChar(item2, CHAR_OWNERCHARANAME,
			CHAR_getChar(meindex, CHAR_NAME));

		CHAR_complianceParameter(item2);
		CHAR_setInt(item2, CHAR_PUTPETTIME, 0);
	}
	snprintf(category, sizeof(category), "K%d", itemindex1);
	CHAR_sendStatusString(meindex, category);
	snprintf(category, sizeof(category), "W%d", itemindex1);
	CHAR_sendStatusString(meindex, category);
	snprintf(category, sizeof(category), "K%d", itemindex2);
	CHAR_sendStatusString(toindex, category);
	snprintf(category, sizeof(category), "W%d", itemindex2);
	CHAR_sendStatusString(toindex, category);
   }
   return	TRUE;
}
*/
BOOL TRADE_HandleItem( int meindex, int showindex, char *message, char *outmess)
{
	char token[256];
	int item, itemindex;
	int fd = getfdFromCharaIndex( meindex );

	if( CONNECT_get_confirm( fd) == TRUE ) return FALSE;

	if( CHAR_getWorkInt( meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_TRADING
		|| CHAR_getWorkInt( meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK )
		return FALSE;

	if( getStringFromIndexWithDelim( message, "|", 6, token, sizeof( token)) == FALSE) return FALSE;
	item = atoi( token);
	if( item < CHAR_STARTITEMARRAY  || item > CHAR_MAXITEMHAVE ){
		return FALSE;
	}
	itemindex = CHAR_getItemIndex(meindex, item);
	if( !ITEM_CHECKINDEX( itemindex))	return FALSE;

	{
		char itemname[256], tmpbuf[256], tmpbuf1[256];
		int crushe;

#ifdef _ITEM_PILEFORTRADE
		int nums = 0;
		if( TRADE_addSomeTradeList( meindex, item, &nums, TRADE_ITEM) == FALSE ){
			return FALSE;
		}
#endif
		strcpy( itemname, ITEM_getChar(itemindex, ITEM_SECRETNAME));//ITEM_NAME
		crushe	= ITEM_getItemDamageCrusheED( itemindex);
		sprintf( token, "祥頗囷輓");

		if( crushe >= 0 ) snprintf( token, sizeof(token), "%d%%", crushe );
#ifdef _ITEM_PILENUMS
		sprintf( outmess, "%d|%s|%s|%s|%d|%s|%d",
#else
		sprintf( outmess, "%d|%s|%s|%d|%s",
#endif
	   					ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
						makeEscapeString( ITEM_getChar( itemindex, ITEM_NAME), tmpbuf1, sizeof(tmpbuf1)),
	   					makeEscapeString( itemname, tmpbuf, sizeof(tmpbuf)),
						ITEM_getChar(itemindex,ITEM_EFFECTSTRING), item, token
#ifdef _ITEM_PILEFORTRADE
						,nums
#endif
						);
	}
	return TRUE;
}

BOOL TRADE_HandleGold( int meindex, int showindex, char *message, char *outmess)
{
	int gold, tmpgold;
	char token[256];
	int fd = getfdFromCharaIndex( meindex );
	if(CONNECT_get_confirm( fd)==TRUE)return FALSE;
	if (CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_TRADING
		|| CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK)
		return FALSE;
	tmpgold = CHAR_getInt(meindex, CHAR_GOLD);
	if (getStringFromIndexWithDelim(message, "|", 6, token, sizeof(token)) == FALSE) return FALSE;
	if( (gold = atoi(token)) < 0 ) return FALSE;
	if( gold > tmpgold )return FALSE;
#ifdef _ITEM_PILEFORTRADE
	if( TRADE_addSomeTradeList( meindex, gold, NULL, TRADE_GOLD) == FALSE ) return FALSE;
#endif
	sprintf( outmess, "%d", gold);
	return TRUE;
}

BOOL TRADE_HandlePet( int meindex, int showindex, char *message, char *outmess)
{
	int havepetindex, petindex;
	char token[256], buf[256];
	int fd = getfdFromCharaIndex( meindex );
	if(CONNECT_get_confirm( fd)==TRUE)return FALSE;
	if (CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_TRADING
		|| CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_LOCK)
		return FALSE;
	if (getStringFromIndexWithDelim(message, "|", 6, token, sizeof(token)) == FALSE) return FALSE;
	if( (havepetindex = atoi(token)) < 0 ) return FALSE;
	petindex = CHAR_getCharPet(meindex, havepetindex);
	if( !CHAR_CHECKINDEX(petindex))	return FALSE;

#ifdef _ITEM_PILEFORTRADE
	if( TRADE_addSomeTradeList( meindex, havepetindex, NULL, TRADE_PET) == FALSE ) return FALSE;
#endif
	sprintf( token, "%d|%s|%d|%d|%d|%d|%d|%d",
			CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER),
			makeEscapeString( CHAR_getUseName(petindex), buf, sizeof(buf)),
			CHAR_getInt( petindex, CHAR_LV),
			CHAR_getWorkInt( petindex, CHAR_WORKATTACKPOWER),
			CHAR_getWorkInt( petindex, CHAR_WORKDEFENCEPOWER),
			CHAR_getWorkInt( petindex, CHAR_WORKQUICK),
			havepetindex, 
			CHAR_getInt( petindex , CHAR_TRANSMIGRATION));

#ifdef _TRADESYSTEM2
	{
		int i;
		char skillname[7][256];
		char buf1[256],buf2[256];
		for( i=0; i<7; i++){
			int skillarray, skillID;
			memset( skillname[i], 0, sizeof(skillname[i]));
			skillID = CHAR_getPetSkill( petindex, i);
			skillarray = PETSKILL_getPetskillArray( skillID);
			if( !PETSKILL_CHECKINDEX( skillarray)) continue;
			sprintf( skillname[i], "%s", PETSKILL_getChar( skillarray, PETSKILL_NAME) );
		}
#ifdef _SHOW_FUSION
		sprintf( outmess , "%s|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d",
#else
		sprintf( outmess , "%s|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s",
#endif
			token , CHAR_getWorkInt(petindex , CHAR_WORKMAXHP) , CHAR_getInt(petindex , CHAR_SLOT),
			skillname[0] , skillname[1] , skillname[2] , skillname[3] ,
			skillname[4] , skillname[5] , skillname[6] ,
			makeEscapeString( CHAR_getChar( petindex, CHAR_NAME ), buf1, sizeof(buf1)),
			makeEscapeString( CHAR_getUseName( petindex), buf2, sizeof(buf2))
#ifdef _SHOW_FUSION
			,CHAR_getInt ( petindex, CHAR_FUSIONBEIT) ) ;
#else
			);
#endif
	}
#else
	sprintf( outmess , "%s|%d", token , CHAR_getWorkInt(petindex , CHAR_WORKMAXHP) );
#endif //_TRADESYSTEM2
	return TRUE;
}

#ifdef _ITEM_PILEFORTRADE
BOOL TRADE_CheckTradeList( int meindex, STradeList *temp1, int toindex, STradeList *temp2)
{
	int i;
	int MeSurplus=0, MeNeeds=0, MeMaxPile;
	int ToSurplus=0, ToNeeds=0, ToMaxPile;


	MeMaxPile = CHAR_getMyMaxPilenum( meindex);//郔湮剽詁杅
	ToMaxPile = CHAR_getMyMaxPilenum( toindex);

	MeSurplus = CHAR_findSurplusItemBox( meindex);
	ToSurplus = CHAR_findSurplusItemBox( toindex);
	//耋撿
	ToNeeds=0; MeNeeds=0;
	for( i=0; i<15; i++ ){
		int pilenum, itemindex;
		if( temp1->ItemTi[i] == -1 ) continue;
		itemindex = CHAR_getItemIndex( meindex, temp1->ItemTi[i] );
		if( !ITEM_CHECKINDEX( itemindex))	return FALSE;
		pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
		if( temp1->ItemNum[i] == pilenum ){
			MeSurplus++;
		}
		if( temp1->ItemNum[i] > ToMaxPile ){
			ToNeeds += (temp1->ItemNum[i]/ToMaxPile) + 1;
		}else{
			ToNeeds++;
		}
	}
	for( i=0; i<15; i++ ){
		int pilenum, itemindex;
		if( temp2->ItemTi[i] == -1 ) continue;
		itemindex = CHAR_getItemIndex( toindex, temp2->ItemTi[i] );
		if( !ITEM_CHECKINDEX( itemindex))	return FALSE;
		pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
		if( temp2->ItemNum[i] == pilenum){
			ToSurplus++;
		}
		if( temp2->ItemNum[i] > MeMaxPile ){
			MeNeeds += (temp2->ItemNum[i]/MeMaxPile) + 1;
		}else {
			MeNeeds++;
		}
	}
	
	if( MeSurplus < MeNeeds ){
		CHAR_talkToCli( meindex, -1, "蝠眢摽昜こ戲弇祥逋﹝", CHAR_COLORYELLOW);
		CHAR_talkToCli( toindex, -1, "勤源蝠眢摽昜こ戲弇祥逋﹝", CHAR_COLORYELLOW);
		return FALSE;
	}
	if( ToSurplus < ToNeeds ){
		CHAR_talkToCli( toindex, -1, "蝠眢摽昜こ戲弇祥逋﹝", CHAR_COLORYELLOW);
		CHAR_talkToCli( meindex, -1, "勤源蝠眢摽昜こ戲弇祥逋﹝", CHAR_COLORYELLOW);
		return FALSE;
	}

	//唾昜
	MeSurplus = CHAR_findSurplusPetBox( meindex);
	ToSurplus = CHAR_findSurplusPetBox( toindex);
	ToNeeds=0; MeNeeds=0;
	for( i=0; i<5; i++ ){
		int petindex;
		if( temp1->PetTi[i] == -1 ) continue;
		petindex = CHAR_getCharPet( meindex, temp1->PetTi[i] );
		if( !CHAR_CHECKINDEX( petindex))	return FALSE;
		MeSurplus++;
		ToNeeds++;
	}
	for( i=0; i<5; i++ ){
		int petindex;
		if( temp2->PetTi[i] == -1 ) continue;
		petindex = CHAR_getCharPet( toindex, temp2->PetTi[i] );
		if( !CHAR_CHECKINDEX( petindex))	return FALSE;
		ToSurplus++;
		MeNeeds++;
	}

	if( MeSurplus < MeNeeds ){
		CHAR_talkToCli( meindex, -1, "蝠眢摽唾昜戲弇祥逋﹝", CHAR_COLORYELLOW);
		CHAR_talkToCli( toindex, -1, "勤源蝠眢摽唾昜戲弇祥逋﹝", CHAR_COLORYELLOW);
		return FALSE;
	}
	if( ToSurplus < ToNeeds ){
		CHAR_talkToCli( meindex, -1, "勤源蝠眢摽唾昜戲弇祥逋﹝", CHAR_COLORYELLOW);
		CHAR_talkToCli( toindex, -1, "蝠眢摽唾昜戲弇祥逋﹝", CHAR_COLORYELLOW);
		return FALSE;
	}

	//踢ヴ
	MeMaxPile = CHAR_getMaxHaveGold( meindex);
	ToMaxPile = CHAR_getMaxHaveGold( toindex);
	MeSurplus = MeMaxPile - CHAR_getInt( meindex, CHAR_GOLD);
	ToSurplus = ToMaxPile - CHAR_getInt( toindex, CHAR_GOLD);
	ToNeeds=0; MeNeeds=0;
	MeSurplus += temp1->Golds;
	ToSurplus += temp2->Golds;
	ToNeeds = temp1->Golds;
	MeNeeds = temp2->Golds;
	if( MeSurplus < MeNeeds ){
		CHAR_talkToCli( meindex, -1, "蝠眢摽坒啟閉徹奻癹﹝", CHAR_COLORYELLOW);
		CHAR_talkToCli( toindex, -1, "勤源蝠眢摽坒啟閉徹奻癹﹝", CHAR_COLORYELLOW);
		return FALSE;
	}
	if( ToSurplus < ToNeeds ){
		CHAR_talkToCli( meindex, -1, "勤源蝠眢摽坒啟閉徹奻癹﹝", CHAR_COLORYELLOW);
		CHAR_talkToCli( toindex, -1, "蝠眢摽坒啟閉徹奻癹﹝", CHAR_COLORYELLOW);
		return FALSE;
	}
	return TRUE;
}
#endif
