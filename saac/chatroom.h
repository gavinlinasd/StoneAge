#ifndef _CHATROOM_H_
#define _CHATROOM_H_
#include "version.h"

#ifdef _UNIVERSE_CHATROOM
#define MAX_CHATROOM 10 
#define MAX_PPLINROOM 100


typedef struct _tagChatRoomCharaList{
	char cdkey[256];
	char name[256];
	char own[256];
	int fd;
	int use;
}CRCharaList;

typedef struct _tagUniChatRoom
{
	int use;
	int charanum;
	int masindex;
	int masfd;
	CRCharaList charalist[MAX_PPLINROOM];
	char chatname[32];
}UniChatRoomlist;

void CHATROOM_resetChat( int chat);
void CHATROOM_Init( void);
int CHATROOM_getfreeChatRoom( void);
int CHATROOM_CreateChatRoom( int fd, char *cdkey, char *data,
							char *message, int userindex, int clifdid);
void CHATROOM_DelChatRoom( char *Cdkey, char *data );
void CHATROOM_LeaveChatRoom( char *Cdkey, char *data );
void CHATROOM_KickChatRoom( char *Cdkey, char *data );
void CHATROOM_JoinChatRoom( char *Cdkey, char *data );
void CHATROOM_AgreeJoinCR( int fd, char *data, int userindex, int clifd);
int CHATROOM_AgreeSomeOne( int chat, char *Cdkey, char *name, char *own, int whofd);
void CHATROOM_AgreeChatRoom( char *Cdkey, char *data );
void CHATROOM_MasterChatRoom( char *Cdkey, char *data );
void CHATROOM_MessageChatRoom( char *Cdkey, char *data );
int CHATROOM_UpdateChatRoom( int fd, int chat);
void CHATROOM_GSClosedHandle( int fd);

void CHATROOM_RecvAll( int fd, char *cdkey, char *data, int userindex, int clifdid );

#endif
#endif
