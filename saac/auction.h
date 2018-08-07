#ifndef _AUCTION_H_
#define _AUCTION_H_

#include "version.h"

#ifdef _PAUCTION_MAN
#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif
#define PAUCTION_ADD 1
#define PAUCTION_DEL 2
#define MAXPAITEMS 20
typedef struct _tagItemAuctionData
{
	char ItemName[256];
	char Data[1024];
	int ItemId;
	int use;
}ItemAuctionData;



void IAData_Init();
BOOL IAData_ITEM( int fd, int ItemID, char *ITEMNAME, char *DATA, int ret, int flg);
BOOL IAData_ADDITEM( int fd, int ItemID, char *ITEMNAME, char *DATA, int flg);
BOOL IAData_DELITEM( int fd, int ItemID, char *ITEMNAME, int ret, int flg);

#endif

#endif



