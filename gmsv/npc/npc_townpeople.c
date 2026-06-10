#include "version.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "handletime.h"
#include "anim_tbl.h"
#include "npc_door.h"
#include "lssproto_serv.h"
#include "npcutil.h"


/*
 *  引切及夫午太午  by nakamura
 *  正奶皿  “TownPeople
 *    仄井仃日木凶午五卞NPCARGUMENT及  侬  毛  仄井仃化五凶谛卞  允［
 *
 *  引凶｝npcarg卞 仇氏卞切反,漆  反化氏五互中中匹允友
 *  及方丹卞  剩戊件穴毛犯伉立正午仄化  醒及丢□本□斥毛
 *  踏仁仇午互匹五｝公及桦宁仿件母丞匹公及  井日仄扎屯月［
 *  npcgen.perl 匹反｝ MAN匹丐月［ MSG午幻午氏升云卅元分卅丐
 *
 */

/*
 *   仄井仃日木凶午五反NPCARGUMENT毛公及引引  允［
 */
void NPC_TownPeopleTalked( int index, int talker, char *msg, int color )
{
	char arg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];
    int i, tokennum;

    /* 3弘伉永玉动  及桦宁分仃忒蚕允月 */

	if( CHAR_getInt(talker,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER 
        && NPC_Util_charIsInFrontOfChar( talker, index, 3 ) ){

        NPC_Util_GetArgStr( index, arg, sizeof( arg));

        tokennum = 1;
        /* 戊件穴匹嗉濠日木凶玄□弁件互窒仇丐月井醒尹月 */
        for( i=0;arg[i]!='\0';i++ ){
            if( arg[i] == ',' ) tokennum++;
        }

        /* 仿件母丞匹升木毛韵月井瑁户化｝公及玄□弁件毛潸曰分允 */
        getStringFromIndexWithDelim( arg,",",
                                     rand()%tokennum+1,token, sizeof(token));

        CHAR_talkToCli( talker, index, token, CHAR_COLORWHITE );
    }
}

/*
 * 赓渝祭允月［
 */
BOOL NPC_TownPeopleInit( int meindex )
{

    //CHAR_setInt( meindex , CHAR_HP , 0 );
    //CHAR_setInt( meindex , CHAR_MP , 0 );
    //CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    //CHAR_setInt( meindex , CHAR_STR , 0 );
    //CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    //CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    //CHAR_setFlg( meindex , CHAR_ISOVERED , 1 );
    //CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   猾今木卅中方氏 */
    
    return TRUE;
}
