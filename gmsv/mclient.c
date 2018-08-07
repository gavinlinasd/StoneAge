#include <stdio.h>
#include <string.h>
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
#include "mclient.h"
#include "log.h"
#include "handletime.h"
#include "chatmagic.h"

#ifdef _RECAL_ASK_PLAYER			// WON 要求人物资料
#include "saacproto_cli.h"
#endif

#ifdef _M_SERVER

#define SEPARATOR ";"

#ifdef _GSERVER_RUNTIME
    BOOL gserver_runtime_starttime0_flag = TRUE;
    time_t gserver_runtime_starttime0;
#endif

//Mserver NETProc
int mproto_ClientDispatchMessage(int fd,char* data)
{
	int func,fieldcount;
	char raw[65500];

	strcpy(PersonalKey, MSPERSIONALKEY);
	util_DecodeMessage(raw,data);
	util_SplitMessage(raw,SEPARATOR);
	if (!util_GetFunctionFromSlice(&func, &fieldcount)) {
		logHack(fd,HACK_GETFUNCFAIL);
		return -1;
	}
	
	if (func==MPROTO_QUE_RECV) {
		int checksum=0, checksumrecv;
		int userfdid;
		char ans[65500],uid[4096];
		checksum += util_destring(2,uid);
		checksum += util_deint(3, &userfdid);
		checksum += util_destring(4, ans);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}
		mproto_Que_Recv(fd,uid,userfdid, ans);
		util_DiscardMessage();
		return 0;
	}

	if( func == MPROTO_MESSAGE_RECV )	{
		int checksum=0, checksumrecv;
		int userfdid;
		char ans[65500],uid[4096];
		checksum += util_destring(2,uid);
		checksum += util_deint(3, &userfdid);
		checksum += util_destring(4, ans);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}
		mproto_Message_Recv(fd,uid,userfdid, ans);
		util_DiscardMessage();
		return 0;
	}

	if (func==MPROTO_HELLO_RECV) {
    	util_DiscardMessage();
		return 0;
	}
	if (func == MPROTO_NOTALK_RECV) {
		int checksum=0, checksumrecv=0;
		int userfdid,ntime;
		char uid[4096];
		checksum = util_destring(2,uid);
		checksum += util_deint(3, &userfdid);
		checksum += util_deint(4, &ntime);
		util_deint(5, &checksumrecv);
		print("notalk here1 sum=%d,recv=%d\n",checksum,checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}
		print("notalk here2\n");
		mproto_NoTalk_Recv(fd,uid,userfdid, ntime);
		util_DiscardMessage();
		return 0;		
	}

#ifdef _GM_WARP_PLAYER				// WON 传送玩家
	if (func == MPROTO_WARP_RECV) {
		int checksum=0, checksumrecv=0;
		int userfdid,floor,x,y;
		char uid[4096];
		checksum = util_destring(2,uid);
		checksum += util_deint(3, &userfdid);
		checksum += util_deint(4, &floor);
		checksum += util_deint(5, &x);
		checksum += util_deint(6, &y);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}
		mproto_WARP_Recv(fd, uid, userfdid, floor, x, y);
		util_DiscardMessage();
		return 0;		
	}
#endif

#ifdef _RECAL_ASK_PLAYER			// WON 要求人物资料

	// 线上人物资料
	if (func == MPROTO_RECALL_PLAYER_RECV) {
		int checksum=0, checksumrecv=0, backup_flag=0;
		int userfdid, char_num, date, GmCliId;
		char uid[4096], id[20];

		checksum = util_destring(2,uid);
		checksum += util_deint(3, &userfdid);
		checksum += util_deint(4, &GmCliId);		
		checksum += util_destring(5,id);
		checksum += util_deint(6, &char_num);
		checksum += util_deint(7, &date);
		checksum += util_deint(8, &backup_flag);
		
		util_deint(9, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}

		mproto_RECALL_ASK_PLAYER_Recv(fd, uid, userfdid, GmCliId, id, char_num, date, backup_flag);
		util_DiscardMessage();
		return 0;		
	}
	// 备份人物资料
	if (func == MPROTO_RECALL_BACKUP_RECV) {
		int checksum=0, checksumrecv=0;
		int userfdid, char_num, date, GmCliId, backup_flag;
		char uid[4096], id[20], char_data[1024];

		checksum = util_destring(2, uid);
		checksum += util_deint(3, &userfdid);
		checksum += util_deint(4, &GmCliId);		
		checksum += util_destring(5,id);
		checksum += util_deint(6, &char_num);
		checksum += util_deint(7, &date);
		checksum += util_destring(8, char_data);
		checksum += util_deint(9, &backup_flag);
			
		util_deint(10, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}

		mproto_RECALL_BACKUP_PLAYER_Recv(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, backup_flag);
		
		util_DiscardMessage();
		return 0;		
	}

#endif

	if( func == MPROTO_WAEIKICK_RECV )	{
		int checksum=0, checksumrecv=0, userfdid;
		char uid[4096];
		checksum = util_destring(2,uid);
		checksum += util_deint(3, &userfdid);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}
		mproto_WaeiKick_Recv( fd, uid, userfdid);
		util_DiscardMessage();
		return 0;
	}

	if( func == MPROTO_JAIL_RECV )	{
		int checksum=0, checksumrecv=0, userfdid;
		char uid[4096];
		checksum = util_destring(2,uid);
		checksum += util_deint(3, &userfdid);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			return -1;
		}
		mproto_Jail_Recv( fd, uid, userfdid);
		util_DiscardMessage();
		return 0;
	}

	if (func == MPROTO_GMTALK_RECV) {
		int checksum=0,checksumrecv=0;
		int ntime;
		char uid[4096],data[65530];
		checksum = util_destring(2,uid);
		checksum += util_deint(3,&ntime);
		checksum += util_destring(4,data);
		util_deint(5,&checksumrecv);
                if (checksum!=checksumrecv) {
                        util_DiscardMessage();
                        logHack(fd, HACK_CHECKSUMERROR);
                        return -1;
                }
		mproto_GMTalk_Recv(fd,uid,ntime,data);
                util_DiscardMessage();
                return 0;
	}

#ifdef _RECAL_SERVER_OFF			// WON MSERVER 关闭 GS	
	if (func == MPROTO_RECALL_SERVER_OFF_RECV) {
		int checksum=0,checksumrecv=0;
		char msg[10];
		
		struct tm *newtime;
		time_t long_time;
		int MIN=0, HOUR=0, SEC=0;

		time( &long_time );
		newtime = localtime( &long_time ); 
	
		HOUR=newtime->tm_hour;
		MIN=newtime->tm_min;		
		SEC=newtime->tm_sec;
		
		checksum = util_destring(2,msg);
		util_deint(3,&checksumrecv);
        
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
            logHack(fd, HACK_CHECKSUMERROR);
            return -1;
        }
		
		// 关机
		if( strcmp(msg, "OFF") == 0){
			print("\n\n\n\n\n GM server shoutdown GS !!  (%d:%d:%d)\n\n\n\n\n", HOUR, MIN, SEC);
			system( "~/gmsvstop.sh" );
        }

		util_DiscardMessage();
		return 0;
	}
#endif

	util_DiscardMessage();
	return 0;
}
//GM回覆问题
void mproto_Que_Recv(int fd,char* uid,int userfdid,char* ans)
{
	int fd_charaindex;
	char sGM[256];
	int clifd=getfdFromFdid(userfdid);
	if  (clifd == -1) return ;
	if (ans == NULL || uid==NULL) return; 
    	sprintf(sGM,"GM[%s]为你解答如下:",uid);
    	fd_charaindex = CONNECT_getCharaindex( clifd );
	if (!CHAR_CHECKINDEX(fd_charaindex)) return;
	CHAR_talkToCli(fd_charaindex, -1, sGM, CHAR_COLORYELLOW);
	CHAR_talkToCli(fd_charaindex, -1, ans, CHAR_COLORWHITE);

	return;
}
//GM讯息
void mproto_Message_Recv(int fd,char* uid,int userfdid,char* ans)
{
	int fd_charaindex;
	char sGM[256];
	int clifd=getfdFromFdid(userfdid);
	if  (clifd == -1)	return ;
	if (ans == NULL || uid==NULL)	return; 
	if( !strcmp( uid, "无") )	{
		sprintf( sGM, "%s", ans);
	}else{
		sprintf( sGM, "GM[%s]讯息:%s", uid, ans);
	}

    fd_charaindex = CONNECT_getCharaindex( clifd );
	if (!CHAR_CHECKINDEX(fd_charaindex)) return;
	CHAR_talkToCli(fd_charaindex, -1, sGM, CHAR_COLORRED);
	return;
}

//GM禁言
void mproto_NoTalk_Recv(int fd,char* uid,int userfdid,int nTime)
{
	int fd_charaindex;
	int clifd=getfdFromFdid(userfdid);
	print("clifd =%d\n",clifd);
	if (clifd == -1) return ;
	if (uid == NULL ) return ;
	fd_charaindex=CONNECT_getCharaindex(clifd);
	if (!CHAR_CHECKINDEX(fd_charaindex)) return;
	CHAR_setInt( fd_charaindex,CHAR_SILENT, nTime );
	CHAR_setWorkInt( fd_charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec );
	CHAR_talkToCli( fd_charaindex, -1, "由於捣乱，被GM禁言10到30分钟", CHAR_COLORYELLOW);
	CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, 0 );
}


#ifdef _GM_WARP_PLAYER				// WON 传送玩家
void mproto_WARP_Recv(int fd,char* uid,int userfdid,int floor, int x, int y)
{
	int fd_charaindex;
	int clifd=getfdFromFdid(userfdid);
	print("clifd =%d\n",clifd);
	if (clifd == -1) return ;
	if (uid == NULL ) return ;
	fd_charaindex=CONNECT_getCharaindex(clifd);
	if (!CHAR_CHECKINDEX(fd_charaindex)) return;
	CHAR_warpToSpecificPoint(fd_charaindex, floor, x, y);

}
#endif

#ifdef _RECAL_ASK_PLAYER			// WON 要求人物资料
void mproto_RECALL_ASK_PLAYER_Recv(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, int backup_flag)
{
	extern int acfd;
	saacproto_Recal_Player_send(acfd, uid, userfdid, GmCliId, id, char_num, date, backup_flag);

}

void mproto_RECALL_BACKUP_PLAYER_Recv(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, char *char_data, int backup_flag)
{
	extern int acfd;
	saacproto_Recal_Backup_send(acfd, uid, userfdid, GmCliId, id, char_num, date, char_data, backup_flag);

}
#endif

//GM JAIL
void mproto_Jail_Recv(int fd,char* uid,int userfdid)
{
	int fd_charaindex;
	int clifd=getfdFromFdid(userfdid);
	if (clifd == -1) return;
	if (uid == NULL ) return;
	fd_charaindex = CONNECT_getCharaindex(clifd);
	if (!CHAR_CHECKINDEX(fd_charaindex))
		return;
	CHAR_talkToCli( fd_charaindex, -1, "由於捣乱，被GM关入监牢", CHAR_COLORYELLOW);
	CHAR_CHAT_DEBUG_jail( -1, CHAR_getChar( fd_charaindex, CHAR_CDKEY));
}

//GM踢人
void mproto_WaeiKick_Recv(int fd,char* uid,int userfdid)
{
	int fd_charaindex;
	int clifd=getfdFromFdid(userfdid);
	if (clifd == -1) return;
	if (uid == NULL ) return;
	fd_charaindex = CONNECT_getCharaindex(clifd);
	if (!CHAR_CHECKINDEX(fd_charaindex))
		return;
	CHAR_talkToCli( fd_charaindex, -1, "由於捣乱，被GM踢除", CHAR_COLORYELLOW);
	CHAR_CHAT_DEBUG_gmkick( -1, CHAR_getChar( fd_charaindex, CHAR_CDKEY));
}
//GM广播
void mproto_GMTalk_Recv(int fd,char* uid,int ntime,char* data)
{
    char    message[65530];
    int     i;
    int     playernum = CHAR_getPlayerMaxNum();
    if (uid == NULL || data == NULL ) return ;
    strcpy(message,"");
    sprintf(message,"GM[%s]:%s",uid,data);	
    for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
			CHAR_talkToCli( i, -1, message, CHAR_COLORWHITE);
        }
    }	
}

void mproto_Que_Send( int fd, int uid, int index, char* question)
{
	char buffer[65500], wheres[256];
	int checksum=0, floor, x, y;

	char *cdkey = CHAR_getChar( index, CHAR_CDKEY);
	char *charname = CHAR_getChar( index, CHAR_NAME);

	if( strlen( question) <= 0 ){
		CHAR_talkToCli( index, -1, "字串长度不足！", CHAR_COLORYELLOW);
		return;
	}
	memset( wheres, 0, sizeof( wheres));
	floor = CHAR_getInt( index, CHAR_FLOOR);
	x = CHAR_getInt( index, CHAR_X);
	y = CHAR_getInt( index, CHAR_Y);
	sprintf( wheres, "%d,%d,%d", floor, x, y);

	memset( buffer, 0, sizeof( buffer));
	strcpy(PersonalKey, MSPERSIONALKEY);
	checksum += util_mkint(buffer, uid);
	checksum += util_mkstring(buffer, cdkey);
	checksum += util_mkstring(buffer, charname);
	checksum += util_mkstring(buffer, question);
	checksum += util_mkstring(buffer, wheres);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, MPROTO_QUE_SEND, buffer);
}

#ifdef _RECAL_ASK_PLAYER			// WON 要求人物资料

// 线上人物资料
void saacproto_ACRecalPlayer_recv(char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, char *char_data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer, "");
	strcpy(PersonalKey, MSPERSIONALKEY);
	checksum += util_mkstring(buffer, uid);
	checksum += util_mkint(buffer, userfdid);
	checksum += util_mkint(buffer, GmCliId);
	checksum += util_mkstring(buffer, id);
	checksum += util_mkint(buffer, char_num);
	checksum += util_mkint(buffer, date);
	checksum += util_mkstring(buffer, char_data);

	util_mkint(buffer, checksum);
	util_SendMesg(mfd, MPROTO_RECALL_PLAYER_SEND, buffer);
}

// 备份人物资料
void saacproto_ACRecalBackupPlayer_recv(char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, char *char_data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer, "");
	strcpy(PersonalKey, MSPERSIONALKEY);
	checksum += util_mkstring(buffer, uid);
	checksum += util_mkint(buffer, userfdid);
	checksum += util_mkint(buffer, GmCliId);
	checksum += util_mkstring(buffer, id);
	checksum += util_mkint(buffer, char_num);
	checksum += util_mkint(buffer, date);
	checksum += util_mkstring(buffer, char_data);

	util_mkint(buffer, checksum);
	util_SendMesg(mfd, MPROTO_RECALL_BACKUP_SEND, buffer);
}

// 回溯ok
void saacproto_ACRecalAllBackupOK_recv(char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, char *char_data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer, "");
	strcpy(PersonalKey, MSPERSIONALKEY);
	checksum += util_mkstring(buffer, uid);
	checksum += util_mkint(buffer, userfdid);
	checksum += util_mkint(buffer, GmCliId);
	checksum += util_mkstring(buffer, id);
	checksum += util_mkint(buffer, char_num);
	checksum += util_mkint(buffer, date);
	checksum += util_mkstring(buffer, char_data);
	util_mkint(buffer, checksum);
	util_SendMesg(mfd, MPROTO_RECALL_BACKUP_OK_SEND, buffer);
}

// 取得备份日期
void saacproto_ACRecalBackupDate_recv(char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, char *char_data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer, "");
	strcpy(PersonalKey, MSPERSIONALKEY);
	checksum += util_mkstring(buffer, uid);
	checksum += util_mkint(buffer, userfdid);
	checksum += util_mkint(buffer, GmCliId);
	checksum += util_mkstring(buffer, id);
	checksum += util_mkint(buffer, char_num);
	checksum += util_mkint(buffer, date);
	checksum += util_mkstring(buffer, char_data);

	util_mkint(buffer, checksum);
	util_SendMesg(mfd, MPROTO_RECALL_BACKUP_DATE_SEND, buffer);
}
#endif


void mproto_Type_Send( int fd, int playernum, int itemuse)
{
	char buffer[65500];
	int checksum=0;

	memset( buffer, 0, sizeof( buffer));
	strcpy(PersonalKey, MSPERSIONALKEY);
	checksum += util_mkint(buffer, playernum);
	checksum += util_mkint(buffer, itemuse);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, MPROTO_TYPE_SEND, buffer);
}

int connectmServer(char* hostname,unsigned short port)
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

        memcpy((void*)&sock.sin_addr.s_addr ,
               hoste->h_addr , sizeof(struct in_addr) );
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
            print("connectGmsv fde\n");
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
                    print("connectGmsv return fd=%d\n",fd);
                    return fd;
            }else{
                    print("connectGmsv errorcode error\n");
                                            close(fd);
                    return -1;
            }
    }else{
            close(fd);
            return -1;
    }

}

#endif

#ifdef _RECAL_SEND_COUNT		// WON 传送GS资讯 
void recal_get_count()
{
	FILE *fp;
	int i, j;
	char temp[3][15], c;
	char buffer[256], msg[64];
	int checksum=0;
	
	if(mfd == -1) return;

	if( !(fp=fopen("recal_count.txt", "r")) ){
		print("\n open recal_count.txt err !!");
	}

	memset(temp, 0, sizeof(temp));

	i=0; j=0; 
	do{
       c = fgetc(fp);
	   if(c==' '){
			i=0; j++;
			continue;
	   }
       temp[j][i]=c;
       i++;
    }while(c != EOF);
	
	temp[2][i-2]=0;
	
	sprintf(msg,"%5s%4d%12s", temp[0], atoi(temp[1]), temp[2]);
	
	strcpy(buffer, "");
	strcpy(PersonalKey, MSPERSIONALKEY);
	checksum += util_mkstring(buffer, msg);

	util_mkint(buffer, checksum);
	util_SendMesg(mfd, MPROTO_RECALL_COUNT_SEND, buffer);

	fclose(fp);

}
#endif

#ifdef _GSERVER_RUNTIME //传送GSERVER执行多少时间给MSERVER
void gserver_runtime()
{
	int checksum=0;
	char buffer[1024];
    time_t starttime1;

	if(mfd == -1) return;
    if( gserver_runtime_starttime0_flag ){
	    gserver_runtime_starttime0_flag = FALSE;
		gserver_runtime_starttime0 = time(NULL);
	}
	print("\nChange->传送时间给mserver\n");
	strcpy(buffer, "");
	strcpy(PersonalKey, MSPERSIONALKEY);
    starttime1 = time(NULL);   
	checksum += util_mkint(buffer, (int)difftime(starttime1,gserver_runtime_starttime0));//传送的单位为秒
	util_mkint(buffer, checksum);
	util_SendMesg(mfd, MPROTO_RECALL_GSERVER_RUNTIME, buffer);
}
#endif
