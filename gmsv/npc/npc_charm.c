#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_charm.h"

//    医毛  仁煌遥挚反

//等级＊        ＊  当前的    /          

//荚汊汹反｛ㄤ匹允［
/*
#define RATE  4		//伊□玄＂
#define CHARMHEAL 5 //    式者竄俘
#define WARU	3	//    除以的值

*/

#define RATE  10	//伊□玄＂
#define CHARMHEAL 5 //    式者竄俘
#define WARU	3	//    除以的值


static void NPC_Charm_selectWindow( int meindex, int toindex, int num);
int NPC_CharmCost(int meindex,int talker);
void NPC_CharmUp(int meindex,int talker);


/*********************************
* 赓渝质  
*********************************/
BOOL NPC_CharmInit( int meindex )
{
	/*--平乓仿及正奶皿毛涩烂--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPECHARM );
	return TRUE;

}


/*********************************
*   仄井仃日木凶凛及质  
*********************************/
void NPC_CharmTalked( int meindex , int talkerindex , char *szMes ,int color )
{

    /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    {
    	return;
    }
	
	/*--  及蟆卞中月井升丹井＂--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		/* ㄠ弘伉永玉动  及心 */
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	NPC_Charm_selectWindow( meindex, talkerindex, 0);
}


/*
 * 备质  卞坌仃月
 */
static void NPC_Charm_selectWindow( int meindex, int toindex, int num)
{

	char token[1024];
	char escapedname[1024];
	int fd = getfdFromCharaIndex( toindex);
	int buttontype = 0;
	int windowtype = 0;
	int windowno = 0;
	int cost = 0;
	int chartype;
	
	/*--窗口类型讯息较多所以先设置--*/
  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	switch( num) {
	  case 0:
  		/*--蓟      --*/
		sprintf(token,"4\n 　　　   = = 美容师 = = "
				     "\n\n 　欢迎光临，今天要做什麽呢？"
				  "\n "
				  "\n\n　　　　《　给我魅力　》 "
				  "\n\n　　　　《 什麽也不做 》 "
		);

	  	buttontype = WINDOW_BUTTONTYPE_NONE;
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
	  	windowno = CHAR_WINDOWTYPE_CHARM_START; 
	  	break;

	case 1:
		cost = NPC_CharmCost( meindex, toindex);
		if(cost == -1){
			sprintf(token,"　　　　     = = 美容师 = =  "
				      "\n\n　　　　你的魅力真是完美"
				      "\n\n　　　请永远保持你的魅力唷。"
			);
		  	buttontype = WINDOW_BUTTONTYPE_OK;
		}else{
			sprintf(token,"　　　　     = = 美容师 = =  "
				 	    "\n　佮齵在,千禧特惠价实施中�"
					  "\n\n 要将你的魅力上升五点的话"
					  "\n　　　　需要%6d的stone唷！"
						"\n\n　　　即使这样也可以吗？",cost
			);
		  	buttontype = WINDOW_BUTTONTYPE_YESNO;

		}
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno = CHAR_WINDOWTYPE_CHARM_END; 

		break;

	case 2:
		cost = NPC_CharmCost( meindex, toindex);
		chartype = CHAR_getInt( toindex, CHAR_IMAGETYPE);
		
		if(cost > CHAR_getInt( toindex, CHAR_GOLD)) {
			sprintf(token,"　　　　     = = 美容师 = =  "
					  "\n\n　　 哎呀!你的钱不够唷！"
				        "\n　　    先去存好钱下次再来吧！"
			);

		}else{
			NPC_CharmUp( meindex, toindex);

			/*--平乓仿及正奶皿卞方匀化丢永本□斥毛  尹化心凶--*/
			switch( chartype) {
			  case CHAR_IMAGETYPE_GIRL:
				sprintf(token,"　　　　     = = 美容师 = =  "
					  "\n\n 　嗯,这样你的魅力就上升罗！"
					  "\n\n　　　愈来愈可爱了呢！"
				);

				break;
			  case CHAR_IMAGETYPE_BOY:
				sprintf(token,"　　　　     = = 美容师 = =  "
					  "\n\n 　嗯,这样你的魅力就上升罗！"
					  "\n\n　　是不是比刚才要帅多了呢？"
				);
			  	break;
			  	
			  case CHAR_IMAGETYPE_CHILDBOY:
			  case CHAR_IMAGETYPE_CHILDGIRL:
				sprintf(token,"　　　　     = = 美容师 = =  "
					  "\n\n 　嗯,这样你的魅力就上升罗！"
					  "\n\n　　  真的变得好可爱唷！"
				);
			 	break;
			 	
			   case CHAR_IMAGETYPE_MAN:
				sprintf(token,"　　　　     = = 美容师 = =  "
					  "\n\n 　嗯,这样你的魅力就上升罗！"
					  "\n\n 　　　变得更酷了呢！"
				);
			 	break;
			 	
			   case CHAR_IMAGETYPE_WOMAN:
				sprintf(token,"　　　　     = = 美容师 = =  "
				  "\n\n 　嗯,这样你的魅力就上升罗！"
				  "\n\n    可真是变得愈来愈美了呢！"
				);
			 	break;
			 
			 }
		}

		buttontype = WINDOW_BUTTONTYPE_OK;
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno = CHAR_WINDOWTYPE_CHARM_END; 
		break;
	}
	
	makeEscapeString( token, escapedname, sizeof( escapedname));
	/*-仇仇匹霜耨允月--*/
	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					escapedname);

}


/*-----------------------------------------
弁仿奶失件玄井日忒匀化五凶凛卞裟太请今木月［
-------------------------------------------*/
void NPC_CharmWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) return;

	switch( seqno){
	  case CHAR_WINDOWTYPE_CHARM_START:
	  	if(atoi( data) == 2) {
			NPC_Charm_selectWindow( meindex, talkerindex, 1 );
		}
		break;

	  case CHAR_WINDOWTYPE_CHARM_END:
	  	if(select == WINDOW_BUTTONTYPE_YES) {
			NPC_Charm_selectWindow( meindex, talkerindex, 2 );
		}
		break;
	}
	
}



/*--        --*/
void NPC_CharmUp(int meindex,int talker)
{
	int cost;
	int i;
	int petindex;
	char petsend[64];	

	/*--减少金钱吧--*/
	cost = NPC_CharmCost( meindex, talker);
	CHAR_setInt( talker, CHAR_GOLD,
			CHAR_getInt( talker, CHAR_GOLD) - cost);
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);

	/*--    超过１００时强制设为１００--*/
	if(CHAR_getInt( talker, CHAR_CHARM) + CHARMHEAL >= 100) {
		CHAR_setInt( talker, CHAR_CHARM, 100);
	}else{
		/*--    设置--*/
		CHAR_setInt(talker, CHAR_CHARM,
	 			(CHAR_getInt( talker, CHAR_CHARM) + CHARMHEAL));
	}
	
	/*--旦  □正旦及凳蕙--*/
	CHAR_complianceParameter( talker );
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);


	/*--更新宠物的参数--*/
	for( i = 0 ; i < CHAR_MAXPETHAVE ; i++){
    	petindex = CHAR_getCharPet( talker, i);

		if( petindex == -1  )  continue;

	   /*  平乓仿及    民尼永弁    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;

		/*--由仿丢□正譬帮--*/
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );
	}
}


/*--堝狹式四劭--*/
int NPC_CharmCost(int meindex,int talker)
{
	int cost;
	int level;
	int charm;
	int trans;

	level = CHAR_getInt( talker, CHAR_LV);
	charm = CHAR_getInt( talker, CHAR_CHARM);
	trans = CHAR_getInt( talker, CHAR_TRANSMIGRATION);

	if(charm >= 100) return -1;
	
	if(charm <= 1) charm = WARU;
	
	/*-- 四劭嵜 --*/
	cost = level * RATE * (charm / WARU) * (trans+1);

	return cost;

}
