#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_transerman.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_makepair.h"
#include "npc_eventaction.h"

#ifdef _NPC_MAKEPAIR
#define MAXPAIRNUMS 30
PairMenu PairList[2][MAXPAIRNUMS];

enum {
	WINDOW_START=10,
	WINDOW_USER,
	WINDOW_SELECT,	//心理测验
	WINDOW_PAIR,	//配对找对象
	WINDOW_PAIROK,
	WINDOW_PAIRNO,
	WINDOW_WARP,	//跳跃
	WINDOW_END,		//结束
	WINDOW_TALKSTART=20,
	WINDOW_TALKSELECT,
	WINDOW_TALKPAIR,
	WINDOW_RETURN,
	WINDOW_TALKEND,
};
enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
	NPC_WORK_PAIRTEAMS = CHAR_NPCWORKINT2,
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
#define MAKEPAIR_STANDBY 100*60
static void NPC_MakePair_selectWindow( int meindex, int toindex, int num,int select);
BOOL MakePairMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token);
void CheckPairUsers( int *male, int *female);
BOOL NPC_PairForage( int meindex, int toindex, char *arg, char *token);
BOOL NPC_PairRegister( int toindex);
void NPC_PairUserAndWarp( int meindex, int toindex, int forindex, char *arg);
int NPC_getPairCode( int toindex);
int NPC_getTitleMsg( int meindex, int toindex, char *arg, char *token, int title);
int NPC_getNextTitle( int meindex, int toindex, char *arg, int title, int select);


BOOL NPC_MakePairManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	int i;
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("err NPC_MakePairMan: GetArgStrErr!!");
		return FALSE;
	}
   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_MAKEPAIR);
	CHAR_setInt( meindex, CHAR_LOOPINTERVAL, MAKEPAIR_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	CHAR_setWorkInt( meindex, NPC_WORK_PAIRTEAMS, 0);
	
	for( i=0; i<MAXPAIRNUMS; i++)	{
		PairList[0][i].use = -1;
		PairList[0][i].forindex = -1;
		PairList[1][i].use = -1;
		PairList[1][i].forindex = -1;
	}
	return TRUE;
}

void NPC_MakePairManTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}
#ifdef _ITEM_CHECKWARES
	if( CHAR_CheckInItemForWares( talkerindex, 0) == FALSE ){
		CHAR_talkToCli( talkerindex, meindex, "无法服务携带货物的人！", CHAR_COLORYELLOW);
		return;
	}
#endif
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	CHAR_setWorkInt( talkerindex, CHAR_MYPAIRCODE, NPC_getPairCode( talkerindex));
	NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

static void NPC_MakePair_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if( Action_PartyCheck( meindex, toindex) == FALSE)	{
		CHAR_talkToCli( toindex, meindex, "请一个一个来！", CHAR_COLORYELLOW);
		return;
	}

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("err NPC_MakePairMan: GetArgStrErr!!");
		return;
	}

	memset( token, 0, sizeof( token));
	switch( num)	{
	case WINDOW_START:
		{
			char buf1[256];
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FREE", buf1, sizeof( buf1) ) != NULL)	{
				if( NPC_ActionPassCheck( meindex, toindex, buf1) == FALSE ){
					CHAR_talkToCli( toindex, meindex, "条件不足！", CHAR_COLORYELLOW);
					return;
				}
			}

			if( MakePairMan_GetMenuStr( meindex, toindex, npcarg, token) == FALSE )	{
				print( "err MakePairMan GetMenu ERROR !!\n");
				return;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 1);
			windowtype = WINDOW_MESSAGETYPE_SELECT;
			buttontype = WINDOW_BUTTONTYPE_CANCEL;
			windowno = WINDOW_TALKSTART;
		}
		break;
	case WINDOW_USER:
		{
			int male=0, female=0;
			CheckPairUsers( &male, &female);
			sprintf( token, "目前登录急寻找对象的人数为：\n男生为 %d，女生为 %d。\n目前配对已有 %d对。",
				male, female, CHAR_getWorkInt( meindex, NPC_WORK_PAIRTEAMS));
			buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
			windowno = WINDOW_RETURN;
		}
		break;
	case WINDOW_SELECT://心理测验
		{
			int title = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
			int type;
			type = NPC_getTitleMsg( meindex, toindex, npcarg, token, title);
			if( type == 1 ){
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowno = WINDOW_TALKSELECT;
			}else if( type == 2 ){
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowno = WINDOW_TALKEND;
			}else{
				return;
			}
		}
		break;
	case WINDOW_PAIR:
		{
			if( CHAR_getWorkInt( toindex, CHAR_MYPAIRFLG) > 0){
				CHAR_setWorkInt( toindex, CHAR_MYPAIRINDEX, -1);
				sprintf( token, "取消前一次的邀请。");
			}else	{
				char buf1[256];
				if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FREE", buf1, sizeof( buf1) ) != NULL)	{
					if( NPC_ActionPassCheck( meindex, toindex, buf1) == FALSE ){
						CHAR_talkToCli( toindex, meindex, "条件不足！", CHAR_COLORYELLOW);
						return;
					}
				}
				NPC_PairForage( meindex, toindex, npcarg, token);
			}
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowno = WINDOW_TALKEND;
		}
		break;
	case WINDOW_PAIROK:	//同意
		if( CHAR_getWorkInt( toindex, CHAR_MYPAIRFLG) > 0){
			int forindex = CHAR_getWorkInt( toindex, CHAR_MYPAIRINDEX);
			NPC_PairUserAndWarp( meindex, toindex, forindex, npcarg);
		}
		return;
	case WINDOW_PAIRNO://不同意
		{
			int forindex = CHAR_getWorkInt( toindex, CHAR_MYPAIRINDEX);
			if( CHAR_CHECKINDEX( forindex) ){
				if( CHAR_getWorkInt( forindex, CHAR_MYPAIRFLG) > 0 ){
					CHAR_talkToCli( forindex, meindex, "对方拒绝邀请。", CHAR_COLORYELLOW);
					CHAR_setWorkInt( forindex, CHAR_MYPAIRINDEX, -1);
					CHAR_setWorkInt( forindex, CHAR_MYPAIRFLG, -1);
				}
			}
			CHAR_setWorkInt( toindex, CHAR_MYPAIRINDEX, -1);
			CHAR_setWorkInt( toindex, CHAR_MYPAIRFLG, -1);
			CHAR_talkToCli( toindex, meindex, "拒绝对方邀请。", CHAR_COLORYELLOW);
			return;
		}
		break;
	case WINDOW_WARP:
		break;
	case WINDOW_END:
		break;
	}
	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_MakePairManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    	return;
	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("err NPC_MakePairMan: GetArgStrErr!!");
		return;
	}
	switch( seqno)	{
	case WINDOW_TALKSTART:
		{
			int choose=atoi( data);
			if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
				return;
			switch( choose){
			case 1://查人数
				NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_USER, 0);
				break;
			case 2://配对
				NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_PAIR, 0);
				break;
			case 3://心理测验
				NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_SELECT, 0);
				break;
			}
		}

		break;
	case WINDOW_TALKSELECT:
		{
			int title = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
			title = NPC_getNextTitle( meindex, talkerindex, npcarg, title, select);
			if( title < 0 ) return ;
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, title);
			NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_SELECT, select);
		}
		break;
	case WINDOW_TALKPAIR://配对邀请回覆
		if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO){
			NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_PAIRNO, 0);
		}else	{
			NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_PAIROK, 0);
		}
		break;
	case WINDOW_RETURN:
		NPC_MakePair_selectWindow( meindex, talkerindex, WINDOW_START, 0);
		break;
	case WINDOW_TALKEND:
		break;
	}
}
void NPC_MakePairManLoop( int meindex)
{
	int i;
	for( i=0; i<MAXPAIRNUMS; i++)	{
		if( PairList[0][i].use > 0 ){
			if( !CHAR_CHECKINDEX( PairList[0][i].pindex) ){
				PairList[0][i].pindex = -1;
				PairList[0][i].use = -1;
			}
		}
		if( PairList[1][i].use > 0 ){
			if( !CHAR_CHECKINDEX( PairList[1][i].pindex) ){
				PairList[1][i].pindex = -1;
				PairList[1][i].use = -1;
			}
		}
	}
}

BOOL MakePairMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token)
{
	char buf1[1024], buf2[256], buf3[256];
	BOOL FINDS=FALSE;
	int talkNo=1;
	if( npcarg == NULL ) return FALSE;
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "MenuHead", buf1, sizeof( buf1) ) == NULL){
		return FALSE;
	}
	sprintf( token, "%s\n", buf1);

	sprintf( buf1, "MEMU%d", talkNo);
	while( NPC_Util_GetStrFromStrWithDelim( npcarg, buf1, buf2, sizeof( buf2)) != NULL )	{
		talkNo++;
		sprintf( buf1, "MEMU%d", talkNo);
		FINDS = TRUE;
		sprintf( buf3, "          %s\n", buf2);
		strcat( token, buf3);
	}
	return FINDS;
}

void CheckPairUsers( int *male, int *female)
{
	int i;
	*male = 0;
	*female = 0;
	for( i=0; i<MAXPAIRNUMS; i++)	{
		if( PairList[0][i].use > 0 ) *male+=1;
		if( PairList[1][i].use > 0 ) *female+=1;
	}
}

BOOL NPC_PairForage( int meindex, int toindex, char *arg, char *token)
{
	char buf1[256];
	int i;
	int codes = CHAR_getWorkInt( toindex, CHAR_MYPAIRCODE);
	int Psex = CHAR_getSexInt( CHAR_getInt( toindex, CHAR_BASEBASEIMAGENUMBER) );
	Psex = (Psex==1)?1:0;


	for( i=0; i<MAXPAIRNUMS; i++)	{//forindex
		char *name;
		char *cdKey;
		int fd;
		char buf[256];
		if( PairList[Psex][i].use <= 0 ) continue;
		if( !CHAR_CHECKINDEX( PairList[Psex][i].pindex) ) continue;
		if( CHAR_getWorkInt( PairList[Psex][i].pindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE )
			continue;
		if( codes != CHAR_getWorkInt( PairList[Psex][i].pindex, CHAR_MYPAIRCODE) ) continue;
		if( CHAR_getWorkInt( PairList[Psex][i].pindex, CHAR_MYPAIRFLG) > 0 ) continue;
		name = CHAR_getChar( PairList[Psex][i].pindex, CHAR_NAME);
		cdKey = CHAR_getChar( PairList[Psex][i].pindex, CHAR_CDKEY);
		if( strcmp( PairList[Psex][i].name, name) || strcmp( PairList[Psex][i].cdKey, cdKey) ){
			PairList[Psex][i].use = -1;
			PairList[Psex][i].forindex = -1;
			continue;
		}
		sprintf( token, "你的配对对象为： %s\n已送出邀请，请等候对方回应。", name);
		if( NPC_Util_GetStrFromStrWithDelim( arg, "InviteMsg", buf1, sizeof( buf1) ) == NULL){
			break;
		}
		PairList[Psex][i].forindex = toindex;
		fd = getfdFromCharaIndex( PairList[Psex][i].pindex);

		CHAR_setWorkInt( toindex, CHAR_MYPAIRFLG, 1);
		CHAR_setWorkInt( toindex, CHAR_MYPAIRINDEX, PairList[Psex][i].pindex);
		CHAR_setWorkInt( PairList[Psex][i].pindex, CHAR_MYPAIRFLG, 2);
		CHAR_setWorkInt( PairList[Psex][i].pindex, CHAR_MYPAIRINDEX, toindex);

		name = CHAR_getChar( toindex, CHAR_NAME);
		sprintf( buf, "%s：\n玩家%s\n%s", CHAR_getChar( meindex, CHAR_NAME),name, buf1);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, WINDOW_BUTTONTYPE_OKCANCEL, WINDOW_TALKPAIR,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), buf);
		break;
	}
	if( i>= MAXPAIRNUMS )	{
		if( NPC_Util_GetStrFromStrWithDelim( arg, "NoPairMsg", buf1, sizeof( buf1) ) != NULL){
			sprintf( token, "%s",buf1);
		}else	{
			sprintf( token, "没有找到对象。");
		}
		if( CHAR_getWorkInt( toindex, CHAR_MYPAIRFLG) <= 0 )	{
			NPC_PairRegister( toindex);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL NPC_PairRegister( int toindex)
{
	char *cdKey = CHAR_getChar( toindex, CHAR_CDKEY);
	char *name = CHAR_getChar( toindex, CHAR_NAME);
	int space=-1,i;
	int Psex = CHAR_getSexInt( CHAR_getInt( toindex, CHAR_BASEBASEIMAGENUMBER) );
	Psex = (Psex==1)?0:1;
	for( i=0; i<MAXPAIRNUMS; i++)	{
		if( PairList[Psex][i].use <= 0 ){
			if( space == -1 ) space = i;
			continue;
		}
		if( !strcmp( cdKey, PairList[Psex][i].cdKey) )
			break;
	}
	if( i>=MAXPAIRNUMS){
		if( space != -1 && CHAR_getWorkInt( toindex, CHAR_MYPAIRCODE) > 0 ){
			PairList[Psex][space].use = 1;
			PairList[Psex][space].forindex = -1;
			sprintf( PairList[Psex][space].cdKey, "%s", cdKey);
			sprintf( PairList[Psex][space].name, "%s", name);
			PairList[Psex][space].pindex = toindex;
			PairList[Psex][space].code = CHAR_getWorkInt( toindex, CHAR_MYPAIRCODE);

			CHAR_talkToCli( toindex, -1, "新增至配对名单。", CHAR_COLORYELLOW);
			return TRUE;
		}
	}
	return FALSE;
}

void NPC_PairUserAndWarp( int meindex, int toindex, int forindex, char *arg)
{
	char buf1[256];
	int fl=-1, x=-1, y=-1;

	if( !CHAR_CHECKINDEX( toindex)) return;
	CHAR_setWorkInt( toindex, CHAR_MYPAIRINDEX, -1);
	CHAR_setWorkInt( toindex, CHAR_MYPAIRFLG, -1);
	if( !CHAR_CHECKINDEX( forindex)) return;
	CHAR_setWorkInt( forindex, CHAR_MYPAIRINDEX, -1);
	CHAR_setWorkInt( forindex, CHAR_MYPAIRFLG, -1);

	if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ) return;
	if( CHAR_getWorkInt( forindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ) return;

	if( NPC_Util_GetStrFromStrWithDelim( arg, "PAIRTOPOINT", buf1, sizeof( buf1) ) != NULL){
		char buf2[256];
		if( getStringFromIndexWithDelim( buf1,",",1,buf2,sizeof( buf2)) != FALSE )
			fl = atoi( buf2);
		if( getStringFromIndexWithDelim( buf1,",",2,buf2,sizeof( buf2)) != FALSE )
			x = atoi( buf2);
		if( getStringFromIndexWithDelim( buf1,",",3,buf2,sizeof( buf2)) != FALSE )
			y = atoi( buf2);
	}
	CHAR_DischargePartyNoMsg( toindex);//解散团队
	CHAR_DischargePartyNoMsg( forindex);//解散团队
	if( MAP_IsValidCoordinate( fl, x, y) != FALSE ){
		CHAR_warpToSpecificPoint( toindex, fl, x, y);
		CHAR_warpToSpecificPoint( forindex, fl, x, y);
	}

	CHAR_JoinParty_Main( toindex, forindex);
	//lssproto_PR_send( fd, 0, 1);
	{
		int i;
		for( i=0; i<MAXPAIRNUMS; i++)	{
			if( PairList[0][i].use > 0 ){
				if( PairList[0][i].pindex == toindex ||
					PairList[0][i].pindex == forindex ){
					PairList[0][i].pindex = -1;
					PairList[0][i].use = -1;
				}
			}
			if( PairList[1][i].use > 0 ){
				if( PairList[1][i].pindex == toindex ||
					PairList[1][i].pindex == forindex ){
					PairList[1][i].pindex = -1;
					PairList[1][i].use = -1;
				}
			}
		}
	}
}

int NPC_getPairCode( int toindex)
{
	char *arg=NULL;
	int code=-1, i;
	if( !CHAR_CHECKINDEX( toindex)) return-1;
	for( i=0; i<15; i++)	{
		char buf1[256];
		int itemindex = CHAR_getItemIndex( toindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ) continue;
		arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );// 性别|个性编号|FLG
		if( arg == NULL )continue;
		if( NPC_Util_GetStrFromStrWithDelim( arg, "PAIRCODE", buf1, sizeof( buf1) ) == NULL) continue;
		code = atoi( buf1);
		break;
	}
	return code;
}

int NPC_getNextTitle( int meindex, int toindex, char *arg, int title, int select)
{
	char buf[NPC_UTIL_GETARGSTR_BUFSIZE], buf1[1024], buf2[256];
	int toID=0;
	memset( buf, 0, sizeof( buf));
	if( getStringFromIndexWithDelim( arg, "PSYCHOMETRICS", 2, buf, sizeof( buf)) == FALSE )
		return -1;
	if( getStringFromIndexWithDelim( buf , "}", title, buf1, sizeof( buf1)) == FALSE )
		return -1;
	if( select == WINDOW_BUTTONTYPE_YES ) {
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "YESFOR", buf2, sizeof( buf2 )) == NULL)
			return -1;
	}else {
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "NOFOR", buf2, sizeof( buf2) ) == NULL)
			return -1;
	}
	toID = atoi( buf2);
	return toID;
}

int NPC_getTitleMsg( int meindex, int toindex, char *arg, char *token, int title)
{
	char buf[NPC_UTIL_GETARGSTR_BUFSIZE], buf1[1024], buf2[256];
	memset( buf, 0, sizeof( buf));

	if( getStringFromIndexWithDelim( arg, "PSYCHOMETRICS", 2, buf, sizeof( buf)) == FALSE )
		return -1;
	//取得下一题
	if( getStringFromIndexWithDelim( buf , "}", title, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TITLEMSG", buf2, sizeof( buf2 )) == NULL){
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2 )) == NULL){
			return -1;
		}
		if( NPC_ActionPassCheck( meindex, toindex, buf2) == FALSE )	{
			CHAR_talkToCli( toindex, meindex, "条件不足", CHAR_COLORYELLOW);
			return -1;
		}
		if( Action_RunDoEventAction( meindex, toindex, buf1) == FALSE ){
			CHAR_talkToCli( toindex, meindex, "条件不足", CHAR_COLORYELLOW);
			return -1;
		}
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "ENDMSG", buf2, sizeof( buf2 )) == NULL){
			return -1;
		}
		sprintf( token, "%s", buf2);
		return 2;	
	}

	sprintf( token, "%s", buf2);
	return 1;
}
#endif
