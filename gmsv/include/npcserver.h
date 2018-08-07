#ifndef _NPCSERVER_H
#define _NPCSERVER_H


int NSproto_DispatchMessage(int fd,char* encoded);
int connectNpcServer(char* hostname,unsigned short port);
void NPCS_SendProbe( int fd);

typedef struct _NPCMessCommand
{
	char buf[4096];
	int len;
	struct _NPCMessCommand *next;
}NPCMessC;

BOOL NPCMESS_setChar( char *buf);
BOOL NPCMESS_getChar( char *buf, int len);
NPCMessC *MESS_getNew();


void NPCS_AskNpcList_recv( int fd, char *Nlist);
void NPCS_AskNpcList_send( int fd);

void NPCS_NpcSLogin_recv( int fd, char *Mess);
void NPCS_NpcSLogin_send( int fd);

void NPCS_AskNpcTalk_send( int objindex, int npcsindex, int charaindex, char *Nlist);

void NPCS_NpcTalkMess_recv( int npcobjindex, int charaindex, int charobjindex, char *TalkMess, int Color);
void NPCS_NpcWinMess_recv( int npcobjindex, int charaindex, int charobjindex, char *WinMess,
						  int seqno, int windowtype, int buttontype, int page);
void NPCS_NpcWinMess_send( int npcobjindex, int npcindex, int charaindex, char *WinMess,
						  int seqno, int select);
void NPCS_NpcCheckFreeMess_recv( int npcobjindex, int charaindex, int charobjindex,
								char *CheckfreeMess);

#endif

