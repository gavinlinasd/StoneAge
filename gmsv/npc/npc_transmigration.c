#include "version.h"
#include <string.h>
#include <math.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "enemy.h"
#include "log.h"
#include "battle.h"
#include "readmap.h"
#include "npc_transmigration.h"

#ifdef _PET_TRANS
#include "npc_pettransman.h"
#endif

static void NPC_Transmigration_selectWindow( int meindex, int toindex, int num);
static int NPC_TransmigrationCheck(int meindex, int talker);
BOOL NPC_TransmigrationAddPet(int meindex, int talker, int petid);
//匴倳摯擭壛
int NPC_StartpointCheck(int meindex,int talker);
BOOL NPC_TransmigrationMain(int meindex, int toindex, char *buf);
int NPC_TransmigrationFlg_CLS(int meindex, int toindex);
BOOL NPC_TransmigrationDelPet(int meindex, int talker);
BOOL NPC_TransmigrationStatus(int meindex, int toindex,int *work);
BOOL NPC_TransmigratiomWarp(int meindex, int toindex, char *buf);
void s_eventsetend( int charaindex, int shiftbit );

#ifdef _PET_TRANS
int Pet_Select;
#endif

/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_TransmigrationInit( int meindex )
{

	/*--す籤溘摯淏騷鏤禱优擭--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETRANSMIGRATION );

    return TRUE;

}

/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_TransmigrationTalked( int meindex , int talkerindex , char *szMes ,int color )
{
#ifdef _PET_TRANS
	int i,petindex;
	char buf[256];
#endif	
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
//	print("savepoot=%d",CHAR_getInt( talkerindex, CHAR_SAVEPOINT));
//	NPC_StartpointCheck( meindex, talkerindex);


//鎖濘滑捇瓚剿 傖蕾 唾昜蛌汜 祥傖蕾 �冼擼罔�
#ifdef _PET_TRANS
  for(i=0 ; i < CHAR_MAXPETHAVE ; i++)	{
    petindex = CHAR_getCharPet( talkerindex, i);
    if( petindex == -1  )
        continue;
    //潰脤唾昜腔 ID 岆瘁峈鎖濘滑捇 1479 ID 峈718
    if( CHAR_getInt( petindex, CHAR_PETID) == 718
#ifdef _PET_2TRANS
		|| CHAR_getInt( petindex, CHAR_PETID) == 401
#endif
		)	{
	  if( CHAR_getInt( petindex, CHAR_LV) > 79 )	{
		//continue;
	  }else	if( CHAR_getInt( petindex, CHAR_LV) == 79 ) {
		if( strcmp( CHAR_getChar( petindex, CHAR_OWNERCDKEY), CHAR_getChar( talkerindex, CHAR_CDKEY) ) ||
			strcmp( CHAR_getChar( petindex, CHAR_OWNERCHARANAME), CHAR_getChar( talkerindex, CHAR_NAME) ))	{
			sprintf( buf,"斕旯奻腔鎖濘滑捇ㄛ甜祥岆斕腔陛ㄐ");
			CHAR_talkToCli( talkerindex, -1, buf,  CHAR_COLORWHITE);
			return;
		}
		break;
	  }
	}
  }

  if(i != CHAR_MAXPETHAVE)	{
    CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,1);
    NPC_PetTransMan_selectWindow( meindex, talkerindex,0,-1);
  }else {
#endif
    CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT,1);
    NPC_Transmigration_selectWindow( meindex, talkerindex, 0);
#ifdef _PET_TRANS
  }
#endif

	//埻蛌汜
	//CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT,1);
	//NPC_Transmigration_selectWindow( meindex, talkerindex, 0);
	
}


//掘窐  勗覕崹堎
static void NPC_Transmigration_selectWindow( int meindex, int toindex, int num)
{

	char token[1024];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[2048];
	char buf2[512];
	char tmp[512];
	char s_tensei[64];
	int buttontype = 0;
	int windowtype = 0;
	int windowno = 0;
	int tenseiNo = 1;
	int fd = getfdFromCharaIndex( toindex);
	int i = 1;
	BOOL tenflg = FALSE;
	char *elder[4] = {"samugiru","marinasu","jaja","karutarna"};
	int point;
	int ten_no;
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("Tensei:GetArgStrErr");
		return;
	}
  	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
	tenseiNo = CHAR_getInt( toindex, CHAR_TRANSMIGRATION);
	tenseiNo++;
	if(CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT) >= 2){
		tenseiNo--;
	}
	 
	//优擭啞栝騷鳴  ぁ摯梱牁樊倳勗堣竣忒摯禱撒屾倜誧摯奼  
	sprintf(s_tensei,"TENSEI:%d",tenseiNo);

	while(getStringFromIndexWithDelim( npcarg,"END",i,buf,sizeof( buf))	!= FALSE){
		i++;
		if(strstr(buf,s_tensei) != NULL) {
		 tenflg = TRUE;
		 break;
		}
	}
#if 0	
	//汔躂勗忒堣趙毀竘�欷汙朗�倜�梇�  
	if(tenflg == FALSE){
		print("SettingFile Error ");
		return ;
	}
#endif
        //ttom
        print("\nTrans meindex=%d,toindex=%d,num=%d",meindex,toindex,num);
        //ttom
        CONNECT_set_state_trans(fd,6);
        //ttom end
         
	//鼠躂�葸噯啄�  勗覕崹堎��
	switch( num){
	  case 0:
	        //ttom
	        //CONNECT_set_state_trans(fd,0);
	        //ttom end
		ten_no = NPC_TransmigrationCheck(meindex, toindex);
 		if(
#ifdef _TRANS_6
			ten_no < 6
#else
#ifdef _PK_SERVER
			ten_no < 3//啃勀PK癹秶善3蛌
#else
			ten_no < 5
#endif
#endif//_TRANS_6
			&& ten_no >= 0){
	  		//邳贀袲惉囮↓
	  		if(NPC_Util_GetStrFromStrWithDelim( buf,"startmsg", token,sizeof( token) ) 
				 == NULL)
				{
					print("startmsgErr");
					return;
				}
			  	buttontype = WINDOW_BUTTONTYPE_NEXT;
			  	//ttom
			  	CONNECT_set_state_trans(fd,0);
			  	//ttom end			  	                                                                                                                                
		}else if(
#ifdef _TRANS_6
			ten_no == 6
#else
#ifdef _PK_SERVER
			ten_no == 3//啃勀PK癹秶善3蛌
#else
			ten_no == 5
#endif
#endif//_TRANS_6
			){
			//梱牁禱其樊盓埱倜
		  	if(NPC_Util_GetStrFromStrWithDelim( npcarg,"maxmsg", token,sizeof( token) ) 
			   == NULL)
			{
				print("maxmsgErr");
				return;
			}
			buttontype = WINDOW_BUTTONTYPE_OK;
		}else{
			//邳贀袲惉囮媃趙埵笢
			if(NPC_Util_GetStrFromStrWithDelim( buf,"nonemsg", token,sizeof( token) ) 
			   == NULL)
			{
				print("nonemsgErr");
				return;
			}
			buttontype = WINDOW_BUTTONTYPE_OK;

		}
		windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_START;
		  	 
	 	break;
	 	
	 	case 1:
		 	if(NPC_Util_GetStrFromStrWithDelim( buf,"mainmsg", token,sizeof( token) ) 
			 == NULL)
			{
				print("mainmsgErr");
				return;
			}
		  	buttontype = WINDOW_BUTTONTYPE_OKCANCEL;
		  	windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_MAIN; 
	 	break;
	 	
	 	case 2:
      	 	        // Robin 2001/03/05 Tensei Check
			//if(!CHAR_getWorkInt(toindex,CHAR_TENSEICHECKED))  return;
	 	       
	 		//梱牁窐  禱
	 		point = NPC_StartpointCheck( meindex, toindex);
	 		if(point == -1) {
	 			return;
	 		}
	 		
			NPC_Util_GetStrFromStrWithDelim( npcarg,elder[point],buf2,sizeof( buf2));
			if(NPC_TransmigrationMain(meindex, toindex, buf2) ==  FALSE) 
			{
				return;
			}

	 		
	 		sprintf( tmp, "yesmsg%d", 
	 						CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT));
	 		
	 		//梱牁敖  隍蚗掛↓喇禱煦埰��
	 		if(NPC_Util_GetStrFromStrWithDelim( buf,tmp, token,sizeof( token) ) 
			 == NULL)
			{
				print("yesmsgErr");
				return;
			}

			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 
					CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT) +1
			);
	 		sprintf( tmp, "yesmsg%d", 
	 						CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT));
			
			//梱牁敖  隍蚗掛↓喇禱煦埰��
	 		//梱牁敖  隍蚗掛↓喇禱煦埰��
	 		if(strstr(buf, tmp) == NULL)	{
				buttontype = WINDOW_BUTTONTYPE_OK;
		  		windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_END; 
			}else	{
				buttontype = WINDOW_BUTTONTYPE_NEXT;
		  		windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_END; 
			}
	 	break;
	
	 	case 3:
	 		
	 		/*--撒      --*/
			/*--す↓極↓迶摯鏍攝蚗袲禱菜竣--*/
			if(NPC_Util_GetStrFromStrWithDelim( buf,"nomsg", token,sizeof( token) ) 
			 == NULL)
			{
				print("nomsgErr");
				return;
			}
		  	buttontype = WINDOW_BUTTONTYPE_OK;
	  		windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_NONE; 
	 		
	 	break;

	 	case 4:
	 		//梱牁  摯隍蚗掛↓喇窐  
	 		sprintf( tmp, "yesmsg%d", 
	 						CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT));
	 		//梱牁敖  隍蚗掛↓喇禱煦埰��
	 		if(NPC_Util_GetStrFromStrWithDelim( buf,tmp, token,sizeof( token) ) 
			 == NULL)
			{
				print("yesmsgErr");
				return;
			}

			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 
					CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT) +1
			);
	 		sprintf( tmp, "yesmsg%d", 
	 						CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT));
			
			//梱牁敖  隍蚗掛↓喇禱煦埰��
	 		//梱牁敖  隍蚗掛↓喇禱煦埰��
	 		if(strstr(buf, tmp) == NULL)
			{
				buttontype = WINDOW_BUTTONTYPE_OK;
		  		windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_END; 
			}else{
				buttontype = WINDOW_BUTTONTYPE_NEXT;
		  		windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_END; 
			}
			
	 	
		break;
		case 5:

                        // Robin 2001/03/05 Tensei Check
                        //if(!CHAR_getWorkInt(toindex,CHAR_TENSEICHECKED))  return;
			//梱牁窐  禱
	 		point = NPC_StartpointCheck( meindex, toindex);
	 		if(point == -1) return;
			NPC_Util_GetStrFromStrWithDelim( npcarg,elder[point],buf2,sizeof( buf2));
			NPC_TransmigratiomWarp(meindex, toindex, buf2);
//			buttontype = WINDOW_BUTTONTYPE_OK;
//	  		windowno = CHAR_WINDOWTYPE_TRANSMIGRATION_END; 
			return;
		break;

	 	
	}
//	makeEscapeString( token, escapedname, sizeof(escapedname));
	/*--這裡搜索處理--*/
	lssproto_WN_send( fd, windowtype, 
					buttontype, 
					windowno,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);
}
/*-----------------------------------------
袲溘騷囮璃哱凝�氪耽�趙拻倜鄹勗蠙怮③踏躂堎��
-------------------------------------------*/
void NPC_TransmigrationWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{

	int datanum = -1;

	int fd = getfdFromCharaIndex( talkerindex);
	/*--       摯竘渦堇勗笢埵笢敁拻毀敖   --*/
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}
       //ttom
       //CONNECT_set_state_trans(fd,6);
       //ttom end
	datanum = atoi( data);
	switch( seqno) {
		/*--  疐摯隍蚗掛↓喇-*/
	  case CHAR_WINDOWTYPE_TRANSMIGRATION_START:
		if( select == WINDOW_BUTTONTYPE_NEXT) {
		        //ttom
		        if(CONNECT_get_state_trans(fd)==0){ } else return;
		        //ttom end
		        NPC_Transmigration_selectWindow( meindex, talkerindex,1);
			//ttom
			CONNECT_set_state_trans(fd,1);
			//ttom end
			         
		}
	  	break;
		
		//梱牁埰堎凝汔竣凝  �妎肩彌鴃麚熉假玾�  
	  case CHAR_WINDOWTYPE_TRANSMIGRATION_MAIN:
	 	if( select == WINDOW_BUTTONTYPE_OK) {
	 	        //ttom
	 	        if(CONNECT_get_state_trans(fd)==1){ } else return;
	 	        //ttom end
	 	        NPC_Transmigration_selectWindow( meindex, talkerindex, 2);
	 	        //ttom
	 	        CONNECT_set_state_trans(fd,2);
	 	        //ttom end
	 	}else if(select == WINDOW_BUTTONTYPE_CANCEL) {
			NPC_Transmigration_selectWindow( meindex, talkerindex, 3);
		}
	    break;

		/*--    摯隍蚗掛↓喇--*/
	  case CHAR_WINDOWTYPE_TRANSMIGRATION_END:
	 	if( select == WINDOW_BUTTONTYPE_NEXT) {
	 	        //ttom
	 	        if(CONNECT_get_state_trans(fd)==2){ } else return;
	 	         //ttom end
	 	        NPC_Transmigration_selectWindow( meindex, talkerindex, 4);
	 	        //ttom
	 	        CONNECT_set_state_trans(fd,4);
	 	        //ttom end
	 	}else if( select == WINDOW_BUTTONTYPE_OK) {
	 	        //ttom
	 	        if(CONNECT_get_state_trans(fd)==4){ } else return;
	 	        //ttom end
	 	        NPC_Transmigration_selectWindow( meindex, talkerindex, 5);
		}
		break;
		
//ANDY_ADD 10/19
#ifdef _PET_TRANS

	case CHAR_WINDOWTYPE_PETTRANS_START:	//羲宎勤趕
		if( select == WINDOW_BUTTONTYPE_NEXT) {
			NPC_PetTransMan_selectWindow( meindex, talkerindex, 1, 0);
		}	
	break;

	case CHAR_WINDOWTYPE_PETTRANS_SELECT:	//岆瘁�籀駘擼罔�
		if( select == WINDOW_BUTTONTYPE_YES) {	
			NPC_PetTransMan_selectWindow( meindex, talkerindex, 2, 0);
		}else	{
			NPC_PetTransMan_selectWindow( meindex, talkerindex, 5, 0);
		}
	break;

	case CHAR_WINDOWTYPE_PETTRANS_SELPET:	//恁唾
		if(select==WINDOW_BUTTONTYPE_CANCEL)	{
			NPC_PetTransMan_selectWindow( meindex, talkerindex, 5,-1 );
		}else	{
			Pet_Select = datanum;	
			NPC_PetTransMan_selectWindow( meindex, talkerindex, 3, datanum );
		}
	break;

	case CHAR_WINDOWTYPE_PETTRANS_MAIN1:	//�毓迅駘�	�毓�/�＋�
		if(select==WINDOW_BUTTONTYPE_YES){
			NPC_PetTransMan_selectWindow( meindex, talkerindex, 4, Pet_Select);
		}else	{
			NPC_PetTransMan_selectWindow( meindex, talkerindex, 5, -1);
		}
	break;
       
	case CHAR_WINDOWTYPE_PETTRANS_END:	//唾昜蛌汜賦旰賦旰
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	break;
	
#endif

	}


}

//************************************************************
//	毀啋誧摯荇  擽勗極↓鏤踏鞠堎
//************************************************************
BOOL NPC_TransmigratiomWarp(int meindex, int toindex, char *buf)
{
	char buf2[512];
	char buf3[128];
	char buf4[64];
	int floor=0 , x = 0, y = 0;
	int i = 1;
	int ret;
	
	//③�嚂昈婠蝻匙紨鶱�  ��
	while(getStringFromIndexWithDelim( buf,",",i,buf2,sizeof( buf2))
	!= FALSE)
	{
		i++;
		if(strstr(buf2,"Warp") != NULL)
		{
			getStringFromIndexWithDelim( buf2 ,"=" ,2, buf3 ,sizeof(buf3));
			//啞痲囮
			ret = getStringFromIndexWithDelim( buf3,".",1 ,buf4 ,sizeof( buf4));
			if(ret ==FALSE) return FALSE;
			floor = atoi(buf4);
			//  淢  
			ret = getStringFromIndexWithDelim( buf3,".",2 ,buf4 ,sizeof( buf4));
			if(ret ==FALSE) return FALSE;
			x = atoi(buf4);
			//  淢  
			ret = getStringFromIndexWithDelim( buf3,".",3 ,buf4 ,sizeof( buf4));
			if(ret ==FALSE) return FALSE;
			y = atoi(buf4);

		}
	}
    
    
	//極↓鏤袸摯鏍攝蚗袲
    if( MAP_IsValidCoordinate( floor, x, y )== FALSE ){
		return FALSE;
	}

        //ttom
        /*
        {
            int tenseiNo;
            tenseiNo=CHAR_getInt( toindex, CHAR_TRANSMIGRATION);
            CHAR_setInt( toindex,CHAR_SKILLUPPOINT,tenseiNo*10);
            print("\n the set the skillpoint %d",tenseiNo*10);
            CHAR_sendStatusString( toindex , "P");
        }
        */
        //ttom
                                                   
	//極↓鏤踏鞠堎
	 CHAR_warpToSpecificPoint(toindex, floor, x, y);

	return TRUE;

}

//************************************************************
//*③  鬻摯鏍攝蚗袲
//
//  堇偯※
//	兔※�衯承袘�
//	兒※悃惉圖筒
//	兕※喇籤喇籤
//	兩※庈鳴淏↓圖
//	√兒※操溘↓
//************************************************************
int NPC_StartpointCheck(int meindex,int talker)
{
	int point;
	int shift = 0;
	point = CHAR_getInt( talker, CHAR_SAVEPOINT);
	for(shift = 0; shift < 4 ;shift++){
		if( (point & ( 1<< shift))  == ( 1<< shift)) {
			return shift;
		}
	}
	return -1;
}

//********************************************************
//
//	梱牁邳贀禱  倜媃趙笢堎凝汔竣凝摯鏍攝蚗袲
//
//  堇偯
//	埬  ※梱牁樊倳
//	謄  ※-1
//
//********************************************************
static int NPC_TransmigrationCheck(int meindex, int talker)
{
	//	梱牁邳贀
	//畛穫鳴誑函兔雄窀
	//具僑摯鶂鍛騷穫璃哱誑敖渦埱趙笢堎喫敁
	//  敖騷穫璃哱禱喫埵埰��(喫躂毀笢�梛狊框�  
	int i,j,k=0;
	int ch_no[4] = {39,40,42,46};
	int petindex;
	int num;
//	int petid[4] = {1, 2, 3, 4}; //妐蚗哱摯    禱炩擭
	int petid[4] = {693, 694, 695, 696}; //妐蚗哱摯    禱炩擭
//	int petidfinal[4] = {11, 12, 13, 14}; //妐蚗哱摯    禱炩擭
	int petidfinal[4] = {693, 694, 695, 696}; //妐蚗哱摯    禱炩擭

	// Robin 2001/03/05
	//CHAR_setWorkInt(talker,CHAR_TENSEICHECKED,FALSE);
	//忒媃其樊雄窀埵�梇鎯滑用槾聿苺�
#ifdef _TRANS_6
	if(CHAR_getInt(	talker, CHAR_TRANSMIGRATION) >= 6){
		return 6;
	}
#else
	if(CHAR_getInt(	talker, CHAR_TRANSMIGRATION) >= 5){
		return 5;
	}
#endif

	//畛穫鳴誑函兔雄窀
	if(CHAR_getInt( talker, CHAR_LV) < 80) return -1;

	//騷穫璃哱摯啞溘精鏍攝蚗袲
#ifdef _TRANS_6
	if(CHAR_getInt(	talker, CHAR_TRANSMIGRATION) < 6){
#else
	if(CHAR_getInt(	talker, CHAR_TRANSMIGRATION) < 5){
#endif
		for(i = 0; i < 4 ;i++)
			if(NPC_EventCheckFlg( talker, ch_no[i] ) == FALSE) return -1;
	}

	num = CHAR_getInt(talker, CHAR_TRANSMIGRATION);

	if(num  < 4 ){
	//炩擭妐蚗哱禱忒埱趙笢堎凝妐蚗哱鏍攝蚗袲
		for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
			petindex = CHAR_getCharPet( talker, i);
			if( petindex == -1  )  continue;
			//ttom
			//print("petid=%d ",CHAR_getInt( petindex, CHAR_PETID));
			//妐蚗哱摯    毀郖灉凝ㄑ
			if(CHAR_getInt( petindex, CHAR_PETID) != petid[num]) continue;

			//妐蚗哱摯畛穫鳴毀邳贀禱  倜媃趙笢堎凝
//			if(CHAR_getInt( petindex, CHAR_LV) < PETLEVEL) continue;
			//喫喫竘ぁ拻倜�桯姨動該檗�毀    敁  茯
			break;
		}
		if(i == CHAR_MAXPETHAVE) return -1;
	}else
#ifdef _TRANS_6
    if( num == 4 )
#endif
	{
		for(j = 0; j < 4 ;j++){
			//察樊  摯鳹譴毀具  禱鏍攝蚗袲
			for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
				petindex = CHAR_getCharPet( talker, i);
				if( petindex == -1  )  continue;
		//		print("petid=%d ",CHAR_getInt( petindex, CHAR_PETID));
				//妐蚗哱摯    毀郖灉凝ㄑ
				if(CHAR_getInt( petindex, CHAR_PETID) != petidfinal[j]) continue;
				k++;
				break;
			}
		}
		if(k < 4) return -1;
	}
#ifdef _TRANS_6
	else{
		/*int lasttalkelder = 2;//拫薯
		if( CHAR_getInt( talker, CHAR_LASTTALKELDER) == 1 )//鎖璨饑佪
			lasttalkelder = 112;//翮掀
		else if( CHAR_getInt( talker, CHAR_LASTTALKELDER) == 2 )//樓樓
			lasttalkelder = 102;//親親嫌
		else if( CHAR_getInt( talker, CHAR_LASTTALKELDER) == 3 )//縐糧坳饒
			lasttalkelder = 34;//哏皎
		print("\npetid:%d",lasttalkelder);*/

		//梑艘艘衄羶衄湍2蛌堤汜唾昜
		for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
			petindex = CHAR_getCharPet( talker, i);
			if( petindex == -1  )  continue;
			if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION) == 2 
				&& ( CHAR_getInt( petindex, CHAR_PETID) == 2 
				|| CHAR_getInt( petindex, CHAR_PETID) == 112
				|| CHAR_getInt( petindex, CHAR_PETID) == 102
				|| CHAR_getInt( petindex, CHAR_PETID) == 34 ) ) break;
			//if(CHAR_getInt( petindex, CHAR_TRANSMIGRATION) != 2 || CHAR_getInt( petindex, CHAR_PETID) != lasttalkelder ) continue;		
		}
		if(i == CHAR_MAXPETHAVE){
			CHAR_talkToCli( talker,-1,"斕羶衄湍2蛌腔堤汜唾韃!",CHAR_COLORWHITE);
			return -1;
		}


		if(NPC_EventCheckFlg( talker, 63) == FALSE){ print("\nerr63");return -1;}
		if(NPC_EventCheckFlg( talker, 136) == FALSE){ print("\nerr136");return -1;}
		if(NPC_EventCheckFlg( talker, 175) == FALSE){ print("\nerr175");return -1;}
		if(NPC_EventCheckFlg( talker, 176) == FALSE){ print("\nerr176");return -1;}
		//潰脤衄拸15跺藹賜眳藷呯え
		j=0;
		for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
			int itemindex;
			itemindex = CHAR_getItemIndex(talker,i);
			if( ITEM_CHECKINDEX(itemindex) == FALSE )continue;
			if( ITEM_getInt( itemindex, ITEM_ID ) == 20613 ) {
				++j;
			}
		}
		if( j < 15 ){
			CHAR_talkToCli( talker,-1,"斕羶衄湍15跺藹賜眳藷腔呯え韃!",CHAR_COLORWHITE);
			print("\nitem:%d",j);
			return -1;
		}

		if( CHAR_getInt(talker, CHAR_HEROCNT) <= 0 ){
			CHAR_talkToCli( talker,-1,"斕羶衄硒俴徹儕鍾欸遢腔�恄�!",CHAR_COLORWHITE);
			return -1;
		}
	}
#endif
	// Robin 2001/03/05
	//CHAR_setWorkInt(talker,CHAR_TENSEICHECKED,TRUE);
	//  敖騷穫璃哱鏍攝蚗袲

	return CHAR_getInt(talker, CHAR_TRANSMIGRATION);
}

BOOL NPC_TransmigrationMain(int meindex, int toindex, char *buf)
{
	int i = 1;
	char buf2[128];
	char buf3[64];
	char buf4[32];
	int item = 10;
	int pet = 5;
	int present = 0;
	int ten_no;
	int work[10];
	int fd = getfdFromCharaIndex( toindex );

#ifdef _ADD_POOL_ITEM			   // WON ADD 崝樓褫敵溫腔耋撿	
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// 崝樓腔耋撿敵溫杅
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// 崝樓腔唾昜敵溫杅
	int j,item_sum=0, pet_sum=0;

	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// 癹秶郔湮腔敵耋撿杅
	pet_sum  += 5;							    // 癹秶郔湮腔敵唾杅

#endif
	while(getStringFromIndexWithDelim( buf,",",i,buf2,sizeof( buf2))!= FALSE){
		i++;

		if(strstr(buf2,"Item") != NULL){
			getStringFromIndexWithDelim( buf2 ,"=" ,2, buf3 ,sizeof(buf3));
			item = atoi(buf3);
		}else if(strstr(buf2,"Pet") != NULL){
			getStringFromIndexWithDelim( buf2 ,"=" ,2, buf3 ,sizeof(buf3));
			pet = atoi(buf3);
		}else if(strstr(buf2,"Present") != NULL){
			ten_no = CHAR_getInt(toindex,CHAR_TRANSMIGRATION);
			ten_no++;
			getStringFromIndexWithDelim( buf2 ,"=" ,2, buf3 ,sizeof(buf3));
			getStringFromIndexWithDelim( buf3,".",ten_no ,buf4 ,sizeof( buf4));
			present = atoi(buf4);
		}
	}

	CHAR_setInt(toindex ,CHAR_TRANSMIGRATION,
		CHAR_getInt(toindex,CHAR_TRANSMIGRATION) + 1);
#ifdef _TRANS_6
	if(CHAR_getInt(	toindex, CHAR_TRANSMIGRATION) > 6)
		CHAR_setInt(toindex ,CHAR_TRANSMIGRATION,6);
	if( CHAR_getInt(toindex,CHAR_TRANSMIGRATION) == 6 )//6蛌摽垀冞腔唾
		present = 2546;
#else
	if(CHAR_getInt(	toindex, CHAR_TRANSMIGRATION) > 5)
		CHAR_setInt(toindex ,CHAR_TRANSMIGRATION,5);
#endif
	NPC_TransmigrationStatus(meindex, toindex, work);
	NPC_TransmigrationFlg_CLS(meindex, toindex);
	CHAR_setInt(toindex, CHAR_LV ,1);
	CHAR_setMaxExp( toindex, 0);

#ifdef _TRANS_6
//刉壺藹賜眳藷呯え
	for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
		int itemindex;
		itemindex = CHAR_getItemIndex(toindex,i);
		if( ITEM_CHECKINDEX(itemindex) == FALSE )continue;
		if( ITEM_getInt( itemindex, ITEM_ID ) == 20613 ) {
			CHAR_DelItem( toindex, i);
		}
	}
#endif

	if(NPC_TransmigrationDelPet(meindex, toindex) == FALSE) return FALSE;
	if(	NPC_TransmigrationAddPet( meindex, toindex, present)== FALSE) return FALSE;
	// Robin 0718
        {
            int tenseiNo;
            tenseiNo=CHAR_getInt( toindex, CHAR_TRANSMIGRATION);
            CHAR_setInt( toindex,CHAR_SKILLUPPOINT,tenseiNo*10);
            CHAR_Skillupsend( toindex );
            //CHAR_setInt( toindex,CHAR_LEARNRIDE, 0);
            CHAR_setInt( toindex,CHAR_RIDEPET, -1 );
            CHAR_setInt( toindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( toindex , CHAR_BASEBASEIMAGENUMBER) );
            
            //CHAR_sendStatusString( toindex , "P");
        }
	lssproto_WO_send(fd ,0 );
	// 梱牁痲精禱噁堎
	LogTensei(
		CHAR_getChar( toindex, CHAR_NAME ), /* す籤溘   */
		CHAR_getChar( toindex, CHAR_CDKEY),
		"Tensei",
		work[0],
		CHAR_getInt(toindex,CHAR_TRANSMIGRATION),
		work[1],
		NPC_StartpointCheck( meindex, toindex),
		item, pet,
		work[2],work[3],work[4],work[5],
		work[6],work[7],work[8],work[9]
		);

	//脾犍
	CHAR_complianceParameter( toindex );
	//HP禱樊蜾踏鞠趙堣ど堎
	CHAR_setInt(toindex, CHAR_HP, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP) );
	//    袸禱③�儢彼勒枅殷荓�  埰��
	CHAR_setInt(toindex,CHAR_LASTTALKELDER, NPC_StartpointCheck(meindex,toindex));
	/*  鏤畛騷媕↓攷  禱邞堎  */
	CHAR_sendStatusString( toindex, "P" );
	/*
	CHAR_send_P_StatusString( toindex,
								  CHAR_P_STRING_LV|CHAR_P_STRING_EXP|
								  CHAR_P_STRING_NEXTEXP|
								  CHAR_P_STRING_HP|CHAR_P_STRING_MAXHP|
								  CHAR_P_STRING_MP|CHAR_P_STRING_MAXMP|
								  CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
								  CHAR_P_STRING_QUICK|CHAR_P_STRING_CHARM|
								  CHAR_P_STRING_LUCK|CHAR_P_STRING_EARTH|
								  CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
								  CHAR_P_STRING_WIND|CHAR_P_STRING_VITAL|
								  CHAR_P_STRING_STR|CHAR_P_STRING_DEX|
								  CHAR_P_STRING_TOUGH|
								  CHAR_P_STRING_DUELPOINT //ttom
								  );
	*/			
	return TRUE;
}


static int questTbl[20] = {
1,2,4,5,8,12,14,15,16,17,19,22,27,30,31,34,35,38,45,47
};
//*******************************************************
//袲操筒哱摯倳禱倳窇堎
//*******************************************************
int NPC_TransmigrationQuestCheck(int toindex)
{
	int i;
	int num = 0;
	
	for(i = 0 ; i < 20; i++){
		if(NPC_EventCheckFlg(toindex ,questTbl[i]) == TRUE){
			num++;
		}
	}
	
	return num;
}

//*******************************************************
//	筒  ↓淏筒禱銓猀埰堎
//*******************************************************
int NPC_TransCalculation(int toindex, int para)
{
	float quest, level, num, tmp;
	float ans;
	int equ;
	equ = CHAR_getInt(toindex, CHAR_TRANSEQUATION);
	//16bit  ぁ窅暵
	quest = (float)(( equ >> 16 ) & 0xFFFF);
	level = (float)(( equ >> 0 ) & 0xFFFF);
	//tttom
	//print("\n this test equ=%d,quest=%f,level=%f",equ,quest,level);
	//梱牁樊倳
	num = CHAR_getInt(toindex, CHAR_TRANSMIGRATION); // ヶ醱眒樓1,嘟0蛌1奀峈1
	tmp = (float)para / 100;
#ifdef _TRANS_6
	if (num == 6) tmp = 620; // 鞠蛌汜奀導萸杅祥數
#endif
	ans =(float)tmp/12 + (float)(quest / 4) +( (float)(level- num * 85) /4);
	return (int)ans;
}

//*******************************************************
//	鞡蛃察  
//*******************************************************
float Rounding(float work,int num)
{

	float p;
	
	if(num  <0 ) return work;
	num--;
	p = (float)pow(10,num);
	
	return ( (work * p + 0.5) /p);


}

//*******************************************************
//
//	筒  ↓淏筒匴祛摯脾犍
//
//*******************************************************
BOOL NPC_TransmigrationStatus(int meindex, int toindex,int work[10])
{
	int vital, str, tgh, dex;
	int quest , level ,equ;
	int t_quest, t_level;
	int total,ans;
	float tmp,lvup;
	
	vital = CHAR_getInt(toindex,CHAR_VITAL);
	str = CHAR_getInt(toindex,CHAR_STR);
	tgh = CHAR_getInt(toindex,CHAR_TOUGH);
	dex = CHAR_getInt(toindex,CHAR_DEX);


	//袲操筒哱啞溘精禱鏍攝蚗袲
	quest = NPC_TransmigrationQuestCheck( toindex);
	//畛穫鳴
	level = CHAR_getInt(toindex,CHAR_LV);
	
	work[0] = level;
	work[1] = quest;
	
	equ = CHAR_getInt(toindex, CHAR_TRANSEQUATION);

	t_quest = (float)(( equ >> 16 ) & 0xFFFF);
	t_level = (float)(( equ >> 0 ) & 0xFFFF);
	
	if( level > 130 ) level = 130;
	t_quest = t_quest + quest;
	t_level = t_level + level;

	CHAR_setInt(toindex, CHAR_TRANSEQUATION,
   		( t_quest << 16 )+( t_level << 0 )
   	);

	total = vital + str + tgh + dex;
	ans = NPC_TransCalculation(toindex, total);

	work[2] = vital;
	tmp = (float)vital/total * ans;
	work[3] = Rounding(tmp,1) * 100;

	work[4] = str;
	tmp = (float)str/total * ans;
	work[5] = Rounding(tmp,1) * 100;

	work[6] = tgh;
	tmp = (float)tgh/total * ans;
	work[7] = Rounding(tmp,1) * 100;
	
	work[8] = dex;
	tmp = (float)dex/total * ans;
	work[9] = Rounding(tmp,1) * 100;

	//vital掛蚗哱
	if(work[3] < 0) work[3] = 0;
	CHAR_setInt(toindex, CHAR_VITAL, work[3]);
	//str掛蚗哱
	if(work[5] < 0) work[5] = 0;
	CHAR_setInt(toindex, CHAR_STR, work[5]);

	//tghr掛蚗哱
	if(work[7] < 0) work[7] = 0;
	CHAR_setInt(toindex, CHAR_TOUGH, work[7]);
	
	//dex掛蚗哱
	if(work[9] < 0) work[9] = 0;
	CHAR_setInt(toindex, CHAR_DEX, work[9]);
	
	
	
	//畛穫鳴    睽騷璃哱忒  脾
	lvup = CHAR_getInt(toindex, CHAR_SKILLUPPOINT );
/*#ifdef _TRANS_6
	if( CHAR_getInt(toindex, CHAR_TRANSMIGRATION) == 6)
		lvup = 620;
#endif*/
	lvup = lvup / 12;
	lvup =  Rounding(lvup, 1);
	if(lvup < 0) lvup = 0;
	CHAR_setInt(toindex, CHAR_SKILLUPPOINT, (int)lvup);
	
	/*  筒す鳴囮蚗鏤ぁ拻倜�梜蟤壅屎肴�  */
	if( CHAR_getInt( toindex,CHAR_SKILLUPPOINT ) >=1 )
		CHAR_Skillupsend(toindex);

	
	return TRUE;


}


/*----------------------------
 *  妐蚗哱禱黹萃埰堎
 ------------------------------*/
BOOL NPC_TransmigrationAddPet(int meindex, int talker, int petid)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	i,j;
	int petindex;
	int petindex2;
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  break;
	}
	if(i == CHAR_MAXPETHAVE) return FALSE;
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
			break;
		}
	}

	if( i == enemynum ) return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( talker, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE ) i = 0;
	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
	}

	petindex2 = CHAR_getCharPet(talker, i);
    if( !CHAR_CHECKINDEX( petindex2) )	return FALSE;

	snprintf( msgbuf,sizeof( msgbuf), "鏽善%s﹝",
								CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
	    if( !CHAR_CHECKINDEX( petindex) ) continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}
	// 妐蚗哱禱鱗勗  躂倜痲精
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex2, CHAR_NAME),
		CHAR_getInt( petindex2, CHAR_LV),
		"TenseiGet",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex2, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

	return TRUE;

}

#ifdef _TEACHER_SYSTEM
extern int FMAdvTbl[];
#endif

int NPC_TransmigrationFlg_CLS(int meindex, int toindex)
{
#ifdef _TEACHER_SYSTEM
	int iGetFame = 0;
#endif
#ifdef _PET_TRANS
#ifdef _PROSK99
	int endevflg[]={69,70,71,72,109,118,122,131,127,158,159,181,-1};//181:啞曖蔣こ
#else
	int endevflg[]={69,70,71,72,109,118,122,131,127,158,159,-1};
#endif	
	int nowevflg[]={69,70,71,72,-1};
#endif
#ifdef _TRANS_6
	int flg[6] = {48,49,50,51,52,179};
#else
	int flg[5] = {48,49,50,51,52};
#endif
	int num= CHAR_getInt(toindex ,CHAR_TRANSMIGRATION);
	int i;
#ifdef _PET_TRANS
	i=0;
	while(endevflg[i] != -1 )	{
		if( NPC_EventCheckFlg( toindex, endevflg[i] ) == FALSE )	{
			endevflg[i] = 0;
		}
		i++;
	}
	i=0;
	while( nowevflg[i] != -1 )	{
		if( NPC_NowEventCheckFlg( toindex, nowevflg[i] ) == FALSE )	{
			nowevflg[i] = 0;
		}
		i++;
	}
#endif

	CHAR_setInt(toindex,CHAR_ENDEVENT,0);
	CHAR_setInt(toindex,CHAR_ENDEVENT2,0);
	CHAR_setInt(toindex,CHAR_ENDEVENT3,0);
#ifdef _NEWEVENT
	CHAR_setInt(toindex,CHAR_ENDEVENT4,0);
	CHAR_setInt(toindex,CHAR_ENDEVENT5,0);
	CHAR_setInt(toindex,CHAR_ENDEVENT6,0);
#endif
	CHAR_setInt(toindex,CHAR_NOWEVENT,0);
	CHAR_setInt(toindex,CHAR_NOWEVENT2,0);
	CHAR_setInt(toindex,CHAR_NOWEVENT3,0);
#ifdef _NEWEVENT
	CHAR_setInt(toindex,CHAR_NOWEVENT4,0);
	CHAR_setInt(toindex,CHAR_NOWEVENT5,0);
	CHAR_setInt(toindex,CHAR_NOWEVENT6,0);
#endif
#ifdef _ADD_NEWEVENT              // WON 多增加資料
	CHAR_setInt(toindex,CHAR_ENDEVENT7,0);
	CHAR_setInt(toindex,CHAR_NOWEVENT7,0);
	CHAR_setInt(toindex,CHAR_ENDEVENT8,0);
	CHAR_setInt(toindex,CHAR_NOWEVENT8,0);
#endif

	//梱牁啞溘精禱  趙堎
#ifdef _PET_TRANS
	i=0;
	while( endevflg[i] != -1 )	{
		if( endevflg[i] > 0 )	{
			// shan Begin
			//NPC_EventSetFlg( toindex, endevflg[i]);			
			s_eventsetend(toindex, endevflg[i]);
			// End
		}
		i++;
	}
	i=0;
	while( nowevflg[i] != -1 )	{
		if( nowevflg[i] > 0 )	{
			NPC_NowEventSetFlg( toindex, nowevflg[i]);
		}
		i++;
	}

#endif

	for(i = 0 ;i < num ;i++)	{
		NPC_EventSetFlg(toindex, flg[i]);
		// CoolFish: Family Adv 2001/8/4
		AddFMAdv(toindex, flg[i]);
#ifdef _TEACHER_SYSTEM
		// 濛樓垀腕汒咡
		if(flg[i] < 179) iGetFame += FMAdvTbl[flg[i]];
#endif
	}
#ifdef _TEACHER_SYSTEM
	iGetFame /= 20; // 絳呇鳳腕悝汜垀腕汒咡腔 5% (1/20)
	// 潰脤垀腕腔汒咡衄羶衄湮黺0
	if(iGetFame > 0){
		// 潰脤衄羶衄絳呇
		if(strlen(CHAR_getChar(toindex,CHAR_TEACHER_ID)) > 0 && strlen(CHAR_getChar(toindex,CHAR_TEACHER_NAME)) > 0){
			int iPlayernum = CHAR_getPlayerMaxNum();
			char szMsg[128];

			// 潰脤絳呇婓祥婓盄奻
			for(i=0;i<iPlayernum;i++){
				if(CHAR_getCharUse(i) == FALSE) continue;
				if(strcmp(CHAR_getChar(toindex,CHAR_TEACHER_ID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
					strcmp(CHAR_getChar(toindex,CHAR_TEACHER_NAME),CHAR_getChar(i,CHAR_NAME)) == 0){
					float fGetFame = (float)iGetFame/100;
					// 絳呇婓盄奻
					CHAR_setWorkInt(i,CHAR_WORK_GET_TEACHER_FAME,CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME) + iGetFame);
					sprintf(szMsg,"鳳腕悝汜 %s %.2f 萸汒咡",CHAR_getChar(toindex,CHAR_NAME),fGetFame);
					CHAR_talkToCli(i,-1,szMsg,CHAR_COLORYELLOW);
					break;
				}
			}
		}
	}
#endif

#ifdef _TRANS_6
	if( num == 6 ){
		NPC_EventSetFlg(toindex, 63); //祥跤俙模賤荎倯腔蛅腦
		NPC_EventSetFlg(toindex, 136);//祥跤俙模賤濡眳坒
		NPC_EventSetFlg(toindex, 175);//濘嫌�恄�
		NPC_EventSetFlg(toindex, 176);//濘嫌�恄�
	}
#endif

	return 1;
}

//******************************************************************
//妐蚗哱禱觛埰
//******************************************************************
BOOL NPC_TransmigrationDelPetDel(int meindex,int talker,int petsel)
{

	int petindex;
	int fd = getfdFromCharaIndex( talker );
	char msgbuf[64];
	char szPet[128];
	int defpet;
	
	petindex = CHAR_getCharPet( talker, petsel);

    if( !CHAR_CHECKINDEX(petindex) ) return FALSE;
	
	/*--鼠摯す籤溘誑橋    埵�掁�妐蚗哱禱竣�桫痡�  --*/
	if( CHAR_getWorkInt( CONNECT_getCharaindex( fd),
   	                     CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return FALSE;
	/*--鼠摯妐蚗哱誑泬哱鳴凝汔竣凝摯鏍攝蚗袲--*/
	defpet = CHAR_getInt( talker, CHAR_DEFAULTPET);
	if(defpet == petsel){
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}

	snprintf( msgbuf,sizeof( msgbuf), "蝠堤%s﹝",
									CHAR_getChar( petindex, CHAR_NAME));	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);
	// 妐蚗哱禱鱗  媃倜痲精
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* す籤溘   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"TenseiDel",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
	/*   з  凝�殏梊� */
	CHAR_setCharPet( talker, petsel, -1);
	CHAR_endCharOneArray( petindex );
	snprintf( szPet, sizeof( szPet ), "K%d", petsel);
	// 犍媃笢妐蚗哱筒  ↓淏筒邞堇僑崹堎
	CHAR_sendStatusString( talker, szPet );
	return TRUE;
}

//***********************************************************
//	觛埰妐蚗哱禱潺埰��
//***********************************************************
BOOL NPC_TransmigrationDelPet(int meindex, int talker)
{
	int petsel;
	int petindex;
	int num;
	int j, k;
//	int petid[4] = {1, 2, 3, 4}; //妐蚗哱摯    禱炩擭
	int petid[4] = {693, 694, 695, 696}; //妐蚗哱摯    禱炩擭
//	int petidfinal[4] = {11, 12, 13, 14}; //妐蚗哱摯    禱炩擭
	int petidfinal[4] = {693, 694, 695, 696}; //妐蚗哱摯    禱炩擭
	int petwork[4] ={ -1, -1, -1, -1};
	
	num = CHAR_getInt(talker, CHAR_TRANSMIGRATION);
	num--;
	if(num  < 4 ){
		//炩擭妐蚗哱禱忒埱趙笢堎凝妐蚗哱鏍攝蚗袲
		for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++){
			petindex = CHAR_getCharPet( talker, petsel);
			if( petindex == -1  )  continue;
			
			//妐蚗哱摯    毀郖灉凝ㄑ
			if(CHAR_getInt( petindex, CHAR_PETID) != petid[num]) continue;
			//喫喫竘ぁ拻倜�桯姨動該檗�毀    敁  茯
			break;
		}
	
		if(petsel  == CHAR_MAXPETHAVE) return FALSE;
		if(NPC_TransmigrationDelPetDel(meindex, talker, petsel) == FALSE) return FALSE;
	
	}
#ifdef _TRANS_6
	else if( num == 4 )
#else
	else
#endif
	{
		//察樊  
		for(j = 0; j < 4; j++){
			//炩擭妐蚗哱禱忒埱趙笢堎凝妐蚗哱鏍攝蚗袲
			for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++){
				petindex = CHAR_getCharPet( talker, petsel);
				if( petindex == -1  )  continue;
				
				//妐蚗哱摯    毀郖灉凝ㄑ
				if(CHAR_getInt( petindex, CHAR_PETID) != petidfinal[j]) continue;
				
				petwork[j] = petsel;
				//喫喫竘ぁ拻倜�桯姨動該檗�毀    敁  茯
				break;
			}
		}
		for(k=0; k < 4 ;k++)
		{
			if(petwork[k] == -1) return FALSE;
		}
		for(k = 0 ; k < 4 ;k++){
			if(NPC_TransmigrationDelPetDel(meindex, talker, petwork[k]) == FALSE) return FALSE;
		}
	}
#ifdef _TRANS_6
	else{
		/*int lasttalkelder = 2;//拫薯
		if( CHAR_getInt( talker, CHAR_LASTTALKELDER) == 1 )//鎖璨饑佪
			lasttalkelder = 112;//翮掀
		else if( CHAR_getInt( talker, CHAR_LASTTALKELDER) == 2 )//樓樓
			lasttalkelder = 102;//親親嫌
		else if( CHAR_getInt( talker, CHAR_LASTTALKELDER) == 3 )//縐糧坳饒
			lasttalkelder = 34;//哏皎
			*/

		//炩擭妐蚗哱禱忒埱趙笢堎凝妐蚗哱鏍攝蚗袲
		for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++){
			int petid;
			petindex = CHAR_getCharPet( talker, petsel);
			if( petindex == -1  )  continue;
			
			//妐蚗哱摯    毀郖灉凝ㄑ
			if(CHAR_getInt( petindex, CHAR_TRANSMIGRATION) != 2) continue;
			petid=CHAR_getInt( petindex, CHAR_PETID );
			if (petid == 2) break;
			if (petid == 112) break;
			if (petid == 102) break;
			if (petid == 34) break;
			//喫喫竘ぁ拻倜�桯姨動該檗�毀    敁  茯
		}
		if(petsel  == CHAR_MAXPETHAVE) return FALSE;
		if(NPC_TransmigrationDelPetDel(meindex, talker, petsel) == FALSE) return FALSE;
	}
#endif
	return TRUE;
}

#ifdef _PET_TRANS
void NPC_PetTransMan_selectWindow(int meindex,int toindex,int num,int select)
{ 
	char token[1024],buf[1024],buf1[256];
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char s_tensei[128];
	char errchar[][56]={"errormy","erroritem","errorpet","failmsg"};
	int buttontype = 0, windowtype = 0, windowno = 0, errtype = 0;
	int petindex = 0,i=1;
	char *petname;
	BOOL tenflg = FALSE;
	int fd = getfdFromCharaIndex( toindex);

	if( fd == -1 ) {
		print( "getfd err\n");
		return;
	}

	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("Tensei:GetArgStrErr");
		return;
	}

	sprintf(s_tensei,"PETTRANS:");

	//掀勤npc抶趕
	while(getStringFromIndexWithDelim( npcarg,"END",i,buf,sizeof( buf)) != FALSE)	{
		i++;
  		//�蝜�沭璃睫磁寀 tenflg 扢峈TRUE
  		if( strstr(buf,s_tensei) != NULL) {
			tenflg = TRUE;
			break;
  		}
	}
  
	if( CHAR_getWorkInt( toindex, CHAR_WORKSHOPRELEVANT) < 0 )
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
  
	token[0] = '0';		
 
	switch(num)	{
		case 0:		//勤趕
			if( NPC_Util_GetStrFromStrWithDelim( buf,"mainmsg", token,sizeof( token)) == NULL )	{
				print("\n ERROR buf str!!");
				return;
			}
			buttontype = WINDOW_BUTTONTYPE_NEXT;
			windowno = CHAR_WINDOWTYPE_PETTRANS_START;
			break;
		
		case 1:	//岆瘁�籀駘擼罔�				
			//蛌汜剒猁昜こ麼唾昜潰脤
			errtype = NPC_PetTransManCheck( meindex, toindex, 0);
			if( errtype >= 0 )
				errtype = 3;
	
			if( errtype >= 0 )	{
				if( NPC_Util_GetStrFromStrWithDelim( buf,errchar[ errtype], token,sizeof( token)) == NULL )	{
					print("\n ERROR str errchar !!");
					return;
				}
				buttontype = WINDOW_BUTTONTYPE_OK;
				windowtype = WINDOW_MESSAGETYPE_MESSAGE;
				windowno =  CHAR_WINDOWTYPE_PETTRANS_END;
				break;                                                
			}
			if( NPC_Util_GetStrFromStrWithDelim( buf,"selectmsg", token,sizeof( token)) == NULL )	{
				print("\n ERROR selectmsg !!");
				return;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 1);
			windowno = CHAR_WINDOWTYPE_PETTRANS_SELECT;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			buttontype=WINDOW_BUTTONTYPE_YESNO;
			break;

		case 2:		//恁唾
			if( NPC_Util_GetStrFromStrWithDelim( buf,"setpetmsg", token,sizeof( token)) == NULL )	{
				print("\n ERROR setpetmsg !!");
				return;
			}
			buttontype = WINDOW_BUTTONTYPE_NONE;
			windowtype = WINDOW_MESSAGETYPE_PETSELECT;
			windowno = CHAR_WINDOWTYPE_PETTRANS_SELPET;
			break;

		case 3:		//唾昜蛌汜	�毓�/�＋�
			if( NPC_Util_GetStrFromStrWithDelim( buf,"transmsg", buf1,sizeof( buf1)) == NULL ){
				print("\n ERROR transmsg !!");
				return;
			}
			strcpy( token,"\0");
			petindex = CHAR_getCharPet( toindex, select-1);
			petname = CHAR_getUseName( petindex);
			sprintf(token,	"\n岆[%s]羶渣勘ˋ \n\n %s",petname,buf1);

			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			buttontype = WINDOW_BUTTONTYPE_YESNO;
			windowno = CHAR_WINDOWTYPE_PETTRANS_MAIN1;
			break;

		case 4:	//唾昜蛌汜 CHAR_TRANSMIGRATION
			//蛌汜沭璃瓚剿
			petindex = CHAR_getCharPet( toindex, select-1);
			if( !CHAR_CHECKINDEX( petindex) ) {
				return;
			}
			if( CHAR_getInt( petindex, CHAR_FUSIONBEIT ) == 1 ) {
				sprintf(token, "\n\n�睆牊�腔唾昜祥夔蛌汜鉊ㄐ"); 
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
			}else
#ifdef _PET_2TRANS
			if( CHAR_getInt( toindex, CHAR_RIDEPET ) == select-1 /*&& CHAR_getInt( petindex, CHAR_TRANSMIGRATION ) > 0*/ ){//る傚笢腔唾昜祥夔蛌汜
				sprintf(token, "\n\nる傚笢腔唾昜祥夔蛌汜鉊ㄐ"); 	
			}else
#endif
			if( CHAR_getInt( petindex, CHAR_LV) < 80 )	{
				sprintf(token, "\n\n諉忳蛌汜蛅腦腔唾昜斛剒猁脹撰80眕奻鉊ㄐ"); 
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
			}else if( CHAR_getInt( petindex, CHAR_PETID) == 718 
#ifdef _PET_2TRANS
					  || CHAR_getInt( petindex, CHAR_PETID) == 401 
#endif
			){
				sprintf(token, "\n\n涴硐唾昜祥夔蛌汜鉊ㄐ"); 
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
#ifdef _PET_2TRANS
			}else if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION ) > 1 )	{
#else
			}else if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION ) > 0 )	{
#endif
				sprintf(token, "\n\n涴硐唾昜眒冪諉忳徹蛅腦賸鉊ㄐ"); 
				CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
				//潰脤唾昜岆瘁笭葩蛌汜
			}else	{	//	蛌汜�洷�
				petindex = CHAR_getCharPet( toindex, select-1);
				if( NPC_PetTransManStatus( meindex, toindex, petindex) == FALSE )
					return;
				if( NPC_Util_GetStrFromStrWithDelim( buf,"main1msg", token,sizeof( token)) == NULL )
					return;
			}

			buttontype = WINDOW_BUTTONTYPE_OK;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			windowno =  CHAR_WINDOWTYPE_PETTRANS_END;	//賦旰
			break;
  
		case 5:		//唾昜蛌汜�＋�	
			if( NPC_Util_GetStrFromStrWithDelim( buf,"cancelmsg", token,sizeof( token)) == NULL )
				return;
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, 0);
			buttontype = WINDOW_BUTTONTYPE_OK;
			windowtype = WINDOW_MESSAGETYPE_MESSAGE;
			windowno =  CHAR_WINDOWTYPE_PETTRANS_END;	//賦旰
			break;	
	
		default:
			break;
	}
	lssproto_WN_send( fd, windowtype, buttontype, windowno, CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);

}

int NPC_PetTransManCheck( int meindex, int toindex, int select)
{
	//char errchar[][56]={"errormy","erroritem","errorpet","failmsg"};
	int i;
	int petindex = 0;
	char token[256];
	int PetEvent_no[3]={4,69,70};
	//int PetEvent_no[3]={4,4,4};

	int petNum = 0;
	//唾昜蛌汜沭璃
	if( CHAR_getInt( toindex, CHAR_LV ) < 80 )	{
	  return 0;
	}else	{
		//潰脤�恄騊騙LAG 岆瘁眒俇傖唾蛌剒猁腔�恄�
	  for(i = 0; i < 3 ;i++)	{
		  if( NPC_EventCheckFlg( toindex, PetEvent_no[i] ) == FALSE)	{
			return 0;
		  }
	  }
	//潰脤垀剒(唾昜/儕鍾) & 潰脤垀剒昜こ
	  for(i=0 ; i < CHAR_MAXPETHAVE ; i++)	{
		petindex = CHAR_getCharPet( toindex, i);
		if( petindex == -1  )
			continue;
		//潰脤唾昜腔 ID 岆瘁峈鎖濘滑捇 1479 ID 峈718
		if(CHAR_getInt( petindex, CHAR_PETID) == 718
#ifdef _PET_2TRANS
			|| CHAR_getInt( petindex, CHAR_PETID) == 401
#endif
			){
			petNum ++;
		  if( CHAR_getInt( petindex, CHAR_LV) > 79 )	{
			return 0;
		  }else	if( CHAR_getInt( petindex, CHAR_LV) == 79 ) {
			if( strcmp( CHAR_getChar( petindex, CHAR_OWNERCDKEY), CHAR_getChar( toindex, CHAR_CDKEY) ) ||
				strcmp( CHAR_getChar( petindex, CHAR_OWNERCHARANAME), CHAR_getChar( toindex, CHAR_NAME) ))	{
				sprintf( token,"斕旯奻腔鎖濘滑捇ㄛ甜祥岆斕腔陛ㄐ");
				CHAR_talkToCli( toindex, -1, token,  CHAR_COLORWHITE);
				return 0;
			}
		  }
		}
	  }   

	  if( petNum > 1 )	{
		sprintf( token,"斕崋頗衄饒欒嗣鎖濘滑捇儸ˋ");
		CHAR_talkToCli( toindex, -1, token,  CHAR_COLORWHITE);
		print("\n error PetTrans : (PETID = 718) > 1 !!");
		return 0;
	  }else if( petNum != 1 )	{
		  print("\n 唾蛌楷汜渣昫 !!");
		  return 0;
	  }
	}
	return -1;
}

BOOL NPC_PetTransManStatus( int meindex, int toindex, int petNo)
{
	int petindex;
	int LevelUpPoint = 0,petrank = 0;
	int i,j,k,itemindex,itemID,count;
	int vital1,str1,tgh1,dex1;
	int vital2,str2,tgh2,dex2;
	char token[128];
	int total1,total2,total,petLV,ans;
	int petID,enemynum,ret;
	int work[4]={0,0,0,0};
	struct PetTransDelItem {
		int minItemID;
		int maxItemID;
	}DelItem[3] = { {19629,19632} , {19672,19687} , {19625,19628} };
#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
#ifdef _PET_2TRANS
	int tran = CHAR_getInt ( petNo, CHAR_TRANSMIGRATION );
	int tpetidx1,tpetidx2;
	if( tran == 0 ){
		tpetidx1 = 718;
		tpetidx2 = 401;
	}
	else{
		tpetidx1 = 401;
		tpetidx2 = 401;
	}
#endif

	for(i=0 ; i < CHAR_MAXPETHAVE ; i++)	{
		petindex = CHAR_getCharPet( toindex, i);
		if( petindex == -1  )
			continue;
		if(
#ifdef _PET_2TRANS
			CHAR_getInt( petindex, CHAR_PETID) == tpetidx1
			|| CHAR_getInt( petindex, CHAR_PETID) == tpetidx2
#else
			CHAR_getInt( petindex, CHAR_PETID) == 718
#endif
			){
			if( CHAR_getInt( petindex, CHAR_LV) == 79 ){
				if( strcmp( CHAR_getChar( petindex, CHAR_OWNERCDKEY), CHAR_getChar( toindex, CHAR_CDKEY) ) ||
					strcmp( CHAR_getChar( petindex, CHAR_OWNERCHARANAME), CHAR_getChar( toindex, CHAR_NAME) ))	{
					continue;
				}
				break;
			}
		}
	}
	if(i == CHAR_MAXPETHAVE)	{
		print("\n 唾昜蛌汜楷汜渣昫 !!");
		return FALSE;
	}

	LevelUpPoint = CHAR_getInt( petindex, CHAR_ALLOCPOINT );
	petrank = CHAR_getInt( petindex, CHAR_PETRANK );

	vital1 = (float)(( LevelUpPoint >> 24 ) & 0xFF);
	str1 = (float)(( LevelUpPoint >> 16 ) & 0xFF);
	tgh1 = (float)(( LevelUpPoint >> 8 ) & 0xFF);
	dex1 = (float)(( LevelUpPoint >> 0 ) & 0xFF);
	total1 = ( vital1 + str1 + tgh1 + dex1 );
	if( total1 > 150 )	total1 = 150;
	if( total1 < 0 ) total1 = 0;
	LevelUpPoint = CHAR_getInt( petNo, CHAR_ALLOCPOINT );
	petrank = CHAR_getInt( petNo, CHAR_PETRANK );
	petLV = CHAR_getInt( petNo, CHAR_LV);
	vital2 = (float)(( LevelUpPoint >> 24 ) & 0xFF);
	str2 = (float)(( LevelUpPoint >> 16 ) & 0xFF);
	tgh2 = (float)(( LevelUpPoint >> 8 ) & 0xFF);
	dex2 = (float)(( LevelUpPoint >> 0 ) & 0xFF);
	total2 = ( vital2 + str2 + tgh2 + dex2 );

#ifdef _PET_2TRANS
	ans = NPC_PetTransManGetAns( total1, total2, petLV, petrank, CHAR_getInt( petNo, CHAR_TRANSMIGRATION ) );
#else
	ans = NPC_PetTransManGetAns( total1, total2, petLV, petrank);
#endif
	total = total1 + (total2*4);

	work[0] = ( ans * ( vital1 + (vital2*4) ) ) / total;
	work[1] = ( ans * ( str1 + (str2*4) ) ) / total;
	work[2] = ( ans * ( tgh1 + (tgh2*4) ) ) / total;
	work[3] = ( ans * ( dex1 + (dex2*4) ) ) / total;
//�｛rray=========================
	enemynum = ENEMY_getEnemyNum();
	for(i=0;i<CHAR_MAXPETHAVE;i++)	{
		if( CHAR_getCharPet( toindex , i) == petNo)	{
			break;
		}
	}

	if( CHAR_MAXPETHAVE == i )	{
		return FALSE;
	}

	petID = CHAR_getInt( petNo, CHAR_PETID);
	for(k=0;k<enemynum;k++)	{
		if(ENEMY_getInt(k, ENEMY_TEMPNO) == petID )
			break;
	}
	if(k == enemynum)	{
		print("\n test *-7 : petID=%d", petID);
		return FALSE;
	}
	{
		BOOL FINDs = FALSE;
#ifdef _PET_2TRANS
		tpetidx1 = CHAR_getInt( petindex, CHAR_PETID);
#endif		
		//刉壺旯奻垀衄鎖唾
		for(j=0 ; j < CHAR_MAXPETHAVE ; j++){
			int pet_index = CHAR_getCharPet( toindex, j);
			if( pet_index == -1  )  continue;
			if( 
#ifdef _PET_2TRANS
				CHAR_getInt( pet_index, CHAR_PETID) == tpetidx1
#else
				CHAR_getInt( pet_index, CHAR_PETID) == 718
#endif
				){
				if( NPC_TransmigrationDelPetDel( meindex, toindex, j) == FALSE){
					return FALSE;
				}else	{
					FINDs = TRUE;
				}
			}
		}
		if( FINDs == FALSE )
			return FALSE;
	}

	ret = GetNewPet( toindex , petNo, k, work);
	if( ret < 0 )
		return FALSE;
	for(count=0;count<CHAR_MAXPETHAVE;count++)	{
		if(CHAR_getCharPet(toindex,count) == ret)	{	
			CHAR_send_K_StatusString(toindex,count,CHAR_K_STRING_HP|CHAR_K_STRING_AI);
		}
	}
	{
		
			LogPetTrans( 
				CHAR_getChar( toindex, CHAR_CDKEY ),
				CHAR_getChar( petNo, CHAR_UNIQUECODE),
				CHAR_getChar( petindex, CHAR_UNIQUECODE),//鎖濟
				CHAR_getChar( toindex, CHAR_NAME),
				CHAR_getInt( toindex, CHAR_FLOOR),
				CHAR_getInt( toindex, CHAR_X), 
				CHAR_getInt( toindex, CHAR_Y), 
				CHAR_getInt( petNo, CHAR_PETID),
				CHAR_getChar( petNo, CHAR_NAME),
				petLV, petrank,
				vital2, str2, tgh2, dex2, total2,
				CHAR_getInt( petindex, CHAR_PETID),
				CHAR_getChar( petindex, CHAR_NAME),
				vital1, str1, tgh1, dex1, total1,
				work[0], work[1], work[2], work[3], ans, CHAR_getInt ( petNo, CHAR_TRANSMIGRATION ) );
	}
    sprintf(token,"%s 諉忳賸儕鍾卼腔蛅腦ㄐ", CHAR_getUseName( ret));
	CHAR_talkToCli( toindex, -1,token,CHAR_COLORWHITE);
	sprintf( token, "K%d", i );
	CHAR_sendStatusString( toindex , token );
 
//--------------------------------------------------------------------------
	//刉壺昜こ	itemindex,itemID;
	for( i =0 ; i < CHAR_MAXITEMHAVE ; i++ ){
		itemindex = CHAR_getItemIndex( toindex , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			itemID = ITEM_getInt(itemindex ,ITEM_ID );
			for( j = 0;j < 3;j++)	{
				if( ( itemID >= DelItem[j].minItemID ) && ( itemID <= DelItem[j].maxItemID ) )	{
							LogItem(
								CHAR_getChar( toindex, CHAR_NAME ),
								CHAR_getChar( toindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
								itemindex,
#else
								ITEM_getInt( itemindex, ITEM_ID),
#endif
								"WarpManDelItem",
								CHAR_getInt( toindex, CHAR_FLOOR),
								CHAR_getInt( toindex, CHAR_X ),
 								CHAR_getInt( toindex, CHAR_Y ),
								ITEM_getChar( itemindex, ITEM_UNIQUECODE),
								ITEM_getChar( itemindex, ITEM_NAME),
								ITEM_getInt( itemindex, ITEM_ID)
							);
					CHAR_DelItem( toindex, i);
				}

			}//for(j)
		}
	}

	NPC_NowEventSetFlg( toindex, 71 );

	return TRUE;
}



// shan add
void s_eventsetend( int charaindex, int shiftbit )
{
#ifdef _ADD_NEWEVENT              // WON 多增加資料
	int event_num = 8;
#else
	int event_num = 6;
#endif
	int point;	
	int array;
	int shift;

	array = shiftbit / 32;
	shift = shiftbit % 32;
		
	if( array>=event_num ){
		//print("渣昫ㄐㄐ垀扢腔�恄鮿麇穇鉭籤捖牲�  峓(0~%d)﹝",32*event_num-1);			
		return;
	}
	point = CHAR_getInt( charaindex, CHAR_ENDEVENT+array);
	point = point | (1<<shift);
	CHAR_setInt( charaindex, CHAR_ENDEVENT+array, point);
}
#endif


