#ifndef _SELLSTH_MAN_H
#define _SELLSTH_MAN_H

void NPC_SellsthManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_SellsthManInit( int meindex );
void NPC_SellsthManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_SellsthManLoop( int meindex);

#endif
