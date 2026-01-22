#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_charm.h"

//    瓟禱  �妡迗�祪毀

//畛穫鳴↘        ↘  藯凜摯    /          

//樊蜾倵毀��其ぁ埰��
/*
#define RATE  4		//畛↓哱ㄑ
#define CHARMHEAL 5 //    摯樊蜾倵
#define WARU	3	//    禱鄎堎偯

*/

#define RATE  10	//畛↓哱ㄑ
#define CHARMHEAL 5 //    摯樊蜾倵
#define WARU	3	//    禱鄎堎偯


static void NPC_Charm_selectWindow( int meindex, int toindex, int num);
int NPC_CharmCost(int meindex,int talker);
void NPC_CharmUp(int meindex,int talker);


/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_CharmInit( int meindex )
{
	/*--す籤溘摯淏騷鏤禱优擭--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPECHARM );
	return TRUE;

}


/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_CharmTalked( int meindex , int talkerindex , char *szMes ,int color )
{

    /* 設定相關參數處理 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    {
    	return;
    }
	
	/*--  摯鞳勗笢堎凝汔竣凝ㄑ--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		/* 角色動畫重置 */
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	NPC_Charm_selectWindow( meindex, talkerindex, 0);
}


/*
 * 掘窐  勗覕崹堎
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
	
	/*--坳騷璃迶坳淏騷鏤隍蚗掛↓喇誑堁堁笢摯ぁ袸勗优擭--*/
  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	switch( num) {
	  case 0:
  		/*--撒      --*/
		sprintf(token,"4\n ﹛﹛﹛   = = 藝�楟� = = "
				     "\n\n ﹛辣茩嫖還ㄛ踏毞猁酕妦欒儸ˋ"
				  "\n "
				  "\n\n﹛﹛﹛﹛▲﹛跤扂黰薯﹛◎ "
				  "\n\n﹛﹛﹛﹛▲ 妦欒珩祥酕 ◎ "
		);

	  	buttontype = WINDOW_BUTTONTYPE_NONE;
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
	  	windowno = CHAR_WINDOWTYPE_CHARM_START; 
	  	break;

	case 1:
		cost = NPC_CharmCost( meindex, toindex);
		if(cost == -1){
			sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
				      "\n\n﹛﹛﹛﹛斕腔黰薯淩岆俇藝"
				      "\n\n﹛﹛﹛③蚗堈悵厥斕腔黰薯遄﹝"
			);
		  	buttontype = WINDOW_BUTTONTYPE_OK;
		}else{
			sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
				 	    "\n﹛�僣{婓,ロ檞杻需歎妗囥笢�1�7"
					  "\n\n 猁蔚斕腔黰薯奻汔拻萸腔趕"
					  "\n﹛﹛﹛﹛剒猁%6d腔stone遄ㄐ"
						"\n\n﹛﹛﹛撈妏涴欴珩褫眕鎘ˋ",cost
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
			sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
					  "\n\n﹛﹛ 陞挼!斕腔ヴ祥劂遄ㄐ"
				        "\n﹛﹛    珂�打皞蠅晅織拏梴敦氿�"
			);

		}else{
			NPC_CharmUp( meindex, toindex);

			/*--す籤溘摯淏騷鏤勗源埱趙隍蚗掛↓喇禱  窇趙陑倜--*/
			switch( chartype) {
			  case CHAR_IMAGETYPE_GIRL:
				sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
					  "\n\n ﹛鉣,涴欴斕腔黰薯憩奻汔蹕ㄐ"
					  "\n\n﹛﹛﹛郛懂郛褫乾賸儸ㄐ"
				);

				break;
			  case CHAR_IMAGETYPE_BOY:
				sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
					  "\n\n ﹛鉣,涴欴斕腔黰薯憩奻汔蹕ㄐ"
					  "\n\n﹛﹛岆祥岆掀試符猁邟嗣賸儸ˋ"
				);
			  	break;
			  	
			  case CHAR_IMAGETYPE_CHILDBOY:
			  case CHAR_IMAGETYPE_CHILDGIRL:
				sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
					  "\n\n ﹛鉣,涴欴斕腔黰薯憩奻汔蹕ㄐ"
					  "\n\n﹛﹛  淩腔曹腕疑褫乾遄ㄐ"
				);
			 	break;
			 	
			   case CHAR_IMAGETYPE_MAN:
				sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
					  "\n\n ﹛鉣,涴欴斕腔黰薯憩奻汔蹕ㄐ"
					  "\n\n ﹛﹛﹛曹腕載蹄賸儸ㄐ"
				);
			 	break;
			 	
			   case CHAR_IMAGETYPE_WOMAN:
				sprintf(token,"﹛﹛﹛﹛     = = 藝�楟� = =  "
				  "\n\n ﹛鉣,涴欴斕腔黰薯憩奻汔蹕ㄐ"
				  "\n\n    褫淩岆曹腕郛懂郛藝賸儸ㄐ"
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
	/*--這裡搜索處理--*/
	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					escapedname);

}


/*-----------------------------------------
袲溘騷囮璃哱凝�氪耽�趙拻倜鄹勗蠙怮③踏躂堎��
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

	/*--堁鉣禱蚍�欷の�媃踝竣--*/
	cost = NPC_CharmCost( meindex, talker);
	CHAR_setInt( talker, CHAR_GOLD,
			CHAR_getInt( talker, CHAR_GOLD) - cost);
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);

	/*--    誑兒兔兔雄窀勗埵堎鳹譴毀倯礎勗兒兔兔勗埰堎--*/
	if(CHAR_getInt( talker, CHAR_CHARM) + CHARMHEAL >= 100) {
		CHAR_setInt( talker, CHAR_CHARM, 100);
	}else{
		/*--    禱掛蚗哱--*/
		CHAR_setInt(talker, CHAR_CHARM,
	 			(CHAR_getInt( talker, CHAR_CHARM) + CHARMHEAL));
	}
	
	/*--筒  ↓淏筒摯脾犍--*/
	CHAR_complianceParameter( talker );
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);


	/*--妐蚗哱摯蚕溘隍↓淏禱脾犍--*/
	for( i = 0 ; i < CHAR_MAXPETHAVE ; i++){
    	petindex = CHAR_getCharPet( talker, i);

		if( petindex == -1  )  continue;

	   /*  す籤溘摯    鏍攝蚗袲    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;

		/*--蚕溘隍↓淏ぅ堆--*/
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );
	}
}


/*--堁鉣摯銓猀--*/
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
	
	/*-- 銓猀祪 --*/
	cost = level * RATE * (charm / WARU) * (trans+1);

	return cost;

}
