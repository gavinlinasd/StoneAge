#ifndef __NPC_WELFARE_H__
#define __NPC_WELFARE_H__

BOOL NPC_WelfareInit( int meindex );
void NPC_WelfareTalked( int meindex , int talker , char *msg ,int color );
void NPC_WelfareWindowTalked(int meindex, int talkerindex, int seqno, int select,char *data);
void NPC_WelfareMakeStr(int meindex,int toindex,int select);


#endif
