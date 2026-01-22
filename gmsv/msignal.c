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
 * lsprotocol 摯極↓す璃精    禱搣  埰堎匴倳禱蠙屾��
 * 礎倳��蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
static void endlsprotocol( void )
{
    lssproto_CleanupServer();
    saacproto_CleanupClient();
}

/*------------------------------------------------------------
 * 埰迋趙摯瞀凎媃埵崹躂�屻牟欷聿郱腹灊�禱譫璃鏤埰堎匴倳
 * 鼠躂�葸噯副像畽芋齠嚗啗�倳禱蠙屾摯陑��
 * 礎倳��蒍堇偯
 *  埵媃
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
 * 鏤痲精溘堜摯敖  窐  摯倜誧勗蠙怮③踏躂堎��
 * 礎倳��蒍堇偯
 *  埵媃
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
    print( "\n梓袧陓洘: %d\n" , number  );
    if( number == 0 )print( "\nGMSV淏都壽敕\n" );
    print( "\n覃彸萸杅(%d)\n", DebugPoint );
    print( "\n郔綴滲杅 (%s)\n", DebugFunctionName );

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
    print("\n羲宎鳳�－驕�..\n");

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
