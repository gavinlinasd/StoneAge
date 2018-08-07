#ifndef __NPC_VERYWELFARE_H__
#define __NPC_VERYWELFARE_H__

BOOL NPC_VeryWelfareInit( int meindex );
void NPC_VeryWelfareTalked( int meindex , int talker , char *msg ,int color );
void NPC_VeryWelfareWindowTalked(int meindex, int talkerindex, int seqno, int select,char *data);

#endif
