#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_windowhealer.h"



/*--佫    ---*/
#define RATE  1000


/* 
 * 优擭踏躂倜坳贖璃迶坳禱③埰NPC
 * 螢挳  す筒哱囮迶穫璃鏍籤↓�岓欶倠牟欳裗戰職晉痑�
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
* 疐趵窐  
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

	/*--樊蜾埰堎敁拻摯堁鉣禱噁堎畛穫鳴禱楔呏--*/
	/*--  崹躂�屺袾桫撻聿�--*/
	if(getStringFromIndexWithDelim(npcarg,"|",1,buf2,sizeof(buf2))!=FALSE){
		CHAR_setWorkInt(meindex,CHAR_WORK_LEVEL,atoi(buf2));

	}else{
		return FALSE;
	}
	/*--膰敯  畛↓哱--*/
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

	/*--坳騷璃迶坳淏騷鏤隍蚗掛↓喇誑堁堁笢摯ぁ袸勗优擭--*/
  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;

	switch( num){
	  case 0:
  		/*--撒      --*/
		sprintf(token," ﹛﹛﹛陞挼ㄐ斕忳夼賸鎘ˋ﹛ \n\n "
				  "﹛﹛﹛﹛﹛﹛ ˉ 騵壅薯隙葩ˇ﹛﹛﹛﹛﹛ \n"
					" ﹛﹛﹛﹛﹛﹛ˉ  ァ薯隙葩 ˇ﹛﹛﹛﹛﹛ \n"
					" ﹛﹛﹛﹛ ˉ 騵壅薯﹞ァ薯隙葩 ˇ﹛﹛ \n\n"
				  " ﹛﹛﹛﹛﹛ ˉ唾昜隙葩(轎煤)ˇ﹛﹛﹛﹛ "
			);

	  	buttontype=WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype=WINDOW_MESSAGETYPE_SELECT;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG; 
	  	break;


	  case 1:
		if( CHAR_getInt(toindex,CHAR_HP) ==CHAR_getWorkInt( toindex, CHAR_WORKMAXHP)){
			if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,
				"﹛﹛﹛﹛﹛﹛ ˉ﹛騵壅薯隙葩﹛ˇ"
				"\n\n\n\n﹛﹛﹛﹛侔綱羶衄斛猁隙葩遄ㄐ﹛");
		  	}else{
		  		sprintf(token,
				"﹛﹛﹛﹛﹛﹛ ˉ﹛騵壅薯隙葩﹛ˇ"
				"\n\n﹛﹛﹛﹛      侔綱羶衄斛猁隙葩遄ㄐ﹛"
					  "\n\n﹛﹛﹛秪峈唾昜疑砉珩忳夼賸ㄐ"
					  "\n                            珂堆坻隙葩勘ㄐ");
		  	
		  		NPC_WindowHealerAllHeal(toindex,0 );
		  	}

		  	buttontype=WINDOW_BUTTONTYPE_OK;
		  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG; 
			break;
		}else if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"﹛﹛﹛﹛﹛﹛ ˉ﹛騵壅薯隙葩﹛ˇ"
					"\n\n\n﹛﹛﹛﹛﹛﹛﹛岆猁隙葩騵壅薯羶渣勘ㄐ﹛﹛﹛ "
					"\n\n﹛珋婓腔脹撰褫眕轎煤堆斕隙葩遄ㄐ");
		
		}else{
			int gold;
			gold=NPC_WindowCostCheck(meindex,toindex);
			sprintf(token,"﹛﹛﹛﹛﹛﹛ ˉ﹛騵壅薯隙葩﹛ˇ"
				"\n\n\n﹛﹛﹛﹛﹛﹛﹛岆猁隙葩騵壅薯羶渣勘ㄐ﹛﹛﹛ "
				 "\n\n﹛﹛﹛﹛﹛彶蠟%d腔STONE ﹝"
				 ,gold);
		}

	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG; 
	  	break;

	  case 2:
		if( CHAR_getInt(toindex,CHAR_MP) ==CHAR_getWorkInt( toindex, CHAR_WORKMAXMP)){
			if(NPC_PetHealerCheck(toindex)==FALSE){
				sprintf(token,  "﹛﹛﹛﹛﹛﹛  ˉ﹛ァ薯隙葩﹛ˇ"
					"\n\n\n\n﹛﹛﹛﹛侔綱羶衄斛猁隙葩遄ㄐ﹛");
			}else{
			sprintf(token,  "﹛﹛﹛﹛﹛﹛  ˉ﹛ァ薯隙葩﹛ˇ"
					"\n\n﹛﹛﹛﹛      侔綱羶衄斛猁隙葩遄ㄐ﹛"
					  "\n\n﹛﹛﹛秪峈唾昜疑砉珩忳夼賸ㄐ"
					  "\n                            珂堆坻隙葩勘ㄐ");
				NPC_WindowHealerAllHeal(toindex,0 );
			}
		  	buttontype=WINDOW_BUTTONTYPE_OK;
		  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
			break;
		}

		if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"﹛﹛﹛﹛﹛﹛  ˉ ァ薯隙葩ˇ"
					  "\n\n﹛﹛﹛﹛﹛﹛﹛岆猁隙葩ァ薯羶渣勘ㄐ﹛﹛﹛﹛ "
					  "\n\n 珋婓腔脹撰褫眕轎煤堆斕隙葩遄ㄐ");
		}else{
			int cost;
			cost=NPC_WindowCostCheckMp(meindex,toindex);
			sprintf(token,"﹛﹛﹛﹛﹛﹛  ˉ ァ薯隙葩ˇ"
					"\n\n\n﹛﹛﹛﹛﹛﹛﹛岆猁隙葩ァ薯羶渣勘ㄐ "
 					  "\n\n﹛﹛﹛﹛彶蠟%d腔STONE ﹝",
					cost);
		}
	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
	  	break;


	  case 3:
		sprintf(token,	    "\n﹛﹛﹛﹛﹛﹛ˉ騵壅薯眒隙葩ˇ"
						    "\n﹛﹛  ˉ唾昜珩眒隙葩珨啜袨怓ˇ"
						"\n\n\n﹛﹛﹛﹛﹛  涴欴赽憩羶恀枙賸ㄐ﹛﹛﹛﹛");

		/*--膰敯  摯陑樊蜾踏鞠堎--*/
	  	NPC_WindowHealerAllHeal( toindex ,1 );
	  	buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;


	  case 4:
		sprintf(token,	    "\n﹛﹛﹛﹛﹛﹛ˉァ薯眒隙葩ˇ"
						    "\n﹛﹛ ˉ唾昜珩眒隙葩珨啜袨怓ˇ"
						"\n\n\n﹛﹛﹛﹛﹛ 涴欴赽憩羶恀枙賸ㄐ ﹛﹛﹛");
		/*--縈  摯陑樊蜾踏鞠堎---*/
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
						"﹛﹛﹛﹛  ˉ﹛騵壅薯ァ薯隙葩﹛ˇ"
				"\n\n\n\n﹛﹛﹛﹛侔綱羶衄斛猁隙葩遄ㄐ﹛");
				}else{
				sprintf(token,"﹛﹛﹛﹛  ˉ﹛騵壅薯ァ薯隙葩﹛ˇ"
					"\n\n﹛﹛﹛﹛      侔綱羶衄斛猁隙葩遄ㄐ﹛"
					  "\n\n﹛﹛﹛秪峈唾昜疑砉珩忳夼賸ㄐ"
					  "\n                            珂堆坻隙葩勘ㄐ");
				NPC_WindowHealerAllHeal(toindex,0 );
			}
		
				
				
			  	buttontype=WINDOW_BUTTONTYPE_OK;
			  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
			  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG; 
				break;

			}else{
				sprintf(token,"﹛﹛﹛  ˉ﹛騵壅薯ァ薯隙葩﹛ˇ"
						"\n\n\n﹛﹛﹛﹛ 岆猁隙葩騵壅薯ァ薯羶渣勘ㄐ"
						"\n\n﹛﹛﹛﹛﹛彶蠟%d腔STONE ﹝",cost);
			}
		}

		if(NPC_WindowHealerLevelCheck(meindex,toindex)==TRUE){
			sprintf(token,"﹛﹛﹛﹛  ˉ﹛騵壅薯ァ薯隙葩﹛ˇ"
					"\n\n\n﹛﹛﹛﹛ ﹛岆猁隙葩騵壅薯ァ薯羶渣勘ㄐ"
					"\n\n﹛珋婓腔脹撰褫眕轎煤堆斕隙葩遄ㄐ");
		}
	
	  	buttontype=WINDOW_BUTTONTYPE_YESNO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_ALLMSG; 
	  	break;


	  case 7:
		sprintf(token,"﹛﹛﹛﹛ ˉ騵壅薯ァ薯眒隙葩ˇ"
				  "\n﹛﹛  ˉ唾昜珩眒隙葩珨啜袨怓ˇ"
				  "\n\n\n﹛﹛﹛﹛涴欴珨懂憩隙葩翩艙賸ㄐ");
	  	
	  	NPC_WindowHealerAllHeal( toindex ,3 );
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKALLMSG; 
	  	break;


	  case 8:
		sprintf(token,"\n\n\n\n ﹛﹛淩褫洇侔綱垀湍腔踢ヴ祥劂遄ㄐ");

		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;

	  case 9:
		sprintf(token,"﹛﹛﹛﹛﹛﹛﹛ˉ唾昜隙葩ˇ﹛﹛﹛﹛﹛﹛"
				"\n\n\n﹛﹛﹛﹛﹛﹛﹛眒冪羶恀枙賸徽ㄐ﹛﹛﹛﹛﹛"
				  "\n\n﹛﹛筍岆怮辭Ч腔趕珩祥俴遄ㄐ﹛﹛");
		NPC_WindowHealerAllHeal(toindex,0 );
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;

	  case 10:
		sprintf(token,"﹛﹛﹛﹛﹛﹛﹛ˉ唾昜隙葩ˇ﹛﹛﹛﹛﹛﹛"
				"\n\n\n﹛ 侔綱羶衄斛猁隙葩唾昜腔欴赽﹝﹛"
				  "\n\n﹛﹛筍岆怮辭Ч腔趕珩祥俴遄ㄐ﹛﹛");
		buttontype=WINDOW_BUTTONTYPE_OK;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=CHAR_WINDOWTYPE_WINDOWHEALER_OKHPMSG; 
	  	break;


	}


	makeEscapeString( token, escapedname, sizeof(escapedname));
	/*-喫喫ぁ邞嚭埰堎--*/
	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					escapedname);

}


/*-----------------------------------------
袲溘騷囮璃哱凝�氪耽�趙拻倜鄹勗蠙怮③踏躂堎��
-------------------------------------------*/
void NPC_WindowHealerWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{

	if( NPC_Util_CharDistance( talkerindex, meindex ) > (CHAR_getWorkInt(meindex,CHAR_WORK_RANGE)+4)) return;

	switch( seqno){
	  case CHAR_WINDOWTYPE_WINDOWHEALER_STARTMSG:
	  	if(atoi(data)==2)	/*--撒  媃倜囂   1--*/
	  	{
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 1 );
	  	
	  	}else if(atoi(data)==3){
	  		/*--撒  媃倜囂   2--*/
	  		NPC_WindowHealer_selectWindow( meindex, talkerindex, 2 );
	  	
	  	}else if(atoi(data)==4){
	  		/*--撒  媃倜囂   2--*/
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 6 );
		
		}else if(atoi(data)==6){
			/*--撒  媃倜囂   2--*/
	  		if(NPC_PetHealerCheck( talkerindex)==TRUE){
		  		NPC_WindowHealer_selectWindow( meindex, talkerindex, 9 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 10 );
			}
		}else if(select==WINDOW_BUTTONTYPE_CANCEL){
			  	/*--喫喫竘ぁ�彸蟲蝐�  埵摯ぁ笰忒媃埵笢--*/
	  	}
	  	break;
	  

	  /*-----膰敯  樊蜾禱堁喫埵竣-----*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_HPMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			/*--堁鉣摯鏍攝蚗袲--*/
			if(NPC_WindowMoneyCheck(meindex,talkerindex,1)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 3 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );

		}
		break;


	  /*-----縈  樊蜾禱堁喫埵竣-----*/
	  case CHAR_WINDOWTYPE_WINDOWHEALER_SPIRITMSG:
		if(select==WINDOW_BUTTONTYPE_OK){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		
		}else if(select==WINDOW_BUTTONTYPE_YES){
			/*--堁鉣摯鏍攝蚗袲--*/
			if(NPC_WindowMoneyCheck(meindex,talkerindex,2)==TRUE){
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 4 );
			}else{
				NPC_WindowHealer_selectWindow( meindex, talkerindex, 8 );
			}
		
		}else if(select==WINDOW_BUTTONTYPE_NO){
			NPC_WindowHealer_selectWindow( meindex, talkerindex, 0 );
		}
		
		break;


	  /*-----膰敯  樊蜾摯踕堅-----*/
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
	

	  /*--蠈樊蜾踏鞠堎--*/
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
	

	  /*--蠈樊蜾摯踕堅--*/
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


/*--磝僑笢趙笢堎摯埵�掁�TRUE禱蒍埰--*/
BOOL NPC_PetHealerCheck(int talker)
{

	int petindex;
	int i;
	
	/*--妐蚗哱毀堁竘崹ぁ樊蜾媃趙堣ど堎--*/
	for(i=0;i<CHAR_MAXPETHAVE;i++){
    	petindex = CHAR_getCharPet(talker,i);

		if( petindex == -1  )  continue;

		/*  す籤溘摯    鏍攝蚗袲    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;
		
		
		if( CHAR_getInt(petindex,CHAR_HP) != CHAR_getWorkInt(petindex,CHAR_WORKMAXHP) )
		{
			return TRUE;
		}
	}
	
	return FALSE;

}


/*-------------------------------------
	  傷勗膰敯  敁縈  禱樊蜾踏鞠堎敁喫Й
---------------------------------------*/
void NPC_WindowHealerAllHeal( int talker,int mode )
{
	int i;
	int petindex;
	char petsend[64];	
	char msgbuf[64];

	/*--膰敯  摯陑樊蜾--*/
	if(mode==1){
		/*--HP誑膰敯  埵摯ぁ堣堎--*/
    	CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
	}else if(mode==2){
		/*--縈  摯陑樊蜾--*/
		/*--MP誑縈  埵摯ぁ堣堎--*/
    	CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );
	}else if(mode==3){
    	/*--蠈樊蜾---*/
    	CHAR_setInt( talker , CHAR_HP ,CHAR_getWorkInt( talker, CHAR_WORKMAXHP ) );
    	CHAR_setInt( talker , CHAR_MP ,CHAR_getWorkInt( talker, CHAR_WORKMAXMP ) );
	}

	/*--妐蚗哱毀堁竘崹ぁ樊蜾媃趙堣ど堎--*/
	for(i=0;i<CHAR_MAXPETHAVE;i++){
    	petindex = CHAR_getCharPet(talker,i);

		if( petindex == -1  )  continue;

	   /*  す籤溘摯    鏍攝蚗袲    */
		if( !CHAR_CHECKINDEX( talker ) )  continue;

		/*--樊蜾--*/
		CHAR_setFlg( petindex,CHAR_ISDIE,0 );
		CHAR_setInt( petindex , CHAR_HP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ) );
		CHAR_setInt( petindex , CHAR_MP ,CHAR_getWorkInt( petindex, CHAR_WORKMAXMP ) );

		/*--蚕溘隍↓淏ぅ堆--*/
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );

	}

	/*--蚕溘隍↓淏邞堇--*/
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_HP);
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_MP);


	/*---黥蹬誑笢躂�朼棺瑄樀笵近�--*/
	if(CHAR_getWorkInt( talker, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE )
	{
		int	topartyarray = -1;
		int	oyaindex = CHAR_getWorkInt( talker	, CHAR_WORKPARTYINDEX1);

		if( CHAR_CHECKINDEX( oyaindex )) {
			int		i;
		
			/* 猷覕敁佫媃倜盓僑摯黥蹬摯    摯鳹鐎禱噁   */
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

}


/*-------------------------------------
畛穫鳴鏍攝蚗袲
畛穫鳴禱峟趙优擭畛穫鳴源堇黖崹躂��    勗埰堎
  堇偯
	优擭畛穫鳴    		TRUE
	优擭畛穫鳴雄窀埵��	FALSE
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
堁鉣  埱趙堎凝鏍攝蚗袲
礎倳
meindex＊＊＊      摯騷璃溢蚗袲筒
talker＊＊＊＊す籤溘袲淏↓摯騷璃溢蚗袲筒
---------------------------------------------*/
BOOL NPC_WindowMoneyCheck(int meindex,int talker,int mode)
{
	int cost=0;
	int level;
	
	level=CHAR_getWorkInt(meindex,CHAR_WORK_LEVEL);
	
	if(mode==1){
		if(level <= CHAR_getInt(talker,CHAR_LV)){
			/*---堁鉣禱鄎堇③埰---*/
			cost=NPC_WindowCostCheck(meindex,talker);
			/*--藯凜毀痄优擭---*/
			/*---堁鉣誑鵿堇堎凝汔竣凝摯鏍攝蚗袲---*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			
			}
			CHAR_DelGold( talker, cost);
		}
	}
	if(mode==2){
		if(level <= CHAR_getInt(talker,CHAR_LV)){
			/*---堁鉣禱鄎堇③埰---*/
			cost=NPC_WindowCostCheckMp(meindex,talker);
	
			/*--藯凜毀痄优擭---*/
			/*---堁鉣誑鵿堇堎凝汔竣凝摯鏍攝蚗袲---*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			}
			CHAR_DelGold( talker, cost);
		}
	}

	if(mode==3){
		if(level <= CHAR_getInt(talker,CHAR_LV)){

			/*--膰敯  摯堁鉣摯銓猀--*/
			if( CHAR_getInt(talker,CHAR_HP) <CHAR_getWorkInt( talker, CHAR_WORKMAXHP))
			{
				cost=NPC_WindowCostCheck(meindex,talker);
			}

			/*--縈  摯堁鉣摯銓猀--*/
			if( CHAR_getInt(talker,CHAR_MP) < CHAR_getWorkInt( talker, CHAR_WORKMAXMP))
			{
				cost=cost+NPC_WindowCostCheckMp(meindex,talker);
			}
		
			/*--痄优擭--*/
			if(CHAR_getInt(talker,CHAR_GOLD) < cost){
				return FALSE;
			}
		
			/*---堁鉣禱蚍�梊�---*/
			CHAR_DelGold( talker, cost);
		}
		
	}
	
	CHAR_send_P_StatusString(talker,CHAR_P_STRING_GOLD);

	return TRUE;

}



/*-----------------------------------------
昡筒哱摯銓猀
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
縈  昡筒哱摯銓猀
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
