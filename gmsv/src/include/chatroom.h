#ifndef __CHATROOM_H__
#define __CHATROOM_H__
#include "version.h"

#define MAX_CHATROOM 10 
#define MAX_PPLINROOM 100

#ifdef _UNIVERSE_CHATROOM

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

void resetChat_users( int chat, int ti);
void InitChatRoom( void );
void ChatRoom_List ( int fd );
int ChatRoom_getfree( void);
void saac_ChatRoom_recvall ( int fd , char *result, char *data, int charaindex, int clifdid);
void ChatRoom_Leave( int charaindex);
void ChatRoom_Destroy ( char *data);
void ChatRoom_recvall( int fd, char *data);
void CHATROOM_getChatRoomList( void);

#else

void InitChatRoom ( void ) ; 
void ChatRoom_List ( int fd );
BOOL ChatCheck_BeMaster( int myindex, int chatnum);
BOOL ChatCheck_Free( int myindex);
BOOL ChatRoom_Create ( int myindex , char *message);
BOOL ChatRoom_Destroy ( int myindex ) ; 
void ChatRoom_Kick ( int myindex , int toindex ) ; 
void ChatRoom_Make ( int myindex , int toindex ) ; 
void ChatRoom_Leave ( int myindex ) ; 
void ChatRoom_Join ( int myindex , int num ) ; 
void ChatRoom_Agree ( int myindex , int toindex , int YesNo ) ; 
void ChatRoom_Message ( int myindex , char *message ) ; 
void ChatRoom_Refresh ( int Num ) ; 
void ChatRoom_recvall ( int fd , char *data ) ; 

#endif

#endif
