#ifndef _MAIN_H_
#define _MAIN_H_



#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>

#include <errno.h>
#include <string.h>
#include "version.h"

#ifdef _MAIN_C_
#define EXT
#else
#define EXT extern
#endif

#define CHARDATASIZE ( 256 * 1024 )
#define MAXWK 50//30

#ifdef _FIX_WORKS
void set_nodelay( int sock );

#define OK     0
#define TCPSTRUCT_ENOMEM -1       /* malloc ¡√   */
#define TCPSTRUCT_ESOCK -2        /* socket ¡√   */
#define TCPSTRUCT_EBIND -3        /* bind ¡√   */
#define TCPSTRUCT_ELISTEN -4      /* listen ¡√   */
#define TCPSTRUCT_EBUG -6         /* ÃÔ∫Î∆•ÿ§‘¬ */
#define TCPSTRUCT_EINVCIND -7     /* con≥ﬂº∞indexª•‘∆æÆÿ∆÷–∑Ω */
#define TCPSTRUCT_EREADFIN -8     /* read ‘ ‘¬∑∏°ı’˝ª•ÿ¶» ªØ closed by remote */
#define TCPSTRUCT_EHOST -9        /* gethostbyname ¡√   */
#define TCPSTRUCT_ECONNECT -10    /* connect ¡√   */
#define TCPSTRUCT_ECFULL -11      /* con ª•÷–‘»ÃÏ÷– */
#define TCPSTRUCT_ETOOLONG -12    /* µÊª•ÿ¶ª•‘ ø∫ */
#define TCPSTRUCT_EMBFULL -13     /* mb ª•÷–‘»ÃÏ÷–  */
#define TCPSTRUCT_ECLOSEAGAIN -14 /* close ª•2º‘ΩÒƒæ–◊ */
#define TCPSTRUCT_WORKERROR -16
#define TCPSTRUCT_WORKEBIND -17

#define INIT_SOMEERROR -18

extern time_t sys_time; // Robin add

struct membuf
{
    int use;
    char buf[512];
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

#define FORWORKPORT 9400
typedef enum
{
    WKSTAT_IDLE = 0,
    WKSTAT_WAITING = 2,
	WKSTAT_DIE = 3,
} WKSTAT;

#define CPARAMNUM 5
#define IPARAMNUM 3

struct worker
{
    int use;
    int port;
#ifdef _FIX_WORKS
	int fd;
#endif
    int ti;
    int request_ti;
    WKSTAT status;
    time_t last_query;
    void (*callback)( int ti , int result , char *cparam0 , char *cparam1,
                      char *cparam2 , char *cparam3, char *cparam4, 
                      int iparam1 , int iparam2 );
    char cparam[CPARAMNUM][CHARDATASIZE];
    int iparam[IPARAMNUM];
};

struct worker wk[MAXWK];

#endif
char *chartime();
#define LOGBASE( filename , format , args...)\
{\
    FILE *f;\
    f = fopen( filename , "a" );\
    if( f != NULL ){\
        fprintf( f , format , ## args);\
        fclose(f);\
    } else{ fprintf( stderr,"fuckup! log fail!:%s:%s\n", filename,strerror(errno));}\
}

// Spock 2000/10/27
#define log(format, args...)    \
{\
    fprintf( stderr, format , ## args );\
}
// Spock end
#define SUCCESSFUL "successful"
#define FAILED "failed"

#ifdef _BAD_PLAYER
#define BADPLAYER "badplayer"
#endif

#define MAXCONNECTION 50
#define WKTIMEOUT 120
#define USERID_MAX 32
#define CHARNAME_MAX 32

EXT char svpass[1024];
EXT char topdir[1024];
EXT char glogdir[1024];
EXT char chardir[1024];
EXT char logdir[1024];
EXT char dbdir[1024];
EXT char lockdir[1024];
EXT char wklogdir[1024];
EXT char maildir[1024];

// CoolFish: Family 2001/5/9
EXT char familydir[1024];
EXT char fmpointdir[1024];
EXT char fmsmemodir[1024];
#ifdef _LOCK_SERVER
EXT char SaName[1024];
EXT char LSADDRESS[1024];
EXT char TESTNAME[1024];
EXT int LSPORT;
#endif
EXT int wknum;
EXT int log_rotate_interval;
EXT int total_ok_charlist, total_ng_charlist;

#ifdef _OACSTRUCT_TCP
EXT int OACbindtype;
#endif

EXT char saacname[1024];
//EXT char movelist[1024];

#ifdef _SLEEP_CHAR
EXT char sleepchardir[1024];
#endif


typedef struct _gmsv
{
    int use;
	int fd;
    char name[1024];
} gmsv;

int addWorkerTask( char *id , char *pas , char *flag ,
					void (*callbk)( int ti , int result ,
					char *cp0 , char *cp1 , char *cp2 ,
					char *cp3 , char *cp4, int i0 , int i1) ,
					int ti ,
					char *cparam0 , char *cparam1 ,
					char *cparam2 , char *cparam3 , char *cparam4 , 
					int iparam0 , int iparam1);

int get_rotate_count(void );
void checkGSUCheck( char *id );
int logout_game_server( int ti );
int is_game_server_login( int ti );
char * getGSName( int i );

void gmsvBroadcast( int fd, char *p1, char *p2, char *p3 , int flag );
int login_game_server( int ti, char *svname, char *svpas, char *result,
					  int resultlen , char *retdata , int retdatalen );

#ifdef _ANGEL_SUMMON
typedef enum
{
	MISSION_NONE =0,
	MISSION_WAIT_ANSWER,
	MISSION_DOING,
	MISSION_HERO_COMPLETE,
	MISSION_TIMEOVER,
}ANGEL_MISSIONFLAG;
#endif


#undef EXT
#endif





