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
/*    print( "酗橾腔竘杅: '%s'\n" , CHAR_getChar(
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

    /* 荇  毀ID禱斐窇趙堁�妏瓮� */
    CHAR_setWorkInt( meindex , CHAR_WORKOLDMANID ,oldmanid );
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEHEALER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );

    if( MAP_IsValidCoordinate( bornfl,bornx,borny )== TRUE
        /*  啞痲囮ID誑笢笢凝汔竣凝禱ぅ迋堎  */
        &&
        CHAR_ElderSetPosition( oldmanid ,bornfl,bornx, borny ) 
        == TRUE ){
        /* 荇  摯痾  禱疐趵撬埰堎埵堇 */
        return TRUE;
    }else{
        printf( "Invalid elder npcarg=%s\n", npcarg );
        
        /* 堣竘堇勗福菺埵操溘↓埵摯ぁ��neverMake媃埵笢��
             樊操溘↓隍蚗掛↓喇③踏鞠堎 */
        return FALSE;
    }
#endif

	return FALSE;

}


/*
 *
 *  荇  ��
 *
 *  堁忒埵蹄痀毀  媃凝崹�桫憶谻覢撅撅嘆紡�  擭禱媃倜堇
 * 埰堎喫敁煦��
 * * by ringo
 */

void NPC_OldmanTalked( int meindex , int talkerindex , char *msg ,
                     int color )

{
    char message[1024];
    char *n = CHAR_getChar( talkerindex , CHAR_NAME );
    int title_change = 0;

    /* 毀埵媃凝崹倜嘍勗��猷覕摯ID禱楔呏踏鞠堎 */
    CHAR_setInt( talkerindex, CHAR_LASTTALKELDER ,
                 CHAR_getWorkInt( meindex, CHAR_WORKOLDMANID ) );

    /*

      if( title_change == 0 && strcmp( n , "し彆" ) == 0 ){
        
        if( NPC_Util_HaveTitle( talkerindex , 3 ) == 0 ){
            char *ts;
            NPC_Util_AddOneTitle( talkerindex , 3 );
            ts = TITLE_makeTitleStatusString( talkerindex , 3 );
            snprintf( message , sizeof(message),
                  "饒岆跺祥渣腔靡趼, 憩請%s勘ㄐ",
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
              "懂腔疑%s婓跤斕備瘍奀"
              "祥岆遜羶懂鎘ˋ" , n );
	    CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
    }
    else {
    	#define		NPC_OLDMAN_TITLE_UNIT1		"TSU"
    	#define		NPC_OLDMAN_TITLE_UNIT2		"KO"
    	
        snprintf( message, sizeof( message ) ,"懂腔疑%s",n );
    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		if( delcnt > 0 && addcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"斕犮囮賸%d%s 備瘍腔訧跡﹝"
	    			"祥岆腕善賸%d%s 腔備瘍訧跡鎘ˋ"
	    			, delcnt,
	    			delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2,
	    			addcnt,
	    			addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2
	    			);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
		else if( delcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"斕眒犮囮腕善%d%s備瘍腔訧跡﹝", delcnt,
	    			delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
		else if( addcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"跤斕%d%s腔備瘍勘ㄐ", addcnt, // CoolFish: d%s -> %d%s 2001/4/18
	    			addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
	    if( delcnt > 0 ) {
	        snprintf( message, sizeof( message), 
	                    "囮��%d%s 備瘍ㄐ", delcnt,
	    				delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	        CHAR_talkToCli( talkerindex, -1, message,  CHAR_COLORYELLOW);
	    }
	    if( addcnt > 0 ) {
	        snprintf( message, sizeof( message), 
	                    "鳳腕%d%s 備瘍ㄐ", addcnt,
	    				addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	        CHAR_talkToCli( talkerindex, -1, message,  CHAR_COLORYELLOW);
	    }
    }
}
    if( title_change ) CHAR_sendStatusString( talkerindex , "T" );
    
}
