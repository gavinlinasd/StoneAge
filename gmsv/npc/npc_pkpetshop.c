//krynn 2001/12/6
//PKPetShop

#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "readmap.h"
#include "log.h"
#include "enemy.h"
#include "npc_pkpetshop.h"
#include "battle.h"

#ifdef _PKPETSHOP
#define MAXSHOPPET 33

static void NPC_PKPetShop_selectWindow( int meindex, int talker, int num,int select);
void NPC_PKPetShop_BuyMain(int meindex,int talker,int before );
void NPC_PKPetShop_GetPetList(char *argstr,char * argtoken2);
BOOL NPC_PKPetShop_SetNewPet(int meindex,int talker,char *data);
BOOL NPC_PKPetShop_SellNewPet(int meindex,int talker,char *data);

void NPC_PKPetShop_Menu(int meindex,int talker);
int NPC_PKPetShop_GetLimtPetList(int talker,char *argstr,char *token2,int sell);
void NPC_PKPetShop_SellMain(int meindex,int talker,int select);
int NPC_GetSellPetList(int itemindex,int flg,char *argstr,char *argtoken,int select,int sell);
BOOL NPC_AddPetBuy(int meindex, int talker,int petID,int kosuu,double rate);
void NPC_PetStrStr(int petID,double rate,char *name,char *token2, int index);
int NPC_SellPetstrsStr(int itemindex,int flg,double rate,char *argtoken,int select,int sell);
void NPC_LimitPetShop(int meindex,int talker,int select);
void NPC_PKPetShop_ExpressmanCheck(int meindex,int talker);
 

/*--極↓袲    擭壛--*/
enum{
	NPC_PKPETSHOP_WORK_NO 		= CHAR_NPCWORKINT1,
	NPC_PKPETSHOP_WORK_EV 		= CHAR_NPCWORKINT2,
	NPC_PKPETSHOP_WORK_EXPRESS	= CHAR_NPCWORKINT3,
};

		  
typedef struct {
	char	arg[32];
	int		type;
}PKPETSHOP_NPC_Shop;


static PKPETSHOP_NPC_Shop		TypeTable[] = {
	{ "FIST",		0 },
	{ "AXE",		1 },
	{ "CLUB",		2 },
	{ "SPEAR",		3},
	{ "BOW",		4},
	{ "SHIELD",		5},
	{ "HELM",		6 },
	{ "ARMOUR",		7 },
	{ "BRACELET",	8},
	{ "ANCLET",		9 },
	{ "NECKLACE",	10},
	{ "RING",		11},
	{ "BELT",		12},
	{ "EARRING",	13},
	{ "NOSERING",	14},
	{ "AMULET",		15},
	{ "OTHER",		16},
	{ "BOOMERANG",	17},
	{ "BOUNDTHROW",	18},
	{ "BREAKTHROW",	19},
#ifdef _ITEM_TYPETABLE
	{ "DISH",	20},
	{ "METAL",	21},
	{ "JEWEL",	22},
	{ "WARES",	23},
	{ "WBELT",	24},
	{ "WSHIELD", 25},
	{ "WSHOES",	26},
	{ "WGLOVE",	27},
	{ "ANGELTOKEN",	28},
	{ "HEROTOKEN",	29},
#endif	
	{ "ACCESSORY",	30},
	{ "OFFENCE",	40},
	{ "DEFENCE",	50},

};


/*********************************
* 場宎
*********************************/
BOOL NPC_PKPetShopInit( int meindex )
{

	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	/*--淏騷鏤优擭--*/
	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEPKPetShop );
	
	if( NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr)) == NULL ) 
	{
		print("NPC_PKPetShopInit_GetArgStrErr");
		return FALSE;
	}
	if(strstr(argstr,"LIMITSHOP") != NULL) 
	{		/*-  笢噁堇障暵啞溘精--*/
		CHAR_setWorkInt( meindex, NPC_PKPETSHOP_WORK_NO, 1);
	}
	else
	{
		CHAR_setWorkInt( meindex, NPC_PKPETSHOP_WORK_NO, 0);
	}
	
	if(strstr( argstr, "EVENT") != NULL) 
	{		/*-  笢噁堇障暵啞溘精--*/
		CHAR_setWorkInt( meindex, NPC_PKPETSHOP_WORK_EV, 1);
	}
	else
	{
		CHAR_setWorkInt( meindex, NPC_PKPETSHOP_WORK_EV, 0);
	}

	if(strstr( argstr, "EXPRESS") != NULL) 
	{		/*-痶邞碟啞溘精--*/
		CHAR_setWorkInt( meindex, NPC_PKPETSHOP_WORK_EXPRESS, 1);
	}
	else
	{
		CHAR_setWorkInt( meindex, NPC_PKPETSHOP_WORK_EXPRESS, 0);
	}
	return TRUE;
}


/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_PKPetShopTalked( int meindex , int talker , char *szMes ,int color )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buff[1024];
	char	buf2[256];
	char 	token[1024];
	int 	i = 1;
	BOOL	sellonlyflg = FALSE;
	char	sellmsg[1024];

    /* 設定相關參數處理 */
    if( CHAR_getInt( talker , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) 
	{
    	return;
    }

	/*--  摯鞳勗笢堎凝汔竣凝ㄑ--*/
	if(NPC_Util_isFaceToFace( meindex, talker, 2) == FALSE) 
	{		/* 角色動畫重置 */
		if( NPC_Util_CharDistance( talker, meindex ) > 1) return;
	}

    if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL)
	{
    	print("NPC_PKPetShopInit_GetArgStrErr");
    	return;
    }

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sellonly_msg", 
										 token, sizeof( token))
		!= NULL)
	{
		sellonlyflg = TRUE;
		strcpysafe(sellmsg, sizeof( sellmsg), token);
	}

	/*--  旽  竣坳騷璃迶坳勗笢崹堎凝汔竣凝--*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "buy_msg", 
		buff, sizeof( buff)) != NULL )
	{
	    while(getStringFromIndexWithDelim(buff,",",i,buf2,sizeof(buf2)) != FALSE)
		{
			i++;
			if( strstr( szMes, buf2) != NULL) {
				if( CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_EV) == 0) {
					/*-  笢噁堇障  凝汔竣凝摯鏍攝蚗袲--*/
					if( CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_NO) == 1) {
						if( sellonlyflg ) {
							NPC_PKPetShop_selectWindow( meindex, talker, 3, -1);
							return;
						}
					}
					else{
						NPC_PKPetShop_selectWindow( meindex, talker, 1, -1);
						return;
					}
				}else{
					if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_NO) == 1) {
						if( sellonlyflg) {
							NPC_PKPetShop_selectWindow( meindex, talker, 3, -1);
							return;
						}
					}else{
						NPC_PKPetShop_selectWindow( meindex, talker, 1, -1);
						return;
					}
		 			return;
		 	 	}
			}
		}
	}
	i=1;

	/*--  旽  堎坳騷璃迶坳勗笢崹堎凝汔竣凝--*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sell_msg", 
		buff, sizeof( buff)) != NULL )
	{
	    while( getStringFromIndexWithDelim(buff,",", i,buf2,sizeof(buf2))
	     != FALSE )
		{
			i++;
			if(strstr(szMes,buf2) != NULL) {
				NPC_PKPetShop_selectWindow( meindex, talker, 2, -1);
				return;
			}
		}
	}
	i = 1;


	/*--鼠摯眥摯樅璃哱禱�岉戰蟬仴穹�  媃凝崹堎--*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "other_msg", 
		buff, sizeof( buff)) != NULL )
	{
	    while(getStringFromIndexWithDelim( buff, ",", i, buf2, sizeof( buf2))
	     !=FALSE)
		{
			i++;
			if(strstr(szMes,buf2) != NULL) {
				/*--樅璃哱隍蚗掛↓喇--*/
				if(NPC_Util_GetStrFromStrWithDelim( argstr, "hint_msg", 
				token, sizeof( token)) != NULL)
				{
					CHAR_talkToCli( talker, meindex, token, CHAR_COLORWHITE);
					return;
				}
			}
		}	
	}

	/*-  笢噁堇障  凝汔竣凝摯鏍攝蚗袲--*/
	if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_NO) == 1)
	{
		if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_EV) == 1) 
		{
			if( sellonlyflg) 
			{
				CHAR_talkToCli( talker, meindex, sellmsg, CHAR_COLORWHITE);
				return;
			}
		}
		else
		{	/*--  笢噁堇障  摯隍蚗掛↓喇--*/
			if( sellonlyflg) 
			{
				NPC_PKPetShop_selectWindow( meindex, talker, 3, -1);
				return;
			}
		}
	}
	else
	{
		if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_EV) == 1) {
			if( sellonlyflg) {
				CHAR_talkToCli( talker, meindex, sellmsg, CHAR_COLORWHITE);
				return;
			}
		}else{
			
			if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_EXPRESS) == 1) {
				NPC_PKPetShop_ExpressmanCheck( meindex, talker);
			}else{ 
				/*--喫喫竘ぁ  倜�梖暯弄�騷璃迶坳(隍俓堥↓)  崚--*/
				/*--  笢噁堇障  ぁ埵�妡�  玊摯壨埵�梮肺萶芋�  崚--*/
				NPC_PKPetShop_selectWindow( meindex, talker, 0, -1);
			}
		}
	}
				
}

static void NPC_PKPetShop_selectWindow( int meindex, int talker, int num,int select)
{

	print("\n NPC_PKPetShop_selectWindow()");
	print("\n num = %d ", num);
	switch( num) {
	  case 0:
		/*--隍俓堥↓    --*/
		/*--蚕溘隍↓淏邞堇--*/
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
		
		if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_EXPRESS) == 1 ) 
		{
			if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_NO) ==0 ) 
			{
				NPC_PKPetShop_ExpressmanCheck( meindex, talker);
			}
		}
		else if(CHAR_getWorkInt( meindex, NPC_PKPETSHOP_WORK_NO) == 1) 
		{
		}
		else
		{
		  	NPC_PKPetShop_Menu( meindex, talker);
		}
	  	break;

	  case 1:
	  	/*--  竣    --*/
	  	NPC_PKPetShop_BuyMain( meindex, talker, select);
	  	break;

	  case 2:
	  	/*--  堎    --*/
	  	NPC_PKPetShop_SellMain( meindex, talker, select);
	  	break;

	  case 3:
	  	/*--  笢噁堇障  ぁ埰源    --*/
	  	NPC_LimitPetShop( meindex, talker, select);
	  	break;

	}
}



/*-----------------------------------------
 * 袲溘騷囮璃哱凝�氪耽�趙拻倜鄹勗蠙怮③踏躂堎��
 *
-------------------------------------------*/
void NPC_PKPetShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	/*--       摯竘渦堇勗笢埵笢敁拻毀敖   --*/
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 3) {
		/*--蚕溘隍↓淏邞堇--*/
		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
		return;
	}

	makeStringFromEscaped( data);
	switch( seqno){

	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG:
		/*--掘隍俓堥↓勗  �尕�--*/
		/*--  竣--*/
		if(atoi( data) == 1 )	NPC_PKPetShop_selectWindow(meindex, talkerindex, 1, -1);

		/*--  堎--*/
		if(atoi( data) == 2)	NPC_PKPetShop_selectWindow(meindex, talkerindex, 2, -1);

		/*--③堎--*/
		if(atoi( data) == 3)	return;/*--笰忒媃埵笢--*/
	
		break;


	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG:
		/*--囮騷  堜摯黹萃--*/
		if(NPC_PKPetShop_SetNewPet(meindex , talkerindex, data) == TRUE) {

			NPC_PKPetShop_selectWindow( meindex, talkerindex, 1, 0);

		}else{
			NPC_PKPetShop_selectWindow( meindex, talkerindex ,0, -1);
		}

		break;


	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG:
		/*--囮騷  堜摯朝謫--*/
		if(NPC_PKPetShop_SellNewPet(meindex , talkerindex, data) == TRUE) {
			NPC_PKPetShop_selectWindow( meindex, talkerindex, 2, 0);

		}else{
			NPC_PKPetShop_selectWindow( meindex,  talkerindex, 0, -1);
		}

		break;
			
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT:
		 if(select == WINDOW_BUTTONTYPE_YES) {
			NPC_PKPetShop_selectWindow( meindex, talkerindex ,2, -1);

		}else  if(select == WINDOW_BUTTONTYPE_NO) {
			return;
		}else if(select == WINDOW_BUTTONTYPE_OK) {
				NPC_PKPetShop_selectWindow( meindex, talkerindex, 2, -1);
		}
		break;
	
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_EXPRESS:
		if(atoi(data) == 2) {
			NPC_PKPetShop_selectWindow( meindex, talkerindex, 1, -1);
		}else if(atoi( data) == 4) {
			NPC_PKPetShop_selectWindow( meindex, talkerindex, 2, -1);
		}
	}
}


/*-----------------------------------------
 *
 *  竣窐  隍騷璃(袲溘騷囮璃哱邞嚭攷  軘埬  
 *
 *krynn 2001/12/9 樓腔蛁庋
 *before == -1
 *before <> -1
 *krynn end
 *-----------------------------------------*/
void NPC_PKPetShop_BuyMain(int meindex,int talker,int before )
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[NPC_UTIL_GETARGSTR_BUFSIZE];
	int fd = getfdFromCharaIndex( talker);

	/*
	  堇  笢啞溘精  鞳摯溢↓淏窅竣凝啞溘精  壨摯  鞳  隍蚗掛↓喇|  竣隍蚗掛↓喇  
	蠀倳撒  隍蚗掛↓喇  畛穫鳴鵿堇埵笢隍蚗掛↓喇  賃  隍蚗掛↓喇  
	  鞳    窇堎  窇埵笢啞溘精  囮騷  堜畛穫鳴  偯騤    鴢  蠕  郖菜騷璃啞б  
	  鞳    窇堎  窇埵笢啞溘精  囮騷  堜畛穫鳴  偯騤    鴢  蠕  郖菜騷璃啞б  
	*/
	
	/*--堁壨摯啞栝騷鳴  笢貺毀啞栝騷鳴誑楣崹埵凝埱倜敁拻毀敖  --*/
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("itemshop_GetArgStr_Err");
       	return;
    }

	/*--鞳樊摯溢↓淏窅竣凝---*/
	if(before != -1) 
	{	/*--鞳樊摯溢↓淏誑呫埱趙笢堎摯ぁ眥摯攷  毀邞�欷牟�    --*/
		sprintf(token,"0|0");

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
	}else{

		char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
		char buff2[256];
    	char buff[256];

		/*--掘隍蚗掛↓喇禱噁  媃趙袲溘騷囮璃哱勗邞堎溢↓淏禱軘堎-*/
		/*--喫摯盓堇  毀拻倜埵笢凝忒--*/
		NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "buy_main", buff2, sizeof( buff2));
		sprintf(token,"0|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getChar( meindex, CHAR_NAME), buff, buff2);

		NPC_Util_GetStrFromStrWithDelim( argstr, "what_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "level_msg", buff2, sizeof( buff));
		snprintf( token2, sizeof( token2), "%s|%s", buff, buff2);

		/*--  棬  譴  --*/
		strncat( token, token2, sizeof( token));
			
		NPC_Util_GetStrFromStrWithDelim( argstr, "realy_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "itemfull_msg", buff2, sizeof( buff2));
		sprintf( token2, "|%s|%s", buff, buff2);

		/*--  棬    踕--*/
		strncat(token , token2,sizeof(token));
		strcpy(token2, "|");
			
		/*--囮騷  堜攷  摯      --*/
		NPC_PKPetShop_GetPetList( argstr, token2 );
		// krynn 2001/12/12 bebug 蚚
		print("%s",token2);
		// end krynn

		/*--隍蚗掛↓喇敁囮騷  堜攷  摯譴  --*/
		strncat( token, token2, sizeof( token));
	}

		/*--  棬  禱操筒艇↓鏤踏鞠堎--*/
//		makeEscapeString( token, escapedname, sizeof(escapedname));

	/*--喫喫ぁ邞嚭--*/
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

}

/*------------------------------------------------------
 *囮騷  堜匴祛摯  棬  禱軘堎(  竣  
 *------------------------------------------------------*/
void NPC_PKPetShop_GetPetList(char *argstr,char *argtoken)
{

	int i = 1;
	int tmp;
	int EnemyCnt;
	char *name ;
	char buff2[256];
	char buff[NPC_UTIL_GETARGSTR_LINEMAX];
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	double rate = 1.0;
	int loopcnt = 0;

	/*-畛↓哱禱  堎��  崹躂�禸鉐甲彯�--*/
	if(NPC_Util_GetStrFromStrWithDelim( argstr, "buy_rate", buff2, sizeof( buff2))
	 != NULL){
		rate = atof( buff2);
	}
		
	/*-  翩竣喘  禱噁    --*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "PetList", buff, sizeof( buff))
	 != NULL )
	{
	    while( getStringFromIndexWithDelim(buff,",",i,buff2,sizeof(buff2))
	     !=FALSE )
	    {
			i++;
	    	/*--优擭啞栝騷鳴摯囮騷  堜誑 "-"ぁ鉏憍�桫噱胱俶職挺冕鯆慪濛�--*/
			if(strstr( buff2, "-") == NULL) {
				/*--竘囀  鞳      --*/
				// krynn 2001/12/10
				EnemyCnt = ENEMY_getEnemyNum();
				for( tmp=0 ; tmp < EnemyCnt ; tmp++ )
				{
					if( ENEMY_getInt( tmp , ENEMY_ID ) == atoi( buff2 ) )
					{
						print("\nNPC_PKPetShop_GetPetList: tmp = %d", tmp);
						break;
					}
				}
				if( tmp == EnemyCnt )
				{
					return;
				}
				name = ENEMY_getChar( tmp , ENEMY_NAME );
				// krynn end
				/*--  鞳誑        埵�掁�囮騷  堜誑凎凜媃埵笢摯ぁ鼠摯  蠕毀  鷋--*/
				if(name == NULL) continue;

				loopcnt++;
				if(loopcnt == MAXSHOPPET) break;

				/*--  傷摯鏤痲哱昡鳴僑�彸�--*/
				NPC_PetStrStr( atoi( buff2), rate, name, token2, tmp);

				/*--隍蚗掛↓喇  棬  敁  踕--*/
	    		strncat( argtoken, token2, sizeof(token2));
			}
			else
			{
				return;
				/*krynn 2001/12/13 涴僇茼蜆蚚祥善賸ㄛ珂 markㄛ蜊峈眻諉 return
				--囮騷  堜誑  15-25  摯魠ぁ鉏憍�桫噱胱俶鎔踽�--
				int start;
				int end;

				//-"-"ぁ鉏憍�桫憶郫宏尬匾扆檣�  摯倳偯禱噁  --
				getStringFromIndexWithDelim( buff2, "-", 1, token2, sizeof(token2));
				start = atoi( token2);
				getStringFromIndexWithDelim( buff2, "-", 2 ,token2, sizeof(token2));
				end = atoi( token2);

				//--  蠕誑楪勗埵埱趙笢倜�掁�  躂袑窇堎
				if(start > end)
				{
					tmp = start;
					start = end;
					end = tmp;
				}

				end++;

				//--"-"ぁ鉏憍�桫憶軘邾動岌�  堜攷  禱  堎--
				for(; start < end ; start++ ) 
				{
					//--竘囀  鞳      --

				 	name = ITEM_getNameFromNumber( start );
					//--  鞳誑        埵�掁�囮騷  堜誑凎凜媃埵笢摯ぁ鼠摯  蠕毀  鷋--
					if(name == NULL) continue;

					loopcnt++;
					if(loopcnt == MAXSHOPPET) break;

					//--  傷摯鏤痲哱昡鳴僑�彸�--
					NPC_PetStrStr( start, rate, name, token2, tmp);

					//--隍蚗掛↓喇  棬  敁  踕--
		    		strncat( argtoken, token2, sizeof(token2));
				}*/
			}
		}
	}
}


/*-----------------------------------------------------------------
	鏤痲哱昡鳴軘埬
-------------------------------------------------------------------*/
void NPC_PetStrStr(int petID,double rate,char *name,char *token2,int index)
{
	int i;
	int gold;
	int level;
	int graNo;
	int TempNo;
	int EnemyTempNum;
	char info[1024];
	//char tryItem[256];	// krynn 2001/12/12 硐岆猁艘 getItemInfoFromNumber 黍堤懂腔趼揹酗妦欒欴腔
	char escape[256] = {"PK Server 唾"};	

	//krynn 2001/12/10 try
	//gold  = ITEM_getcostFromITEMtabl( itemID);
	//level = ITEM_getlevelFromITEMtabl( itemID);
	//graNo = ITEM_getgraNoFromITEMtabl( itemID);
	TempNo = ENEMY_getInt( index , ENEMY_TEMPNO );
	EnemyTempNum = ENEMYTEMP_getEnemyNum();
	for( i=0 ; i < EnemyTempNum ; i++ )
	{
		if( ENEMYTEMP_getInt( i , E_T_TEMPNO ) == TempNo )
		{
			break;
		}
	}
	if( i == EnemyTempNum )
	{
		return;
	}
	gold  = RAND(0,20);
	level = 0;
	graNo = ENEMYTEMP_getInt( i , E_T_IMGNUMBER );
	//print("\nPKPetShop::NPC_PetStrStr(): TempNo = %d ; graNo = %d",TempNo,graNo);
	/*--畛↓哱禱凝崹堎--*/
	gold=(int)(gold * rate);

	//strcpy( tryItem,ITEM_getItemInfoFromNumber( 10 ) );
	//print("PKPetShop::NPC_PetStrStr(): try = %s\n",tryItem);
	//krynn end

	makeEscapeString( escape, info, sizeof( info));

	makeEscapeString( name, escape, sizeof( escape));

	sprintf( token2, "%s|0|%d|%d|%d|%s|", escape, level, gold, graNo, info);
}


/*-------------------------------------------
 *(  竣)
 *袲溘騷囮璃哱凝�氪耽�倜  倜踕堅禱  昅踏鞠堎
 *
 *------------------------------------------*/
BOOL NPC_PKPetShop_SetNewPet(int meindex,int talker,char *data)
{

	char buf[1024];
	char buff2[128];
	int i = 1, j = 1;
	int select;
	int kosuu = 0;
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	double rate = 1.0;
	int gold = 0;		
	int EmptyPetCnt=0;

	/*--蒍埱趙拻倜溢↓淏禱掛畛袲哱敁蠀倳勗覕搣--*/
	print("\nNPC_PKPetShop_SetNewPet: data = %s",data);
	getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf));
	select=atoi(buf);		// krynn 2001/12/10  select 岆俙模換隙猁鎗腔菴撓欴
	print("\nNPC_PKPetShop_SetNewPet: select = %d",select);
	if(select == 0) return FALSE;
	getStringFromIndexWithDelim( data, "|", 2, buf, sizeof( buf));
	kosuu=atoi(buf);
	print("\nNPC_PKPetShop_SetNewPet: kosuu(俙模猁鎗腔杅講) = %d",kosuu);

	if( kosuu <= 0 ) return FALSE;
    
	/* �堌族�  摯妐蚗哱摯昶拻禱潺埰 */
    if( !CHAR_CHECKINDEX(talker) )
	{
		return FALSE;
	}
	for( i=0 ; i < CHAR_MAXPETHAVE ; i++ ) 
	{
	    if( CHAR_getCharPet( talker,i) == -1 )
		{
			EmptyPetCnt++;
		}
    }
	// 衄 EmptyPetCnt 跺唾昜諾弇
    /* 昶拻誑  笢 */
 	print("\nNPC_PKPetShop_SetNewPet:EmptyPetCnt(俙模衄腔諾弇) = %d",EmptyPetCnt);
	if( EmptyPetCnt <= 0 ) return FALSE;
	if( EmptyPetCnt > CHAR_MAXPETHAVE )
	{
		EmptyPetCnt = CHAR_MAXPETHAVE;
	}
	if( kosuu > EmptyPetCnt )
	{
		kosuu = EmptyPetCnt;
	}

	// krynn 2001/12/10
	// 茼蜆蚚祥善ㄛ垀眕 mark れ懂
	/*--蠀倳摯鏍攝蚗袲  騍勗蠈    堎凝ㄑ
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
		itemindex = CHAR_getItemIndex( talker , i );

		if( !ITEM_CHECKINDEX( itemindex) ) {
			kosuucnt++;
		 }
	}

	--蒍埱趙  倜蠀倳摯  誑  傷摯蠀倳  �荂懽鼢�  源堇蘗笢敁堁凝媃笢摯ぁ
	--�荂懽鵅黥□�  禱  躂堎
	if( kosuucnt < kosuu){
		kosuu = kosuucnt;
	}
	--帤痲摯鳹譴毀操溘↓
	if(kosuucnt == 0 ){
		return FALSE;
	}
	krynn end */

	i = 1;

	/*--堁壨摯啞栝騷鳴  笢貺毀啞栝騷鳴誑楣崹埵凝埱倜敁拻毀敖  --*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return FALSE;
	}

	/*---畛↓哱禱噁    埵崹躂��1.0)-*/
	if(NPC_Util_GetStrFromStrWithDelim( argstr, "buy_rate", buf, sizeof( buf))
	 != NULL) {
		rate= atof( buf);
	}
	
	/*--囮騷  堜摯黹萃禱菜竣敁喫Й-*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "PetList", 
		buf, sizeof( buf)) != NULL )
	{
		while(getStringFromIndexWithDelim(buf , "," , j, buff2, sizeof(buff2))
		 != FALSE )
		{
			j++;
			/*--  "-"誑硈竘躂趙笢堎凝汔竣凝--*/
			if(strstr( buff2, "-") == NULL)
			{	// krynn 2001/12/10 try
				//if( ITEM_getcostFromITEMtabl(atoi(buff2)) !=-1) {
					// 
					if ( i == select)
					{
						/*---囮騷  堜摯軘埬---*/
						/*--蠀倳覕軘埬--*/
						if(NPC_AddPetBuy(meindex, talker,atoi(buff2),kosuu,rate) != TRUE)
						{
							return FALSE;
						}
						return TRUE;
					}
					i++;		
				// krynn end}
			}else{
				/*--囮騷  堜誑  15-25  摯魠ぁ邞�桫憶郯踽�--*/
				int start;
				int end;

				/* "-"ぁ鉏憍�桫憶郫宏尬匾扆檣�  摯倳偯禱噁  --*/
				getStringFromIndexWithDelim( buff2, "-", 1, argstr, sizeof(argstr));
				start = atoi( argstr);
				getStringFromIndexWithDelim( buff2, "-", 2 ,argstr, sizeof(argstr));
				end = atoi( argstr);
				end++;

				/*--  蠕誑楪勗埵埱趙笢倜�掁�  躂袑窇堎**/
				if(start > end){
					gold = start;
					start = end;
					end = gold;
				}

				/*--"-"ぁ鉏憍�桫憶軘邾動岌�  堜禱攷  禱  堎--*/
				for(; start < end ; start++ ) {
					if( ITEM_getcostFromITEMtabl( start) != -1) {
						if ( i == select) {
							/*---囮騷  堜摯軘埬---*/
							/*--蠀倳覕軘埬--*/
							if(NPC_AddPetBuy(meindex, talker, start, kosuu, rate) != TRUE)
							{
								return FALSE;
							}
							return TRUE;
						}
						i++;
					}
				}
			}
		}
	}

	return FALSE;

}


/*---------------------------------------------
 *囮騷  堜摯黹萃禱菜竣
 *--------------------------------------------*/
BOOL NPC_AddPetBuy(int meindex, int talker,int petID,int kosuu,double rate)
{
	int i,j,k,index,EnemyCnt,UpLevel;
	int gold;
	int ret;
	int maxgold;
	int Grade=0;
	char buf[1024];
	char msgbuf[64];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	
	// krynn 2001/12/15 get get's grade of this PKPetShop
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return FALSE;
	}
	if(NPC_Util_GetStrFromStrWithDelim( argstr, "Grade", buf, sizeof( buf))
	 != NULL) 
	{
		Grade = atoi( buf );
	}
	// end krynn

	/*--畛↓哱禱凝崹堎--*/
	// krynn 2001/12/11 mark and change
	//gold = ITEM_getcostFromITEMtabl( itemID);
	gold = 4;
	// krynn end
	gold = (int)(gold * rate);
	maxgold = gold * kosuu;

	if(CHAR_getInt( talker, CHAR_GOLD) < maxgold ) return FALSE;

	/*--堁鉣禱蚍�梊�--*/
	CHAR_setInt( talker, CHAR_GOLD,CHAR_getInt( talker, CHAR_GOLD) - maxgold);

	EnemyCnt = ENEMY_getEnemyNum();
	for( index=0 ; index < EnemyCnt ; index++ )
	{
		if( ENEMY_getInt( index , ENEMY_ID ) == petID )
		{
			print("\nNPC_AddPetBuy: index = %d", index);
			break;
		}
	}
	if( index == EnemyCnt )
	{
		return FALSE;
	}

	/*--蠀倳覕軘埬--*/
	for(i = 0 ; i < kosuu ; i++)
	{
		if( (ret = ENEMY_createPetFromEnemyIndex( talker , index )) == -1 )
		{
			return FALSE;
		};
		/******************/
		/* 妐蚗哱攷  邞堎 */
		/******************/
		// 汔喫勗毀笢埱倜凝埵
		for( j = 0 ; j < CHAR_MAXPETHAVE ; j++ )
		{
			if( CHAR_getCharPet( talker , j ) == ret )
			{
				break;
			}
		}
		if( j == CHAR_MAXPETHAVE ){
			return FALSE;
		}
		if( CHAR_CHECKINDEX( ret ) == TRUE ){
			CHAR_setMaxExpFromLevel( ret, Grade);
			UpLevel = CHAR_LevelUpCheck( ret , talker);
			for( k = 0; k < UpLevel; k ++ ){
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
			}
			CHAR_complianceParameter( ret );
			CHAR_setInt( ret , CHAR_HP , CHAR_getWorkInt( ret , CHAR_WORKMAXHP ) );

			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( talker, msgbuf );
			
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( talker, msgbuf );
		}
	}
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	return TRUE;
}



/*----------------------------------------
 *隍俓堥↓    
 *----------------------------------------*/
void NPC_PKPetShop_Menu(int meindex,int talker)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	token[NPC_UTIL_GETARGSTR_LINEMAX];
	char	buff[256];
	int		fd = getfdFromCharaIndex( talker);

	/* 壨摯  鞳  隍蚗掛↓喇| */
	/*--堁壨摯啞栝騷鳴  笢貺毀啞栝騷鳴誑楣崹埵凝埱倜敁拻毀敖  --*/
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("shop_GetArgStr_Err");
       	return;
    }
    	
    NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
	snprintf(token, sizeof(token),"%s|%s",CHAR_getChar( meindex, CHAR_NAME), buff);

	//	print("%s",escapedname);
	/*--喫喫ぁ邞嚭--*/
	//krynn 2001/12/10	涴爵腔統杅侔綱褫眕晊蚚 ItemShop 腔 codeㄛ珂蚚蚚艘
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMENU, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
	//krynn end
}


/*-------------------------------------------
 *	  堎窐  (鏤痲哱昡鳴禱軘埬  
 *	
 *-------------------------------------------*/
void NPC_PKPetShop_SellMain(int meindex,int talker,int before)
{

	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	token[NPC_UTIL_GETARGSTR_BUFSIZE];
	int fd = getfdFromCharaIndex( talker);


	/*
	  堇  笢啞溘精  鞳摯溢↓淏窅竣凝啞溘精  壨摯  鞳  隍蚗掛↓喇|  堎隍蚗掛↓喇  
	堁鉣誑笢埱毞笢勗埵堎源隍蚗掛↓喇  賃  隍蚗掛↓喇  
	  鞳    躂堎  躂埵笢啞溘精  偯騤    鴢  蠕  郖菜騷璃啞б  囮騷  堜    蠕  兒凝��  
	  鞳    躂堎  躂埵笢啞溘精  偯騤    鴢  蠕  郖菜騷璃啞б  囮騷  堜    蠕  兒凝��  
	*/
	
	/*--堁壨摯啞栝騷鳴  笢貺毀啞栝騷鳴誑楣崹埵凝埱倜敁拻毀敖  --*/
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
    }

	/*--鞳樊摯溢↓淏窅竣凝---*/
	if(before != -1) {

		/*--鞳樊摯溢↓淏誑呫埱趙笢堎摯ぁ眥摯攷  毀邞�欷牟�    --*/
		sprintf(token,"1|0");
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN
							+CHAR_getWorkInt(meindex,NPC_PKPETSHOP_WORK_NO), 
					WINDOW_BUTTONTYPE_NONE, 
					CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);
	
	}else{

		char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
		char buff2[256];
	   	char buff[256];

		/*--掘隍蚗掛↓喇禱噁  媃趙袲溘騷囮璃哱勗邞堎溢↓淏禱軘堎-*/
		/*--喫摯盓堇  毀拻倜埵笢凝忒--*/
		NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "sell_main", buff2, sizeof( buff));
		sprintf( token, "1|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getChar( meindex, CHAR_NAME), buff, buff2);

		NPC_Util_GetStrFromStrWithDelim( argstr, "stone_msg", buff, sizeof( buff));

		if(CHAR_getWorkInt(meindex,NPC_PKPETSHOP_WORK_EXPRESS) == 1 ) {
			NPC_Util_GetStrFromStrWithDelim( argstr, "exrealy_msg", buff2, sizeof(buff2));
		}else{
			NPC_Util_GetStrFromStrWithDelim( argstr, "realy_msg", buff2, sizeof( buff2));
		}
		sprintf( token2,"%s|%s|", buff, buff2);

		/*--囮騷  堜攷  摯      --*/
		NPC_PKPetShop_GetLimtPetList( talker,argstr, token2, -1);

		/*--隍蚗掛↓喇敁囮騷  堜攷  摯譴  --*/
		strncat( token, token2, sizeof( token));
	
		/*--喫喫ぁ邞嚭--*/
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN+
					CHAR_getWorkInt(meindex,NPC_PKPETSHOP_WORK_NO), 
					WINDOW_BUTTONTYPE_NONE, 
					CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);
	}
}





/*--------------------------------

猷覕誑堁壨勗  堎囮騷  堜摯  棬  禱摯奼  

 *-------------------------------*/
int NPC_PKPetShop_GetLimtPetList(int talker, char *argstr, char* token2,int sell)
{

	char token[NPC_UTIL_GETARGSTR_LINEMAX];
	char buff[NPC_UTIL_GETARGSTR_LINEMAX];
	char token3[NPC_UTIL_GETARGSTR_LINEMAX];
	int k = 0 , i = 1 , j = 0;
	int imax;
	int itemtype = 0;
	int itemindex;
	int okflg = 0;
	char buf[256];
	int flg=0;
	int cost;
	
	
	if(sell == -1 ){
		i = CHAR_STARTITEMARRAY;
		imax = CHAR_MAXITEMHAVE;
		flg = -1;
	}else{
		i= sell;
		imax= sell + 1;
		flg = 1;
	}	
	
	/*---  躂堎囮騷  堜禱噁  埰堎��--*/
	/**  筒妐で籤鳴囮騷  堜禱忒埱趙笢倜��  畛↓哱ぁ銓猀--*/
	/*-- 筒妐で籤鳴窐  ぁ鏤痲精溘堜誑    --*/

	for( ; i < imax ; i++ ){
		okflg=0;
		itemindex = CHAR_getItemIndex( talker , i );
		
		if( ITEM_CHECKINDEX( itemindex) ){

			/*--囮騷  堜摯淏騷鏤誑郖窙媃倜�掁�  躂堎--*/
			if( NPC_Util_GetStrFromStrWithDelim( argstr,"LimitItemType",
			buff, sizeof( buff))
			 != NULL )
			{
				k = 1;
				while(getStringFromIndexWithDelim(buff , "," , k, token, sizeof(token))
				 != FALSE )
				{
#ifdef _ITEM_TYPETABLE
					int cmpmaxitem = sizeof(TypeTable)/sizeof(TypeTable[0]);
#endif
					k++;
#ifdef _ITEM_TYPETABLE
					for(j = 0 ; j < cmpmaxitem ; j++){
#else
					for(j = 0 ; j < ITEM_CATEGORYNUM+3 ; j++){
#endif
						if(strcmp( TypeTable[ j].arg  , token) == 0 ) {
							itemtype = TypeTable[ j].type;
							if(ITEM_getInt(itemindex,ITEM_TYPE) == itemtype) {

								/*--  棬  軘埬--*/
								cost = NPC_GetSellPetList(itemindex,0,argstr,token3,i,sell);
								if(cost != -1) return cost;
								strncat( token2, token3, sizeof( token3));
								okflg = 1;
							}else if(itemtype == 30){
								if( 8 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 15) ){
									/*--  棬  軘埬--*/
									cost = NPC_GetSellPetList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}else if(itemtype == 40){
								if(( 0 <= ITEM_getInt(itemindex,ITEM_TYPE) 
								  && (ITEM_getInt(itemindex,ITEM_TYPE) <= 4)) 
								 || (17 <= ITEM_getInt(itemindex,ITEM_TYPE) 
								  && (ITEM_getInt(itemindex,ITEM_TYPE) <= 19))
								) {
									/*--  棬  軘埬--*/
									cost = NPC_GetSellPetList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}else if(itemtype == 50){
								if( 5 <= ITEM_getInt(itemindex,ITEM_TYPE) 
								&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 7) ){
									/*--  棬  軘埬--*/
									cost = NPC_GetSellPetList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}
							break;
						}
					}
						
					if(okflg == 1) break;
				}
			}
			/*--淏騷鏤勗埵�妡�  旽  蠕媃趙  躂堎囮騷  堜凝鏍攝蚗袲--*/
			if( (NPC_Util_GetStrFromStrWithDelim( argstr, "LimitItemNo",
			buff,sizeof( buff))
			 != NULL)
				&& okflg == 0 )
			{
				k = 1;
				while(getStringFromIndexWithDelim(buff , "," , k, token, sizeof(token))
				 != FALSE )
				{
					k++;
					/*--笰忒优擭踏躂趙埵凝埱倜�掁�  鷋埰堎-*/
					if(strstr( token, "-")==NULL && strcmp(token,"") != 0) {
						if(ITEM_getInt(itemindex,ITEM_ID) == atoi(token)) {
							/*--  棬  軘埬--*/
							cost = NPC_GetSellPetList(itemindex,0,argstr,token3,i,sell);
							if(cost != -1) return cost;
							strncat(token2,token3,sizeof(token3));
							okflg=1;
						}
					}else if (strstr( token, "-") != NULL){
						int start;
						int end;
						int work;

						/*--  棬  軘埬--*/
						/* "-"ぁ鉏憍�桫憶郫宏尬匾扆檣�  摯倳偯禱噁  --*/
						getStringFromIndexWithDelim( token, "-", 1, buf, sizeof(buf));
						start = atoi( buf);
						getStringFromIndexWithDelim( token, "-", 2 ,buf, sizeof(buf));
						end = atoi( buf);

						/*--  蠕誑楪勗埵埱趙笢倜�掁�  躂袑窇堎**/
						if(start > end){
							work = start;
							start = end;
							end = work;
						}

						end++;
						/*--"-"ぁ鉏憍�桫憶軘邾動岌�  堜禱攷  禱  堎--*/
						if( (start <= ITEM_getInt(itemindex,ITEM_ID))
						 && (ITEM_getInt(itemindex,ITEM_ID) < end) )
						 {
							/*--  棬  軘埬--*/
							cost = NPC_GetSellPetList(itemindex,0,argstr,token3,i,sell);
							if(cost != -1) return cost;
							strncat(token2,token3,sizeof(token3));
							okflg = 1;
						}
					}
				}
			}

			/*--  堎喫敁誑ぁ拻埵笢--*/
			if(okflg == 0) {
				cost = NPC_GetSellPetList(itemindex, 1, argstr, token3, i, sell);
				if(sell != -1) return -1;
				strncat( token2, token3, sizeof( token3));
			}
			
		}
	}
	return -1;
}



/*----------------------------------------------------------

	袲溘騷囮璃哱勗邞嚭埰堎鏤痲哱昡鳴摯軘埬

 *----------------------------------------------------------*/
int NPC_GetSellPetList(int itemindex,int flg, char *argstr,char *argtoken,int select,int sell)
{

	char buff[256];
	double rate = 0.2;
	char buff2[256];
	char buff3[64];
	int k = 1;
	int cost = -1;

	/*--筒妐で籤鳴畛↓哱--**/
	if(NPC_Util_GetStrFromStrWithDelim( argstr,"special_item",buff, sizeof( buff))
	!= NULL)
	{
		if(NPC_Util_GetStrFromStrWithDelim( argstr,"special_rate",buff2, sizeof( buff2))
		 != NULL )
		{
			rate = atof(buff2);
		}else{
			rate = 1.2;
		}
		
		while(getStringFromIndexWithDelim(buff , "," , k, buff2, sizeof(buff2)) !=FALSE )
		{
			k++;

			if(strstr( buff2, "-") == NULL && strcmp(buff2,"") != 0) {
				if(ITEM_getInt(itemindex,ITEM_ID) == atoi(buff2)){
					cost = NPC_SellPetstrsStr( itemindex,0, rate, argtoken,select,sell);
					return cost;
				}
			}else if (strstr( buff2, "-") != NULL){
				int start;
				int end;
				int work;

				/*--  棬  軘埬--*/
				/* "-"ぁ鉏憍�桫憶郫宏尬匾扆檣�  摯倳偯禱噁  --*/
				getStringFromIndexWithDelim( buff2, "-", 1, buff3, sizeof(buff3));
				start = atoi( buff3);
				getStringFromIndexWithDelim( buff2, "-", 2 ,buff3, sizeof(buff3));
				end = atoi( buff3);

				/*--  蠕誑楪勗埵埱趙笢倜�掁�  躂袑窇堎**/
				if(start > end){
					work = start;
					start = end;
					end = work;
				}
				end++;
	
				/*--"-"ぁ鉏憍�桫憶軘邾動岌�  堜禱攷  禱  堎--*/
				if( (start <= ITEM_getInt(itemindex,ITEM_ID))
					&&  (ITEM_getInt(itemindex,ITEM_ID) < end)
				){
			
					cost = NPC_SellPetstrsStr( itemindex,0, rate, argtoken,select,sell);
					return cost;
				}
			}
		}
	}

	/*--蚚↓悃鳴畛↓哱--*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr,"sell_rate",buff, sizeof( buff))
	 != NULL )
	{
		rate = atof(buff);
		cost = NPC_SellPetstrsStr( itemindex, flg ,rate, argtoken,select,sell);
		return cost;
	}

	return cost;
}


/*------------------------------------------
 
   棬  禱軘堎(竣堇暵)
 
*------------------------------------------*/
int NPC_SellPetstrsStr(int itemindex,int flg,double rate,char *argtoken,int select,int sell)
{
	int cost;
	char escapedname[256];
	char name[256];	
	char *eff;
	

	cost = ITEM_getInt( itemindex, ITEM_COST);
	cost = (int)(cost * rate);

	if(sell != -1) return cost;

		
	//strcpy( escapedname, ITEM_getChar( itemindex, ITEM_NAME));
	strcpy( escapedname, ITEM_getChar( itemindex, ITEM_SECRETNAME));
	makeEscapeString( escapedname, name, sizeof( name));
	eff=ITEM_getChar(itemindex, ITEM_EFFECTSTRING);
	makeEscapeString( eff, escapedname, sizeof(escapedname));



	sprintf(argtoken,"%s|%d|%d|%d|%s|%d|",
			name,
			flg,
			cost,
			ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
			escapedname,
			select
	);

	return -1;

}

/*--------------------------------------------
 *
 *袲溘騷囮璃哱凝�淢灠簏立耽�趙拻倜敁拻    堎    
 *
 *-------------------------------------------*/
BOOL NPC_PKPetShop_SellNewPet(int meindex,int talker,char *data)
{
	int select;
	int cost;
	int oldcost;
	char token[32];
	int k;
	int itemindex;
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token2[256];
	
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("GetArgStrErr");
		return FALSE;
	}


	/*--蒍埱趙  倜溢↓淏摯覕搣--*/
	getStringFromIndexWithDelim(data , "|" ,1, token, sizeof( token));
	select = atoi(token);
	getStringFromIndexWithDelim(data , "|" ,2, token, sizeof( token));
	oldcost = atoi(token);

	if(select == 0) return FALSE;

	cost = NPC_PKPetShop_GetLimtPetList( talker,argstr, token2,select);


	/*--鄖竣忒摯禱竣Й竣敁媃倜敁拻摯操溘↓--*/
	if(oldcost != cost || cost == -1)
	{
		int fd = getfdFromCharaIndex( talker);
		char token[256];
		
		sprintf(token,"\n\n陞挼!勤祥れ"
					"\n\n勤祥れ陛 ! 褫祥褫眕婬恁珨棒儸ˋ"
		);
		
		k = select;
		itemindex = CHAR_getItemIndex( talker ,k);
		
		
		/*--痲精摯③  --*/
		if(itemindex != -1) {
			print("\n%s(%d,%d,%d):睿恁寁腔陲昹祥肮error([%s(%d)]猁秏囮賸ㄘ",
					CHAR_getChar(talker, CHAR_NAME),
					CHAR_getInt( talker, CHAR_FLOOR),
					CHAR_getInt( talker, CHAR_X ),
					CHAR_getInt( talker, CHAR_Y ),
					ITEM_getChar(itemindex, CHAR_NAME),
					ITEM_getInt( itemindex, ITEM_ID )
			);
			LogItem(
					CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
					CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
					itemindex,
#else
	       			ITEM_getInt( itemindex, ITEM_ID ),  /* 錯誤訊息 */
#endif
					"SellErr",
					CHAR_getInt( talker, CHAR_FLOOR),
					CHAR_getInt( talker, CHAR_X ),
					CHAR_getInt( talker, CHAR_Y ),
					ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
			);
		}else{
			print("\n%s(%d,%d,%d):睿恁寁腔陲昹祥肮error(羶衄�庥姽tem湔婓ㄘ",
					CHAR_getChar(talker, CHAR_NAME),
					CHAR_getInt( talker, CHAR_FLOOR),
					CHAR_getInt( talker, CHAR_X ),
					CHAR_getInt( talker, CHAR_Y )
			);
			LogItem(
					CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
					CHAR_getChar( talker, CHAR_CDKEY ),
	       			-1,  /* 錯誤訊息 */
					"SellErr",
					CHAR_getInt( talker, CHAR_FLOOR),
					CHAR_getInt( talker, CHAR_X ),
					CHAR_getInt( talker, CHAR_Y ),
					"-1", "NULL", -1 );
		}
		/*--喫喫ぁ邞嚭--*/
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
				WINDOW_BUTTONTYPE_OK, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

		
		return FALSE;
	}

	k = select;
	itemindex=CHAR_getItemIndex( talker ,k);

	if(itemindex != -1) {
		{
			LogItem(
				CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
				CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
	       		ITEM_getInt( itemindex, ITEM_ID ),  /* 錯誤訊息 */
#endif
				"Sell",
				CHAR_getInt( talker,CHAR_FLOOR),
				CHAR_getInt( talker,CHAR_X ),
				CHAR_getInt( talker,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)

			);
		}
		
	}

	CHAR_DelItem( talker, k);
	CHAR_AddGold( talker, cost);
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);

	return TRUE;
}



void NPC_LimitPetShop(int meindex,int talker,int select)
{

	int fd = getfdFromCharaIndex( talker);
	char token[NPC_UTIL_GETARGSTR_LINEMAX];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];

	/*--堁壨摯啞栝騷鳴  笢貺毀啞栝騷鳴誑楣崹埵凝埱倜敁拻毀敖  --*/
	if( NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
	}

	/*--  笢噁堇障  壨ぁ埰隍蚗掛↓喇-*/
	if(NPC_Util_GetStrFromStrWithDelim( argstr, "sellonly_msg", buf, sizeof( buf))
	!=NULL)
	{
		sprintf(token,"\n\n%s", buf);

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
				WINDOW_BUTTONTYPE_YESNO, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

	}else{
		CHAR_talkToCli( talker, meindex, "涴岆鎗闖蚳藷虛﹝",CHAR_COLORWHITE);
	}
	return;
}


/*--痶邞碟踏庌--*/
void NPC_PKPetShop_ExpressmanCheck(int meindex,int talker)
{
	int fd = getfdFromCharaIndex( talker);
	char token[1024];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];

	/*--堁壨摯啞栝騷鳴  笢貺毀啞栝騷鳴誑楣崹埵凝埱倜敁拻毀敖  --*/
	if( NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
	}


	/*--  笢噁堇障  壨ぁ埰隍蚗掛↓喇-*/
	NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buf, sizeof( buf));
	sprintf(token,"4\n﹛﹛﹛﹛﹛﹛﹛%s\n\n%s"
					"\n\n﹛﹛﹛﹛﹛ˉ  湖馱  ˇ﹛﹛﹛"
				  "\n\n﹛﹛﹛﹛  ˉ蝠葆俴燠ˇ"
					,CHAR_getChar(meindex,CHAR_NAME),buf);

	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
			WINDOW_BUTTONTYPE_CANCEL, 
			CHAR_WINDOWTYPE_WINDOWITEMSHOP_EXPRESS,
			CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
			token);

	return;


}


#endif
 // _PKPETSHOP




