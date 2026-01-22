#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"


//藯凜摯窅暵ぁ拻堎啞溘精摯倳
#define MAXEVENTFLG 96

/*
 * 騷穫璃哱摯啞溘精禱鏍攝蚗袲埰堎      
 *
 */
 
static void NPC_CheckMan_selectWindow( int meindex, int toindex, int num);
int NPC_NowFlgCheck(int meindex,int talker,int now[MAXEVENTFLG]);
int NPC_EndFlgCheck(int meindex,int talker ,int nowflg[MAXEVENTFLG]);
BOOL NPC_FlgCheckMain( int meindex,int talker,int nowindex,int now[MAXEVENTFLG],char *work2);

/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_CheckManInit( int meindex )
{
	/*--す籤溘摯淏騷鏤禱优擭--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPECHECKMAN);

	return TRUE;
}


/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_CheckManTalked( int meindex , int talkerindex , char *szMes ,int color )
{

    /* 鏤畛騷媕↓勗葡媃趙煦崹  伀埰堎 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    {
    	return;
    }

	/*--  摯鞳勗笢堎凝汔竣凝ㄑ--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		/* 兒精惉蚗迶雄  摯陑 */
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE) return;
	}

	//  疐摯坳騷璃迶坳勗
	NPC_CheckMan_selectWindow( meindex, talkerindex, 0);

}


/*
 * 掘窐  勗覕崹堎
 */
static void NPC_CheckMan_selectWindow( int meindex, int talker, int num)
{

	char token[1024];
	char work[256];
	char work2[512];

	char escapedname[1024];
	int fd = getfdFromCharaIndex( talker);
	int buttontype = 0;
	int windowtype = 0;
	int windowno = 0;
	int now[MAXEVENTFLG];
	int nowindex;
	int i;	
	int page;
	
	work[0] = 0;
	work2[0] = 0;
	token[0] = 0;

	now[0] =0;
	
	/*--坳騷璃迶坳淏騷鏤隍蚗掛↓喇誑堁堁笢摯ぁ袸勗优擭--*/
  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	switch( num) {
	
	  case 0:
		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,0);
		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,0);
		
  		/*--撒      --*/
		sprintf(token,"3\n ��������ˇˇ��鏍攝蚗袲悃璃��ˇˇ "
				  "\n������藯凜摯啞溘精鏍攝蚗袲禱媃竘↓埰"
				  "\n"
				  "\n�������� �掁�NOW啞溘精鏍攝蚗袲���� "
				  "\n�������� �掁�END啞溘精鏍攝蚗袲���� "
				  "\n\n���� �掁�NOW啞溘精鏍攝蚗袲  寁備  �� "
				  "\n���� �掁�END啞溘精鏍攝蚗袲  寁備  �� "
		);

	  	buttontype = WINDOW_BUTTONTYPE_CANCEL;
	  	windowtype = WINDOW_MESSAGETYPE_SELECT;
	  	windowno = CHAR_WINDOWTYPE_CHECKMAN_START; 
	  	break;

	//NOW啞溘精摯螢ワ  崚
	  case 1:
	  	
	  	//NOW啞溘精摯鏍攝蚗袲
		nowindex = NPC_NowFlgCheck( meindex, talker, now);

		//笰妐↓喇  凝
		page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;
		
		if(page == 0 || page == 1){
			i = 0;
		}else{
			i = 83;
		}
		
		//妐↓喇
		for(; i < nowindex ; i++)
		{
			sprintf(work,"%d,",now[ i]);
			strcat(work2,work);
		}
		
		sprintf(token,"����������ˇˇ��鏍攝蚗袲悃璃��ˇˇ "
					"\n����藯凜摯耟  摯  埱趙笢堎 NOW騷穫璃哱"
					"\n%s"
			 	,work2);	
	  	
	  	if(page != 2 && nowindex > 83)
	  	{
	  		//さ↓迶
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,1);
	  		//妐↓喇
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,2);

			buttontype = WINDOW_BUTTONTYPE_NEXT;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 

		}else{
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		}
		
		break;


	//END啞溘精摯螢ワ  崚
	  case 2:
		//敖  啞溘精摯鏍攝蚗袲
		nowindex = NPC_EndFlgCheck( meindex, talker, now);
		
		//笰妐↓喇  凝
		page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;
		
		if(page == 0 || page == 1){
			i = 0;
		}else{
			i = 83;
		}
		work2[0]=0;
		
		//妐↓喇
		for(; i < nowindex ; i++)
		{
			sprintf(work,"%d,",now[ i]);
			strcat(work2,work);
		}
		sprintf(token,"����������ˇˇ��鏍攝蚗袲悃璃��ˇˇ "
					"\n����藯凜摯耟  摯  埱趙笢堎 END騷穫璃哱"
					"\n%s"
			 	,work2);	
	  	
	  	if(page != 2 && nowindex > 83)
	  	{
	  		//さ↓迶
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,2);
	  		//妐↓喇
	  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,2);

			buttontype = WINDOW_BUTTONTYPE_NEXT;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 

		}else{
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		}
		
		break;
		
	
	// NOW啞溘精摯寁備  崚
	  case 4:
		{
			
			//NOW啞溘精鏍攝蚗袲
			nowindex = NPC_NowFlgCheck( meindex, talker, now);

			if(NPC_FlgCheckMain( meindex, talker, nowindex,now,work2)
			 == FALSE)
			 {
			 	return;
			 }
			

			sprintf(token,"����������ˇˇ��鏍攝蚗袲悃璃��ˇˇ "
						"\n����藯凜摯耟  摯  埱趙笢堎 NOW騷穫璃哱"
						"\n%s"
				 	,work2);	

			page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;
		  	nowindex = (nowindex / ((6*page)+1));

		  	if(page != 16 &&  nowindex != 0)
		  	{
		  		//さ↓迶
		  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANT,4);
		  		//妐↓喇
		  		page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC);
		  		page++;
		  		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,page);
			
				buttontype = WINDOW_BUTTONTYPE_NEXT;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 
			}else{
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			}

		}
	break;

	// END啞溘精摯寁備  崚
	  case 5:
		{
			//END啞溘精摯鏍攝蚗袲
			nowindex = NPC_EndFlgCheck( meindex, talker, now);
			
			
			if(NPC_FlgCheckMain( meindex, talker, nowindex, now, work2)
			 == FALSE)
			 {
			 	return;
			 }

			sprintf(token,"����������ˇˇ��鏍攝蚗袲悃璃��ˇˇ "
						"\n����藯凜摯耟  摯  埱趙笢堎 END騷穫璃哱"
						"\n%s"
				 	,work2);	

			page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;

		  	nowindex = (nowindex / ((6*page)+1));

		  	if(page != 16 &&  nowindex != 0)
		  	{
		  		//さ↓迶
		  		CHAR_setWorkInt(talker, CHAR_WORKSHOPRELEVANT, 5);
		  		//妐↓喇
		  		page = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC);
		  		page++;
		  		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, page);
			
				buttontype = WINDOW_BUTTONTYPE_NEXT;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			  	windowno = CHAR_WINDOWTYPE_CHECKMAN_MAIN; 
			}else{
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			}

		}
	}
	
	makeEscapeString( token, escapedname, sizeof( escapedname));
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
void NPC_CheckManWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	int datano;
	
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) return;

	datano = atoi(data);
	
	if(select == WINDOW_BUTTONTYPE_OK) 
	{
		NPC_CheckMan_selectWindow( meindex, talkerindex, 0 );
	}else if(select == WINDOW_BUTTONTYPE_CANCEL) {
		return;
	}



	
	switch(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT)){
	  case 1:
	  	if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) == 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 1 );
	  	}
	  	break;

	  case 2:
	  	if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) == 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 2 );
	  	}
	  	break;

	  case 4:
	  	if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) >= 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 4 );
	  	}

	  case 5:
		if(CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTSEC) >= 2){
			NPC_CheckMan_selectWindow( meindex, talkerindex, 5 );
		}

		break;
	}

	switch( datano ){
	  case 1:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 1 );
		break;

	  case 2:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 2 );
		break;

	  case 4:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 4 );
		break;
		
	  case 5:
			NPC_CheckMan_selectWindow( meindex, talkerindex, 5 );
		break;
	}

}

/*
 *��騷穫璃哱  啞溘精禱鏍攝蚗袲埰堎
 */
int NPC_NowFlgCheck(int meindex,int talker ,int nowflg[MAXEVENTFLG])
{
	int i = 0;
	int j = 0;
	
	for(i= 0; i < MAXEVENTFLG ; i++){
		if(NPC_NowEventCheckFlg( talker, i) == TRUE)
		{
			nowflg[j] = i;
			j++;
		}
 	}
 	
	return j;
}

/*
 *��騷穫璃哱敖  啞溘精禱鏍攝蚗袲埰堎
 */
int NPC_EndFlgCheck(int meindex,int talker ,int nowflg[MAXEVENTFLG])
{
	int i = 0;
	int j = 0;
	
	for(i= 0; i < MAXEVENTFLG ; i++){

		if(NPC_EventCheckFlg( talker, i) == TRUE)
		{
			nowflg[j] = i;
			j++;
		}
 	}
 	
	return j;
}

/*
 * 寁備
 *
 */
BOOL NPC_FlgCheckMain( int meindex,int talker,int nowindex,int now[MAXEVENTFLG],char *work2)
{
	int page;
	int max;
	int i;
	int shou;
	int j=1;
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char work[512];
	char buf[40];
	char buf2[42];
	
	//笰妐↓喇  凝
	page = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) ;

	if(page == 0) {
		page =1;
		CHAR_setWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC,1) ;
	}

	if(page == 1){
		i = 0;
		if( nowindex >6 ){
			max =7;
		}else{
			max =nowindex;
		}
	}else{
		max = (page * 6) +1;
		i = max - 7;
		shou = nowindex / max;

		if(shou == 0){
			max = nowindex;
		}else{
			i = max - 7;
		}
	}
			
	//騷穫璃哱摯寁備誑怳凝躂趙笢堎啞栝騷鳴禱  陑喫資
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("NPC_CheckMan:GetArgStrErr");
		return FALSE;
	}
			
	work[0] = 0;
	work2[0] = 0;

	//妐↓喇
	for(; i < max ; i++)
	{
		sprintf(work,"#%d:",now[ i]);
		j = 1;
		while( getStringFromIndexWithDelim(argstr, "|", j, buf,sizeof( buf))
		 !=FALSE )
		 {	
			j++;
			if(strstr(buf,work) != NULL){
				sprintf(buf2,"%s\n",buf);
				strcat(work2,buf2);
				break;
			}
		}
	}
	
	return TRUE;
}
