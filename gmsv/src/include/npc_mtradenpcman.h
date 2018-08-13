#ifndef __NPC_MAPTRADEMAN_H__
#define __NPC_MAPTRADEMAN_H__

BOOL MapTradeManInit( int meindex );
void MapTradeManLoop( int meindex);
void MapTradeManTalked( int meindex , int talkerindex , char *msg , int color );
void MapTradeManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif

