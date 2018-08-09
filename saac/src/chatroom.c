#include "version.h"
#include "main.h"
#include "util.h"

#include "saacproto_serv.h"
#include "saacproto_util.h"

#ifdef _UNIVERSE_CHATROOM
#include "chatroom.h"

UniChatRoomlist UniChatRoom[MAX_CHATROOM];

void CHATROOM_resetChat( int chat)
{
	int i;
	if( chat<0 || chat>=MAX_CHATROOM ) return;
	UniChatRoom[chat].use=0;
	UniChatRoom[chat].masindex=-1;
	UniChatRoom[chat].charanum=0;
	for( i=0; i<MAX_PPLINROOM; i++){
		UniChatRoom[chat].charalist[i].use=0;
	}
}

void CHATROOM_Init( void)
{
	int i;
	for( i=0; i<MAX_CHATROOM; i++){
		CHATROOM_resetChat( i);
	}
}

int CHATROOM_getfreeChatRoom( void)
{
	int i;
	for( i=0; i<MAX_CHATROOM; i++){
		if( UniChatRoom[i].use == 0 ) return i;
	}
	return -1;
}

int CHATROOM_CreateChatRoom( int fd, char *cdkey, char *data,
							char *message, int userindex, int clifdid)
{
	int chat=-1;
	char buf1[256];
	if( (chat=CHATROOM_getfreeChatRoom()) == -1 ){
		sprintf( message, "频道已满！");
		return -1;
	}
	CHATROOM_resetChat( chat);

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	sprintf( UniChatRoom[chat].chatname, "%s", buf1);

	easyGetTokenFromBuf( data, '|', 3, buf1, sizeof(buf1) );
	sprintf( UniChatRoom[chat].charalist[0].cdkey, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 4, buf1, sizeof(buf1) );
	sprintf( UniChatRoom[chat].charalist[0].name, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 5, buf1, sizeof(buf1) );
	sprintf( UniChatRoom[chat].charalist[0].own, "%s", buf1);
	UniChatRoom[chat].charalist[0].use = 1;
	UniChatRoom[chat].charalist[0].fd = fd;
	UniChatRoom[chat].masindex = 0;
	UniChatRoom[chat].masfd = fd;
	UniChatRoom[chat].charanum = 1;
	UniChatRoom[chat].use = 1;
	CHATROOM_UpdateChatRoom( -1, chat);
	{
		char token[256];
		sprintf( token, "C|%s|%d|%s|%d|%s|%s|%s|",
				SUCCESSFUL,
				chat,
				UniChatRoom[chat].chatname,
				UniChatRoom[chat].masindex,
				UniChatRoom[chat].charalist[0].cdkey,
				UniChatRoom[chat].charalist[0].name,
				UniChatRoom[chat].charalist[0].own
				);
		saacproto_ACUniChatroom_send( fd, "", SUCCESSFUL, token, userindex, clifdid);
	}
	return chat;
}

void CHATROOM_DelChatRoom( char *Cdkey, char *data )
{
	int chat=-1, uti;
	char buf1[256];
	char cdkey[256], name[256];
extern gmsv gs[MAXCONNECTION];

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;

	easyGetTokenFromBuf( data, '|', 3, buf1, sizeof(buf1) );
	sprintf( cdkey, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 4, buf1, sizeof(buf1) );
	sprintf( name, "%s", buf1);
	
	uti = UniChatRoom[chat].masindex;
	if( uti<0 || uti >= MAX_PPLINROOM ) return;
	if( UniChatRoom[chat].charalist[uti].use == 0 ) return;
	if( !strcmp( UniChatRoom[chat].charalist[uti].cdkey, cdkey) &&
		!strcmp( UniChatRoom[chat].charalist[uti].name, name) ){
		int i;
		char token[256];
		CHATROOM_resetChat( chat);
		sprintf( token, "D|%d|", chat);
		for( i=0; i<MAXCONNECTION; i++) {
			if( !gs[i].use ) continue;
			saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
		}
	}
}

void CHATROOM_LeaveChatRoom( char *Cdkey, char *data )
{
	int chat=-1, uti, i;
	char buf1[256], token[256];

extern gmsv gs[MAXCONNECTION];

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;

	easyGetTokenFromBuf( data, '|', 3, buf1, sizeof(buf1) );
	uti = atoi( buf1);
	if( uti<0 || uti >= MAX_PPLINROOM ) return;
	if( UniChatRoom[chat].charalist[uti].use == 0 ) return;

	sprintf( token, "L|%d|%d|%s|", chat, uti, UniChatRoom[chat].charalist[uti].name );
	for( i=0; i<MAXCONNECTION; i++) {
		if( !gs[i].use ) continue;
		saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
	}
	UniChatRoom[chat].charalist[uti].use = 0;
	UniChatRoom[chat].charanum--;
	{
		int mti = UniChatRoom[chat].masindex;
		if( mti == uti ){ //室长离开
			for( i=0; i<MAX_PPLINROOM; i++){
				if( UniChatRoom[chat].charalist[i].use == 0 ) continue;
				UniChatRoom[chat].masindex = i;
				UniChatRoom[chat].masfd = UniChatRoom[chat].charalist[i].fd;
				sprintf( token, "M|%d|%d|", chat, i);
				break;
			}

			if( i>=MAX_PPLINROOM ){
				CHATROOM_resetChat( chat);
				sprintf( token, "D|%d|", chat);
				for( i=0; i<MAXCONNECTION; i++) {
					if( !gs[i].use ) continue;
					saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
				}
				return;
			}else{
				for( i=0; i<MAXCONNECTION; i++) {
					if( !gs[i].use ) continue;
					saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
				}
			}
		}
		CHATROOM_UpdateChatRoom( -1, chat);
	}
}

void CHATROOM_KickChatRoom( char *Cdkey, char *data )
{
	int chat=-1, uti, whoti;
	char buf1[256];
	char cdkey[256],name[256];
extern gmsv gs[MAXCONNECTION];

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;

	easyGetTokenFromBuf( data, '|', 3, buf1, sizeof(buf1) );
	sprintf( cdkey, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 4, buf1, sizeof(buf1) );
	sprintf( name, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 5, buf1, sizeof(buf1) );
	whoti = atoi( buf1);

	uti = UniChatRoom[chat].masindex;
	if( uti<0 || uti >= MAX_PPLINROOM ) return;
	if( UniChatRoom[chat].charalist[uti].use == 0 ) return;
	if( whoti<0 || whoti >= MAX_PPLINROOM ) return;
	if( UniChatRoom[chat].charalist[whoti].use == 0 ) return;

	if( !strcmp( UniChatRoom[chat].charalist[uti].cdkey, cdkey) &&
		!strcmp( UniChatRoom[chat].charalist[uti].name, name) ){
		int i;
		char token[256];

		sprintf( token, "K|%d|%d|", chat, whoti);

		for( i=0; i<MAXCONNECTION; i++) {
			if( !gs[i].use ) continue;
			saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
		}
		UniChatRoom[chat].charalist[whoti].use = 0;
		UniChatRoom[chat].charanum--;
	}
	CHATROOM_UpdateChatRoom( -1, chat);
}

void CHATROOM_JoinChatRoom( char *Cdkey, char *data )
{
	int chat=-1, uti;
	char buf1[256];
extern gmsv gs[MAXCONNECTION];
	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;

	uti = UniChatRoom[chat].masindex;
	if( uti<0 || uti >= MAX_PPLINROOM ) return;
	if( UniChatRoom[chat].charalist[uti].use == 0 ) return;
	if( !gs[ UniChatRoom[chat].masfd].use ) return;
	saacproto_ACUniChatroom_send( UniChatRoom[chat].masfd, "", SUCCESSFUL, data, -1, -1);
	UniChatRoom[chat].charanum++;

}

void CHATROOM_AgreeJoinCR( int fd, char *data, int userindex, int clifd)
{
	int chat=-1, ti, i;
	char buf1[256], cdkey[256], name[256], own[256];
	char token[256];
extern gmsv gs[MAXCONNECTION];

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;
	easyGetTokenFromBuf( data, '|', 3, buf1, sizeof(buf1) );
	sprintf( cdkey, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 4, buf1, sizeof(buf1) );
	sprintf( name, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 5, buf1, sizeof(buf1) );
	sprintf( own, "%s", buf1);

	if( (ti=CHATROOM_AgreeSomeOne( chat, cdkey, name, own, fd)) < 0 ){
		sprintf( token, "A|%d|FULL|", chat);
		saacproto_ACUniChatroom_send( fd, "", SUCCESSFUL, token, userindex, clifd);
	}else{
		sprintf( token, "A|%d|OK|", chat);
		saacproto_ACUniChatroom_send( fd, "", SUCCESSFUL, token, userindex, clifd);
		UniChatRoom[chat].charanum++;
	}
	CHATROOM_UpdateChatRoom( -1, chat);


}

void CHATROOM_GSClosedHandle( int fd)
{
	int i, j, mindex;	
extern gmsv gs[MAXCONNECTION];
	//andy_log
	log( "\nCHATROOM_GSClosedHandle( %d)\n", fd);

	for( j=0; j<MAX_CHATROOM; j++){
		if( UniChatRoom[j].use == 0 ) continue;
		UniChatRoom[j].charanum=0;
		for( i=0; i<MAX_PPLINROOM; i++){
			if( UniChatRoom[j].charalist[i].use == 0 ) continue;
			if( UniChatRoom[j].charalist[i].fd == fd ){
				UniChatRoom[j].charalist[i].use = 0;
			}else{
				UniChatRoom[j].charanum++;
			}
		}

		mindex = UniChatRoom[j].masindex;
		if( mindex < 0 || mindex >= MAX_PPLINROOM ||
			UniChatRoom[j].charalist[mindex].use == 0 ){
			char token[512];

			CHATROOM_resetChat( j);
			sprintf( token, "D|%d|", j);
			for( i=0; i<MAXCONNECTION; i++) {
				if( !gs[i].use ) continue;
				saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
			}
			continue;
		}
		CHATROOM_UpdateChatRoom( -1, j);
	}
}

int CHATROOM_AgreeSomeOne( int chat, char *cdkey, char *name, char *own, int whofd)
{
	int i;
	if( chat<0 || chat>=MAX_CHATROOM ) return -1;
	if( UniChatRoom[chat].use == 0 ) return -1;
	for( i=0; i<MAX_PPLINROOM; i++){
		if( UniChatRoom[chat].charalist[i].use == 1 ) continue;
		sprintf( UniChatRoom[chat].charalist[i].cdkey, "%s", cdkey);
		sprintf( UniChatRoom[chat].charalist[i].name, "%s", name);
		sprintf( UniChatRoom[chat].charalist[i].own, "%s", own);
		UniChatRoom[chat].charalist[i].fd = whofd;
		UniChatRoom[chat].charalist[i].use = 1;
		return i;
	}
	return -2;
}

void CHATROOM_AgreeChatRoom( char *Cdkey, char *data )
{
	int chat=-1, uti, whofd;
	char buf1[256];
	char cdkey[256],name[256], own[256], result[256];

extern gmsv gs[MAXCONNECTION];

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;

	easyGetTokenFromBuf( data, '|', 3, buf1, sizeof(buf1) );
	sprintf( cdkey, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 4, buf1, sizeof(buf1) );
	sprintf( name, "%s", buf1);

	uti = UniChatRoom[chat].masindex;
	if( uti<0 || uti >= MAX_PPLINROOM ) return;
	if( UniChatRoom[chat].charalist[uti].use == 0 ) return;

	if( !strcmp( UniChatRoom[chat].charalist[uti].cdkey, cdkey) &&
		!strcmp( UniChatRoom[chat].charalist[uti].name, name) ){
		char token[256];

		easyGetTokenFromBuf( data, '|', 5, buf1, sizeof(buf1) );
		sprintf( result, "%s", buf1);

		easyGetTokenFromBuf( data, '|', 6, buf1, sizeof(buf1) );
		sprintf( cdkey, "%s", buf1);
		easyGetTokenFromBuf( data, '|', 7, buf1, sizeof(buf1) );
		sprintf( name, "%s", buf1);
		easyGetTokenFromBuf( data, '|', 8, buf1, sizeof(buf1) );
		sprintf( own, "%s", buf1);
		easyGetTokenFromBuf( data, '|', 9, buf1, sizeof(buf1) );
		whofd = atoi( buf1);
		if( !gs[ whofd].use ) return;

		sprintf( token, "A|%d|%s|%s|%s|%s|", chat, result, cdkey, name, own );
		if( !strcmp( result, "OK") ){
//			UniChatRoom[chat].charanum++;
			if( CHATROOM_AgreeSomeOne( chat, cdkey, name, own, whofd) < 0 ){
				sprintf( token, "A|%d|FULL|%s|%s|%s|", chat, cdkey, name, own );
			}
		}else{
			UniChatRoom[chat].charanum--;
		}
		saacproto_ACUniChatroom_send( whofd, "", SUCCESSFUL, token, -1, -1);
	}
}

void CHATROOM_MasterChatRoom( char *Cdkey, char *data )
{
	int chat=-1, uti;
	char buf1[256];
	char cdkey[256],name[256];

extern gmsv gs[MAXCONNECTION];

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;
	if( UniChatRoom[chat].use == 0 ) return;
	easyGetTokenFromBuf( data, '|', 3, buf1, sizeof(buf1) );
	sprintf( cdkey, "%s", buf1);
	easyGetTokenFromBuf( data, '|', 4, buf1, sizeof(buf1) );
	sprintf( name, "%s", buf1);

	uti = UniChatRoom[chat].masindex;
	if( uti<0 || uti >= MAX_PPLINROOM ) return;
	if( UniChatRoom[chat].charalist[uti].use == 0 ) return;

	if( !strcmp( UniChatRoom[chat].charalist[uti].cdkey, cdkey) &&
		!strcmp( UniChatRoom[chat].charalist[uti].name, name) ){
		int i,ti;
		char token[256];

		easyGetTokenFromBuf( data, '|', 5, buf1, sizeof(buf1) );
		ti = atoi( buf1);
		if( UniChatRoom[chat].charalist[ti].use == 0 ) return;
		UniChatRoom[chat].masindex = ti;
		UniChatRoom[chat].masfd = UniChatRoom[chat].charalist[ti].fd;
		sprintf( token, "M|%d|%d|", chat, ti );

		for( i=0; i<MAXCONNECTION; i++) {
			if( !gs[i].use ) continue;
			saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
		}
		return;
	}
}

void CHATROOM_MessageChatRoom( char *Cdkey, char *data )
{
	int chat=-1, i;
	char buf1[256];

extern gmsv gs[MAXCONNECTION];

	easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
	chat = atoi( buf1);
	if( chat<0 || chat>=MAX_CHATROOM ) return;
	
	for( i=0; i<MAXCONNECTION; i++) {
		if( !gs[i].use ) continue;
		saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, data, -1, -1);
	}
}

int CHATROOM_UpdateChatRoom( int fd, int chat)
{
	char token[4096], buf[256];
	int i;
extern gmsv gs[MAXCONNECTION];

	if( chat<0 || chat>=MAX_CHATROOM ) return 0;
//	if( UniChatRoom[chat].use == 0 ) return 0;

	sprintf( token, "U|%d|%d|%s|%d|%d|",
		chat,
		UniChatRoom[chat].use,
		UniChatRoom[chat].chatname,
		UniChatRoom[chat].masindex,
		UniChatRoom[chat].charanum
	);

		
	for( i=0; i<MAX_PPLINROOM; i++ ){
		if( UniChatRoom[chat].charalist[i].use == 0 ) continue;
		sprintf( buf, ",%d|%s|%s|%s",
			i,
			UniChatRoom[chat].charalist[i].cdkey,
			UniChatRoom[chat].charalist[i].name,
			UniChatRoom[chat].charalist[i].own
			);
		strcat( token, buf);
	}
	//更新.
	if( fd == -1 ){
		for( i=0; i<MAXCONNECTION; i++) {
			if( !gs[i].use ) continue;
			saacproto_ACUniChatroom_send( i, "", SUCCESSFUL, token, -1, -1);
		}
	}else{
		saacproto_ACUniChatroom_send( fd, "", SUCCESSFUL, token, -1, -1);
	}
	return chat;
}

void CHATROOM_RecvAll( int fd, char *cdkey, char *data, int userindex, int clifdid )
{
	char token[1024];
	char comm[256], buf1[256];
	int i;

	//andy_log
	log( "_RecvAll:%s\n", data);

	easyGetTokenFromBuf( data, '|', 1, comm, sizeof(comm) );

	if( !strcmp( comm, "C") ){//建立频道
		if( CHATROOM_CreateChatRoom( fd, cdkey, data, token, userindex, clifdid) >= 0 ){
		}else{
			saacproto_ACUniChatroom_send( fd, cdkey, FAILED, data, userindex, clifdid );
		}
	}else if( !strcmp( comm, "D") ) {//删除频道
		CHATROOM_DelChatRoom( cdkey, data);
	}else if( !strcmp( comm, "L") ) {//离开频道
		CHATROOM_LeaveChatRoom( cdkey, data);
	}else if( !strcmp( comm, "T") ) {//频道讯息
		CHATROOM_MessageChatRoom( cdkey, data);
	}else if( !strcmp( comm, "K") ) {//踢出频道
		CHATROOM_KickChatRoom( cdkey, data);
	}else if( !strcmp( comm, "J") ) {//申请加入频道
		CHATROOM_AgreeJoinCR( fd, data, userindex, clifdid);
		//CHATROOM_JoinChatRoom( cdkey, data);
	}else if( !strcmp( comm, "A") ) {//同意加入
		CHATROOM_AgreeChatRoom( cdkey, data);
	}else if ( strcmp ( comm , "M" ) == 0 ) { // 更换室长
		CHATROOM_MasterChatRoom( cdkey, data);
	}else if( !strcmp( comm, "U") ) {//更新频道
		easyGetTokenFromBuf( data, '|', 2, buf1, sizeof(buf1) );
		if( atoi( buf1) == -1 ){
			for( i=0; i<MAX_CHATROOM; i++){
				CHATROOM_UpdateChatRoom( fd, i);
			}
		}else{
			CHATROOM_UpdateChatRoom( fd, atoi( buf1));
		}
	}else{
		
	}
}
#endif
