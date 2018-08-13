#define __NET_C__ 
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>     
//ttom+1
#include <sys/timeb.h>
#include "net.h"
#include "buf.h"
#include "link.h"
#include "common.h"
#include "msignal.h"
#include "configfile.h"
#include "util.h"
#include "saacproto_cli.h"
#include "lssproto_serv.h"
#include "char.h"
#include "handletime.h"
#include "log.h"
#include "object.h"
#include "item_event.h"
#include "enemy.h"
// Arminius 7.31 cursed stone
#include "battle.h"
#include "version.h"
#include "pet_event.h"
#include "char_talk.h"
#include "petmail.h"

#ifdef _TEST_PETCREATE
#include "chatmagic.h"
#endif

#ifdef _M_SERVER
#include "mclient.h"
#endif

#ifdef _NPCSERVER_NEW
#include "npcserver.h"
#endif

#define MIN(x,y)     ( ( (x) < (y) ) ? (x) : (y) )
#define IS_2BYTEWORD( _a_ ) ( (char)(0x80) <= (_a_) && (_a_) <= (char)(0xFF) )

#ifdef	_NEW_SERVER_
BOOL bNewServer = TRUE;
#else
BOOL bNewServer = FALSE;
#endif 

// Nuke +1 0901: For state monitor
int StateTable[WHILESAVEWAIT+1];

int ITEM_getRatio();
int CHAR_players();
#define CONO_CHECK_LOGIN 0x001
#define CONO_CHECK_ITEM 0x010
#define CONO_CHECK_PET 0x100
int cono_check=0x111;

int     AC_WBSIZE = (65536*16);
//ttom+1 for the performatce
static unsigned int MAX_item_use=0;
int i_shutdown_time=0;//ttom
BOOL b_first_shutdown=FALSE;//ttom

int mfdfulll = 0;

/*------------------------------------------------------------
 * 扔□田及橇谪
 ------------------------------------------------------------*/
typedef struct tag_serverState
{
    BOOL            acceptmore;     /*  1分匀凶日｝accept 仄凶丐午
                                        切斤仁匹｝close 允月 */
    unsigned int    fdid;           /*  fd 及骚曰袄 */
    unsigned int    closeallsocketnum;  /*   closeallsocket   及酸曰及
                                             醒*/
	int				shutdown;		/*  扔□田毛shutdown允月乒□玉 
									 *	0:骚橘 公木动陆:扑乓永玄扑乓永玄乒□玉
									 * 乒□玉卞卅匀凶凛棉互  匀化月［
									 */
	int				dsptime;		/* shutdown 乒□玉及伐□弁  醒*/
	int				limittime;		/* 仇木手 */
}ServerState;

typedef struct tagCONNECT
{
    BOOL  use;

#if USE_MTIO
#define MTIO_FIXED_BUFSIZE (65536)
    char rb[MTIO_FIXED_BUFSIZE];
    int rbuse;
    char wb[MTIO_FIXED_BUFSIZE];
    int wbuse;
    pthread_mutex_t mutex;    /* Connect毛中元月午五卞勾井丹夫永弁 */
#else
    char *rb;
    int rbuse;
    char *wb;
    int wbuse;
    int check_rb_oneline_b;
	int check_rb_time;
#endif    

    struct sockaddr_in sin; /* 涛粮燮及失玉伊旦 */
    ConnectType ctype;       /* 戊生弁扑亦件及潘   */

    char    cdkey[CDKEYLEN];    /* CDKEY */
    char    passwd[PASSWDLEN];  /* 由旦伐□玉 */
    LoginType state;        /* 蜇箕及夫弘奶件橇谪 */
	int		nstatecount;
    char    charname[CHARNAMELEN];  /* 夫弘奶件  及平乓仿   */
    int     charaindex;     /* char    尺及奶件犯永弁旦［
                             * 夫弘奶件  卞袄互涩烂今木月［-1互犯白巧伙玄
                             *     卞卅中凛［
                             */
    char    CAbuf[2048];         /*  CA() 毛做谅允月啃及田永白央 */
    int     CAbufsiz;       /*  CAbuf 及扔奶术  */
    struct timeval  lastCAsendtime;     /*    卞CA毛霜匀凶凛棉 */

    char    CDbuf[2048];         /*  CD() 毛做谅允月啃及田永白央 */
    int     CDbufsiz;       /*  CDbuf 及扔奶术  */
    struct timeval  lastCDsendtime;     /*    卞CD毛霜匀凶凛棉 */

    struct timeval  lastCharSaveTime; /*     卞平乓仿犯□正毛本□皮仄凶凛棉 */
    
    struct timeval  lastprocesstime;    /*     卞皿夫玄戊伙毛质  仄凶凛棉*/
    struct timeval  lastreadtime;       /*     卞read仄凶凛棉［晓午反切互丹*/
    
    // Nuke start 08/27 : For acceleration avoidance
    // WALK_TOLERANCE: Permit n W messages in a second (3: is the most restricted)
    #define WALK_TOLERANCE 4
    #define WALK_SPOOL 5
    #define WALK_RESTORE 100
    unsigned int        Walktime;
    unsigned int        lastWalktime;
    unsigned int        Walkcount;
    int Walkspool;      // For walk burst after release key F10
    int Walkrestore;
    // B3_TOLERANCE: Time distance between recently 3 B message (8: is the latgest)
    // BEO_TOLERANCE: Time distance between the lastmost B and EO (5: is the largest)
    #define B3_TOLERANCE 5
    #define BEO_TOLERANCE 3
    #define BEO_SPOOL 10
    #define BEO_RESTORE 100
    unsigned int        Btime;
    unsigned int        lastBtime;
    unsigned int        lastlastBtime;
    unsigned int        EOtime;

#ifdef _BATTLE_TIMESPEED
//	unsigned int		DefBtime;
	int		BDTime;
	int		CBTime;
#endif

#ifdef _TYPE_TOXICATION
	int		toxication;
#endif

#ifdef _CHECK_GAMESPEED
	int		gamespeed;
#endif
#ifdef _ITEM_ADDEXP	//vincent 经验提升
	int		EDTime;
#endif
    //    unsigned int      BEO;
    int BEOspool;
    int BEOrestore;
    // Nuke 0219: Avoid cheating
    int die;
    // Nuke end
    // Nuke 0310
    int credit;
    int fcold;
    // Nuke 0406: New Flow Control
    int nu;
    int nu_decrease;
    int ke;
    // Nuke 1213: Flow Control 2
    int packetin;

    // Nuke 0624: Avoid Null Connection
    unsigned int cotime;
    // Nuke 0626: For no enemy
    int noenemy;
    // Arminius 7.2: Ra's amulet
    int eqnoenemy;
#ifdef _Item_MoonAct
	int eqrandenemy;
#endif

#ifdef _CHIKULA_STONE
	int chistone;
#endif
	// Arminius 7.31: cursed stone
    int stayencount;

    /* close 允月屯五井升丹井及筏盛 */
#if USE_MTIO    
    int     closed;
#endif    
	int		battlecharaindex[CONNECT_WINDOWBUFSIZE];
	int		duelcharaindex[CONNECT_WINDOWBUFSIZE];			
	int		tradecardcharaindex[CONNECT_WINDOWBUFSIZE];		
	int		joinpartycharaindex[CONNECT_WINDOWBUFSIZE];		

	// CoolFish: Trade 2001/4/18
	int		tradecharaindex[CONNECT_WINDOWBUFSIZE];
    int     errornum;
    int     fdid;
    
    int     close_request; //the second have this
    
    int appendwb_overflow_flag;  /* 1荚匹手appendWb互撩  仄凶日1卞允月 */
    //ttom+1 avoidance the watch the battle be kept out
    BOOL in_watch_mode;
    BOOL b_shut_up;//for avoid the user wash the screen
    BOOL b_pass;      //for avoid the unlimited area
    struct timeval Wtime;
    struct timeval WLtime;
    BOOL b_first_warp;
    int  state_trans;
    
    // CoolFish: Trade 2001/4/18
    char TradeTmp[256];

#ifdef _ITEM_PILEFORTRADE
	int tradelist;
#endif
    // Shan Recvdata Time
    struct timeval lastrecvtime;      // 'FM' Stream Control time
    struct timeval lastrecvtime_d;    // DENGON Talk Control time

    // Arminius: 6.22 encounter
    int CEP;	// Current Encounter Probability
    // Arminius 7.12 login announce
    int announced;

	// shan battle delay time 2001/12/26
    struct timeval battle_recvtime;
#ifdef _NO_WARP
	// shan hjj add Begin 
	int seqno;
    int selectbutton;
	// shan End
#endif	
	BOOL confirm_key;    // shan  trade(DoubleCheck)

#ifdef _BLACK_MARKET
	int  BMSellList[12];
#endif
}
CONNECT;

CONNECT *Connect;      /*戊生弁扑亦件忡切迕*/


/* 楮醒及燮  卞勾仃化歹井月方丹卞允月分仃及穴弁夫 */
#define SINGLETHREAD
#define MUTLITHREAD
#define ANYTHREAD

ServerState servstate;
#if USE_MTIO
pthread_mutex_t MTIO_servstate_m;     /* servstate 及夫永弁 */
#define SERVSTATE_LOCK() pthread_mutex_lock( &MTIO_servstate_m );
#define SERVSTATE_UNLOCK() pthread_mutex_unlock( &MTIO_servstate_m );

#if 1
#define CONNECT_LOCK_ARG2(i,j) fprintf(stderr,"LO T:%d(%d:%d) %s %d", (int)pthread_self(), i,j, __FILE__,__LINE__ );pthread_mutex_lock( &Connect[i].mutex );fprintf( stderr, "CK T:%d(%d:%d)\n" , (int)pthread_self(), i,j );
#define CONNECT_UNLOCK_ARG2(i,j) fprintf(stderr,"UNLO T:%d(%d:%d) %s %d", (int)pthread_self(), i,j, __FILE__,__LINE__ );pthread_mutex_unlock( &Connect[i].mutex );fprintf( stderr, "CK T:%d(%d:%d)\n",(int)pthread_self(), i,j);
#define CONNECT_LOCK(i) fprintf(stderr,"LO T:%d(%d) %s %d", (int)pthread_self(), i, __FILE__,__LINE__ );pthread_mutex_lock( &Connect[i].mutex );fprintf( stderr, "CK T:%d(%d)\n" ,(int)pthread_self(), i );
#define CONNECT_UNLOCK(i) fprintf(stderr,"UNLO T:%d(%d) %s %d", (int)pthread_self(), i, __FILE__,__LINE__ );pthread_mutex_unlock( &Connect[i].mutex );fprintf( stderr, "CK T:%d(%d)\n",(int)pthread_self(), i);
/* 晓筏及夫弘反户切扎仁切扎犯奴旦弁毛罹丹及匹旦伉□皿毛中木凶曰 */
#define MTIO_DEBUG_LOG_REDUCE 1
#else
#define CONNECT_LOCK_ARG2(i,j) pthread_mutex_lock( &Connect[i].mutex );
#define CONNECT_UNLOCK_ARG2(i,j) pthread_mutex_unlock( &Connect[i].mutex );
#define CONNECT_LOCK(i) pthread_mutex_lock( &Connect[i].mutex );
#define CONNECT_UNLOCK(i) pthread_mutex_unlock( &Connect[i].mutex );
#define MTIO_DEBUG_LOG_REDUCE 0
#endif

#else
#define SERVSTATE_LOCK()
#define SERVSTATE_UNLOCK()
#define CONNECT_LOCK_ARG2(i,j) 
#define CONNECT_UNLOCK_ARG2(i,j)
#define CONNECT_LOCK(i) 
#define CONNECT_UNLOCK(i)
#endif

#ifdef _CHECK_BATTLE_IO
int InBattleLoop =FALSE;
int battle_write =0;
int other_write =0;
int battle_write_cnt =0;
int other_write_cnt =0;
#endif



/*------------------------------------------------------------
 * servstate毛赓渝祭允月［
 * 娄醒｝忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
ANYTHREAD static void SERVSTATE_initserverState( void )
{
    SERVSTATE_LOCK();
    servstate.acceptmore = TRUE;
    servstate.fdid = 0;
    servstate.closeallsocketnum = -1;
	servstate.shutdown = 0;
	servstate.limittime = 0;
	servstate.dsptime = 0;
    SERVSTATE_UNLOCK();    
}

ANYTHREAD int SERVSTATE_SetAcceptMore( int nvalue )
{
    BOOL buf;
    SERVSTATE_LOCK();
    buf = servstate.acceptmore;
    servstate.acceptmore = nvalue;
    SERVSTATE_UNLOCK();
    return buf;
}
ANYTHREAD static int SERVSTATE_incrementFdid( void )
{
    int ret;
    SERVSTATE_LOCK();
    ret = servstate.fdid++;
    SERVSTATE_UNLOCK();
    return ret;
}
ANYTHREAD static void SERVSTATE_setCloseallsocketnum( int a )
{
    SERVSTATE_LOCK();
    servstate.closeallsocketnum = a;
    SERVSTATE_UNLOCK();
}
ANYTHREAD static void SERVSTATE_incrementCloseallsocketnum(void)
{
    SERVSTATE_LOCK();
    servstate.closeallsocketnum ++;
    SERVSTATE_UNLOCK();    
}
ANYTHREAD void SERVSTATE_decrementCloseallsocketnum(void)
{
    SERVSTATE_LOCK();
    servstate.closeallsocketnum --;
    SERVSTATE_UNLOCK();    
}
ANYTHREAD int SERVSTATE_getCloseallsocketnum( void )
{
    int a;
    SERVSTATE_LOCK();
    a = servstate.closeallsocketnum;
    SERVSTATE_UNLOCK();
    return a;
}

ANYTHREAD static int SERVSTATE_getAcceptmore(void)
{
    int a;
    SERVSTATE_LOCK();
    a = servstate.acceptmore;
    SERVSTATE_UNLOCK();
    return a;
}
ANYTHREAD int SERVSTATE_getShutdown(void)
{
    int a;
    SERVSTATE_LOCK();
    a = servstate.shutdown;
    SERVSTATE_UNLOCK();
    return a;    
}
ANYTHREAD void SERVSTATE_setShutdown(int a)
{
    SERVSTATE_LOCK();
    servstate.shutdown = a;
    SERVSTATE_UNLOCK();    
}
ANYTHREAD int SERVSTATE_getLimittime(void)
{
    int a;
    SERVSTATE_LOCK();
    a = servstate.limittime;
    SERVSTATE_UNLOCK();
    return a;        
}
ANYTHREAD void SERVSTATE_setLimittime(int a)
{
    SERVSTATE_LOCK();
    servstate.limittime = a;
    SERVSTATE_UNLOCK();
}
ANYTHREAD int SERVSTATE_getDsptime(void)
{
    int a;
    SERVSTATE_LOCK();
    a = servstate.dsptime;
    SERVSTATE_UNLOCK();
    return a;        
}
ANYTHREAD void SERVSTATE_setDsptime(int a)
{
    SERVSTATE_LOCK();
    servstate.dsptime = a;
    SERVSTATE_UNLOCK();
}


#if USE_MTIO == 0

static int appendWB( int fd, char *buf, int size )
{

	if( fd != acfd ) {
	    if( Connect[fd].wbuse + size >= WBSIZE ) {
			print( "appendWB:err buffer over[%d]:%s \n",
				Connect[fd].wbuse + size, Connect[fd].cdkey );	
	    	return -1;
	    }
	}else {
	    if( Connect[fd].wbuse + size > AC_WBSIZE ) {
			FILE *fp=NULL;
			print( "appendWB:err buffer over[%d+%d]:(SAAC) \n", Connect[fd].wbuse, size);
			if( (fp=fopen("appendWBerr.log", "a+"))==NULL) return -1;
			fprintf( fp, "(SAAC) appendWB:err buffer over[%d+%d/%d]:\n", Connect[fd].wbuse, size, AC_WBSIZE);
			fclose( fp);
	    	return -1;
	    }
	}


	
    memcpy( Connect[fd].wb + Connect[fd].wbuse ,
            buf, size );
    Connect[fd].wbuse += size;
    return size;
}
static int appendRB( int fd, char *buf, int size )
{
	if( fd != acfd ) {
	    if( Connect[fd].rbuse + size > RBSIZE ) {
			if( fd == mfd )	print( "appendRB:MSERVER err buffer over \n");
			else print( "appendRB:OTHER(%d) err buffer over \n", fd);
			return -1;
	    }
	}else {
		if( strlen( buf) > size ){
			print( "appendRB AC buffer len err : %d/%d=\n(%s)!!\n", strlen( buf), size, buf);
		}
	    if( Connect[fd].rbuse + size > AC_RBSIZE ) {
			print( "appendRB AC err buffer over:\n(%s)\n len:%d - rbuse:%d \n",
				buf, strlen(buf), Connect[fd].rbuse);
	    	return -1;
	    }
	}
    memcpy( Connect[fd].rb + Connect[fd].rbuse , buf, size );
    Connect[fd].rbuse += size;
    return size;
}

static int shiftWB( int fd, int len )
{
    if( Connect[fd].wbuse < len ) {
		print( "shiftWB: err\n");
    	return -1;
    }
    memmove( Connect[fd].wb, Connect[fd].wb + len, Connect[fd].wbuse - len  );
    Connect[fd].wbuse -= len;
	if( Connect[fd].wbuse < 0 ) {
		print( "shiftWB:wbuse err\n");
		Connect[fd].wbuse = 0;
	}
    return len;
}
static int shiftRB( int fd, int len )
{
    if( Connect[fd].rbuse < len ) {
		print( "shiftRB: err\n");
    	return -1;
    }
    memmove( Connect[fd].rb, Connect[fd].rb + len, Connect[fd].rbuse - len  );
    Connect[fd].rbuse -= len;
	if( Connect[fd].rbuse < 0 ) {
		print( "shiftRB:rbuse err\n");
		Connect[fd].rbuse = 0;
	}

    return len;
}

SINGLETHREAD int lsrpcClientWriteFunc( int fd , char* buf , int size )
{
    int r;
    if( Connect[fd].use == FALSE ){
        return FALSE;
    }
    if( Connect[fd].appendwb_overflow_flag ){
        print( "lsrpcClientWriteFunc: buffer overflow fd:%d\n" , fd );
        return -1;
    }
    r = appendWB( fd,  buf ,  size);

    // Nuke *1 0907: Ignore acfd from WB error
    if(( r < 0 ) && (fd != acfd)) {
        Connect[fd].appendwb_overflow_flag = 1;
        CONNECT_endOne_debug(fd);
        close(fd);
        // Nuke + 1 0901: Why close
       //  print("closed in lsrpcClientWriteFunc");
    }
    return r;
}

static int logRBuseErr = 0;
SINGLETHREAD BOOL GetOneLine_fix( int fd, char *buf, int max )
{
    int i;
    if( Connect[fd].rbuse == 0 ) return FALSE;

    if( Connect[fd].check_rb_oneline_b == 0 && 
		Connect[fd].check_rb_oneline_b == Connect[fd].rbuse ){
		return FALSE;
	}


    for( i = 0; i < Connect[fd].rbuse && i < ( max -1); i ++ ){
        if( Connect[fd].rb[i] == '\n' ){
 			memcpy( buf, Connect[fd].rb, i+1);
            buf[i+1]='\0';
            shiftRB( fd, i+1 );

//--------
/*
			//andy_log
			if( strstr( Connect[fd].rb , "ACCharLoad") != NULL &&
				Connect[fd].check_rb_oneline_b != 0 )//Connect[fd].rb
				LogAcMess( fd, "GetOne", Connect[fd].rb );
*/
//--------
			logRBuseErr = 0;
			Connect[fd].check_rb_oneline_b=0;
			Connect[fd].check_rb_time = 0;
            return TRUE;
        }
    }

	//print("rbuse lens: %d!!\n", Connect[fd].rbuse);
	logRBuseErr++;
//--------
	//andy_log
	if( fd == acfd && strstr( Connect[fd].rb , "ACCharLoad") != NULL && 
		logRBuseErr >= 50 ){//Connect[fd].rb
		char buf[AC_RBSIZE];
		memcpy( buf, Connect[fd].rb, Connect[fd].rbuse+1);
		buf[Connect[fd].rbuse+1]=0;
		LogAcMess( fd, "RBUFFER", buf );
		logRBuseErr=0;
	}
//--------
    Connect[fd].check_rb_oneline_b = Connect[fd].rbuse;

    return FALSE;
}

#endif /* if USE_MTIO == 0*/

ANYTHREAD BOOL initConnectOne( int sockfd, struct sockaddr_in* sin ,int len )
{
    CONNECT_LOCK(sockfd);

    Connect[sockfd].use = TRUE;
    Connect[sockfd].ctype = NOTDETECTED;
#if USE_MTIO    
    Connect[sockfd].closed = 0;    
#else
	Connect[sockfd].wbuse = Connect[sockfd].rbuse = 0;
	Connect[sockfd].check_rb_oneline_b = 0;
	Connect[sockfd].check_rb_time = 0;
#endif



    memset( Connect[sockfd].cdkey , 0 , sizeof( Connect[sockfd].cdkey ) );
    memset( Connect[sockfd].passwd, 0 , sizeof( Connect[sockfd].passwd) );

    Connect[sockfd].state = NOTLOGIN;
	Connect[sockfd].nstatecount = 0;
    memset( Connect[sockfd].charname,0, sizeof(Connect[sockfd].charname));
    Connect[sockfd].charaindex = -1;

    Connect[sockfd].CAbufsiz = 0;
    Connect[sockfd].CDbufsiz = 0;
    Connect[sockfd].rbuse = 0;
    Connect[sockfd].wbuse = 0;
    Connect[sockfd].check_rb_oneline_b = 0;
	Connect[sockfd].check_rb_time = 0;
    
     Connect[sockfd].close_request = 0;      /* 濠蝇邰菲白仿弘 */
    // Nuke 08/27 For acceleration avoidance
    Connect[sockfd].Walktime = 0;
    Connect[sockfd].lastWalktime = 0;
    Connect[sockfd].Walkcount = 0;
    Connect[sockfd].Walkspool = WALK_SPOOL;
    Connect[sockfd].Walkrestore = WALK_RESTORE;
    Connect[sockfd].Btime = 0;
    Connect[sockfd].lastBtime = 0;
    Connect[sockfd].lastlastBtime = 0;
    Connect[sockfd].EOtime = 0;
	Connect[sockfd].nu_decrease = 0;
#ifdef _BATTLE_TIMESPEED
//	Connect[sockfd].DefBtime = 0;
	Connect[sockfd].BDTime = 0;
	Connect[sockfd].CBTime = 0;
#endif
#ifdef _CHECK_GAMESPEED
	Connect[sockfd].gamespeed = 0;
#endif
#ifdef _TYPE_TOXICATION
	Connect[sockfd].toxication = 0;
#endif
#ifdef _ITEM_ADDEXP	//vincent 经验提升
	Connect[sockfd].EDTime = 0;
#endif
//      Connect[sockfd].BEO = 0;
    Connect[sockfd].BEOspool = BEO_SPOOL;
    Connect[sockfd].BEOrestore = BEO_RESTORE;
    //ttom
    Connect[sockfd].b_shut_up=FALSE;
    Connect[sockfd].Wtime.tv_sec=0;//
    Connect[sockfd].Wtime.tv_usec=0;//
    Connect[sockfd].WLtime.tv_sec=0;//
    Connect[sockfd].WLtime.tv_usec=0;//
    Connect[sockfd].b_first_warp=FALSE;
    Connect[sockfd].state_trans=0;//avoid the trans 
    // Nuke
    Connect[sockfd].die=0;
    Connect[sockfd].credit=3;
    Connect[sockfd].fcold=0;
    // Nuke 0406: New Flow Control
    Connect[sockfd].nu=30;
    Connect[sockfd].ke=10;
    // Nuke 1213: Flow Control 2
    Connect[sockfd].packetin=30; // if 10x10 seconds no packet, drop the line
    
    // Nuke 0624: Avoid Useless Connection
    Connect[sockfd].cotime=0;
    // Nuke 0626: For no enemy
    Connect[sockfd].noenemy=0;
    // Arminius 7.2: Ra's amulet
    Connect[sockfd].eqnoenemy = 0;

#ifdef _Item_MoonAct
	Connect[sockfd].eqrandenemy = 0;
#endif
#ifdef _CHIKULA_STONE
	Connect[sockfd].chistone = 0;
#endif
    // Arminius 7.31: cursed stone
    Connect[sockfd].stayencount = 0;

    // CoolFish: Init Trade 2001/4/18
    memset(&Connect[sockfd].TradeTmp, 0, sizeof(Connect[sockfd].TradeTmp));
#ifdef _ITEM_PILEFORTRADE
	Connect[sockfd].tradelist = -1;
#endif
    // Arminius 6.22 Encounter
    Connect[sockfd].CEP = 0;

    // Arminius 7.12 login announce
    Connect[sockfd].announced=0;
#ifdef _NO_WARP
    // shan hjj add Begin
    Connect[sockfd].seqno=-1;
    Connect[sockfd].selectbutton=1;
	// shan End
#endif
	Connect[sockfd].confirm_key=FALSE;   // shan trade(DoubleCheck)

    if( sin != NULL )memcpy( &Connect[sockfd].sin , sin  , len );
    memset( &Connect[sockfd].lastprocesstime, 0 ,
            sizeof(Connect[sockfd].lastprocesstime) );
    memcpy( &Connect[sockfd].lastCAsendtime, &NowTime ,
            sizeof(Connect[sockfd].lastCAsendtime) );
    memcpy( &Connect[sockfd].lastCDsendtime, &NowTime ,
            sizeof(Connect[sockfd].lastCDsendtime) );
    memcpy( &Connect[sockfd].lastCharSaveTime, &NowTime ,
            sizeof(Connect[sockfd].lastCharSaveTime) );
    // Shan Add
    memcpy( &Connect[sockfd].lastrecvtime, &NowTime ,
            sizeof(Connect[sockfd].lastrecvtime) );
    memcpy( &Connect[sockfd].lastrecvtime_d, &NowTime ,
            sizeof(Connect[sockfd].lastrecvtime_d) );
	memcpy( &Connect[sockfd].battle_recvtime, &NowTime ,
            sizeof(Connect[sockfd].battle_recvtime) );

#ifdef _BLACK_MARKET
	{
		int i;
		for(i=0; i<12; i++)
			Connect[sockfd].BMSellList[i] = -1;
	}
#endif
               
    memcpy( &Connect[sockfd].lastreadtime , &NowTime,
            sizeof(struct timeval));
    Connect[sockfd].lastreadtime.tv_sec -= DEBUG_ADJUSTTIME;

    Connect[sockfd].errornum = 0;
    Connect[sockfd].fdid = SERVSTATE_incrementFdid();

    CONNECT_UNLOCK(sockfd);

    Connect[sockfd].appendwb_overflow_flag = 0;
    return TRUE;
}

ANYTHREAD BOOL _CONNECT_endOne( char *file, int fromline, int sockfd , int line )
{
    CONNECT_LOCK_ARG2(sockfd,line);

    if( Connect[sockfd].use == FALSE ){
        CONNECT_UNLOCK_ARG2(sockfd,line);
		//andy_log
		print("连接已关闭[%d]!!\n", sockfd );
        return TRUE;
    }
	Connect[sockfd].use = FALSE;
    if( Connect[sockfd].ctype == CLI && Connect[sockfd].charaindex >= 0 ){
        CONNECT_UNLOCK_ARG2( sockfd,line );
        if( !CHAR_logout( sockfd,TRUE )) {
        	print( "err %s:%d from %s:%d \n", __FILE__, __LINE__, file, fromline);
        }
        CONNECT_LOCK_ARG2( sockfd ,line);
    }
    
    unsigned long ip;
    int a,b,c,d;
    int i;
    ip=CONNECT_get_userip(sockfd);
    a=(ip % 0x100); ip=ip / 0x100;
    b=(ip % 0x100); ip=ip / 0x100;
    c=(ip % 0x100); ip=ip / 0x100;
    d=(ip % 0x100);
    
    print( "账号=%s 编号=%d\n", Connect[sockfd].cdkey,sockfd );

		for(i=0;i<getMaxLockip();i++)
			 if(cmpLockip(i,a,b,c,d,0))return;
#if USE_MTIO == 0
    Connect[sockfd].rbuse = Connect[sockfd].wbuse = 0;
#else
    Connect[sockfd].wbuse = Connect[sockfd].rbuse = 0;
#endif
    Connect[sockfd].CAbufsiz = 0;
    Connect[sockfd].CDbufsiz = 0;
    CONNECT_UNLOCK_ARG2(sockfd,line);
    return TRUE;
}

SINGLETHREAD BOOL initConnect( int size )
{
    int i,j;
    ConnectLen = size;
    Connect = calloc( 1, sizeof( CONNECT ) * size );
    if( Connect == NULL )return FALSE;
    for( i = 0 ; i < size ; i ++ ){
        memset( &Connect[i] , 0 , sizeof( CONNECT ) );
        Connect[i].charaindex = -1;
		Connect[i].rb = calloc( 1, RBSIZE);
		if( Connect[i].rb == NULL ) {
			fprint( "calloc err\n");
			for( j = 0; j < i ; j ++ ) {
				free( Connect[j].rb);
				free( Connect[j].wb);
			}
			return FALSE;
		}
		memset( Connect[i].rb, 0, RBSIZE);
		Connect[i].wb = calloc( 1, WBSIZE);
		if( Connect[i].wb == NULL ) {
			fprint( "calloc err\n");
			for( j = 0; j < i ; j ++ ) {
				free( Connect[j].rb);
				free( Connect[j].wb);
			}
			free( Connect[j].rb);
			return FALSE;
		}
		memset( Connect[i].wb, 0, WBSIZE);

    }
#if USE_MTIO
    for( i = 0 ;i < size ; i ++ ){
        pthread_mutex_init( & Connect[i].mutex , NULL );
    }
#endif
    
    SERVSTATE_initserverState( );
    //ttom for the performance of gmsv
    MAX_item_use=getItemnum()*0.98;
            
    return TRUE;
}
BOOL CONNECT_acfdInitRB( int fd )
{
	if( fd != acfd ) return FALSE;
	Connect[fd].rb = realloc( Connect[acfd].rb, AC_RBSIZE);
	if( Connect[acfd].rb == NULL ) {
		fprint( "realloc err\n");
		return FALSE;
	}
	memset( Connect[acfd].rb, 0, AC_RBSIZE);
	return TRUE;
}
BOOL CONNECT_acfdInitWB( int fd )
{
	if( fd != acfd ) return FALSE;
	Connect[fd].wb = realloc( Connect[acfd].wb, AC_WBSIZE);
	if( Connect[acfd].wb == NULL ) {
		fprint( "realloc err\n");
		return FALSE;
	}
	memset( Connect[acfd].wb, 0, AC_WBSIZE);
	return TRUE;
}

ANYTHREAD void endConnect( void )
{
    int i;
    for(i = 0 ; i < ConnectLen  ; i ++ ){
        int lco;
        lco = close( i );
        if( lco == 0 ){
            CONNECT_endOne_debug(i);
        }
        free( Connect[i].rb);
        free( Connect[i].wb);
    }
    free(Connect);
}

ANYTHREAD BOOL CONNECT_appendCAbuf( int fd , char* data, int size )
{

    CONNECT_LOCK(fd);
    /*      及犯伉立正及 ',' 及坌聂仁割忡仄卅中井氏仪卞镗啦   */
    if( (Connect[fd].CAbufsiz + size) >= sizeof( Connect[fd].CAbuf ) ){
        CONNECT_UNLOCK(fd);
        return FALSE;
    }
    
    memcpy( Connect[fd].CAbuf + Connect[fd].CAbufsiz  , data , size );
    Connect[fd].CAbuf[Connect[fd].CAbufsiz+size]=',';
    Connect[fd].CAbufsiz += (size + 1);
    CONNECT_UNLOCK(fd);
    return TRUE;
}

ANYTHREAD static int CONNECT_getCAbuf( int fd, char *out, int outmax,
                                       int *outlen )
{
    
    CONNECT_LOCK(fd);
    if( Connect[fd].use == TRUE ){
        int cplen = MIN( outmax, Connect[fd].CAbufsiz );
        memcpy( out, Connect[fd].CAbuf , cplen );
        *outlen = cplen;
        CONNECT_UNLOCK(fd);
        return 0;
    } else {
        CONNECT_UNLOCK(fd);
        return -1;
    }
}
ANYTHREAD static int CONNECT_getCDbuf( int fd, char *out, int outmax,
                                       int *outlen )
{
    CONNECT_LOCK(fd);
    if( Connect[fd].use == TRUE ){
        int cplen = MIN( outmax, Connect[fd].CDbufsiz );
        memcpy( out, Connect[fd].CDbuf, cplen );
        *outlen = cplen;
        CONNECT_UNLOCK(fd);
        return 0;
    } else {
        CONNECT_UNLOCK(fd);
        return 0;
    }
}

ANYTHREAD static int CONNECT_setCAbufsiz( int fd, int len )
{
    CONNECT_LOCK(fd);
    if( Connect[fd].use == TRUE ){
        Connect[fd].CAbufsiz = len;
        CONNECT_UNLOCK(fd);
        return 0;
    } else {
        CONNECT_UNLOCK(fd);
        return -1;
    }
}
ANYTHREAD static int CONNECT_setCDbufsiz( int fd, int len )
{
    CONNECT_LOCK(fd);
    if( Connect[fd].use == TRUE ){
        Connect[fd].CDbufsiz = len;
        CONNECT_UNLOCK(fd);
        return 0;
    } else {
        CONNECT_UNLOCK(fd);
        return -1;
    }    
}

ANYTHREAD static void CONNECT_setLastCAsendtime( int fd, struct timeval *t)
{
    CONNECT_LOCK(fd);
    Connect[fd].lastCAsendtime = *t;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD static void CONNECT_getLastCAsendtime( int fd, struct timeval *t )
{
    CONNECT_LOCK(fd);
    *t = Connect[fd].lastCAsendtime;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD static void CONNECT_setLastCDsendtime( int fd, struct timeval *t )
{
    CONNECT_LOCK(fd);
    Connect[fd].lastCDsendtime = *t;
    CONNECT_UNLOCK(fd);    
}
ANYTHREAD static void CONNECT_getLastCDsendtime( int fd, struct timeval *t )
{
    CONNECT_LOCK(fd);
    *t = Connect[fd].lastCDsendtime;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getUse_debug( int fd, int i )
{
    int a;
    CONNECT_LOCK_ARG2(fd,i);
    a = Connect[fd].use;
    CONNECT_UNLOCK_ARG2(fd,i);
    return a;
    
}

ANYTHREAD int CONNECT_getUse( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].use;
    CONNECT_UNLOCK(fd);
    return a;
}
void CONNECT_setUse( int fd , int a)
//ANYTHREAD static void CONNECT_setUse( int fd , int a)
{
    CONNECT_LOCK(fd);
    Connect[fd].use = a;
    CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_checkStatecount( int a )
{
	int i;
	int count=0;
	for( i=0; i < ConnectLen; i++ ){
		if( Connect[i].use == FALSE || Connect[i].state != a ) continue;
		if( Connect[i].nstatecount <= 0 ){
			Connect[i].nstatecount=(int)time(NULL)+60;
		}else{
			if( Connect[i].nstatecount < (int)time(NULL) ){
				CONNECT_endOne_debug(i);
				close( i );
				count++;
			}
		}
	}
	{
		memset(StateTable, 0, sizeof(StateTable));
		for (i=0; i < ConnectLen; i++)
			if (Connect[i].use == TRUE)
				StateTable[Connect[i].state]++;
	}
}

ANYTHREAD int CONNECT_checkStateSomeOne( int a, int maxcount)
{
	char temp[80],buffer[1024];
	int i, ret=1;

	if( StateTable[a] >= maxcount ) ret =-1;
	buffer[0]=0;
	for (i=0; i <= WHILESAVEWAIT; i++){
		sprintf(temp, "%4d", StateTable[i]);
		strcat(buffer, temp);
	}
	print("\nNOW{{%s}}", buffer);
	return ret;
}

ANYTHREAD void CONNECT_setState( int fd, int a )
{
    CONNECT_LOCK(fd);
    Connect[fd].state = a;
	Connect[fd].nstatecount = 0;

    // Nuke start 0829: For debugging
    {
       char temp[80],buffer[1024];
       int i;
       memset(StateTable, 0, sizeof(StateTable));
       for (i=0; i < ConnectLen; i++)
          if (Connect[i].use == TRUE)
             StateTable[Connect[i].state]++;

       buffer[0]=0;
       for (i=0; i <= WHILESAVEWAIT; i++){
            sprintf(temp, "%4d", StateTable[i]);
            strcat(buffer, temp);
       }
       print("\n{{%s}}", buffer);
    }
    // Nuke end
                                                                                                                                                            
    CONNECT_UNLOCK(fd);
}

ANYTHREAD int CONNECT_getState( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].state;
    CONNECT_UNLOCK(fd);
    return a;
}
ANYTHREAD void CONNECT_incrementErrornum(int fd )
{
    CONNECT_LOCK(fd);
    Connect[fd].errornum ++;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_setCharaindex( int fd, int a )
{
    CONNECT_LOCK(fd);
    Connect[fd].charaindex = a;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getCharaindex( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].charaindex;
    CONNECT_UNLOCK(fd);
    return a;
}
ANYTHREAD void CONNECT_getCdkey( int fd , char *out, int outlen )
{
    CONNECT_LOCK(fd);
    strcpysafe( out, outlen, Connect[fd].cdkey );
    CONNECT_UNLOCK(fd);
}
ANYTHREAD void
CONNECT_setCdkey( int sockfd, char *cd )
{
    CONNECT_LOCK(sockfd);
    snprintf( Connect[sockfd].cdkey, sizeof( Connect[sockfd].cdkey ),"%s",
              cd );
    CONNECT_UNLOCK(sockfd);
}

ANYTHREAD void CONNECT_getPasswd( int fd , char *out, int outlen )
{
    CONNECT_LOCK(fd);
    strcpysafe( out, outlen, Connect[fd].passwd );
    CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_setPasswd( int fd, char *in )
{
    CONNECT_LOCK(fd);
    strcpysafe( Connect[fd].passwd, sizeof( Connect[fd].passwd ), in );
    CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getCtype( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].ctype;
    CONNECT_UNLOCK(fd);
    return a;
}
ANYTHREAD void CONNECT_setCtype( int fd , int a )
{
    CONNECT_LOCK(fd);
    Connect[fd].ctype = a;
    CONNECT_UNLOCK(fd);    
}

ANYTHREAD void CONNECT_getCharname( int fd , char *out, int outlen )
{
    CONNECT_LOCK(fd);
    strcpysafe( out, outlen, Connect[fd].charname );
    CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_setCharname( int fd, char *in )
{
    CONNECT_LOCK(fd);
    strcpysafe( Connect[fd].charname, sizeof( Connect[fd].charname ),
                in );
    CONNECT_UNLOCK(fd);
}

ANYTHREAD int CONNECT_getFdid( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].fdid;
    CONNECT_UNLOCK(fd);
    return a;
}
ANYTHREAD void CONNECT_setDuelcharaindex( int fd, int i , int a )
{
    CONNECT_LOCK(fd);
    Connect[fd].duelcharaindex[i]=a;
    CONNECT_UNLOCK(fd);    
}
ANYTHREAD int CONNECT_getDuelcharaindex( int fd, int i )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].duelcharaindex[i];
    CONNECT_UNLOCK(fd);
    return a;
}
ANYTHREAD void CONNECT_setBattlecharaindex( int fd, int i , int a )
{
    CONNECT_LOCK(fd);
    Connect[fd].battlecharaindex[i] = a;
    CONNECT_UNLOCK(fd);    
}
ANYTHREAD int CONNECT_getBattlecharaindex( int fd, int i )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].battlecharaindex[i];
    CONNECT_UNLOCK(fd);
    return a;
}
ANYTHREAD void CONNECT_setJoinpartycharaindex( int fd, int i , int a)
{
    CONNECT_LOCK(fd);
    Connect[fd].joinpartycharaindex[i]=a;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getJoinpartycharaindex( int fd, int i )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].joinpartycharaindex[i];
    CONNECT_UNLOCK(fd);
    return a;
}

// CoolFish: Trade 2001/4/18
ANYTHREAD void CONNECT_setTradecharaindex( int fd, int i , int a )
{
    CONNECT_LOCK(fd);
    Connect[fd].tradecharaindex[i] = a;
    CONNECT_UNLOCK(fd);
}

// Shan Begin
ANYTHREAD void CONNECT_setLastrecvtime( int fd, struct timeval *a )
{
    CONNECT_LOCK(fd);
    Connect[fd].lastrecvtime = *a;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getLastrecvtime( int fd, struct timeval *a )
{
    CONNECT_LOCK(fd);
    *a = Connect[fd].lastrecvtime;
    CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setLastrecvtime_D( int fd, struct timeval *a )
{
    CONNECT_LOCK(fd);
    Connect[fd].lastrecvtime_d = *a;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getLastrecvtime_D( int fd, struct timeval *a )
{
    CONNECT_LOCK(fd);
    *a = Connect[fd].lastrecvtime_d;
    CONNECT_UNLOCK(fd);
}
// 2001/12/26
ANYTHREAD void CONNECT_SetBattleRecvTime( int fd, struct timeval *a )
{
    CONNECT_LOCK(fd);
    Connect[fd].battle_recvtime = *a;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_GetBattleRecvTime( int fd, struct timeval *a )
{
    CONNECT_LOCK(fd);
    *a = Connect[fd].battle_recvtime;
    CONNECT_UNLOCK(fd);
}
// Shan End


#ifdef _ITEM_PILEFORTRADE
ANYTHREAD void CONNECT_setTradeList( int fd, int num)
{
	Connect[fd].tradelist = num;
}
ANYTHREAD int CONNECT_getTradeList(int fd)
{
	return Connect[fd].tradelist;
}
#endif

ANYTHREAD void CONNECT_setTradeTmp(int fd, char* a)
{
	CONNECT_LOCK(fd);
		strcpysafe( Connect[fd].TradeTmp, sizeof(Connect[fd].TradeTmp), a);

        CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getTradeTmp(int fd, char *trademsg, int trademsglen)
{
	CONNECT_LOCK(fd);
        strcpysafe(trademsg, trademsglen, Connect[fd].TradeTmp);
        CONNECT_UNLOCK(fd);
}
                                            
ANYTHREAD void CONNECT_setTradecardcharaindex( int fd, int i , int a )
{
    CONNECT_LOCK(fd);
    Connect[fd].joinpartycharaindex[i] = a;
    CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getTradecardcharaindex( int fd, int i )
{
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].joinpartycharaindex[i];
    CONNECT_UNLOCK(fd);
    return a;
}
ANYTHREAD int CONNECT_getClosed( int fd )
{
#if USE_MTIO    
    int a;
    CONNECT_LOCK(fd);
    a = Connect[fd].closed;
    CONNECT_UNLOCK(fd);
    return a;
#else
    return 0;
#endif
}
ANYTHREAD void CONNECT_setClosed( int fd, int a )
{
#if USE_MTIO    
    CONNECT_LOCK(fd);
    if( !Connect[fd].use){
        CONNECT_UNLOCK(fd);            
        return;
    }
    Connect[fd].closed =a;
    CONNECT_UNLOCK(fd);
#endif    
}
ANYTHREAD void CONNECT_setCloseRequest( int fd, int count )
{
    CONNECT_LOCK(fd);
    Connect[fd].close_request = count;
    // Nuke
    print("关闭请求设置为 %d ",fd);
    CONNECT_UNLOCK(fd);
}
                       

/*------------------------------------------------------------
 * CAcheck 卅升卞银歹木月楮醒［  端卞霜月［
 * 娄醒
 *  fd      int     白央奶伙犯奴旦弁伉皿正
 * 忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
ANYTHREAD void CAsend( int fd )
{
    char buf[sizeof(Connect[0].CAbuf)];
    int bufuse=0;

    if( CONNECT_getCAbuf( fd, buf, sizeof(buf), &bufuse ) < 0 )return;
    if( bufuse == 0 )return;

    //print("\nshan--->(CAsend)->%s fd->%d", buf, fd);

    /*    及犯伉立正 ',' 毛'\0' 午井尹月*/
    buf[bufuse-1] = '\0';
    lssproto_CA_send( fd , buf );

    CONNECT_setCAbufsiz( fd, 0 );	
}


/*------------------------------------------------------------
 * CA毛霜月［
 * 娄醒
 * 忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
ANYTHREAD void CAcheck( void )
{
    int     i;
    unsigned int interval_us = getCAsendinterval_ms()*1000;

    /* Connect及蜊醒坌分仃支月井日褐中氏分卅［ */
    for( i = 0; i < ConnectLen; i ++) {
        struct timeval t;
        if( !CONNECT_getUse_debug(i,1008) )continue;
        CONNECT_getLastCAsendtime( i, &t );
        if( time_diff_us( NowTime, t ) > interval_us ){
            CAsend( i);
            CONNECT_setLastCAsendtime( i, &NowTime );
        }
    }
}
ANYTHREAD void CAflush( int charaindex )
{
    int i;
    i = getfdFromCharaIndex( charaindex);
    if( i == -1 )return;
    CAsend(i);
}


/*------------------------------------------------------------
 * CDbuf 卞馨笛允月［
 * 娄醒
 *  fd      int     白央奶伙犯奴旦弁伉皿正
 *  data    char*   犯□正
 *  size    int     犯□正及扔奶术
 * 忒曰袄
 *  岳      TRUE(1)
 *  撩      FALSE(0)
 ------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_appendCDbuf( int fd , char* data, int size )
{
    CONNECT_LOCK(fd);

    if( ( Connect[fd].CDbufsiz + size ) >= sizeof( Connect[fd].CDbuf )){
        CONNECT_UNLOCK(fd);
        return FALSE;
    }
    memcpy( Connect[fd].CDbuf + Connect[fd].CDbufsiz , data, size );
    Connect[fd].CDbuf[Connect[fd].CDbufsiz+size] = ',';
    Connect[fd].CDbufsiz += ( size + 1 );
    CONNECT_UNLOCK(fd);
    return TRUE;
}


/*------------------------------------------------------------
 * CDcheck 卅升卞银歹木月楮醒［  端卞霜月［
 * 娄醒
 *  fd      int     白央奶伙犯奴旦弁伉皿正
 * 忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
ANYTHREAD void CDsend( int fd )
{
    char buf[sizeof(Connect[0].CAbuf )];
    int bufuse=0;
    if( CONNECT_getCDbuf( fd, buf, sizeof(buf), &bufuse ) < 0 ) return;
    if( bufuse == 0 ) return;
    buf[bufuse-1] = '\0';
    lssproto_CD_send(fd, buf );
    CONNECT_setCDbufsiz(fd,0);
}


/*------------------------------------------------------------
 * CD毛霜月［
 * 娄醒
 * 忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
ANYTHREAD void CDcheck( void )
{
    int     i;
    unsigned int interval_us = getCDsendinterval_ms()*1000;
    for(i=0; i<ConnectLen; i++ ){
        struct timeval t;
        if( !CONNECT_getUse_debug(i,1082) ) continue;
        CONNECT_getLastCDsendtime( i, &t );
        if( time_diff_us( NowTime, t ) > interval_us ){
            CDsend( i);
            CONNECT_setLastCDsendtime( i , &NowTime );
        }
    }
}

ANYTHREAD void CDflush( int charaindex )
{
    int i;
    i = getfdFromCharaIndex( charaindex);
    if( i == -1 )return;
    CDsend(i);
}

void chardatasavecheck( void )
{
    int i;
    int interval = getCharSavesendinterval();
    static struct timeval chardatasavecheck_store;
    if( NowTime.tv_sec > (chardatasavecheck_store.tv_sec +10)){
        chardatasavecheck_store = NowTime;
        
        for( i = 0; i < ConnectLen; i ++) {
            CONNECT_LOCK(i);
            if( Connect[i].use == TRUE 
                && Connect[i].state == LOGIN 
                && NowTime.tv_sec - Connect[i].lastCharSaveTime.tv_sec 
                > interval ){
                Connect[i].lastCharSaveTime = NowTime;
                CONNECT_UNLOCK(i);
                CHAR_charSaveFromConnect( i, FALSE );
            } else {
                CONNECT_UNLOCK(i);
            }
        }
    } else {
        ;
    }
}
 

#ifdef _DEATH_FAMILY_STRUCT		// WON ADD 家族战存放胜负资料
void Init_FM_PK_STRUCT()
{
	saacproto_Init_FM_PK_STRUC_send( acfd  );
}
#endif

#ifdef _GM_BROADCAST					// WON ADD 客服公告系统
void Init_GM_BROADCAST( int loop, int time, int wait, char *msg )
{
	int i, count = 0;
	char *temp;

	BS.loop = loop;
	BS.time = time;
	BS.wait = wait;
	BS.next_msg = 0;

	// 清空msg
	memset( BS.msg, -1, sizeof(BS.msg) );

    // 载入公告讯息
    if( ( temp = strtok( msg, " " ) ) ){
		strcpy( BS.msg[count], temp );
        for( i=1; i<10; i++ ){
			char *temp1;
            if( ( temp1 = strtok( NULL, " " ) ) ){
                strcpy( BS.msg[++count], temp1 );
			}
		}
	}

	// 公告讯息数
	BS.max_msg_line = count;

	return;
}

void GM_BROADCAST()
{
    int i;
    static struct timeval broadcast;
	int next_msg = BS.next_msg;
	static int wait_time=0;

	if( BS.loop <= 0 ) return;

    if( NowTime.tv_sec > (broadcast.tv_sec + BS.time + wait_time) ){
        broadcast = NowTime;
		wait_time = 0;

		if( BS.msg[next_msg] != NULL ){
			for( i = 0; i < ConnectLen; i ++) {
				if( Connect[i].use == TRUE ){
					if( Connect[i].charaindex >= 0 )
						CHAR_talkToCli( Connect[i].charaindex, -1, BS.msg[next_msg], CHAR_COLORYELLOW);					
				}
			}
		}
	
		if( ++BS.next_msg > BS.max_msg_line ){			
			BS.next_msg = 0;
			wait_time = BS.wait;

			// BS.loop = 1000 时一直公告
			if( BS.loop < 1000 )	BS.loop--;
		}
    }
}
#endif

/*------------------------------------------------------------
 * fd 互 valid 卅手及井升丹井毛譬屯月
 * 娄醒
 *  fd          int         fd
 * 忒曰袄
 *  valid   TRUE(1)
 *  invalid FALSE(0)
 ------------------------------------------------------------*/
ANYTHREAD INLINE int CONNECT_checkfd( int fd )
{
    if( 0 > fd ||  fd >= ConnectLen ){
        return FALSE;
    }
    CONNECT_LOCK(fd);
    if( Connect[fd].use == FALSE ){
        CONNECT_UNLOCK(fd);
        return FALSE;
    } else {
        CONNECT_UNLOCK(fd);
        return TRUE;
    }
}


/*------------------------------------------------------------
 * cdkey 井日 fd 毛  月［
 * 娄醒
 *  cd      char*       cdkey
 * 忒曰袄
 *  白央奶伙犯奴旦弁伉皿正  方匀化｝ -1 及凛反巨仿□
 ------------------------------------------------------------*/
ANYTHREAD int getfdFromCdkey( char* cd )
{
    int i;
    for( i = 0 ;i < ConnectLen ; i ++ ){
        CONNECT_LOCK(i);
        if( Connect[i].use == TRUE &&
            strcmp( Connect[i].cdkey , cd ) == 0 ){
            CONNECT_UNLOCK(i);
            return i;
        }
        CONNECT_UNLOCK(i);
    }
    return -1;
}


/*------------------------------------------------------------
 * charaindex 井日 fd 毛  月［
 *   陆质  及民尼永弁毛聂仁仄凶［
 * 娄醒
 *  charaindex      int     平乓仿及奶件犯永弁旦
 * 忒曰袄
 *  白央奶伙犯奴旦弁伉皿正  方匀化｝ -1 及凛反巨仿□
 ------------------------------------------------------------*/
ANYTHREAD int getfdFromCharaIndex( int charaindex )
{
#if 1
	int ret;
	if( !CHAR_CHECKINDEX( charaindex)) return -1;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return -1;
	ret = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	if( ret < 0 || ret >= ConnectLen ) return -1;
	return ret;
#else
    int i;
    for( i = 0 ;i < ConnectLen ; i ++ ){
        CONNECT_LOCK(i);
        if( Connect[i].use == TRUE
            && Connect[i].charaindex == charaindex ){
            CONNECT_UNLOCK(i);
            return i;
        }
        CONNECT_UNLOCK(i);
    }
    return -1;
#endif
}
/*------------------------------------------------------------
 * charaindex 井日 cdkey 毛  月［
 * 娄醒
 *  charaindex  int     平乓仿及奶件犯永弁旦
 * 忒曰袄
 *  0卅日岳  ｝  卅日撩  
 ------------------------------------------------------------*/
ANYTHREAD int getcdkeyFromCharaIndex( int charaindex , char *out, int outlen )
{
    int i;

    for( i = 0 ;i < ConnectLen ; i ++ ){
        CONNECT_LOCK(i);
        if( Connect[i].use == TRUE
            && Connect[i].charaindex == charaindex ){
            snprintf( out, outlen, "%s" , Connect[i].cdkey );
            CONNECT_UNLOCK(i);
            return 0;
        }
        CONNECT_UNLOCK(i);
    }
    return -1;
}


/*------------------------------------------------------------
 *   元fdid 及蛊毛茧允
 * 娄醒
 *  fdid    int     fd及id
 * 忒曰袄
 *  -1 及凛反巨仿□
 ------------------------------------------------------------*/
ANYTHREAD int getfdFromFdid( int fdid )
{
    int i;

    for( i=0; i<ConnectLen ; i ++ ){
        CONNECT_LOCK(i);
        if( Connect[i].use == TRUE
            && Connect[i].fdid == fdid ){
            CONNECT_UNLOCK(i);
            return i;
        }
        CONNECT_UNLOCK(i);
    }
    return -1;
}

/*------------------------------------------------------------
 * fdid 井日平乓仿及index 毛手午户月［
 * 娄醒
 *  fdid    int     fd及id
 * 忒曰袄
 *  -1 及凛反夫弘奶件  及平乓仿反心勾井日卅井匀凶［0动晓卅日
 * 夫弘奶件  及平乓仿及平乓仿反中木勾尺及 index
 ------------------------------------------------------------*/
ANYTHREAD int getCharindexFromFdid( int fdid )
{
    int i;

    for( i=0; i<ConnectLen ; i ++ ){
        CONNECT_LOCK(i);
        if( Connect[i].use == TRUE
            && Connect[i].fdid == fdid &&
            Connect[i].charaindex >= 0 ){
            int a = Connect[i].charaindex;
            CONNECT_UNLOCK(i);
            return a;
        }
        CONNECT_UNLOCK(i);
    }

    return -1;
}
/*------------------------------------------------------------
 * 平乓仿index 井日 fdid 毛手午户月［
 * 娄醒
 *  charind  int     白央奶伙犯奴旦弁伉皿正
 * 井尹曰切 fdid    分匀凶日平乓仿ind互云井仄中
 ------------------------------------------------------------*/
ANYTHREAD int getFdidFromCharaIndex( int charind )
{
    int i;

    for( i=0; i<ConnectLen ; i ++ ){
        CONNECT_LOCK(i);
        if( Connect[i].use == TRUE
            && Connect[i].charaindex == charind ){
            int a = Connect[i].fdid;
            CONNECT_UNLOCK(i);
            return a;
        }
        CONNECT_UNLOCK(i);
    }

    return -1;
}


/*------------------------------------------------------------
 * fd卞喃曰癫化日木凶涛粮互弁仿奶失件玄分午｝BOOL毛忒允
 * 称中巨仿□民尼永弁反仄卅中［
 * 娄醒
 *  fd  int     白央奶伙犯奴旦弁伉皿正
 ------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isCLI( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = ( Connect[fd].ctype == CLI ? TRUE : FALSE  );
    CONNECT_UNLOCK(fd);
    return a;
}


/*------------------------------------------------------------
 * fd卞喃曰癫化日木凶涛粮互失市它件玄扔□田分午｝BOOL毛忒允
 * 称中巨仿□民尼永弁反仄卅中［
 * 娄醒
 *  fd  int     白央奶伙犯奴旦弁伉皿正
 ------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isAC( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = ( Connect[fd].ctype == AC ? TRUE : FALSE  );
    CONNECT_UNLOCK(fd);
    return a;
}

/*------------------------------------------------------------
 * fd卞喃曰癫化日木凶涛粮互夫弘奶件橇谪匹丐月井升丹井
 * 毛忒允
 * 娄醒
 *  fd  int     白央奶伙犯奴旦弁伉皿正
 ------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isUnderLogin( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = ( Connect[fd].state == LOGIN ? TRUE : FALSE  );
    CONNECT_UNLOCK(fd);
    return a;
}

/*------------------------------------------------------------
 * Login质    井升丹井譬屯月
 * 娄醒
 *  fd  int     白央奶伙犯奴旦弁伉皿正
 ------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isWhileLogin( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = ( Connect[fd].state == WHILELOGIN ? TRUE : FALSE  );
    CONNECT_UNLOCK(fd);
    return a;
}

/*------------------------------------------------------------
 * 夫弘奶件仄化中卅中橇谪井
 * 升丹井毛忒允
 * 娄醒
 *  fd  int     白央奶伙犯奴旦弁伉皿正
 ------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isNOTLOGIN( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = ( Connect[fd].state == NOTLOGIN ? TRUE : FALSE  );
    CONNECT_UNLOCK(fd);
    return a;
}

/*------------------------------------------------------------
 * 夫弘奶件仄化中月橇谪井
 * 升丹井毛忒允
 * 娄醒
 *  fd  int     白央奶伙犯奴旦弁伉皿正
 ------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isLOGIN( int fd )
{
    int a;
    CONNECT_LOCK(fd);
    a = ( Connect[fd].state == LOGIN ? TRUE : FALSE  );
    CONNECT_UNLOCK(fd);
    return a;
}



/*------------------------------------------------------------
 * 蝈够及涛粮毛濠匀化｝失市它件玄扔□田卞犯□正毛忡绣仄方丹午允月［
 * 娄醒｝忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
void closeAllConnectionandSaveData( void )
{
    int     i;
	int		num;

    /*  仇木动晓 accept 仄卅中方丹卞允月    */
    SERVSTATE_setCloseallsocketnum(0);

    /*  蝈  绰轮允月    */
    for( i = 0 ; i<ConnectLen ; i++ ){
        if( CONNECT_getUse_debug(i,1413) == TRUE ){
            BOOL    clilogin=FALSE;
            if( CONNECT_isAC( i ) )continue;
            if( CONNECT_isCLI( i ) && CONNECT_isLOGIN( i ) )clilogin = TRUE;
            CONNECT_endOne_debug(i);
            close(i);
            // Nuke +1 0901: Why close
            //print("closed in closeAllConnectionandSaveData");
                        
            if( clilogin ){
                CONNECT_setUse(i,TRUE);
                CONNECT_setState(i,WHILECLOSEALLSOCKETSSAVE );
                SERVSTATE_incrementCloseallsocketnum();
            }
        }
    }
    num = SERVSTATE_getCloseallsocketnum();
    if( num == 0 ) {
	    SERVSTATE_SetAcceptMore( -1 );
	}else {
	    SERVSTATE_SetAcceptMore( 0 );
    }
    print( "\n发送人物数据数目:%d\n", num );
}
//andy_add 2003/02/12
void CONNECT_SysEvent_Loop( void)
{
	static time_t checkT=0;
	static int chikulatime = 0;
#ifdef _PETSKILL_BECOMEPIG
	static int chikulatime2 = 0;
	static time_t checkT2=0;
#endif
	int NowTimes = time(NULL);

	if( checkT != NowTimes && (checkT+10) <= NowTimes )	{
		int i;
		checkT = time(NULL);
		chikulatime++;//每10秒
		if( chikulatime > 10000 ) chikulatime = 0;
#ifdef _NPCSERVER_NEW
		if( npcfd != -1)
			NPCS_SendProbe( npcfd);
#endif
		for (i=0;i<ConnectLen; i++)	{
			if ((Connect[i].use) && (i!=acfd)
#ifdef _M_SERVER
				&& (i!=mfd)
#endif
#ifdef _NPCSERVER_NEW
				&& (i!=npcfd)
#endif
			) {
				
				if( chikulatime%6 == 0 ){ // 每60秒
					// shan 2001/12/27 Begin
					if( CHAR_getWorkInt( Connect[i].charaindex, CHAR_WORKBATTLEMODE)
							!=BATTLE_CHARMODE_NONE){
						struct timeval recvtime;
						CONNECT_GetBattleRecvTime( i, &recvtime);								
						if( time_diff( NowTime, recvtime) > 360 ){									
							CONNECT_endOne_debug(i);
							close(i);
						}								
					}
					// End
				}//%30

				if( chikulatime%30 == 0 ){ // 每300秒

#ifdef _ITEM_ADDEXP	//vincent 经验提升
					if( CHAR_getWorkInt(Connect[i].charaindex,CHAR_WORKITEM_ADDEXP)>0 && 
					   CHAR_getInt( Connect[i].charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){
#if 1
						int charaindex, exptime;
						charaindex = Connect[i].charaindex;
						exptime = CHAR_getWorkInt( charaindex, CHAR_WORKITEM_ADDEXPTIME) - 300;
						if( exptime <= 0 ) {
							CHAR_setWorkInt( charaindex, CHAR_WORKITEM_ADDEXP, 0);
							CHAR_setWorkInt( charaindex, CHAR_WORKITEM_ADDEXPTIME, 0);
							CHAR_talkToCli( charaindex,-1,"提升学习经验的能力消失了!",CHAR_COLORYELLOW);
						}
						else {
							CHAR_setWorkInt( charaindex, CHAR_WORKITEM_ADDEXPTIME, exptime);
							//print("\n 检查ADDEXPTIME %d ", exptime);
							
							if( (exptime % (60*60)) < 300 && exptime >= (60*60) )
							{
								char msg[1024];
								sprintf( msg, "提升学习经验的能力剩大约 %d 小时。", (int)(exptime/(60*60)) );
								//sprintf( msg, "提升学习经验的能力剩大约 %d 分。", (int)(exptime/(60)) );
								CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
							}
						}
#else
						if(Connect[i].EDTime < CHAR_getWorkInt(Connect[i].charaindex,CHAR_WORKITEM_ADDEXPTIME)){//功能作用有效期限
							Connect[i].EDTime=Connect[i].EDTime+300;
						}else{
							Connect[i].EDTime = 0;
							CHAR_setWorkInt( Connect[i].charaindex, CHAR_WORKITEM_ADDEXP, 0);
							CHAR_setWorkInt( Connect[i].charaindex, CHAR_WORKITEM_ADDEXPTIME, 0);
							CHAR_talkToCli(Connect[i].charaindex,-1,"提升学习经验的能力消失了!",CHAR_COLORYELLOW);
						}
#endif
					}
#endif
#ifdef _ITEM_METAMO
					if( CHAR_getWorkInt( Connect[i].charaindex, CHAR_WORKITEMMETAMO) < NowTime.tv_sec 
							&& CHAR_getWorkInt( Connect[i].charaindex, CHAR_WORKITEMMETAMO) != 0 ){
							CHAR_setWorkInt( Connect[i].charaindex, CHAR_WORKITEMMETAMO, 0);
							CHAR_setWorkInt( Connect[i].charaindex, CHAR_WORKNPCMETAMO, 0 ); //与npc对话後的变身也要变回来
							CHAR_complianceParameter( Connect[i].charaindex );
							CHAR_sendCToArroundCharacter( CHAR_getWorkInt( Connect[i].charaindex , CHAR_WORKOBJINDEX ));
							CHAR_send_P_StatusString( Connect[i].charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
							CHAR_talkToCli( Connect[i].charaindex,-1,"变身失效了。",CHAR_COLORWHITE);
					}
#endif
#ifdef _ITEM_TIME_LIMIT
					ITEM_TimeLimit(Connect[i].charaindex);	// (可开放) shan time limit of item. code:shan
#endif
				}//%30

#ifndef _USER_CHARLOOPS
				//here 原地遇敌
				if( Connect[i].stayencount ) {
					if( Connect[i].BDTime < time( NULL) )	{
						if( CHAR_getWorkInt(Connect[i].charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE){
							lssproto_EN_recv( i, CHAR_getInt(Connect[i].charaindex,CHAR_X),
								CHAR_getInt(Connect[i].charaindex,CHAR_Y));
							Connect[i].BDTime = time( NULL);
						}
					}
				}
#endif
#ifdef _CHIKULA_STONE
				if( chikulatime%3 == 0 && getChiStone( i) > 0)	{	//自动补血
					CHAR_AutoChikulaStone( Connect[i].charaindex, getChiStone( i));
				}
#endif

				if( chikulatime%6 == 0 )	{	//水世界状态
#ifdef _STATUS_WATERWORD
					CHAR_CheckWaterStatus( Connect[i].charaindex);
#endif
					// Nuke 0626: No enemy
					if (Connect[i].noenemy>0) {
						Connect[i].noenemy--;
						if (Connect[i].noenemy==0) {
							CHAR_talkToCli(CONNECT_getCharaindex(i),-1,"守护消失了。",CHAR_COLORWHITE);
						}
					}
				}
				//每10秒
#ifdef _TYPE_TOXICATION	//中毒
				if( Connect[i].toxication > 0 ){
					CHAR_ComToxicationHp( Connect[i].charaindex);
				}
#endif
				// Nuke 0624 Avoid Useless Connection
				if (Connect[i].state == NOTLOGIN) {
					Connect[i].cotime++;
					if (Connect[i].cotime>30) {
						print( "LATE" );
						CONNECT_endOne_debug(i);
						close(i);
					}
				}else{
					Connect[i].cotime=0;
				}

				if ((Connect[i].nu <= 22)) {
					int r;
					if (Connect[i].nu<=0) {
						Connect[i].nu_decrease++;
						if( Connect[i].nu_decrease >= 30 )
							Connect[i].nu_decrease = 30;
						if (Connect[i].nu_decrease>22) logSpeed(i);
						}else	{
							Connect[i].nu_decrease-=1;
							if( Connect[i].nu_decrease < 0 )
								Connect[i].nu_decrease = 0;
						}
		        		r=22-Connect[i].nu_decrease;
        				r=(r>=15)?r:15;
		        		lssproto_NU_send(i,r);
		        		Connect[i].nu+=r;
				}
		        // Nuke 1213: Flow control 2
		        Connect[i].packetin--; // Remove a counter
		        if (Connect[i].packetin<=0) { // Time out, drop this line
		        	print("Drop line: sd=%d\n",i);
					CONNECT_endOne_debug(i);
					close(i);
		        }
#ifdef _PETSKILL_BECOMEPIG
				/*if( CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG) > -1 ){ //处於乌力化状态
					if( ( CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG ) - 10 ) <= 0 ){ //乌力时间结束了
						CHAR_setInt( Connect[i].charaindex, CHAR_BECOMEPIG, 0 ); 
						if( CHAR_getWorkInt( Connect[i].charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){ //不是在战斗状态下
						    CHAR_setInt( Connect[i].charaindex, CHAR_BECOMEPIG, -1 );//结束乌力状态
						    CHAR_complianceParameter( Connect[i].charaindex );
						    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( Connect[i].charaindex , CHAR_WORKOBJINDEX ));
					        CHAR_send_P_StatusString( Connect[i].charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
						    CHAR_talkToCli( Connect[i].charaindex,-1,"乌力化失效了。",CHAR_COLORWHITE);
						}
					}
					else{
						char temp[256];
                        CHAR_setInt( Connect[i].charaindex, CHAR_BECOMEPIG, CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG ) - 10 );
    					if( CHAR_getWorkInt( Connect[i].charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){ //不是在战斗状态下
                            if( chikulatime%6 == 0 ){//60秒
						        sprintf(temp, "乌力时间:%d秒", CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG ));
						        CHAR_talkToCli( Connect[i].charaindex,-1,temp,CHAR_COLORWHITE);
						    }
						}
					}
				}*/
				if( CHAR_getWorkInt( Connect[i].charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){ //不是在战斗状态下
				    if( CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG) > -1 ){ //处於乌力化状态
					    char temp[256];
						sprintf(temp, "乌力时间:%d秒", CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG ));
						CHAR_talkToCli( Connect[i].charaindex,-1,temp,CHAR_COLORWHITE);
					}
				}
#endif
			//10秒
#ifdef _MAP_TIME
				if(CHAR_getWorkInt(Connect[i].charaindex,CHAR_WORK_MAP_TIME) > 0
					&& CHAR_getWorkInt(Connect[i].charaindex,CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
					CHAR_setWorkInt(Connect[i].charaindex,CHAR_WORK_MAP_TIME,CHAR_getWorkInt(Connect[i].charaindex,CHAR_WORK_MAP_TIME) - 10);
					if(CHAR_getWorkInt(Connect[i].charaindex,CHAR_WORK_MAP_TIME) <= 0){
						// 时间到了,传回入口
						CHAR_talkToCli(Connect[i].charaindex,-1,"你因为受不了高热而热死！传回裂缝入口。",CHAR_COLORRED);
						CHAR_warpToSpecificPoint(Connect[i].charaindex,30008,39,38);
						CHAR_setInt(Connect[i].charaindex,CHAR_HP,1);
						CHAR_AddCharm(Connect[i].charaindex,-3);
						CHAR_send_P_StatusString(Connect[i].charaindex,CHAR_P_STRING_HP);
						CHAR_send_P_StatusString(Connect[i].charaindex,CHAR_P_STRING_CHARM);
					}
					else{
						char szMsg[64];
						sprintf(szMsg,"在这高热的环境下你只能再待 %d 秒。",CHAR_getWorkInt(Connect[i].charaindex,CHAR_WORK_MAP_TIME));
						CHAR_talkToCli(Connect[i].charaindex,-1,szMsg,CHAR_COLORRED);
					}
				}
#endif
			}
		}
	}

#ifdef _PETSKILL_BECOMEPIG
	if( checkT2 != NowTimes && (checkT2) <= NowTimes )	{
        int i;
		checkT2 = time(NULL);
		++chikulatime2;//每1秒
		if( chikulatime2 > 1000 ) chikulatime2 = 0;
		for (i=0;i<ConnectLen; i++)	{
			if ((Connect[i].use) && (i!=acfd)
#ifdef _M_SERVER
				&& (i!=mfd)
#endif
#ifdef _NPCSERVER_NEW
				&& (i!=npcfd)
#endif
			){
				//计算用
				if( CHAR_CHECKINDEX( Connect[i].charaindex ) )
                if( CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG) > -1 ){ //处於乌力化状态
					if( ( CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG ) - 1 ) <= 0 ){ //乌力时间结束了
						CHAR_setInt( Connect[i].charaindex, CHAR_BECOMEPIG, 0 ); 
						if( CHAR_getWorkInt( Connect[i].charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){ //不是在战斗状态下
						    CHAR_setInt( Connect[i].charaindex, CHAR_BECOMEPIG, -1 );//结束乌力状态
							CHAR_complianceParameter( Connect[i].charaindex );
						    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( Connect[i].charaindex , CHAR_WORKOBJINDEX ));
					        CHAR_send_P_StatusString( Connect[i].charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
						    CHAR_talkToCli( Connect[i].charaindex,-1,"乌力化失效了。",CHAR_COLORWHITE);
						}
					}
					else{
                        CHAR_setInt( Connect[i].charaindex, CHAR_BECOMEPIG, CHAR_getInt( Connect[i].charaindex, CHAR_BECOMEPIG ) - 1 );
					}
				}
			}
		}
	}
#endif

}

// Nuke 0126: Resource protection
int isThereThisIP(unsigned long ip)
{
	int i;
	unsigned long ipa;
        for(i = 0; i< ConnectLen ; i++ )
            if( !Connect[i].use ) continue;
			if( Connect[i].state == NOTLOGIN || Connect[i].state == WHILEDOWNLOADCHARLIST ) {
            	memcpy(&ipa,&Connect[i].sin.sin_addr,4);
            	if (ipa==ip) return 1;
            }
        return 0;
}

#if USE_MTIO == 0

int player_online = 0;

SINGLETHREAD BOOL netloop_faster( void )
{

    int ret , loop_num;
    struct timeval tmv;    /*timeval*/
    static int fdremember = 0;

    static unsigned int total_item_use=0;
	static int petcnt=0;
//    static unsigned int nu_time=0;
    struct timeval st, et;
    unsigned int casend_interval_us , cdsend_interval_us;
	int acceptmore = SERVSTATE_getAcceptmore();
    fd_set fdset;
    unsigned int looptime_us;
    int allowerrornum = getAllowerrornum();
    int acwritesize = getAcwriteSize();
#ifdef _AC_PIORITY
	static int flag_ac=1;
	static int fdremembercopy=0;
	static int totalloop=0;
	static int totalfd=0;
	static int totalacfd=0;
	static int counter=0;
#endif

#if 0
#ifdef _M_SERVER
	static unsigned int m_time=0;
	unsigned int mm;
	static int connectnum = 0;

	if (mfd < 0){
		mm=time(NULL);
		if( (mm - m_time > 120) && connectnum < 5 ){
			m_time = mm;
			mfd = connectmServer(getmservername(),getmserverport());
			connectnum ++;
			if (mfd != -1){
				initConnectOne(mfd,NULL,0);
				print("connect to mserver successed\n");
				connectnum = 0; //重设连线次数
			}else{
				print("connect to mserver failed\n");
			}
		}
	}else	{
		connectnum = 0;
	}
#endif
#endif

/*
	{
		int errorcode;
		int errorcodelen;
		int qs;

		errorcodelen = sizeof(errorcode);
		qs = getsockopt( acfd, SOL_SOCKET, SO_RCVBUF , &errorcode, &errorcodelen);
		//andy_log
		print("\n\n GETSOCKOPT SO_RCVBUF: [ %d, %d, %d] \n", qs, errorcode, errorcodelen);
	}
*/
#ifdef _NPCSERVER_NEW
	{
		static unsigned int NStime=0;
		static int connectNpcnum = 0;
		if( (NStime + 120) < time(NULL) && connectNpcnum < 10 ){
			NStime = time(NULL);
			if (npcfd < 0){
				npcfd = connectNpcServer( getnpcserveraddr(), getnpcserverport());
				connectNpcnum ++;
				if (npcfd != -1){
					initConnectOne( npcfd, NULL, 0);
					print("connect to NpcServer successed\n");
					NPCS_NpcSLogin_send( npcfd);
					connectNpcnum = 0; //重设连线次数
				}
			}else	{
				connectNpcnum = 0;
			}
		}
	}
#endif
    looptime_us = getOnelooptime_ms()*1000 ;
    casend_interval_us = getCAsendinterval_ms()*1000;
    cdsend_interval_us = getCDsendinterval_ms()*1000;

    FD_ZERO( &fdset );
    FD_SET( bindedfd ,& fdset);
    tmv.tv_sec = tmv.tv_usec = 0;
    ret = select( bindedfd + 1 , &fdset,(fd_set*)NULL,(fd_set*)NULL,&tmv );
    if( ret < 0 && ( errno != EINTR )){
        print( "accept select() error:%s\n", strerror(errno));
    }
    if( ret > 0 && FD_ISSET(bindedfd , &fdset ) ){
        struct sockaddr_in sin;
        int addrlen=sizeof( struct sockaddr_in );
        int sockfd;
        sockfd = accept( bindedfd ,(struct sockaddr*) &sin  , &addrlen );
        if( sockfd == -1 && errno == EINTR ){
            ;
        }else if( sockfd != -1 ){
			unsigned long sinip;
			int cono=1, from_acsv = 0;
			if (cono_check&CONO_CHECK_LOGIN){
				if( StateTable[WHILELOGIN]+StateTable[WHILELOGOUTSAVE] > QUEUE_LENGTH1 ||
					StateTable[WHILEDOWNLOADCHARLIST] > QUEUE_LENGTH2 ){
					print("err State[%d,%d,%d]!!\n", StateTable[WHILELOGIN],
						StateTable[WHILELOGOUTSAVE],
						StateTable[WHILEDOWNLOADCHARLIST] );

					CONNECT_checkStatecount( WHILEDOWNLOADCHARLIST);
					cono=0;
				}
			}
			if (cono_check&CONO_CHECK_ITEM)
				if (total_item_use >= MAX_item_use){
					print("err item_use full!!");
					cono=0;
				}
			if (cono_check&CONO_CHECK_PET)
				if( petcnt >= CHAR_getPetMaxNum() ){
					print("err pet_use full!!");
					cono=0;
				}

			{
				char temp[80];
				sprintf(temp,"%d.%d.%d.%d",
				((unsigned char *)&sin.sin_addr)[0],
				((unsigned char *)&sin.sin_addr)[1],
				((unsigned char *)&sin.sin_addr)[2],
				((unsigned char *)&sin.sin_addr)[3]);
				if (strcmp(getAccountservername(),temp)==0) {
					cono=1; from_acsv=1;
					print("From acsv. ");
				}
#ifdef _M_SERVER
				if (strcmp(getmservername(),temp) == 0){
					cono=1;
					print("From mserver\n");
				}
#endif
			}
            //print("CO");

			{
				float fs=0.0;
				if( (fs = ((float)Connect[acfd].rbuse/AC_RBSIZE) ) > 0.6 ){
					print( "andy AC rbuse: %3.2f [%4d]\n", fs, Connect[acfd].rbuse );
					if( fs > 0.78 ) cono = 0;
				}
			}

			memcpy( &sinip, &sin.sin_addr, 4);
            // Nuke *1 0126: Resource protection
            if((cono == 0) || (acceptmore <= 0) || isThereThisIP( sinip) ){
				// Nuke +2 Errormessage
				char mess[1024]="E伺服器忙线中，请稍候再试。";
				if (!from_acsv)
					write(sockfd,mess,strlen(mess)+1);
				print( "accept but drop[cono:%d,acceptmore:%d]\n", cono, acceptmore);
				close( sockfd );
            }else if( sockfd < ConnectLen ){
                char mess[1024] = "A";// Nuke +2 Errormessage
				if( bNewServer )
#ifdef _SA_VERSION_70         // WON ADD 石器时代7.0 的版本控制
					mess[0]='F';	  // 7.0
#endif
				else
					mess[0]='$';

				//char mess[1024]="E伺服器忙线中，请稍候再试。";
				if (!from_acsv)
					send(sockfd,mess,strlen(mess)+1,0);
				initConnectOne(sockfd,&sin,addrlen);
		        if( getNodelay() ){
		            int flag = 1;
		            int result = setsockopt( sockfd, IPPROTO_TCP, TCP_NODELAY,
		                                     (char*)&flag, sizeof(int));
		            if( result < 0 ){
		                print("setsockopt TCP_NODELAY failed:%s\n",
		                      strerror( errno ));
		            } else {
		                print( "NO" );
                    }
		        }
            }else {
                    // Nuke +2 Errormessage	            
                     char mess[1024]="E伺服器人数已满，请稍候再试。";
                     if (!from_acsv) write(sockfd,mess,strlen(mess)+1);
                        close( sockfd );
                        // Nuke +1 0901: Why close
            }
        }
    }
    loop_num=0;
    gettimeofday( &st, NULL );
    while(1)
	{
        char buf[65535*2];
		int	j;
        //ttom+1 for the debug
        static int i_tto=0;
        static int i_timeNu=0;
		
        gettimeofday( &et, NULL );
        if( time_diff_us( et,st) >= looptime_us ) //执行每超过0.1秒资要做的的工作
		{
#define LOOP_NUM_ADD_CREDIT 5
#define CREDIT_SPOOL 3
            switch( acceptmore) 
			{
			case -1:
                print( "#");
#ifdef _KILL_12_STOP_GMSV      // WON ADD 下sigusr2後关闭GMSV
				//andy_reEdit 2003/04/28
				//system("./stop.sh"); 
#endif
                break;
			case 0:
                print( "$");
                if(!b_first_shutdown)
				{
					b_first_shutdown=TRUE;
					i_shutdown_time=SERVSTATE_getLimittime();
					print("\n 关闭服务器时间=%d",i_shutdown_time);
                }
                break;
			default:
                {
					static int i_counter=0;
					// Syu ADD 定时读取Announce
					static int j_counter=0;
					// Syu ADD 每小时重新更新英雄战厂排行榜资料
					static int h_counter=0;
					// 不会归零的计时器
					static long total_count=0;
					
					int i;
					int item_max;
					
					if(i_counter>10)
					{//10秒
						player_online = 0;//looptime_us
#ifdef _AC_PIORITY
						//print("\n<TL:%0.2f,FD=%d,LOOP=%d,ACFD=%d>",
						//	(totalfd*1.0)/(totalloop*1.0),
						//	totalfd,totalloop,totalacfd);
						totalloop=0; totalfd=0; totalacfd=0;
#endif
						i_counter=0;
						item_max = ITEM_getITEM_itemnum();
						total_item_use = ITEM_getITEM_UseItemnum();
						for (i=0;i<ConnectLen; i++)	
						{
							if ((Connect[i].use) && (i!=acfd)
#ifdef _M_SERVER
								&& (i!=mfd)
#endif
#ifdef _NPCSERVER_NEW
								&& (i!=npcfd)
#endif
								) 
							{
								if( CHAR_CHECKINDEX( Connect[ i].charaindex ) )
									player_online++;
							}
						}
						{
							int max, min, MaxItemNums;
							char buff1[512];
							char szBuff1[256];
#ifdef _ASSESS_SYSEFFICACY
							{
								float TVsec;
								ASSESS_getSysEfficacy( &TVsec);
								sprintf( szBuff1, "Sys:[%2.4f].", TVsec);
							}
#endif
							//MaxItemNums = ITEM_getITEM_itemnum();
							MaxItemNums = getItemnum();
							memset( buff1, 0, sizeof( buff1));
							CHAR_getCharOnArrayPercentage( 1, &max, &min, &petcnt);
							sprintf( buff1,"\n玩家=%d 宠物=%3.1f%% 物品=%3.1f%% 邮件:%d 战斗:%d %s",
							player_online, (float)((float)(petcnt*100)/max),
							(float)((float)(total_item_use*100)/MaxItemNums),
							PETMAIL_getPetMailTotalnums(), Battle_getTotalBattleNum(), szBuff1 );
							//sprintf( buff1,"\nPlayer=%d PM:%d B:%d %s",
							//player_online, 
							//PETMAIL_getPetMailTotalnums(), Battle_getTotalBattleNum(), szBuff1 );
							
							buff1[ strlen( buff1)+1]	= 0;
							print("%s.", buff1);
#ifdef _ASSESS_SYSEFFICACY_SUB
							{
								float TVsec;
								ASSESS_getSysEfficacy_sub( &TVsec, 1);
								sprintf( szBuff1, "Net:[%2.4f] ", TVsec);
								strcpy( buff1, szBuff1);
								
								//ASSESS_getSysEfficacy_sub( &TVsec, 2);
								//sprintf( szBuff1, "NG:[%2.4f] ", TVsec);
								//strcat( buff1, szBuff1);
								
								ASSESS_getSysEfficacy_sub( &TVsec, 3);
								sprintf( szBuff1, "BT:[%2.4f] \n", TVsec);
								strcat( buff1, szBuff1);
								
								//ASSESS_getSysEfficacy_sub( &TVsec, 4);
								//sprintf( szBuff1, "CH:[%2.4f] \n", TVsec);
								//strcat( buff1, szBuff1);
								
								//ASSESS_getSysEfficacy_sub( &TVsec, 5);
								//sprintf( szBuff1, "PM:[%2.4f] ", TVsec);
								//strcat( buff1, szBuff1);
								
								//ASSESS_getSysEfficacy_sub( &TVsec, 6);
								//sprintf( szBuff1, "FM:[%2.4f] ", TVsec);
								//strcat( buff1, szBuff1);
								
								//ASSESS_getSysEfficacy_sub( &TVsec, 7);
								//sprintf( szBuff1, "SV:[%2.4f] ", TVsec);
								//strcat( buff1, szBuff1);
								
								//ASSESS_getSysEfficacy_sub( &TVsec, 8);
								//sprintf( szBuff1, "GB:[%2.4f] ", TVsec);
								//strcat( buff1, szBuff1);
								
								buff1[ strlen( buff1)+1]	= 0;
								print("%s.", buff1);
							}
#endif
#ifdef _CHECK_BATTLE_IO
							print("\nall_write=%d :%d  battle_write=%d :%d \n",
									other_write+battle_write, other_write_cnt+battle_write_cnt,
									battle_write, battle_write_cnt );
							other_write =0;	battle_write =0;
							other_write_cnt =0; battle_write_cnt =0;
#endif

						}
#ifdef _M_SERVER
						if (mfd != -1)
							mproto_Type_Send( mfd, player_online, total_item_use);
#endif

#ifdef _TIME_TICKET
						check_TimeTicket();
#endif

					}

					
					// Syu ADD 定时读取Announce
					if ( j_counter > 60*10 )
					{//6000 约 600秒=10分钟
						j_counter=0;
						print("\n定时读取公告");
						LoadAnnounce();
#ifdef _CHECK_BATTLETIME
						check_battle_com_show();
#endif
					}
					/*
					#ifdef _ALLDOMAN					// Syu ADD 排行榜NPC
					// Syu ADD 每小时重新更新英雄战厂排行榜资料
					if ( h_counter > 60*60 ){//36000 约 3600秒=60分钟
					h_counter=0;
					print("\nSyu log LoadHerolist");
					saacproto_UpdataStele_send ( acfd , "FirstLoad", "LoadHerolist" , "华义" , 0 , 0 , 0 , 999 ) ; 
					}
					#endif
					*/
					if( i_timeNu != time(NULL) ) // 每整秒执行一次
					{
						i_timeNu = time(NULL);

#ifdef _DEL_DROP_GOLD
						//if( total_count % 60 == 0 ) { //每分钟执行
						//	GOLD_DeleteTimeCheckLoop();
						//}
#endif
						//if( total_count % 60*10 == 0 ) { //每10分钟执行
						//}
						
						//if( total_count % 60*60 == 0 ) { //每60分钟执行
						//}

						
						i_counter++;
						// Syu ADD 定时读取Announce
						j_counter++;
						// Syu ADD 每小时重新更新英雄战厂排行榜资料
						h_counter++;
						
						total_count++;
					}
				}
				
				if((i_tto%60)==0)
				{
					i_tto=0;
					print(".");
				}
#ifdef _TEST_PETCREATE
				if(i_tto % 30 ==0 )	
				{
					TEST_CreatPet();
				}
#endif
				i_tto++;
				
				//andy add 2003/0212------------------------------------------
				CONNECT_SysEvent_Loop( );
				//------------------------------------------------------------
			} // switch()
#ifdef _AC_PIORITY
			if (flag_ac==2)	fdremember=fdremembercopy;
			flag_ac=1;
			totalloop++;
#endif
            break; // Break while
        } // if(>0.1sec)

        loop_num++;

#ifdef _AC_PIORITY
		switch (flag_ac)
		{
		case 1:
			fdremembercopy=fdremember;
			fdremember=acfd;
			flag_ac=2;
			break;
		case 2:
			counter++;
			if (counter>=3)
			{
				counter=0;
				fdremember=fdremembercopy+1;
				flag_ac=0;
			}
			break;
		default:
			fdremember++;
			break;
		}
#else
        fdremember++;
#endif		
        if( fdremember == ConnectLen ) fdremember =0;
        if( Connect[fdremember].use == FALSE )continue;
        if( Connect[fdremember].state == WHILECLOSEALLSOCKETSSAVE ) continue;
#ifdef _AC_PIORITY
		totalfd++;
		if (fdremember==acfd) totalacfd++;
#endif
        /* read select */
        FD_ZERO( &fdset );
        FD_SET( fdremember , &fdset );
        tmv.tv_sec = tmv.tv_usec = 0;
        ret = select( fdremember + 1, &fdset, (fd_set*)NULL,(fd_set*)NULL,&tmv);
        if( ret > 0 ){
            if( FD_ISSET( fdremember, &fdset ) ){
                errno = 0;
				memset( buf, 0, sizeof( buf));
                ret = read( fdremember, buf, sizeof(buf));
				if( ret > 0 && sizeof( buf ) <= ret ){
					print( "Read(%s) buf len:%d - %d !!\n", (fdremember==acfd)?"AC":"OTHERFD", ret, sizeof(buf) );
				}
				
                if( ret == -1 && errno == EINTR ){
                    ;
                } else if( ret == -1 || ret == 0 ){
                    if( fdremember == acfd ){
                        print( "read return:%d %s\n",ret,strerror(errno));
                        print( "netloop_faster: acsv is down! aborting...\n" );
                        shutdownProgram();
                        exit(1);
#ifdef _M_SERVER
					}else if ( fdremember == mfd ) {
						CONNECT_endOne_debug(fdremember);
						close(fdremember);
						mfd = -1;
#endif
#ifdef _NPCSERVER_NEW
					}else if ( fdremember == npcfd ) {
						CONNECT_endOne_debug(fdremember);
						close(fdremember);
						npcfd = -1;
						
#endif
                    } else {
                        if( ret == -1 )
													print( "读取返回: %d %s \n", errno, strerror( errno));
						
                        print( "\n玩家" );
                        CONNECT_endOne_debug(fdremember );
                        close( fdremember );
                        continue;
                    }
                } else {
					
                    if( appendRB( fdremember, buf, ret ) == -2 && getErrUserDownFlg() == 1){
						CONNECT_endOne_debug(fdremember );
						close( fdremember );
						continue;
                    }else{
						Connect[fdremember].lastreadtime = NowTime;
						Connect[fdremember].lastreadtime.tv_sec -=
							DEBUG_ADJUSTTIME;
						Connect[fdremember].packetin = 30;
					}             
                }
            } else {
            }
        } else if( ret < 0 && errno != EINTR){
            print( "读取选择错误:%s\n", strerror( errno ));
        }
		
		for( j = 0; j < 3; j ++ ) {
			char rbmess[65535*2];
			memset( rbmess, 0, sizeof( rbmess));
			if( GetOneLine_fix( fdremember, rbmess, sizeof(rbmess)) == FALSE )continue;
			if( !(  (rbmess[0]== '\r' && rbmess[1] == '\n') || rbmess[0] == '\n') ){
                if( fdremember == acfd ){
                    if( saacproto_ClientDispatchMessage( fdremember, rbmess)< 0){
                    }
#ifdef _M_SERVER
				}else if ( fdremember == mfd ) {
					if ( mproto_ClientDispatchMessage( fdremember, rbmess) < 0) {}
#endif
#ifdef _NPCSERVER_NEW
				}else if ( fdremember == npcfd ) {
					if (NSproto_DispatchMessage(fdremember,rbmess) < 0) {}
					
#endif
                }else{
                    if( lssproto_ServerDispatchMessage( fdremember, rbmess )< 0){
						//print(" DispatchMsg_Error!!! ");
                        Connect[fdremember].errornum ++;
                    }
                }
            }
			if( Connect[fdremember].errornum > allowerrornum ){
				print( "Too many Errors:%s close\n",
	                   inet_ntoa(Connect[fdremember].sin.sin_addr ));
				CONNECT_endOne_debug(fdremember);
				close(fdremember);
				break;
			}
		}
        if( Connect[fdremember].errornum > allowerrornum ){
			continue;
		}
        if( Connect[fdremember].CAbufsiz > 0 &&
            time_diff_us( et, Connect[fdremember].lastCAsendtime)
            > casend_interval_us ){
            CAsend( fdremember);
            Connect[fdremember].lastCAsendtime = et;
		}
        if( Connect[fdremember].CDbufsiz > 0 &&
			time_diff_us( et, Connect[fdremember].lastCDsendtime)
            > cdsend_interval_us ){
            CDsend( fdremember);
            Connect[fdremember].lastCDsendtime = et;
		}
        if( Connect[fdremember].wbuse >0 ){
            FD_ZERO( &fdset );
            FD_SET( fdremember, &fdset );
            tmv.tv_sec = tmv.tv_usec = 0;
            ret = select( fdremember + 1,
				(fd_set*)NULL, &fdset, (fd_set*)NULL,&tmv);
			
            if( ret > 0 ){
                if( FD_ISSET( fdremember , &fdset ) ){
					//Nuke start 0907: Protect gmsv
					if (fdremember == acfd) {
						ret = write( fdremember , Connect[fdremember].wb ,
							( Connect[fdremember].wbuse < acwritesize) ?
							Connect[fdremember].wbuse : acwritesize );
                    } else {
#ifdef _BACK_VERSION
						ret = write( fdremember , Connect[fdremember].wb ,
							( Connect[fdremember].wbuse < acwritesize) ?
							Connect[fdremember].wbuse : acwritesize );
#else
						ret = write( fdremember , Connect[fdremember].wb ,
							( Connect[fdremember].wbuse < 16384) ?
							Connect[fdremember].wbuse : 16384 );
#endif           
					}
					// Nuke end
					
                    if( ret == -1 && errno == EINTR ){
                        ;
                    } else if( ret == -1 ){
                        CONNECT_endOne_debug(fdremember );
                        close( fdremember );
#ifdef _M_SERVER
						if (fdremember == mfd ) mfd =-1;
#endif
#ifdef _NPCSERVER_NEW
						if (fdremember == npcfd ) npcfd =-1;
#endif
                        continue;
                    } else if( ret > 0 ){
                        shiftWB( fdremember, ret );
                    }
                }
            } else if( ret < 0 && errno != EINTR ){
                print( "select() error:%s\n", strerror(errno));
            }
        }
        /* 正奶丞失它玄及质   */
        if( fdremember == acfd 
#ifdef _M_SERVER
			|| fdremember == mfd
#endif
#ifdef _NPCSERVER_NEW
			|| fdremember == npcfd
#endif
			)
			continue;
		
        //ttom start : because of the second have this        
        if( Connect[fdremember].close_request ){
            print( "强迫关闭:%s \n",
				inet_ntoa(Connect[fdremember].sin.sin_addr ));
            CONNECT_endOne_debug(fdremember);
            close(fdremember);
            continue;
        }                                                                                               
        //ttom end
    }
    return TRUE;
}
#endif /* if USE_MTIO == 0 */

ANYTHREAD void outputNetProcLog( int fd, int mode)
{
    int i;
    int c_use=0,c_notdetect=0 ;
    int c_ac=0, c_cli=0 , c_adm =0, c_max=0;
    int login=0;
    char buffer[4096];
    char buffer2[4096];

    strcpysafe(buffer,sizeof(buffer),  "Server Status\n");
    c_max = ConnectLen;


    for(i=0;i<c_max;i++){
        CONNECT_LOCK(i);
        if( Connect[i].use ){
            c_use ++;
            switch( Connect[i].ctype ){
            case NOTDETECTED: c_notdetect++; break;
            case AC: c_ac ++; break;
            case CLI: c_cli ++; break;
            case ADM: c_adm ++; break;
            }
            if( Connect[i].charaindex >= 0 ){
                login ++;
            }
        }
        CONNECT_UNLOCK(i);
    }

    snprintf( buffer2 , sizeof( buffer2 ) ,
              "connect_use=%d\n"
              "connect_notdetect=%d\n"
              "connect_ac=%d\n"
              "connect_cli=%d\n"
              "connect_adm=%d\n"
              "connect_max=%d\n"
              "login=%d\n",
             c_use , c_notdetect,c_ac,c_cli,c_adm,c_max,login );
    strcatsafe( buffer , sizeof(buffer),buffer2 );
    {
        int char_max = CHAR_getCharNum();
        int char_use = 0 ;
		int	pet_use = 0;
        for(i=0;i<char_max;i++){
            if( CHAR_getCharUse(i) ){
                char_use++;
            	if( CHAR_getInt( i, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
            		pet_use ++;
            	}
            }
        }
        snprintf( buffer2, sizeof( buffer2 ) ,
                  "char_use=%d\n"
                  "char_max=%d\n"
                  "pet_use=%d\n",
                  char_use , char_max, pet_use );
        strcatsafe( buffer , sizeof(buffer),buffer2 );
    }
    {

        int i;
        int item_max = ITEM_getITEM_itemnum();
        int item_use = 0;
        for(i=0;i<item_max;i++){
            if( ITEM_getITEM_use( i ) ){
                item_use ++;
            }
        }
        snprintf( buffer2, sizeof(buffer2),
                  "item_use=%d\n"
                  "item_max=%d\n",
                  item_use , item_max );
        strcatsafe( buffer , sizeof(buffer),buffer2);
    }
    {
        int i , obj_use =0;
        int obj_max = OBJECT_getNum();
        for(i=0;i<obj_max;i++){
            if( OBJECT_getType(i) != OBJTYPE_NOUSE ){
                obj_use ++;
            }
        }
        snprintf( buffer2, sizeof(buffer2) ,
                  "object_use=%d\n"
                  "object_max=%d\n",
                  obj_use , obj_max );
        strcatsafe( buffer , sizeof(buffer) , buffer2 );
    }
    if( mode == 0 ) {
    	printl( LOG_PROC , buffer );
	}else if( mode == 1 ) { 
		lssproto_ProcGet_send( fd, buffer);
	}
}

/*------------------------------------------------------------
 * cdkey 井日 fd 毛  月［
 * 娄醒
 *  cd      char*       cdkey
 * 忒曰袄
 *  白央奶伙犯奴旦弁伉皿正  方匀化｝ -1 及凛反巨仿□
 ------------------------------------------------------------*/
ANYTHREAD int getfdFromCdkeyWithLogin( char* cd )
{
    int i;

    for( i = 0 ;i < ConnectLen ; i ++ ){
        CONNECT_LOCK(i);
        if( Connect[i].use == TRUE
            && Connect[i].state != NOTLOGIN // Nuke 0514: Avoid duplicated login
            && strcmp( Connect[i].cdkey , cd ) == 0 ){
            CONNECT_UNLOCK(i);
            return i;
        }
        CONNECT_UNLOCK(i);
    }

    return -1;
}


/***********************************************************************
  MTIO 楮  
***********************************************************************/  
#if USE_MTIO

#include <pthread.h>

/*
    邰卅  醒凶切
 */
pthread_t main_thread , io_thread;
int mt_r1, mt_r2;

/*
  IO旦伊永玉及  匹及夫弘 
 */
MUTLITHREAD void MTIO_print( char *s )
{
    fprintf( stderr , "%s", s );
}

/*
  
  穴伙民旦伊永玉棕雁毛赓渝祭允月
  
 */
static void MTIO_loop( void );
void mainloop(void);     /* main.c */
SINGLETHREAD int
MTIO_setup( void )
{
    if( pthread_create( &main_thread , NULL , (void *)mainloop,
                        &mt_r1 ) ){
        print( "cannot create main thread\n" );
        return -1;
    }
    if( pthread_create( &io_thread , NULL, (void*)MTIO_loop, &mt_r2 ) ){
        print( "cannot create io thread\n" );
        return -1;
    }
    return 0;
}
SINGLETHREAD void
MTIO_join( void )
{
    pthread_join( main_thread, NULL );
    pthread_join( io_thread, NULL );
}


/*
 * 引月切允木匀升  appendRB
 * TRUE : 岳  
 * FALSE : 撩  
 *
 * 签账仇及丹尹卅中卅［
 */
MUTLITHREAD static BOOL MTIO_appendRB( int conindex, char *src, int length )
{
    CONNECT_LOCK(conindex);
    if( ( Connect[conindex].rbuse + length ) > MTIO_FIXED_BUFSIZE ){
        CONNECT_UNLOCK(conindex);
        return FALSE;
    }
    memcpy( Connect[conindex].rb + Connect[conindex].rbuse ,
            src, length );
    Connect[conindex].rbuse += length;
    
    CONNECT_UNLOCK(conindex);

    return TRUE;
}
MUTLITHREAD static BOOL MTIO_appendWB( int conindex, char *src, int length )
{
    CONNECT_LOCK(conindex);
    if( ( Connect[conindex].wbuse + length ) > MTIO_FIXED_BUFSIZE ){
        CONNECT_UNLOCK(conindex);
        return FALSE;
    }
    memcpy( Connect[conindex].wb + Connect[conindex].wbuse ,
            src, length );
    Connect[conindex].wbuse += length;        
    
    CONNECT_UNLOCK(conindex);
    return FALSE;
}

MUTLITHREAD static void MTIO_shiftWB( int conindex, int len )
{
    CONNECT_LOCK(conindex);
    memmove( Connect[conindex].wb, Connect[conindex].wb + len,
             Connect[conindex].wbuse - len );
    Connect[conindex].wbuse -= len;
    CONNECT_UNLOCK(conindex);
}

/*
  1垫午曰分允［午曰分仄凶卅互今毛井尹允
  嫖簧祭及凶户｝ outlen 卞反勾友卞蜗坌卅扔奶术互反中匀化月午移烂允月冗
 */
MUTLITHREAD int MTIO_getOneLine( int conindex, char *out, int outlen )
{
    int i;
    char *p;
    CONNECT_LOCK(conindex);
    
    p = Connect[conindex].rb;
    for(i=0 ; i < (outlen-1) && i < Connect[conindex].rbuse ; i++ ){
        out[i] = p[i];
        if( p[i] == '\n' ){
            int len = i+1;
            /* 敦岳仄化中月″″*/
            memmove( Connect[conindex].rb, Connect[conindex].rb+len,
                     Connect[conindex].rbuse - len );
            Connect[conindex].rbuse -= len;
            CONNECT_UNLOCK(conindex);
            out[len]=0;
            
            return len;
        }
    }
    /* 垫反卅中 */
    CONNECT_UNLOCK(conindex);
    return 0;
}



/*
  IO 分仃毛支月捩菁伙□皿
 */
MUTLITHREAD static void
MTIO_loop( void )
{
    while(1){
        int ret , i;
        fd_set fds;
        struct timeval tmv;
        char tmp[MTIO_FIXED_BUFSIZE];

        MTIO_print("$");
#if MTIO_DEBUG_LOG_REDUCE
        usleep( 100 * 1000 );
#endif        
        
        /* 引内accept仄切扎丹升 */
        tmv.tv_sec = tmv.tv_usec = 0;
        FD_ZERO( &fds );
        FD_SET( bindedfd, &fds );
        ret = select( bindedfd+1, &fds, (fd_set*)NULL,(fd_set*)NULL,&tmv);
        if( ret > 0 && FD_ISSET( bindedfd, &fds ) ){
            struct sockaddr_in sin;
            int addrlen = sizeof( struct sockaddr_in );
            int sockfd;
            sockfd = accept( bindedfd, (struct sockaddr*) &sin, &addrlen );
            if( sockfd == -1 && errno == EINTR ){
                ;
            } else if( sockfd != -1 ){
                int acceptmore = SERVSTATE_getAcceptmore();
                MTIO_print("CO" );
                if( acceptmore <= 0 ){
                    MTIO_print( "accept but drop\n" );
                    close( sockfd );
                } else if( sockfd < ConnectLen ){
                    if( getNodelay() ){

                        int flag = 1, result;
                        result = setsockopt( sockfd, IPPROTO_TCP, TCP_NODELAY,
                                             (char*)&flag,sizeof(int));
                        if( result == 0 ) MTIO_print( "NO" );
                    }
                    initConnectOne( sockfd,&sin, addrlen );
                } else {
                    MTIO_print( "CO_full" );
                    close(sockfd );
                }
            }
        }     /* FD_ISSET */

        FD_ZERO( &fds );
        for(i = 0; i< ConnectLen ; i++ ){
            CONNECT_LOCK(i);
            if( Connect[i].use ) FD_SET( i , &fds );
            CONNECT_UNLOCK(i);
        }
        /* 穴伙民旦伊永玉卅及匹谨匀化手方中及分［ 50ms引匀化心月*/
        tmv.tv_sec = 0;
        tmv.tv_usec = 50 * 1000;
        ret = select( ConnectLen+1, &fds, (fd_set*)NULL,(fd_set*)NULL,&tmv );
		if( ret > 0 ) {
			for(i=0;i<ConnectLen;i ++ ){
				int r;
				int readsize;
				
				if( !FD_ISSET( i, &fds ) ){
					continue;
				}
				CONNECT_LOCK_ARG2(i,2593);
				readsize = MTIO_FIXED_BUFSIZE - Connect[i].rbuse ;
				CONNECT_UNLOCK_ARG2(i,2596);
				r = read( i, tmp, readsize );
                
				if( r > 0 ){
					MTIO_appendRB( i, tmp, r );
				} else {
					if( i == acfd ){
						MTIO_print( "saac is down???? \n" );
						sigshutdown( -1 );
						exit(1);
					} else {
						MTIO_print("REr" );
						/* 穴伙民旦伊永玉匹反｝白仿弘毛凶化月分仃卞允月 */
						CONNECT_setClosed(i,1);
					}
				}
			}
		}
        /* 勾亢卞write  */
        FD_ZERO( &fds );
        for(i=0;i<ConnectLen;i++){
            CONNECT_LOCK_ARG2(i,2618);
            if( Connect[i].use && Connect[i].wbuse > 0 ) FD_SET( i ,&fds );
            CONNECT_UNLOCK_ARG2(i,2620);
        }
        tmv.tv_sec = tmv.tv_usec = 0;
        ret = select( ConnectLen+1, (fd_set*)NULL,&fds,(fd_set*)NULL,&tmv);
		if( ret > 0 ) {
			for(i=0;i<ConnectLen; i++ ){
				int w;
				
				if( !FD_ISSET( i, &fds ) ){
					continue;
				}
				
				CONNECT_LOCK_ARG2(i,2632);            
				w = write( i , Connect[i].wb, Connect[i].wbuse );
				CONNECT_UNLOCK_ARG2(i,2634);
				
				if( w > 0 ){
					MTIO_shiftWB( i, w );
				} else if( w == -1 ){
					MTIO_print( "WEr" );
					CONNECT_setClosed(i,1);
					close(i);
					continue;
				}
			}
		}
    }
}
/*
  I/O匹卅中旦伊永玉井日及心裟壬木月［
  close反I/O旦伊永玉匹反域濠方壬卅中
 */
MUTLITHREAD static void closeCheck( void )
{
    int i;
    for(i=0;i<ConnectLen;i++){
        if( CONNECT_getUse_debug(i,2628) &&
            CONNECT_getClosed(i) ){
            print("CLOSING %d\n",i );
            close(i);
            CONNECT_endOne_debug(i);
        }
    }
}

/* netloop 反丢奶件旦伊永玉匹裟太分今木月［ 烂聒仄卅仃木壬卅日卅中 */
MUTLITHREAD BOOL netloop( void )
{
    return netloop_faster();
}
MUTLITHREAD BOOL netloop_faster( void )
{
    int i;
    struct timeval tmv;
    static time_t faster_display_interval;
    if( faster_display_interval != time(NULL )){
        static int mtio_faster_counter =0 ;
        char msg[100];
        snprintf( msg, sizeof(msg), "#%d", ++mtio_faster_counter );
        MTIO_print( msg );
        faster_display_interval = time(NULL);
    }

    gettimeofday( &tmv, NULL );
    
    for(i=0; i< ConnectLen ; i++ ){
        char oneline[MTIO_FIXED_BUFSIZE];
        int ll;
        if( CONNECT_getUse_debug(i,2527) == FALSE )continue;
        if( CONNECT_getState(i) == WHILECLOSEALLSOCKETSSAVE ) continue;
        ll = MTIO_getOneLine( i, oneline, sizeof(oneline) );
        if( !( ( oneline[0] == '\r' && oneline[1] == '\n' ) ||
               oneline[0] == '\n' || ll == 0 ) ){
            if( i == acfd ){
                fprintf(stderr,"acDISP:[%s]", oneline);
                if( saacproto_ClientDispatchMessage( i, oneline )<0){
                    CONNECT_incrementErrornum(i);
                }
#ifdef _M_SERVER
			}else if( mfd == i ){
#endif
            } else {
                fprintf(stderr,"lssDISP:[%s]", oneline);
                if( lssproto_ServerDispatchMessage( i, oneline )<0){
					//print(" DispatchMsg_Error!!! ");
                    CONNECT_incrementErrornum(i);                    
                }
            }

            if( Connect[i].errornum > allowerrornum ){
                CONNECT_setClosed(i,1);
                close(i);
                continue;
            }
        }
    }
    {
        static struct timeval cacd_check_store_time;        
        if( time_diff( NowTime , cacd_check_store_time ) > 0.1 ){
           cacd_check_store_time = NowTime;
           CAcheck();
           CDcheck();
           closeCheck();
           MTIO_print("!");           
        }
    }
    return TRUE;
}

MUTLITHREAD int lsrpcClientWriteFunc( int fd , char* buf , int size )
{
    int r;

    if( CONNECT_getUse_debug(fd,2573) == FALSE ){
        return FALSE;
    }
    r = MTIO_appendWB( fd, buf, size );
    return r;
}


#endif /* if  USE_MTIO */
// Nuke start 08/27: For acceleration avoidance
//ttom+1
#define m_cktime 500
//static float m_cktime=0;

int checkWalkTime(int fd)
{
 int me, interval;
 //ttom
 float f_Wtime,f_WLtime,f_interval;
 // Nuke
 return 0;
 
  //ARM&Tom correct
  //me=CONNECT_getCharaindex(fd);
  me=fd;
  Connect[me].Walktime = time(0);
  gettimeofday( &Connect[me].Wtime, (struct timezone*)NULL);//
  interval = abs(Connect[me].Walktime - Connect[me].lastWalktime);
  //ttom
  f_Wtime =Connect[me].Wtime.tv_sec*1000+Connect[me].Wtime.tv_usec/1000;
  f_WLtime=Connect[me].WLtime.tv_sec*1000+Connect[me].WLtime.tv_usec/1000;
  f_interval=abs(f_Wtime-f_WLtime);
  //ttom
  Connect[me].Walkrestore--;

  if (Connect[me].Walkrestore <= 0) {
     //print("Walkspool restored ");
     Connect[me].Walkspool = WALK_SPOOL;
     Connect[me].Walkrestore = WALK_RESTORE;
  }
  if (f_interval > m_cktime ) {
      Connect[me].WLtime = Connect[me].Wtime;
      Connect[me].Walkcount = 0;
  } else {
    Connect[me].Walkcount++;
    if (Connect[me].Walkcount > 1) {
        Connect[me].Walkspool--;
        if (Connect[me].Walkspool > 0) {
            Connect[me].Walkcount = 0;
            print("Walkspool consumed as %d", Connect[me].Walkspool);
            return 0;
        }
        /*Connect[me].Walkspool=WALK_SPOOL;
        Connect[me].Walkrestore=WALK_RESTORE;
        print("Walk dropped \n");
        Connect[me].credit=-10;
        return 0;*/
        return -1;
    }
  }
  return 0;
}
int setBtime(int fd)
{
    int me, interval;
    //ARM & Tom
    //me=CONNECT_getCharaindex(fd);
    me=fd;
    Connect[me].BEOrestore--;
    if (Connect[me].BEOrestore <= 0) {
       Connect[me].BEOrestore = BEO_RESTORE;
       Connect[me].BEOspool = BEO_SPOOL;
       //print("BEOspool restored ");
    }
    Connect[me].lastlastBtime = Connect[me].lastBtime;
    Connect[me].lastBtime = Connect[me].Btime;
    Connect[me].Btime = time(0);
    interval = abs(Connect[me].Btime - Connect[me].lastlastBtime);
    //print("B3interval:%d ",interval);
    if ( interval < B3_TOLERANCE ) {
       Connect[me].BEOspool--;
       //print("B3spool consumed as:%d ",Connect[me].BEOspool);
       if (Connect[me].BEOspool <= 0) return -1;
          else return 0;
    } else return 0;
}
int checkBEOTime(int fd)
{
    int me, interval;
    //ARM & Tom
    //me=CONNECT_getCharaindex(fd);
    me=fd;
    Connect[me].EOtime = time(0);
    interval = abs(Connect[me].EOtime - Connect[me].Btime);
    //print("BEOinterval:%d ",interval);

    if ( interval < BEO_TOLERANCE) {
       Connect[me].BEOspool--;
       //print("BEOspool consumed as:%d ",Connect[me].BEOspool);
       // Nuke 0626: Do not kick out
       if (Connect[me].BEOspool <= 0) { Connect[me].nu_decrease++; return -1; }
       else return 0;
    } else return 0;
}
int ITEM_getRatio()
{
    int i, r;
    int item_max = ITEM_getITEM_itemnum();
    int item_use = 0;
    for(i=0;i<item_max;i++) {
        if( ITEM_getITEM_use( i ) ){
             item_use ++;
        }
    }
    r=(item_use * 100) / item_max;
    print("ItemRatio=%d%% ",r);
    return r;
}
int CHAR_players()
{
    int i;
    int     chars=0;
    int     players=0,pets=0,others=0;
    int     whichtype= -1;
    int objnum = OBJECT_getNum();
    /* 引内反obj及橇谪 */
    for( i=0 ; i<objnum ; i++){
       switch( OBJECT_getType( i )){
       	     case OBJTYPE_CHARA:
                  chars++;
                  whichtype =  CHAR_getInt( OBJECT_getIndex( i), CHAR_WHICHTYPE);
                  if( whichtype == CHAR_TYPEPLAYER) players++;
                  else if( whichtype == CHAR_TYPEPET) pets++;
                  else others ++;
                  break;
            default:
                  break;
       }
    }
    return players;
}
void sigusr1(int i)
{
    signal(SIGUSR1,sigusr1);
    cono_check=(cono_check+1)%4;
    print("Cono Check is login:%d item:%d",cono_check&1,cono_check&2);
}
// Arminius 6.26
void sigusr2(int i)
{
#ifdef _GM_SIGUSR2
	FILE *f;
	typedef void (*C)(int,char*);//功能相同typedef void (*CHATMAGICFUNC)(int,char*);
    C func;

    char message[256],message2[256];
//	int id=0;
    print("\nChange ==> 进入了sigusr2了\n");
    f = fopen("./gm_sigusr_command", "r");
	memset( message, 0, sizeof(message));
	memset( message2, 0, sizeof(message2));

	if( f ){
		fscanf(f,"%s",message);
        print("fev ==> 档案内容(%s)\n",message);
		if( strcmp(message,"") == 0 
			|| strcmp(message,"Shutdown") == 0){
			if( fgets(message2, sizeof(message2), f) ){
				print("\nReceived Shutdown signal...n分钟後维护\n\n");
                lssproto_Shutdown_recv(0, "hogehoge", atoi(message2));	// n分钟後维修
			}
			else{
                print("\nReceived Shutdown signal...\n\n");
                lssproto_Shutdown_recv(0, "hogehoge", 5);	// 5分钟後维修
			}
		}
		else
		{
			print("fev ==> 执行GM指令\n");
			func = gm_CHAR_getChatMagicFuncPointer(message,TRUE);
			if( func ){
				if( fgets(message2, sizeof(message2), f) )
					print("fev ==> 参数内容(%s)\n",message2);
		            
				func(1000,message2);
			}
		}
	    fclose(f);    
	}
    else{
        print("\nReceived Shutdown signal...\n\n");
        lssproto_Shutdown_recv(0, "hogehoge", 5);	// 5分钟後维修
	}
	signal(SIGUSR2,sigusr2);
#else
    signal(SIGUSR2,sigusr2);
    print("\nReceived Shutdown signal...\n\n");
    lssproto_Shutdown_recv(0, "hogehoge", 5);	// 5分钟後维修
#endif
}

// Nuke end
//ttom start
void CONNECT_set_watchmode(int fd, BOOL B_Watch)
{
     int me;
     me=CONNECT_getCharaindex(fd);
     Connect[me].in_watch_mode = B_Watch;
}
BOOL CONNECT_get_watchmode(int fd)
{
     int me;
     BOOL B_ret;
     me=CONNECT_getCharaindex(fd);
     B_ret=Connect[me].in_watch_mode;
     return B_ret;
}
BOOL CONNECT_get_shutup(int fd)
{
     int me;
     BOOL B_ret;
     me=CONNECT_getCharaindex(fd);
     B_ret=Connect[me].b_shut_up;
     return B_ret;
}
void CONNECT_set_shutup(int fd,BOOL b_shut)
{
     int me;
     me=CONNECT_getCharaindex(fd);
     Connect[me].b_shut_up = b_shut;
}
unsigned long CONNECT_get_userip(int fd)
{
    unsigned long ip;
    memcpy(&ip,&Connect[fd].sin.sin_addr, sizeof(long));
    return ip;
}
void CONNECT_set_pass(int fd,BOOL b_ps)
{
     int me;
     me=CONNECT_getCharaindex(fd);
     Connect[me].b_pass = b_ps;
}
BOOL CONNECT_get_pass(int fd)
{
    int me;
    BOOL B_ret;
    me=CONNECT_getCharaindex(fd);
    B_ret=Connect[me].b_pass;
    return B_ret;
}
void CONNECT_set_first_warp(int fd,BOOL b_ps)
{
     int me;
     me=CONNECT_getCharaindex(fd);
     Connect[me].b_first_warp = b_ps;
}
BOOL CONNECT_get_first_warp(int fd)
{
    int me;
    BOOL B_ret;
    me=CONNECT_getCharaindex(fd);
    B_ret=Connect[me].b_first_warp;
    return B_ret;
}
void CONNECT_set_state_trans(int fd,int a)
{
   int me;
   me=CONNECT_getCharaindex(fd);
   Connect[me].state_trans=a;
}
int CONNECT_get_state_trans(int fd)
{
  int me,i_ret;
  me=CONNECT_getCharaindex(fd);
  i_ret=Connect[me].state_trans;
  return i_ret;   
}
//ttom end

// Arminius 6.22 encounter
int CONNECT_get_CEP(int fd)
{
  return Connect[fd].CEP;
}

void CONNECT_set_CEP(int fd, int cep)
{
  Connect[fd].CEP=cep;
}
// Arminius end

// Arminius 7.12 login announce
int CONNECT_get_announced(int fd)
{
  return Connect[fd].announced;
}

void CONNECT_set_announced(int fd, int a)
{
  Connect[fd].announced=a;
}

// shan trade(DoubleCheck) begin
int CONNECT_get_confirm(int fd)
{
	return Connect[fd].confirm_key;
}
void CONNECT_set_confirm(int fd, BOOL b)
{
	Connect[fd].confirm_key = b;
}
// end

#ifdef _BLACK_MARKET
int CONNECT_get_BMList(int fd, int i)
{	
	return Connect[fd].BMSellList[i];
}
void CONNECT_set_BMList(int fd,int i, int b)
{
	Connect[fd].BMSellList[i] = b;
}
#endif

#ifdef _NO_WARP
// shan hjj add Begin
int CONNECT_get_seqno(int fd)
{   
	return Connect[fd].seqno;
}
void CONNECT_set_seqno(int fd, int a)
{
	if( (Connect[fd].seqno==CHAR_WINDOWTYPE_QUIZ_MAIN)&&(a==0) )  
		a = CHAR_WINDOWTYPE_QUIZ_MAIN;
    Connect[fd].seqno = a; 
}
int CONNECT_get_selectbutton(int fd)
{
	return Connect[fd].selectbutton;
}
void CONNECT_set_selectbutton(int fd, int a)
{
	Connect[fd].selectbutton = a;	
}
// shan End
#endif

int isDie(int fd)
{
  return (Connect[fd].die);
}

void setDie(int fd)
{
  Connect[fd].die=1;
}
 
int checkNu(fd)
{
	Connect[fd].nu--;
	//print("NU=%d\n",Connect[fd].nu);
	if (Connect[fd].nu<0) return -1;
	return 0;
}

int checkKe(fd)
{
	Connect[fd].ke--;
	//print("KE=%d\n",Connect[fd].ke);
	if (Connect[fd].ke<0) return -1;
	return 0;
}

// Nuke start 0626: For no enemy function
void setNoenemy(fd)
{
        Connect[fd].noenemy=6;
}
void clearNoenemy(fd)
{
        Connect[fd].noenemy=0;
}
int getNoenemy(fd)
{
        return Connect[fd].noenemy;
}
// Nuke end

// Arminius 7/2: Ra's amulet
void setEqNoenemy(int fd, int level)
{
        Connect[fd].eqnoenemy=level;
}
        
void clearEqNoenemy(int fd)
{
        Connect[fd].eqnoenemy=0;
}
                
int getEqNoenemy(int fd)
{
        return Connect[fd].eqnoenemy;
}

#ifdef _Item_MoonAct
void setEqRandenemy(int fd, int level)
{
        Connect[fd].eqrandenemy=level;
}
        
void clearEqRandenemy(int fd)
{
        Connect[fd].eqrandenemy=0;
}
                
int getEqRandenemy(int fd)
{
        return Connect[fd].eqrandenemy;
}

#endif

#ifdef _CHIKULA_STONE
void setChiStone(int fd, int nums)
{
	Connect[fd].chistone=nums;
}
int getChiStone(int fd)
{
	return Connect[fd].chistone;
}
#endif

// Arminius 7.31 cursed stone
void setStayEncount(int fd)
{
	Connect[fd].stayencount=1;
}

void clearStayEncount(int fd)
{
	Connect[fd].stayencount=0;
}

int getStayEncount(int fd)
{
	return Connect[fd].stayencount;
}

void CONNECT_setBDTime( int fd, int nums)
{
	Connect[fd].BDTime = nums;
}

int CONNECT_getBDTime( int fd)
{
	return Connect[fd].BDTime;
}

#ifdef _TYPE_TOXICATION
void setToxication( int fd, int flg)
{
	Connect[fd].toxication = flg;
}
int getToxication( int fd)
{
	return Connect[fd].toxication;
}
#endif

#ifdef _BATTLE_TIMESPEED
void RescueEntryBTime( int charaindex, int fd, unsigned int lowTime, unsigned int battletime)
{
	int NowTime = (int)time(NULL);

	Connect[fd].CBTime = NowTime;
//Connect[fd].CBTime+battletime
}

BOOL CheckDefBTime( int charaindex, int fd, unsigned int lowTime, unsigned int battletime, unsigned int addTime)//lowTime延迟时间
{
	int delayTime = 0;
	unsigned int NowTime = (unsigned int)time(NULL);

	//print(" NowTime=%d lowTime=%d battleTime=%d CBTime=%d", NowTime, lowTime, battletime, Connect[fd].CBTime);

	lowTime += battletime;
	if( (Connect[fd].CBTime+battletime) > lowTime ) lowTime = Connect[fd].CBTime+battletime;
	if(  NowTime < lowTime ){//lowTime应该的战斗结束时间
		int r=0;
		delayTime = lowTime - NowTime;
		delayTime = ( delayTime<=0 )?1:delayTime;
		r = (-4)*(delayTime+2);
		lssproto_NU_send( fd, r);
		Connect[fd].nu += r;
	}
	//Connect[fd].BDTime = (NowTime+20)+delayTime;
	Connect[fd].BDTime = (NowTime+rand()%5)+delayTime+addTime; // 恶宝等待时间
	//print(" BDTime=%d ", Connect[fd].BDTime);
	return TRUE;
}
#endif

#ifdef _CHECK_GAMESPEED
int getGameSpeedTime( int fd)
{
	return Connect[fd].gamespeed;
}
void setGameSpeedTime( int fd, int times)
{
	Connect[fd].gamespeed =times;
}
#endif


BOOL MSBUF_CHECKbuflen( int size, float defp)
{
	if( mfd == -1 ) return FALSE;
	if( Connect[mfd].wbuse + size >= WBSIZE*defp)	return FALSE;

	return TRUE;
}

