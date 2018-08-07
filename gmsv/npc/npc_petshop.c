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
/* 丢永本□斥及enum */
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
	{ "over_msg",		"钱用完罗!没问题吗?"},
	{ "main_msg",		"欢迎"},
	{ "realy_msg",		"真的可以吗?"},
	{ "thanks_msg",		"谢谢!"},
	{ "cost_msg",		"钱不够喔!"},
	{ "pooltanks_msg",	"谢谢!"},
	{ "poolfull_msg",	"超过的无法处理喔"},
	{ "getfull_msg",	"你不是有很多宠物吗"}
	
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


//   仃  木嗯喊煌遥挚
#define		NPC_GETPOOLCOST( talker)		( 50 + CHAR_getInt( talker, CHAR_LV)*4)

//   仃  木嗯犯白巧伙玄
#define		NPC_PETSHOP_POOLCOST_DEFAULT	200

/*--伐□弁    及割忡--*/
enum {
	CHAR_WORK_NOMALRATE	= CHAR_NPCWORKINT1,
	CHAR_WORK_SPECIALRATE	= CHAR_NPCWORKINT2,
	CHAR_WORK_POOLCOST	= CHAR_NPCWORKINT3,
	CHAR_WORK_POOLFLG	= CHAR_NPCWORKINT4,
};


/*********************************
* 赓渝质  
*********************************/
BOOL NPC_PetShopInit( int meindex )
{

	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[256];
	double rate;
	int intrate;
//	int cost;
	int tmp;

	/*--正奶皿涩烂--*/
   	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTONESHOP );


	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL){
		print("GetArgStrErr");
		return FALSE;
	}

	/*--伊□玄毛铵户卞筏盛今六化云仁*/
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "nomal_rate", buf, sizeof( buf)) != NULL){
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_NOMALRATE ,1000 );
	}

		
	/*--旦矢扑乓伙伊□玄手筏盛今六化云仁--*/
	if(NPC_Util_GetStrFromStrWithDelim( npcarg, "special_rate", buf, sizeof( buf))
	!=NULL)
	{
		rate = atof( buf);		
		intrate = ( int)( rate *1000);		
   		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,intrate );
	}else{
		CHAR_setWorkInt( meindex , CHAR_WORK_SPECIALRATE ,1200 );
	}
	
	//   仃  木嗯筏盛
	//cost = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_cost");
	//if( cost == -1 ) cost = NPC_PETSHOP_POOLCOST_DEFAULT;
	//CHAR_setWorkInt( meindex, CHAR_WORK_POOLCOST, cost);
	
	
	//   仃  木互请  月井升丹井毛筏盛
	tmp = NPC_Util_GetNumFromStrWithDelim( npcarg, "pool_flg");
	if( tmp != 1 ) tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_POOLFLG, tmp);
    
    return TRUE;

}




/*********************************
*   仄井仃日木凶凛及质  
*********************************/
void NPC_PetShopTalked( int meindex , int talkerindex , char *szMes ,int color )
{

    /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	

	/*--  及蟆卞中月井升丹井＂--*/
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		/* ㄠ弘伉永玉动  及心 */
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}

	/*--伐□弁及赓渝祭--*/
	CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,0);

	/*-反元户及蓟      --*/
	NPC_PetShop_selectWindow( meindex, talkerindex,0,-1);

}

/*********************************
* 苇日木凶凛及质  
*********************************/
void NPC_PetShopLooked( int meindex , int lookedindex)
{
    /* 皿伊奶乩□卞覆仄化分仃  杀允月 */
    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/* ㄠ弘伉永玉动  及心 */
	if( NPC_Util_CharDistance( lookedindex, meindex ) > 2) return;

	/*--伐□弁及赓渝祭--*/
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
      //   赓及它奴件玉它
	  case 0:
		NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_MAIN, buf);
#ifdef _NPC_DEPOTPET
		if( !CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG)) {
			sprintf(token, "4\n　　　　　　☆宠物商店☆"
							"%s"
							"\n"
							"\n　　　　　　 《 卖宠物 》"
							"\n"
							"\n"
							"\n             《 离开 》",
							buf);
		}else {
			sprintf(token, "4\n　　　　　　☆宠物商店☆"
							"%s"
							"\n            《 寄放宠物 》"
							"\n            《 领取宠物 》"
							"\n　　　　　　 《 卖宠物 》"
							"\n            《 使用仓库 》"
							"\n              《 离开 》",
							buf);
		}
#else
		if( !CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG)) {
			sprintf(token, "4\n　　　　　　☆宠物商店☆"
							"%s"
							"\n"
							"\n　　　　　　 《 卖宠物 》"
							"\n"
							"\n             《 离开 》",
							buf);
		}else {
			sprintf(token, "4\n　　　　　　☆宠物商店☆"
							"%s"
							"\n            《 寄放宠物 》"
							"\n            《 领取宠物 》"
							"\n　　　　　　 《 卖宠物 》"
							"\n              《 离开 》",
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
	  case 5: // 领取寄放宠列表
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
	  //   仃  木  互域  
	  case 8:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_POOLFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // 娄五潸曰矢永玄域  它奴件玉它
	  case 9: // 取出寄放宠列表
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG) != 1 )
	  		return;
	  	NPC_getDrawPet( meindex, toindex, token, &buttontype, select);
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_DRAWSELECT; 
		break;
	  // 娄五潸曰    ＂
	  case 10:
		NPC_PetDate3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_ASKDRAW;
	    break;
	  // 忒允
	  case 11:
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	  
		NPC_PetDel3( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
	  // 矢永玄  互域  
	  case 12:
	  	NPC_MaxGoldOver( meindex, toindex, NPC_PETSHOP_MSG_GETFULL, token);
		buttontype = WINDOW_BUTTONTYPE_OK;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
	    break;
#ifdef _NPC_DEPOTPET
	  case 13: // 存入仓库列表
	  	if( !CHAR_CheckDepotPet( toindex) )
	  		return;
	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_PETSELECT;
		windowno=CHAR_WINDOWTYPE_DEPOTPETSHOP_ADD;
	    break;
	  case 14: // 取出仓库列表
	  	if( !CHAR_CheckDepotPet( toindex) )
	  		return;
		NPC_DepotPet_List( meindex, toindex, token, &buttontype, select);
	  	//buttontype=WINDOW_BUTTONTYPE_CANCEL;
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_GET;
		break;

	  case 15: // 询问确认存入仓库
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

	  case 16: // 询问确认取出仓库
		NPC_DepotPet_AskGet( meindex, toindex, select, token);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
	  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		windowno = CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKGET;
	    break;

	  case 17: // 确认存入仓库
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

	  case 18: // 确认取出仓库
	  	if( CHAR_getWorkInt( meindex, CHAR_WORK_POOLFLG ) != 1 )
	  		return;	  
		if( NPC_DepotPet_CheckLevel( meindex, toindex, select, token) == TRUE) {
			NPC_DepotPet_Get( meindex, toindex, select, token);
			buttontype = WINDOW_BUTTONTYPE_OK;
	  		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			windowno = CHAR_WINDOWTYPE_WINDOWPETSHOP_END; 
		}
		else {
			sprintf( token, "\n你无法照顾这只宠物。");
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
	
	/*--巨旦弗□皿--*/
	//makeEscapeString( token, escapedname, sizeof(escapedname));
		
		
	/*--霜耨--*/
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
			
			if( ( datanum == 3 && poolflg) || // 卖宠物
				( datanum == 2 && !poolflg ) ){	// 领取宠物
				NPC_PetShop_selectWindow( meindex, talkerindex, 1, -1 );
			}else if( datanum == 1 && poolflg) { // 寄放宠物 
				if( CHAR_getCharPoolPetElement( talkerindex) != -1 ) {
					NPC_PetShop_selectWindow( meindex, talkerindex, 5, -1 );
				}
				else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 8, -1 );
				}
			}else if( datanum == 2 && poolflg ) { // 领取宠物
				if( CHAR_getCharPetElement( talkerindex) != -1 ) {
					CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
					NPC_PetShop_selectWindow( meindex, talkerindex, 9, 0);
				}else {
					NPC_PetShop_selectWindow( meindex, talkerindex, 12, -1 );
				}
			}
#ifdef _NPC_DEPOTPET
			else if( datanum == 4 ) { // 使用宠物仓库
				print(" 使用宠物仓库 ");
				CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
				if( !CHAR_CheckDepotPet( talkerindex) ){
					CHAR_GetDepotPet( meindex, talkerindex);
					CHAR_talkToCli( talkerindex, -1, "取得宠物，请稍後！", CHAR_COLORYELLOW);
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
		  case 1://放入
				if( CHAR_findEmptyDepotPet( talkerindex) == -1 ){
					CHAR_talkToCli( talkerindex, -1, "宠物仓库已满！", CHAR_COLORYELLOW);
					return;
				}
				//NPC_DepotPet_Pet_printWindow( meindex, talkerindex);
				NPC_PetShop_selectWindow( meindex, talkerindex, 13, -1 );
			  break;
		  case 2://取出
				if( CHAR_getCharPetElement( talkerindex) < 0 ) {
					CHAR_talkToCli( talkerindex, -1, "身上宠物栏位已满！", CHAR_COLORYELLOW);
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
	  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKADD: // 确认存入仓库
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetShop_selectWindow( meindex, talkerindex, 17,-1 );
		}else {
			NPC_PetShop_selectWindow( meindex, talkerindex,13,-1);
	  	}
		break;
			
	  case CHAR_WINDOWTYPE_DEPOTPETSHOP_ASKGET: // 确认取出仓库
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

	/*--伐□弁及赓渝祭--*/
	CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,select);

	petindex = CHAR_getCharPet( talker, select-1);
        

   if( !CHAR_CHECKINDEX(petindex) ) return;

  	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_REALY, buf);

	cost=NPC_GetCost(meindex,talker,petindex);

	petname = CHAR_getUseName( petindex);

	sprintf(token,"\n是[%s]没错吧？""%s"
					"\n\n    买卖价格         %d STONE\n"
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
	sprintf(token,"\n寄放[%s]"
					"\n\n    寄放价格            %d stone\n"
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
	sprintf(token,"\n领回[%s]", petname);	
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
	
	/*--袄蟾毛菲户月--*/
	levelcost=(level*level)*10;
	getlevel=(getlevel*getlevel)*10;
// Syu ADD 修改卖转生宠金钱会变负的bug
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
	/*--旦矢扑乓伙伊□玄匹煌遥今木月矢永玄毛喃曰请允  矢永玄及隙烂反  飓  寞)--*/
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )
	{
		if(strstr( buff2, "-")==NULL){
			if(graNo == atoi(buff2)){
				intrate=CHAR_getWorkInt( meindex , CHAR_WORK_SPECIALRATE);
				rate=(double) intrate / 1000;
				break; 
			}
		}else{
			/*--失奶  丞互  15-25  及溥匹嗉濠日木化中月桦宁--*/
			int start;
			int end;
			int tmp;
			int graflg=0;
			char	token2[128];
	
			/*-"-"匹嗉濠日木凶铵户及醒袄午  及醒袄毛潸  --*/
			getStringFromIndexWithDelim( buff2, "-", 1, token2, sizeof(token2));
			start = atoi( token2);
			getStringFromIndexWithDelim( buff2, "-", 2 ,token2, sizeof(token2));
			end = atoi( token2);

			/*--  寞互菅卞卅匀化中凶日｝  木赘尹月**/
			if(start > end){
				tmp=start;
				start=end;
				end=tmp;
			}

			end++;
			/*--"-"匹嗉濠日木凶坌及失奶  丞树  毛  月--*/
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

	/*--矢永玄及镝擦蘸毛      ---*/
	petai=CHAR_getWorkInt(petindex,CHAR_WORKFIXAI);

	/*--    毛必永玄--*/
	charm = CHAR_getWorkInt(talker,CHAR_WORKFIXCHARM);

	/*--      ≈镝擦蘸  / ㄡ  */
	charm = charm + petai;

	if(charm < 20){
		charm = 20;
	}

	charm = charm / 2;

	cost = (int)cost*rate;

	rate=(double)charm/100;

	/*--    镝擦喘仃--*/
	cost = cost * rate;

	/*--云嗯毛筏盛--*/
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
		makeEscapeString("\n\n很抱歉～守护兽无法贩卖喔！\n请自己好好照顾！", buf, sizeof(buf)));
*/
		sprintf(token,"\n\n很抱歉～守护兽无法贩卖喔！\n请自己好好照顾！");
		return;
	}

        // Robin 0725
        if (CHAR_getInt( talker, CHAR_RIDEPET ) == petsel ){	
/*        	char buf[1024];
        	lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
        		WINDOW_BUTTONTYPE_OK,
        		-1, -1,
        		makeEscapeString("\n\n骑乘中的宠物无法贩卖喔！", buf, sizeof(buf)));
*/
					sprintf(token,"\n\n骑乘中的宠物无法贩卖喔！");
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
		CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"sell(卖宠)",
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
	// 矢永玄犯□正  凳［矢永玄      仃  木  
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
        		makeEscapeString("\n\n骑乘中的宠物无法寄放喔！", buf, sizeof(buf)));
*/
					sprintf(token,"\n\n骑乘中的宠物无法寄放喔！");
        	return;
        }
        
	// 坞五毛茧允［蟆卞茧仄化民尼永弁仄化中月及匹］仇仇匹坞中化中卅中及反云井仄中
	emptyindex = CHAR_getCharPoolPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}
	// 爵  赏  迕矢永玄卅日］公木及瓒  毛壅允［
	if( CHAR_getInt( talker, CHAR_DEFAULTPET) == selectpet) {
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}
	//   仃  木  卞］矢永玄及奶件犯永弁旦毛戊疋□
	CHAR_setCharPoolPet( talker, emptyindex, petindex);
	// 矢永玄  井日壅允
	CHAR_setCharPet( talker, selectpet, -1);
	
	// 丢永本□斥潸  
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	// 云嗯蛹日允

	CHAR_DelGold( talker, NPC_GETPOOLCOST( talker) );

	// 云嗯犯□正霜月
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	snprintf( szPet, sizeof( szPet ), "K%d", selectpet);
	// 蕙仄中矢永玄旦  □正旦霜曰勾仃月
	CHAR_sendStatusString( talker, szPet );
	// 矢永玄毛  仃凶夫弘
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"pool(寄放宠物店)",
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

	// 如果是守护兽改为普通兽
	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 
		&& CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 3 ) {
		CHAR_setInt( petindex, CHAR_PETFAMILY, 0);
		CHAR_talkToCli( talker, -1, "守护兽改为普通兽", CHAR_COLORYELLOW);
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
		"draw(领取宠物店)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
#ifdef _PET_EVOLUTION
	if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) == 1 &&
		CHAR_getInt( petindex, CHAR_FUSIONRAISE) > 0 ){//检查是否为融合宠
		CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time( NULL));
	}
#endif
}	

void NPC_getDrawPet( int meindex, int toindex, char *token, int *buttontype, int select)
{
	// ㄠ矢□斥丐凶曰及垫醒
	#define		LINENUM		5
	char buf[1024];
	int i;
	int startnum, endnum;
	int petindex = -1;
	// 蜇箕及矢□斥
	int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
	int limit;
	//andy_reEdit 2003/09/18
	limit = (CHAR_getInt(toindex,CHAR_TRANSMIGRATION)*2)+5;

	endnum = (page + select+1)*LINENUM ;
	startnum = endnum - LINENUM;
	*buttontype = WINDOW_BUTTONTYPE_CANCEL;
	// 羹矢□斥譬帮［
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
	
	strcpy( token, "2\n亖亖亖　请选择宠物　亖亖亖\n\n");
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
	// 戚及域蜊毛譬屯化］  井匀凶日戚尺示正件毛潸月
	if( i != limit ) {
		petindex = CHAR_getCharPoolPet( toindex, i);
		if( !CHAR_CHECKINDEX( petindex)) {
			*buttontype &= ~WINDOW_BUTTONTYPE_NEXT;
		}
	}
}

/*
 *云嗯互左□田□公及职中欠中欠
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
			"          使用宠物仓库\n\n"
			"          ＜存放宠物＞\n"
			"          ＜取回宠物＞\n"
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
	// ㄠ矢□斥丐凶曰及垫醒
	#define		LINENUM		5
	char buf[1024];
	int i;
	int startnum, endnum;
	int petindex = -1;
	// 蜇箕及矢□斥
	int page = CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT);
	int limit;

	limit = CHAR_MAXDEPOTPETHAVE;

	endnum = (page + select+1)*LINENUM ;
	startnum = endnum - LINENUM;
	*buttontype = WINDOW_BUTTONTYPE_CANCEL;
	// 羹矢□斥譬帮［
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
	
	strcpy( token, "2\n亖亖亖　请选择要从仓库取出的宠物　亖亖亖\n\n");
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
	// 戚及域蜊毛譬屯化］  井匀凶日戚尺示正件毛潸月
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
	// 矢永玄犯□正  凳［矢永玄      仃  木  
	selectpet = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) -1;
	//print("\n selectpet:%d ", selectpet);
	petindex = CHAR_getCharPet( talker, selectpet);
	if( !CHAR_CHECKINDEX( petindex) ) return;
	
    // Robin 0725
    if (CHAR_getInt( talker, CHAR_RIDEPET ) == selectpet )
    {
		sprintf(token,"\n\n骑乘中的宠物无法存入仓库喔！");
		return;
	}

	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 )
	{
		sprintf(token,"\n\n家族守护兽无法存入仓库喔！");
		return;
	}

#ifdef _CHECK_ENEMY_PET
	{
		int j;
		for( j=0;j<ENEMYTEMP_getEnemyNum();j++ ) {//ENEMY_getEnemyNum()
			if( CHAR_getInt( petindex, CHAR_PETID) == ENEMYTEMP_getInt( j, E_T_TEMPNO ) ) {//ENEMY_getInt( j, ENEMY_TEMPNO)
				if( ENEMYTEMP_getInt( j, E_T_PETFLG ) == 0 ){//ENEMY_getInt( j, ENEMY_PETFLG) 
				    sprintf(token,"\n\n这是不能持有的宠物喔！");
					return;
				}
			}
		}
	}
#endif    
	// 坞五毛茧允［蟆卞茧仄化民尼永弁仄化中月及匹］仇仇匹坞中化中卅中及反云井仄中
	emptyindex = CHAR_getCharDepotPetElement( talker);
	if( emptyindex == -1 ) {
		fprint( "err\n");
		return;
	}

	// 爵  赏  迕矢永玄卅日］公木及瓒  毛壅允［
	if( CHAR_getInt( talker, CHAR_DEFAULTPET) == selectpet) {
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}
	//   仃  木  卞］矢永玄及奶件犯永弁旦毛戊疋□
	CHAR_setDepotPetIndex( talker, emptyindex, petindex);
	// 矢永玄  井日壅允
	CHAR_setCharPet( talker, selectpet, -1);
	
	// 丢永本□斥潸  
	NPC_MaxGoldOver( meindex, talker, NPC_PETSHOP_MSG_POOLTHANKS, token);
	// 云嗯蛹日允

	CHAR_DelGold( talker, NPC_GETPOOLCOST( talker) );

	// 云嗯犯□正霜月
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	snprintf( szPet, sizeof( szPet ), "K%d", selectpet);
	// 蕙仄中矢永玄旦  □正旦霜曰勾仃月
	CHAR_sendStatusString( talker, szPet );
	// 矢永玄毛  仃凶夫弘
	if( NPC_DepotPet_CheckRepeat( talker) == 0)  // 检查编码重复宠
	{
		LogPet(
			CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( talker, CHAR_CDKEY ),
			CHAR_getChar( petindex, CHAR_NAME),
			CHAR_getInt( petindex, CHAR_LV),
			"pool(存入宠物仓库)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
			CHAR_getInt( talker,CHAR_Y ),
			CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
		);
	}
	else
	{
		CHAR_talkToCli( talker, -1, "这是非法的宠物喔!!", CHAR_COLORYELLOW);
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
		"draw(领取仓库宠物)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
#ifdef _PET_EVOLUTION
	if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) == 1 &&
		CHAR_getInt( petindex, CHAR_FUSIONRAISE) > 0 ){//检查是否为融合宠
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
	sprintf(token,"\n领回[%s]", petname);	
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
				print(" 在仓库找到非法宠了 ");
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
		CHAR_getChar( charaindex, CHAR_NAME ), /* 平乓仿   */
		CHAR_getChar( charaindex, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"del(清除仓库洗宠)",
		CHAR_getInt( charaindex,CHAR_FLOOR),
		CHAR_getInt( charaindex,CHAR_X ),
		CHAR_getInt( charaindex,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
		);

	CHAR_endCharOneArray( petindex );

}

// 检查等级是否足够
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

	// 如果是守护兽改为普通兽
	if( CHAR_getInt( petindex, CHAR_PETFAMILY) == 1 
		&& CHAR_getInt( talker, CHAR_FMLEADERFLAG) != 3 ) {
		CHAR_setInt( petindex, CHAR_PETFAMILY, 0);
	}

	if( CHAR_getWorkInt( talker, CHAR_PickAllPet) == TRUE ) { // 装备驯兽戒指的话...
		return TRUE;
	}

	if( CHAR_getInt( talker, CHAR_TRANSMIGRATION) > 0 
		|| CHAR_getInt( talker, CHAR_LV)+5 >= CHAR_getInt( petindex, CHAR_LV) ) {
		return TRUE;
	}
	
	return FALSE;
	
}

#endif

