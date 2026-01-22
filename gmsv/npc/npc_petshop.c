#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#ifdef _CHECK_ENEMY_PET
#include "enemy.h"
#endif
/* 隍蚗掛↓喇摯enum */
enum {
	NPC_PETSHOP_MSG_OVER,
	NPC_PETSHOP_MSG_MAIN,
	NPC_PETSHOP_MSG_REALY,
	NPC_PETSHOP_MSG_THANKS,
	NPC_PETSHOP_MSG_COST,
	NPC_PETSHOP_MSG_POOLTHANKS,
	NPC_PETSHOP_MSG_POOLFULL,
	NPC_PETSHOP_MSG_GETFULL,
};
typedef struct {
	char	option[32];
	char	defaultmsg[128];
}NPC_PETSHOP_MSG;

NPC_PETSHOP_MSG		shopmsg[] = {
	{ "over_msg",		"ヴ蚚俇蹕!羶恀枙鎘?"},
	{ "main_msg",		"辣茩"},
	{ "realy_msg",		"淩腔褫眕鎘?"},
	{ "thanks_msg",		"郅郅!"},
	{ "cost_msg",		"ヴ祥劂鉊!"},
	{ "pooltanks_msg",	"郅郅!"},
	{ "poolfull_msg",	"閉徹腔拸楊揭燴鉊"},
	{ "getfull_msg",	"斕祥岆衄竭嗣唾昜鎘"}
	
};

#define PETCOST	10

void NPC_PetDate(int meindex,int toindex,int select,char *token);
void NPC_PetDate2(int meindex,int talker,int select,char *token);
void NPC_PetDate3(int meindex,int talker,int select,char *token);
void NPC_PetDel2( int meindex, int talker, int select, char *token);
void NPC_PetDel3( int meindex, int talker, int select, char *token);
void NPC_getDrawPet( int meindex, int toindex, char *token, int *buttontype, int select);

static void NPC_PetShop_selectWindow( int meindex, int toindex, int num,int select);
int NPC_GetCost(int meindex,int talker,int petindex);
void NPC_PetDel(int meindex, int toindex,int select,char* token);
void NPC_MaxGoldOver(int meindex,int toindex,int select,char *token);

#ifdef _NPC_DEPOTPET
void NPC_PoolPetShop_DepotPet_Menu( int meindex, int talkerindex);
void NPC_DepotPet_List( int meindex, int toindex, char *token, int *buttontype, int select);
void NPC_DepotPet_Add( int meindex, int talker, int select, char *token);
void NPC_DepotPet_Get( int meindex, int talker, int select, char *token);
void NPC_DepotPet_AskGet(int meindex,int talker,int select,char *token);
//void NPC_DepotPet_Pet_printWindow( int meindex, int talkerindex);
//void NPC_DepotPet_Depot_printWindow( int meindex, int talkerindex);
//BOOL NPC_DepotPet_InsertPet( int meindex, int talkerindex, int num);
//BOOL NPC_DepotPet_getPet( int meindex, int talkerindex, int num);
#endif


//   崹  躂鉣滌銓猀祪
#define		NPC_GETPOOLCOST( talker)		( 50 + CHAR_getInt( talker, CHAR_LV)*4)

//   崹  躂鉣溢啞б鳴哱
#define		NPC_PETSHOP_POOLCOST_DEFAULT	200

/*--極↓袲    摯賃瞀--*/
enum {
	CHAR_WORK_NOMALRATE	= CHAR_NPCWORKINT1,
	CHAR_WORK_SPECIALRATE	= CHAR_NPCWORKINT2,
	CHAR_WORK_POOLCOST	= CHAR_NPCWORKINT3,
	CHAR_WORK_POOLFLG	= CHAR_NPCWORKINT4,
};


/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_PetShopInit( int meindex )
{

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[256];
	double rate;
	int intrate;
//	int cost;
	int tmp;

	/*--淏騷鏤优擭--*/
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTONESHOP );


	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL){
		print("GetArgStrErr");
		return FALSE;
	}

	/*--畛↓哱禱鴽誧勗楔呏踏鞠趙堁��*/
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "nomal_rate", buf, sizeof( buf)) != NULL){
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,1000 );
	}

		
	/*--筒妐で籤鳴畛↓哱忒楔呏踏鞠趙堁��--*/
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "special_rate", buf, sizeof( buf))
	!=NULL)
	{
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,1200 );
	}
	
	//   崹  躂鉣楔呏
	//cost = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_cost");
	//if( cost == -1 ) cost = NPC_PETSHOP_POOLCOST_DEFAULT;
	//CHAR_setWorkInt( meindex, CHAR_WORK_POOLCOST, cost);
	
	
	//   崹  躂誑③  堎凝汔竣凝禱楔呏
	tmp = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_flg");
	if( tmp != 1 ) tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_POOLFLG, tmp);
    
    return TRUE;

}




/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_PetShopTalked( int meindex , int talkerindex , char *szMes ,int color )
{

    /* 設定相關參數處理 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	

	/*--  摯鞳勗笢堎凝汔竣凝ㄑ--*/
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		/* 角色動畫重置 */
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}

	/*--極↓袲摯疐趵撬--*/
	CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,0);

	/*-毀啋誧摯撒      --*/
	NPC_PetShop_selectWindow( meindex, talkerindex,0,-1);

}

/*********************************
* 峟�桫憶袬敯啄�  
*********************************/
void NPC_PetShopLooked( int meindex , int lookedindex)
{
    /* 設定相關參數處理 */
    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/* 角色動畫重置 */
	if( NPC_Util_CharDistance( lookedindex, meindex ) > 2) return;

	/*--極↓袲摯疐趵撬--*/
//	CHAR_setWorkInt(lookedindex,CHAR_WORKSHOPRELEVANT,0);


//	NPC_PetShop_selectWindow( meindex, lookedindex,0,-1);

}

static void NPC_PetShop_selectWindow( int meindex, int toindex, int num,int select)
{

	char token[1024];
	int buttontype=0,windowtype=0,windowno=0;
	char buf[1024];
	int fd = getfdFromCharaIndex( toindex);
	
	//print("\n NPC_PetShop_selectWindow num:%d select:%d  ", num, select);

	if( fd == -1 ) {
		fprint( "getfd err\n");
		return;
	}
	
	token[0] = '\0';

	switch(num){
      //   疐摯坳贖璃迶坳
	  case 0:
		NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_MAIN, buf);
#ifdef _NPC_DEPOTPET
		if( !CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG)) {
			sprintf(token, "4\n﹛﹛﹛﹛﹛﹛∵唾昜妀虛∵"
							"%s"
							"\n"
							"\n﹛﹛﹛﹛﹛﹛ ▲ 闖唾昜 ◎"
							"\n"
							"\n"
							"\n             ▲ 燭羲 ◎",
							buf);
		}else {
			sprintf(token, "4\n﹛﹛﹛﹛﹛﹛∵唾昜妀虛∵"
							"%s"
							"\n            ▲ 敵溫唾昜 ◎"
							"\n            ▲ 鍰�○駘� ◎"
							"\n﹛﹛﹛﹛﹛﹛ ▲ 闖唾昜 ◎"
							"\n            ▲ 妏蚚累踱 ◎"
							"\n              ▲ 燭羲 ◎",
							buf);
		}
#else
		if( !CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG)) {
			sprintf(token, "4\n﹛﹛﹛﹛﹛﹛∵唾昜妀虛∵"
							"%s"
							"\n"
							"\n﹛﹛﹛﹛﹛﹛ ▲ 闖唾昜 ◎"
							"\n"
							"\n             ▲ 燭羲 ◎",
							buf);
		}else {
			sprintf(token, "4\n﹛﹛﹛﹛﹛﹛∵唾昜妀虛∵"
							"%s"
							"\n            ▲ 敵溫唾昜 ◎"
							"\n            ▲ 鍰�○駘� ◎"
							"\n﹛﹛﹛﹛﹛﹛ ▲ 闖唾昜 ◎"
							"\n              ▲ 燭羲 ◎",
							buf);
		}
#endif

		buttontype=WINDOW_BUTTONTYPE_NONE;
		windowtype=WINDOW_MESSAGETYPE_SELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_START; 
		break;
	  case 1:
	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT; 
		break;
	  case 2:
		NPC_PetDate(meindex,toindex,select,token);
		buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN; 
		break;
	  case 3:
		if( (CHAR_getInt(toindex,CHAR_GOLD)+CHAR_getWorkInt(toindex,CHAR_WORKSHOPRELEVANTSEC))
			> CHAR_getMaxHaveGold( toindex) ){

			NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_OVER, token);
			windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER; 
			buttontype=WINDOW_BUTTONTYPE_YESNO;
		}else{
			NPC_PetDel(meindex,toindex,select,token);
			buttontype=WINDOW_BUTTONTYPE_OK;
			windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		}

	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		break;
	  case 4:
	 	NPC_PetDel(meindex,toindex,select,token);
		buttontype=WINDOW_BUTTONTYPE_OK;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		windowtype=WINDOW_MESSAGETYPE_MESSAGE;
		break;
	  case 5: // 鍰�□譟鑒餑訇�
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;
	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2; 
		break;
	  case 6:
		if( CHAR_getInt( toindex, CHAR_GOLD) 
			< NPC_GETPOOLCOST( toindex)) 
		{
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
			buttontype = WINDOW_BUTTONTYPE_OK;
		}
		else {
			NPC_PetDate2( meindex, toindex, select, token);
			buttontype = WINDOW_BUTTONTYPE_YESNO;
		}
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2; 
	    break;
	  
	  case 7:
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	    
		if( CHAR_getInt( toindex, CHAR_GOLD) < NPC_GETPOOLCOST( toindex)){
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
		}else {
			NPC_PetDel2( meindex, toindex, select, token);
		}
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  //   崹  躂  誑郖  
	  case 8:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_POOLFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // 礎拻噁堇妐蚗哱郖  坳贖璃迶坳
	  case 9: // �○鶻譟鑒餑訇�
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG) != 1 )
	  		return;
	  	NPC_getDrawPet( meindex, toindex, token, &buttontype, select);
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT; 
		break;
	  // 礎拻噁堇    ㄑ
	  case 10:
		NPC_PetDate3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW;
	    break;
	  // 蒍埰
	  case 11:
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	  
		NPC_PetDel3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // 妐蚗哱  誑郖  
	  case 12:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_GETFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
#ifdef _NPC_DEPOTPET
	  case 13: // 湔�貒祪煻訇�
	  	if( !CHAR_CheckDepotPet( toindex) )
	  		return;
	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_DEPOTPETSHOP_ADD;
	    break;
	  case 14: // �○鶷祪煻訇�
	  	if( !CHAR_CheckDepotPet( toindex) )
	  		return;
		NPC_DepotPet_List( meindex, toindex, token, &buttontype, select);
	  	//buttontype=WINDOW_BUTTONTYPE_CANCEL;
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_GET;
		break;

	  case 15: // 戙恀�溜炴磃貒祪�
		if( CHAR_getInt( toindex, CHAR_GOLD) 
			< NPC_GETPOOLCOST( toindex)) 
		{
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
			buttontype = WINDOW_BUTTONTYPE_OK;
		}
		else {
			NPC_PetDate2( meindex, toindex, select, token);
			buttontype = WINDOW_BUTTONTYPE_YESNO;
		}
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKADD;
	    break;

	  case 16: // 戙恀�溜珂○鶷祪�
		NPC_DepotPet_AskGet( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKGET;
	    break;

	  case 17: // �溜炴磃貒祪�
	  	if( !CHAR_CheckDepotPet(toindex) )
	  		return;
		if( CHAR_getInt( toindex, CHAR_GOLD) < NPC_GETPOOLCOST( toindex)){
		  	NPC_MaxGoldOver(meindex, toindex, NPC_PETSHOP_MSG_COST, token);
		}else {
			NPC_DepotPet_Add( meindex, toindex, select, token);
		}
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;

	  case 18: // �溜珂○鶷祪�
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	  
		if( NPC_DepotPet_CheckLevel( meindex, toindex, select, token) == TRUE) {
			NPC_DepotPet_Get( meindex, toindex, select, token);
			buttontype = WINDOW_BUTTONTYPE_OK;
	  		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		}
		else {
			sprintf( token, "\n斕拸楊桽嘈涴硐唾昜﹝");
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END;
		}
	    break;
#endif
	  // CoolFish: Add 2001/7/4
	  default:
	    break;
	
	}
	
	/*--操筒艇↓鏤--*/
	//makeEscapeString( token, escapedname, sizeof(escapedname));
		
		
	/*--邞嚭--*/
	lssproto_WN_send( fd, windowtype, 
				buttontype, 
				windowno,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

}

void NPC_PetShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	int datanum = -1;
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}
	
	//print("\n NPC_PetShopWindowTalked: data:%s seq:%d sel:%d ", data, seqno, select);

	makeStringFromEscaped( data);
	
	datanum = atoi( data);

	switch( seqno){
	case CHAR_WINDOWTYPE_WINDOWPETSHOP_START:
		{
			int poolflg = CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG);
			
			if( ( datanum == 3 && poolflg) || // 闖唾昜
				( datanum == 2 && !poolflg ) ){	// 鍰�○駘�
				NPC_PetShop_selectWindow( meindex, talkerindex, 1, -1 );
			}else if( datanum == 1 && poolflg) { // 敵溫唾昜 
				if( CHAR_getCharPoolPetElement( talkerindex) != -1 ) {
					NPC_PetShop_selectWindow( meindex, talkerindex, 5, -1 );
				}
				else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 8, -1 );
				}
			}else if( datanum == 2 && poolflg ) { // 鍰�○駘�
				if( CHAR_getCharPetElement( talkerindex) != -1 ) {
					CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
					NPC_PetShop_selectWindow( meindex, talkerindex, 9, 0);
				}else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
				}
			}
#ifdef _NPC_DEPOTPET
			else if( datanum == 4 ) { // 妏蚚唾昜累踱
				print(" 妏蚚唾昜累踱 ");
				CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
				if( !CHAR_CheckDepotPet( talkerindex) ){
					CHAR_GetDepotPet( meindex, talkerindex);
					CHAR_talkToCli( talkerindex, -1, "�△籀駘鵃甭輶婛嶂�", CHAR_COLORYELLOW);
				}else{
					NPC_PoolPetShop_DepotPet_Menu( meindex, talkerindex);
				}
			}
#endif
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT:
		if(select==WINDOW_BUTTONTYPE_CANCEL){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
			
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 2, datanum );
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_PETSELECT2:
		if(select==WINDOW_BUTTONTYPE_CANCEL){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 6, datanum );
		}
		break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 3,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex,1,-1);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_MAIN2:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 7,-1 );
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex,5,-1);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW:
		if( select == WINDOW_BUTTONTYPE_YES){
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 11,-1 );
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, 0);
	  	}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_GOLDOVER:
		if( select == WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 4,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
	  	break;
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT:
		if( select == WINDOW_BUTTONTYPE_PREV){
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, -1);
		}else if( select == WINDOW_BUTTONTYPE_NEXT) {
			NPC_PetShop_selectWindow( meindex, talkerindex, 9, 1);
		}else if( select != WINDOW_BUTTONTYPE_CANCEL) {
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 10, datanum);
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
		break;

#ifdef _NPC_DEPOTPET
	  case CHAR_WINDOWTYPE_DEPOTPETSHOP_HANDLE:
		  if( !CHAR_CheckDepotPet( talkerindex) ) return ;
		  switch( datanum ) {
		  case 1://溫��
				if( CHAR_findEmptyDepotPet( talkerindex) == -1 ){
					CHAR_talkToCli( talkerindex, -1, "唾昜累踱眒雛ㄐ", CHAR_COLORYELLOW);
					return;
				}
				//NPC_DepotPet_Pet_printWindow( meindex, talkerindex);
				NPC_PetShop_selectWindow( meindex, talkerindex, 13, -1 );
			  break;
		  case 2://�○�
				if( CHAR_getCharPetElement( talkerindex) < 0 ) {
					CHAR_talkToCli( talkerindex, -1, "旯奻唾昜戲弇眒雛ㄐ", CHAR_COLORYELLOW);
					return;
				}
				NPC_PetShop_selectWindow( meindex, talkerindex, 14, 0);
			  break;
		  case 0:
			  NPC_PetShop_selectWindow( meindex, talkerindex, 0, -1);
			  break;
		  }
		  break;

	  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ADD:
		if(select==WINDOW_BUTTONTYPE_CANCEL){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}else{
			NPC_PetShop_selectWindow( meindex, talkerindex, 15, datanum );
		}
		break;

	  case CHAR_WINDOWTYPE_DEPOTPETSHOP_GET:
		if( select == WINDOW_BUTTONTYPE_PREV){
			NPC_PetShop_selectWindow( meindex, talkerindex, 14, -1);
		}else if( select == WINDOW_BUTTONTYPE_NEXT) {
			NPC_PetShop_selectWindow( meindex, talkerindex, 14, 1);
		}else if( select != WINDOW_BUTTONTYPE_CANCEL) {
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 16, datanum);
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}

		break;
	  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKADD: // �溜炴磃貒祪�
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 17,-1 );
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex,13,-1);
	  	}
		break;
			
	  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKGET: // �溜珂○鶷祪�
		if( select == WINDOW_BUTTONTYPE_YES){
			if( CHAR_getCharPetElement( talkerindex) != -1 ) {
				NPC_PetShop_selectWindow( meindex, talkerindex, 18,-1 );
			}else {
				NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
			}
		}else {
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
			NPC_PetShop_selectWindow( meindex, talkerindex, 14, 0);
	  	}
		break;
#endif
	  
	  case CHAR_WINDOWTYPE_WINDOWPETSHOP_END:
		if( select == WINDOW_BUTTONTYPE_OK){
			NPC_PetShop_selectWindow( meindex, talkerindex, 0,-1 );
		}
	  	break;

	  // CoolFish: Add 2001/7/4
	  default:
	  	break;

	}
}

void NPC_PetDate(int meindex,int talker,int select,char *token)
{
	
	int petindex;
	int cost;
	char *petname;
	char buf[1024];

	/*--極↓袲摯疐趵撬--*/
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,select);

	petindex = CHAR_getCharPet( talker, select-1);
        

   if( !CHAR_CHECKINDEX(petindex) ) return;

  	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_REALY, buf);

	cost=NPC_GetCost(meindex,talker,petindex);

	petname = CHAR_getUseName( petindex);

	sprintf(token,"\n岆[%s]羶渣勘ˋ""%s"
					"\n\n    鎗闖歎跡         %d STONE\n"
					,petname,buf,cost);	
		
}

void NPC_PetDate2(int meindex,int talker,int select,char *token)
{
	
	int petindex;
	char *petname;
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,select);

	petindex = CHAR_getCharPet( talker, select-1);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	petname = CHAR_getUseName( petindex);
	sprintf(token,"\n敵溫[%s]"
					"\n\n    敵溫歎跡            %d stone\n"
					,petname, NPC_GETPOOLCOST( talker)
					);	
}

void NPC_PetDate3(int meindex,int talker,int select,char *token)
{
	int poolindex;
	int petindex;
	char *petname;
	poolindex = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) * 5 + select - 1;
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,poolindex );
	petindex = CHAR_getCharPoolPet( talker, poolindex);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	petname = CHAR_getUseName( petindex);
	sprintf(token,"\n鍰隙[%s]", petname);	
}

int NPC_GetCost(int meindex,int talker,int petindex)
{
	int cost;
	int levelcost;
	int level;
	int getlevel;
	int graNo;
	int rare;
	int charm=50;
	int petai;
	int i=1;
	double rate=1.0;
	int intrate;
	char buf[1024];
	char buff2[128];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("GetArgStrErr");
		return FALSE;
	}

		
	getlevel=CHAR_getInt(petindex,CHAR_PETGETLV);
	level=CHAR_getInt(petindex,CHAR_LV);
	graNo=CHAR_getInt(petindex,CHAR_BASEBASEIMAGENUMBER);
	rare=CHAR_getInt(petindex,CHAR_RARE);

	if(getlevel==0)	getlevel=1	;
	
	/*--偯騤禱滑誧堎--*/
	levelcost=(level*level)*10;
	getlevel=(getlevel*getlevel)*10;
// Syu ADD 党蜊闖蛌汜唾踢ヴ頗曹蛹腔bug
#ifdef _PET_2TRANS
	if( CHAR_getInt(petindex , CHAR_TRANSMIGRATION)>0)
#else
	if( CHAR_getInt(petindex , CHAR_TRANSMIGRATION)==1)
#endif
		getlevel=10;


	if(rare==0){ rare=1;
	}else if(rare==1){ rare=5;
	}else if(rare==2){ rare=8;}

	cost = ((levelcost-getlevel)+(level*PETCOST))*rare;
	intrate = CHAR_getWorkInt( meindex , CHAR_WORK_NOMALRATE);
	rate=(double) intrate / 1000;
	

	NPC_Util_GetStrFromStrWithDelim( npcarg, "special_pet", buf, sizeof( buf));

	i=1;
	/*--筒妐で籤鳴畛↓哱ぁ銓猀踏躂堎妐蚗哱禱鄎堇③埰  妐蚗哱摯炩擭毀  鴢  蠕)--*/
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )
	{
		if(strstr( buff2, "-")==NULL){
			if(graNo == atoi(buff2)){
				intrate=CHAR_getWorkInt( meindex , CHAR_WORK_SPECIALRATE);
				rate=(double) intrate / 1000;
				break; 
			}
		}else{
			/*--囮騷  堜誑  15-25  摯魠ぁ鉏憍�桫噱胱俶鎔踽�--*/
			int start;
			int end;
			int tmp;
			int graflg=0;
			char	token2[128];
	
			/*-"-"ぁ鉏憍�桫憶郫宏尬匾扆檣�  摯倳偯禱噁  --*/
			getStringFromIndexWithDelim( buff2, "-", 1, token2, sizeof(token2));
			start = atoi( token2);
			getStringFromIndexWithDelim( buff2, "-", 2 ,token2, sizeof(token2));
			end = atoi( token2);

			/*--  蠕誑楪勗埵埱趙笢倜�掁�  躂袑窇堎**/
			if(start > end){
				tmp=start;
				start=end;
				end=tmp;
			}

			end++;
			/*--"-"ぁ鉏憍�桫憶軘邾動岌�  堜攷  禱  堎--*/
			if(start <= graNo && graNo < end){
				intrate=CHAR_getWorkInt( meindex , CHAR_WORK_SPECIALRATE);
				rate=(double) intrate / 1000;
				graflg=1;
				break; 
			}
			
			if(graflg ==1) break;
		}
		i++;
	}

	/*--妐蚗哱摯櫆笠梣禱      ---*/
	petai=CHAR_getWorkInt(petindex,CHAR_WORKFIXAI);

	/*--    禱斛蚗哱--*/
	charm = CHAR_getWorkInt(talker,CHAR_WORKFIXCHARM);

	/*--      ＞櫆笠梣  / 兕  */
	charm = charm + petai;

	if(charm < 20){
		charm = 20;
	}

	charm = charm / 2;

	cost = (int)cost*rate;

	rate=(double)charm/100;

	/*--    櫆笠揚崹--*/
	cost = cost * rate;

	/*--堁鉣禱楔呏--*/
   	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,cost);

	return cost;
}


#if 1

void NPC_PetDel(int meindex, int talker,int select,char* token)
{
	int petindex;
	int petsel;
	char szPet[128];
	int cost;
	int defpet;
	int fd = getfdFromCharaIndex( talker );

	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_THANKS, token);
	petsel=(CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANT)-1);
	petindex = CHAR_getCharPet( talker, petsel);
	
    if( !CHAR_CHECKINDEX(petindex) ) return;

        // CoolFish: Family 2001/6/14
	if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){	
/*		char buf[1024];
		lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
		makeEscapeString("\n\n竭惕К‵忐誘忤拸楊毽闖鉊ㄐ\n③赻撩疑疑桽嘈ㄐ", buf, sizeof(buf)));
*/
		sprintf(token,"\n\n竭惕К‵忐誘忤拸楊毽闖鉊ㄐ\n③赻撩疑疑桽嘈ㄐ");
		return;
	}

        // Robin 0725
        if (CHAR_getInt( talker, CHAR_RIDEPET ) == petsel ){	
/*        	char buf[1024];
        	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
        		WINDOW_BUTTONTYPE_OK,
        		-1, -1,
        		makeEscapeString("\n\nる傚笢腔唾昜拸楊毽闖鉊ㄐ", buf, sizeof(buf)));
*/
					sprintf(token,"\n\nる傚笢腔唾昜拸楊毽闖鉊ㄐ");
        	return;
        }

	if( CHAR_getWorkInt( CONNECT_getCharaindex(fd),
                         CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;
	defpet=CHAR_getInt(talker,CHAR_DEFAULTPET);
	if(defpet==petsel){
		CHAR_setInt(talker,CHAR_DEFAULTPET,-1);
		lssproto_KS_send( fd, -1, TRUE);
	}
   	CHAR_setCharPet( talker, petsel, -1);
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"sell(闖唾)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
	CHAR_endCharOneArray( petindex );
		
	snprintf( szPet, sizeof( szPet ), "K%d", petsel);
	CHAR_sendStatusString( talker, szPet );
	cost=NPC_GetCost(meindex,talker,petindex);
	CHAR_AddGold( talker, cost);

}
#endif
void NPC_PetDel2( int meindex, int talker, int select, char *token)
{
	
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int fd = getfdFromCharaIndex( talker );

	if( fd == -1 ) {
		fprint( "err\n");
		return;
	}
	// 妐蚗哱溢↓淏  脾�袼該檗�      崹  躂  
	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) -1;
	petindex = CHAR_getCharPet( talker, selectpet);
	if( !CHAR_CHECKINDEX( petindex) ) return;
	
        // Robin 0725
        if (CHAR_getInt( talker, CHAR_RIDEPET ) == selectpet )
        {
/*        	char buf[1024];
        	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
        		WINDOW_BUTTONTYPE_OK,
        		-1, -1,
        		makeEscapeString("\n\nる傚笢腔唾昜拸楊敵溫鉊ㄐ", buf, sizeof(buf)));
*/
					sprintf(token,"\n\nる傚笢腔唾昜拸楊敵溫鉊ㄐ");
        	return;
        }
        
	// 昶拻禱潺埰�跇§撘醓し紐鯆慪濛阹し胱俶翹副奴摀薱蟜用鄵郅胱倠聿邾健椒ず框ひ�
	emptyindex = CHAR_getCharPoolPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}
	// 橋  奼  暵妐蚗哱埵�掁搮孩噯啗�  禱觛埰��
	if( CHAR_getInt( talker, CHAR_DEFAULTPET) == selectpet) {
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}
	//   崹  躂  勗�楟該檗�摯騷璃溢蚗袲筒禱昡鼀↓
	CHAR_setCharPoolPet( talker, emptyindex, petindex);
	// 妐蚗哱  凝�殏梊�
	CHAR_setCharPet( talker, selectpet, -1);
	
	// 隍蚗掛↓喇噁  
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	// 堁鉣蚍�梊�

	CHAR_DelGold( talker, NPC_GETPOOLCOST( talker) );

	// 堁鉣溢↓淏邞堎
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	snprintf( szPet, sizeof( szPet ), "K%d", selectpet);
	// 犍媃笢妐蚗哱筒  ↓淏筒邞堇僑崹堎
	CHAR_sendStatusString( talker, szPet );
	// 妐蚗哱禱  崹倜痲精
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"pool(敵溫唾昜虛)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
        CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

}	

void NPC_PetDel3( int meindex, int talker, int select, char *token)
{
	
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int workindex[CHAR_MAXPOOLPETHAVE];
	int i;
	int cnt;

	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) ;
	petindex = CHAR_getCharPoolPet( talker, selectpet);
	if( !CHAR_CHECKINDEX( petindex) ) return;
	emptyindex = CHAR_getCharPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}

	// �蝜�岆忐誘忤蜊峈ぱ籵忤
	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 
		&& CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 3 ) {
		CHAR_setInt( petindex, CHAR_PETFAMILY, 0);
		CHAR_talkToCli( talker, -1, "忐誘忤蜊峈ぱ籵忤", CHAR_COLORYELLOW);
	}

	CHAR_setCharPet( talker, emptyindex, petindex);
	CHAR_setCharPoolPet( talker, selectpet, -1);
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	snprintf( szPet, sizeof( szPet ), "K%d", emptyindex);
	CHAR_sendStatusString( talker, szPet );
	snprintf( szPet, sizeof( szPet), "W%d",emptyindex);
	CHAR_sendStatusString( talker, szPet );
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		workindex[i] = -1;
	}
	cnt = 0;
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		int petindex = CHAR_getCharPoolPet( talker, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			workindex[cnt++] = petindex;
		}
	}
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		CHAR_setCharPoolPet( talker, i, workindex[i]);
	}
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ),
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"draw(鍰�○駘齔�)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
#ifdef _PET_EVOLUTION
	if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) == 1 &&
		CHAR_getInt( petindex, CHAR_FUSIONRAISE) > 0 ){//潰脤岆瘁峈�睆炡�
		CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time( NULL));
	}
#endif
}	

void NPC_getDrawPet( int meindex, int toindex, char *token, int *buttontype, int select)
{
	// 兒妐↓喇堣倜堇摯菜倳
	#define		LINENUM		5
	char buf[1024];
	int i;
	int startnum, endnum;
	int petindex = -1;
	// 藯凜摯妐↓喇
	int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
	int limit;
	//andy_reEdit 2003/09/18
	limit = (CHAR_getInt(toindex,CHAR_TRANSMIGRATION)*2)+5;

	endnum = (page + select+1)*LINENUM ;
	startnum = endnum - LINENUM;
	*buttontype = WINDOW_BUTTONTYPE_CANCEL;
	// 輊妐↓喇ぅ堆��
	if( endnum >= limit ) {
		endnum = limit;
		if( page +select!= 0 ) *buttontype |= WINDOW_BUTTONTYPE_PREV;
	}
	else if( endnum <= LINENUM ) {
		endnum = LINENUM;
		*buttontype |= WINDOW_BUTTONTYPE_NEXT;
	}
	else {
		*buttontype |= WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
	}
	CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page +select);
	
	strcpy( token, "2\n������﹛③恁寁唾昜﹛������\n\n");
	for( i =  startnum; i < endnum; i ++ ) {
		petindex = CHAR_getCharPoolPet( toindex, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			snprintf( buf, sizeof( buf), "LV.%3d MaxHP %3d %s\n", 
					CHAR_getInt( petindex, CHAR_LV),
					CHAR_getWorkInt( petindex, CHAR_WORKMAXHP),
					CHAR_getUseName( petindex)
					);
		}
		else {
			strcpy( buf, "\n");
		}
		strcat( token, buf);
	}
	// べ摯郖蠀禱ぅ迋趙��  凝埱倜�梪摀萯擅�璃禱噁堎
	if( i != limit ) {
		petindex = CHAR_getCharPoolPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex)) {
			*buttontype &= ~WINDOW_BUTTONTYPE_NEXT;
		}
	}
}

/*
 *堁鉣誑酘↓泬↓鼠摯眥笢Й笢Й
 */
void	NPC_MaxGoldOver(int meindex,int toindex,int select,char *token)
{

	char buf[1024];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg));
	
	if( select < 0 || select >= arraysizeof( shopmsg)) return;
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, shopmsg[select].option, buf, sizeof( buf))
		== NULL ) 
	{
		strcpy( buf, shopmsg[select].defaultmsg);
	}

	snprintf(token,sizeof(buf),"\n\n%s",buf);

}

#ifdef _NPC_DEPOTPET
void NPC_PoolPetShop_DepotPet_Menu( int meindex, int talkerindex)
{
	int fd;
	
	//print(" NPC_PoolPetShop_DepotPet_Menu ");

	fd = getfdFromCharaIndex( talkerindex);
	if( fd != -1 ) {
		char	message[1024];
		char	buf[2048];

		strcpy( message, 
			"3\n\n"
			"          妏蚚唾昜累踱\n\n"
			"          ˉ湔溫唾昜ˇ\n"
			"          ˉ�◆堻駘鵃閱n"
		);

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
						WINDOW_BUTTONTYPE_CANCEL,
						CHAR_WINDOWTYPE_DEPOTPETSHOP_HANDLE,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( message, buf, sizeof(buf)));
	}
}

void NPC_DepotPet_List( int meindex, int toindex, char *token, int *buttontype, int select)
{
	// 兒妐↓喇堣倜堇摯菜倳
	#define		LINENUM		5
	char buf[1024];
	int i;
	int startnum, endnum;
	int petindex = -1;
	// 藯凜摯妐↓喇
	int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
	int limit;

	limit = CHAR_MAXDEPOTPETHAVE;

	endnum = (page + select+1)*LINENUM ;
	startnum = endnum - LINENUM;
	*buttontype = WINDOW_BUTTONTYPE_CANCEL;
	// 輊妐↓喇ぅ堆��
	if( endnum >= limit ) {
		endnum = limit;
		if( page +select!= 0 ) *buttontype |= WINDOW_BUTTONTYPE_PREV;
	}
	else if( endnum <= LINENUM ) {
		endnum = LINENUM;
		*buttontype |= WINDOW_BUTTONTYPE_NEXT;
	}
	else {
		*buttontype |= WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_NEXT;
	}
	CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, page +select);
	
	strcpy( token, "2\n������﹛③恁寁猁植累踱�○鷁議駘鵅�������\n\n");
	for( i =  startnum; i < endnum; i ++ ) {
		petindex = CHAR_getDepotPetIndex( toindex, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			snprintf( buf, sizeof( buf), "LV.%3d MaxHP %3d %s\n", 
					CHAR_getInt( petindex, CHAR_LV),
					CHAR_getWorkInt( petindex, CHAR_WORKMAXHP),
					CHAR_getUseName( petindex)
					);
		}
		else {
			strcpy( buf, "\n");
		}
		strcat( token, buf);
	}
	// べ摯郖蠀禱ぅ迋趙��  凝埱倜�梪摀萯擅�璃禱噁堎
	if( i != limit ) {
		petindex = CHAR_getDepotPetIndex( toindex, i);
		if( !CHAR_CHECKINDEX( petindex)) {
			*buttontype &= ~WINDOW_BUTTONTYPE_NEXT;
		}
	}
	//print("\n DepotPet_List:%s ", token);
}


void NPC_DepotPet_Add( int meindex, int talker, int select, char *token)
{
	
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int fd = getfdFromCharaIndex( talker );

	if( fd == -1 ) {
		fprint( "err\n");
		return;
	}
	// 妐蚗哱溢↓淏  脾�袼該檗�      崹  躂  
	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) -1;
	//print("\n selectpet:%d ", selectpet);
	petindex = CHAR_getCharPet( talker, selectpet);
	if( !CHAR_CHECKINDEX( petindex) ) return;
	
    // Robin 0725
    if (CHAR_getInt( talker, CHAR_RIDEPET ) == selectpet )
    {
		sprintf(token,"\n\nる傚笢腔唾昜拸楊湔�貒祪瘔腦�");
		return;
	}

	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 )
	{
		sprintf(token,"\n\n模逜忐誘忤拸楊湔�貒祪瘔腦�");
		return;
	}

#ifdef _CHECK_ENEMY_PET
	{
		int j;
		for( j=0;j<ENEMYTEMP_getEnemyNum();j++ ) {//ENEMY_getEnemyNum()
			if( CHAR_getInt( petindex, CHAR_PETID) == ENEMYTEMP_getInt( j, E_T_TEMPNO ) ) {//ENEMY_getInt( j, ENEMY_TEMPNO)
				if( ENEMYTEMP_getInt( j, E_T_PETFLG ) == 0 ){//ENEMY_getInt( j, ENEMY_PETFLG) 
				    sprintf(token,"\n\n涴岆祥夔厥衄腔唾昜鉊ㄐ");
					return;
				}
			}
		}
	}
#endif    
	// 昶拻禱潺埰�跇§撘醓し紐鯆慪濛阹し胱俶翹副奴摀薱蟜用鄵郅胱倠聿邾健椒ず框ひ�
	emptyindex = CHAR_getCharDepotPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}

	// 橋  奼  暵妐蚗哱埵�掁搮孩噯啗�  禱觛埰��
	if( CHAR_getInt( talker, CHAR_DEFAULTPET) == selectpet) {
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}
	//   崹  躂  勗�楟該檗�摯騷璃溢蚗袲筒禱昡鼀↓
	CHAR_setDepotPetIndex( talker, emptyindex, petindex);
	// 妐蚗哱  凝�殏梊�
	CHAR_setCharPet( talker, selectpet, -1);
	
	// 隍蚗掛↓喇噁  
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	// 堁鉣蚍�梊�

	CHAR_DelGold( talker, NPC_GETPOOLCOST( talker) );

	// 堁鉣溢↓淏邞堎
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	snprintf( szPet, sizeof( szPet ), "K%d", selectpet);
	// 犍媃笢妐蚗哱筒  ↓淏筒邞堇僑崹堎
	CHAR_sendStatusString( talker, szPet );
	// 妐蚗哱禱  崹倜痲精
	if( NPC_DepotPet_CheckRepeat( talker) == 0)  // 潰脤晤鎢笭葩唾
	{
		LogPet(
			CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
			CHAR_getChar( talker, CHAR_CDKEY ),
			CHAR_getChar( petindex, CHAR_NAME),
			CHAR_getInt( petindex, CHAR_LV),
			"pool(湔�貐駘鼤祪�)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
			CHAR_getInt( talker,CHAR_Y ),
			CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
		);
	}
	else
	{
		CHAR_talkToCli( talker, -1, "涴岆準楊腔唾昜鉊!!", CHAR_COLORYELLOW);
	}

}	



void NPC_DepotPet_Get( int meindex, int talker, int select, char *token)
{
	
	int petindex;
	int selectpet;
	int emptyindex;
	char szPet[128];
	int workindex[CHAR_MAXPOOLPETHAVE];
	int i;
	int cnt;

	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) ;
	petindex = CHAR_getDepotPetIndex( talker, selectpet);
	if( !CHAR_CHECKINDEX( petindex) ) return;
	emptyindex = CHAR_getCharPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}
	CHAR_setCharPet( talker, emptyindex, petindex);
	CHAR_setChar( petindex, CHAR_OWNERCHARANAME, CHAR_getChar( talker, CHAR_NAME));
	CHAR_setDepotPetIndex( talker, selectpet, -1);
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	snprintf( szPet, sizeof( szPet ), "K%d", emptyindex);
	CHAR_sendStatusString( talker, szPet );
	snprintf( szPet, sizeof( szPet), "W%d",emptyindex);
	CHAR_sendStatusString( talker, szPet );
	for( i = 0; i < CHAR_MAXDEPOTPETHAVE; i ++ ) {
		workindex[i] = -1;
	}
	cnt = 0;
	for( i = 0; i < CHAR_MAXDEPOTPETHAVE; i ++ ) {
		int petindex = CHAR_getDepotPetIndex( talker, i);
		if( CHAR_CHECKINDEX( petindex) ) {
			workindex[cnt++] = petindex;
		}
	}
	for( i = 0; i < CHAR_MAXDEPOTPETHAVE; i ++ ) {
		CHAR_setDepotPetIndex( talker, i, workindex[i]);
	}
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ),
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"draw(鍰�〃祪滼駘�)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
#ifdef _PET_EVOLUTION
	if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) == 1 &&
		CHAR_getInt( petindex, CHAR_FUSIONRAISE) > 0 ){//潰脤岆瘁峈�睆炡�
		CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time( NULL));
	}
#endif
}	

void NPC_DepotPet_AskGet(int meindex,int talker,int select,char *token)
{
	int depotindex;
	int petindex;
	char *petname;
	depotindex = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) * 5 + select - 1;
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,depotindex );
	petindex = CHAR_getDepotPetIndex( talker, depotindex);
	if( !CHAR_CHECKINDEX(petindex) ) return;
	petname = CHAR_getUseName( petindex);
	sprintf(token,"\n鍰隙[%s]", petname);	
}

int NPC_DepotPet_CheckRepeat( int charaindex)
{
	int petindex1, petindex2;
	int i, j, find, ret = FALSE;
	int workindex[CHAR_MAXDEPOTPETHAVE], cnt =0;

	for( i =0; i <(CHAR_MAXDEPOTPETHAVE-1); i++)
	{
		petindex1 = CHAR_getDepotPetIndex( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex1)) continue;
		find = 0;
		for( j =(i+1); j <CHAR_MAXDEPOTPETHAVE; j++)
		{
			petindex2 = CHAR_getDepotPetIndex( charaindex, j);
			if( !CHAR_CHECKINDEX( petindex2)) continue;
			if( petindex1 == petindex2 ) continue;
			if( !strcmp( CHAR_getChar( petindex1, CHAR_UNIQUECODE),
						CHAR_getChar( petindex2, CHAR_UNIQUECODE) ) )
			{
				print(" 婓累踱梑善準楊唾賸 ");
				CHAR_setDepotPetIndex( charaindex, j, -1);
				NPC_DepotPet_CheckRepeat_del( charaindex, petindex2);
				find ++;
				ret = TRUE;
			}
		}
		if( find >0)
		{
			CHAR_setDepotPetIndex( charaindex, i, -1);
			NPC_DepotPet_CheckRepeat_del( charaindex, petindex1);
		}
	}

	for( i = 0; i < CHAR_MAXDEPOTPETHAVE; i ++ ) {
		workindex[i] = -1;
	}
	for( i = 0; i < CHAR_MAXDEPOTPETHAVE; i ++ ) {
		int pindex = CHAR_getDepotPetIndex( charaindex, i);
		if( CHAR_CHECKINDEX( pindex) ) {
			workindex[cnt++] = pindex;
		}
	}
	for( i = 0; i < CHAR_MAXDEPOTPETHAVE; i ++ ) {
		CHAR_setDepotPetIndex( charaindex, i, workindex[i]);
	}

	return ret;
}

int NPC_DepotPet_CheckRepeat_del( int charaindex, int petindex)
{
	int i;

	LogPet(
		CHAR_getChar( charaindex, CHAR_NAME ), /* す籤溘   */
		CHAR_getChar( charaindex, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"del(ь壺累踱炴唾)",
		CHAR_getInt( charaindex,CHAR_FLOOR),
		CHAR_getInt( charaindex,CHAR_X ),
		CHAR_getInt( charaindex,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
		);

	CHAR_endCharOneArray( petindex );

}

// 潰脤脹撰岆瘁逋劂
int NPC_DepotPet_CheckLevel(int meindex,int talker,int select,char *token)
{
	int petindex;
	int selectpet;

	if( !CHAR_CheckDepotPet( talker) )
		return FALSE;

	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) ;
	petindex = CHAR_getDepotPetIndex( talker, selectpet);
	if( !CHAR_CHECKINDEX(petindex) ) return FALSE;
	
	//print("\n trans:%d char_lv:%d pet_lv:%d ",
	//	CHAR_getInt( talker, CHAR_TRANSMIGRATION),
	//	CHAR_getInt( talker, CHAR_LV),
	//	CHAR_getInt( petindex, CHAR_LV) );

	// �蝜�岆忐誘忤蜊峈ぱ籵忤
	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 
		&& CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 3 ) {
		CHAR_setInt( petindex, CHAR_PETFAMILY, 0);
	}

	if( CHAR_getWorkInt( talker, CHAR_PickAllPet) == TRUE ) { // 蚾掘拲忤賭硌腔趕...
		return TRUE;
	}

	if( CHAR_getInt( talker, CHAR_TRANSMIGRATION) > 0 
		|| CHAR_getInt( talker, CHAR_LV)+5 >= CHAR_getInt( petindex, CHAR_LV) ) {
		return TRUE;
	}
	
	return FALSE;
	
}

#endif

