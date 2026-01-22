#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_action.h"

/* 
 * 鏤畛騷媕↓摯囮袲で砫璃勗  伀埰堎NPC��
 * ワ勗婘堇蒍媃倜堇埰堎煦崹煦誑��
 * 倵�呁椹�倜毀  埱趙笢堎摯囮袲で砫璃勗毀  伀媃埵笢��
 *
 * 礎倳※
 *      msgcol:		隍蚗掛↓喇摯褗�蛪萼觙伒懩�毀棺褗
 *      normal:		  玊勗婘埱趙拻倜鄹盓  嗚埵囮袲で砫璃勗葡媃趙摯蒍紮
 *		attack:		  賓囮袲で砫璃勗葡埰堎蒍紮
 *		damage:		譫隍↓喇禱偏崹倜囮袲で砫璃勗葡埰堎蒍紮
 *		down:		槸躂堎囮袲で砫璃勗葡埰堎蒍紮
 *		sit:		淢堎囮袲で砫璃勗葡媃趙摯蒍紮
 *		hand:		鱗禱�擸薹抄芢刳鉏�勗葡媃趙摯蒍紮
 *		pleasure:	熬屾囮袲で砫璃勗葡媃趙摯蒍紮
 *		angry:		曹堎囮袲で砫璃勗葡媃趙摯蒍紮
 *		sad:		  媃資囮袲で砫璃勗葡媃趙摯蒍紮
 *		guard:		票↓迶埰堎囮袲で砫璃勗葡媃趙摯蒍紮
 */
 

#define		NPC_ACTION_MSGCOLOR_DEFAULT		CHAR_COLORYELLOW

enum {
	CHAR_WORK_MSGCOLOR	= CHAR_NPCWORKINT1,
};

/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_ActionInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int		tmp;
	
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "msgcol");
	if( tmp == -1 ) tmp = NPC_ACTION_MSGCOLOR_DEFAULT;
	CHAR_setWorkInt( meindex, CHAR_WORK_MSGCOLOR, tmp);
    
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEACTION );
	
	
    return TRUE;
}




/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_ActionTalked( int meindex , int talkerindex , char *szMes ,
                     int color )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];

    /* 鏤畛騷媕↓勗葡媃趙煦崹  伀埰堎 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	/* 兒精惉蚗迶雄  摯陑 */
	if( !NPC_Util_charIsInFrontOfChar( talkerindex, meindex, 1 )) return; 

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if( NPC_Util_GetStrFromStrWithDelim( argstr, 
										"normal",
										buf, sizeof( buf))
    	!= NULL )
	{
    	CHAR_talkToCli( talkerindex, meindex ,buf , 
    					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR ));
	}
	
}
/*********************************
* watch窐  
*********************************/
void NPC_ActionWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen )
{
	int		meindex;
	int		index;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];
	int		i;
    struct  {
    	CHAR_ACTION		act;
    	char			*string;
    }searchtbl[] = {
	    { CHAR_ACTATTACK,	"attack"},
	    { CHAR_ACTDAMAGE,	"damage"},
		{ CHAR_ACTDOWN,		"down"},
		{ CHAR_ACTSIT,		"sit"},
		{ CHAR_ACTHAND,		"hand"},
		{ CHAR_ACTPLEASURE,	"pleasure"},
		{ CHAR_ACTANGRY,	"angry"},
		{ CHAR_ACTSAD,		"sad"},
		{ CHAR_ACTGUARD,	"guard"},
		{ CHAR_ACTNOD,		"nod"},
		{ CHAR_ACTTHROW,	"throw"},
    };
	
	if( OBJECT_getType( objindex) != OBJTYPE_CHARA) return;
	index = OBJECT_getIndex( objindex);
	/* 鏤畛騷媕↓勗摯陑  伀埰堎 */
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;
    
    meindex = OBJECT_getIndex( meobjindex);
    
    /* 澺拻譴埱趙兒精惉蚗迶ぁ埵笢敁  伀媃埵笢 */
    if( NPC_Util_isFaceToFace( meindex, index, 1 ) != TRUE ) return;
	
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	
	for( i = 0; i < arraysizeof( searchtbl); i ++ ) {
		if( searchtbl[i].act == act) {
			if( NPC_Util_GetStrFromStrWithDelim( argstr, 
												searchtbl[i].string,
												buf, sizeof( buf))
		    	!= NULL )
			{
		    	CHAR_talkToCli( index, meindex ,buf , 
		    					CHAR_getWorkInt( meindex, CHAR_WORK_MSGCOLOR ));
				break;
			}
		}
	}
}
