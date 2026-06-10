#include "version.h"
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <stdio.h>

#include "buf.h"
#include "net.h"
#include "char.h"
#include "item.h"
#include "object.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "log.h"
#include "petmail.h"

/*------------------------------------------------------------
 * lsprotocol 及伐□平件弘    毛荸  允月楮醒毛裟少［
 * 娄醒｝忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
static void endlsprotocol( void )
{
    lssproto_CleanupServer();
    saacproto_CleanupClient();
}

/*------------------------------------------------------------
 * 允屯化及忡绣仄卅仃木壬卅日卅中犯□正毛母件皿允月楮醒
 * 公木冗木及乒斥亘□伙及楮醒毛裟少及心［
 * 娄醒｝忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
static void allDataDump( void )
{
    closeAllLogFile();
#ifndef _SIMPLIFY_ITEMSTRING
	storeObjects( getStoredir() );
	storePetmail();
#endif
	storeCharaData();
}

/*------------------------------------------------------------
 * 皿夫弘仿丞及蔽  质  及凶户卞裟太请今木月［
 * 娄醒｝忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
void shutdownProgram( void )
{
    close( acfd );
    close( bindedfd );
    endlsprotocol();
    endConnect();
    memEnd();
}

char *DebugFunctionName = NULL;
int DebugPoint = 0;

void sigshutdown( int number )
{


#if USE_MTIO
    {
        void MTIO_join(void);
        MTIO_join();
    }
#endif        
    print( "\n标准信息: %d\n" , number  );
    if( number == 0 )print( "\nGMSV正常关闭\n" );
    print( "\n调试点数(%d)\n", DebugPoint );
    print( "\n最后函数 (%s)\n", DebugFunctionName );

	remove( "gmsvlog.err2");
	rename( "gmsvlog.err1", "gmsvlog.err2" );
	rename( "gmsvlog.err", "gmsvlog.err1" );
	rename( "gmsvlog", "gmsvlog.err" );

    allDataDump();
    

    signal( SIGINT , SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGKILL, SIG_IGN );
    signal( SIGSEGV, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, SIG_IGN );


    shutdownProgram();
    exit(number);
}



void signalset( void )
{
    // CoolFish: Test Signal 2001/10/26
    print("\n开始获取信号..\n");

	print("SIGINT:%d\n", SIGINT);
	print("SIGQUIT:%d\n", SIGQUIT);
	print("SIGKILL:%d\n", SIGKILL);
	print("SIGSEGV:%d\n", SIGSEGV);
	print("SIGPIPE:%d\n", SIGPIPE);
	print("SIGTERM:%d\n", SIGTERM);
    
    signal( SIGINT , sigshutdown );
    signal( SIGQUIT, sigshutdown );
    signal( SIGKILL, sigshutdown );
    signal( SIGSEGV, sigshutdown );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, sigshutdown );
}
