#ifndef _NPC_EXCHANGEMAN_H_
#define _NPC_EXCHANGEMAN_H_

#include "common.h"

BOOL NPC_EventAddEgg(int meindex, int talker, char *buff2,int mode);
BOOL NPC_EventAddItem(int meindex,int talker,char *buf);
BOOL NPC_EventBigSmallLastCheck(int point1,int mypoint,int flg);
BOOL NPC_TiemCheck(int meindex,int talker,int time,int flg);

BOOL NPC_ExChangeManInit( int meindex );
void NPC_ExChangeManTalked( int meindex , int talkerindex , char *szMes ,int color );
void NPC_ExChangeManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
BOOL NPC_EventBigSmallCheck( int meindex, int talkerindex, char *data);

#endif
