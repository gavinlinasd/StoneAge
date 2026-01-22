#include "version.h"
#define _NPC_MSG_C_

#include "char.h"
#include "char_base.h"
#include "npcutil.h"


/*
  鶷菲黍埵娹  ��  鴢毀笰ぁ忒源�呁�
  敁堇堣窇囀汔摯  澺凝�梪瓜笥倡�堎源竣勗埰堎��

  init , looked 摯陑勗  伀

  */  
BOOL NPC_MsgInit( int meindex )
{
    //CHAR_setInt( meindex , CHAR_HP , 0 );

    //CHAR_setInt( meindex , CHAR_MP , 0 );
    //CHAR_setInt( meindex , CHAR_MAXMP , 0 );

    //CHAR_setInt( meindex , CHAR_STR , 0 );
    //CHAR_setInt( meindex , CHAR_TOUGH, 0 );

    //CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    //CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    //CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /*   賓踏躂埵笢源庌 */

    return TRUE;
}


/*
 * 峟�桫憶蛅蛪楮耒妢笭磩�
 */
void NPC_MsgLooked( int meindex , int lookedindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    
	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));
    CHAR_talkToCli( lookedindex , -1 , arg , CHAR_COLORWHITE );

}
