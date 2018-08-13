#include "version.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "chatmagic.h"
#include "battle.h"
#include "log.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "family.h"
#include "chatroom.h"
#include "net.h"
#include "util.h"
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道



#ifdef _UNIVERSE_CHATROOM

UniChatRoomlist ChatRoom[MAX_CHATROOM];
BOOL ChatCheck_BeMaster( int myindex, int chatnum)
{
	if( !CHAR_CHECKINDEX ( myindex ) ) 
		return FALSE;

	if( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM ) == 0 ) return FALSE;

	if( chatnum != -1 ){
		if( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM ) != chatnum )
			return FALSE;
	}
	if( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 1 ) return TRUE;

	return FALSE;
}

BOOL ChatCheck_Free( int myindex)
{
	int old_gold;
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return FALSE;
	old_gold = CHAR_getInt( myindex, CHAR_GOLD );
	if( old_gold < 200 ){
		CHAR_talkToCli ( myindex , -1 , "成立聊天室需花费２００石币" , CHAR_COLORYELLOW ); 
		return FALSE;
	}
	if ( CHAR_getInt ( myindex , CHAR_LV ) < 30 &&
		CHAR_getInt ( myindex , CHAR_TRANSMIGRATION ) < 1 ) {
		CHAR_talkToCli ( myindex , -1 , "成立聊天室需０转３０级以上！" , CHAR_COLORYELLOW );
		return FALSE;
	}
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) != 0 ) {
		CHAR_talkToCli ( myindex , -1 , "你已经在其他聊天室中！" , CHAR_COLORYELLOW );
		return FALSE;
	}
	return TRUE;
}

void resetChat_users( int chat, int ti)
{
	if( chat<0 || chat >= MAX_CHATROOM ) return;
	if( ti<0 || ti >= MAX_PPLINROOM ) return;
	memset( ChatRoom[chat].charalist[ti].cdkey, 0, sizeof(ChatRoom[chat].charalist[ti].cdkey));
	memset( ChatRoom[chat].charalist[ti].name, 0, sizeof(ChatRoom[chat].charalist[ti].name));
	memset( ChatRoom[chat].charalist[ti].own, 0, sizeof(ChatRoom[chat].charalist[ti].own));
}

void InitChatRoom( void )
{
	int i, j;
	for ( i = 0 ; i < MAX_CHATROOM ; i ++ ) {
		ChatRoom[ i ].use=0; 
		ChatRoom[ i ].masindex= -1; 
		ChatRoom[ i ].charanum= 0; 
 		for ( j = 0 ; j < MAX_PPLINROOM ; j ++ )
			resetChat_users( i, j);
	}
}

int ChatRoom_getfree( void)
{
	int i;
	for ( i=0; i<MAX_CHATROOM; i++) {
		if( ChatRoom[ i ].use !=0 ) continue;
		return i;
	}
	return -1;
}

void ChatRoom_Refresh ( int Num )
{
	int fd , i, j; 
	char token[4096];
	char token2[4096];
	char buf[64];
	int playernum = CHAR_getPlayerMaxNum();
	if( ChatRoom[ Num ].use == FALSE ) return;
	if( ChatRoom[Num].masindex<0 || ChatRoom[Num].masindex>MAX_PPLINROOM ) return;

	sprintf ( token , "R|r%d|%s|%d|%s|p%d|" , 
		Num, ChatRoom[ Num ].chatname, ChatRoom[ Num ].masindex,
		ChatRoom[Num].charalist[ChatRoom[Num].masindex].name, ChatRoom[ Num ].charanum  );

	for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
		if ( ChatRoom[ Num ].charalist[i].use == 0 ) continue;
		sprintf( buf , "I%d|%s|%s|" , 
			i, ChatRoom[Num].charalist[i].name, ChatRoom[Num].charalist[i].own	);
		strncat ( token , buf , sizeof ( buf ) ) ; 
	}

	for ( j=0; j<playernum; j++) {
		if( !CHAR_getCharUse( j) ||
			CHAR_getInt( j, CHAR_WHICHTYPE)  != CHAR_TYPEPLAYER ) continue;
		if( CHAR_getWorkInt ( j, CHAR_WORKCHATROOMNUM) != Num ) continue;
		if( CHAR_getWorkInt( j, CHAR_WORKCHATROOMTYPE) == 0 ) continue;
//		if( !strcmp( ChatRoom[Num].charalist[ChatRoom[Num].masindex].cdkey, CHAR_getChar( j, CHAR_CDKEY)) &&
//			!strcmp( ChatRoom[Num].charalist[ChatRoom[Num].masindex].name, CHAR_getChar( j, CHAR_NAME)) ){
			fd=getfdFromCharaIndex ( j);
			sprintf ( token2, "%s%d|", token, CHAR_getWorkInt( j, CHAR_WORKCHATROOMTYPE));
			lssproto_CHATROOM_send ( fd , token2 ) ; 
//			break;
//		}
	}
}

void ChatRoom_List ( int fd )
{
	int i; 
	char buf[256];
	char token[2048] = "B|";
	for( i=0; i<MAX_CHATROOM; i++) {
		if( ChatRoom[ i ].use == 0 ) continue;
		sprintf( buf, "r%d|%s|%d|%s|p%d|", 
			i , ChatRoom[ i ].chatname, 
			ChatRoom[ i ].masindex , 
			ChatRoom[ i ].charalist[ChatRoom[ i ].masindex].name,
			ChatRoom[ i ].charanum );
		strncat ( token , buf , sizeof ( buf ) ) ; 

	}
	lssproto_CHATROOM_send( fd, token); 
}

void ChatRoom_Leave( int charaindex)
{
	int i, ti=-1, chat, fd;
	char token[1024];
	fd = getfdFromCharaIndex( charaindex);

	if( (chat = CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMNUM)) < 0 ) return;
	CHAR_setWorkInt( charaindex, CHAR_WORKCHATROOMNUM, -1);
	if( CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMTYPE) == 0 ) return;
	CHAR_setWorkInt( charaindex, CHAR_WORKCHATROOMTYPE, 0);

	for( i=0; i<MAX_PPLINROOM; i++){
		if( ChatRoom[chat].charalist[i].use == 0 ) continue;
		if( !strcmp( ChatRoom[chat].charalist[i].cdkey, CHAR_getChar(charaindex, CHAR_CDKEY)) &&
			!strcmp( ChatRoom[chat].charalist[i].name, CHAR_getChar(charaindex, CHAR_NAME)) ){
			ti = i;
			break;
		}
	}
	if( ti == -1 ) return;
	snprintf( token, sizeof( token),"L|%d|%d|", chat, ti );
	fd = getfdFromCharaIndex( charaindex);
	saacproto_ACUniChatroom_send( acfd, charaindex, CONNECT_getFdid(fd),
			   CHAR_getChar( charaindex, CHAR_CDKEY), token);

}

void ChatRoom_Destroy ( char *data)
{
	char buf[256];
	int chat, j, fd;
	int playernum = CHAR_getPlayerMaxNum();
	if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof( buf)) == FALSE ) return;
	chat = atoi( buf);
	if( chat < 0 || chat >= MAX_CHATROOM ) return;

	sprintf ( buf , "D|%d", chat);
	for ( j=0; j<playernum; j++) {
		if( !CHAR_getCharUse( j) ||
			CHAR_getInt( j, CHAR_WHICHTYPE)  != CHAR_TYPEPLAYER ) continue;
		if( CHAR_getWorkInt ( j, CHAR_WORKCHATROOMNUM) != chat ) continue;
		fd = getfdFromCharaIndex( j);
		lssproto_CHATROOM_send ( fd, buf);
		CHAR_setWorkInt ( j, CHAR_WORKCHATROOMTYPE, 0) ; 
		CHAR_setWorkInt ( j, CHAR_WORKCHATROOMNUM, -1) ; 
	}
	ChatRoom[ chat].use=0;
	ChatRoom[ chat].masindex= -1; 
	ChatRoom[ chat].charanum= 0; 
 	for ( j = 0 ; j < MAX_PPLINROOM ; j ++ )
		resetChat_users( chat, j);

}

//saacproto_ACUniChatroom_recv
void ChatRoom_recvall ( int fd , char *data )
{ 
	char Head[4];
	char token[1024];
	char buf[256];
	int charaindex;

	charaindex = CONNECT_getCharaindex(fd);
	if ( !CHAR_CHECKINDEX ( charaindex ) )return ;

	getStringFromIndexWithDelim( data , "|", 1, Head, sizeof(Head));

	//andy_log
	print( "CR:%s.\n", data);
	if ( strcmp ( Head , "C" ) == 0 ) { // 成立频道
		char chatname[256];
		if( ChatCheck_Free( charaindex) == FALSE ) return;
		if( getStringFromIndexWithDelim( data , "|", 2, chatname, sizeof(chatname)) == FALSE ) return;
		if( strlen( chatname) >= 32 ) return;
		snprintf( token, sizeof( token),"C|%s|%s|%s|%s|",
			chatname, CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME),
			CHAR_getChar( charaindex, CHAR_OWNTITLE)
		);
	}else if ( strcmp ( Head , "D" ) == 0 ) { // 删除频道
		int chat;

		if( CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMTYPE) != 1 ) return;
		if( (chat = CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMNUM)) < 0 ) return;
		if( chat >= MAX_CHATROOM ) return;
		snprintf( token, sizeof( token),"D|%d|%s|%s|",
			chat, CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME)
		);
		//ChatRoom_Destroy( myindex);
/*
	}else if ( strcmp ( Head, "A") == 0 ) {// 同意加入频道
		getStringFromIndexWithDelim( data , "|", 2, message, sizeof(message));
		getStringFromIndexWithDelim( data , "|", 3, buf, sizeof(buf));
		ChatRoom_Agree ( myindex , atoi( message ) , atoi( buf ) ) ;
*/
	}else if ( strcmp( Head , "J") == 0 ) {//加入频道
		int chat;
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof(buf)) == FALSE ) return;
		chat = atoi( buf);
		if( chat < 0 || chat >= MAX_CHATROOM ) return;
		snprintf( token, sizeof( token),"J|%d|%s|%s|%s|",
			chat, CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME),
			CHAR_getChar( charaindex, CHAR_OWNTITLE)
		);
//		ChatRoom_Join ( myindex , atoi( message ) ) ;
	}else if ( strcmp( Head , "L") == 0 ) {// 离开频道
		ChatRoom_Leave( charaindex);
		return;
	}else if ( strcmp ( Head , "K" ) == 0 ) {//踢出频道
		int ti, chat;
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof( buf)) == FALSE ) return;
		ti = atoi( buf);
		if( ti<0 || ti>=MAX_PPLINROOM ) return;
		if( CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMTYPE) != 1 ) return;
		if( (chat = CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMNUM)) < 0 ) return;

		if( chat >= MAX_CHATROOM ) return;
		snprintf( token, sizeof( token),"K|%d|%s|%s|%d|",
			chat,
			CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME),
			ti );

//		ChatRoom_Kick ( myindex , atoi( message ) ); 
	}else if ( strcmp ( Head , "M" ) == 0 ) { // 更换室长
		int ti, chat;
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof(buf)) == FALSE ) return;
		ti = atoi( buf);
		if( CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMTYPE) != 1 ) return;
		if( (chat = CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMNUM)) < 0 ) return;
		if( chat >= MAX_CHATROOM ) return;
		if( ChatRoom[chat].charalist[ti].use == 0 ) return;
		snprintf( token, sizeof( token),"M|%d|%s|%s|%d|",
			chat,
			CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME),
			ti );
//		ChatRoom_Make ( myindex , atoi( message ) ); 
	}else if ( strcmp ( Head , "T" ) == 0 ) {// 频道讯息
		int chat;
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof(buf)) == FALSE ) return;
		if( CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMTYPE) == 0 ) return;
		if( (chat = CHAR_getWorkInt( charaindex, CHAR_WORKCHATROOMNUM)) < 0 ) return;
		if( chat >= MAX_CHATROOM ) return;
		snprintf( token, sizeof( token),"T|%d|%s|",	chat, buf);
		
//		ChatRoom_Message ( myindex , message ) ; 
	}else if ( strcmp ( Head , "B" ) == 0 ) {// 聊天室清单
		ChatRoom_List ( fd );
		return;
	}

	fd = getfdFromCharaIndex( charaindex);
	saacproto_ACUniChatroom_send( acfd, charaindex, CONNECT_getFdid(fd),
			   CHAR_getChar( charaindex, CHAR_CDKEY), token);
}

void saac_ChatRoom_recvall ( int fd , char *result, char *data, int charaindex, int clifdid)
{ 
	char Head[4], buf[256];
	int chat;

	charaindex = getCharindexFromFdid(clifdid);
	//andy_log
	print( "saac CR:%s.\n", data);
	if( getStringFromIndexWithDelim( data , "|", 1, Head, sizeof(Head)) == FALSE ) return;

	if( strcmp( Head, "C") == 0 ) { // 成立频道
//		int fd = getfdFromCharaIndex( charaindex);
//		if( getStringFromIndexWithDelim( data , "|", 2, result, sizeof(result)) == FALSE ) return;
		if( !strcmp( result, SUCCESSFUL) ){

			if( getStringFromIndexWithDelim( data , "|", 3, buf, sizeof(buf)) == FALSE ) return;
			chat = atoi( buf);
			if( chat < 0 || chat >= MAX_CHATROOM ) return;
			if( getStringFromIndexWithDelim( data , "|", 4, ChatRoom[chat].chatname,
				sizeof(ChatRoom[chat].chatname)) == FALSE ) return;
			if( getStringFromIndexWithDelim( data , "|", 5, buf, sizeof(buf)) == FALSE ) return;
			ChatRoom[chat].masindex = atoi( buf);
			if( getStringFromIndexWithDelim( data , "|", 6,
				ChatRoom[chat].charalist[0].cdkey, sizeof(ChatRoom[chat].charalist[0].cdkey)) == FALSE ) return;
			if( getStringFromIndexWithDelim( data , "|", 7,
				ChatRoom[chat].charalist[0].name, sizeof(ChatRoom[chat].charalist[0].name)) == FALSE ) return;
			if( getStringFromIndexWithDelim( data , "|", 8,
				ChatRoom[chat].charalist[0].own, sizeof(ChatRoom[chat].charalist[0].own)) == FALSE ) return;
			ChatRoom[chat].charalist[0].use=1;
			if( CHAR_DelGold( charaindex, 200) == 0 ) return;
			ChatRoom[chat].use = 1;
			CHAR_setWorkInt ( charaindex , CHAR_WORKCHATROOMTYPE , 1 ) ; 
			CHAR_setWorkInt ( charaindex , CHAR_WORKCHATROOMNUM , chat ) ; 
			CHAR_talkToCli ( charaindex , -1 , "成立聊天室扣除２００石币。" , CHAR_COLORYELLOW );

			ChatRoom_Refresh( chat);
		}else{
			CHAR_talkToCli ( charaindex , -1 , "无法成立聊天室，聊天频道已满或条件不足！" , CHAR_COLORYELLOW );
		}
	}else if( strcmp( Head, "U") == 0 ) {
		int k=2, ti;
		char bufarg[512];
		if( getStringFromIndexWithDelim( data , ",", 1, bufarg, sizeof(bufarg)) == FALSE ) return;
		if( getStringFromIndexWithDelim( bufarg , "|", 2, buf, sizeof(buf)) == FALSE ) return;
		chat = atoi( buf);
		if( chat < 0 || chat >= MAX_CHATROOM ) return;
		if( getStringFromIndexWithDelim( bufarg , "|", 3, buf, sizeof(buf)) == FALSE ) return;
		ChatRoom[chat].use = atoi( buf);
		if( getStringFromIndexWithDelim( bufarg , "|", 4,
			ChatRoom[chat].chatname, sizeof(ChatRoom[chat].chatname)) == FALSE ) return;
		if( getStringFromIndexWithDelim( bufarg , "|", 5, buf, sizeof(buf)) == FALSE ) return;
		ChatRoom[chat].masindex = atoi( buf);
		if( getStringFromIndexWithDelim( bufarg , "|", 6, buf, sizeof(buf)) == FALSE ) return;
		ChatRoom[chat].charanum = atoi( buf);

		while( getStringFromIndexWithDelim( data , ",", k, bufarg, sizeof( bufarg) ) ){
			k++;
			if( bufarg[0] == 0 || strlen( bufarg) <= 0 ) continue;
			if( getStringFromIndexWithDelim( bufarg , "|", 1, buf, sizeof(buf)) == FALSE ) return;
			ti = atoi( buf);
			if( getStringFromIndexWithDelim( bufarg , "|", 2,
				ChatRoom[chat].charalist[ti].cdkey, sizeof(ChatRoom[chat].charalist[ti].cdkey)) == FALSE ) return;
			if( getStringFromIndexWithDelim( bufarg , "|", 3,
				ChatRoom[chat].charalist[ti].name, sizeof(ChatRoom[chat].charalist[ti].name)) == FALSE ) return;
			if( getStringFromIndexWithDelim( bufarg , "|", 4,
				ChatRoom[chat].charalist[ti].own, sizeof(ChatRoom[chat].charalist[ti].own)) == FALSE ) return;
			ChatRoom[chat].charalist[ti].use = 1;
		}
		ChatRoom_Refresh( chat);
	}else if( strcmp( Head, "D") == 0 ) {
		ChatRoom_Destroy ( data);
	}else if( strcmp( Head, "L") == 0 ) {
		int chat, ti;
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof( buf)) == FALSE ) return;
		chat = atoi(buf);
		if( chat < 0 || chat >= MAX_CHATROOM ) return;
		if( getStringFromIndexWithDelim( data , "|", 3, buf, sizeof( buf)) == FALSE ) return;
		ti = atoi(buf);
		if( ti<0||ti>=MAX_PPLINROOM)return;
		ChatRoom[chat].charalist[ti].use = 0;
		ChatRoom_Refresh( chat);
	}else if( strcmp( Head, "M") == 0 ) {
		int ti, chat, j;
		int playernum = CHAR_getPlayerMaxNum();
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof( buf)) == FALSE ) return;
		chat = atoi( buf);
		if( chat < 0 || chat >= MAX_CHATROOM ) return;
		if( getStringFromIndexWithDelim( data , "|", 3, buf, sizeof( buf)) == FALSE ) return;
		ti = atoi( buf);
		if( ti<0 || ti>=MAX_PPLINROOM )return;
		if( ChatRoom[chat].charalist[ti].use == 0 ) return;
		for ( j=0; j<playernum; j++) {
			if( !CHAR_getCharUse( j) ||
				CHAR_getInt( j, CHAR_WHICHTYPE)  != CHAR_TYPEPLAYER ) continue;
			if( CHAR_getWorkInt ( j, CHAR_WORKCHATROOMNUM) != chat ) continue;


			if( !strcmp( CHAR_getChar( j, CHAR_CDKEY), ChatRoom[chat].charalist[ti].cdkey) &&
				!strcmp( CHAR_getChar( j, CHAR_NAME), ChatRoom[chat].charalist[ti].name) ){
				CHAR_setWorkInt( j, CHAR_WORKCHATROOMTYPE, 1);
				CHAR_talkToCli( j, -1 , "你现在是聊天室的室长！", CHAR_COLORRED);
			}
			if( !strcmp( CHAR_getChar( j, CHAR_CDKEY), ChatRoom[chat].charalist[ChatRoom[chat].masindex].cdkey) &&
				!strcmp( CHAR_getChar( j, CHAR_NAME), ChatRoom[chat].charalist[ChatRoom[chat].masindex].name) ){
				CHAR_setWorkInt( j, CHAR_WORKCHATROOMTYPE, 2);
				CHAR_talkToCli( j, -1 , "你现在已经不是聊天室的室长！", CHAR_COLORRED);
			}
		}
		ChatRoom[ chat].masindex = ti;
		ChatRoom_Refresh( chat);
	}else if( strcmp( Head, "K") == 0 ) {
		int ti, j, chat, fd;
		int playernum = CHAR_getPlayerMaxNum();
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof( buf)) == FALSE ) return;
		chat = atoi( buf);
		if( chat < 0 || chat >= MAX_CHATROOM ) return;
		if( getStringFromIndexWithDelim( data , "|", 3, buf, sizeof( buf)) == FALSE ) return;
		ti = atoi( buf);
		if( ti<0 || ti>=MAX_PPLINROOM ) return;
//andy_log
print( "\nK|%d|%d|\n", chat, ti);

		for ( j=0; j<playernum; j++) {
			if( !CHAR_getCharUse( j) ||
				CHAR_getInt( j, CHAR_WHICHTYPE)  != CHAR_TYPEPLAYER ) continue;
			if( CHAR_getWorkInt ( j, CHAR_WORKCHATROOMNUM) != chat ) continue;
			if( CHAR_getWorkInt ( j, CHAR_WORKCHATROOMTYPE) != 2 ) continue;

			if( !strcmp( CHAR_getChar( j, CHAR_CDKEY), ChatRoom[chat].charalist[ti].cdkey) &&
				!strcmp( CHAR_getChar( j, CHAR_NAME), ChatRoom[chat].charalist[ti].name) ){
				fd = getfdFromCharaIndex( j );
				
				CHAR_setWorkInt ( j, CHAR_WORKCHATROOMNUM, -1);
				CHAR_setWorkInt ( j, CHAR_WORKCHATROOMTYPE, 0);
//andy_log
print( "_CHATROOM_send( %d, K|) \n", fd );
				lssproto_CHATROOM_send ( fd , "K|" );

				break;
			}
		}
//andy_log
print( "ChatRoom_Refresh( %d) \n", chat);
		ChatRoom[chat].charalist[ti].use = 0;
		ChatRoom_Refresh( chat);
	}else if ( strcmp ( Head , "J" ) == 0 ) {//加入频道
	}else if ( strcmp ( Head , "A" ) == 0 ) {
		if ( !CHAR_CHECKINDEX ( charaindex ) ) return;

		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof( buf)) == FALSE ) return;
		chat = atoi( buf);
		if( chat < 0 || chat >= MAX_CHATROOM ) return;
		if( getStringFromIndexWithDelim( data , "|", 3, result, sizeof( result)) == FALSE ) return;
		if( !strcmp( result, "FULL") ){
			CHAR_talkToCli( charaindex, -1, "该频道已满！", CHAR_COLORYELLOW);
		}else if( !strcmp( result, "OK") ){
			CHAR_talkToCli( charaindex, -1, "加入聊天频道！", CHAR_COLORYELLOW);
			CHAR_setWorkInt( charaindex, CHAR_WORKCHATROOMNUM, chat);
			CHAR_setWorkInt( charaindex, CHAR_WORKCHATROOMTYPE, 2);
		}
		ChatRoom_Refresh( chat);
	}else if( !strcmp( Head, "T") ) {//频道讯息
		int j;
		char message[256];
		int playernum = CHAR_getPlayerMaxNum();
		if( getStringFromIndexWithDelim( data , "|", 2, buf, sizeof( buf)) == FALSE ) return;
		chat = atoi( buf);
		if( chat < 0 || chat >= MAX_CHATROOM ) return;
		if( getStringFromIndexWithDelim( data , "|", 3, buf, sizeof( buf)) == FALSE ) return;
		snprintf ( message, sizeof ( message ) , "T|%s" , buf );
		for ( j=0; j<playernum; j++) {
			if( !CHAR_getCharUse( j) ||
				CHAR_getInt( j, CHAR_WHICHTYPE)  != CHAR_TYPEPLAYER ) continue;
			if( CHAR_getWorkInt ( j, CHAR_WORKCHATROOMNUM) != chat ) continue;
			if( CHAR_getWorkInt( j, CHAR_WORKCHATROOMTYPE) == 0 ) continue;
			snprintf ( message, sizeof ( message ) , "T|%s" , buf );
			fd = getfdFromCharaIndex ( j);
			lssproto_CHATROOM_send ( fd , message) ; 
		}
	}else{

	}
}

void CHATROOM_getChatRoomList( void)
{
	char token[512];
	snprintf( token, sizeof( token),"U|-1|"	);
	saacproto_ACUniChatroom_send( acfd, -1, -1, "SYS", token);
}
#else


/*
CHAR_WORKCHATROOMTYPE :
	0 : 无
	1 : 聊天室室长
	2 : 聊天室成员
*/
typedef struct {
	BOOL useFlag ;
	int NowPeople;
	int Maker;
	int MemberList[MAX_PPLINROOM];
	char RoomName[32];
} CHATROOM_CLASS ; 
CHATROOM_CLASS ChatRoom[MAX_CHATROOM];


BOOL ChatCheck_BeMaster( int myindex, int chatnum)
{
	if( !CHAR_CHECKINDEX ( myindex ) ) 
		return FALSE;

	if( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM ) == 0 ) return FALSE;

	if( chatnum != -1 ){
		if( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM ) != chatnum )
			return FALSE;
	}
	if( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 1 ) return TRUE;

	return FALSE;
}

BOOL ChatCheck_Free( int myindex)
{
	int old_gold;
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return FALSE;
	old_gold = CHAR_getInt( myindex, CHAR_GOLD );
	if( old_gold < 200 ){
		CHAR_talkToCli ( myindex , -1 , "成立聊天室需花费２００石币" , CHAR_COLORYELLOW ); 
		return FALSE;
	}
	if ( CHAR_getInt ( myindex , CHAR_LV ) < 30 &&
		CHAR_getInt ( myindex , CHAR_TRANSMIGRATION ) < 1 ) {
		CHAR_talkToCli ( myindex , -1 , "成立聊天室需０转３０级以上！" , CHAR_COLORYELLOW );
		return FALSE;
	}
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) != 0 ) {
		CHAR_talkToCli ( myindex , -1 , "你已经在其他聊天室中！" , CHAR_COLORYELLOW );
		return FALSE;
	}
	return TRUE;
}

void InitChatRoom ( void ) {
	int i , j ; 
	for ( i = 0 ; i < MAX_CHATROOM ; i ++ ) {
		ChatRoom[ i ].useFlag = FALSE ; 
		ChatRoom[ i ].Maker = -1 ; 
		ChatRoom[ i ].NowPeople = -1 ; 
		sprintf ( ChatRoom[ i ].RoomName , " " ) ; 
		for ( j = 0 ; j < MAX_PPLINROOM ; j ++ ) 
			ChatRoom[ i ].MemberList[ j ] = -1 ; 
	}
}

BOOL ChatRoom_Create ( int myindex , char *message )
{
	int i ; 

	if( ChatCheck_Free( myindex) == FALSE ) return FALSE;

	for ( i = 0 ; i < MAX_CHATROOM ; i ++ ) {
		if ( !ChatRoom[ i ].useFlag ) {
			ChatRoom[ i ].useFlag = TRUE ; 
			ChatRoom[ i ].Maker = myindex ; 
			ChatRoom[ i ].NowPeople = 1 ; 
			ChatRoom[ i ].MemberList[ 0 ] = myindex ; 
			sprintf ( ChatRoom[ i ].RoomName , "%s" , message ) ; 
			CHAR_setWorkInt ( myindex , CHAR_WORKCHATROOMTYPE , 1 ) ; 
			CHAR_setWorkInt ( myindex , CHAR_WORKCHATROOMNUM , i ) ; 
			ChatRoom_Refresh ( i ) ; 

			CHAR_DelGold( myindex, 200);
			CHAR_talkToCli ( myindex , -1 , "成立聊天室扣除２００石币" , CHAR_COLORYELLOW ); 
			return TRUE; 
		}
	}
	CHAR_talkToCli ( myindex , -1 , "聊天室已满无法建立新的聊天频道！" , CHAR_COLORYELLOW ); 
	return FALSE; 
}

BOOL ChatRoom_Destroy ( int myindex )
{
	int i , j ; 
	int fd ;/*= getfdFromCharaIndex(myindex);*/
	char buf[16];
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return FALSE;
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 1 ) {
		i = CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM ) ; 
		ChatRoom[ i ].useFlag = FALSE ; 
		ChatRoom[ i ].Maker = -1 ; 
		ChatRoom[ i ].NowPeople = -1 ; 

		// WON FIX 
		memset( ChatRoom[ i ].RoomName, 0 , sizeof(ChatRoom[ i ].RoomName) );
		//sprintf ( ChatRoom[ i ].RoomName , " " ) ; 

		for ( j = 0 ; j < MAX_PPLINROOM ; j ++ ) {
			if ( !CHAR_CHECKINDEX ( ChatRoom[ i ].MemberList[ j ] ) ) {
				ChatRoom[ i ].MemberList[ j ] = -1 ; 
				continue;
			}
			CHAR_setWorkInt ( ChatRoom[ i ].MemberList[ j ] , CHAR_WORKCHATROOMTYPE , 0 ) ; 
			CHAR_setWorkInt ( ChatRoom[ i ].MemberList[ j ] , CHAR_WORKCHATROOMNUM , 0 ) ; 
			fd = getfdFromCharaIndex( ChatRoom[ i ].MemberList[ j ] );
			sprintf ( buf , "D|%d" , i ) ; 
			lssproto_CHATROOM_send ( fd , buf ) ; 
			ChatRoom[ i ].MemberList[ j ] = -1 ; 
		}
		return TRUE;
	}
	return FALSE;
}

void ChatRoom_Message ( int myindex ,  char *message )
{
	int i , j , fd ; 
	char buf[1024];
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return ;
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) > 0 ) {
		i = CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM ) ; 
		
		// WON FIX
		if( i < 0 ) return;

		for ( j = 0 ; j < MAX_PPLINROOM ; j ++ ) {
			if ( !CHAR_CHECKINDEX ( ChatRoom[ i ].MemberList[ j ] ) ) 
				continue;
			snprintf ( buf , sizeof ( buf ) , "T|%s" , message ) ; 
			fd = getfdFromCharaIndex ( ChatRoom[ i ].MemberList[ j ] );
			lssproto_CHATROOM_send ( fd , buf ) ; 
		}
	}
}

void ChatRoom_Kick ( int myindex , int toindex )
{
	int i , fd ; 
	int Num = CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM );
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return ;
	if ( !CHAR_CHECKINDEX ( toindex ) ) 
		return ;
	if ( myindex == toindex ) return;
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 1 ) {
		if ( Num != CHAR_getWorkInt ( toindex , CHAR_WORKCHATROOMNUM ) )
			return;
		else {
			CHAR_setWorkInt ( toindex , CHAR_WORKCHATROOMTYPE , 0 ) ; 
			CHAR_setWorkInt ( toindex , CHAR_WORKCHATROOMNUM , -1) ; 
			//CHAR_talkToCli ( toindex , -1 , "室长将你踢出聊天室！" , CHAR_COLORRED ) ; 
			fd = getfdFromCharaIndex( toindex );
			lssproto_CHATROOM_send ( fd , "K|" ) ; 
			ChatRoom[ Num ].NowPeople --;
			for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
				if ( ChatRoom[ Num ].MemberList[ i ] == toindex ) {
					ChatRoom[ Num ].MemberList[ i ] = -1 ; 
					break;
				}
			}
		}
	}
	ChatRoom_Refresh ( Num ) ; 
}

void ChatRoom_Make ( int myindex , int toindex )
{
	int Num = CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM );
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return ;
	if ( !CHAR_CHECKINDEX ( toindex ) ) 
		return ;
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 1 ) {
		if ( Num != CHAR_getWorkInt ( toindex , CHAR_WORKCHATROOMNUM ) )
			return;
		else if ( CHAR_getInt ( toindex , CHAR_LV ) >= 30 || 
			CHAR_getInt ( toindex , CHAR_TRANSMIGRATION ) >= 1 ) {
			CHAR_setWorkInt ( myindex , CHAR_WORKCHATROOMTYPE , 2 ) ; 
			CHAR_setWorkInt ( toindex , CHAR_WORKCHATROOMTYPE , 1 ) ; 
			CHAR_talkToCli ( toindex , -1 , "你现在是聊天室的室长！" , CHAR_COLORRED ) ; 
		
			ChatRoom[ Num ].Maker = toindex ; 			
		}
		else {
			CHAR_talkToCli ( myindex , -1 , "您选择的继任人物等级不足以担任室长！" , CHAR_COLORRED ) ; 
		}
	}
	ChatRoom_Refresh ( Num ) ; 
}

void ChatRoom_Leave ( int myindex )
{
	int i , NextMaker = -1 ; 
	int Num = CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM );
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return ;
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 1 ) {
		for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
			if ( ChatRoom[ Num ].MemberList[ i ] != myindex &&
				ChatRoom[ Num ].MemberList[ i ] != -1 ) {
				if ( CHAR_CHECKINDEX ( ChatRoom[ Num ].MemberList[ i ] ) ) {
					if ( CHAR_getInt ( ChatRoom[ Num ].MemberList[ i ] , CHAR_LV ) >= 30 || 
						CHAR_getInt ( ChatRoom[ Num ].MemberList[ i ] , CHAR_TRANSMIGRATION ) >= 1 ) {
						NextMaker = ChatRoom[ Num ].MemberList[ i ] ; 
						break;
					}
				}
			}
		}
		if ( NextMaker != -1 ) {
			ChatRoom_Make( myindex , NextMaker ) ; 
			CHAR_setWorkInt ( myindex , CHAR_WORKCHATROOMTYPE , 0) ; 
			CHAR_setWorkInt ( myindex , CHAR_WORKCHATROOMNUM, -1) ; 
			ChatRoom[ Num ].NowPeople --;
			for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
				if ( ChatRoom[ Num ].MemberList[ i ] == myindex ) {
					ChatRoom[ Num ].MemberList[ i ] = -1 ; 
					break;
				}
			}
		}else{
			ChatRoom_Destroy( myindex ) ; 
			return ; 
		}

	}else if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 2 ) {
		CHAR_setWorkInt ( myindex, CHAR_WORKCHATROOMTYPE, 0) ; 
		CHAR_setWorkInt ( myindex, CHAR_WORKCHATROOMNUM, -1) ; 
		ChatRoom[ Num ].NowPeople --;
		for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
			if ( ChatRoom[ Num ].MemberList[ i ] == myindex ) {
				ChatRoom[ Num ].MemberList[ i ] = -1 ; 
				break;
			}
		}
	}
	ChatRoom_Refresh ( Num ) ; 
}

void ChatRoom_Join ( int myindex , int num )
{
	int fd ;
	char buf[64];
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return ;
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) != 0 ) 
		return ; 
	if ( ChatRoom[ num ].useFlag == TRUE && ChatRoom[ num ].NowPeople < MAX_PPLINROOM ) {
		sprintf ( buf , "J|%s|%d" , CHAR_getChar( myindex , CHAR_NAME ) , myindex ) ; 
		fd = getfdFromCharaIndex( ChatRoom[ num ].Maker );
		lssproto_CHATROOM_send ( fd , buf );
	}else if ( ChatRoom[ num ].NowPeople >= MAX_PPLINROOM ) 
		CHAR_talkToCli ( myindex , -1 , "聊天室人数已满！" , CHAR_COLORRED ) ; 

}

void ChatRoom_Agree ( int myindex , int toindex , int YesNo ) {
	int i ; 
	int Num = CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMNUM );
	if ( !CHAR_CHECKINDEX ( myindex ) ) 
		return ;
	if ( !CHAR_CHECKINDEX ( toindex ) ) 
		return ;
	if ( CHAR_getWorkInt ( toindex , CHAR_WORKCHATROOMTYPE ) != 0 ) 
		return;
	if ( CHAR_getWorkInt ( myindex , CHAR_WORKCHATROOMTYPE ) == 1 ) {
		if ( ChatRoom[ Num ].NowPeople < MAX_PPLINROOM && YesNo == 1 ) {
			CHAR_setWorkInt ( toindex , CHAR_WORKCHATROOMTYPE , 2 ) ; 
			CHAR_setWorkInt ( toindex , CHAR_WORKCHATROOMNUM , Num ) ; 
			ChatRoom[ Num ].NowPeople ++ ; 
			for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
				if ( ChatRoom[ Num ].MemberList[ i ] == -1 ) {
					ChatRoom[ Num ].MemberList[ i ] = toindex ; 
					break;
				}
			}
		}else if ( YesNo == 0 ) {
			CHAR_talkToCli ( toindex , -1 , "您申请的聊天室室长拒绝您的加入！" , CHAR_COLORRED ) ; 
		}else if ( ChatRoom[ Num ].NowPeople >= MAX_PPLINROOM ) {
			CHAR_talkToCli ( toindex , -1 , "您申请的聊天室人数已满！" , CHAR_COLORRED ) ; 
			CHAR_talkToCli ( myindex , -1 , "聊天室人数已满！" , CHAR_COLORRED ) ; 
		}
	}
	ChatRoom_Refresh ( Num ) ; 
}

void ChatRoom_List ( int fd )
{
	int i ; 
	char buf[128];
	char token[2048] = "B|";
	for ( i = 0 ; i < MAX_CHATROOM ; i ++ ) {
		if ( ChatRoom[ i ].useFlag == TRUE ) {
			sprintf ( buf , "聊天室%2d资讯：室名=>%20s , 室长=>%16s , 人数=>%2d" , 
				i , 
				ChatRoom[ i ].RoomName , 
				CHAR_getChar ( ChatRoom[ i ].Maker , CHAR_NAME ) , 
				ChatRoom[ i ].NowPeople );

			sprintf ( buf , "r%d|%s|%d|%s|p%d|" , 
				i , 
				ChatRoom[ i ].RoomName , 
				ChatRoom[ i ].Maker , 
				CHAR_getChar ( ChatRoom[ i ].Maker , CHAR_NAME ) , 
				ChatRoom[ i ].NowPeople  );
			strncat ( token , buf , sizeof ( buf ) ) ; 
		}
	}
	lssproto_CHATROOM_send ( fd , token ) ; 
}

void ChatRoom_Refresh ( int Num )
{
	int fd , i , RoomLeader = -1 ; 
	char token[2048] ;
	char token2[2096] ;
	char buf[64] ; 
	if ( ChatRoom[ Num ].useFlag == TRUE ) {
		sprintf ( token , "R|r%d|%s|%d|%s|p%d|" , 
			Num , 
			ChatRoom[ Num ].RoomName , 
			ChatRoom[ Num ].Maker , 
			CHAR_getChar ( ChatRoom[ Num ].Maker , CHAR_NAME ) , 
			ChatRoom[ Num ].NowPeople  );
		for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
			if ( ChatRoom[ Num ].MemberList[ i ] != -1 ) {
				sprintf( buf , "I%d|%s|%s|" , 
					ChatRoom[ Num ].MemberList[ i ] , 
					CHAR_getChar ( ChatRoom[ Num ].MemberList[ i ] , CHAR_NAME ) , 
					CHAR_getChar ( ChatRoom[ Num ].MemberList[ i ] , CHAR_OWNTITLE )  
					);
				strncat ( token , buf , sizeof ( buf ) ) ; 
			}
		}
	}
	for ( i = 0 ; i < MAX_PPLINROOM ; i ++ ) {
		if ( ChatRoom[ Num ].MemberList[ i ] != -1 ) {
			if ( CHAR_getWorkInt ( ChatRoom[ Num ].MemberList[ i ] , CHAR_WORKCHATROOMTYPE ) == 1 ) RoomLeader = 1;
			else RoomLeader = 0 ;
			sprintf ( token2 , "%s%d|" , token , RoomLeader ) ; 
			fd= getfdFromCharaIndex ( ChatRoom[ Num ].MemberList[ i ] );
			lssproto_CHATROOM_send ( fd , token2 ) ; 
		}
	}
}

void ChatRoom_recvall ( int fd , char *data )
{ 
	char Head[4];
	char message[1024];
	char buf[16];
	int myindex ;

	myindex = CONNECT_getCharaindex(fd);
	if ( !CHAR_CHECKINDEX ( myindex ) )return ;
	getStringFromIndexWithDelim( data , "|", 1, Head, sizeof(Head));
	getStringFromIndexWithDelim( data , "|", 2, message, sizeof(message));

	if ( strcmp ( Head , "C" ) == 0 ) { // 成立频道
		if ( !ChatRoom_Create ( myindex , message ) )
			print("\nSyu log Create Channel Error" );
	}else if ( strcmp ( Head , "D" ) == 0 ) { // 删除频道
		if ( !ChatRoom_Destroy ( myindex ) )
			print("\nSyu log Destroy Channel Error" ) ; 
	}else if ( strcmp ( Head , "A" ) == 0 ) {// 同意加入频道
		getStringFromIndexWithDelim( data , "|", 2, message, sizeof(message));
		getStringFromIndexWithDelim( data , "|", 3, buf, sizeof(buf));
		ChatRoom_Agree ( myindex , atoi( message ) , atoi( buf ) ) ; 
	}else if ( strcmp ( Head , "J" ) == 0 ) {// 申请频道
		getStringFromIndexWithDelim( data , "|", 2, message, sizeof(message));
		ChatRoom_Join ( myindex , atoi( message ) ) ; 
	}else if ( strcmp ( Head , "L" ) == 0 ) {// 离开频道
		ChatRoom_Leave ( myindex ) ; 
	}else if ( strcmp ( Head , "K" ) == 0 ) {//踢出频道
		getStringFromIndexWithDelim( data , "|", 2, message, sizeof(message));
		ChatRoom_Kick ( myindex , atoi( message ) ); 
	}else if ( strcmp ( Head , "M" ) == 0 ) { // 更换室长
		getStringFromIndexWithDelim( data , "|", 2, message, sizeof(message));
		ChatRoom_Make ( myindex , atoi( message ) ); 
	}else if ( strcmp ( Head , "T" ) == 0 ) {// 频道讯息
		getStringFromIndexWithDelim( data , "|", 2, message, sizeof(message));
		ChatRoom_Message ( myindex , message ) ; 
	}else if ( strcmp ( Head , "B" ) == 0 ) {// 聊天室清单
		ChatRoom_List ( fd );
	}else 
		print("\nSyu log None");
}

#endif
#endif
