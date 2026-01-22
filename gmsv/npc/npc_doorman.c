#include "version.h"
#include <string.h>
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "item.h"
#include "util.h"
#include "handletime.h"
#include "npc_doorman.h"
#include "npc_door.h"
#include "npcutil.h"
#include "configfile.h"
/*
 *
 *    堇勗迶囮誑堣堎敁拻勗鼠摯迶囮禱笰�桴挺匙紨鰡撌諂�趙楣�艿PC.
 *  迶囮摯敁埵堇勗create埰堎煦崹ぁ��鼠摯迶囮禱堯軘踏鞠堎喫敁誑ぁ拻堎��
 *  毀埵媃凝崹�桫憶谻覢撅殣暲褐珝藏姜像鵗し砠�竘渦堇8悃筒勗迶囮誑笢堎
 *  鳹譴毀鼠摯埰迋趙勗葡媃趙齬抈埰堎�蛑豝袡智商鯓宏�  鄹勗楣�妘蠁蝐槾耒瞿�
 *
 *  騷璃淏↓啞攝騷筒毀Talkぁ
 *
 * 迶囮禱楣�庈袘妤�
 *
 * 1 堁鉣禱高靚埰堎�菾芾副用槱觕梫藭桱�     gold|100
 * 2 囮騷  堜禱1蠀高靚埰堎 �菾芾副用槱觕梫藭桱�  item|45
 * 3 囮騷  堜禱  埱趙笢堎凝汔竣凝ぅ迋堎��   埱趙笢倜�梫務呁菈temhave|44
 * 4 囮騷  堜禱  埱趙笢埵笢凝汔竣凝ぅ迋堎��  埱趙笢埵凝埱倜�梫務呁�
 *          itemnothave|333
 * 5 措蠕禱忒埱趙笢堎凝汔竣凝ぅ迋堎��  埱趙笢倜�梫務呁� titlehave|string
 * 6 措蠕禱忒埱趙笢埵笢凝汔竣凝ぅ迋堎��  埱趙笢埵凝埱倜�梫務呁�
 *      titlenothave|string
 *
 * 凝埵�桫笻�  勗紮窇堎敁楣�呁詈髒啗踽�毀��
 *ˇ100湖↓鳴迶笢倜煦拻竘埰誑笢笢ぁ埰凝ㄑˊぁˇ毀笢ˊ敁笢竣敁100湖↓鳴迶
 * 敁�桫戰瞿詎倛槾耒誨噸棉苺蔆笭縡尕�趙忒敁�桫戰瞿袸奴�ˇ100湖↓鳴迶
 * 笢倜煦拻竘媃倜�菇諄褉伝囃戰瞿�
 *
 * 囮騷  堜高靚摯鳹譴毀��ˇ笰∴禱郖蠀笢倜煦拻竘埰誑笢笢ぁ埰凝ㄑˊ敁拻�呁�
 *  3凝��6摯鳹譴毀��笰凝毀埵媃凝崹趙邳贀誑鼠Й埱趙倜�梫務呁�
 *
 *
 *
 *    筒哱摯  傮
 *
 *1  迶囮禱趙拻敁竣勗  ��
 *2  喫摯NPC禱媏騍勗迶囮摯敁埵堇勗  �呁衕羽捙� gold|100 勗埰堎
 *3  喫摯NPC勗葡媃趙��100湖↓鳴迶雄窀忒埱趙笢堎Щ稃ぁˇ毀笢ˊ敁虭竣
 *4  迶囮誑痲�欶郅能驍申樊�倜�梊�  ��
 *
 */

static void NPC_DoormanOpenDoor( char *nm  );

BOOL NPC_DoormanInit( int meindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    char dname[1024];

	/* 騷穫璃哱摯淏騷鏤优擭 */
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE,CHAR_EVENT_NPC);

    CHAR_setInt( meindex , CHAR_HP , 0 );
    CHAR_setInt( meindex , CHAR_MP , 0 );
    CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    CHAR_setInt( meindex , CHAR_STR , 0 );
    CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   賓踏躂埵笢源庌 */

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if(!getStringFromIndexWithDelim( arg, "|", 3, dname, sizeof(dname ))){
        print("RINGO: 扢隅艘藷氪奀剒猁藷腔靡趼遄ㄐ:%s:\n",
              arg );
        return FALSE;
    }
    print( "RINGO: Doorman create: arg: %s dname: %s\n",arg,dname);
    CHAR_setWorkChar( meindex , CHAR_WORKDOORMANDOORNAME , dname );

    return TRUE;
}

void NPC_DoormanTalked( int meindex , int talkerindex , char *msg ,
                     int color )
{
    char mode[128];
    char opt[256];
    char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];

    /* 鏤畛騷媕↓誑迶囮悃璃摯1精惉蚗迶雄  埵�梫棵牳做� */
    if(NPC_Util_CharDistance( talkerindex, meindex ) > 1)return;

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if( !getStringFromIndexWithDelim( arg, "|", 1, mode, sizeof( mode )))
        return;

    if( !getStringFromIndexWithDelim( arg, "|", 2, opt, sizeof( opt ) ))
        return;

    if( strcmp( mode , "gold" ) == 0 ){
        int g = atoi( opt );
        int yn = NPC_Util_YN( msg );
        /*char *nm = CHAR_getChar( meindex , CHAR_NAME );*/
        char msg[256];

        if( g > 0 && yn < 0 ){
            snprintf( msg ,sizeof( msg ) ,
                      "湖羲藷剒猁跤扂%d腔踢赽涴欴褫眕鎘ˋ", g );
            CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
        } else if( g > 0 && yn == 0 ){
            snprintf( msg , sizeof( msg ),
                      "湖羲藷 %d腔踢赽岆斛猁腔﹝", g );
        } else if( g > 0 && yn == 1 ){
            int now_g = CHAR_getInt( talkerindex, CHAR_GOLD );
            if( now_g < g ){
                snprintf( msg , sizeof( msg ) ,
                          "湖羲藷 %d腔踢赽岆斛猁腔﹝", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
            } else {
                snprintf( msg , sizeof( msg ),
                          "%d 彶善踢赽賸﹝珋婓憩懂羲藷﹝", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );

                /* 堁鉣禱斛蚗哱 */
                now_g -= g;
                CHAR_setInt( talkerindex , CHAR_GOLD , now_g );
                /* 堣倜�欷ひ迮�  ↓淏筒禱邞嚭 */
                CHAR_send_P_StatusString(talkerindex, CHAR_P_STRING_GOLD);

                /* 迶囮痲�桱� */
                NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
            }
        }
    } else if( strcmp( mode , "item" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "奾婓帤盓堔耀宒﹝",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemhave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "奾婓帤盓堔耀宒﹝",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemnothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "奾婓帤盓堔耀宒﹝",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "titlehave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "奾婓帤盓堔耀宒﹝",
                        CHAR_COLORWHITE);

    } else if( strcmp( mode , "roomlimit" ) == 0 ){

		/*   碟摯硞倳�羬袘目夭鎔踽� */
		char szOk[256], szNg[256], szBuf[32];
		int checkfloor;
		int maxnum, i, iNum;

	    if( !getStringFromIndexWithDelim( arg, "|", 2, szBuf, sizeof( szBuf ) ))
    	    return;

		/* ぅ迋堎啞痲囮敁    硞倳 */
		if( sscanf( szBuf, "%d:%d", &checkfloor, &maxnum ) != 2 ){
			return;
		}

		for( iNum = 0,i = 0; i < getFdnum(); i ++ ){
			/* 鏤畛騷媕↓雄翻勗毀煻  誑  笢 */
			if( CHAR_getCharUse( i ) == FALSE )continue;
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			/* 炩擭摯啞痲囮雄翻勗煻  誑  笢 */
			if( CHAR_getInt( i, CHAR_FLOOR ) != checkfloor )continue;
			iNum++;
		}
	    if( !getStringFromIndexWithDelim( arg, "|", 5, szNg, sizeof( szNg ))){
   			strcpy( szNg, "﹝﹝﹝﹝" );	/* 瘀氶埵媃摯掛惉啞 */
		}
    	if( !getStringFromIndexWithDelim( arg, "|", 4, szOk, sizeof( szOk ))){
   			strcpy( szOk, "羲藷勘﹝﹝﹝" );	/* 瘀氶堣堇摯掛惉啞 */
   		}

		if( iNum >= maxnum ){
			/*     禱祒窇趙笢堎鳹譴 */
	        CHAR_talkToCli( talkerindex, meindex ,szNg, CHAR_COLORWHITE);
		}else{
			/*     勗  倜埵笢鳹譴 */
	        CHAR_talkToCli( talkerindex, meindex ,szOk, CHAR_COLORWHITE);
            NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
		}

    } else if( strcmp( mode , "titlenothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "奾婓帤盓堔耀宒﹝",
                        CHAR_COLORWHITE);
    }
}

/*
 *    鞳ぁ號喙媃趙樅蚗哱媃倜摯禱埰迋趙楣�呁�
 *
 */
static void NPC_DoormanOpenDoor( char *nm)
{
    int doori = NPC_DoorSearchByName( nm );
    print( "RINGO: Doorman's Door: index: %d\n", doori );

    NPC_DoorOpen( doori , -1 );

}

