#define _MAIN_C_

#ifdef _SASQL1
#include "sasql.h"
#endif


#include "main.h"
#include "util.h"
#include "mail.h"
#include "db.h"
#include "saacproto_util.h"
#include "saacproto_serv.h"
#ifdef _UNIVERSE_CHATROOM
#include "chatroom.h"
#endif
// CoolFish: Family 2001/5/9
#include "acfamily.h"
#include "version.h"

#ifdef _DEATH_CONTEND
#include "deathcontend.h"
#endif

#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>
#include <getopt.h>
#include <stdio.h>
#include <malloc.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "saacproto_work.h"
#ifdef _OACSTRUCT_TCP
#include "saacproto_oac.h"
#endif
#ifdef _PAUCTION_MAN
#include "auction.h"
#endif
#include "lock.h"
#define BACKLOGNUM 5


#ifdef _FIX_WORKS
#include "saacproto_work.h"
int worksockfd;
#endif

#ifdef _LOCK_SERVER
#include "saacproto_lserver.h"
#endif

#ifdef _SEND_EFFECT				  // WON ADD 送下雪、下雨等特效 
#include "recv.h"
#endif

#include "defend.h"

#include "char.h"

#ifdef _SASQL
#include "sasql.h"
#endif

#ifndef _FIX_WORKS

struct membuf
{
    int use;
    char buf[512];
//    char buf[1024*128];
    int len;
    int next;
};

struct connection
{
    int use;    
    int fd;       
    int mbtop_ri;
    int mbtop_wi;
    struct sockaddr_in remoteaddr;
    int closed_by_remote;
};
#else
#endif

struct membuf *mb;
int mbsize;
int mbuse ;

int mainsockfd;             /* accept 及  域娄醒卞中木月 */
struct sockaddr_in localaddr;       /* bind 允月失玉伊旦 */
    

struct connection *con;        /* 戊生弁扑亦件     */

static int mb_finder=0;              /* mb及坞五毛腹绸允月凶户及
                               腹绸玄永皿及匏  筏盛迕 */
// WON FIX
char tmpbuf[65536];
//char tmpbuf[65536*3];         /* read迕 */

struct timeval select_timeout;

time_t sys_time =0; // Robin add

extern gmsv gs[MAXCONNECTION];

int tcpstruct_init( char *addr, int port, int timeout_ms, int mem_use, int deb);
int tcpstruct_accept1( void );
int tcpstruct_accept( int *tis , int ticount );
int tcpstruct_close( int ti );
int tcpstruct_read( int ti , char *buf , int len );
int tcpstruct_readline( int ti , char *buf , int len ,int k ,int r);
int tcpstruct_readline_chop( int ti , char *buf, int len );
int tcpstruct_write( int ti , char *buf , int len );
int tcpstruct_countmbuse( void );
int tcpstruct_connect( char *addr , int port );
#ifndef _FIX_WORKS
void set_nodelay( int sock );


#define OK     0        /* 岳   */
#define TCPSTRUCT_ENOMEM -1       /* malloc 撩   */
#define TCPSTRUCT_ESOCK -2        /* socket 撩   */
#define TCPSTRUCT_EBIND -3        /* bind 撩   */
#define TCPSTRUCT_ELISTEN -4      /* listen 撩   */
#define TCPSTRUCT_EBUG -6         /* 田弘匹丐月 */
#define TCPSTRUCT_EINVCIND -7     /* con尺及index互云井仄中方 */
#define TCPSTRUCT_EREADFIN -8     /* read 允月犯□正互卅仁化 closed by remote */
#define TCPSTRUCT_EHOST -9        /* gethostbyname 撩   */
#define TCPSTRUCT_ECONNECT -10    /* connect 撩   */
#define TCPSTRUCT_ECFULL -11      /* con 互中匀天中 */
#define TCPSTRUCT_ETOOLONG -12    /* 垫互卅互允亢 */
#define TCPSTRUCT_EMBFULL -13     /* mb 互中匀天中  */
#define TCPSTRUCT_ECLOSEAGAIN -14 /* close 互2荚今木凶 */
#endif


int port;               /* 必□丞扔□田□互涛粮仄化仁月禾□玄 */
int Total_Charlist;
int Expired_mail;
int Del_Family_or_Member;
int Write_Family;
#ifdef _NEW_PLAYERGOLD
int New_PlayerGold;
#endif

// Nuke start
char *chartime()
{
    static char buf[80];
    time_t t;
    t=time(0);
    strcpy(buf,ctime(&t));
    buf[strlen(buf)-1]=0;
    return(buf);
}

/*
  sigaction白弁
 */
void sighandle( int a )
{
    if (a==SIGUSR1) log("sigusr1信号!\n");

	if (a==SIGUSR2) {
		sigusr2( a);
		log("sigusr2信号!\n");
		return;
	}
	#ifdef _SQL_BACKGROUND
      if(!sasql_initonline())
      	log("\n数据库保存失败！\n");
  #endif
    log("得到一个信号! 异常中断......\n" );
    exit(1);
}

void sigusr2(int a);

// Arminius 7.20 memory unlock
void sigusr1(int a)
{
  int i;
  FILE *f;
  char key[4096],buf[4096];

  signal(SIGUSR1, sigusr1);
  
  f = fopen("./unlock.arg", "r");

  if (f) {
    memset(key, 0, 4096);
    fread(key, 4096, 1, f);
    for (i=0; i<strlen(key); i++) if (key[i]=='\n') key[i]='\0';

    switch (key[0]) {
    case 'P':	// unlock player
      if (DeleteMemLock(getHash(&key[1]) & 0xff,&key[1],&i)) {
        log("ADM: memunlock: %s success.\n", key);
      } else {
        log("ADM: memunlock: %s failed.\n", key);
      }
    break;
    case 'S':	// unlock server
      DeleteMemLockServer(&key[1]);
      log("ADM: memunlock: %s\n", key);
    break;
    case 'C':	// check player lock
      GetMemLockState(getHash(&key[1]) & 0xff, &key[1], buf);
      sprintf(key, "echo \"%s\" > ./sigusr1.result", buf);
      system(key);
    break;
#ifdef _CK_ONLINE_PLAYER_COUNT    // WON ADD 计算线上人数
	case 'W':	
      PlayerCount();
	break;
#endif

#ifdef _SEND_EFFECT		   // WON ADD 送下雪、下雨等特效
	case 'E':	
		log("\nAC 向 GS 发送下雪特效!!\n");
	    SendEffect(&key[1]);
	break;
#endif

#ifdef _BAD_PLAYER            // WON ADD 送坏玩家去关
	case 'B':
		log("\nAC 发送坏玩家监狱!!\n");
		BadPlayer();
	break;
#endif

#ifdef _RECAL_BACKUP_PLAYER			// WON 回溯功能
	case 'U':
		log("\n发送备份人物资料");
		// 得到备份人物资料
		send_backup_player_data();
	break;
#endif
	
#ifdef _GM_BROADCAST					// WON ADD 客服公告系统
	case 'G':
		log("\n won test 1 ==> _GM_BROADCAST" );
		broadcast(key);
	break;
#endif

#ifdef _KILLDEADWORKER	// 将所有Worker初始化
	case 'K':
		log("\nKILL ALL WORKER!!!!!\n");
		initWorkerPort();
	break;
#endif

	case 'L':  // Robin 列出所有Server连线
		log("\nList All Server Conncet!!!!!\n");
		for( i =0; i <MAXCONNECTION; i++)
			if( gs[i].use)
				log("\n gs[%d] fd:%d name:%s ", i, gs[i].fd, gs[i].name );
	break;

    }
	log(" sigusr1_over_1 ");
	fclose(f);
	log(" sigusr1_over_2 ");

  }
}

static int netWrite( int ti , char *buf , int len)
{
    return tcpstruct_write( ti , buf, len );
}

#ifndef _FIX_WORKS
typedef enum
{
    WKSTAT_IDLE = 0,            /* 窒手仄化卅中橇谪 */
    WKSTAT_WAITING = 2          /* 伐□市井日及忒蚕毛引匀化月橇谪 */
} WKSTAT;

struct worker
{
    int use;
    int port;
    int ti;
    int request_ti;
    WKSTAT status;
    time_t last_query;
    void (*callback)( int ti , int result , char *cparam0 , char *cparam1,
                      char *cparam2 , char *cparam3, char *cparam4, 
                      int iparam1 , int iparam2 );
#define CPARAMNUM 5
    char cparam[CPARAMNUM][CHARDATASIZE];
#define IPARAMNUM 3    
    int iparam[IPARAMNUM];
};

struct worker wk[MAXWK];
#endif
#ifndef _FIX_WORKS
static void addWorkerPort( int port )
{
    int i;
    for(i=0;i<MAXWK;i++){
        if( wk[i].use == 0 ){
            wk[i].use = 1;
            wk[i].port = port;
            wk[i].ti = -1;
            return;
        }
    }
    exit(1);
}
#endif

int worker_finder = 0;

static void initWorkerPort( void )
{
    int i;
    bzero( &wk , sizeof( wk ));
    for(i=0;i<MAXWK;i++){
#ifdef _FIX_WORKS
		wk[i].use = 0;
		wk[i].status = WKSTAT_IDLE;
		if( wk[i].fd < 0 )
			close( wk[i].fd);
		wk[i].fd = -1;
		wk[i].last_query = -1;
#else
        wk[i].status = WKSTAT_IDLE;
#endif
    }
	worker_finder = 0;
}

static void connectToWorker( void )
{
#ifndef _FIX_WORKS
    int i;
    for(i=0;i<MAXWK;i++){
        if( wk[i].use ){
            int ti = tcpstruct_connect( "127.0.0.1" , wk[i].port );
            if( ti >= 0 ){
                wk[i].ti = ti;
            } else {
                exit(1);
            }
        }
    }
#endif
}

static int countWorker( void );

// Nuke
int addWorkerTask( char *id , char *pas , char *flag,
                   void (*callbk)( int ti , int result ,
                                   char *cp0 , char *cp1 , char *cp2 ,
                                   char *cp3 , char *cp4, int i0 , int i1) ,
                   int ti , 
                   char *cparam0 , char *cparam1 ,
                   char *cparam2 , char *cparam3 , char *cparam4 , 
                   int iparam0 , int iparam1)
{

    int i;
    // Spock +1 2000/11/2
    char *process = cparam3;
    for(i=0;i<MAXWK;i++){
        int n;
        worker_finder ++;
        if( worker_finder >= MAXWK ) worker_finder = 0;
        n = worker_finder;
        if( wk[n].use && wk[n].status == WKSTAT_IDLE ){
            char s[256];

			log("使用工作站:%d \n", n);

            // Nuke
            {
            	int i;
            	for (i=0;id[i];i++) if (id[i]==' ') return -1;
            }

	    // Spock 2000/11/2
            snprintf( s , sizeof( s ) , "%s %s %s %s\n", id , pas , flag , process);
	    // Spock end
#ifdef _FIX_WORKS
			if( cWork_write( wk[n].ti, s, strlen(s)) <= 0 )	{
#else
            if( tcpstruct_write( wk[n].ti , s , strlen(s)) != strlen(s) ){
#endif
                return -1;
            } else {
            }

            snprintf( wk[n].cparam[0] , CHARDATASIZE , "%s", cparam0 );
            snprintf( wk[n].cparam[1] , CHARDATASIZE , "%s", cparam1 );
            snprintf( wk[n].cparam[2] , CHARDATASIZE , "%s", cparam2 );
            snprintf( wk[n].cparam[3] , CHARDATASIZE , "%s", cparam3 );
            snprintf( wk[n].cparam[4] , CHARDATASIZE , "%s", cparam4 );
            wk[n].iparam[0] = iparam0;
            wk[n].iparam[1] = iparam1;
            wk[n].status = WKSTAT_WAITING;
            wk[n].callback = callbk;
            wk[n].request_ti = ti ;
            wk[n].last_query = time(NULL);
			
			return 0;
        }
    }
    log( "工作站繁忙或工作站停止工作. 活动中的工作站有:%d\n", countWorker() );
    return -1;
}

static int countWorker( void )
{
    int i,c=0;
    for(i=0;i<MAXWK;i++){
        if( wk[i].use )c++;
    }
    return c;
}

#ifndef _FIX_WORKS
static void checkWorkerClosed( int ti )
{
    int i;
    for(i=0;i<MAXWK;i++){
        if( wk[i].ti == ti ){
            wk[i].use = 0;
            wk[i].ti = -1;
        }
    }
}
#endif
static void doWorker( int ti , char *m )
{
    int i;
    int res=-1;

    char deadline[128];

#ifdef _TIMEOUTKICK
	//log("工作站:%s ", m);
	// Nuke 20040519: Protect from error from worker
	if (m) 
	{
		char *p1,*p2;
		p1=strtok(m," ");
		p2=strtok(0," ");
		
		if (p1)
			res = atoi( p1 );
		else
			res=-1;
		
		if (p2)
			strcpy( deadline, p2 ); // 取得 deadline
		else {
			strcpy( deadline, "0" );
			//log("\n 工作站回传消息:%d:%s ", ti, p1);
		}
	}
	else
	{
		res=-1;
		strcpy( deadline, "0" );
	}
#else
	res = atoi( m );
#endif

#ifdef _NO_DOUBLECHECK
	res =0;
#endif
    for( i=0; i<MAXWK; i++){
        if( wk[i].use && wk[i].ti == ti ){
            if( wk[i].status == WKSTAT_WAITING ){
					if( wk[i].callback ){

#ifdef _TIMEOUTKICK
						strcpy( wk[i].cparam[4] ,deadline);
#endif
						// Robin log
						//log(" wk[%d] c0:%s c1:%s c2:%s c3:%s c4:%s i0:%d i1:%d ",
						//		wk[i].request_ti, wk[i].cparam[0], wk[i].cparam[1],
						//		wk[i].cparam[2], wk[i].cparam[3], wk[i].cparam[4],
						//		wk[i].iparam[0], wk[i].iparam[1]);

						wk[i].callback( wk[i].request_ti , res,
										wk[i].cparam[0] ,
										wk[i].cparam[1] ,
										wk[i].cparam[2] ,
										wk[i].cparam[3] ,
										wk[i].cparam[4] ,
										wk[i].iparam[0] ,
										wk[i].iparam[1] );
					} else {
						log( "工作站: 漏洞! callback 不能初始化\n");
					}
				wk[i].status = WKSTAT_IDLE;
            }
        }
    }
}

static int isWorker( int ti )
{
#ifndef _FIX_WORKS
    int i;
    for(i=0;i<MAXWK;i++){
        if( wk[i].use && wk[i].ti == ti )return 1;
    }
#endif
    return 0;
}

gmsv gs[MAXCONNECTION];

int login_game_server( int ti , char *svname , char *svpas ,
               char *result , int resultlen ,
               char *retdata , int retdatalen )
{
    if( strcmp( svpas , svpass ) == 0 ){
        log( "服务器密码正确 %s\n" , svname );
    } else {
        log( "服务器密码错误 %s\n" , svname );
        snprintf( result , resultlen , FAILED );
        snprintf( retdata , retdatalen , "bad passwd" );
        return 0;
    }
    {
        int i;
        for(i=0;i<MAXCONNECTION;i++){
            if( gs[i].use &&
                strcmp( gs[i].name , svname ) == 0 ){
                snprintf( result, resultlen, FAILED );
                snprintf( retdata , retdatalen, "duplicate login" );
                return 0;
            } 
        }
    }
    snprintf( gs[ti].name , sizeof(gs[ti].name), "%s" , svname );
	gs[ti].fd = ti;
    snprintf( result , resultlen ,SUCCESSFUL );
    snprintf( retdata , retdatalen , "Nothing special" );
    DeleteMemLockServer(svname);	// Arminius 7.31 unlock server
    return 0;
}

int logout_game_server( int ti )
{

    gs[ti].use = 0;
	gs[ti].fd = -1;
    gs[ti].name[0] = 0;
    tcpstruct_close( ti );
    return 0;
}
int is_game_server_login( int ti )
{
    return gs[ti].use;
}

static int readConfig( char *path )
{
    char buf[2048];
    FILE *fp;

    fp = fopen( path , "r" );
    if( fp == NULL ){ return -2; }

    while( fgets( buf , sizeof( buf ) , fp )){
        char command[1024];
        char param[2048];
        chop(buf);
        
        easyGetTokenFromString( buf , 1 , command , sizeof( command ));
        easyGetTokenFromString( buf , 2 , param , sizeof( param ));

        if( strcmp( command , "port" ) == 0 ){
            port = atoi( param );
        		log( "端口:%d\n",port );
        } else if( strcmp( command , "logdir" ) == 0 ){
            snprintf( logdir , sizeof( logdir) , param );
            log( "日志目录:%s\n",logdir );
        } else if( strcmp( command , "lockdir" ) == 0 ){
            snprintf( lockdir , sizeof( lockdir) , param );
            log( "锁定目录:%s\n",lockdir );
        } else if( strcmp( command , "chardir" ) == 0 ){
            snprintf( chardir , sizeof( chardir) , param );
            log( "档案目录:%s\n",chardir );
#ifdef _SLEEP_CHAR
			snprintf( sleepchardir , sizeof( sleepchardir), "%s_sleep", chardir);
						log( "睡眠目录:%s\n",sleepchardir );
#endif
        } else if( strcmp( command , "pass" ) == 0 ){
            snprintf( svpass , sizeof( svpass ) , param );
            log( "密码:%s\n",svpass );
        } else if( strcmp( command , "wklogdir" ) == 0 ){
            snprintf( wklogdir , sizeof( wklogdir) , param );
            log( "工作日志:%s\n",wklogdir );
        } else if( strcmp( command , "dbdir" ) == 0 ){
            snprintf( dbdir , sizeof( dbdir) , param );    
            log( "数据目录:%s\n",dbdir );        
        } else if( strcmp( command, "rotate_interval" ) == 0 ){
            log_rotate_interval = atoi( param );
            log( "日志循环间隔:%d\n",log_rotate_interval ); 
        } else if( strcmp( command, "maildir" ) == 0 ){
            snprintf( maildir, sizeof( maildir ), param );
            log( "邮件目录:%s\n",maildir );
#ifdef	__FAMILY
        // CoolFish: Family 2001/5/9
        } else if( strcmp( command, "familydir" ) == 0 ){
            snprintf( familydir, sizeof( familydir ), param );
            log( "家族目录:%s\n",familydir );
        } else if( strcmp( command, "fmpointdir" ) == 0 ){
            snprintf( fmpointdir, sizeof( fmpointdir ), param );
            log( "庄园表列:%s\n",fmpointdir );
        } else if( strcmp( command, "fmsmemodir" ) == 0 ){
            snprintf( fmsmemodir, sizeof( fmsmemodir ), param );
            log( "家族备份:%s\n",fmsmemodir );
#endif
#ifdef _LOCK_SERVER
        } else if( strcmp( command, "SANAME" ) == 0 ){
            snprintf( SaName, sizeof( SaName ), param );
            log( "家族备份:%s\n",SaName );
        } else if( strcmp( command, "LSADDRESS" ) == 0 ){
            snprintf( LSADDRESS, sizeof( LSADDRESS ), param );
            log( "上锁服务器地址:%s\n",LSADDRESS );
        } else if( strcmp( command, "LSPORT" ) == 0 ){//TESTNAME
						LSPORT = atoi( param);
						log( "上锁服务器端口:%d\n",LSPORT );
				} else if( strcmp( command, "TESTNAME" ) == 0 ){
            snprintf( TESTNAME, sizeof( TESTNAME ), param );
            log( "测试名:%s\n",TESTNAME );
#endif

#ifdef _OACSTRUCT_TCP
				} else if( strcmp( command, "OACBIND" ) == 0 ){
            OACbindtype = atoi( param );
            log( "人物列表:%d\n",OACbindtype );
#endif

        } else if( strcmp( command, "saacname" ) == 0 ){ // 星系名
					snprintf( saacname, sizeof( saacname ), param );
					log( "saac名:%s\n",saacname );
			  } else if( strcmp( command , "Total_Charlist" ) == 0 ){
        	Total_Charlist = atoi( param );
        	log( "更新人物点数间隔:%d\n",Total_Charlist );
        } else if( strcmp( command , "Expired_mail" ) == 0 ){
        	Expired_mail = atoi( param );
        	log( "更新进期邮件间隔:%d\n",Expired_mail );
        } else if( strcmp( command , "Del_Family_or_Member" ) == 0 ){
        	Del_Family_or_Member = atoi( param );
        	log( "删除家族成员间隔:%d\n",Del_Family_or_Member );
        } else if( strcmp( command , "Write_Family" ) == 0 ){
        	Write_Family = atoi( param );
        	log( "更新家族信息间隔:%d\n",Write_Family );
#ifdef _NEW_PLAYERGOLD
        } else if( strcmp( command , "NEW_PLAYERGOLD" ) == 0 ){
        	New_PlayerGold = atoi( param );
        	log( "新帐号给钱:%d\n",New_PlayerGold );
#endif
		}
    }

    fclose(fp);
    return 0;
}


static void parseOpts( int argc, char **argv )
{
    int c , option_index;

		while(1){
        static struct option long_options[] = {
            {"wkport" , 1 , 0 , 'w'},
            {"help" , 0 , 0 , 'h' },
            {0,0,0,0}
        };
        c = getopt_long( argc , argv , "w:h" , long_options , &option_index );
        if( c == -1 )break;
        switch( c ){
        case 'h':
            fprintf( stderr ,
                     "使用方法: saac [-h] [-w port] [-w port] ... \n"
                     "-h : 显示saac的帮助\n"
                     "-w port : 添加一个工作站进程端口\n"
                     "Copyright 2006 龙zoro工作室 "
                     "( Longzoro System Supply )\n");
            exit(0);
            break;
        case 'w':
			wknum ++;
#ifndef _FIX_WORKS
			log( "添加工作站端口 %d\n", atoi ( optarg ));
            addWorkerPort( atoi( optarg ));
#endif
            break;
        default:
            log( "不能读懂选项 %c\n" , c );
            exit(0);
        }
    }
}
double
time_diff(struct timeval subtrahend,
          struct timeval subtractor);
int main( int argc , char **argv )
{

    // Nuke +1 1012: Loop counter
    int counter = 0;

    signal(SIGUSR1, sigusr1);

    log_rotate_interval = 3600 * 24 * 7;

    Lock_Init();	// Arminius 7.17 memory lock

#ifdef _SASQL1
    sasql_init();
#endif

#ifdef _SASQL
    sasql_init();
#endif

#ifdef _UNLOCKPOOL
	init_UnlockPool();
#endif

#ifdef _LOCK_SERVER
	LServerInit();
#endif
#ifdef _PAUCTION_MAN
	IAData_Init();
#endif
#ifdef _NEW_PLAYERGOLD
	NEWPLAYER_FreeGoldInit();
#endif
#ifdef _UNIVERSE_CHATROOM
	CHATROOM_Init();
#endif
	UNlockM_Init();
    initWorkerPort();
    parseOpts( argc, argv );
    if(readConfig( "acserv.cf" )<0){
        log( "无法在当前目录里读取 acserv.cf .\n" );
        exit(1);
    }

#ifdef _DEATH_CONTEND
	if( PKLIST_InitPkTeamList( 2000 ) == -1 ) exit(1);

	if( PKLIST_LoadUserfromFile( "pklist", "user1.txt" ) != -1 ){
		PKLIST_CutStartUserfromFile( "pklist", "d.txt" );
	}

	if( PKLIST_LoadPkTeamListfromFile( "pklist", "pk_list_team.txt" ) == - 1 ) exit(1);
	

	PKLIST_alteUserfromFile( "pklist", "alteuser.txt");
	PKLIST_CheckRepeatBattleHistory();
	PKLIST_Sort_PKListSort();
	

	PKLIST_CheckAllUser();
	PKLIST_BackupPkTeamListfromFile( "pklist", "pk_list_team.txt" );
#endif



    log( "读取数据目录\n" );
    dbRead( dbdir );
#ifdef _DEFEND_NEWUSERS
	DEFEND_InitList();
#endif
#ifdef	__FAMILY
    log("读取 家族庄园\n");
    readFMSMemo(fmsmemodir);
    log("读取 家族留言\n");
    readFMPoint(fmpointdir);
    log("读取 家族目录\n");
    readFamily(familydir);
#endif
    log( "准备 档案目录\n" );
    prepareDirectories( chardir );
    log( "准备 日志目录\n" );
    prepareDirectories( logdir );
    log( "准备 锁档目录\n");
    prepareDirectories( lockdir );
    log( "准备 邮件目录\n" );
    prepareDirectories( maildir );

#ifdef _SLEEP_CHAR
    prepareDirectories( sleepchardir );
    log( "准备 睡眠档案目录\n" );
#endif

    /* 凶引匀化月丢□伙毛  心仇戈 */
    if( readMail(maildir) < 0 ){
        log( "不能初始化邮件\n" );
        exit(1);
  
	}

    /* TCPSTRUCT 毛赓渝祭 */
    {
        int tcpr;
        if( ( tcpr = tcpstruct_init( NULL , port , 0 ,
		CHARDATASIZE * 16 * MAXCONNECTION , 1 /* DEBUG */ ) ) < 0 ){
            log( "不能开启TCP: %d\n", tcpr );
            return 1;
        }
    }
    saacproto_InitServer( netWrite , CHARDATASIZE );
    connectToWorker();

#ifdef _OACSTRUCT_TCP
	{
		int ret;
		if( (ret=OAC_InitTcpIp( OACbindtype, NULL, 0)) != OK ){
			log( "开启TCP/ID错误:%d\n", ret );
			return 1;
		}
	}
#endif

#ifdef _LOCK_SERVER
	if( connectToLServer( LSADDRESS, LSPORT) == FALSE )	{
		log("%s", "登陆锁定服务器失败!!\n");
			return 1;
	}else	{
		log("%s", "连接锁定服务器成功!!");
		usleep( 100*10000);
		LServer_SALogin_send();
	}
#endif
    {
        struct sigaction s,os;
        
        bzero( &s, sizeof(s));
        s.sa_handler = sighandle;
        s.sa_flags = SA_NOMASK;
        sigaction( SIGTERM,  &s, &os );

        bzero( &s, sizeof(s));
        s.sa_handler = sighandle;
        s.sa_flags = SA_NOMASK;
        sigaction( SIGINT,  &s, &os );

        bzero( &s, sizeof( s ));
        s.sa_handler = SIG_IGN;
        s.sa_flags = SA_NOMASK;
        sigaction( SIGPIPE, &s, &os );

    }
  
#ifdef _BAD_PLAYER            // WON ADD 送坏玩家去关       
	log("\n 开启送坏玩家进监狱!!");
	BadPlayer();
#endif	
  
#ifdef _AC_SEND_FM_PK		 // WON ADD 庄园对战列表储存在AC
	load_fm_pk_list();
#endif

#ifdef _ACFMPK_LIST
	FMPK_LoadList();
#endif
#ifdef _ALLDOMAN
	LOAD_herolist();  // Syu ADD 排行榜NPC
#endif

#ifdef _ANGEL_SUMMON
	initMissionTable();
#endif

#ifdef _RACEMAN
	initRankTable();
#endif

		log( "\n版本：龙zoro版SAAC服务端v1.1\n" );
		
    log( "\n开始循环\n" );

    signal(SIGUSR1,sigusr1);	// Arminius 7.20 memory lock
#ifdef _CHANGEGALAXY
	signal(SIGUSR2, sighandle);	// Robin 2003/10/27 星系移民
#endif
	while(1){
        int newti,i;
        static time_t main_loop_time;
		
#if 1
		sys_time = time(NULL);

		if( main_loop_time != sys_time){
#else
		if( main_loop_time != time(NULL)){
#endif
            main_loop_time = time(NULL);
            counter++;
			//andy add 2002/06/20
			UNlockM_UnlockPlayer();

#ifdef _ANGEL_SUMMON
			checkMissionTimelimit();
#endif
#ifdef _UNLOCKPOOL
			UnlockPoolProc();
#endif

            // Nuke *1 1012
            if( ( counter % Total_Charlist ) == 0 ){
                char *c = ctime( &main_loop_time );
                if( c ){
                    struct timeval st,et;
                    log( "\nTIME:%s\n",c );
                    gettimeofday( &st,NULL);
                    dbFlush(dbdir);
                    gettimeofday( &et,NULL);
                    log( "Flushed db(%fsec)\n", time_diff(et,st) );
                    log( "档案表列总数:%d NG:%d\n",
                         total_ok_charlist, total_ng_charlist );
                }
            }
            // Nuke **1 1012
            if( ( counter % 600 ) == 0 ){
                struct timeval st,et;
                gettimeofday( &st,NULL);
                expireMail();
                gettimeofday( &et,NULL);
                log( "过期邮件(%fsec)\n", time_diff(et,st) );
            }
#ifdef _NEW_PLAYERGOLD
			if( ( counter % 300 ) == 0 ){
				backupNEWPlayerListFile( "newuser.txt", "newuser.bak");
			}
#endif
#ifdef	__FAMILY

#ifndef _DEATH_FAMILY_LOGIN_CHECK   // WON ADD 家族战登入检查
			if ((counter % /*10800*/600) == 0)	// 3hr( -> 1min)
            {
            	struct timeval st, et;
            	time_t t1;
            	gettimeofday(&st, NULL);
            	time(&t1);
            	delovertimeFMMem(t1);
            	gettimeofday(&et, NULL);
            	log("删除家族或成员(%fsec)\n", time_diff(et, st));
            }
#endif


            if ((counter % 300) == 0) // 300( -> 60)
            {
            	struct timeval st, et;
            	gettimeofday(&st, NULL);
            	writeFamily(familydir);
            	writeFMPoint(fmpointdir);
            	writeFMSMemo(fmsmemodir);
            	gettimeofday(&et, NULL);
            	log("记录家族(%fsec)\n", time_diff(et, st));
            }
#endif
        }

#ifdef _OACSTRUCT_TCP
		OAC_select();
		OAC_ReadMessLine();
#endif

        newti = tcpstruct_accept1();
        if( newti >= 0 ){
			log( "同意: %d\n" , newti );
			gs[newti].use = 1;
        }

#ifdef _LOCK_SERVER
		{
			int mlen;
			char buf[4096];
			if( CHECK_LS_BEUSE() == TRUE )	{
				TcpIp_LS_accept();

			}
			//Show_LS_MEMBufferList();

			mlen = MMGet_LsReadBuflist( buf, sizeof( buf));
			if( mlen > 0 )	{
				if( lockproto_SaacDispatchMessage( buf) < 0 )	{
					log( "锁定信息:%s", buf);
				}
			}
			CHECK_LSTYPE();
#ifdef _TEST_LOCKSERVER
			TEST_TOLOCKSERVER();
#endif
		}
#endif

#ifdef _FIX_WORKS
		{
			int wkti=-1;
			int tis[BACKLOGNUM];
			newti = tcpstruct_work_accept( tis , 1);
			if( newti == 1 )	{
				log( "同意工作站: %d\n" , newti );
			}


			for( i=0; i<MAXWORKCONNECTION; i++){
//				char buf6[CHARDATASIZE*16];
				char buf6[256];

				if( MEMBuffer_getWkLineReadBuffer( i, buf6, sizeof(buf6), WK_R) <= 0 ){
					continue;
				}
				wkti = i;
//#ifndef _TIMEOUTKICK
				{
					int j;
					for( j=0;; j++)	{
						if( buf6[j] == '\n' ) buf6[j]= 0;
						if( buf6[j] == 0 )break;
					}
				} 
//#endif
				doWorker( wkti, buf6 );
			}
		}
#endif
        for(i=0;i<MAXCONNECTION;i++){
            char buf[CHARDATASIZE*16];
            int l;
            l = tcpstruct_readline_chop( i , buf , sizeof( buf )- 1);
			if( isWorker( i ) ){
#ifndef _FIX_WORKS
                if( l > 0 ){
                    doWorker( i , buf );
                } else if( l == TCPSTRUCT_ETOOLONG || l < 0 ){
                    checkWorkerClosed( i );
                    tcpstruct_close( i );
                } else if( l == 0 ){
                    ;
                }
#endif
            } else {
                if( !gs[i].use )continue;
                if( l > 0 ){
					char debugfun[256];
                    buf[l]=0;
                    if( saacproto_ServerDispatchMessage( i , buf, debugfun)<0){
                        // Nuke start
                        tcpstruct_close(i);// avoid the shutdown the gmsv ttom
                        log( "GMSV(%s) 消息:%s\n", gs[i].name, debugfun);
                    }
                } else if( l == TCPSTRUCT_ETOOLONG ){
                    log( "很长:%d 服务器名::%s\n", i , gs[i].name );

#ifdef _AC_UNLOCK_GS_PLAYER     // WON ADD 自动UNLOCK
					DeleteMemLockServer(gs[i].name);
					log("\n用户(%s) 登出! 玩家解锁!!\n", gs[i].name );
#endif
                    logout_game_server( i );
                } else if( l < 0 ){
                    log( "关闭:%d 服务器名:%s\n", i , gs[i].name );
#ifdef _UNIVERSE_CHATROOM
					CHATROOM_GSClosedHandle( i);
#endif
#ifdef _AC_UNLOCK_GS_PLAYER     // WON ADD 自动UNLOCK
					DeleteMemLockServer(gs[i].name);
					log("\n用户(%s) 登出! 玩家解锁!!\n", gs[i].name );
#endif


#ifdef _WORKER_AUTO_LOGOUT     // WON ADD GS断线自动送LOGOUT给WORKER
					Worker_auto_logout( i, gs[i].name );
#endif
                    logout_game_server(i);
                } else if( l == 0 ){
                    ;
                }
            }
        }
        {
            static struct timeval tv_loop_store ;
            struct timeval now;
            double dif;
            gettimeofday( &now , NULL );
            dif = time_diff( now, tv_loop_store );
            if( dif > 0.5 ){ // CoolFish 0.1 -> 0.5 2001/5/19
                //log( "Sl:%f" , dif );
            }
            tv_loop_store = now;
        }

        /* 伐□市□及正奶丞失它玄毛譬屯月［
           正奶丞失它玄仄凶日夫弘毛分仄化伐□市□毛诮仄化仄引丹［
           匹必□丞扔□田□卞  蔽读卞反骚襞允月           */
        {
            int i;
            time_t thre = time(NULL) - WKTIMEOUT;            
            for(i =0;i<MAXWK;i++){
                if( !wk[i].use )continue;
                if( wk[i].last_query &&
                    wk[i].status == WKSTAT_WAITING &&
                    wk[i].last_query < thre ){
                    // Nuke start--
                    log( "工作站 %d 超时! 关闭中...\n", i );
#ifdef _FIX_WORKS
					Work_close( wk[i].ti );
					wk[i].use = 0;
					wk[i].fd = -1;
					wk[i].status = WKSTAT_IDLE;
#else
                    tcpstruct_close( wk[i].ti );
#endif
                    log( "工作站 %d 超时! uhmmm...\n", i);

                    if( wk[i].callback ){
						// Robin log
						//log(" wk[%d] c0:%s c1:%s c2:%s c3:%s c4:%s i0:%d i1:%d ",
						//		wk[i].request_ti, wk[i].cparam[0], wk[i].cparam[1],
						//		wk[i].cparam[2], wk[i].cparam[3], wk[i].cparam[4],
						//		wk[i].iparam[0], wk[i].iparam[1]);

                        wk[i].callback( wk[i].request_ti , -1,
                                        wk[i].cparam[0] ,
                                        wk[i].cparam[1] ,
                                        wk[i].cparam[2] ,
                                        wk[i].cparam[3] ,
                                        wk[i].cparam[4] ,
                                        wk[i].iparam[0] ,
                                        wk[i].iparam[1] );
                        wk[i].use = 0;
                    }
                }
            }
        }
    }
    
#ifdef _SASQL
	sasql_close();
#endif

    return 0;       
}
double
time_diff(struct timeval subtrahend,
          struct timeval subtractor)
{
    return( (subtrahend.tv_sec - subtractor.tv_sec)
            + (subtrahend.tv_usec
            - subtractor.tv_usec  ) / (double)1E6 );
}


/*
  夫弘及夫□  □玄市它件玄毛  月［
  云卅元凛渝及手及反允屯化及夫弘白央奶伙互云卅元卞卅月方丹卞允月［

  仇及酷  反醒侬及瑕互云云仁卅月互｝    匹奶件正□田伙毛  凳仄化手
    暹屺分［
  
 */
int get_rotate_count(void )
{
    int a;
    unsigned int t = (unsigned int ) time(NULL);
        
    a = ( t / log_rotate_interval ) * log_rotate_interval;
    
    return a;
}

static int findregBlankMemBuf( void );
static int unregMemBuf( int index );
static int findregBlankCon( void );
static int getFreeMem( void );

static int appendReadBuffer( int index, char *data, int len );
static int appendWriteBuffer( int index , char *data, int len );
static int appendMemBufList( int top, char *data, int len );
static int consumeMemBufList( int top, char *out, int len, int flag, int copyflag );
static int getLineReadBuffer( int index, char *buf, int len );

int tcpstruct_init( char *addr , int p , int timeout_ms , int mem_use , int db )
{
    mbsize = mem_use / sizeof( struct membuf );
    mbuse =0;
    mb_finder = 0;
    mb = ( struct membuf * ) calloc( 1, mbsize * sizeof(struct membuf ));

    if( mb == NULL ) return TCPSTRUCT_ENOMEM;
    bzero( mb , mbsize * sizeof( struct membuf ));

    con = ( struct connection *) calloc( 1, MAXCONNECTION * sizeof( struct connection ));
    if( con == NULL ){
        free( mb );
        return TCPSTRUCT_ENOMEM;
    } else {
        int i;
        for(i=0;i<MAXCONNECTION;i++){
            con[i].use = 0;
            con[i].fd = -1;
        }
    }
    select_timeout.tv_sec = timeout_ms / 1000;
    select_timeout.tv_usec = (timeout_ms - ( timeout_ms/1000)*1000)*1000;

    /* socket */
    mainsockfd = socket( AF_INET , SOCK_STREAM ,  0 );
    if( mainsockfd < 0 ) return TCPSTRUCT_ESOCK;
    
    /* bind */
    bzero( &localaddr ,sizeof( localaddr ));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons( p );
    
    if( addr ){
        localaddr.sin_addr.s_addr = inet_addr( addr );
    } else {
        localaddr.sin_addr.s_addr = htonl( INADDR_ANY );
    }
    if( bind( mainsockfd , (struct sockaddr*) &localaddr ,
              sizeof( localaddr )) < 0 ) return TCPSTRUCT_EBIND;

    /* listen */
    if( listen( mainsockfd , BACKLOGNUM )<0) return TCPSTRUCT_ELISTEN;

#ifdef _FIX_WORKS	//for work
	MEMBuffer_Init();
	if( SetWorkConnectionInit( 1) <= 0 )
		return TCPSTRUCT_WORKERROR;

	worksockfd = socket( AF_INET , SOCK_STREAM ,  0 );
	log( "开始工作: %d...\n" , worksockfd);
	if( worksockfd < 0 ) return TCPSTRUCT_ESOCK;
    bzero( &localaddr ,sizeof( localaddr ));
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons( FORWORKPORT );
    localaddr.sin_addr.s_addr = /*htonl( INADDR_ANY );*/inet_addr( "127.0.0.1" );
    if( bind( worksockfd , (struct sockaddr*) &localaddr ,
              sizeof( localaddr )) < 0 ) return TCPSTRUCT_WORKEBIND;
    if( listen( worksockfd , BACKLOGNUM )<0) return TCPSTRUCT_ELISTEN;
#endif
    return OK;
}

int tcpstruct_accept1( void )
{
    int tis[BACKLOGNUM];
    int ret;

    ret = tcpstruct_accept( tis , 1 );
    if( ret < 0 ){
        return ret;
    } else if( ret == 1 ){
        return tis[0];
    } else {
        return TCPSTRUCT_EBUG ;
    }
}

int tcpstruct_accept( int *tis , int ticount )
{
    int i;
    int sret;
    int accepted = 0;
    struct timeval t;
    fd_set rfds, wfds , efds;  
    FD_ZERO( & rfds );
    FD_ZERO( & wfds );
    FD_ZERO( & efds );    

    for(i=0;i<MAXCONNECTION;i++){
        if( con[i].use &&
            con[i].fd >= 0 && con[i].closed_by_remote ==0 ){
            FD_SET( con[i].fd , & rfds );
            FD_SET( con[i].fd , & wfds );
            FD_SET( con[i].fd , & efds );
        }
    }
    
    t = select_timeout;
    sret = select( 1024, & rfds , (fd_set*)NULL, & efds , &t);
	if( sret > 0 ) {
		for(i=0;i< MAXCONNECTION;i++){
			if( ( con[i].fd >= 0 ) && FD_ISSET( con[i].fd , &rfds ) ){
				int fr = getFreeMem( );
				int rr , readsize ;
				if( fr <= 0 ) continue;
				if( fr > sizeof(tmpbuf ) ){
					readsize = sizeof( tmpbuf);
				} else {
					readsize = fr;
				}
				rr = read( con[i].fd , tmpbuf , readsize );
				if( rr <= 0 ){
					con[i].closed_by_remote = 1;
				} else {
					appendReadBuffer( i , tmpbuf , rr );
				}
			}
		}
    }    
    /* write */
    t = select_timeout;    
    sret = select( 1024, (fd_set*)NULL, &wfds, & efds , &t);
	if( sret > 0 ) {
		for(i=0;i<MAXCONNECTION;i++){
			if( ( con[i].fd >= 0 ) && FD_ISSET( con[i].fd , &wfds )){
				char send_buf[4096];
				int l , rr;
				memset( send_buf, 0, sizeof( send_buf));
				l = consumeMemBufList( con[i].mbtop_wi ,send_buf, sizeof(send_buf),0 , 1 );
				rr = write( con[i].fd , send_buf , l );
				if( rr < 0 ){
					con[i].closed_by_remote = 1;
				} else {
					consumeMemBufList( con[i].mbtop_wi , send_buf, l, 1 , 0 );
				}
			}
		}
	}

    for( i=0; i<ticount; i++){
        int asret;
        struct timeval t;
        t.tv_sec =0;
        t.tv_usec =0;
        FD_ZERO( & rfds );
        FD_ZERO( & wfds );
        FD_ZERO( & efds );

        FD_SET( mainsockfd , & rfds );
        FD_SET( mainsockfd , & wfds );
        FD_SET( mainsockfd , & efds );
        asret = select( 1024, &rfds , &wfds , &efds, &t );
		// Nuke 20040610: add asret>0 to avoid signal interrupt in select
        if( (asret>0) && FD_ISSET( mainsockfd , & rfds )){
            struct sockaddr_in c;
            int len , newsockfd;
            int newcon;
            bzero( &c , sizeof( c ));
            len = sizeof( c );
            fprintf( stderr, "i can accept " );
            newcon = findregBlankCon( );
            if( newcon < 0 ) continue;
            newsockfd = accept( mainsockfd, (struct sockaddr*)&c , &len );
            log( "同意: %d\n" , newsockfd );
            if( newsockfd < 0 ){
                unregMemBuf( newcon );
                continue;
            }
            set_nodelay( newsockfd );
            con[newcon].fd = newsockfd;
            memcpy( &con[newcon].remoteaddr , &c ,sizeof(c));
            tis[accepted] = newcon;
            accepted ++;
        }
    }

    return accepted;
}


int tcpstruct_close( int ti )
{

    if( ti < 0 || ti >= MAXCONNECTION )return TCPSTRUCT_EINVCIND;
    if( con[ti].use == 0 ){
        return TCPSTRUCT_ECLOSEAGAIN;
    }
    close( con[ti].fd );
    con[ti].use = 0;
    con[ti].fd = -1;

    /* 伉旦玄毛凶升匀化蝈  毛弁伉失允月 */
    consumeMemBufList( con[ti].mbtop_ri , NULL,
                   mbsize * sizeof( mb[0].buf ), 1, 0 );
    consumeMemBufList( con[ti].mbtop_wi , NULL,
                   mbsize * sizeof( mb[0].buf ), 1, 0 );
    unregMemBuf( con[ti].mbtop_ri );
    unregMemBuf( con[ti].mbtop_wi );
    con[ti].mbtop_ri = -1;
    con[ti].mbtop_wi = -1;    
    return OK;
}

/*
    心仇戈手及互手丹卅仁化｝仄井手 remoteclose 分匀凶日 -1 毛井尹允
  
 */
int tcpstruct_read( int ti , char *buf , int len )
{
    int l;

    if( ti < 0 || ti >= MAXCONNECTION || con[ti].use == 0 )
        return TCPSTRUCT_EINVCIND;
    l = consumeMemBufList( con[ti].mbtop_ri , buf , len , 1 , 1);
    if( l == 0  && con[ti].closed_by_remote ) return TCPSTRUCT_EREADFIN;

    return l;
}

/*
  1垫毛方心仇戈［
  int kend : 1卅日垫  及 \n 毛诮允
  int kend_r : 1卅日垫  及 \r 手诮允(丐木壬)

    心仇戈手及互手丹卅仁化｝井勾 remote closed 分匀凶日-1毛井尹允
  // Nuke
	Read 1 line
	if kend==1 then delete \n at the tail
	if kend_r== 1 then delete \r at the tail (if any)
	if no data read, it means 'remote closed' then return -1
 */
int tcpstruct_readline( int ti , char *buf , int len , int kend , int kend_r )
{
    int l;
    int minus = 0;

    if( ti < 0 || ti >= MAXCONNECTION || con[ti].use == 0 )
        return TCPSTRUCT_EINVCIND;

    l = getLineReadBuffer( ti , buf , len );
    if( l == 0 ){
        if( con[ti].closed_by_remote ){
            return TCPSTRUCT_EREADFIN;
        } else {
            return 0;
        }
    }
    
    if( kend ){
        if( buf[l-1]=='\n' ){
            buf[l-1] = 0; minus =-1;
        }
    }
    if( kend_r ){
        if( buf[l-1]=='\r' ){
            buf[l-1] = 0; minus = -1;
        }
        if( buf[l-2]=='\r' ){
            buf[l-2] = 0; minus = -2;
        }
    }
    return l + minus;
}
int tcpstruct_readline_chop( int ti , char *buf, int len )
{
    return tcpstruct_readline( ti , buf , len , 1,1);
}

int tcpstruct_write( int ti , char *buf , int len )
{
    if( ti < 0 || ti >= MAXCONNECTION || con[ti].use == 0 )
        return TCPSTRUCT_EINVCIND;    
    return appendWriteBuffer( ti , buf , len );
}

int tcpstruct_connect( char *addr , int port )
{
    int newti ;
    int s, r;
    struct sockaddr_in svaddr;
    struct hostent *he;
    
    s = socket( AF_INET, SOCK_STREAM , 0 );
    if(s<0)return -2;

    bzero( &svaddr , sizeof( svaddr ));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons( port );

    if( inet_aton( addr, &svaddr.sin_addr ) == 0 ){
        he = gethostbyname( addr );
        if( he == NULL ){
            return TCPSTRUCT_EHOST;
        }
        memcpy( & svaddr.sin_addr.s_addr , he->h_addr ,
                sizeof( struct in_addr));
    }
    r = connect( s , ( struct sockaddr*)&svaddr,sizeof(svaddr));
    if( r < 0 ){
        return TCPSTRUCT_ECONNECT;
    }
    set_nodelay( s );
    newti = findregBlankCon( );
    if( newti < 0 ){
        fprintf( stderr , "连接失败: newti:%d\n", newti );
        return TCPSTRUCT_ECFULL;
    }
    con[newti].fd = s;
    memcpy( & con[newti].remoteaddr , &svaddr ,
            sizeof( struct sockaddr_in));
    return newti;
}

static int appendReadBuffer(  int index , char *data , int len )
{
    int top;

    top = con[index].mbtop_ri;
    for(;;){
        int nextind = mb[top].next;

        if( nextind == -1 ) break;
        top = nextind;
    }
    return appendMemBufList( top , data , len );
}

static int appendWriteBuffer( int index , char *data , int len )
{
    int top;
    top = con[index].mbtop_wi;
    for(;;){
        int nextind = mb[top].next;
        if( nextind == -1 ) break;
        top = nextind;
    }
    return appendMemBufList( top , data , len );
}
static int appendMemBufList( int top , char *data , int len )
{
    int fr = getFreeMem( );
    int rest = len;
    int data_topaddr = 0;
    
    if( len >= fr ){
		FILE *fp;
		if( (fp=fopen( "badsysinfo.txt", "a+")) != NULL ){
			fprintf( fp, "appendMemBufList() len:%d / fr:%d err !! \n", len, fr);
			fclose( fp);
		}
		//andy_log
		log( "appendMemBufList() len:%d / fr:%d err !! \n", len, fr);
        return -1;
    }
	data[len] = 0;
    for(;;){
        int blanksize = sizeof( mb[0].buf ) - mb[top].len;
        int cpsize = ( rest <= blanksize ) ? rest : blanksize;
        memcpy( mb[top].buf + mb[top].len ,
                data + data_topaddr , cpsize );
        mb[top].len += cpsize;
        if( rest <= blanksize ){
            return len;
        } else {
            int newmb;
            rest -= cpsize;
            data_topaddr += cpsize;
            if( (newmb = findregBlankMemBuf( ) ) == TCPSTRUCT_EMBFULL ){
				FILE *fp;
				if( (fp=fopen( "badsysinfo.txt", "a+")) != NULL ){
					fprintf( fp, "find newmb == TCPSTRUCT_EMBFULL err data:%s !!\n", data);
					fclose( fp);
				}
				log( "find newmb == TCPSTRUCT_EMBFULL err data:%s !!\n", data);
			}
            mb[top].next = newmb;
            top = mb[top].next;
        }
    }
    return TCPSTRUCT_EBUG;
}

static int consumeMemBufList( int top , char *out , int len ,
                          int consumeflag , int copyflag )
{
    int total = 0;
    int top_store = top;
    for(;;){
        int cpsize;
        if( top == -1 ) break;
        cpsize = ( mb[top].len <= ( len - total) ) ?
            mb[top].len : ( len - total );

        if( copyflag ) memcpy( out + total , mb[top].buf , cpsize );
        total += cpsize;

        if( consumeflag ){
            mb[top].len -= cpsize;
            if( mb[top].len > 0 ){
                /* 勾井中反凶仄化卅中及匹memmove */
                memmove( mb[top].buf , mb[top].buf + cpsize ,
                         sizeof( mb[top].buf ) - cpsize );
            }
        }
        top = mb[top].next;
        if( total == len ){
            break;
        }
    }

    if( consumeflag ){
        /* 卅互今互0卞卅匀化月卅日荸  ［匹手  赓及支勾反荸  仄卅中冗 */
        top = mb[top_store].next;
        for(;;){
            if( top == -1 )break;
            if( mb[top].len == 0 ){
                int prev;
                mb[top_store].next = mb[top].next;
                prev = top;
                top = mb[top].next;
                unregMemBuf( prev );
            } else {
                top = mb[top].next;
            }
        }
    }
    
    return total;
}

static int getLineReadBuffer( int index , char *buf, int len )
{

    int top = con[index].mbtop_ri;
    int ti = 0 , breakflag = 0;

    for(;;){
        int i;        
        int l = mb[top].len;
        if( top == -1 )break;
        for( i=0 ; i < l ; i++){
            if( mb[top].buf[i] == '\n' ){
                breakflag = 1;
                break;
            }
            ti ++;
        }
        if( breakflag )break;
        top = mb[top].next;
    }
    if( ti > len ){
        /* 1垫互卅互允亢月［    卅巨仿□毛井尹六 */
        return TCPSTRUCT_ETOOLONG;
    }
    /* 垫互敦岳仄化卅中 */
    if( breakflag == 0 ){
        return 0;
    }

    return consumeMemBufList( con[index].mbtop_ri , buf , ti+1 , 1 , 1 );
}



/*
    心仇户月    赢今毛忒允
  int index : con index

  return:
    反巨仿□
  0动晓及桦宁反 read 仄化手方中赢今［


  mbsize 井日mbuse 毛娄中化扔奶术毛井仃月分仃［
  仇引井中芴曰及坌反  骰允月［公氏卅称井中袄卞仇分歹日卅仁化手第［
  
 */   
static int getFreeMem( void )
{
    return ( mbsize - mbuse ) * sizeof( mb[0].buf );
}

/*
  
  membuf 及坞五毛茧仄分允［

  return : 心勾井匀凶日 >=0 匹 index.
  心勾井日卅井匀凶日  

  腹绸及午五卞反 mb_finder 毛勾井丹［
  仇木匹腹绸仄化｝    卞reg允月［
 */

static int findregBlankMemBuf( void  )
{
    int i;
    for(i=0;i<mbsize;i++){
        mb_finder ++;
        if( mb_finder >= mbsize || mb_finder < 0 ) mb_finder = 0;

        if( mb[mb_finder].use == 0 ){
            mb[mb_finder].use = 1;
            mb[mb_finder].len = 0;
            mb[mb_finder].next = -1;
            mbuse ++;
            return mb_finder;
        }
    }
    return TCPSTRUCT_EMBFULL;
}

/*
  mb 毛荸  允月
  
 */
static int unregMemBuf(  int index )
{
    mb[index].use=0;
    mb[index].next = -1;
    mb[index].len = 0;
    mbuse --;
    return OK;
}

static int findregBlankCon( void )
{
    int i;
		// Nuke changed 0->1
		//for(i=0;i<MAXCONNECTION;i++){
    for(i=1;i<MAXCONNECTION;i++){
        if( con[i].use == 0 ){
            con[i].use = 1;
            con[i].fd = -1;

            con[i].mbtop_ri = findregBlankMemBuf();
            if( con[i].mbtop_ri < 0 ){
                fprintf( stderr , "EMBFULL\n" );
                return TCPSTRUCT_EMBFULL;
            }
            
            con[i].mbtop_wi = findregBlankMemBuf();
            if( con[i].mbtop_wi < 0 ){
                unregMemBuf( con[i].mbtop_ri );
                fprintf( stderr , "EMBFULL\n" );
                return TCPSTRUCT_EMBFULL;
            }
            bzero( & con[i].remoteaddr , sizeof( struct sockaddr_in));
            con[i].closed_by_remote = 0;
            return i;
        }
    }
    return TCPSTRUCT_ECFULL;
}

int tcpstruct_countmbuse( void )
{
    int i,c=0;
    for(i=0;i<mbsize;i++){
        if( mb[i].use )c++;
    }
    return c;
}

char * getGSName( int i )
{
    return gs[i].name;
}

void checkGSUCheck( char *id )
{
    int i;
    char gname[256];
    if(!id[0])	return;
	memset( gname, 0,  sizeof( gname) );
	if( LockNode_getGname( (getHash(id) & 0xff), id, gname) <= 0 ){
		log("Can't find gname from id:%x/%s err !!\n", getHash( id), id);
		return;
	}
	log("\n");
    for(i=0; i < MAXCONNECTION; i++ ){
        if( gs[i].name[0] && strcmp( gs[i].name , gname )==0){
            log("sending UCheck[%s] to %d.%x/%s fd:%d !!\n", id, i, getHash( id), gname, gs[i].fd);
			saacproto_ACUCheck_send( gs[i].fd , id );
			return;
        }
    }
	log("Can't find gname:%s sending err !!\n", gname);
}

void set_nodelay( int sock )
{
    int flag = 1;
    int result = setsockopt( sock, IPPROTO_TCP, TCP_NODELAY,
                             (char*)&flag, sizeof(int));
    if( result < 0 ){
        log( "不能设置延迟.\n" );
    } else {
        log( "设置延迟: fd:%d\n", sock );
    }
}


/*
  允屯化及必□丞扔□田□卞  霜［

  int flag : 1分匀凶日霜耨葭卞反云仁日卅中
  
 */
void gmsvBroadcast( int fd, char *p1, char *p2, char *p3 , int flag )
{
    int i,c=0;
    
    for(i=0;i<MAXCONNECTION;i++){
        if( ( flag == 1 ) && ( i == fd ) )continue;
        if( gs[i].use && gs[i].name[0] ){
            saacproto_Broadcast_send( i, p1,p2,p3);
            c++;
        }
    }
/*
#if 1
    {
        char fn[1000];
        FILE *fp;
        snprintf( fn, sizeof(fn), "%s/broadcast.log" , logdir );
        fp = fopen( fn,"a" );
        if(fp){
            fprintf( fp, "From %s(%d) Id:[%s] Char:[%s] Mesg:[%s]\n",
                     gs[fd].name, fd, p1,p2,p3 );
            fclose(fp);
        }
    }
#endif
*/
}

// 星系移民
void sigusr2(int a)
{
	
	FILE *listfile;
	char databuf[1024];
	//char fname[1024];
	//char new_fname[1024];
	char backup_movelist[1024];
	//char playerinfo[1024], frominfo[1024], toinfo[1024];
	char *playerinfo;
	int frominfo, toinfo;
	int i, j;

	char servername[16][20] =
	{
		"仙女", // 0
		"太阳", // 1
		"天神", // 2
		"北斗", // 3
		"紫微", // 4
		"苍龙", // 5
		"银河系", // 6
		"香港", // 7
		"星乐园", // 8
		"网路家庭", // 9
		"圣兽", // 10
		"天鹰", // 11
		"新界", // 12
		"仙女1", // 13
		"仙女2", // 14
		"仙女3" // 15
	};


	log("\n  %s Change_Galaxies.......\n", saacname);

	// 解除全部移民锁定
	DeleteGalaxyChangeLock(0);

	for( j=1; j<=12; j++) {

		char listfilename[1024];
		
		// 读取移民列表
		sprintf( listfilename, "StaMove_%d.txt", j);
		listfile = fopen( listfilename, "r");
		if( listfile == NULL)
		{
			continue;
		}
		log("打开列表文件:%s\n", listfilename);

		
		while(1)
		{
			if( fgets( databuf, sizeof(databuf), listfile) == NULL )
				break;
			
			if( databuf[0] == '#')
				continue;
			
			// 读一笔资料  (帐号 来源星系 目的星系)
			//if( sscanf( databuf, "%s,%s,%s", playerinfo, frominfo, toinfo) != 3)
			//	continue;
			playerinfo = strtok( databuf, ",");
			if( playerinfo == NULL) continue;
			frominfo = atoi( strtok( NULL, ","));
			toinfo = atoi( strtok( NULL, ","));
			
			log("\n %s,%d,%d ", playerinfo, frominfo, toinfo);

			if( (frominfo <0) || ( frominfo >15) || (toinfo <0) || ( toinfo >15))
				continue;
			
			// 是否要移民出去或移民进来
			if( !strcmp( servername[frominfo], saacname) || !strcmp( servername[toinfo], saacname) )
			{
				
				log(" from:%s to:%s ", servername[frominfo], servername[toinfo]);

				// 锁帐号
#ifdef _LOCK_ADD_NAME
				InsertMemLock( getHash(playerinfo) & 0xff, playerinfo, "", "", "星系移民", 0, "0");
#else
				InsertMemLock( getHash(playerinfo) & 0xff, playerinfo, "", "星系移民", 0, "0");
#endif
				
				// 踢人
				log("移民前踢人 ");
				for(i=0;i<MAXCONNECTION;i++)	{
					if( is_game_server_login(i) ){
						saacproto_ACKick_send( i , 1 , playerinfo , -2/*mesgid*/ );
						log(" %d->%s->%s", i, gs[i].name, playerinfo);
					}
				}
			}
			//else
			//{
			//	continue;
			//}
			
		}
		
		// 列表档案改名备份
		//sprintf( backup_movelist, "%s.old", listfilename);
		//rename( listfilename, backup_movelist);
		
		fclose( listfile);
		
	}

	signal(SIGUSR2, sighandle);
}



#ifdef _ANGEL_SUMMON

#define MISSIONFILE "db/missiontable.txt"

static int initMissionTable( void )
{
	FILE *fp;
	char onedata[1024];
	char buf[1024];
	int index =0;

	memset( missiontable, 0, sizeof(missiontable));
	fp = fopen( MISSIONFILE, "r");
	if( !fp ) {
		log("\n加载任务错误!!!! \n");
		//return false;
	}
	log("\n加载任务...");
	while(1) {
		//
		if( fgets( onedata, sizeof(onedata), fp) == NULL)
			break;
		if( onedata[0] == NULL || onedata[0] == '#' )
			continue;
		//easyGetTokenFromBuf( onedata, ',', 1, buf, sizeof( buf));
		//index = atoi( buf);
		easyGetTokenFromBuf( onedata, ',', 1, buf, sizeof( buf));
		if( buf[0] == NULL ) continue;
		strcpy( missiontable[index].angelinfo, buf);
		easyGetTokenFromBuf( onedata, ',', 2, buf, sizeof( buf));
		if( buf[0] == NULL ) continue;
		strcpy( missiontable[index].heroinfo, buf);
		easyGetTokenFromBuf( onedata, ',', 3, buf, sizeof( buf));
		if( buf[0] == NULL ) continue;
		missiontable[index].mission = atoi( buf);
		easyGetTokenFromBuf( onedata, ',', 4, buf, sizeof( buf));
		if( buf[0] == NULL ) continue;
		missiontable[index].flag = atoi( buf);
		easyGetTokenFromBuf( onedata, ',', 5, buf, sizeof( buf));
		if( buf[0] == NULL ) continue;
		missiontable[index].time = atoi( buf);
		easyGetTokenFromBuf( onedata, ',', 6, buf, sizeof( buf));
		if( buf[0] == NULL ) continue;
		missiontable[index].limittime = atoi( buf);

		log(" %d=%s,%s,%d,%d,%d,%d \n", index,
			missiontable[index].angelinfo,
			missiontable[index].heroinfo,
			missiontable[index].mission,
			missiontable[index].flag,
			missiontable[index].time,
			missiontable[index].limittime );

		index++;
		if( index >= MAXMISSIONTABLE) break;
	}
	fclose( fp);
	log("..成功! \n");
	//return true;
}


int saveMissionTable( void )
{
	FILE *fp;
	char onedata[1024];
	char buf[1024];
	int index =0;

	fp = fopen( MISSIONFILE, "w");
	if( !fp ) {
		log("\n打开任务错误!!!! \n");
		//return false;
	}
	log("\n\n保存任务...");
	for( index =0; index < MAXMISSIONTABLE; index++) {

		if( missiontable[index].angelinfo[0] == NULL )
			continue;
		sprintf( onedata, "%s,%s,%d,%d,%d,%d\n",
			missiontable[index].angelinfo,
			missiontable[index].heroinfo,
			missiontable[index].mission,
			missiontable[index].flag,
			missiontable[index].time,
			missiontable[index].limittime );
		fputs( onedata, fp);
	}
	fclose( fp);
	log("..成功! \n");
	//return true;
}

void delMissionTableOnedata( int index)
{
	int gi;

	log("\n删除任务:%d:%s:%s \n", index, missiontable[index].angelinfo, missiontable[index].heroinfo);

	if( index <0 || index >=MAXMISSIONTABLE) return;
	strcpy( missiontable[index].angelinfo, "");
	strcpy( missiontable[index].heroinfo, "");
	missiontable[index].mission = 0;
	missiontable[index].flag = MISSION_NONE;
	missiontable[index].time = 0;
	missiontable[index].limittime = 0;
	
	for( gi=0; gi<MAXCONNECTION; gi++) {
		if (gs[gi].use && gs[gi].name[0]) {
			saacproto_ACMissionTable_send( gi, index, 3, "", "");
		}
	}
}

#define ANSWERTIME 1 // 等待回答时间(小时)
//#define DOINGTIME 3*24 // 任务时间(小时)
#define BOUNDSTIME 1*24 // 保留时间(小时)

void checkMissionTimelimit( void)
{
	int index;
	static time_t lastcheck =0;

	if( sys_time < lastcheck + 5*60 )
		return;

	log("\n检查任务时间限制:%d \n", sys_time);
	for( index =0; index < MAXMISSIONTABLE; index++) {
		if( missiontable[index].flag == MISSION_NONE) {
			continue;
		}
		// 等待使者回应1小时
		else if( missiontable[index].flag == MISSION_WAIT_ANSWER
				&& sys_time > missiontable[index].time + ANSWERTIME*60*60 ) {
			
			delMissionTableOnedata( index);// 删
		}
		// 等待领奖完成 limittime小时
		else if( ( missiontable[index].flag == MISSION_DOING || missiontable[index].flag == MISSION_HERO_COMPLETE )
				&& ( sys_time > (missiontable[index].time + missiontable[index].limittime*60*60))		 ) {

			char buf[1024];
			int gi;
			// 改TIMEOVER
			log(" 任务及领奖时间过:%d ", index);
			missiontable[index].flag = MISSION_TIMEOVER;
			missiontable[index].time = time(NULL);
			missiontable[index].limittime = BOUNDSTIME;
			
			sprintf( buf, "%d|%s|%s|%d|%d|%d|%d ", index,
				missiontable[index].angelinfo,
				missiontable[index].heroinfo,
				missiontable[index].mission,
				missiontable[index].flag,
				missiontable[index].time,
				missiontable[index].limittime );
			for( gi=0; gi<MAXCONNECTION; gi++) {
				if (gs[gi].use && gs[gi].name[0]) {
					saacproto_ACMissionTable_send( gi, 1, 1, buf, "");
				}
			}
			
			continue;
		}
		//else if( missiontable[index].flag == MISSION_HERO_COMPLETE
		//		&& sys_time > missiontable[index].time + BOUNDSTIME*60*60 ) {
		//	log(" 领奖时间过:%d ", index);
		//	delMissionTableOnedata( index);// 删
		//}

		// 资料保留时间(BOUNDSTIME小时)
		else if( missiontable[index].flag == MISSION_TIMEOVER
				&& sys_time > missiontable[index].time + BOUNDSTIME*60*60 ) {
			log(" 保留时间过:%d ", index);
			delMissionTableOnedata( index);// 删
		}

	}
	saveMissionTable();

	lastcheck = sys_time;
}

#endif

#ifdef _RACEMAN
//#define RANKFILE "race/ranktable.txt"
struct RankTable ranktable[DEFMAXRANK];

static int initRankTable( void )
{
	//FILE *fp;
	memset( ranktable, 0, sizeof(ranktable));
/*
	fp = fopen( RANKFILE, "r");
	if( !fp ) {
		log("\n\n Load RankTable ERROR!!!! \n\n");
	}
	log("\n\n Load RankTable... \n");
*/
	return;
}

#endif
