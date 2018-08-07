#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include "autil.h"
#include "util.h"
#include "net.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "log.h"
#include "handletime.h"
#include "npcserver.h"
#include "lssproto_serv.h"
#include "lssproto_util.h"
#include "npcshandle.h"
#include "npcutil.h"
#include "npc_eventaction.h"

#ifdef _NPCSERVER_NEW
enum{
	uEQUAL = 1, //等於
	uBIGEQUALt,	//大於等於
	uSMALLEQUALt, //小於等於
	uBIGTHAN = 5,	//大於
	uSMALLTHAN, //小於
	uNEQUAL, //不等於
};
BOOL NPCS_HandleCheckFreeMess( int npcobjindex, int charaindex, int charobjindex,
				char *CheckfreeMess)
{
	BOOL ReCall = FALSE;
	BOOL Enable = FALSE;
	char buf1[1024], buf2[256];
	int i=1;

	//主条件判断
	while( getStringFromIndexWithDelim( CheckfreeMess, "OVER", i, buf1, sizeof( buf1)) != FALSE ){
		i++;
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2)) == NULL  ) continue;
		if( NPCS_FreePassCheck( charaindex, buf2) == TRUE ){
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "FreeMess", buf2, sizeof( buf2)) == NULL  ) continue;
			Enable = TRUE;
			if( NPCS_RunDoEventAction( charaindex, buf1) == FALSE )
				return FALSE;

			//事件处理
			NPCS_NpcstalkToCli( charaindex, npcobjindex, buf2, CHAR_COLORYELLOW);
			break;
		}
	}
	if( Enable == FALSE ){//所有条件不成立
		NPCS_NpcstalkToCli( charaindex, npcobjindex, "条件不足！", CHAR_COLORYELLOW);
		return FALSE;
	}
	if( ReCall == TRUE ){
		//判断是否recall
	}
	return TRUE;
}

BOOL NPCS_FreePassCheck( int charaindex, char *buf)
{
	char buff2[256];
	char buff3[128];
	int i=1,j=1;
	int loop=0;
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )	{	
		i++;
		if(strstr(buff2,"&")!=NULL){
			j=1;
			loop=0;
			while( getStringFromIndexWithDelim(buff2,"&",j,buff3,sizeof(buff3)) !=FALSE )	{
				j++;
				if(NPCS_FreeBigSmallCheck( charaindex, buff3)==FALSE)	{
					loop=1;
					break;
				}
			}
			if(loop==0)	return TRUE;
		}else{
			if( NPCS_FreeBigSmallCheck( charaindex, buff2) == TRUE) return TRUE;
		}
	}
	CHAR_setWorkInt( charaindex, CHAR_WORKWARPCHECK, FALSE );
	return FALSE;

}

BOOL NPCS_FreeBigSmallCheck( int charaindex,char* buf)
{
	char buff1[128], buff2[128], Head[128];
	int kosuu,temp=-1,flg=0;

	memset( Head, 0, sizeof( Head));
	if( strstr( buf, "=" ) != NULL ){
		strcpy( Head, "=");
		flg = uEQUAL;
		if( strstr( buf, ">") != NULL ){
			strcat( Head, ">");
			flg = uBIGEQUALt;
		}else if( strstr( buf, "<") != NULL ){
			strcat( Head, "<");
			flg = uSMALLEQUALt;
		}else if( strstr( buf, "!") != NULL )	{
			strcat( Head, "!");
			flg = uNEQUAL;
		}
	}else if(strstr( buf, "<") != NULL){
		strcpy( Head, "<");
		flg = uSMALLTHAN;
	}else if(strstr( buf, ">") != NULL){
		strcpy( Head, ">");
		flg = uBIGTHAN;
	}
	Head[ strlen( Head)+1] = 0;
	getStringFromIndexWithDelim( buf, Head, 2, buff2, sizeof(buff2));
	getStringFromIndexWithDelim( buf, Head, 1, buff1, sizeof(buff1));

	if( strstr( buff2, "*") != NULL ){
		char buff3[128];
		getStringFromIndexWithDelim( buff2, "*", 1, buff3, sizeof(buff3));
		temp = atoi( buff3);
		getStringFromIndexWithDelim( buff2, "*", 2, buff3, sizeof(buff3));
		kosuu = atoi( buff3);
	}else {
		kosuu = atoi( buff2);
		temp = 1;
	}
	return NPCS_ArgumentFreeCheck( charaindex, buff1, kosuu, flg, temp);
}

BOOL NPCS_ArgumentFreeCheck( int charaindex, char* Argument, int amount, int temp, int probjID)
{//amount 数量 Argument 参数
	int MyPoint=0, reAmount;
	reAmount = 0;
	if( strcmp( Argument, "LV") == 0)	{//等级
		MyPoint = CHAR_getInt( charaindex, CHAR_LV);
	}else if( strcmp( Argument, "GOLD") == 0)	{//石币
		MyPoint = CHAR_getInt( charaindex, CHAR_GOLD);
	}else if( strcmp( Argument, "TRANS") == 0)	{//转生次数
		MyPoint = CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
	}else if( strcmp( Argument, "PET") == 0)	{//宠物 ID 数量
		MyPoint = NPCS_getUserPetAmount( charaindex, probjID, 1);
	}else if( strcmp( Argument, "ITEM") == 0)	{//道具 ID 数量
		MyPoint = NPCS_getUserItemAmount( charaindex, probjID, 1);
	}else if( strcmp( Argument, "ENDEV") == 0)	{//结束旗标
		return NPC_EventCheckFlg( charaindex, amount);
	}else if( strcmp( Argument, "NOWEV") == 0)	{//开启旗标
		return NPC_NowEventCheckFlg( charaindex, amount);
	}else if( strcmp( Argument, "PARTY") == 0)	{//检查团队人数
		MyPoint = NPCS_getUserPartyAmount( charaindex );
	}else if( strcmp( Argument, "reITEM")== 0)	{//剩馀道具栏位
		NPCS_getUserItemAmount( charaindex, probjID, 0);
		MyPoint = reAmount;
	}else if( strcmp( Argument, "rePET") == 0)	{//剩馀宠物栏位
		NPCS_getUserPetAmount( charaindex, probjID, 0);
		MyPoint = reAmount;
	}else {
	}

	return NPCS_ArgumentBigSmallCheck( amount, MyPoint, temp);
}

BOOL NPCS_ArgumentBigSmallCheck( int point, int mypoint, int flg)
{
	switch( flg){
	case uEQUAL: //等於
		if( mypoint == point ) return TRUE;
		break;
	case uBIGEQUALt: //大於等於
		if( mypoint >= point ) return TRUE;
		break;
	case uSMALLEQUALt: //小於等於
		if( mypoint <= point ) return TRUE;
		break;
	case uBIGTHAN: //大於
		if( mypoint > point ) return TRUE;
		break;
	case uSMALLTHAN: //小於
		if( mypoint < point ) return TRUE;
		break;
	case uNEQUAL: //不等於
		if( mypoint != point ) return TRUE;
		break;
	}
	return FALSE;
}

int NPCS_getUserPetAmount( int charaindex, int objID, int flg)
{
	int i, Amount=0, reAmount=0;
	for( i=0 ;i <CHAR_MAXPETHAVE; i++) {
		int petindex = CHAR_getCharPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex)){
			reAmount++;
		}else{
			if( CHAR_getInt( petindex, CHAR_PETID) == objID)
				Amount++;
		}
	}
	if( flg == 1 ){
		return Amount;
	}else{
		return reAmount;
	}

}

int NPCS_getUserItemAmount( int charaindex, int objID, int flg)
{
	int i, Amount=0, reAmount=0;
	for( i=CHAR_STARTITEMARRAY ;  i<CHAR_MAXITEMHAVE; i++){
		int itemindex = CHAR_getItemIndex( charaindex, i);
		if( !ITEM_CHECKINDEX( itemindex) ){
			reAmount++;
		}else{
			if( ITEM_getInt( itemindex, ITEM_ID) == objID)
				Amount++;
		}
	}
	if( flg == 1 ){
		return Amount;
	}else {
		return reAmount;
	}
}

int NPCS_getUserPartyAmount( int charaindex )
{
	int partys=0;
	int leaderindex=-1;

	leaderindex = charaindex;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
		return 0;
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ){
		leaderindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
	}
	
	if( CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ){
		int i;
		for( i=0; i<5; i++)	{
			int pindex = CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYINDEX1+i);
			if( !CHAR_CHECKINDEX( pindex) )
				partys++;
		}
	}
	return partys;
}

BOOL NPCS_RunDoEventAction( int charaindex, char *buf1)
{
	char buf2[256];
	// WON ADD
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddGold", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddGold2( charaindex, buf2) == FALSE )
			return FALSE;
	}			
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelGold", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelGold( charaindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelItem", buf2, sizeof( buf2)) != NULL ) {
		//print("\n DelItem:%s:%d ", __FILE__, __LINE__);
		if( NPC_ActionDelItem( charaindex, buf2) == FALSE ) {
			//print("\n 改封包??NPC_ActionDelItem:%s:%s  ", CHAR_getChar(toindex, CHAR_CDKEY), buf2);
			return FALSE;
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddItem( charaindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelPet( charaindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddPet( charaindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvEnd", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionSetEend( charaindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvNow", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionSetNow( charaindex, buf2) == FALSE )
			return FALSE;
	}
	return TRUE;
}
BOOL NPCS_NpcstalkToCli( int charaindex,int npcobjindex, char* message, CHAR_COLOR color )
{
	static char    lastbuf[2048];
	static char    escapebuf[2048];
	int fd;

	if( !CHAR_CHECKINDEX(charaindex) ) return FALSE;
	if( CHAR_getInt( charaindex,CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER) return FALSE;
	if( (fd=getfdFromCharaIndex( charaindex )) == -1 ) return FALSE;
	if( color < CHAR_COLORWHITE && color > CHAR_COLORGREEN2 )return FALSE;
#ifndef _CHANNEL_MODIFY    
	snprintf( lastbuf, sizeof(lastbuf), "P|%s:%s",
#else
	snprintf( lastbuf, sizeof(lastbuf), "P|P|%s:%s",
#endif
			makeEscapeString( OBJECT_getName( npcobjindex), escapebuf, sizeof(escapebuf)), message);

	lssproto_TK_send( fd, npcobjindex, lastbuf, color);
	return TRUE;
}
#endif
