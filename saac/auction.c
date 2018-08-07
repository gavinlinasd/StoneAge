#define _AUCTION_C_
#include "version.h"

#ifdef _PAUCTION_MAN
#include "lock.h"
#include "util.h"
#include "main.h"
#include "saacproto_serv.h"
#include "char.h"
#include "auction.h"

//#define PAUCTION_ADD 1
//#define PAUCTION_DEL 2
//#define MAXPAITEMS 20
/*typedef struct _tagItemAuctionData
{
	char ItemName[256];
	char Data[1024];
	int ItemId;
	int use;
}ItemAuctionData;
*/
ItemAuctionData IAData[MAXPAITEMS];

void IAData_Init()
{
	int i;
	for( i=0; i<MAXPAITEMS; i++)	{
		IAData[i].use = -1;
		IAData[i].ItemId = -1;
	}
}

BOOL IAData_ITEM( int fd, int ItemID, char *ITEMNAME, char *DATA, int ret, int flg)
{
	if( fd != -1 )	{
		if( !is_game_server_login(fd) )
			return FALSE;
	}
	switch( flg)	{
	case PAUCTION_ADD:
		//void IAData_ADDITEM( int fd, int ItemID, char *ITEMNAME, char *DATA, int flg)
		if( IAData_ADDITEM( fd, ItemID, ITEMNAME, DATA, flg) == FALSE )
			return FALSE;
		break;
	case PAUCTION_DEL:
		IAData_DELITEM( fd, ItemID, ITEMNAME, ret, flg);
		break;
	}
	return TRUE;
}

BOOL IAData_ADDITEM( int fd, int ItemID, char *ITEMNAME, char *DATA, int flg)
{
	int i;
	int num=-1;
	for( i=0; i<MAXPAITEMS; i++)	{
		if( IAData[i].use < 0 )	{
			IAData[i].ItemId = ItemID;
			IAData[i].use = 1;
			memset( IAData[i].ItemName, 0, sizeof( IAData[i].ItemName));
			memset( IAData[i].Data , 0, sizeof( IAData[i].Data));
			sprintf( IAData[i].ItemName,"%s", ITEMNAME);
			sprintf( IAData[i].Data,"%s", DATA);
			num = i;
			break;
		}
	}
	if( i>= MAXPAITEMS ) return FALSE;
	//ËÍ»Ø¸øGS
	for(i=0;i<MAXCONNECTION;i++)	{
		if( is_game_server_login(i) ){
			saacproto_ACItemAuction_send( i, IAData[num].ItemName, IAData[num].Data,
				IAData[num].ItemId, num, flg);
		}
	}

	return TRUE;
}

BOOL IAData_DELITEM( int fd, int ItemID, char *ITEMNAME, int ret, int flg)
{
	int i;
	if( IAData[ret].use < 0 )
		return FALSE;
	if( ret < 0 || ret >= MAXCONNECTION ) return FALSE;

	for(i=0;i<MAXCONNECTION;i++)	{
		if( is_game_server_login(i) ){
			saacproto_ACItemAuction_send( i, ITEMNAME, "TEST", IAData[ret].ItemId , ret, flg);
		}
	}
	IAData[ret].use = -1;
	IAData[ret].ItemId = -1;
	memset( IAData[ret].ItemName, 0, sizeof( IAData[ret].ItemName));
	return TRUE;
}
#endif
