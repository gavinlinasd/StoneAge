#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"

#ifdef _BLACK_MARKET
extern struct  BlackMarketItem BMItem[BMIMAX];
extern int     BMINum;
extern int     BMSellList[12];

void NPC_BlackMarketTalked( int meindex, int talker, char *msg, int color)
{		
	char sBuf[512]="", tBuf[512]="";
	if(!CHAR_CHECKINDEX( talker)) return;	
    if(getfdFromCharaIndex( talker)==-1) return;
	if(CHAR_getInt( talker, CHAR_WHICHTYPE)!=CHAR_TYPEPLAYER) return;    
    if(NPC_Util_isFaceToFace( talker, meindex, 1)!=TRUE) return;
		
	sprintf( tBuf, "嘿嘿．．．．\n在我这边有各式各样的武器、防具及道具，你想要跟我交换吗？换好了可就不能反悔喔～");

	lssproto_WN_send(getfdFromCharaIndex( talker), 
	                 WINDOW_MESSAGETYPE_MESSAGE,
        	         WINDOW_BUTTONTYPE_OK,
                     CHAR_WINDOWTYPE_BLACKMARKET,
 		             CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
		             makeEscapeString( tBuf, sBuf, sizeof(sBuf)));   	
	return;
}
BOOL NPC_BlackMarketInit( int meindex)
{
	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEWINDOWMAN);
	CHAR_setFlg( meindex, CHAR_ISATTACKED , 0);
	return TRUE;
}
void NPC_BlackMarketWindowTalked( int index, int talker, int seqno, int select, char *data)
{
	int  i;
	char sBuf[1024]="", sBuf1[256]="";
    
    if(!CHAR_CHECKINDEX( talker)) return;	
    if(getfdFromCharaIndex( talker)==-1) return;
	if(CHAR_getInt( talker, CHAR_WHICHTYPE)!=CHAR_TYPEPLAYER) return;    
    if(NPC_Util_isFaceToFace( talker, index, 1)!=TRUE) return;
        
    if( seqno == CHAR_WINDOWTYPE_BLACKMARKET){
		for(i=0; i<12; i++){
		    CONNECT_set_BMList( getfdFromCharaIndex( talker), i, BMSellList[i]);		    
		    sprintf( sBuf1, "%d|%d|%s\n", BMSellList[i], BMItem[BMSellList[i]].iGraphicsNum, BMItem[BMSellList[i]].iName); 
		    strcat( sBuf, sBuf1);
		}

	    lssproto_WN_send( getfdFromCharaIndex( talker), 
		                  WINDOW_MESSAGETYPE_BLACKMARKET, 
		                  WINDOW_BUTTONTYPE_NONE,
				    	  CHAR_WINDOWTYPE_BLACKMARKET,
				          CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
					      sBuf );
	}
}
#endif




