#define _RECV_C_

#include "version.h"
#include "main.h"
#include "saacproto_util.h"
#include "saacproto_serv.h"
#include "saacproto_lserver.h"
#include "saacproto_work.h"
#include "recv.h"
#include "char.h"
#include "db.h"
#include "mail.h"
#include "lock.h"
#include "util.h"

#ifdef _SASQL1
#include "sasql.h"
#endif

// CoolFish: Family 2001/5/9
#include "acfamily.h"
#include "version.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef _PAUCTION_MAN
#include "auction.h"
#endif
#ifdef _DEATH_CONTEND
#include "deathcontend.h"
#endif
#include "defend.h"

#ifdef _UNIVERSE_CHATROOM
#include "chatroom.h"
#endif

#ifdef _RECAL_BACKUP_PLAYER			// WON 回溯功能
#include <dirent.h>
#include <sys/stat.h>

#define MAX_HAVE_ITEM	20
#define MAX_POOL_ITEM	30
#define MAX_HAVE_PET	5
#define MAX_POOL_PET	20

static int char_data_count=0;


#endif



#ifdef _CK_ONLINE_PLAYER_COUNT    // WON ADD 计算线上人数
struct play_count
{
	int playcount;
};
struct play_count online_player[MAXCONNECTION];
#endif



char retdata[CHARDATASIZE];     
char result[CHARDATASIZE];        

// Spock +1 2000/11/1
#define MAX_PROCESS 16777216

void saacproto_ACServerLogin_recv( int ti,char* servername ,
                                   char* serverpas )
{
    login_game_server( ti ,
             servername , serverpas ,
             result , sizeof( result ) ,
             retdata , sizeof( retdata ));

    saacproto_ACServerLogin_send( ti ,result ,retdata );
}
/*
   必□丞扔□田□互失市它件玄扔□田□井日夫弘失它玄允月［
   int fd : 覆擂socket

*/
void  saacproto_ACServerLogout_recv( int ti  )
{

    logout_game_server( ti );
}

void saacproto_ACCharLoad_recv( int ti,char* id,char* pas,char* charname ,
                                int lock,char* opt , int mesgid)
{
    // Spock 2000/10/31
    static int process = 0;
    char buf[16];
    // Spock end
#ifdef _NewSave
    int charindex = -1;
#endif
 

if (!sasql_query(id,pas)){
		if(!sasql_register(id,pas)){
	    log("密码错误或没有注册\n");
	    saacproto_ACCharList_send( ti , FAILED , "Password error" , mesgid);
	    return;}}


    if( !is_game_server_login( ti ) ){
#ifdef _NewSave
        saacproto_ACCharLoad_send( ti , FAILED , "not login" , mesgid, charindex );
#else
        saacproto_ACCharLoad_send( ti , FAILED , "not login" , mesgid );
#endif
        return;
    }

#ifdef _CHANGEGALAXY
	if( isChangeGalaxyLocked( id) ) {
		log(" 因星系移民被锁 ");
		saacproto_ACCharLoad_send( ti , FAILED , "CHANGE_GALAXY" , mesgid, charindex );
		return;
	}
#endif


#ifdef _UNLOCKPOOL // 登入时检查是否已在UnlockPool中
	if( inUnlockPool(id) ) {
		log(" 已经在UNLOCKPOOL中 ");
		saacproto_ACCharLoad_send( ti , FAILED ,
                                   "server load too high" , mesgid , charindex);
		return;
	}
#endif

    process++;
    if ( process > MAX_PROCESS ) process = 1;
    snprintf( buf , sizeof(buf) , "%d" , process );

    if( addWorkerTask( id, pas , "2", charLoadCallback , ti , 
                       id , pas , charname , buf , "" , lock, mesgid) < 0 ){


#ifdef _NewSave
        saacproto_ACCharLoad_send( ti , FAILED ,
                                   "server load too high" , mesgid , charindex);
#else
        saacproto_ACCharLoad_send( ti , FAILED ,
                                   "server load too high" , mesgid );
#endif
        return;
    }

    // Spock 2000/10/31
    //log( "读取档案: 附加作业 %s %s, 进程=%d\n", id,pas,process );
    // Spock end
}

#ifdef _RELOGIN
void saacproto_ACCharLoadII_recv( int ti,char* id,char* pas,char* charname ,
                                int lock,char* opt , int mesgid,int gs1, int gs2)
{
    char buf[16];
    int charindex = -1;
   
    if( !is_game_server_login( ti ) ){
        saacproto_ACCharLoad_send( ti , FAILED , "not login" , mesgid, charindex );
        return;
    }
	// 检查Buffer中的资料是否符合
	// 检查原先从Wgs传回CharLoad做了什麽事
	// 更新名片资料
}
#endif

/*
   平乓仿忡绣

   char *id : ID
   char *pas : 天允歹□升
   char *charname : 平乓仿  
   char *opt : 左皿扑亦件  侬  ［ CharList及午五卞丢乒午仄化忒允树  
   char *charinfo : 白央奶伙卞忡绣允月平乓仿弁正及旦  □正旦树  ［
   int id : 丢永本□斥id
*/
#ifdef _NewSave
void saacproto_ACCharSave_recv( int ti, char* id,
                                char *charname,char* opt ,
                                char* charinfo , int unlock , int mesgid , int charindex )
#else
void saacproto_ACCharSave_recv( int ti, char* id,
                                char *charname,char* opt ,
                                char* charinfo , int unlock , int mesgid )
#endif
{
    char process[16];
    int ret;
    if( !is_game_server_login(ti) ){
        saacproto_ACCharSave_send( ti , FAILED , "not login" , mesgid );
        return;
    }
#ifdef _NewSave
    ret = charSave( ti, id, charname, opt, charinfo,
                      unlock,mesgid, charindex );
#else
    ret = charSave( ti, id, charname, opt, charinfo,
                      unlock,mesgid );
#endif
    snprintf( process , sizeof(process) , "%d" , ret );
//	log( "unlock:%d, process:%s\n", unlock, process);
    if( unlock) {
#ifdef _UNLOCKPOOL
		if( addUnlockPool( ti, id, charname, process, mesgid) <0  ) {
#else
		if( addWorkerTask( id , "dummy" , "3" , dummyCallback , ti ,
	                       id , "dummy" , charname , process ,"",
	                       mesgid , 0 ) < 0 ) { 
#endif
			log("游戏退出失败, 进程=%s\n" , process );    
	    } else {
	        log("游戏退出成功, 进程=%s\n" , process );
	    }

    } else {
	    log("自动保存\n\n");
    }

    log("在线玩家保存\n");
}

#ifdef _RELOGIN
void saacproto_ACCharSaveII_recv( int ti, char* id,
                                char *charname,char* opt ,
                                char* charinfo , int unlock , int mesgid , int charindex )
{
    char process[16];
    int ret;
    
    if( !is_game_server_login(ti) ){
        saacproto_ACCharSave_send( ti , FAILED , "not login" , mesgid );
        return;
    }
    ret = charSave( ti, id, charname, opt, charinfo,
                      unlock,mesgid, charindex );
	// 储存人物资料
	// 记录相关资讯
	// 不送GE至WGS
	// 不更新名片资料
    if (unlock) {
#ifdef _UNLOCKPOOL
		if( addUnlockPool( ti, id, charname, process, mesgid) <0  ) {
#else
		if( addWorkerTask( id , "dummy" , "3" , dummyCallback , ti ,
	                       id , "dummy" , charname , process ,"",
	                       mesgid , 0 ) < 0 ) { 
#endif
		log("游戏退出失败, 进程=%s\n" , process );    
	    } else {
	        log("游戏退出成功, 进程=%s\n" , process );
	    }
    } else {
	    log("自动保存\n\n");
    }
    log("在线玩家保存\n");
}
#endif


void saacproto_ACCharList_recv( int ti,char* id,char* pas , int mesgid)
{
    static int process=0;
    char buf[10];

	//log(" get_charlist ");

    process++;
    if (process > MAX_PROCESS) process=1;
    snprintf(buf, sizeof(buf), "%d", process);

#ifdef _CHANGEGALAXY
	if( isChangeGalaxyLocked( id) ) {
		log(" 因星系移民被锁 ");
		saacproto_ACCharList_send( ti , FAILED , "CHANGE_GALAXY" , mesgid );
		return;
	}
#endif

    if( !is_game_server_login( ti ) ){
        saacproto_ACCharList_send( ti , FAILED , "not login" , mesgid );
        return;
    }
    if( addWorkerTask( id , pas , "1" , charListCallback , ti , id , pas , "", buf ,"0",
                       mesgid , 0 ) < 0 ){
        saacproto_ACCharList_send( ti , FAILED , "server load too high",
                                   mesgid );
        return;
    }
    log( "档案列表: %s\n", id );
}

void saacproto_ACCharDelete_recv( int ti,char* id,char* pas,
                                  char* charname , char *option , int mesgid)
{

    if( !is_game_server_login(ti) ){
        saacproto_ACCharDelete_send( ti , FAILED , "not login" , mesgid );
        return;
    }
#ifdef _DEATH_FAMILY_LOGIN_CHECK //无法删除人物资料
	saacproto_ACCharDelete_send( ti , FAILED , "server load too high",
							mesgid );
	return;
#endif

#ifdef _DEATH_CONTEND //无法删除人物资料
	saacproto_ACCharDelete_send( ti , FAILED , "server load too high",
							mesgid );
	return;
#endif
    // Nuke
    if( addWorkerTask( id , pas , "4" , charDeleteCallback , ti ,
                       id , pas , charname , "0" ,"",
                       mesgid , 0 ) < 0 ){
        saacproto_ACCharDelete_send( ti , FAILED , "server load too high",
                                   mesgid );
        return;
    }
    log( "档案删除: 附加作业 %s %s\n",id , pas );
}

void saacproto_ACLock_recv( int ti ,char* id,int lock,int mesgid )
{
	if( !is_game_server_login(ti) ){
		saacproto_ACLock_send( ti , FAILED , "not login" , mesgid );
		return;
	}
	// Arminius 7.25 test unlock
	log("ACLock recv:%d\n",lock);
    if (lock==2) {
		char buf[4096];
		if (GetMemLockState(getHash(id) & 0xff, id, buf)) {
			sprintf(retdata, "USRLOCKED:%s", buf);
		} else {
			sprintf(retdata, "USRUNLOCKED:%s", buf);
		}
		log(retdata);
    } else if (lock==3) {
		int proc;
		if (DeleteMemLock(getHash(id) & 0xff, id, &proc)) {
			sprintf(retdata, "GMUNLOCKSUCCESS");
		} else {
			sprintf(retdata, "GMUNLOCKFAIL");
		}
		log(retdata);
	} else if (lock==4) {
		DeleteMemLockServer(id);
		sprintf(retdata, "GMUNLOCKALL");
    } else {
#ifdef _LOCK_ADD_NAME
		if ( lockUser( getGSName( ti ) , id , "" , "0" , lock , result,sizeof(result),
										retdata ,sizeof( retdata ) , "0" , "0") < 0 ) {
#else
		if ( lockUser( getGSName( ti ) , id , "0" , lock , result,sizeof(result),
										retdata ,sizeof( retdata ) , "0" , "0") < 0 ) {
#endif
			log( "锁定用户: %s 失败\n" , id );
		} else {
			log( "锁定用户: %s 成功\n" , id );
		}
    }
    // Spock end
    saacproto_ACLock_send( ti , result , retdata , mesgid );
}

void saacproto_ACUCheck_recv( int ti , char *id , int status )
{
    if( !is_game_server_login(ti) ){
        log( "UCHECK: server not logging in: ID:%s status:%d\n",
             id, status );
        return;
    }
	
	if( status == 0 ){
       log( "UCHECK: user %s is not locked by %s\n", id , getGSName( ti ) );
#ifdef _DELAY_UNLOCK
	   //remeber the cdkey into delay unlock list.
	   if( UNlockM_addPlayer( id) != -1 ){
			log( "remeber:[%s] unlock 15min later!!\n", id);
			saacproto_ACKick_recv( ti, id, 6, -1);	//unlock
	   }
#ifdef _WAEI_KICK
		saacproto_ACKick_recv( ti, id, 1, -1);
#endif
#endif
    } else {
        log( "UCHECK: user %s is still locked by %s\n",
             id , getGSName( ti ) );
#ifdef _WAEI_KICK
		saacproto_ACKick_recv( ti, id, 1, -1);
#endif
    }
}



void saacproto_DBUpdateEntryString_recv( int fd,
                                         char* table,
                                         char* key,
                                         char* value,
                                         int msgid,
                                         int msgid2 )
{
    int r;
    r = dbUpdateEntryString( table, key, value );

	if( r != 0 ){
		log( "failed: DBUpdateEntryString err !!\n");
	}
/* 2003/06/26
    if( r == 0 ){
//        saacproto_DBUpdateEntryString_send( fd, SUCCESSFUL,table,key,
                                            msgid,msgid2 );
    } else {
//        saacproto_DBUpdateEntryString_send( fd, FAILED,table,key,
                                            msgid,msgid2 );        
    }
*/
}


void saacproto_DBDeleteEntryString_recv( int fd,
                                         char* table,
                                         char* key,
                                         int msgid,
                                         int msgid2 )
{
    int r;

    r = dbDeleteEntryString( table,key );
    if( r == 0 ){
        saacproto_DBDeleteEntryString_send( fd,SUCCESSFUL,table,key,
                                            msgid,msgid2 );
    } else {
        saacproto_DBDeleteEntryString_send( fd,FAILED,table,key,
                                            msgid,msgid2 );        
    }
}


void saacproto_DBGetEntryString_recv( int fd,
                                      char* table,
                                      char* key,
                                      int msgid,
                                      int msgid2 )
{
    char output[16384];
    int r;
    r = dbGetEntryString( table, key, output, sizeof( output) );
    if( r == 0 ){
        saacproto_DBGetEntryString_send( fd,SUCCESSFUL,output,table,key,
                                         msgid,msgid2 );
    } else {
        saacproto_DBGetEntryString_send( fd,FAILED,"",table,key,
                                         msgid,msgid2 );        
    }
    
}



void saacproto_DBUpdateEntryInt_recv( int fd,
                                   char* table,
                                   char* key,
                                   int value,
                                   char *info,
                                   int msgid,
                                   int msgid2)
{
    if( !is_game_server_login(fd) ){
        saacproto_DBUpdateEntryInt_send( fd, FAILED, "","",msgid, msgid2 );
        return;
    }    
    if( dbUpdateEntryInt( table , key , value, info ) == 0 ){
        saacproto_DBUpdateEntryInt_send(
            fd , SUCCESSFUL ,table,key, msgid, msgid2 );
    } else {
        saacproto_DBUpdateEntryInt_send( fd , FAILED , table,key, msgid, msgid2 );        
    }
}

#ifdef _ALLDOMAN					// (不可开) Syu ADD 排行榜NPC
void saacproto_UpdataStele_recv( int fd , char *cdkey , char *name , char *title ,
								int level , int trns , int time , int floor )
{
#ifdef _FIX_ALLDOMAN
	int i, j;

	if( !strcmp( cdkey , "FirstLoad") ){
		Send_A_herolist( fd );
	}else{
		for( i=0; i<MAX_HERO_LIST; i++){
			if( Herolist[i].use == 0 )continue;
			if( !strcmp( Herolist[i].strings[0], cdkey) &&
				!strcmp( Herolist[i].strings[1], name) ){
				Herolist[i].use = 0;
			}
		}
		for( i=0; i<MAX_HERO_LIST; i++){
			if( Herolist[i].use == 0 )continue;
			if( floor <= Herolist[i].intdata[3] ) continue;
			for( j=MAX_HERO_LIST-1; j>i; j--){ //20031001bug (原j++)
				sprintf( Herolist[j].strings[0], "%s", Herolist[j-1].strings[0] );
				sprintf( Herolist[j].strings[1], "%s", Herolist[j-1].strings[1] );
				sprintf( Herolist[j].strings[2], "%s", Herolist[j-1].strings[2] );
				Herolist[j].intdata[0] = Herolist[j-1].intdata[0];
				Herolist[j].intdata[1] = Herolist[j-1].intdata[1];
				Herolist[j].intdata[2] = Herolist[j-1].intdata[2];
				Herolist[j].intdata[3] = Herolist[j-1].intdata[3];
				Herolist[j].use = Herolist[j-1].use;
				Herolist[j-1].use = 0;
			}
			sprintf( Herolist[i].strings[0], "%s", cdkey );
			sprintf( Herolist[i].strings[1], "%s", name );
			sprintf( Herolist[i].strings[2], "%s", title );
			Herolist[i].intdata[0] = level;
			Herolist[i].intdata[1] = trns;
			Herolist[i].intdata[2] = time;
			Herolist[i].intdata[3] = floor;
			Herolist[i].use = 1;
			Send_S_herolist( i);
			break;
		}
		SAVE_herolist( fd);
	}
#else
	int i, min=140, minnum=-1, nameflag=-1;
	for ( i=0; i<MAX_HERO_LIST; i ++ ) {
		if( atoi( Herolist[i][6] ) <  min ) {
			min = atoi (Herolist[i][6]) ; 
			minnum=i;
		}
		if ( strcmp ( Herolist[i][0] , cdkey ) == 0 && strcmp ( Herolist[i][1] , name ) == 0 )
			nameflag = i ; 
	}
	if( strcmp ( cdkey , "FirstLoad") == 0 &&
		strcmp ( name , "LoadHerolist") == 0 &&
		floor == 999 )

		Send_A_herolist( fd );
	else if ( nameflag != -1 && floor > atoi(Herolist[nameflag][6]) ) {
		Send_S_herolist ( Herolist[nameflag][0] , Herolist[nameflag][1] , 
			cdkey , name , title , level , trns , floor ) ; 
		sprintf ( Herolist[nameflag][2] , "%s" , title ) ; 
		sprintf ( Herolist[nameflag][3] , "%d" , level ) ; 
		sprintf ( Herolist[nameflag][4] , "%d" , trns ) ; 
		sprintf ( Herolist[nameflag][5] , "%d" , time ) ; 
		sprintf ( Herolist[nameflag][6] , "%d" , floor ) ; 
		SAVE_herolist( fd );
	}else if ( nameflag == -1 && floor > min ) {
		Send_S_herolist ( Herolist[minnum][0] , Herolist[minnum][1] , 
			cdkey , name , title , level , trns , floor ) ; 
		sprintf ( Herolist[minnum][0] , "%s" , cdkey ) ; 
		sprintf ( Herolist[minnum][1] , "%s" , name  ) ; 
		sprintf ( Herolist[minnum][2] , "%s" , title ) ; 
		sprintf ( Herolist[minnum][3] , "%d" , level ) ; 
		sprintf ( Herolist[minnum][4] , "%d" , trns ) ; 
		sprintf ( Herolist[minnum][5] , "%d" , time ) ; 
		sprintf ( Herolist[minnum][6] , "%d" , floor ) ; 
		SAVE_herolist( fd );
	}
#endif
}
#endif

void saacproto_DBGetEntryRank_recv( int fd,
                                    char* table,
                                    char* key,
                                    int msgid,
                                    int msgid2)
{
    int rk,c;

    if( !is_game_server_login(fd) ){
        saacproto_DBGetEntryRank_send( fd, FAILED, 0,0,"","",msgid, msgid2 );
        return;
    }
    
    dbGetEntryRank( table, key , &rk, &c );
    if( rk <0 ){
        saacproto_DBGetEntryRank_send(
            fd, FAILED, -1, -1, table, key, msgid, msgid2 );
    } else {
        saacproto_DBGetEntryRank_send( fd, SUCCESSFUL,
                                       rk, c, table, key, msgid, msgid2 );
    }
}


void saacproto_DBDeleteEntryInt_recv( int fd,
                                   char* table,
                                   char* key,
                                   int msgid,
                                   int msgid2)
{
    if( !is_game_server_login(fd) ){
        saacproto_DBDeleteEntryInt_send( fd, FAILED,"","",msgid, msgid2 );
        return;
    }
    
    if( dbDeleteEntryInt( table, key ) < 0 ){
        saacproto_DBDeleteEntryInt_send(
            fd, FAILED, table, key, msgid, msgid2 );
    } else {
        saacproto_DBDeleteEntryInt_send(
            fd, SUCCESSFUL, table, key, msgid, msgid2 );
    }
}
    

void saacproto_DBGetEntryInt_recv( int fd,
                                char* table,
                                char* key,
                                int msgid,
                                int msgid2)
{
    int r ;
    int output;

    if( !is_game_server_login(fd) ){
        saacproto_DBGetEntryInt_send( fd, FAILED,-1,"","",msgid, msgid2 );
        return;
    }
    
    r = dbGetEntryInt( table, key, &output );
    
    if( r < 0 ){
        saacproto_DBGetEntryInt_send( fd,
                                      FAILED, -1, table, key, msgid, msgid2);
    } else {
        saacproto_DBGetEntryInt_send(
            fd, SUCCESSFUL, output, table, key, msgid, msgid2 );
    }

}


void saacproto_DBGetEntryByRank_recv( int fd,
                                char* table,
                                int start, int end,
                                int msgid ,
                                int msgid2)
{
    char output[16384];
    int r;

    if( !is_game_server_login(fd) ){
        saacproto_DBGetEntryByRank_send( fd, FAILED,"","",msgid, msgid2 );
        return;
    }
    
    r = dbGetEntryRankRange( table, start, end, output, sizeof( output ));

    if( r < 0 ){
        saacproto_DBGetEntryByRank_send(
            fd, FAILED, "", table, msgid, msgid2 );
    } else {
        saacproto_DBGetEntryByRank_send(
            fd, SUCCESSFUL, output,table, msgid, msgid2 );
    }
}

void saacproto_DBGetEntryByCount_recv( int fd,
                                       char* table,
                                       int count_start,
                                       int num,
                                       int msgid ,
                                       int msgid2)
{
    char output[16384];
    int r;

    if( !is_game_server_login(fd) ){
        saacproto_DBGetEntryByCount_send(
            fd, FAILED,"",table,count_start,msgid, msgid2 );
        return;
    }
    
    r = dbGetEntryCountRange( table, count_start, num, output, sizeof(output));

    if( r < 0 ){
        saacproto_DBGetEntryByCount_send( fd, FAILED, "", table,
                                          count_start, msgid, msgid2 );
    } else {
        saacproto_DBGetEntryByCount_send( fd, SUCCESSFUL, output, table,
                                          count_start,  msgid, msgid2 );       
    }
}

void saacproto_Broadcast_recv( int fd, char* id, char* charname, char* message, int flag)
{
    if( !is_game_server_login( fd) ){
        log( "Broadcast: no login. id:%s charname:%s message:%s flag:%d\n",
             id, charname, message , flag );
        return;
    }
    gmsvBroadcast( fd, id, charname, message , flag );
}


void saacproto_Message_recv( int fd,
                             char* id_from,
                             char* charname_from,
                             char* id_to,
                             char* charname_to,
                             char* message,
                             int option )
{
    receiveMail( id_from, charname_from,
                       id_to, charname_to, message, option ,0,0);
}

void saacproto_MessageAck_recv( int fd,
                                char* id,
                                char* charname,
                                char* result,
                                int mesgid )
{
    int a;
    if( strcmp( result, SUCCESSFUL ) )a=0; else a =-1;
    receiveMailAck( id, charname, a , mesgid );
}

void saacproto_MessageFlush_recv( int fd,
                                  char* id,
                                  char* charname )
{
    flushMail( fd, id , charname );
}

/*******************************************************
 * CoolFish: Family 2001/5/18
 ******************************************************/
#ifdef _PERSONAL_FAME
void saacproto_ACAddFM_recv( int fd, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petattr,
	char *fmrule, int fmsprite, int fmleadergrano, int fame, int charfdid)
#else
void saacproto_ACAddFM_recv( int fd, char *fmname, char *fmleadername,
	char *fmleaderid, int fmleaderlv, char *petname, char *petattr,
	char *fmrule, int fmsprite, int fmleadergrano, int charfdid)
#endif
{
#ifdef __FAMILY
	int r = 0, index = 0;
#ifdef _PERSONAL_FAME
	r = ACAddFM(fd, &index, fmname, fmleadername, fmleaderid, fmleaderlv,
	   petname, petattr, fmrule, fmsprite, fmleadergrano, fame, charfdid);
#else
	r = ACAddFM(fd, &index, fmname, fmleadername, fmleaderid, fmleaderlv,
	   petname, petattr, fmrule, fmsprite, fmleadergrano, charfdid);
#endif
	if (r < 0){
		saacproto_ACAddFM_send(fd, FAILED, r, index, charfdid);
	}else{
		saacproto_ACAddFM_send(fd, SUCCESSFUL, r, index, charfdid);
	}
#endif
}

void saacproto_ACJoinFM_recv(int fd, char *fmname, int fmindex,
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	char *charname, char *charid, int charlv, int index, int fame, int charfdid)
#else
	char *charname, char *charid, int charlv, int index, int charfdid)
#endif
{
#ifdef __FAMILY
	int r = 0;
#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	r =  ACJoinFM(fd, index, fmname, fmindex, charname, charid, charlv, fame, charfdid);
#else
	r =  ACJoinFM(fd, index, fmname, fmindex, charname, charid, charlv, charfdid);
#endif
	if (r < 0){
		saacproto_ACJoinFM_send(fd, FAILED, r, charfdid);
	}else{
		saacproto_ACJoinFM_send(fd, SUCCESSFUL, r, charfdid);
	}
#endif
}

void saacproto_ACLeaveFM_recv(int fd, char *fmname, int fmindex,
	char *charname, char *charid, int index, int charfdid)
{
#ifdef __FAMILY
	int r = 0;
	r =  ACLeaveFM(index, fmname, fmindex, charname, charid);
	if (r < 0){
		saacproto_ACLeaveFM_send(fd, FAILED, r, charfdid);
	}else{
		saacproto_ACLeaveFM_send(fd, SUCCESSFUL, r, charfdid);
	}
#endif
}

#ifdef _LEADERFUNCHECK
void saacproto_ACDelFM_recv(int fd, char *fmname, int fmindex,
	int index, char *charname, char *charid, int charfdid)
{
	int r = 0, result = 0;
	result = CheckLeaderQ(index, fmname, fmindex, charname, charid);
	if(result < 0)
	{
		saacproto_ACDelFM_send(fd, FAILED, charfdid);
		return;
	}
	// shan modify begin
	// original
	/*if (ChangeFMLeader(index, fmname, fmindex) < 0)    
		r = ACDelFM(index, fmname, fmindex); 
	else 
		r = -1;  // shan  1 -> -1
	if (r < 0)
		saacproto_ACDelFM_send(fd, FAILED, charfdid);
	else
		saacproto_ACDelFM_send(fd, SUCCESSFUL, charfdid);
	*/   
	r = ACDelFM(index, fmname, fmindex); 	
	if (r < 0)
		saacproto_ACDelFM_send(fd, FAILED, charfdid);
	else
		saacproto_ACDelFM_send(fd, SUCCESSFUL, charfdid);
	// shan end
}
#else
void saacproto_ACDelFM_recv(int fd, char *fmname, int fmindex,
	int index, int charfdid)
{
#ifdef __FAMILY
	int r = 0;
#ifdef _FMVER21
	if (ChangeFMLeader(index, fmname, fmindex) >= 0)
		r = ACDelFM(index, fmname, fmindex);
	else r = 1;
#else
	r =  ACDelFM(index, fmname, fmindex);
#endif	
	if (r < 0){
		saacproto_ACDelFM_send(fd, FAILED, charfdid);
	}
#ifdef _FMVER21
	else if (r == 1){
	}
#endif
	else{
		saacproto_ACDelFM_send(fd, SUCCESSFUL, charfdid);
	}
#endif
}
#endif

void saacproto_ACShowFMList_recv(int fd)
{
#ifdef __FAMILY
	int r = 0;
	char data[150 * MAX_FAMILY];
	r =  ACShowFMList(data);
	if (r < 0){
		saacproto_ACShowFMList_send(fd, FAILED, r, "Nothing");
	}else{
		saacproto_ACShowFMList_send(fd, SUCCESSFUL, r, data);
	}
#endif
}

#ifdef _DEATH_FAMILY_GM_COMMAND	// WON ADD 家族战GM指令
void saacproto_ACShowMemberList_2_recv( int fd, int charindex, int fm1, int fm2, int time, int id )
{
	char *fm1_name, *fm2_name;
	
	log("\n won test 7.2 ==> fm1(%d) fm2(%d) time(%d) id(%d)", fm1, fm2, time, id );

	fm1_name = get_fm_leader_index( fm1 );
	fm2_name = get_fm_leader_index( fm2 );
	log("\n won test 7.3 ==> fm1_name(%s) fm2_name(%s)", fm1_name, fm2_name );

	saacproto_ACSHOWMEMBERLIST_2_send( fd, charindex, fm1, fm1_name, fm2, fm2_name, time, id );

}
#endif


void saacproto_ACShowMemberList_recv(int fd, int index)
{
#ifdef __FAMILY
	int r = 0, fmacceptflag = 0, fmjoinnum = 0;
	char data[150 * MAX_MEMBERNUM];
	r =  ACShowFMMemberList(index, &fmacceptflag, &fmjoinnum, data);
	if (r < 0){
		saacproto_ACShowMemberList_send(fd, FAILED, index, r,
			fmacceptflag, fmjoinnum, "Nothing");
	}else{
		saacproto_ACShowMemberList_send(fd, SUCCESSFUL, index, r,
			fmacceptflag, fmjoinnum, data);
	}
#endif
}

void saacproto_ACFMDetail_recv(int fd, char *fmname,
	int fmindex, int index, int charfdid)
{
#ifdef __FAMILY
	int r = 0;
	char data[15000];
	r =  ACFMDetail(index, fmname, fmindex, data);
	if (r < 0){
		saacproto_ACFMDetail_send(fd, FAILED, "Nothing", charfdid);
	}else{
		saacproto_ACFMDetail_send(fd, SUCCESSFUL, data, charfdid);
	}
#endif
}

#ifdef _FMVER21
void saacproto_ACMemberJoinFM_recv(int fd, char *fmname, int fmindex,
	char *charname, int charindex, int index, int result, int meindex,
	int charfdid)
#else
void saacproto_ACMemberJoinFM_recv(int fd, char *fmname, int fmindex,
	char *charname, int charindex, int index, int result, int charfdid)
#endif
{
	int r = 0;
#ifdef _FMVER21
   	r =  ACMemberJoinFM(index, fmname, fmindex, charname, charindex, result, meindex);
#else
	r =  ACMemberJoinFM(index, fmname, fmindex, charname, charindex);
#endif		
	if (r < 0){
		saacproto_ACMemberJoinFM_send(fd, FAILED, charfdid);
	}else{
		saacproto_ACMemberJoinFM_send(fd, SUCCESSFUL, charfdid);
	}
}

#ifdef _FMVER21
void saacproto_ACMemberLeaveFM_recv(int fd, char *fmname, int fmindex,
	char *charname, int charindex, int index, int meindex, int charfdid)
#else
void saacproto_ACMemberLeaveFM_recv(int fd, char *fmname, int fmindex,
	char *charname, int charindex, int index, int charfdid)
#endif
{
#ifdef __FAMILY
	int r = 0;
	int flag = 1; // 主动离开家族
#ifdef _FMVER21
	r =  ACMemberLeaveFM(index, fmname, fmindex, charname, flag,
		charindex, meindex);
#else
	r =  ACMemberLeaveFM(index, fmname, fmindex, charname, flag, charindex);
#endif
	if (r < 0){
		saacproto_ACMemberLeaveFM_send(fd, FAILED, charfdid);
	}else{
		saacproto_ACMemberLeaveFM_send(fd, SUCCESSFUL, charfdid);
	}
#endif
}

#ifdef _FM_MODIFY
void saacproto_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
		char *charname, char *charid, int charlv, int eventflag, int charfdid,int gsnum)
#else
	#ifdef _FMVER21
	void saacproto_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
		char *charname, char *charid, int charlv, int eventflag, int charfdid)
	#else
	void saacproto_ACFMCharLogin_recv(int fd, char *fmname, int fmindex,
		char *charname, char *charid, int charlv, int charfdid)
	#endif
#endif
{
#ifdef __FAMILY
	int r = 0, floor = 0, fmpopular = 0, joinflag = 0, fmsetupflag = 0;
	int flag = 0, charindex = 0;
#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
	int charfame = 0;
#endif
#ifdef _NEW_MANOR_LAW
	int momentum = 0;
#endif
#ifdef _FM_MODIFY
	r =  ACFMCharLogin(fd, -1, fmname, fmindex, charname, charid, charlv,
			&floor, &fmpopular, &joinflag, &fmsetupflag, &charindex, charfdid,
			&charfame, eventflag,gsnum
	#ifdef _NEW_MANOR_LAW
			,&momentum
	#endif
			);
#else
	#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
		r =  ACFMCharLogin(fd, -1, fmname, fmindex, charname, charid, charlv,
			&floor, &fmpopular, &joinflag, &fmsetupflag, &charindex, charfdid,
			&charfame, eventflag);
	#else
		r =  ACFMCharLogin(fd, -1, fmname, fmindex, charname, charid, charlv,
			&floor, &fmpopular, &joinflag, &fmsetupflag, &charindex, charfdid);
	#endif
#endif

#ifdef _PERSONAL_FAME   // Arminius: 家族个人声望
	if (r < 0){
		saacproto_ACFMCharLogin_send(fd, FAILED, r, floor, fmpopular,
			joinflag, fmsetupflag, flag, charindex, charfame, charfdid
	#ifdef _NEW_MANOR_LAW
			,momentum
	#endif
			);
	}else{
		saacproto_ACFMCharLogin_send(fd, SUCCESSFUL, r, floor, fmpopular,
			joinflag, fmsetupflag, flag, charindex, charfame, charfdid
	#ifdef _NEW_MANOR_LAW
			,momentum
	#endif
			);
	}
#else
	if (r < 0){
		saacproto_ACFMCharLogin_send(fd, FAILED, r, floor, fmpopular,
			joinflag, fmsetupflag, flag, charindex, charfdid);
	}else{
		saacproto_ACFMCharLogin_send(fd, SUCCESSFUL, r, floor, fmpopular,
			joinflag, fmsetupflag, flag, charindex, charfdid);
	}
#endif

#endif
}

void saacproto_ACFMCharLogout_recv(int fd, char *fmname, int fmindex,
	char *charname, char *charid, int charlv, int index,int charfdid)
{
#ifdef __FAMILY
	int r = 0;
	r =  ACFMCharLogout(index, fmname, fmindex, charname, charid, charlv,
		charfdid);
	if (r < 0){
		saacproto_ACFMCharLogout_send(fd, FAILED, charfdid);
	}else{
		saacproto_ACFMCharLogout_send(fd, SUCCESSFUL, charfdid);
	}
#endif
}

void saacproto_ACFMReadMemo_recv(int fd, int index)
{

#ifdef __FAMILY
	int r = 0, dataindex;
	char data[15000];
	r =  ACFMReadMemo(index, &dataindex, data);
	if (r < 0){
		saacproto_ACFMReadMemo_send(fd, FAILED, index, r, dataindex, "Nothing");
	}else{
		saacproto_ACFMReadMemo_send(fd, SUCCESSFUL, index, r, dataindex, data);
	}
#endif

}
                
void saacproto_ACFMWriteMemo_recv(int fd, char *fmname, int fmindex,
	char *data, int index)
{
#ifdef __FAMILY
	int r = 0;
	r =  ACFMWriteMemo(index, fmname, fmindex, data);
	if (r < 0){
		saacproto_ACFMWriteMemo_send(fd, FAILED, index);
	}else{
		saacproto_ACFMWriteMemo_send(fd, SUCCESSFUL, index);
	}
#endif
}

void saacproto_ACFMPointList_recv(int fd)
{
#ifdef __FAMILY
        int r = 0;
        char data[15000];
        r =  ACFMPointList(data);
        if (r < 0)
        {
        	saacproto_ACFMPointList_send(fd, FAILED, "nothing");
        }else{
        	saacproto_ACFMPointList_send(fd, SUCCESSFUL, data);
        }
#endif
}



#ifdef _CK_ONLINE_PLAYER_COUNT    // WON ADD 计算线上人数
void PlayerCount(void)
{
	int i, total_count=0;
	char msg[64], out_msg[1024];
	FILE *fpcount; 
    extern gmsv gs[MAXCONNECTION];

	strcpy(out_msg, "");

	for(i=0; i<MAXCONNECTION; i++){
		if( gs[i].use ){
			sprintf(msg, "%s:%d  Item ratio-->0%%\n", gs[i].name, online_player[i].playcount );	
			strcat( out_msg, msg );
			total_count += online_player[i].playcount;
		}
	}
	sprintf(msg, "-----------------------\n");
	strcat( out_msg, msg );	
	sprintf(msg, "Total:%d\n", total_count);
	strcat( out_msg, msg );

    fpcount = fopen("count.txt","w");
    if (fpcount!=NULL){
        fprintf(fpcount,"%s", out_msg );
        fclose(fpcount);
    }

}

void saacproto_GS_ACK_PLAYER_COUNT_recv( int fd, int playcount )
{		
	online_player[fd].playcount = playcount;
}
#endif




void saacproto_ACSetFMPoint_recv(int fd, char *fmname, int fmindex, 
	int index, int fmpointindex, int fl, int x, int y, int charfdid)
{
#ifdef __FAMILY
	int r = 0;
	r =  ACSetFMPoint(index, fmname, fmindex, fmpointindex, fl, x, y);
	if (r < 0){
		saacproto_ACSetFMPoint_send(fd, FAILED, r, charfdid);
	}else{
		saacproto_ACSetFMPoint_send(fd, SUCCESSFUL, r, charfdid);
	}
#endif
}

void saacproto_ACFixFMPoint_recv(int fd, char *winfmname, int winfmindex, int winindex,
	char *losefmname, int losefmindex, int loseindex, int village)
{
#ifdef __FAMILY
	int r = 0;
	r =  ACFixFMPoint(winindex, winfmname, winfmindex,
		loseindex, losefmname, losefmindex, village);
	if (r < 0){
		saacproto_ACFixFMPoint_send(fd, FAILED, r);
	}else{
		saacproto_ACFixFMPoint_send(fd, SUCCESSFUL, r);
	}
#endif
}

void saacproto_ACFMAnnounce_recv(int fd, char *fmname, int fmindex, int index,
        char *data, int color)
{
#ifdef __FAMILY
	int r = 0, i = 0;
	r = ACFMAnnounce(fmname, fmindex, index, data, color);
	if (r < 0){
		saacproto_ACFMAnnounce_send(fd, FAILED, fmname, fmindex, index,
			1, data, color);
	}else{
		extern gmsv gs[MAXCONNECTION];
		for (i = 0; i < MAXCONNECTION; i ++)
		{
		   if (gs[i].use && gs[i].name[0])
		   {
		      saacproto_ACFMAnnounce_send(i, SUCCESSFUL, fmname, fmindex,
		      	index, 1, data, color);
		   }
		}
	}	
#endif
}

void saacproto_ACShowTopFMList_recv(int fd, int kindflag)
{
#ifdef __FAMILY
	int r = 0;
	char data[150 * MAX_FAMILY];
	strcpy(data, "");
	r =  ACShowTopFMList(data, sizeof(data), kindflag);
	if (r < 0)	{
		saacproto_ACShowTopFMList_send(fd, FAILED, kindflag, r, "Nothing");
	}else	{
		saacproto_ACShowTopFMList_send(fd, SUCCESSFUL, kindflag, r, data);
	}
#endif
}

#ifdef _FAMILYBANKSTONELOG
void saacproto_ACgetFMBankgold_recv(int fd, char *fmname, int fmindex,
									int index, int charindex, int charfdid)
{
	int r = 0;
	r =  ACgetFMBankgold(index, fmname, fmindex, charindex);
	saacproto_ACgetFMBankgold_send(fd,charfdid,r);
}
#endif
void saacproto_ACFixFMData_recv(int fd, char *fmname, int fmindex, int index,
	int kindflag, char *data1, char *data2, int charindex, int charfdid)
{
#ifdef __FAMILY
	int r = 0;
	r =  ACFixFMData(index, fmname, fmindex, kindflag, charindex, data1, data2);
	if (r < 0){
		saacproto_ACFixFMData_send(fd, FAILED, kindflag, data1, data2, charfdid);
	}else{
		saacproto_ACFixFMData_send(fd, SUCCESSFUL, kindflag, data1, data2, charfdid);
	}
#endif
}

void saacproto_ACFixFMPK_recv(int fd, char *winfmname, int winfmindex, int winindex,
	char *losefmname, int losefmindex, int loseindex)
{
#ifdef __FAMILY
	int r = 0;
	r =  ACFixFMPK(winindex, winfmname, winfmindex,
		loseindex, losefmname, losefmindex);
	if (r < 0){
		saacproto_ACFixFMPK_send(fd, FAILED, r, winindex,
			loseindex);
	}else{
		saacproto_ACFixFMPK_send(fd, SUCCESSFUL, r, winindex,
			loseindex);
	}
#endif
}

void saacproto_ACGMFixFMData_recv(int fd, int index, char *charid, char *cmd,
	char *data, int charfdid)
{
#ifdef __FAMILY
	int r = 0;
	char fmname[256];
	r =  ACGMFixFMData(index, fmname, charid, cmd, data);
	if (r < 0){
		saacproto_ACGMFixFMData_send(fd, FAILED, fmname, charfdid);
	}
	else{
		saacproto_ACGMFixFMData_send(fd, SUCCESSFUL, fmname, charfdid);
	}
#endif
}

void saacproto_ACGetFMData_recv(int fd, char *fmname, int fmindex, int index,
	int kindflag, int charfdid)
{
#ifdef __FAMILY
	int r = 0, data = 0;
	r =  ACGetFMData(index, fmname, fmindex, kindflag, &data);
	if (r < 0){
		saacproto_ACGetFMData_send(fd, FAILED, kindflag, data, charfdid);
	}else{
		saacproto_ACGetFMData_send(fd, SUCCESSFUL, kindflag, data, charfdid);
	}
#endif
}

void saacproto_ACreLoadFmData_recv(int fd, int type, int data)
{
	int i=0;
extern gmsv gs[MAXCONNECTION];
	switch( type){
	case 1:
		log( "reload FM_DATA:%d !\n", data);
		readOneFamilyFromTi( data);
		break;
	case 2:
		{
			char data[15000];
			readFMPoint( fmpointdir);
			if( ACFMPointList( data) >= 0 ){
				log( "reload FM_POINT !\n" );
				for (i=0; i<MAXCONNECTION; i++){
					if( gs[i].use && gs[i].name[0] )
						saacproto_ACFMPointList_send( i, SUCCESSFUL, data);
				}
			}
		}
		break;
	case 3:	//fmpk_list
		{
			int j=0;
			log( "reload FMPK_LIST !\n" );
			FMPK_InitList();
			FMPK_LoadList();
			for( j=0; j<FMPKLIST_MAXNUM; j++){ 
				for (i=0; i<MAXCONNECTION; i++){
					if( gs[i].use && gs[i].name[0] )
						saacproto_ACLoadFmPk_send(i, FMPK_GetData( j) );
				}
			}
		}
		break;
	}
}

#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
#ifdef _ACFMPK_LIST
void saacproto_ACLoadFmPk_recv(int fd, int fmpks_pos)
{
	if( fmpks_pos < 0 || fmpks_pos > MAX_FMPOINT) return;
	saacproto_ACLoadFmPk_send( fd, FMPK_GetData( fmpks_pos-1) );
}

void saacproto_ACSendFmPk_recv(int fd, int fmpks_pos, int userindex, int flg, char *data)
{
	extern  gmsv gs[MAXCONNECTION];
	char buf[1024];
	int i=0;
	sprintf( buf, "%d|%s", fmpks_pos, data);

	if( ( userindex != -1 && FMPK_GetTypeFlg( fmpks_pos-1) == 1 )||
		FMPK_SetData( fmpks_pos-1, flg, buf, strlen( buf)) != 1 ){
		if(userindex != -1) saacproto_ACSendFmPk_send( fd, userindex, 0);
		log("err ACSendFmPk_send(%d, %d)\n", userindex, 0);
		return;
	}
	FMPK_BackUpList();
	for (i=0; i<MAXCONNECTION; i++){
		if( gs[i].use && gs[i].name[0] ){
			saacproto_ACLoadFmPk_send(i, FMPK_GetData( fmpks_pos-1) );
		}
	}
	//送确认讯息
	if(userindex != -1) saacproto_ACSendFmPk_send( fd, userindex, 1);
}
#else
void saacproto_ACLoadFmPk_recv(int fd, int fmpks_pos)
{
	extern char fm_pk_list[9][255];
	if(fmpks_pos > MAX_FMPOINT){
		log("\n fmpks_pos(%d) too big", fmpks_pos);
	}	
	saacproto_ACLoadFmPk_send(fd, fm_pk_list[fmpks_pos-1]);
}
void saacproto_ACSendFmPk_recv(int fd, int fmpks_pos, char *data)
{
	extern  gmsv gs[MAXCONNECTION];
	int i=0;

	if( (fmpks_pos>MAX_FMPOINT) || (fmpks_pos<1) ){
		log("\n fmpks_pos(%d) err !!", fmpks_pos);
	}		
	sprintf(fm_pk_list[fmpks_pos-1], "%d|%s", fmpks_pos, data);

	for (i=0; i<MAXCONNECTION; i++){
		if (gs[i].use && gs[i].name[0] && i!=fd){
			saacproto_ACLoadFmPk_send(i, fm_pk_list[fmpks_pos-1]);
		}
	}

	save_fm_pk_list();
}
#endif

#endif




#ifdef _RECAL_BACKUP_PLAYER			// WON 回溯功能

#define arraysizeof( x ) (sizeof(x)/sizeof(x[0]))

void saacproto_ACRecalPlayer_recv(int fd, char *uid, int userfdid, int GmCliId, 
								  char *id, int char_num, int date, int backup_flag)
{

	if( strlen(id) < 1 )		  { log("\n id(%s) too shout !!", id);		 return; }
	if( char_num<0 || char_num>1) { log("\n char_num(%d) err !!", char_num); return; }
	if( date<20000000 )			  { log("\n date(%d) format err!!", date); 	 return; }


	if( backup_flag == 1 ){ // 备份日期
		// 回传有备份的日期
		get_backup_date(fd, uid, userfdid, GmCliId, id, char_num, date);	

	}else if( backup_flag == 2 ){	// 人物资料
		get_online_player_data(fd, uid, userfdid, GmCliId, id, char_num, date);	// 传回线上人物资料
		get_backup_player_data(fd, uid, userfdid, GmCliId, id, char_num, date);	// 传回备份人物资料
	}

}

void saacproto_ACRecalBackup_recv(int fd, char *uid, int userfdid, int GmCliId, char *id, 
								  int char_num, int date, char *char_data, int backup_flag)
{
	extern struct ITEM_PET_CHANG item_pet_chang;
	char temp[64];
	int i, count=0;
	char filename1[128], filename2[128];
	char body[256];	

	if(backup_flag == 1){		// 回溯人物
		// 线上人物
		sprintf(filename1, "~/saac/char/0x%x/%s.%d.char", (getHash(id)&0xff), id, char_num );
		
		// 备份人物
		sprintf(filename2, "%s/char/0x%x/%s.%d.char", BACKUP_DIR, (getHash(id)&0xff), id, char_num );

		// 移动备份人物至线上人物
		strcpy(body, "");
		sprintf(body, "mv %s %s", filename2, filename1);
		system(body);
		
		/* 回溯成功 */
		saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 4);
	
	
	}else if(backup_flag == 2 ){		// 部分回溯人物

		log("\n won test 44 ==> char_data_chang(%s)", char_data);
		
		// 取出需回溯项目
		for(i=0;i<5;i++){
			memset(temp, 0, sizeof(temp));
			easyGetTokenFromString( char_data , count , temp , sizeof( temp ));
			item_pet_chang.equit_item[i] = atoi(temp);		count++;
		}
		for(i=0;i<15;i++){
			memset(temp, 0, sizeof(temp));
			easyGetTokenFromString( char_data , count , temp , sizeof( temp ));
			item_pet_chang.have_item[i] = atoi(temp);		count++;
		}
		for(i=0;i<30;i++){
			memset(temp, 0, sizeof(temp));
			easyGetTokenFromString( char_data , count , temp , sizeof( temp ));
			item_pet_chang.pool_item[i] = atoi(temp);		count++;
		}
		for(i=0;i<5;i++){
			memset(temp, 0, sizeof(temp));
			easyGetTokenFromString( char_data , count , temp , sizeof( temp ));
			item_pet_chang.have_pet[i] = atoi(temp);		count++;
		}
		for(i=0;i<20;i++){
			memset(temp, 0, sizeof(temp));
			easyGetTokenFromString( char_data , count , temp , sizeof( temp ));
			item_pet_chang.pool_pet[i] = atoi(temp);		count++;
		}

		
		// 处理部分回溯功能
		recal_part_backup(fd, uid, userfdid, GmCliId, id, char_num, date);
		
	}	

}

void recal_part_backup(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date)
{
/*
	extern struct ITEM_PET_CHANG item_pet_chang;
	FILE *fp;
	char filename[128], filename2[128], body[128];
	char onlin_char[65500], backup_char[65500];
	char *c_ptr, c_temp;
	int i=0;
	char *temp=NULL, *temp2, *temp3, *temp4, *temp5, *msg1, delim[20], out_msg[65530];
	

	// 线上人物
	{
		c_ptr=onlin_char;
		snprintf(body , sizeof(body ) , "%s.%d.char" , id , char_num );
		makeDirFilename(filename , sizeof(filename), chardir , getHash(id) ,body);

		if( !(fp=fopen(filename, "r")) ){
			log("\n open (%s) file err", filename );	return; 
		}

		memset(onlin_char, 0, sizeof(onlin_char));

		do{	
			c_temp = fgetc(fp);
			*c_ptr=c_temp;
			c_ptr++;
		}while(c_temp != EOF);
		*c_ptr='\0';

		fclose(fp);	
	}

	// 备份人物
	{
		c_ptr=backup_char;
		snprintf(body , sizeof(body ) , "%s.%d.char" , id , char_num );
		makeDirFilename(filename , sizeof(filename), chardir , getHash(id) ,body);

		sprintf(filename2, "%s/%s", BACKUP_DIR, filename);
		if( !(fp=fopen( filename2, "r" )) ){
			log("\n won test 100 ==> open (%s) file err", filename );	return; 
		}

		memset(backup_char, 0, sizeof(backup_char));

		do{
			c_temp = fgetc(fp);
			*c_ptr=c_temp;
			c_ptr++;
		}while(c_temp != EOF);
		*c_ptr='\0';

		fclose(fp);	
	}

	log("\n won test 1 ==>");

	{
		for(i=0;i<5;i++){
			if( item_pet_chang.equit_item[i] != i ){
				log("\n won test 2 ==> i(%d) item(%d)", i, item_pet_chang.equit_item[i]);
				
				// 线上人物
				while( temp==NULL ){
					log("\n won test 2.1 ==> temp(%s)", temp);
					sprintf( delim, "item%d=", i);
					log("\n won test 2.2 ==> delim(%s)", delim);
					temp=strstr( onlin_char, delim);
					log("\n won test 2.3 ==> temp(%s)", temp);
					if((i==0)&&(temp==NULL)){
						sprintf( delim, "flg2=");
						temp=strstr( onlin_char, delim);
						log("\n won test 2.4 ==> temp(%s)", temp);
						break;
					}
					i--;	
				}


				log("\n won test 3 ==>  temp(%s)", temp);
				temp2=strstr( temp, "\n");						
				log("\n won test 3.1 ==> temp2(%s)", temp2);
				
				msg1=(char*)calloc( 1,sizeof(char)*strlen(onlin_char)-strlen(temp2)+1);
				snprintf(msg1, strlen(onlin_char)-strlen(temp2), "%s", onlin_char );
				log("\n won test 3.2 ==> msg1(%s)", msg1);
			
				// 备份人物
				sprintf( delim, "item%d=", item_pet_chang.equit_item[i]-5+1);
				temp3=strstr( backup_char, delim);
				temp4=strstr( temp3, "\n");
				temp5=(char*)calloc( 1,sizeof(char)*strlen(temp3)-strlen(temp4)+1);
				snprintf(temp5, strlen(temp3)-strlen(temp4), "%s", temp3);
				log("\n won test 4 ==> temp5(%s)", temp5);
				
				// 组合
				memset(out_msg, 0, sizeof(out_msg));
				strcpy(out_msg, msg1);
				strcat(out_msg, temp5);				// 回溯的部分
				strcat(out_msg, temp2);
				log("\n won test 5 ==> out_msg(%s)", out_msg);
			}
		} 
	
	}
*/
}
#endif


#ifdef _CHECKFMSCHEDULE	 // Syu ADD 透过AC确认家族挑战排程
void saacproto_ACCheckSchedule_recv(int fd,char *data)
{
	extern  gmsv gs[MAXCONNECTION];
	int		i,j;
	char	filename[256];
	char	token[256];
	char	line[64];
	char	line1[64];
	char	line2[64];
	FILE*   fpm;
	easyGetTokenFromBuf(data, '|', 2, line1, sizeof(line1));
	j = atoi(data);
	snprintf( filename ,sizeof( filename ) ,"%s/%d.%s" ,
			"family",j,"fmschedule.txt");

	if( !(fpm=fopen( filename , "r")) )
	{
		log("\nSyu log open file error!!");
		sprintf(line2,"%d %s",j,"FALSE");
		for (i=0; i<MAXCONNECTION; i++)
			if (gs[i].use && gs[i].name[0])
				saacproto_ACCheckSchedule_send(i,line2);
	}
	else 
	{
		fgets( token , sizeof(token) , fpm );
		easyGetTokenFromBuf(token, '|', 2, line, sizeof(line));
		log("\nSyu test line -> %s , %s",line,line1);
		if ( atoi(line1) > atoi(line) )
		{
			sprintf(line2,"%d %s",j,"TRUE");
			for (i=0; i<MAXCONNECTION; i++)
				if (gs[i].use && gs[i].name[0])
					saacproto_ACCheckSchedule_send(i,line2);
		}
		else
		{
			sprintf(line2,"%d %s",j,"FALSE");
			for (i=0; i<MAXCONNECTION; i++)
				if (gs[i].use && gs[i].name[0])
					saacproto_ACCheckSchedule_send(i,line2);
		}
	}
}
#endif
void saacproto_ACManorPKAck_recv(int fd, char *data)
{
#ifdef __FAMILY

#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
	// 不处理
#else
	extern gmsv gs[MAXCONNECTION];
	int i;

	for (i=0; i<MAXCONNECTION; i++) 
	{
		if (gs[i].use && gs[i].name[0]) 
		{
			saacproto_ACManorPKAck_send(i, data);
		}
	}
#endif

#endif
}

#ifdef _AUCTIONEER
void saacproto_ACAuctionSold_recv(int fd, char *data)
{
  AuctionSold(data);
}
#endif

#ifdef _WAEI_KICK
void saacproto_ACKick_recv( int ti ,char* id, int lock,int mesgid )
{
	if( ti != -1 )	{
		if( !is_game_server_login(ti) ){	//MAXCONNECTION
			saacproto_ACKick_send( ti , 0 , "not login" , mesgid );
			return;
		}
	}
	sprintf(retdata, "ACTION !!");
	//"LSLOCK","KICK","DEUNLOCK","UNLOCKALL","LOCK","TYPE", "UNLOCK"
	switch( lock)	{
	case 0://LSLOCK
#ifdef _LOCK_SERVER	
		LServer_SAUNLock_send( ti, id, mesgid, 0);
#endif
		break;
	case 1:		//跨星球踢人 KICK
		{
			int i;
			for(i=0;i<MAXCONNECTION;i++)	{
				if( is_game_server_login(i) ){
					saacproto_ACKick_send( i , 1 , id , mesgid );
				}
			}
		}
		break;
	case 2:		//DEUNLOCK
		break;
	case 3:	//UNLOCKALL
		{
			extern gmsv gs[MAXCONNECTION];

			DeleteMemLockServer( gs[ti].name);
			sprintf(retdata, "GM UNLOCK ALL");
		}
		break;
	case 4: //LOCK
#ifdef _LOCK_ADD_NAME
			if ( lockUser( getGSName( ti ) , id , "" , "0" , lock , result,sizeof(result),
					retdata ,sizeof( retdata ) , "0" , "0") < 0 ) {
#else
		  if ( lockUser( getGSName( ti ) , id , "0" , lock , result,sizeof(result),
					retdata ,sizeof( retdata ) , "0" , "0") < 0 ) {
#endif
				sprintf(retdata, "LOCK USER !!");
		  }
		break;
	case 5:	//查询玩家状态 TYPE
		{
			char buf[4096];
			if (GetMemLockState(getHash(id) & 0xff, id, buf)) {
				sprintf(retdata, "USR LOCKED: %s", buf);
			} else {
				sprintf(retdata, "USR UNLOCKED: %s", buf);
			}
		}
		break;
	case 6:		//MEMORY UNLOCK; UNLOCK
		{
			char result[100], retdata[100], process[16];
			int ret = 0;
#ifdef _LOCK_ADD_NAME
					ret = lockUser(  getGSName(ti), id , "" , "0" , 0 , result , sizeof( result ) ,
                       retdata , sizeof( retdata ) , "0" , "0");
#else			
	        ret = lockUser(  getGSName(ti), id , "0" , 0 , result , sizeof( result ) ,
                       retdata , sizeof( retdata ) , "0" , "0");
#endif

			if( strcmp( result , SUCCESSFUL ) == 0 ){
				log( "UCHECK: unlock %s OK\n" , id );
			} else {
				log( "UCHECK: unlock %s FAILED\n" , id );
				return;
			}

			snprintf( process , sizeof(process) , "%d" , ret );
#ifdef _UNLOCKPOOL
			if( addUnlockPool( ti, id, "", process, mesgid) <0  ) {
#else
			if( addWorkerTask( id , "dummy" , "3" , dummyCallback , ti ,
				id , "dummy" , charname , process ,"", mesgid , 0 ) < 0 ) {
#endif
				log("Game end error, process=%s\n" , process );    
			}else	{
				log("Game end :process=%s\n" , process );
			}
		}
		break;
#ifdef _FIX_MESSAGE
	case 7:
		{
			char buf[4096];
			int i;
			if (GetMemLockState(getHash(id) & 0xff, id, buf)) {
				log("\n won test 20.1 ==>");
				// 有lock的话踢人
				for(i=0;i<MAXCONNECTION;i++)	{
					if( is_game_server_login(i) ){
						saacproto_ACKick_send( i , 1 , id , mesgid );
					}
				}			
			} else {	// 没有lock的话，送 game end					
				char result[100], retdata[100], process[16];
				int ret = 0;			
				log("\n won test 20.2 ==>");
#ifdef _LOCK_ADD_NAME
				ret = lockUser(  getGSName(ti), id , "" , "0" , 0 , result , sizeof( result ) ,
					       retdata , sizeof( retdata ) , "0" , "0");
#else
				ret = lockUser(  getGSName(ti), id , "0" , 0 , result , sizeof( result ) ,
					       retdata , sizeof( retdata ) , "0" , "0");
#endif
				snprintf( process , sizeof(process) , "%d" , ret );
#ifdef _UNLOCKPOOL
				if( addUnlockPool( ti, id, "", process, mesgid) <0  ) {
#else
				if( addWorkerTask( id , "dummy" , "3" , dummyCallback , ti ,
					id , "dummy" , charname , process ,"", mesgid , 0 ) < 0 ) {
#endif
					log("Game end error, process=%s\n" , process );    
				}else	{
					log("Game end :process=%s\n" , process );
				}

			}
		}
		break;
#endif
	case 10:	//跨星球踢人不含load该星球
		{
			int i;
			for(i=0;i<MAXCONNECTION;i++)	{
				if( is_game_server_login(i) && i != ti ){
					saacproto_ACKick_send( i , 1 , id , mesgid );
				}
			}
		}
		break;
	}
    saacproto_ACKick_send( ti, 0, retdata, mesgid );
}

#endif


#ifdef _WORKER_AUTO_LOGOUT     // WON ADD GS断线自动送LOGOUT给WORKER
void Worker_send_logout( int ti ,char* id )
{
	char result[100], retdata[100], process[16];
	int ret = 0;			

#ifdef _LOCK_ADD_NAME
	ret = lockUser(  getGSName(ti), id , "" , "0" , 0 , result , sizeof( result ) ,
                       retdata , sizeof( retdata ) , "0" , "0");
#else
	ret = lockUser(  getGSName(ti), id , "0" , 0 , result , sizeof( result ) ,
                       retdata , sizeof( retdata ) , "0" , "0");
#endif
	snprintf( process , sizeof(process) , "%d" , ret );

#ifdef _UNLOCKPOOL
	if( addUnlockPool( ti, id, "", process, -1) <0  ) {
#else
	if( addWorkerTask( id , "dummy" , "3" , dummyCallback , ti ,
		id , "dummy" , charname , process ,"", -1 , 0 ) < 0 ) {
#endif
			log("\n Game end error, process(%s)\n" , process );  
	}else	{
			log("\n Game end :process(%s)\n" , process );
	}

}
#endif


#ifdef _NEW_PLAYERGOLD
void saacproto_ACNewPlayerList_recv( int fd, char *CdKey, char *UserName, int UserIndex, int RunType)
{
	int ret=-1;
	switch( RunType){
	case 1:	//Del
		ret = DelNEWPlayerfromFile( CdKey, UserName, RunType);
		break;
	case 2: //Add
		ret = AddNEWPlayertoFile( CdKey, UserName, strlen( CdKey));
		break;
	}
	saacproto_ACNewPlayerList_send( fd, CdKey, UserName, UserIndex, ret);

}
#endif

#ifdef _DEFEND_NEWUSERS
void saacproto_DefendList_recv( int fd, int type, char *data)
{
	switch( type){
	case 1:
		DEFEND_LoadUserForList( data);
		break;
	case 2:
		DEFEND_LoadScoreForList( data);
		break;
	case 3:	//检查积分
		break;
	case 4:	//删除积分纪录
		break;
	}
}
void saacproto_CheckDefendScore_recv( int fd, int userindex, char *cdkey, char *name)
{
	if( userindex == -1 ) return;
	if( cdkey == NULL ) return;
	if( name == NULL ) return;

	DEFEND_CheckScoreForList( fd, userindex, cdkey, name);
}
#endif

#ifdef _PAUCTION_MAN
void saacproto_ACItemAuction_recv( int fd, char *ITEMNAME, char *data, int itemID, int ret, int flg)
{
	log( "ACItemAuction( %d, %s, %s, %d, %d, %d)\n", fd, ITEMNAME, data, itemID, ret, flg);
	IAData_ITEM( fd, itemID, ITEMNAME, data, ret, flg);

}
#endif

#ifdef _FIX_WORKS
void saacproto_WORKLogin_recv( int fd , char* workname, char* workpass, int work_port )
{

}
#endif


#ifdef _NEW_PLAYERGOLD
void saacproto_LoadNEWPlayer_recv( int ti , int charaindex, char* filename)
{
	char Token[256];
	if( LoadNEWPlayerListAdd( filename) <= 0 ){
		sprintf( Token, "重读新手名单-错误。");
	}else{
		sprintf( Token, "重读新手名单-完成。");
	}

	saacproto_LoadNEWPlayer_send( ti, charaindex, Token);
}
#endif

#ifdef _DEATH_CONTEND
void saacproto_PKLISTGETDATA_recv( int fd, int start, int count)
{
	log( "PKLISTGETDATA_recv( %d, %d) \n", start, count);
	PKLIST_GetPkTeamListData( fd, start, count );
}

void saacproto_PKLISTGETDATA_send( int fd, char *ret, int endti, char *data)
{
	saacproto_CreateHeader( saacproto.work , "PKLISTGETDATA" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( endti ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( ret ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}

void saacproto_PKLISTUPDATE_recv( int fd, int menum, char *mecdkey, int tonum, char *tocdkey, int winer, int flg)
{
	PKLIST_UpdatePkTeamList( fd, menum, mecdkey, tonum, tocdkey, winer, flg);
}

void saacproto_PKLISTCHARTS_recv( int fd, int type, int flg)
{
	//andy_log
	log( "PKLISTCHARTS_recv( %d, %d)\n", type, flg);
	PKLIST_TheCharts( fd, type, flg);
}

void saacproto_PKLISTCHARTS_send( int fd, char *data, int type, int flg )
{
	saacproto_CreateHeader( saacproto.work , "PKLISTCHARTS" );

	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( type ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( flg ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
/*
void saacproto_PKLISTUPDATE_send( int fd, char *ret, int mesgid, int charindex, char *data)
{
	saacproto_CreateHeader( saacproto.work , "PKLISTUPDATE" );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( mesgid ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_int( charindex ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( ret ) ,saacproto.workbufsize );
	saacproto_strcatsafe( saacproto.work , saacproto_mkstr_string( data ) ,saacproto.workbufsize );
	saacproto_Send( fd , saacproto.work );
}
*/
#endif

#ifdef _GM_BROADCAST					// WON ADD 客服公告系统
void broadcast( char *msg )
{
    extern gmsv gs[MAXCONNECTION];
	int time = 0, loop = 0, wait = 0, i = 0;
	char post_msg[10*80] = {0};

	strtok( msg, " " );	
	time = atoi( strtok( NULL, " " ) );
	loop = atoi( strtok( NULL, " " ) );
	wait = atoi( strtok( NULL, " " ) );

	for( i=0; i<10; i++ ){
		char *temp;
        temp=strtok( NULL, " " );
        if( temp ){
			strcat( post_msg, temp );
            strcat( post_msg, " " );
        }
    }

    post_msg[ strlen( post_msg ) - 1 ] = 0;

	log("\n won test 2 ==> time(%d) loop(%d) wait(%d) \n post_msg(%s)", time, loop, wait, post_msg );

	for(i=0; i<MAXCONNECTION; i++){
		if(gs[i].use && gs[i].name[0]){
				log("\n won test 2.1 ==> i(%d)", i  );
				saacproto_GmBroadcast_send(i, time, loop, wait, post_msg );
		}
	}
}
#endif

#ifdef _SEND_EFFECT		   // WON ADD 送下雪、下雨等特效
void SendEffect(char *effect)
{
    extern gmsv gs[MAXCONNECTION];
	int i;

	for (i=0; i<MAXCONNECTION; i++) {
		if (gs[i].use && gs[i].name[0]) {
				saacproto_SendEffect_send(i, effect);
		}
	}
}
#endif


#ifdef _BAD_PLAYER            // WON ADD 送坏玩家去关
void BadPlayer(void)
{
	int i;
	extern char BadPlayerList[500][20];			// 定义最大 500 名
	char *filename={"sendjail.txt"};
	FILE *fp;
	extern int MAX_BAD_PLAYER;

	memset( BadPlayerList, 0, sizeof(BadPlayerList));
	
	if( !(fp=fopen(filename, "r"))  ){
		log("\n read sendjail.txt FAIL !!");
		return;
	}

	for( i=0; i<500; i++){
		char buf[256];
		if( fscanf( fp, "%s", buf) == EOF ) break;
		buf[strlen(buf)+1] = 0;
		memcpy( BadPlayerList[i], buf, strlen(buf)+1);				
	}

	fclose( fp);
	
	// WON TEST 
	for( i=0; i<500; i++){
		if( strcmp( BadPlayerList[i], "" ) ==0 ){
			MAX_BAD_PLAYER = i;
			break;
		} 
	}

}
#endif

#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
void load_fm_pk_list()
{
	int i;
	extern char fm_pk_list[9][255];
	char *filename={"family/fm_pk_list.txt"};
	FILE *fp;	

	if( !(fp=fopen(filename, "r"))  ){
		return;
	}

	for( i=0; i<9; i++){
		char buf[256];
		if( fscanf( fp, "%s", buf) == EOF ) break;
		buf[strlen(buf)+1] = 0;
		memcpy( fm_pk_list[i], buf, strlen(buf)+1);				
	}

	fclose( fp);
	log("\n");
}

void save_fm_pk_list()
{
	int i;
	extern char fm_pk_list[9][255];
	char *filename={"family/fm_pk_list.txt"};
	FILE *fp;	

	if( !(fp=fopen(filename, "w"))  ){
		return;
	}

	for( i=0; i<9; i++){	
		fprintf(fp, "%s\n", fm_pk_list[i]);
	}

	fclose( fp);

}
#endif

#ifdef _CHAR_POOLITEM
void saacproto_ACCharInsertPoolItem_recv( int fd, char *cdkey, int userindex, int clifdid, char *Pooldataarg)
{
	if( !is_game_server_login( fd) ){
        return;
    }
	if( InsertCharPoolItem( cdkey, Pooldataarg, strlen(Pooldataarg)) <= 0 ) {
		log("\n InsertPoolItem( %s) err!!\n", cdkey);
		saacproto_ACCharSavePoolItem_send( fd , FAILED , Pooldataarg, clifdid );
	}
}

void saacproto_ACCharSavePoolItem_recv( int fd, char *cdkey, int userindex, int clifdid, char *Pooldataarg)
{
	if( !is_game_server_login( fd) ){
        return;
    }
	if( saveCharPoolItem( cdkey, Pooldataarg, strlen(Pooldataarg)) < 0 ) {
		saacproto_ACCharSavePoolItem_send( fd , FAILED , Pooldataarg, clifdid );
	}
}

void saacproto_ACCharGetPoolItem_recv( int fd, char *cdkey, int userindex, int clifdid, int npcid)
{
	char loadbuf[CHARDATASIZE];
	int ret=-1;
	if( !is_game_server_login( fd) ){
        return;
    }

	ret = loadCharPoolItemOne( cdkey , loadbuf , sizeof(loadbuf));

	if( ret == -1 ){//找不到档案
		saacproto_ACCharGetPoolItem_send( fd , FAILED, loadbuf, clifdid, npcid);
		return;
	}
	saacproto_ACCharGetPoolItem_send( fd , SUCCESSFUL, loadbuf, clifdid, npcid);
}
#endif


#ifdef _CHAR_POOLPET
void saacproto_ACCharInsertPoolPet_recv( int fd, char *cdkey, int userindex, int clifdid, char *Pooldataarg)
{
	if( !is_game_server_login( fd) ){
        return;
    }
	if( InsertCharPoolPet( cdkey, Pooldataarg, strlen(Pooldataarg)) <= 0 ) {
		log("\n InsertPoolPet( %s) err!!\n", cdkey);
		saacproto_ACCharSavePoolPet_send( fd , FAILED , Pooldataarg, clifdid );
	}
}

void saacproto_ACCharSavePoolPet_recv( int fd, char *cdkey, int userindex, int clifdid, char *Pooldataarg)
{
	if( !is_game_server_login( fd) ){
        return;
    }
	if( saveCharPoolPet( cdkey, Pooldataarg, strlen(Pooldataarg)) < 0 ) {
		saacproto_ACCharSavePoolPet_send( fd , FAILED , Pooldataarg, clifdid );
	}
}

void saacproto_ACCharGetPoolPet_recv( int fd, char *cdkey, int userindex, int clifdid, int npcid)
{
	char loadbuf[CHARDATASIZE];
	int ret=-1;
	if( !is_game_server_login( fd) ){
        return;
    }

	ret = loadCharPoolPetOne( cdkey , loadbuf , sizeof(loadbuf));

	if( ret == -1 ){//找不到档案
		saacproto_ACCharGetPoolPet_send( fd , FAILED, loadbuf, clifdid, npcid);
		return;
	}
	saacproto_ACCharGetPoolPet_send( fd , SUCCESSFUL, loadbuf, clifdid, npcid);
}
#endif



#ifdef _UNIVERSE_CHATROOM
void saacproto_ACUniChatroom_recv( int fd, char *cdkey, char *data, int userindex, int clifdid )
{
	CHATROOM_RecvAll( fd, cdkey, data, userindex, clifdid );
}

#endif


#ifdef _RECAL_BACKUP_PLAYER			// WON 回溯功能


// 取有备份的日期
void get_backup_date(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date)
{
		int filenum=-1, i;
		STRING64 filenames[256*30];
		char char_data[65500];
		char id_dir[20];

		filenum = rgetFileName( BACKUP_DIR, filenames, arraysizeof(filenames) );
		if( filenum == -1 ){
			log("\n get_backup_date ==> filenum err !!");
			return;
		}	

		sprintf(id_dir, "%x",  (getHash(id) & 0xff) );

		memset(char_data, 0, sizeof(char_data));

		// 找出符合帐号的备份日期
		for( i = 0 ; i < filenum ; i ++ ){
			if(strcmp(filenames[i].string, "") != 0 ){
				char temp[64];
				get_delim(filenames[i].string, temp, "_0x", ".tar" );
			
				if(strcmp(temp, id_dir)==0){
					char temp2[20];
					get_delim(filenames[i].string, temp, "AC_", "_0x" );
					snprintf(temp2, 11, "%s", temp);
					strcat( char_data, temp2);
					strcat( char_data, "|");
				}
			}
		}

		saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 1);
	
}

// 取得线上人物资料
void get_online_player_data(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date)
{
	FILE *fp;
	char filename[128];
	char body[128];
	char char_data[65500];
	char *c_ptr, c_temp;

    c_ptr=char_data;

	snprintf(body , sizeof(body ) , "%s.%d.char" , id , char_num );
	makeDirFilename(filename , sizeof(filename), chardir , getHash(id) ,body);

	if( !(fp=fopen(filename, "r")) ){
		log("\n open (%s) file err", filename );		
		strcpy(char_data, "无此帐号人物资料");
		saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 2);
		return; 
	}

	strcpy(char_data, "");

    do{
       c_temp = fgetc(fp);
       *c_ptr=c_temp;
       c_ptr++;
    }while(c_temp != EOF);
    *c_ptr='\0';

	fclose(fp);	

    if( char_data[0]=='|' && char_data[1]=='|' ){
		log("\n open (%s) file err", filename );		
		strcpy(char_data, "此帐号人物资料异常");
		saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 2);
		return; 
	}		
	
	// 分析人物资料，取出所需的人物资料
	analysis_char(char_data);

	saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 2);

}

void send_backup_player_data()
{
	FILE *fp;
	char filename[128];
	char char_data[65500];
	char *c_ptr, c_temp;
	int  fd=-1, userfdid=-1, GmCliId=-1, char_num=-1, date=-1, i, j, count;
	char uid[64], id[64], temp[8][64], buf[255];

	sprintf( filename, "%s", "backup_player_name2.txt");
	if( !(fp=fopen(filename, "r")) ){
		log("\n open backup_player_name2.txt err!!");
		return; 
	}

	strcpy(buf, "");
	while(1)
		if( fscanf( fp, "%s", buf) == EOF ) break;

	fclose(fp);

	for(i=0; i<8; i++)	memset(temp[i], 0, sizeof(temp[i]) );

	i=0; j=0; count=0;
	for( i=0; i<strlen(buf)+1; i++ ){
		if( buf[i] == '|' ){
			j++;	count=0;
			continue;
		}
		temp[j][count] = buf[i];
		count++;
	}

	strcpy(filename, temp[0]);	
	fd=atoi(temp[1]);
	strcpy(uid, temp[2]);
	userfdid=atoi(temp[3]);
	GmCliId=atoi(temp[4]);
	strcpy(id, temp[5]);
	char_num=atoi(temp[6]);
	date=atoi(temp[7]);

	if( (strcmp(uid,"")==0)||(userfdid==-1)||(GmCliId==-1)||(strcmp(id,"")==0)||(char_num==-1)||(date==-1) ) {		
		strcpy(char_data, "无此帐号人物资料1");
		saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 3);
		return; 	
	}

	sprintf( filename, "%s%s.%d.char", temp[0], id, char_num ); 

	if( !(fp=fopen(filename, "r")) ){
		log("\n open (%s) file err", filename );		
		strcpy(char_data, "无此帐号人物资料2");
		saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 3);
		return; 
	}
	
	c_ptr=char_data;
	strcpy(char_data, "");

    do{
       c_temp = fgetc(fp);
       *c_ptr=c_temp;
       c_ptr++;
    }while(c_temp != EOF);
    *c_ptr='\0';

	fclose(fp);	

    if( char_data[0]=='|' && char_data[1]=='|' ){
		log("\n open (%s) file err", filename );		
		strcpy(char_data, "此帐号人物资料异常");
		saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 3);
		return; 
	}		
	
	analysis_char(char_data);

	saacproto_ACRecalPlayer_send(fd, uid, userfdid, GmCliId, id, char_num, date, char_data, 3);
}

void get_backup_player_data(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date)
{
	char filename[128];
	char body[255];	

	sprintf(filename, "AC_%d*_0x%x.tar.gz", date, (getHash(id)&0xff) );

	strcpy(body, "");
	
	sprintf(body, "echo \"%s|%d|%s|%d|%d|%s|%d|%d|0x%x\" > backup_player_name.txt;  ~/saac/recal_player.sh", 
					filename, fd, uid, userfdid, GmCliId, id, char_num, date, (getHash(id)&0xff) );
	
	system(body);


}

void analysis_char(char *char_data)
{
	int i;
	char delime[64];
	extern struct PLAYER_DATA player_data;

	char_data_count=0;

	// player name 
	memset( player_data.name, 0, sizeof(player_data.name) );
	won_get(player_data.name, "", "|");

	// player information
	_won_get_player( char_data );

    // have item
	for(i=0; i<MAX_HAVE_ITEM; i++){
		sprintf(delime, "item%d=", i);
		memset( player_data.item[i], 0, sizeof(player_data.item[i]) );
		memset( player_data.item_name[i], 0, sizeof(player_data.item_name[i]) );
		won_get(player_data.item[i], delime,	"\\z");	
		if((strcmp( player_data.item[i], "")) == 0) continue;
		won_get(player_data.item_name[i], "na=",		"\\z");	
		won_get(player_data.item[i], "ucode=",	"\\z");	
	}

	// pool item
	for(i=MAX_HAVE_ITEM; i<MAX_HAVE_ITEM+MAX_POOL_ITEM; i++){
		sprintf(delime, "poolitem%d=", i);
		memset( player_data.item[i], 0, sizeof(player_data.item[i]) );
		memset( player_data.item_name[i], 0, sizeof(player_data.item_name[i]) );
		won_get(player_data.item[i], delime,	"\\z");	
		if((strcmp( player_data.item[i], "")) == 0) continue;
		won_get(player_data.item_name[i], "na=",		"\\z");	
		won_get(player_data.item[i], "ucode=",  "\\z");	
	}

	// have pet
	for(i=0; i<MAX_HAVE_PET; i++){
		sprintf(delime, "pet%d=", i);
		memset( player_data.pet[i], 0, sizeof(player_data.pet[i]) );
		memset( player_data.pet_name[i], 0, sizeof(player_data.pet_name[i]) );
		won_get(player_data.pet[i], delime,		"\\z");	
		if((strcmp( player_data.pet[i], "")) == 0) continue;
		won_get(player_data.pet[i], "lv:",		"\\z");	
		won_get(player_data.pet[i], "hp:",		"\\z");	
		won_get(player_data.pet[i], "vi:",		"\\z");	
		won_get(player_data.pet[i], "str:",		"\\z");	
		won_get(player_data.pet[i], "tou:",		"\\z");	
		won_get(player_data.pet[i], "dex:",		"\\z");	
		won_get(player_data.pet_name[i], "name:",	"\\z");	
		won_get(player_data.pet[i], "ocd:",		"\\z");	
		won_get(player_data.pet[i], "ocn:",		"\\z");	
		won_get(player_data.pet[i], "ucode:",	"\\z");	
	}

	// pool pet
	for(i=MAX_HAVE_PET; i<MAX_HAVE_PET+MAX_POOL_PET; i++){
		sprintf(delime, "poolpet%d=", i);
		memset( player_data.pet[i], 0, sizeof(player_data.pet[i]) );
		memset( player_data.pet_name[i], 0, sizeof(player_data.pet_name[i]) );
		won_get(player_data.pet[i], delime,		"\\z");	
		if((strcmp( player_data.pet[i], "")) == 0) continue;
		won_get(player_data.pet[i], "lv:",		"\\z");	
		won_get(player_data.pet[i], "hp:",		"\\z");	
		won_get(player_data.pet[i], "vi:",		"\\z");	
		won_get(player_data.pet[i], "str:",		"\\z");	
		won_get(player_data.pet[i], "tou:",		"\\z");	
		won_get(player_data.pet[i], "dex:",		"\\z");	
		won_get(player_data.pet_name[i], "name:",	"\\z");	
		won_get(player_data.pet[i], "ocd:",		"\\z");	
		won_get(player_data.pet[i], "ocn:",		"\\z");	
		won_get(player_data.pet[i], "ucode:",	"\\z");	
	}
	
	strcpy(char_data, "");

	// player name
	strcpy(char_data,"名称：");
	strcat(char_data, player_data.name);

	// player information
	strcat(char_data,"转生：");
	strcat(char_data, player_data.trn);
	strcat(char_data,"等级：");
	strcat(char_data, player_data.lv);
	strcat(char_data,"经验值：");
	strcat(char_data, player_data.exp);
	strcat(char_data,"HP：");
	strcat(char_data, player_data.hp);
	strcat(char_data,"MP：");
	strcat(char_data, player_data.mp);
	strcat(char_data,"体：");
	strcat(char_data, player_data.vi);
	strcat(char_data,"攻：");
	strcat(char_data, player_data.str);
	strcat(char_data,"防：");
	strcat(char_data, player_data.tou);
	strcat(char_data,"敏：");
	strcat(char_data, player_data.dex);
	strcat(char_data,"金钱：");
	strcat(char_data, player_data.gold);
	strcat(char_data,"银行存款：");
	strcat(char_data, player_data.bankgld);
	strcat(char_data,"伊甸个人存款：");
	strcat(char_data, player_data.pgold);
	strcat(char_data,"家族编号：");
	strcat(char_data, player_data.fmi);
	strcat(char_data,"家族名称：");
	strcat(char_data, player_data.fmname);
	strcat(char_data,"族长旗标：");
	strcat(char_data, player_data.fmlf);
	strcat(char_data,"声望：");
	strcat(char_data, player_data.fame);
	strcat(char_data,"图层：");
	strcat(char_data, player_data.fl);
	strcat(char_data,"X：");
	strcat(char_data, player_data.x);
	strcat(char_data,"Y：");
	strcat(char_data, player_data.y);

	strcat(char_data, "|");

	for(i=0; i<MAX_HAVE_ITEM+MAX_POOL_ITEM; i++){	
		strcat(char_data, "item:");
		strcat(char_data, player_data.item_name[i]);		
		strcat(char_data, player_data.item[i]);		
		strcat(char_data, "|");
	}
	for(i=0; i<MAX_HAVE_PET+MAX_POOL_PET; i++){		
		strcat(char_data, "pet:");
		strcat(char_data, player_data.pet_name[i] );	
		strcat(char_data, player_data.pet[i] );		
		strcat(char_data, "|");		
	}

}

void _won_get(char *get, char *delime1, char *delime2, char *char_data)
{	
	char temp[1024];
	memset(temp, 0, sizeof(temp));

	if( get_delim( char_data+char_data_count, temp, delime1, delime2) ){	
		strcat(get, temp);	
		strcat(get, "\n");
	}else{
		strcat(get, "");
	}
}

int get_delim(char *msg, char *temp, char *delime1, char *delime2)
{
    char *out1, *out2;
    int buffsize=0;

    if( (out1=strstr( msg, delime1)) == NULL )						  return 0;
	if( (out2=strstr( out1+strlen(delime1), delime2)) == NULL )		  return 0;		
	if( (buffsize=strlen(out1)-strlen(out2)-strlen(delime1) ) <= 0 )  return 0; 
	
	char_data_count += strlen(msg) - strlen(out2) -1;
	
	snprintf( temp, buffsize+1, out1+strlen(delime1) );

	return 1;
}

int rgetFileName( char* dirname , STRING64* string, int size)
{
    int     index=0;
    if( rrd( dirname , string ,size ,&index ) == -1 )return -1;
    else return index;
}

int rrd( char* dirname , STRING64* buf ,int bufsize, int* index)
{
    DIR*    d;
    char    dirn[1024];

    d = opendir( dirname );
    if( d == NULL )return -1;

    while( 1 ){
        struct dirent* dent;
        struct  stat    st;
        dent = readdir( d );
        if( dent == NULL ){
            if( errno == EBADF ){
                closedir(d);
                return -1;
            }else
                /*  successful */
                break;
        }

        if(  dent->d_name[0] == '.' )continue;

        snprintf(dirn, sizeof(dirn), "%s/%s" , dirname,dent->d_name );
        if( stat( dirn , &st ) == -1 )continue;
        if( S_ISDIR( st.st_mode ) ){
            if( rrd( dirn , buf ,bufsize, index) == -1 ){
                closedir(d);
                return -1;
            }
        }else{
            if( *index >= bufsize )
                break;

            strcpy( buf[*index].string , dirn );
            (*index)++;
        }
    }
    closedir(d);
    return 1;
}

void _won_get_player(char *char_data)
{

	_won_get_2( char_data, player_data.fl, "nfl=" );
	_won_get_2( char_data, player_data.x, "nx=" );
	_won_get_2( char_data, player_data.y, "ny=" );
	_won_get_2( char_data, player_data.lv, "nlv=" );
	_won_get_2( char_data, player_data.gold, "ngld=" );
	_won_get_2( char_data, player_data.hp, "nhp=" );
	_won_get_2( char_data, player_data.mp, "nmp=" );
	_won_get_2( char_data, player_data.vi, "nvi=" );
	_won_get_2( char_data, player_data.str, "nstr=" );
	_won_get_2( char_data, player_data.tou, "ntou=" );
	_won_get_2( char_data, player_data.dex, "ndx=" );
	_won_get_2( char_data, player_data.exp, "nexp=" );
	_won_get_2( char_data, player_data.trn, "ntrn=" );
	_won_get_2( char_data, player_data.fmi, "nfmi=" );
	_won_get_2( char_data, player_data.fmlf, "nfmlf=" );
	_won_get_2( char_data, player_data.bankgld, "nbankgld=" );
	_won_get_2( char_data, player_data.fame, "nfame=" );
	_won_get_2( char_data, player_data.pgold, "npersonaglod=" );
	_won_get_2( char_data, player_data.fmname, "nfmname=" );
	

}

void _won_get_2(char *char_data, char *fun, char *check)
{
	memset( fun , 0, sizeof(fun) );
	won_get( fun,  check, "\\n");	
}

#endif // end RECAL_ASK_PLAYER

#ifdef _ALLDOMAN // Syu ADD 排行榜NPC
void LOAD_herolist()
{
	FILE *fdb;
	int i, linenum=0;
	
	char filename[256], token[512];

	snprintf( filename , sizeof( filename ) ,"%s/%s" ,	"db/herolist","db_herolist.txt");
	if( !( fdb = fopen ( filename , "r+" ) ) ) {
		log("\nSyu log open db_herolist.txt error!!");
		return ; 
	}else {
#ifdef _FIX_ALLDOMAN
		int j;
		char buf1[256];
		for( i=0; i<MAX_HERO_LIST; i++) Herolist[i].use =0;
		linenum = 0 ; 
		while( fgets( token, sizeof(token)-1, fdb) ){
			if( token[0] == '\0' || token[0] == '\n' || token[0] == '#' ) continue;
			token[strlen(token)-1] = 0;
			for( i=0; i<3; i++) {
				easyGetTokenFromBuf( token, '|', i+1,
					Herolist[linenum].strings[i], sizeof( Herolist[linenum].strings[i]));
			}
			for( i=0; i<4; i++) {
				easyGetTokenFromBuf( token, '|', i+4, buf1, sizeof( buf1));
				Herolist[linenum].intdata[i] = atoi( buf1);
			}
			Herolist[linenum].use = 1;
			linenum ++;
			if( linenum >= MAX_HERO_LIST ) break;
		}
		fclose( fdb);

		for( i=0; i<MAX_HERO_LIST; i++){
			if( Herolist[i].use == 0 ) continue;
			for( j=(MAX_HERO_LIST-1); j>i; j-- ){
				if( Herolist[j].use == 0 ) continue;
				if( !strcmp( Herolist[i].strings[0], Herolist[j].strings[0]) &&
					!strcmp( Herolist[i].strings[1], Herolist[j].strings[1]) ){
					Herolist[j].use = 0;
				}
			}
		}
		for( j=(MAX_HERO_LIST-1); j>0; j-- ){
			if( Herolist[j].use == 0 ) continue;
			if( Herolist[j-1].use == 1 ) continue;

			for( i=0; i<3; i++) {
				sprintf( Herolist[j-1].strings[i], "%s", Herolist[j].strings[i]);
			}
			for( i=0; i<4; i++) {
				Herolist[j-1].intdata[i] = Herolist[j].intdata[i];
			}
			Herolist[j].use = 0;
			Herolist[j-1].use = 1;
		}
		//andy_log
		for( i=0; i<MAX_HERO_LIST; i++){
			if( Herolist[i].use == 0 ) continue;
			log( "hero[%d]:[%s,%s,%s,%d,%d,%d,%d]\n", i,
				Herolist[i].strings[0],
				Herolist[i].strings[1],
				Herolist[i].strings[2],
				Herolist[j-1].intdata[0],
				Herolist[j-1].intdata[1],
				Herolist[j-1].intdata[2],
				Herolist[j-1].intdata[3] );
		}
#else
		char *addr;
		fread ( &token , sizeof ( token ) , 1 , fdb ) ;
		fclose ( fdb ) ; 
		do {
			for ( i = 0 ; i < MAX_HERO_COLUMN ; i ++ ) {
				easyGetTokenFromBuf( token , '|', i + 1 , Herolist[ linenum ][ i ] , sizeof( Herolist[ linenum ][ i ] ) );
			}
			linenum ++;
			if ( (addr = strstr ( token , "\n" )) ) 
					strcpy ( token , addr + 1 ) ;
			else 
				break;
		} while ( 1 ) ;

#endif
	}
}

void SAVE_herolist (int fd)
{
	int i ; 
	FILE *fdb;
	char filename[256];
	snprintf( filename , sizeof( filename ) ,"%s/%s" ,	"db/herolist","db_herolist.txt");
	if( !( fdb = fopen ( filename , "w" ) ) ) {
		log("\nSyu log open db_herolist.txt error!!");
		return ; 
	}else {
#ifdef _FIX_ALLDOMAN
		for ( i=0; i<MAX_HERO_LIST; i++) {
			if( Herolist[i].use == 0 ) continue;
			fprintf( fdb , "%s|%s|%s|%d|%d|%d|%d\n" , 
				Herolist[i].strings[0],
				Herolist[i].strings[1],
				Herolist[i].strings[2],
				Herolist[i].intdata[0],
				Herolist[i].intdata[1],
				Herolist[i].intdata[2],
				Herolist[i].intdata[3] );
		}
#else
		char tmp[256];
		for ( i = 0 ; i < MAX_HERO_LIST ; i ++ ) {
			sprintf( tmp , "%s|%s|%s|%d|%d|%d|%d\n" , 
				Herolist[i][0] , 
				Herolist[i][1] , 
				Herolist[i][2] , 
				atoi (Herolist[i][3]) , 
				atoi (Herolist[i][4]) , 
				atoi (Herolist[i][5]) , 
				atoi (Herolist[i][6]) );				
			fwrite(tmp, strlen(tmp), 1, fdb );
		}
#endif
		fclose ( fdb ) ; 
	}
}

void Send_A_herolist( int fd )
{
	FILE *fdb;
	int lens=0;
	char filename[256],buf[256];
	
	
	snprintf( filename , sizeof( filename ) ,"%s/%s" ,	"db/herolist","db_herolist.txt");
	if( !( fdb = fopen ( filename , "r+" ) ) ) {
		log("\n Syu log open db_herolist.txt error!!");
		return ; 
	}else {
#ifdef _FIX_ALLDOMAN
		char token[65535], buf1[512];
		int i;
		memset( token, 0, sizeof(token));
		strcat( token, "A|,");
		for( i=0; i<=MAX_HERO_LIST; i++){
			if( Herolist[i].use == 0 ) continue;
			if( i != 0 ) strcat( token, ",");
			snprintf( buf1, sizeof(buf1)-1, "%d|%s|%s|%s|%d|%d|%d|%d|",
				i,
				Herolist[i].strings[0], Herolist[i].strings[1], Herolist[i].strings[2],
				Herolist[i].intdata[0], Herolist[i].intdata[1],
				Herolist[i].intdata[2], Herolist[i].intdata[3] );
			strcat( token, buf1);
		}
		saacproto_UpdataStele_send( fd, token); 
#else
		char token[100 * 100];
		memset( token, 0, sizeof(token));
		memset( buf, 0, sizeof(buf));
		while( fgets( buf , sizeof( buf ) , fdb )){
			buf[strlen(buf)] = 0 ;
			if( lens+strlen(buf) >= 65535 ){
				saacproto_UpdataStele_send ( fd , token );
				memset( token, 0, sizeof(token));
				lens = 0;
			}
			strcat( token, buf);
			lens += strlen( buf);
		}
		fclose ( fdb );
		if( strlen( token) > 0 )
			saacproto_UpdataStele_send ( fd , token ); 
#endif
	}
}

#ifdef _FIX_ALLDOMAN
void Send_S_herolist( int ti )
{
	int i;
	char token[512];
    extern gmsv gs[MAXCONNECTION];
	memset( token, 0, sizeof(token));
	strcat( token, "O|,");
	for (i = 0 ; i < MAXCONNECTION ; i ++ ) {
		if ( gs[i].use && gs[i].name[0] ) {
			snprintf( token, sizeof(token)-1, "%d|%s|%s|%s|%d|%d|%d|%d|",
				ti,
				Herolist[ti].strings[0], Herolist[ti].strings[1], Herolist[ti].strings[2],
				Herolist[ti].intdata[0], Herolist[ti].intdata[1],
				Herolist[ti].intdata[2], Herolist[ti].intdata[3] );
			saacproto_UpdataStele_send ( i, token); 
		}
	}		
}
#else
void Send_S_herolist( char *ocdkey , char *oname , char *ncdkey , char *nname , 
					 char *title , int level , int trns , int floor ) 
{
	int i;
    extern gmsv gs[MAXCONNECTION];

	for (i = 0 ; i < MAXCONNECTION ; i ++ ) {
		if ( gs[i].use && gs[i].name[0] ) {
			saacproto_S_UpdataStele_send( i , ocdkey , oname ,
				ncdkey , nname , title
				, level , trns , floor );
			log("\nSyu log AC Send Single to Gmsv ");
		}
	}		
}
#endif
#endif


#define MAXUNlockM 50
#define delaytime (60*3)
UNLockMenus UNlockM[MAXUNlockM];

int UNlockM_Init( void)
{
	int i;
	for( i=0; i<MAXUNlockM; i++)	{
		memset( UNlockM[i].PlayerId, 0, sizeof( UNlockM[i].PlayerId));
		UNlockM[i].use = 0;
		UNlockM[i].time = 0;
	}

	return 1;
}
void reset_UNlockMPlayer( int ti)
{
	memset( UNlockM[ti].PlayerId, 0, sizeof( UNlockM[ti].PlayerId));
	UNlockM[ti].use = 0;
	UNlockM[ti].time = 0;
}

int UNlockM_isBe( char *id)
{
	int i;
	for( i=0; i<MAXUNlockM; i++)	{
		if( UNlockM[i].use == 0 )continue;
		if( !strcmp( UNlockM[i].PlayerId, id ) ) return 1;
	}
	return 0;
}

//andy add 2002/06/20
int UNlockM_addPlayer( char *id)
{
	int i, post=-1;
	
	for( i=0; i<MAXUNlockM; i++)	{
		if( UNlockM[i].use == 0 ){
			post = i;
			continue;
		}
		if( !strcmp( UNlockM[i].PlayerId, id ) ){
			reset_UNlockMPlayer( i);
			return -1;
		}
	}
	if( post == -1 )return -1;

	memset( UNlockM[post].PlayerId, 0, sizeof( UNlockM[post].PlayerId));
	sprintf( UNlockM[post].PlayerId, "%s", id);
	UNlockM[post].use = 1;
	UNlockM[post].time = time(NULL)+delaytime;

	return post;
}

//andy add 2002/06/20
int UNlockM_UnlockPlayer( void)
{
	int i, nums=0;
	static int UnlockClock = 0;

	if( (++UnlockClock)<60 != 0 ) return 0;

	//log(" UNlockM_UnlockPlayer ");

	UnlockClock =0;
	for( i=0; i<MAXUNlockM; i++)	{
		if( !UNlockM[i].use ) continue;
		if( UNlockM[i].time >= time(NULL) ) continue;
		if( isLocked( UNlockM[i].PlayerId ) ){
			log( "等待解锁玩家: %s 还需锁定!!\n", UNlockM[i].PlayerId);
		}else{
			log( "等待解锁玩家: %s 已经解锁!!\n", UNlockM[i].PlayerId);
		}
		reset_UNlockMPlayer( i);
		nums++;
	}
	log( "等待解锁玩家: 总计 %d 个用户解锁 !!\n", nums);
	return nums;
}


#ifdef _ANGEL_SUMMON

extern int saveMissionTable( void );

//void saacproto_ACMissionTable_recv( int fd, int num, int type, char *data, int charaindex)
void saacproto_ACMissionTable_recv( int fd, int num, int type, char *data, char* angelinfo)
{
	extern  gmsv gs[MAXCONNECTION];
	int i;
	char buf[1024];

	if( type == 1 ) { // ask data list
		if( num == -1) { // ask all data
			char alldata[MAXMISSIONTABLE*100];
			alldata[0]='\0';
			for( i =0; i <MAXMISSIONTABLE; i++) {
				if( missiontable[i].flag == MISSION_NONE)
					continue;
				sprintf( buf, "%d|%s|%s|%d|%d|%d|%d ",
					i,
					missiontable[i].angelinfo,
					missiontable[i].heroinfo,
					missiontable[i].mission,
					missiontable[i].flag,
					missiontable[i].time,
					missiontable[i].limittime );
				strcat( alldata, buf);
			}
			saacproto_ACMissionTable_send( fd, -1, 1, alldata, "");
			return;
		}
		/*
		else { // ask one data
			sprintf( buf, "%d|%s|%s|%d|%d|%d ",
				num,
				missiontable[num].angelinfo,
				missiontable[num].heroinfo,
				missiontable[num].mission,
				missiontable[num].flag,
				missiontable[num].time );
			saacproto_ACMissionTable_send( fd, 1, 1, buf, "");
			return;
		}
		*/
	}
	else if( type == 2 ) { // add data
		int empty =-1;

		log("\n Add_MissionTable:%s \n", data);
		for( i =0; i <MAXMISSIONTABLE; i++) {
			if( missiontable[i].angelinfo[0] == NULL ) {
				empty = i;
				break;
			}
		}
		if( empty == -1) {
			saacproto_ACMissionTable_send( fd, -1, 2, "", angelinfo);
			return;
		}

		easyGetTokenFromBuf( data, '|', 1, buf, sizeof( buf));
		strcpy( missiontable[empty].angelinfo, buf);
		easyGetTokenFromBuf( data, '|', 2, buf, sizeof( buf));
		strcpy( missiontable[empty].heroinfo, buf);
		easyGetTokenFromBuf( data, '|', 3, buf, sizeof( buf));
		missiontable[empty].mission = atoi( buf);
		easyGetTokenFromBuf( data, '|', 4, buf, sizeof( buf));
		missiontable[empty].flag = atoi( buf);
		//easyGetTokenFromBuf( data, '|', 5, buf, sizeof( buf));
		//missiontable[empty].time = atoi( buf);
		missiontable[empty].time = (int)time(NULL);
		missiontable[empty].limittime = 1;

		saveMissionTable();

		sprintf( buf, "%d|%s|%s|%d|%d|%d|%d ",
			empty,
			missiontable[empty].angelinfo,
			missiontable[empty].heroinfo,
			missiontable[empty].mission,
			missiontable[empty].flag,
			missiontable[empty].time,
			missiontable[empty].limittime );

		for( i=0; i<MAXCONNECTION; i++) {
			if (gs[i].use && gs[i].name[0]) {
				saacproto_ACMissionTable_send( i, 1, 1, buf, "");
			}
		}

		saacproto_ACMissionTable_send( fd, 0, 2, "", angelinfo);

		return;
	}
	else if( type == 3 ) { // del data

		delMissionTableOnedata( num);

		saveMissionTable();

		return;
	}
	else if( type == 4 ) { // change data flag

		char angelinfo[64];
		int limittime;

		log("\n Add_MissionTable:%s:%d \n", data, num);

		if( num == MISSION_DOING ) {
			easyGetTokenFromBuf( data, '|', 1, buf, sizeof( buf));
			strcpy( angelinfo, buf);
			easyGetTokenFromBuf( data, '|', 2, buf, sizeof( buf));
			limittime = atoi( buf);
		}
		else {
			strcpy( angelinfo, data);
		}

		for( i =0; i <MAXMISSIONTABLE; i++) {
			if( missiontable[i].angelinfo[0] == NULL )
				continue;
			if( strcmp( missiontable[i].angelinfo, angelinfo)
					&& strcmp( missiontable[i].heroinfo, angelinfo) )
				continue;
			
			missiontable[i].flag = num;
			
			missiontable[i].time = time(NULL);
			if( num == MISSION_DOING ) {
				missiontable[i].limittime = limittime;
			}

			saveMissionTable();
			
			sprintf( buf, "%d|%s|%s|%d|%d|%d|%d ",
				i,
				missiontable[i].angelinfo,
				missiontable[i].heroinfo,
				missiontable[i].mission,
				missiontable[i].flag,
				missiontable[i].time,
				missiontable[i].limittime );
			for( i=0; i<MAXCONNECTION; i++) {
				if (gs[i].use && gs[i].name[0]) {
					saacproto_ACMissionTable_send( i, 1, 1, buf, "");
				}
			}
			return;
		}
	}
}

#endif

#ifdef _TEACHER_SYSTEM
void saacproto_ACCheckCharacterOnLine_recv( int fd, int charaindex, char *id, char *name, int flag)
{
	LockNode *ln = userlock[getHash(id) & 0xff];
	// 须要确认名字
	if(name != NULL){
		while(ln != NULL){
			if(ln->use != 0){
				if(strcmp(ln->cdkey,id) == 0 && strcmp(ln->name,name) == 0){
					saacproto_ACCheckCharacterOnLine_send(fd,charaindex,1,ln->server,flag);
					return;
				}
			}
			ln = ln->next;
		}
		// 玩家不在线上
		saacproto_ACCheckCharacterOnLine_send(fd,charaindex,0,"",flag);
	}
	// 不须要确认名字
	else{
		while(ln != NULL){
			if(ln->use != 0){
				if(strcmp(ln->cdkey,id) == 0){
					saacproto_ACCheckCharacterOnLine_send(fd,charaindex,1,ln->server,flag);
					return;
				}
			}
			ln = ln->next;
		}
		// 玩家不在线上
		saacproto_ACCheckCharacterOnLine_send(fd,charaindex,0,"",flag);
	}
}
#endif

#ifdef _RACEMAN
//racetype  0:登记 9:删除 1~5:读资料 11~55:读资料
void saacproto_ACRaceRecordandSort_recv( int fd, int charaindex, char *code , char *id, char *name , int racetype, int catchcnt, int ranknum )
{
	//比较目前的排行 如果可以进入排行榜则写入  并且回传目前的排行榜
	FILE *fp;
	char rankfile[64];
	char allfile[64];
	char line[512];
	int  i,j,findflag = 0;
	char buf[64];
	char data[4096];  //看server要多少笔就塞多少资料

	//time_t endtime = (time_t)limittime;
	//struct tm *limit;
		
	//time( &endtime );
	//limit = localtime( &endtime );
	//limit->tm_year + 1900 , limit->tm_mon + 1 , limit->tm_mday );
	//now->tm_hour , now->tm_min , now->tm_sec
	//USERLOG( id , "ranktable W: successful\n" );
	data[0]='\0';
	memset( ranktable, 0, sizeof(ranktable)); 

	sprintf( rankfile , "race/%s.txt", code );

	fp = fopen( rankfile, "a+");	
	if( fp == NULL ){
		log("create file:%s error !!\n", rankfile);
		return;
	}
	if( fseek( fp , 0, SEEK_SET ) == -1 ){
        fprintf( fp ,"Seek Error\n" );
        fclose(fp);
        return;
    }

	i = 0;
	while(1) 
	{
		if(fgets( line, sizeof( line), fp) == NULL)	break;
		chop(line);
		easyGetTokenFromBuf( line, '|', 1, buf, sizeof( buf ));
		sprintf( ranktable[i].id, "%s", buf);
		easyGetTokenFromBuf( line, '|', 2, buf, sizeof( buf ));
		sprintf( ranktable[i].name,"%s", buf);
		easyGetTokenFromBuf( line, '|', 3, buf, sizeof( buf ));
		ranktable[i].catchcnt = atoi(buf);
		i++;		
	}
	fclose(fp);

	if(racetype == 0){
		if(catchcnt >= 230 ){
			sprintf( allfile , "race/%s_a.txt", code );
			fp = fopen( allfile , "a+");
			fprintf(fp,"%s|%s|%d|\n",id,name,catchcnt);
			fclose(fp);
		}
		//处理排名
		fp = fopen( rankfile, "w");
		if( fp == NULL ){
			log("create file:%s error !!\n", rankfile);
			return;
		}
		for(i=0; i < DEFMAXRANK ;i++)	
		{

			if( catchcnt > ranktable[i].catchcnt){   //只有大於才能把人家挤掉  
				for( j=DEFMAXRANK ; j>i+1 ; j-- )
				{
					strcpy(ranktable[j-1].id , ranktable[j-2].id);
					strcpy(ranktable[j-1].name , ranktable[j-2].name);
					ranktable[j-1].catchcnt = ranktable[j-2].catchcnt;
				}
				strcpy( ranktable[i].id , id);
				strcpy( ranktable[i].name , name);
				ranktable[i].catchcnt = catchcnt;
				catchcnt = 0;
			}else{
				if(strcmp(id,ranktable[i].id)==0 && strcmp(name,ranktable[i].name)==0) continue;
			}

			if(ranktable[i].catchcnt == 0) break;

			fprintf(fp,"%s|%s|%d|\n",ranktable[i].id , ranktable[i].name , ranktable[i].catchcnt);
		}
		fclose(fp);	
		
	}else if(racetype == 9){	//清除某笔资料

		fp = fopen( rankfile, "w");
		if( fp == NULL ){
			log("create file:%s error !!\n", rankfile);
			return;
		}
		for(i=0; i < DEFMAXRANK ;i++)	
		{
			if(ranktable[i].catchcnt == 0) break;

			if(strcmp(id,ranktable[i].id)==0 && strcmp(name,ranktable[i].name)==0) continue;

			fprintf(fp,"%s|%s|%d|\n",ranktable[i].id , ranktable[i].name , ranktable[i].catchcnt);
		}
		fclose(fp);	

	}else{	//server要资料而已
		for(i=0;i<ranknum;i++)
		{ 
			buf[0]='\0';
			if(ranktable[i].catchcnt==0) break;
			sprintf(buf,"%s|%s|%d|",ranktable[i].id , ranktable[i].name,ranktable[i].catchcnt);
			strcat(data,buf);
		}
	}

	//最後再送给server
	saacproto_ACRaceRecordandSort_send( fd, charaindex , racetype , data );
}

void saacproto_ACRaceRecordfmdo_recv( int fd , int charaindex , int fmid , int bbi , char *unicode , char *petname  )
{
	FILE *fp;
	char fmfile[64];

	sprintf(fmfile,"race/family/%d.txt",fmid);
	fp = fopen( fmfile, "a+");
	if( fp ){
		fprintf(fp,"%d|%s|%s|\n",bbi,petname,unicode);
		fclose(fp);
	}
	else{
		log("\nOpen %s File Error!!",fmfile);
	}
}
#endif 

/*
		while(1)
		{
			if(fgets( line, sizeof( line), fp) == NULL)	break;
			chop(line);
			easyGetTokenFromBuf( line, '|', 1, buf, sizeof( buf ));
			if(strcmp(id,buf)==0){
				easyGetTokenFromBuf( line, '|', 2, buf, sizeof( buf ));
				if(strcmp(name,buf)==0){
					easyGetTokenFromBuf( line, '|', 3 , buf, sizeof( buf ));
					if( catchcnt > atoi(buf)){
						
						findflag = 1;
						break;
					}
				}
			}			
		}
		if( findflag == 0 ){
			fprintf(fp,"%s|%s|%d|\n",id,name,catchcnt);
		}
		fclose(fp);

*/
