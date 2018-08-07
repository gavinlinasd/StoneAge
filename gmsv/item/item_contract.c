#include "version.h"
#ifdef _CONTRACT
#include <string.h> /* strstr */
#include <ctype.h>  /* isdigit, isalnum */
#include "common.h"
#include "char.h"
#include "char_base.h"
#include "char_data.h"
#include "configfile.h"
#include "item.h"
#include "item_event.h"
#include "object.h"
#include "anim_tbl.h"
#include "magic.h"
#include "handletime.h"
#include "log.h"
#include "util.h"
#include "encount.h"
#include "battle.h"
#include "battle_item.h"
#include "lssproto_serv.h"
#include "net.h"
#include "pet.h"
#include "npcutil.h"
#include "enemy.h"

typedef struct{
	int used;
	char name[32];
	char fmname[32];
} contractSignTag;

//ITEM_contractTable	ITEM_contractTbl[MAX_CONTRACTTABLE];
static ITEM_contractTable	*ITEM_contractTbl;

BOOL ITEM_initContractTable( )
{
	//
	FILE *fp;
	char data[2048];
	int i;

	print("\读取 contract.txt ....");

    ITEM_contractTbl = (ITEM_contractTable *)allocateMemory( sizeof(struct tagITEM_contract) * MAX_CONTRACTTABLE );

    if( ITEM_contractTbl == NULL ){
        fprint( "无法分配内存 %d\n" ,
                sizeof(struct tagITEM_contract)*MAX_CONTRACTTABLE);
        fclose( fp );
        return FALSE;
    }

	memset( ITEM_contractTbl, 0 , sizeof(struct tagITEM_contract) * MAX_CONTRACTTABLE);

	fp = fopen("data/contract.txt", "r");
	if( fp == NULL ) {
		print("\n无法读取 contract.txt!! ");
		return FALSE;
	}

	for( i =0; i >MAX_CONTRACTTABLE; i++) {
		ITEM_contractTbl[i].used = FALSE;
	}
	
	while(1) {
		int ret;
		int index;
		char token[2048];
		ret = fgets( data, sizeof(data), fp);
		if( ret == NULL || ret == EOF)
			break;

		if( getStringFromIndexWithDelim( data, "|", 1, token, sizeof(token)) == TRUE) {
			index = atoi( token);
			if( index <0 || index >= MAX_CONTRACTTABLE) continue;
		}
		else { continue; }

		if( getStringFromIndexWithDelim( data, "|", 2, token, sizeof(token)) == TRUE) {
			ITEM_contractTbl[index].argnum = atoi( token);
		}
		else { continue; }

		if( getStringFromIndexWithDelim( data, "|", 3, token, sizeof(token)) == TRUE) {
			strcpy( ITEM_contractTbl[index].detail, token);			
		}
		else { continue; }

		ITEM_contractTbl[i].used = TRUE;

		print("\n契约[%d] %d %s", index, ITEM_contractTbl[index].argnum, ITEM_contractTbl[index].detail);
	}

	fclose( fp);

	return TRUE;

}

void ITEM_contract( int charaindex, int toindex, int haveitemindex)
{
	int itemindex;
	int conindex;
	char data[2048+1024];
	
	if( !CHAR_CHECKINDEX( charaindex ) )  return;

	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) )  return;

	//conindex = atoi( ITEM_getChar( itemundex, ITEM_CONTRACTNUM));
	conindex = atoi( ITEM_getChar( itemindex, ITEM_ARGUMENT));

	sprintf( data, "%s|%d|%s|%s", ITEM_contractTbl[conindex].detail, 
		ITEM_contractTbl[conindex].argnum,
		ITEM_getChar( itemindex, ITEM_CONTRACTARG),
		ITEM_getChar( itemindex, ITEM_CONTRACTTIME) );

	print("\n contract_send:%s\n", data);

	lssproto_WN_send( getfdFromCharaIndex( charaindex), WINDOW_MESSAGETYPE_CONTRACT,
		WINDOW_BUTTONTYPE_YESNO,
		CHAR_WINDOWTYPE_CONTRACT_ANSWER,
		itemindex,
		data );

}


void ITEM_contractSign( int fd, int itemindex, int signindex)
{
	int charaindex;
	int i;
	int conIndex;
	int complete =0;
	int item_id;
	char contractSignData[512];
	contractSignTag contractSign[4];

	print("\n contractSign:%d:%d ", itemindex, signindex);
	
	charaindex = CONNECT_getCharaindex( fd );
	if( !CHAR_CHECKINDEX(charaindex))	return;

	if( signindex <0 || signindex >=4)	return;
    for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
        if( CHAR_getItemIndex( charaindex,i ) == itemindex )
			break;
	}
	if( i >= CHAR_MAXITEMHAVE )	return;
	if( strcmp( ITEM_getChar( itemindex, ITEM_USEFUNC), "ITEM_contract") )	return;
	
	conIndex = atoi( ITEM_getChar( itemindex, ITEM_ARGUMENT));
	if( signindex >= ITEM_contractTbl[conIndex].argnum )	return;

	strcpy( contractSignData, ITEM_getChar( itemindex, ITEM_CONTRACTARG) );

	for( i=0; i<4; i++) contractSign[i].used = FALSE;

	for( i=0; i<4; i++) {
		int index;
		char buf[512];
		char token[32];
		if( getStringFromIndexWithDelim( contractSignData, ",", i+1, buf, sizeof( buf )-1) == FALSE )
			break;
		if( strlen( buf) == 0)	break;
		getStringFromIndexWithDelim( buf, ":", 1, token, sizeof( token )-1 );
		index = atoi( token);
		if( index <0 || index >=4)	break;
		getStringFromIndexWithDelim( buf, ":", 2, contractSign[index].name, sizeof( contractSign[index].name )-1 );
		getStringFromIndexWithDelim( buf, ":", 3, contractSign[index].fmname, sizeof( contractSign[index].fmname )-1 );
		contractSign[index].used = TRUE;
	}

	if( contractSign[signindex].used == TRUE)	return;
	
	strcpy( contractSign[signindex].name, CHAR_getChar( charaindex, CHAR_NAME) );
	strcpy( contractSign[signindex].fmname, CHAR_getChar( charaindex, CHAR_FMNAME) );
	contractSign[signindex].used = TRUE;

	// 检查是否完成契约
	for( i =0; i<4; i++) {
		if( contractSign[i].used == TRUE)
			complete++;
	}
	if( complete >= ITEM_contractTbl[conIndex].argnum ) {
		char buf[512];
		if( CHAR_findEmptyItemBoxNo( charaindex ) < ITEM_contractTbl[conIndex].argnum -1 ) {
			sprintf( buf, "要完成这个契约，请先准备%d个空道具栏位。", ITEM_contractTbl[conIndex].argnum -1);
			CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
			return;
		}
	}


	strcpy( contractSignData, "");
	for( i=0; i<4; i++) {
		char buf[512];
		if( contractSign[i].used == FALSE)	continue;
		sprintf( buf, "%d:%s:%s,", i, contractSign[i].name, contractSign[i].fmname);
		strcat( contractSignData, buf);
	}
	ITEM_setChar( itemindex, ITEM_CONTRACTARG, contractSignData);

	CHAR_talkToCli( charaindex, -1, "完成契约签名。", CHAR_COLORYELLOW);


	// 完成契约
	if( complete >= ITEM_contractTbl[conIndex].argnum ) {
		struct tm *nowtime;
		char timebuf[512];

		// 纪录日期
		nowtime = localtime( (time_t *)&NowTime.tv_sec );
		sprintf( timebuf, "%04d年%02d月%02d日",
			nowtime->tm_year +1900, nowtime->tm_mon+1, nowtime->tm_mday);
		ITEM_setChar( itemindex, ITEM_CONTRACTTIME, timebuf);

		CHAR_talkToCli( charaindex, -1, "完成所有签名，契约成立！", CHAR_COLORYELLOW);
		// 复制契约
		item_id = ITEM_getInt( itemindex, ITEM_ID);
		for( i =0 ; i < ITEM_contractTbl[conIndex].argnum -1 ; i++)
		{
			int	emptyindex;
			int newitemindex;
			
			emptyindex = CHAR_findEmptyItemBox( charaindex );
			
			if( emptyindex < 0 ){
				CHAR_talkToCli( charaindex, -1, "空间栏位不足。", CHAR_COLORYELLOW);
				break;
			}
			
			newitemindex = ITEM_makeItemAndRegist( item_id );
			
			if( newitemindex != -1 ){
				CHAR_setItemIndex( charaindex, emptyindex, newitemindex );
				ITEM_setWorkInt( newitemindex, ITEM_WORKOBJINDEX, -1);
				ITEM_setWorkInt( newitemindex, ITEM_WORKCHARAINDEX, charaindex);
				CHAR_sendItemDataOne( charaindex, emptyindex);
				ITEM_setChar( newitemindex, ITEM_CONTRACTARG, contractSignData);
				ITEM_setChar( newitemindex, ITEM_CONTRACTTIME, timebuf);
			}
			
			CHAR_talkToCli( charaindex, -1, "复制契约书。", CHAR_COLORYELLOW);
		}
	}
}

#endif
