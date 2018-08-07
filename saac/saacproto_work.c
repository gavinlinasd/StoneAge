#include "version.h"
#include "main.h"
#include "recv.h"
#ifdef _FIX_WORKS
#include "saacproto_serv.h"
#include "saacproto_work.h"
#include "char.h"

extern int worksockfd;
struct timeval select_timeout;
static int findWk = 0;
int FindWorkRegBlankCon( void );


int SetWorkConnectionInit( int mem_use)
{
	cWork = ( struct connection *) calloc( 1, MAXWORKCONNECTION * sizeof( struct connection ));
    if( cWork == NULL ){
        //free( wb );
        return 0;
    } else {
        int i;
        for( i=0; i<MAXWORKCONNECTION; i++){
            cWork[i].use = 0;
            cWork[i].fd = -1;
        }
    }
	return 1;
}

int tcpstruct_work_accept( int *tis , int ticount )
{
    int i,j;
    int sret;
    int accepted = 0;
    struct timeval t;
	char tmpbuf[256];
    fd_set rfds, wfds , efds;  

	FD_ZERO( &rfds );
    FD_ZERO( &wfds );
    FD_ZERO( &efds );    
    select_timeout.tv_sec = 0;
    select_timeout.tv_usec = 0;

    for( i=0; i<MAXWORKCONNECTION; i++){
        if( cWork[i].use && cWork[i].fd >= 0 && cWork[i].closed_by_remote ==0 ){
            FD_SET( cWork[i].fd , & rfds );
            FD_SET( cWork[i].fd , & wfds );
            FD_SET( cWork[i].fd , & efds );
        }
    }

    t = select_timeout;
    sret = select( 1024, & rfds , (fd_set*)NULL, & efds , &t);
	if( sret > 0 ) {
		for(i=0;i< MAXWORKCONNECTION;i++){
			if( cWork[i].use && ( cWork[i].fd >= 0 ) && FD_ISSET( cWork[i].fd , &rfds ) ){
				int rr , readsize ;
				readsize = sizeof( tmpbuf);
				memset( tmpbuf, 0, sizeof( tmpbuf));
				rr = read( cWork[i].fd , tmpbuf , readsize );
				if( rr <= 0 ){
					cWork[i].closed_by_remote = 1;
				} else {
					MEMBuffer_AddWkReadBuffer( i, tmpbuf, WK_R);
				}
			}
		}
	}
    t = select_timeout;    
    sret = select( 1024 , (fd_set*)NULL, &wfds, & efds , &t);
	if( sret > 0 ) {
		for(i=0;i<MAXWORKCONNECTION;i++){
			if( cWork[i].use && ( cWork[i].fd >= 0 ) && FD_ISSET( cWork[i].fd , &wfds )){
				int rr;
				memset( tmpbuf, 0, sizeof( tmpbuf));
				if( MEMBuffer_getWkLineReadBuffer( i, tmpbuf, sizeof( tmpbuf), WK_W) <= 0 )
					continue;
				rr = write( cWork[i].fd , tmpbuf , strlen(tmpbuf) );
				if( rr <= 0 ){
					cWork[i].closed_by_remote = 1;
				}else	{
				}
			}
		}
	}
    for(i=0; i<ticount; i++){
        int asret;
        struct timeval t;

        t.tv_sec =0;
        t.tv_usec =0;

        FD_ZERO( & rfds );
        FD_ZERO( & wfds );
        FD_ZERO( & efds );
        FD_SET( worksockfd , & rfds );
        FD_SET( worksockfd , & wfds );
        FD_SET( worksockfd , & efds );
        asret = select( 1024, &rfds , &wfds , &efds, &t );
        //if( FD_ISSET( worksockfd , & rfds )){
		if( (asret>0) && FD_ISSET( worksockfd , & rfds )){
            struct sockaddr_in c;
            int len , newsockfd;
            int newcon;
            bzero( &c , sizeof( c ));
            len = sizeof( c );
			newcon = FindWorkRegBlankCon();
            //newcon = FindWorkRegBlankCon( );
            if( newcon < 0 ){
				continue;
			}
            newsockfd = accept( worksockfd , (struct sockaddr*)&c , &len );
			
            if( newsockfd < 0 ){
//              UnWorkRegMemBuf( newcon );
                continue;
            }
			if( MAXWK <= findWk ) continue;
            set_nodelay( newsockfd );
            cWork[newcon].fd = newsockfd;
            memcpy( &cWork[newcon].remoteaddr , &c ,sizeof(c));
            tis[accepted] = newcon;
			//andy_add
			for( j=0; j<MAXWK; j++)	{
				findWk++;
				if( findWk >= MAXWK ) findWk=0;
				if( wk[findWk].use != 1 && wk[findWk].fd < 0 )	{
					wk[findWk].status = WKSTAT_IDLE;
					wk[findWk].use = 1;
					wk[findWk].ti = newcon;
					wk[findWk].fd = newsockfd;
					log( "同意工作: sockfd:%d,newfd:%d=aWork:%d\n" , worksockfd, newsockfd, findWk);
					break;
				}
			}
            accepted ++;
        }
    }

    return accepted;
}

int cWork_write( int ti , char *buf , int len )
{
    if( ti < 0 || ti >= MAXWORKCONNECTION || cWork[ti].use == 0 )
        return TCPSTRUCT_EINVCIND; 
	return MEMBuffer_AddWkReadBuffer( ti, buf, WK_W);
}

int Work_close( int ti )
{
    if( ti < 0 || ti >= MAXWORKCONNECTION )return TCPSTRUCT_EINVCIND;
    if( cWork[ti].use == 0 ){
        return TCPSTRUCT_ECLOSEAGAIN;
    }
    close( cWork[ti].fd );
    cWork[ti].use = 0;
    cWork[ti].fd = -1;

	MEMBuffer_clean( ti);

    cWork[ti].mbtop_ri = -1;
    cWork[ti].mbtop_wi = -1;
	
    return OK;
}
#endif


MEMBuffers memBuf[MAXWORKCONNECTION]; //30
void MEMBuffer_Init()
{
	int i;
	for( i=0; i<MAXWORKCONNECTION; i++)	{
		memset( memBuf[i].buff, 0, sizeof( memBuf[i].buff));
		memBuf[i].use = 0;
		memBuf[i].WR =-1;
	}
}

int MEMBuffer_AddWkReadBuffer( int ti, char *data, int flg)
{
	if( !memBuf[ti].use ) return -1;//flg = 1 r flg = 2 w
	if( memBuf[ti].WR != -1 ) return -1;
	memset( memBuf[ti].buff, 0, sizeof( memBuf[ti].buff));
	if( strlen( data) > sizeof( memBuf[ti].buff) ) return -1;
	memcpy( memBuf[ti].buff, data, strlen( data)+1);
	memBuf[ti].WR = flg;

	return 1;
}

int MEMBuffer_getWkLineReadBuffer( int ti, char *data, int sizes, int flg)
{
	if( !memBuf[ti].use ) return -1;
	if( memBuf[ti].WR != flg )return -1;

	if( sizes < sizeof( memBuf[ti].buff) ){
		memBuf[ti].WR = -1;
		memset( memBuf[ti].buff, 0, sizeof( memBuf[ti].buff));
		return -1;
	}
	memcpy( data, memBuf[ti].buff, sizeof( memBuf[ti].buff));
	memBuf[ti].WR = -1;
	memset( memBuf[ti].buff, 0, sizeof( memBuf[ti].buff));
	return 1;
}

void MEMBuffer_clean( int ti)
{
	memset( memBuf[ti].buff, 0, sizeof( memBuf[ti].buff) );
	memBuf[ti].use = 0;
	memBuf[ti].WR =-1;
}

int MEMBuffer_Find( int ti)
{
	if( !memBuf[ti].use ){
		memBuf[ti].use = 1;
		memBuf[ti].WR =-1;
		return 1;
	}

	return -1;
}

int FindWorkRegBlankCon( void )
{
    int i;

    for(i=0;i<MAXWORKCONNECTION;i++){
        if( cWork[i].use == 0 ){
			if( MEMBuffer_Find( i) <= 0 )	continue;
            cWork[i].use = 1;
            cWork[i].fd = -1;
            return i;
        }
    }
    return TCPSTRUCT_ECFULL;
}
