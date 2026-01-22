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
 *  竘з摯痲敁怮敁  by nakamura
 *  淏騷鏤  ※TownPeople
 *    媃凝崹�桫憶谻覢撅墣PCARGUMENT摯  棬  禱  媃凝崹趙拻倜硞勗  埰��
 *
 *  竘倜��npcarg勗 喫庌勗з毀,ぽ  毀趙庌拻誑笢笢ぁ埰衭
 *  摯源竣勗  呁昡璃悃禱溢惉蕾淏敁媃趙  倳摯隍↓掛↓喇禱
 *  怳�妘蠁蝏氾用憯�鼠摯鳹譴溘璃譫堜ぁ鼠摯  凝�欷ば�迋堎��
 *  npcgen.perl ぁ毀�� MANぁ堣堎�� MSG敁酵敁庌汔堁埵啋煦埵堣
 *
 */

/*
 *   媃凝崹�桫憶谻覢撌就PCARGUMENT禱鼠摯竘竘  埰��
 */
void NPC_TownPeopleTalked( int index, int talker, char *msg, int color )
{
	char arg[NPC_UTIL_GETARGSTR_BUFSIZE], token[NPC_UTIL_GETARGSTR_LINEMAX];
    int i, tokennum;

    /* 3精惉蚗迶雄  摯鳹譴煦崹蒍紮埰堎 */

	if( CHAR_getInt(talker,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER 
        && NPC_Util_charIsInFrontOfChar( talker, index, 3 ) ){

        NPC_Util_GetArgStr( index, arg, sizeof( arg));

        tokennum = 1;
        /* 昡璃悃ぁ鉏憍�桫憶豝�↓袲璃誑笰喫堣堎凝倳窇堎 */
        for( i=0;arg[i]!='\0';i++ ){
            if( arg[i] == ',' ) tokennum++;
        }

        /* 溘璃譫堜ぁ汔躂禱婘堎凝鋆誧趙��鼠摯哱↓袲璃禱噁堇煦埰 */
        getStringFromIndexWithDelim( arg,",",
                                     rand()%tokennum+1,token, sizeof(token));

        CHAR_talkToCli( talker, index, token, CHAR_COLORWHITE );
    }
}

/*
 * 疐趵撬埰堎��
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
    //CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   賓踏躂埵笢源庌 */
    
    return TRUE;
}
