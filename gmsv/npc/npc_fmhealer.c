#include "version.h"
/*----------  Shan (BEGIN) ----------*/
/*---------  AD 2001/05/28  ---------*/
// 
//            模逜瓟谿 
// 
/*----------   Shan(END)   ----------*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "npcutil.h"
#include "item.h"
#include "readmap.h"
  
#define RANGE 2
void NPC_FmHealerSpeak( int index, int talker);
void NPC_FmHealerAllHeal( int talker );
BOOL NPC_FmMoneyCheck(int meindex,int talker);
int  NPC_FmCostCheck(int talker);
void NPC_FmCharCheckPoint(int meindex,int talker);
int  NPC_FmWorkInput(int meindex,int talker);

BOOL NPC_FmHealerInit( int meindex )
{
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEHEALER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    return TRUE;
}

void NPC_FmHealerTalked( int meindex , int talker , char *msg ,int color )
{
    char* npcarg;
    char  token[32];
    int   msgNo;

    npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

    getStringFromIndexWithDelim( npcarg, "|", 1, token,sizeof( token));
    msgNo  = atoi( token );

    if( CHAR_getInt( talker , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )	
        return;

    if( NPC_Util_isFaceToFace( talker, meindex , 2 ) == FALSE ) 
    {
    	if( NPC_Util_isFaceToFace( talker, meindex , 1 ) == FALSE ) 
        	return;
    }
     
    //if( CHAR_getInt(meindex, CHAR_FLOOR) != CHAR_getWorkInt(talker, CHAR_WORKFMFLOOR)){
        //print("\n npc floor->%d player floor->%d",CHAR_getInt(meindex, CHAR_FLOOR),
        //       CHAR_getWorkInt(talker, CHAR_WORKFMFLOOR));
    //    CHAR_talkToCli(talker, meindex,
    //             "惕Кㄐ斕祥岆掛模逜�刱悵疥瑀辰巀蒚婺插�",CHAR_COLORWHITE);
    //    return;
    //}
     
    if( (CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) == 0)
	|| (CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) == 2)){
	NPC_FmHealerAllHeal( talker);
	if(msgNo == 1) {
	    CHAR_talkToCli( talker, meindex,
		    "眒冪�垓蕃婺插�③苤陑赻撩腔旯极遄ㄐ",CHAR_COLORWHITE);
	}else if(msgNo == 2){
	    CHAR_talkToCli( talker, meindex,
	       	   "蚕黺斕岆掛模逜�刱悵甭躽襞珧巀蒚婺敦氿�",CHAR_COLORWHITE);
	}
    }else{
	int i = 0;
	int otherindex;
	for( i=0 ; i < CHAR_PARTYMAX ; i++){
	    otherindex = CHAR_getWorkInt( talker, CHAR_WORKPARTYINDEX1 + i);
   	    if(otherindex != -1){
	    	NPC_FmHealerAllHeal( otherindex);
		if(msgNo == 1){
                    CHAR_talkToCli( otherindex, meindex,
		       	"眒冪�垓蕃婺插�③苤陑赻撩腔旯极遄ㄐ",CHAR_COLORWHITE);
		}else if(msgNo == 2){
		    CHAR_talkToCli( otherindex, meindex, 
			"蚕黺斕岆掛模逜�刱悵甭躽襞珧巀蒚婺敦氿�",CHAR_COLORWHITE);
		}
            }
        }
    }
}

void NPC_FmHealerAllHeal( int talker )
{
    int i;
    int petindex;
    char petsend[3];	
    char msgbuf[5];
	
    CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
    CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );

    for(i = 0 ; i < CHAR_MAXPETHAVE; i ++){
	petindex = CHAR_getCharPet( talker, i);
	if( petindex == -1  )  continue;

	if( !CHAR_CHECKINDEX( talker ) )  continue;
	if( CHAR_CHECKINDEX( petindex) == FALSE ) continue;
	CHAR_setFlg( petindex, CHAR_ISDIE, 0);
	CHAR_setInt( petindex , CHAR_HP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) );
	CHAR_setInt( petindex , CHAR_MP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXMP ) );
	CHAR_complianceParameter( petindex );
	sprintf( petsend, "K%d", i );
	CHAR_sendStatusString( talker , petsend );
    }
	
    if(CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ){
	int topartyarray = -1;
	int oyaindex = CHAR_getWorkInt( talker, CHAR_WORKPARTYINDEX1);
	if( CHAR_CHECKINDEX( oyaindex )){
	    int	i;
	    for( i = 0; i < CHAR_PARTYMAX; i ++ ){
		int workindex = CHAR_getWorkInt( oyaindex, CHAR_WORKPARTYINDEX1 + i);
		if( CHAR_CHECKINDEX( workindex) ){
		    if( workindex == talker ){
			topartyarray = i;
			break;
		    }
		}
	    }
	    for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
	        int otherindex = CHAR_getPartyIndex( talker, i);
		if( CHAR_CHECKINDEX( otherindex) ){
		    snprintf( msgbuf, sizeof( msgbuf), "N%d", topartyarray);
		    if( otherindex != talker){
		        CHAR_sendStatusString( otherindex, msgbuf);
		    }
		}
	    }
	}
    }
    CHAR_send_P_StatusString( talker, CHAR_P_STRING_HP);
    CHAR_send_P_StatusString( talker, CHAR_P_STRING_MP);
}

