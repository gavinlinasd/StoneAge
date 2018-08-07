#include "version.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "npcutil.h"
#include "npc_oldman.h"
#include "title.h"
#include "char_data.h"
#include "readmap.h"

/* hoge */

BOOL NPC_OldmanInit( int meindex )
{

#if 0
    int     oldmanid=0, bornfl=0, bornx=0 ,borny=0;
    char*   npcarg;
    char    token[32]="";
/*    print( "长老的引数: '%s'\n" , CHAR_getChar(
                         meindex, CHAR_NPCARGUMENT ));*/
    npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

    getStringFromIndexWithDelim( npcarg,"|",1,token,sizeof(token));
    oldmanid = atoi( token );
    getStringFromIndexWithDelim( npcarg,"|",2,token,sizeof(token));
    bornfl = atoi( token );
    getStringFromIndexWithDelim( npcarg,"|",3,token,sizeof(token));
    bornx = atoi( token );
    getStringFromIndexWithDelim( npcarg,"|",4,token,sizeof(token));
    borny = atoi( token );

    /* 赢  反ID毛创尹化云仁瓜伉 */
    CHAR_setWorkInt( meindex , CHAR_WORKOLDMANID ,oldmanid );
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEHEALER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );

    if( MAP_IsValidCoordinate( bornfl,bornx,borny )== TRUE
        /*  白夫失ID互中中井升丹井毛譬屯月  */
        &&
        CHAR_ElderSetPosition( oldmanid ,bornfl,bornx, borny ) 
        == TRUE ){
        /* 赢  及匏  毛赓渝祭允月卅曰 */
        return TRUE;
    }else{
        printf( "Invalid elder npcarg=%s\n", npcarg );
        
        /* 丐引曰卞褐邰卅巨仿□卅及匹｝neverMake仄卅中［
             荚巨仿□丢永本□斥请今六月 */
        return FALSE;
    }
#endif

	return FALSE;

}


/*
 *
 *  赢  ［
 *
 *  云手卅酷仪反  仄井仃日木凶午五卞惫寞及  烂毛仄凶曰
 * 允月仇午分［
 * * by ringo
 */

void NPC_OldmanTalked( int meindex , int talkerindex , char *msg ,
                     int color )

{
    char message[1024];
    char *n = CHAR_getChar( talkerindex , CHAR_NAME );
    int title_change = 0;

    /* 反卅仄井仃凶蛊卞｝愤坌及ID毛筏盛今六月 */
    CHAR_setInt( talkerindex, CHAR_LASTTALKELDER ,
                 CHAR_getWorkInt( meindex, CHAR_WORKOLDMANID ) );

    /*

      if( title_change == 0 && strcmp( n , "苹果" ) == 0 ){
        
        if( NPC_Util_HaveTitle( talkerindex , 3 ) == 0 ){
            char *ts;
            NPC_Util_AddOneTitle( talkerindex , 3 );
            ts = TITLE_makeTitleStatusString( talkerindex , 3 );
            snprintf( message , sizeof(message),
                  "那是个不错的名字, 就叫%s吧！",
                  ts );
            title_change = 1;
        }
    }
    */
{
	int		addcnt,delcnt;
	title_change = TITLE_TitleCheck_Nomsg( talkerindex, 0, &addcnt, &delcnt);
	
    if( !title_change){
        snprintf( message, sizeof( message ) ,
              "来的好%s在给你称号时"
              "不是还没来吗？" , n );
	    CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
    }
    else {
    	#define		NPC_OLDMAN_TITLE_UNIT1		"TSU"
    	#define		NPC_OLDMAN_TITLE_UNIT2		"KO"
    	
        snprintf( message, sizeof( message ) ,"来的好%s",n );
    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		if( delcnt > 0 && addcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"你丧失了%d%s 称号的资格。"
	    			"不是得到了%d%s 的称号资格吗？"
	    			, delcnt,
	    			delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2,
	    			addcnt,
	    			addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2
	    			);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
		else if( delcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"你已丧失得到%d%s称号的资格。", delcnt,
	    			delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
		else if( addcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"给你%d%s的称号吧！", addcnt, // CoolFish: d%s -> %d%s 2001/4/18
	    			addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
	    if( delcnt > 0 ) {
	        snprintf( message, sizeof( message), 
	                    "失去%d%s 称号！", delcnt,
	    				delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	        CHAR_talkToCli( talkerindex, -1, message,  CHAR_COLORYELLOW);
	    }
	    if( addcnt > 0 ) {
	        snprintf( message, sizeof( message), 
	                    "获得%d%s 称号！", addcnt,
	    				addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	        CHAR_talkToCli( talkerindex, -1, message,  CHAR_COLORYELLOW);
	    }
    }
}
    if( title_change ) CHAR_sendStatusString( talkerindex , "T" );
    
}
