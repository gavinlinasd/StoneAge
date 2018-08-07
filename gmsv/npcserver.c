#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include "autil.h"
#include "util.h"
#include "net.h"
#include "char.h"
#include "char_base.h"
#include "log.h"
#include "handletime.h"
#include "npcserver.h"
#include "lssproto_serv.h"
//#include "chatmagic.h"
#include "lssproto_util.h"

#ifdef _NPCSERVER_NEW
#include "npcshandle.h"
#define SEPARATOR ";"

NPCMessC *NPCMess;
static int AskListOK = 0;
//Mserver NETProc
int NSproto_DispatchMessage(int fd,char* encoded)
{
	char funcname[1024];
	if( NPCMESS_setChar( encoded) == FALSE )
		return -1;
	memset( funcname, 0, sizeof( funcname));
	if( NPCMESS_getChar( funcname, sizeof( funcname)) == FALSE )	{
		return 0;
	}
	if( !strcmp( funcname , "TEST" )){
		char buf1[1024],buf2[1024],buf3[1024];
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		if( NPCMESS_getChar( buf2, sizeof( buf2)) == FALSE ) return 0;
		if( NPCMESS_getChar( buf3, sizeof( buf3)) == FALSE ) return 0;
		return 0;
	}else if( !strcmp( funcname , "NPC_LOGIN" )){
		char buf1[256];
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		NPCS_NpcSLogin_recv( fd, buf1);
	}else if( !strcmp( funcname , "NPC_ASKLIST" )){
		char buf1[4096];
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		NPCS_AskNpcList_recv( fd, buf1);
		return 0;
	}else if( !strcmp( funcname , "NPCS_CREATE" )){
		int dir, floor, x, y, image, npcsindex;
		char Name[256], buf1[256];
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		npcsindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		memcpy( Name, buf1, strlen( buf1)+1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		dir = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		image = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		floor = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		x = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		y = atoi( buf1);

		if( NPCSERVER_CreateObjindexFromServer( fd, npcsindex, Name, image, dir, floor, x, y) == FALSE ){
		}
		return 0;
	}else if( !strcmp( funcname , "NPC_TALKMESS" )){
		char TalkMess[1024], buf1[1024];
		int npcobjindex, charaindex, charobjindex, Color;
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		npcobjindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		charaindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		charobjindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		memcpy( TalkMess, buf1, strlen( buf1)+1 );
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		Color = atoi( buf1);
		NPCS_NpcTalkMess_recv( npcobjindex, charaindex, charobjindex, TalkMess, Color);
	}else if( !strcmp( funcname , "NPC_WINMESS") ){
		int charaindex, charobjindex, npcobjindex;
		int seqno, windowtype, buttontype, page;
		char Token[1024], buf1[1024];

		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		npcobjindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		charaindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		charobjindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		memcpy( Token, buf1, strlen( buf1)+1);
		Token[ strlen( Token)+1] = 0;
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		seqno = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		windowtype = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		buttontype = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		page = atoi( buf1);
		NPCS_NpcWinMess_recv( npcobjindex, charaindex, charobjindex, Token,
						  seqno, windowtype, buttontype, page);
	}else if( !strcmp( funcname , "NPC_CHECKFREE") ){
		int charaindex, charobjindex, npcobjindex;
		char buf1[4096], freeMess1[4096];
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		npcobjindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		charaindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		charobjindex = atoi( buf1);
		if( NPCMESS_getChar( buf1, sizeof( buf1)) == FALSE ) return 0;
		memcpy( freeMess1, buf1, strlen( buf1)+1);

		NPCS_NpcCheckFreeMess_recv( npcobjindex, charaindex, charobjindex, freeMess1);
	}else{

	}
	return 0;
}

void NPCS_NpcCheckFreeMess_recv( int npcobjindex, int charaindex, int charobjindex,
								char *CheckfreeMess)
{
	if( NPCS_HandleCheckFreeMess( npcobjindex, charaindex, charobjindex,
			CheckfreeMess) == TRUE ){
	}else {
	}
}

void NPCS_NpcWinMess_recv( int npcobjindex, int charaindex, int charobjindex, char *WinMess,
						  int seqno, int windowtype, int buttontype, int page)
{
	int fd = getfdFromCharaIndex( charaindex);
	if( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX) != charobjindex )
		return;
	CHAR_setWorkInt( charaindex, CHAR_WORKSHOPRELEVANT, page);
	lssproto_WN_send( fd, windowtype, buttontype, seqno, npcobjindex, WinMess);
}

void NPCS_NpcWinMess_send( int npcobjindex, int npcindex, int charaindex, char *WinMess,
						  int seqno, int select)
{
	char buf[4096];
	int charobjindex, page;
	memset( buf, 0, sizeof( buf));
	charobjindex = CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX);
	page = CHAR_getWorkInt( charaindex, CHAR_WORKSHOPRELEVANT);
	sprintf( buf, "NPC_WINMESS %d %d %d %d %s %d %d %d\n", npcindex, npcobjindex, charaindex, charobjindex,
			WinMess, seqno, select, page);
	lssproto_Send( npcfd, buf);
}

void NPCS_AskNpcTalk_send( int objindex, int npcsindex, int charaindex, char *Nlist)
{//NPCS_TALK
	char buf[4096];
	int charobjindex;
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	memset( buf, 0, sizeof( buf));
	charobjindex = CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX);
	sprintf( buf, "NPCS_TALK %d %d %d %d %s\n", npcsindex, objindex, charaindex, charobjindex, Nlist);
	lssproto_Send( npcfd, buf);
}

void NPCS_AskNpcList_recv( int fd, char *Nlist)
{
	char Npclist[256];
	int i=1, createnpc = 0;
	while( getStringFromIndexWithDelim( Nlist, "|", i, Npclist,sizeof(Npclist) ) != FALSE ){
		char buf1[256], NpcName[256];
		int npcsindex, dir, floor, x, y, image;
		i++;
		if( getStringFromIndexWithDelim( Npclist, ",", 1, buf1, sizeof( buf1)) == FALSE ) continue;
		npcsindex = atoi( buf1);
		if( getStringFromIndexWithDelim( Npclist, ",", 2, buf1, sizeof( buf1)) == FALSE ) continue;
		memcpy( NpcName, buf1, strlen( buf1)+1);
		if( getStringFromIndexWithDelim( Npclist, ",", 3, buf1, sizeof( buf1)) == FALSE ) continue;
		dir = atoi( buf1);
		if( getStringFromIndexWithDelim( Npclist, ",", 4, buf1, sizeof( buf1)) == FALSE ) continue;
		image = atoi( buf1);
		if( getStringFromIndexWithDelim( Npclist, ",", 5, buf1, sizeof( buf1)) == FALSE ) continue;
		floor = atoi( buf1);
		if( getStringFromIndexWithDelim( Npclist, ",", 6, buf1, sizeof( buf1)) == FALSE ) continue;
		x = atoi( buf1);
		if( getStringFromIndexWithDelim( Npclist, ",", 7, buf1, sizeof( buf1)) == FALSE ) continue;
		y = atoi( buf1);
		NpcName[ strlen( NpcName)+1] = 0;
		if( NPCSERVER_CreateObjindexFromServer( fd, npcsindex, NpcName, image, dir, floor, x, y) == FALSE ){
		}else{
			createnpc++;
		}
	}
}

void NPCS_AskNpcList_send( int fd)
{
	char buf[4096];
	memset( buf, 0, sizeof( buf));
	sprintf( buf, "NPC_ASKLIST\n");
	lssproto_Send(fd, buf);
}

void NPCS_NpcSLogin_recv( int fd, char *Mess)
{
	if( !strcmp( Mess, "OK") ){
		if( AskListOK == 0 ){
			NPCS_AskNpcList_send( fd);
			AskListOK = 1;
		}
	}else {
	}
}

void NPCS_NpcSLogin_send( int fd)
{
	char buf[4096];
	memset( buf, 0, sizeof( buf));
	sprintf( buf, "NPC_LOGIN ABC\n");
	lssproto_Send(fd, buf);
}

void NPCS_NpcTalkMess_recv( int npcobjindex, int charaindex, int charobjindex, char *TalkMess, int Color)
{
	NPCS_NpcstalkToCli( charaindex, npcobjindex, TalkMess, Color);
}

int connectNpcServer(char* hostname,unsigned short port)
{
	struct sockaddr_in sock;
    struct hostent* hoste;
    int     fd,lr;
    fd_set  fdset,fde;
    struct timeval tmv;

    memset( &sock ,  0 , sizeof( struct sockaddr_in ) );
    sock.sin_family = AF_INET;
    sock.sin_port = htons( port );
    sock.sin_addr.s_addr = inet_addr( hostname );

    if( sock.sin_addr.s_addr == -1 ){
        hoste = gethostbyname( hostname );
        if( hoste == NULL ){
            print( "获取主机名: %s\n", hostname);
            return -1;
        }
        memcpy((void*)&sock.sin_addr.s_addr , hoste->h_addr , sizeof(struct in_addr) );
    }

    fd = socket( AF_INET, SOCK_STREAM , 0 );
    if( fd == -1 ){
        print("Cannot Create Socket( errno:%d)\n",errno );
        return -1;
    }
    //set the no block
	if ( fcntl(fd,F_SETFL,O_NONBLOCK) == -1 ){
		print("set noblock error!\n");
		return -1;
	}
	lr = connect(fd,(struct sockaddr*)&sock,sizeof(struct sockaddr_in));
    if( lr != 0 ){
        if ( errno != EINPROGRESS ){
            print("Cannot connect. (errno:%d)\n",errno );
            close(fd);
            return -1;
        }
    }
    FD_ZERO(&fdset);
    FD_SET(fd,&fdset);
    FD_SET(fd,&fde);
    tmv.tv_sec =3;
    tmv.tv_usec = 0;
    lr=select(fd+1,NULL,&fdset,&fde,&tmv);
    if (FD_ISSET(fd,&fde)){
            print("connectNPCS fde\n");
            close(fd);   //add more
            return -1;
    }
    if ( lr > 0 && FD_ISSET(fd,&fdset)){
            int errorcode;
            int errorcodelen;
            errorcode=1;
            errorcodelen=sizeof(errorcode);
            getsockopt(fd,SOL_SOCKET,SO_ERROR,&errorcode,&errorcodelen);
            if (errorcode == 0 ){
                    fcntl(fd,F_SETFL,0);
                    print("connectNPCS return fd=%d\n",fd);
                    return fd;
            }else{
                    print("connectNPCS errorcode error\n");
                                            close(fd);
                    return -1;
            }
    }else{
            close(fd);
            return -1;
    }

}

void NPCS_SendProbe( int fd)
{
	char buf[65535];
	int i;

	memset( buf, 0, sizeof( buf));

	sprintf( buf, "TEST %d,%d,%s,%s", 99999, 99999,
		"1234567891012345678901234567890", "1234567891012345678901234567890" );

	strcat( buf, " ");
	for( i=0; i<5; i++)	{
		if( i==0 ) strcat( buf, "99999");
		else strcat( buf, ",99999");
	}
	strcat( buf, " ");
	for( i=0; i<15; i++)	{
		if( i==0 ) strcat( buf, "99999");
		strcat( buf, ",99999");
	}

	strcat( buf, "\n");
	lssproto_Send(fd, buf);
}

BOOL NPCMESS_setChar( char *buf)
{
	char Mbuf[4096];
	int i=0,j=0;
	int len=0;
	NPCMessC *Works;
	NPCMessC *point;
	point = NULL;

	memset( Mbuf, 0, sizeof( Mbuf));
	sprintf( Mbuf, "%s", buf);
	NPCMess = NULL;
	while( Mbuf[i] != '\0' )	{
		if( Mbuf[i] == '\n' )
			Mbuf[i] = '\0';
		i++;
	}
	i=0;
	len = 0;
	while( Mbuf[i] != '\0' && Mbuf[j] != '\0' )	{
		j=i;
		len = 0;
		while( Mbuf[j] != ' ' && Mbuf[j] != '\0')	{
			len++;
			j++;
		}
		Works = MESS_getNew();
		if( Works == NULL )
			return FALSE;
		memcpy( Works->buf, &Mbuf[i], len);
		Works->len = len;
		if( NPCMess == NULL )	{
			NPCMess = Works;
			point = Works;
		}else	{
			point->next = Works;
			point = point->next;
		}
		i = (j+1);
	}
	return TRUE;
}


BOOL NPCMESS_getChar( char *buf, int len)
{
	NPCMessC *Works;
	Works = NPCMess;

	if( Works == NULL )
		return FALSE;
	memcpy( buf, Works->buf, len);
	buf[ strlen( buf)+1] = 0;
	NPCMess = Works->next;
	free( Works);
	return TRUE;
}

NPCMessC *MESS_getNew()
{
	NPCMessC *MBUF=NULL;
	MBUF = (NPCMessC *) calloc( 1, sizeof( struct _NPCMessCommand) );
	if( MBUF == NULL )
		return NULL;
	memset( MBUF->buf, 0, sizeof( MBUF->buf));
	MBUF->len = 0;
	MBUF->next = NULL;
	return MBUF;
}
#endif
