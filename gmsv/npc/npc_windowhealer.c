#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_windowhealer.h"



/*--怂    ---*/
#define RATE  1000


/* 
 * 必醒書直俔万笛周囁万谷萩塋NPC
 * 棵哑  平旦玄失玉矛件民乓□仁日中卅日综木月井手［
 *
 */

enum {
	CHAR_WORK_LEVEL	= CHAR_NPCWORKINT1,
	CHAR_WORK_RANGE	= CHAR_NPCWORKINT2,
	CHAR_WORK_HP	= CHAR_NPCWORKINT3,
	CHAR_WORK_MP	= CHAR_NPCWORKINT4,

};


static void NPC_WindowHealer_selectWindow( int meindex, int toindex, int num);
void NPC_WindowHealerAllHeal( int talker,int mode );
BOOL NPC_WindowHealerLevelCheck(int meindex,int talker);
BOOL NPC_WindowMoneyCheck(int meindex,int talker,int mode);
int NPC_WindowCostCheck(int meindex,int talker);
BOOL NPC_PetHealerCheck(int talker);
int NPC_WindowCostCheckMp(int meindex,int talker);


/*********************************
* 赓渝质  
*********************************/
BOOL NPC_WindowHealerInit( int meindex )
{

	char *npcarg;
	char buf2[256];
	int range=1;
	int rate;
	double drate;
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEHEALER );

    npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

	/*--记住回复时收取金钱的等级--*/
	/*--  没有的话不会被创建--*/
	if(getStringFromIndexWithDelim(npcarg,"|",1,buf2,sizeof(buf2))!=FALSE){
		CHAR_setWorkInt(meindex,CHAR_WORK_LEVEL,atoi(buf2));

	}else{
		return FALSE;
	}
	/*--耐久力倍率--*/
	if(getStringFromIndexWithDelim(npcarg, "|", 2, buf2, sizeof( buf2)) != FALSE ){
		drate = atof(buf2);
		if(drate==0) {
			rate=500;
		}else{
			rate=(int) (drate * RATE);
		}
		CHAR_setWorkInt(meindex,CHAR_WORK_HP,rate);
	}
	if(getStringFromIndexWithDelim(npcarg, "|", 3, buf2, sizeof( buf2)) != FALSE ){
		drate = atof(buf2);
		if(drate==0){
			rate=2000;
		}else{
			rate=(int)( drate * RATE);
		}
		CHAR_setWorkInt(meindex,CHAR_WORK_MP,rate);
	}
	if(getStringFromIndexWithDelim(npcarg, "|", 4, buf2, sizeof( buf2)) != FALSE ){
		range=atoi(buf2);
		if(range == 0){
			range=1;
		}
	}
	CHAR_setWorkInt(meindex,CHAR_WORK_RANGE,range);
    return TRUE;

}

void NPC_WindowHealerTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	if( NPC_Util_CharDistance( talkerindex, meindex ) 
		> CHAR_getWorkInt(meindex,CHAR_WORK_RANGE)){
		return;
	}
	if( (CHAR_getWorkInt(talkerindex,CHAR_WORKPARTYMODE) == 0)
	 || (CHAR_getWorkInt(talkerindex,CHAR_WORKPARTYMODE) == 2) ){
		NPC_WindowHealer_selectWindow( meindex, talkerindex,0);
	}else{
		int i=0;
		int otherindex;

		for( i=0 ; i < CHAR_PARTYMAX ;i++){
			otherindex=CHAR_getWorkInt(talkerindex,CHAR_WORKPARTYINDEX1+i);
			if(otherindex != -1){
				NPC_WindowHealer_selectWindow( meindex, otherindex,0);
			}
		}
	}
}
void NPC_WindowHealerLooked( int meindex , int lookedindex)
{
    if( CHAR_getInt( lookedindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	if( NPC_Util_CharDistance( lookedindex, meindex ) 
		> CHAR_getWorkInt(meindex,CHAR_WORK_RANGE)){
		 return;
	}
	NPC_WindowHealer_selectWindow( meindex, lookedindex,0);
}

static void NPC_WindowHealer_selectWindow( int meindex, int toindex, int num)
{

	char token[1024];
	char escapedname[1024];
	int fd = getfdFromCharaIndex( toindex);
	int buttontype=0;
	int windowtype=0;
	int windowno=0;

	/*--窗口类型讯息较多所以先设置--*/
  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;

	switch( num){
	  case 0:
  		/*--蓟      --*/
		sprintf(token," 　　　哎呀！你受伤了吗？　 \n\n "
				  "　　　　　　 ＜ 耐久力回复＞　　　　　 \n"
					" 　　　　　　＜  气力回复 ＞　　　　　 \n"
					" 　　　　 ＜ 耐久力·气力回复 ＞　　 \n\n"
				  " 　　　　　 ＜宠物回复(免费)＞　　　　 "
			);

	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_SELECT;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG; 
	  	break;


	  case 1:
		if( CHAR_getInt(toindex,CHAR_HP) ==CHAR_getWorkInt( toindex, CHAR_WORKMAXHP)){
			if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,
				"　　　　　　 ＜　耐久力回复　＞"
				"\n\n\n\n　　　　似乎没有必要回复唷！　");
		  	}else{
		  		sprintf(token,
				"　　　　　　 ＜　耐久力回复　＞"
				"\n\n　　　　      似乎没有必要回复唷！　"
					  "\n\n　　　因为宠物好像也受伤了！"
					  "\n                            先帮他回复吧！");
		  	
		  		NPC_WindowHealerAllHeal(toindex,0 );
		  	}

		  	buttontype=WINDOW_BUTTONTYPE_OK;
		  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG; 
			break;
		}else if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"　　　　　　 ＜　耐久力回复　＞"
					"\n\n\n　　　　　　　是要回复耐久力没错吧！　　　 "
					"\n\n　现在的等级可以免费帮你回复唷！");
		
		}else{
			int gold;
			gold=NPC_WindowCostCheck(meindex,toindex);
			sprintf(token,"　　　　　　 ＜　耐久力回复　＞"
				"\n\n\n　　　　　　　是要回复耐久力没错吧！　　　 "
				 "\n\n　　　　　收您%d的STONE 。"
				 ,gold);
		}

	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG; 
	  	break;

	  case 2:
		if( CHAR_getInt(toindex,CHAR_MP) ==CHAR_getWorkInt( toindex, CHAR_WORKMAXMP)){
			if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,  "　　　　　　  ＜　气力回复　＞"
					"\n\n\n\n　　　　似乎没有必要回复唷！　");
			}else{
			sprintf(token,  "　　　　　　  ＜　气力回复　＞"
					"\n\n　　　　      似乎没有必要回复唷！　"
					  "\n\n　　　因为宠物好像也受伤了！"
					  "\n                            先帮他回复吧！");
				NPC_WindowHealerAllHeal(toindex,0 );
			}
		  	buttontype=WINDOW_BUTTONTYPE_OK;
		  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
			break;
		}

		if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"　　　　　　  ＜ 气力回复＞"
					  "\n\n　　　　　　　是要回复气力没错吧！　　　　 "
					  "\n\n 现在的等级可以免费帮你回复唷！");
		}else{
			int cost;
			cost=NPC_WindowCostCheckMp(meindex,toindex);
			sprintf(token,"　　　　　　  ＜ 气力回复＞"
					"\n\n\n　　　　　　　是要回复气力没错吧！ "
 					  "\n\n　　　　收您%d的STONE 。",
					cost);
		}
	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
	  	break;


	  case 3:
		sprintf(token,	    "\n　　　　　　＜耐久力已回复＞"
						    "\n　　  ＜宠物也已回复一般状态＞"
						"\n\n\n　　　　　  这样子就没问题了！　　　　");

		/*--觐菁  及心荚汊今六月--*/
	  	NPC_WindowHealerAllHeal( toindex ,1 );
	  	buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;


	  case 4:
		sprintf(token,	    "\n　　　　　　＜气力已回复＞"
						    "\n　　 ＜宠物也已回复一般状态＞"
						"\n\n\n　　　　　 这样子就没问题了！ 　　　");
		/*--竣  及心荚汊今六月---*/
	  	NPC_WindowHealerAllHeal( toindex ,2 );
	 	buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG; 
	  	break;


	  case 6:
		{
			int cost=0;

			if(CHAR_getInt(toindex,CHAR_HP) < CHAR_getWorkInt( toindex, CHAR_WORKMAXHP))
			{
				cost+=NPC_WindowCostCheck(meindex,toindex);
			}

			if( CHAR_getInt(toindex,CHAR_MP) < CHAR_getWorkInt( toindex, CHAR_WORKMAXMP))
			{
				cost=cost+NPC_WindowCostCheckMp(meindex,toindex);
			}
		
			if(cost==0) {
				if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,
						"　　　　  ＜　耐久力气力回复　＞"
				"\n\n\n\n　　　　似乎没有必要回复唷！　");
				}else{
				sprintf(token,"　　　　  ＜　耐久力气力回复　＞"
					"\n\n　　　　      似乎没有必要回复唷！　"
					  "\n\n　　　因为宠物好像也受伤了！"
					  "\n                            先帮他回复吧！");
				NPC_WindowHealerAllHeal(toindex,0 );
			}
		
				
				
			  	buttontype=WINDOW_BUTTONTYPE_OK;
			  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
			  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
				break;

			}else{
				sprintf(token,"　　　  ＜　耐久力气力回复　＞"
						"\n\n\n　　　　 是要回复耐久力气力没错吧！"
						"\n\n　　　　　收您%d的STONE 。",cost);
			}
		}

		if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"　　　　  ＜　耐久力气力回复　＞"
					"\n\n\n　　　　 　是要回复耐久力气力没错吧！"
					"\n\n　现在的等级可以免费帮你回复唷！");
		}
	
	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG; 
	  	break;


	  case 7:
		sprintf(token,"　　　　 ＜耐久力气力已回复＞"
				  "\n　　  ＜宠物也已回复一般状态＞"
				  "\n\n\n　　　　这样一来就回复健康了！");
	  	
	  	NPC_WindowHealerAllHeal( toindex ,3 );
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG; 
	  	break;


	  case 8:
		sprintf(token,"\n\n\n\n 　　真可惜似乎所带的金钱不够唷！");

		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;

	  case 9:
		sprintf(token,"　　　　　　　＜宠物回复＞　　　　　　"
				"\n\n\n　　　　　　　已经没问题了啦！　　　　　"
				  "\n\n　　但是太勉强的话也不行唷！　　");
		NPC_WindowHealerAllHeal(toindex,0 );
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;

	  case 10:
		sprintf(token,"　　　　　　　＜宠物回复＞　　　　　　"
				"\n\n\n　 似乎没有必要回复宠物的样子。　"
				  "\n\n　　但是太勉强的话也不行唷！　　");
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;


	}


	makeEscapeString( token, escapedname, sizeof(escapedname));
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
void NPC_WindowHealerWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{

	if( NPC_Util_CharDistance( talkerindex, meindex ) > (CHAR_getWorkInt(meindex,CHAR_WORK_RANGE)+4)) return;

	switch( seqno){
	  case CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG:
	  	if(atoi(data)==2)	/*--蓟  仄凶嫩   1--*/
	  	{
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 1 );
	  	
	  	}else if(atoi(data)==3){
	  		/*--蓟  仄凶嫩   2--*/
	  		NPC_WindowHealer_selectWindow( meindex, talkerindex, 2 );
	  	
	  	}else if(atoi(data)==4){
	  		/*--蓟  仄凶嫩   2--*/
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 6 );
		
		}else if(atoi(data)==6){
			/*--蓟  仄凶嫩   2--*/
	  		if(NPC_PetHealerCheck( talkerindex)==TRUE){
		  		NPC_WindowHealer_selectWindow( meindex, talkerindex, 9 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 10 );
			}
		}else if(select==WINDOW_BUTTONTYPE_CANCEL){
			  	/*--仇仇引匹仁月午蔽  卅及匹窒手仄卅中--*/
	  	}
	  	break;
	  

	  /*-----进行耐久力回复-----*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			/*--云嗯及民尼永弁--*/
			if(NPC_WindowMoneyCheck(meindex,talkerindex,1)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 3 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );

		}
		break;


	  /*-----进行气力回复-----*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			/*--云嗯及民尼永弁--*/
			if(NPC_WindowMoneyCheck(meindex,talkerindex,2)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 4 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		
		break;


	  /*-----耐久力回复的结果-----*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;


	  case CHAR_WINDOWTYPE_WINDOWHEALER_OKSPIRITMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;
	

	  /*--蝈荚汊今六月--*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			if(NPC_WindowMoneyCheck(meindex,talkerindex,3)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 7 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;
	

	  /*--全回复的结果--*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG:
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		break;
	}

	if(select==WINDOW_BUTTONTYPE_CANCEL 
	&& CHAR_getWorkInt( talkerindex, CHAR_WORKPARTYMODE ) == 2)
	{
		CHAR_sendWatchEvent( CHAR_getWorkInt( talkerindex, CHAR_WORKOBJINDEX),
								CHAR_ACTPLEASURE,NULL,0,TRUE);
		CHAR_setWorkInt( talkerindex, CHAR_WORKACTION, CHAR_ACTPLEASURE);
	}

}


/*--辎勾中化中月及卅日｝TRUE毛忒允--*/
BOOL NPC_PetHealerCheck(int talker)
{

	int petindex;
	int i;
	
	/*--矢永玄反云引仃匹荚汊仄化丐仆月--*/
	for(i=0;i<CHAR_MAXPETHAVE;i++){
    	petindex = CHAR_getCharPet(talker,i);

		if( petindex == -1  )  continue;

		/*  平乓仿及    民尼永弁    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;
		
		
		if( CHAR_getInt(petindex,CHAR_HP) != CHAR_getWorkInt(petindex,CHAR_WORKMAXHP) )
		{
			return TRUE;
		}
	}
	
	return FALSE;

}


/*-------------------------------------
	  实际回复耐久力和气力的地方
---------------------------------------*/
void NPC_WindowHealerAllHeal( int talker,int mode )
{
	int i;
	int petindex;
	char petsend[64];	
	char msgbuf[64];

	/*--觐菁  及心荚汊--*/
	if(mode==1){
		/*--HP就是耐久力--*/
    	CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
	}else if(mode==2){
		/*--竣  及心荚汊--*/
		/*--MP互竣  卅及匹丐月--*/
    	CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );
	}else if(mode==3){
    	/*--鱚者竄---*/
    	CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
    	CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );
	}

	/*--矢永玄反云引仃匹荚汊仄化丐仆月--*/
	for(i=0;i<CHAR_MAXPETHAVE;i++){
    	petindex = CHAR_getCharPet(talker,i);

		if( petindex == -1  )  continue;

	   /*  平乓仿及    民尼永弁    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;

		/*--者竄--*/
		CHAR_setFlg( petindex,CHAR_ISDIE,0 );
		CHAR_setInt( petindex , CHAR_HP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) );
		CHAR_setInt( petindex , CHAR_MP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXMP ) );

		/*--由仿丢□正譬帮--*/
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );

	}

	/*--由仿丢□正霜曰--*/
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_HP);
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_MP);


	/*---有队友的话也发给队友--*/
	if(CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE )
	{
		int	topartyarray = -1;
		int	oyaindex = CHAR_getWorkInt( talker	, CHAR_WORKPARTYINDEX1);

		if( CHAR_CHECKINDEX( oyaindex )) {
			int		i;
		
			/* 愤坌午怂仄凶支勾及醮棉及    及桦赭毛潸   */
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int workindex = CHAR_getWorkInt( oyaindex, CHAR_WORKPARTYINDEX1 +i);
		
				if( CHAR_CHECKINDEX( workindex) ) {
					if( workindex == talker ) {
						topartyarray = i;
						break;
					}
				}
			}
		
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int otherindex = CHAR_getPartyIndex( talker, i);
		
				/* 醮棉由仿丢□正毛霜月 */
				if( CHAR_CHECKINDEX( otherindex) ) {
					snprintf( msgbuf, sizeof( msgbuf), "N%d", topartyarray);
					if( otherindex != talker) {
						CHAR_sendStatusString( otherindex, msgbuf);
					}
				}
			}
		}
	}

}


/*-------------------------------------
等级检查
看等级,低于设置等级的话设为有效
  曰袄
	必醒卅狸誌    		TRUE
	设置等级以上则	FALSE
---------------------------------------*/
BOOL NPC_WindowHealerLevelCheck(int meindex,int talker)
{
		int level;
	
		level=CHAR_getWorkInt(meindex,CHAR_WORK_LEVEL);

		if(level > CHAR_getInt(talker,CHAR_LV)){
			return TRUE;
		}

		return FALSE;

}


/*-------------------------------------------
云嗯  匀化月井民尼永弁
引数
meindex・・・      的索引
talker・・・・人物的索引
---------------------------------------------*/
BOOL NPC_WindowMoneyCheck(int meindex,int talker,int mode)
{
	int cost=0;
	int level;
	
	level=CHAR_getWorkInt(meindex,CHAR_WORK_LEVEL);
	
	if(mode==1){
		if(level <= CHAR_getInt(talker,CHAR_LV)){
			/*---计算金钱---*/
			cost=NPC_WindowCostCheck(meindex,talker);
			/*--蜇箕反移涩烂---*/
			/*---检查金钱是否足够---*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			
			}
			CHAR_DelGold( talker, cost);
		}
	}
	if(mode==2){
		if(level <= CHAR_getInt(talker,CHAR_LV)){
			/*---计算金钱---*/
			cost=NPC_WindowCostCheckMp(meindex,talker);
	
			/*--蜇箕反移涩烂---*/
			/*---检查金钱是否足够---*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			}
			CHAR_DelGold( talker, cost);
		}
	}

	if(mode==3){
		if(level <= CHAR_getInt(talker,CHAR_LV)){

			/*--耐久力的金钱计算--*/
			if( CHAR_getInt(talker,CHAR_HP) <CHAR_getWorkInt( talker, CHAR_WORKMAXHP))
			{
				cost=NPC_WindowCostCheck(meindex,talker);
			}

			/*--竣  及云嗯及煌遥--*/
			if( CHAR_getInt(talker,CHAR_MP) < CHAR_getWorkInt( talker, CHAR_WORKMAXMP))
			{
				cost=cost+NPC_WindowCostCheckMp(meindex,talker);
			}
		
			/*--移涩烂--*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			}
		
			/*---减少金钱---*/
			CHAR_DelGold( talker, cost);
		}
		
	}
	
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	return TRUE;

}



/*-----------------------------------------
计算成本
------------------------------------------*/
int NPC_WindowCostCheck(int meindex,int talker)
{
	int cost;
	double drate;
	int rate;
	
	rate=CHAR_getWorkInt(meindex,CHAR_WORK_HP);
	drate=(double) rate / 1000 ;
	cost=CHAR_getInt(talker,CHAR_LV);
	cost=(int)cost*drate;
	if(cost < 1) cost=1;
	return cost;
}

/*-----------------------------------------
计算气力成本
------------------------------------------*/
int NPC_WindowCostCheckMp(int meindex,int talker)
{
	int cost;
	double drate;
	int rate;
	
	rate=CHAR_getWorkInt(meindex,CHAR_WORK_MP);
	drate=(double) rate / 1000 ;
	cost=CHAR_getInt(talker,CHAR_LV);
	cost=(int)cost*drate;
	if(cost==0) cost=1;
	return cost;
}
