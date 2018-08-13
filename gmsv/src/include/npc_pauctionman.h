#ifndef _PAUCTION_MAN_H
#define _PAUCTION_MAN_H


BOOL NPC_PauctionManInit( int meindex );
void NPC_PauctionManTalked( int meindex, int talkerindex, char *msg, int color );
void NPC_PauctionManWindowTalked( int meindex, int talkerindex, int seqno,
								 int select, char *data);
void NPC_PauctionManLoop( int meindex);

int NPC_PAItemShop_AddItem( int itemID, int ret);
BOOL NPC_PAItemShop_DelItem( int ti, int index);
BOOL NPC_PAItemShop_reItem( int itemindex, char *Data);

#endif


