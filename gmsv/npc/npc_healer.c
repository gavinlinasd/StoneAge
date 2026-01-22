#include "version.h"
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
void NPC_HealerSpeak( int index, int talker);
void NPC_HealerAllHeal( int talker );
BOOL NPC_MoneyCheck(int meindex,int talker);
int NPC_CostCheck(int talker);
void NPC_CharCheckPoint(int meindex,int talker);
int NPC_WorkInput(int meindex,int talker);


/**********************************
疐趵撬
************************************/
BOOL NPC_HealerInit( int meindex )
{
	//淏騷鏤禱樅↓溘↓勗优擭
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEHEALER );
    //  賓ぁ拻埵笢    菺埵笢凝忒  
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    //窀勗篥躂埵笢
//    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );

	return TRUE;

}




/*-------------------------------------------
 *
 *   踏躂��HP,MP禱蠈楣勗埰堎
 *
 --------------------------------------------*/
void NPC_HealerTalked( int meindex , int talker , char *msg ,int color )
{

    char*   npcarg;
    char    token[32];
	int msgNo;


	npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

    getStringFromIndexWithDelim( npcarg, "|", 1, token,sizeof( token));
    msgNo  = atoi( token );

    /*---鏤畛騷媕↓勗葡媃趙煦崹  伀埰堎---*/
    if( CHAR_getInt( talker , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )	return;

	/*---  ⑹雄  凝ㄑ  ⑹毀1竘倜毀��媃庌ぁ笢堎凝ㄑ---*/
	/* 角色動畫重置 */
	if( NPC_Util_CharDistance( talker, meindex ) > 2) return;

	if( (CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) == 0)
	|| (CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) == 2) )
	{
		/*--樊蜾踏鞠竘媃踝竣--*/
		NPC_HealerAllHeal( talker);
	    if(msgNo == 1) {
		    CHAR_talkToCli( talker, meindex,
			    "眒冪�垓蕃婺插�③婓狟棒腔掀��笢樓蚐遄ㄐ",CHAR_COLORWHITE);

		}else if(msgNo == 2) {
		    CHAR_talkToCli( talker, meindex,
		    	"蚕黺斕竭剴妗ㄛ�襞珧巀蒚婺敦氿�",CHAR_COLORWHITE);
		}

	}else{
		int i = 0;
		int otherindex;

		for( i=0 ; i < CHAR_PARTYMAX ; i++)
		{
			otherindex = CHAR_getWorkInt( talker, CHAR_WORKPARTYINDEX1 + i);
			if(otherindex != -1){

				NPC_HealerAllHeal( otherindex);
				if(msgNo == 1) {
				    CHAR_talkToCli( otherindex, meindex,
				    	"眒冪�垓蕃婺插�③婓狟棒腔掀��笢樓蚐遄ㄐ",CHAR_COLORWHITE);

				}else if(msgNo == 2) {
					CHAR_talkToCli( otherindex, meindex, 
						"蚕黺斕竭剴妗ㄛ�襞珧巀蒚婺敦氿�",CHAR_COLORWHITE);
				}
			}
		}
	}
}


/*----------------------*/
/* 樊蜾 */
/*-----------------------*/
void NPC_HealerAllHeal( int talker )
{
	int i;
	int petindex;
	char petsend[3];	
	char msgbuf[5];
	
    CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
    CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );



	for(i = 0 ; i < CHAR_MAXPETHAVE; i ++) {
		petindex = CHAR_getCharPet( talker, i);

		if( petindex == -1  )  continue;

		/*  す籤溘摯    鏍攝蚗袲    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;

		/* 妐蚗哱摯index鏍攝蚗袲禱埰堎 */
		if( CHAR_CHECKINDEX( petindex) == FALSE ) continue;

		/*--樊蜾--*/
		CHAR_setFlg( petindex, CHAR_ISDIE, 0);
		CHAR_setInt( petindex , CHAR_HP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) );
		CHAR_setInt( petindex , CHAR_MP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXMP ) );

		/*--蚕溘隍↓淏ぅ堆--*/
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );
	}
	
	/*---黥蹬誑笢躂�朼棺瑄樀笵近�--*/
	if(CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE )
	{
		int	topartyarray = -1;
		int	oyaindex = CHAR_getWorkInt( talker	, CHAR_WORKPARTYINDEX1);
	
		if( CHAR_CHECKINDEX( oyaindex )) {
			int	i;
	
			/* 猷覕敁佫媃倜盓僑摯黥蹬摯    摯鳹鐎禱噁   */
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int workindex = CHAR_getWorkInt( oyaindex, CHAR_WORKPARTYINDEX1 + i);
				if( CHAR_CHECKINDEX( workindex) ) {
					if( workindex == talker ) {
						topartyarray = i;
						break;
					}
				}
			}
			
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int otherindex = CHAR_getPartyIndex( talker, i);
				/* 黥蹬蚕溘隍↓淏禱邞堎 */
				if( CHAR_CHECKINDEX( otherindex) ) {
					snprintf( msgbuf, sizeof( msgbuf), "N%d", topartyarray);
					if( otherindex != talker) {
						CHAR_sendStatusString( otherindex, msgbuf);
					}
				}
			}
		}
	}

	/*--蚕溘隍↓淏邞堇--*/
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_HP);
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_MP);

}

