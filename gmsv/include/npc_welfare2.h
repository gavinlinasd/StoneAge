#ifndef __NPC_WELFARE2_H__
#define __NPC_WELFARE2_H__

BOOL NPC_WelfareInit2( int meindex );
void NPC_WelfareTalked2( int meindex , int talker , char *msg ,int color );
void NPC_WelfareWindowTalked2(int meindex, int talkerindex, int seqno, int select,char *data);
void NPC_WelfareMakeStr2(int meindex,int toindex,int select);

#endif
