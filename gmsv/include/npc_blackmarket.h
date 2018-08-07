#ifndef __NPC_BLACKMARKET_H__
#define __NPC_BLACKMARKET_H__

#ifdef _BLACK_MARKET
void NPC_BlackMarketTalked( int meindex, int talker, char *msg, int color);
BOOL NPC_BlackMarketInit( int meindex);
void NPC_BlackMarketWindowTalked( int index, int talker, int seqno, int select, char *data);

#endif

#endif
