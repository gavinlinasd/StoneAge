#ifndef __NPC_MANORSMAN_H__
#define __NPC_MANORSMAN_H__

BOOL NPC_ManorSmanInit(int meindex);
void NPC_ManorSmanTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_ManorSmanWindowTalked(int meindex, int talkerindex,
				 int seqno, int select, char *data);
void NPC_ManorSmanLoop(int meindex);
//andy_123
int NPC_getManorsmanListIndex( int ID);
#ifndef _NEW_MANOR_LAW
void NPC_ManorSavePKSchedule(int meindex, int toindex, int flg);
#else
void NPC_ManorSavePKSchedule(int meindex, int toindex, int flg,int setTime,struct tm tm1);
void NPC_ManorAddToSchedule(int meindex,int charaindex);
#endif

#endif 
/*__NPC_MANORSMAN_H__*/
