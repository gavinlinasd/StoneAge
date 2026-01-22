#ifndef __NPC_STONESERVICEMAN_H__
#define __NPC_STONESERVICEMAN_H__

// 訧蹋揹蹈
typedef struct _List
{
  char Data[32];        // 訧蹋
  struct _List *Next;   // 硌砃狟珨跺List
}List,*LPList;    

BOOL NPC_StoneServiceManInit(int meindex);
void NPC_StoneServiceManLoop(int meindex);
void NPC_StoneServiceManTalked(int meindex,int talkerindex,char *msg,int color);
void NPC_StoneServiceManWindowTalked(int meindex,int talkerindex,int seqno,
                                     int select,char *data);
int SendToAP(int nAid,int nIndex,int nLen,void *Data1,void *Data2);
int RecvFromAP(int *nAid,int nIndex,int *nLen,LPList *Data);
int GetMsg(int meindex);
int ConnectToAP(void);
#endif
