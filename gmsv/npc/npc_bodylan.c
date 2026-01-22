#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_windowhealer.h"




/*
 *詞��尨溢贖溘璃斛↓喇禱窅窇��  伀埰堎NPC
 * 竘囀  媃凝崹�桫憶觕掁�鼠摯鏤畛騷媕↓勗
 * 喫摯NPC摯INDEX禱瞀凎埰堎��
 *
 *つ��NPC摯薛堇ぁ囮袲で砫璃誑堣埱倜�掁�囮袲で砫璃禱厔喫媃倜鏤畛騷媕↓誑
 *NPC摯INDEX禱瞀  媃趙笢堎凝鏍攝蚗袲
 *鏍攝蚗袲    埵�梪揧棵抄芢刳鉏�摯で↓艇璃筒鏍攝蚗袲埰堎
 *恦雇摯囮袲で砫璃禱媃趙笢倜�桼鯜謐杻嚏齞動倓�璃哱禱囮蚗鏤踏鞠堎��
 *庈坳璃哱囮蚗鏤  ��蠈で↓艇璃筒    埵�梤黃羹啗鋷鰴葽央劗騣魌齉�
 *
 */

enum {
	BODYLAN_E_COMMANDNUM = CHAR_NPCWORKINT1,	// 昡悃璃迶摯荇踏
};


// 坳騷璃迶坳さ↓迶
enum{
	BODYLAN_WIN_FIRST,
	BODYLAN_WIN_LAST_GOOD,
	BODYLAN_WIN_LAST_NG,
	BODYLAN_WIN_GOOD_NO,
	BODYLAN_WIN_ALREADY,
	BODYLAN_WIN_NOT_PREEVENT,
	BODYLAN_WIN_END
};

static void NPC_BodyLan_Profit( int meindex, int playerindex );

static void NPC_BodyLan_Window(
	int meindex,
	int talkerindex,
	int mode
);


/*********************************
* 疐趵窐  
*********************************/
BOOL NPC_BodyLanInit( int meindex )
{

	char szP[256], szArg[4096];
	char buf[256];
	int i, needSeq;

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEEVENT );

    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:羶衄竘杅(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return FALSE;
    }


	// で↓艇璃筒禱潺埰
	if( NPC_Util_GetStrFromStrWithDelim( szArg, "Act", szP, sizeof( szP ) ) == NULL ){
		print( "npc_bodylan:雄釬恅趼蹈奾帤扢隅(%s)\n",	szArg );
		return FALSE;
	}

	for( i = 0 ; ; i ++ ){
		//   菺埵棹  
		if( getStringFromIndexWithDelim( szP, ",", i, buf, sizeof( buf)) != FALSE ){
			needSeq = atoi(buf);
			// 悃騷圖筒誑  倜�梜薱襓�ぁ
			if( needSeq < 0 ){
				if( i <= 0 ){
					print( "npc_bodylan:雄釬蹈奾帤扢隅(%s)\n", szArg );
				}
				//     倳禱掛蚗哱
				CHAR_setWorkInt( meindex, BODYLAN_E_COMMANDNUM, i );
				break;
			}else{
				// 鳴↓鏤媃趙倳窇堎
			}
		}else{
			if( i <= 0 ){
				print( "npc_bodylan:雄釬蹈奾帤扢隅(%s)\n", szArg );
			}
			//     倳禱掛蚗哱
			CHAR_setWorkInt( meindex, BODYLAN_E_COMMANDNUM, i );
			break;
		}
	}



    return TRUE;

}




/*********************************
*   媃凝崹�桫憶袬敯啄�  
*********************************/
void NPC_BodyLanTalked( int meindex , int talkerindex , char *szMes ,int color )
{
	char szP[256], szArg[4096];
	int EventNo = -1,Pre_Event = -1;

    /* 設定相關參數處理 */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/* 角色動畫重置 */
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2 )
	{
		return;
	}

	// 礎倳  棬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:羶衄竘杅(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }else{
		// 騷穫璃哱  蠕噁  
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "EventNo", szP, sizeof( szP ) ) != NULL ){
			EventNo = atoi(szP);
		}
		// 痀鞳  菺騷穫璃哱  蠕噁  
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Pre_Event", szP, sizeof( szP ) ) != NULL ){
			Pre_Event = atoi(szP);
		}
	}

	// 痀鞳勗  菺騷穫璃哱  蠕誑堣躂��
	if( Pre_Event >= 0 ){
		// 騷穫璃哱勗葡媃趙汔竣媃趙笢堎凝鏍攝蚗袲
		if( NPC_EventCheckFlg( talkerindex, Pre_Event ) == FALSE ){
			//   菺騷穫璃哱禱袲惉囮媃趙笢埵凝埱倜�梜蟦停撻灠�
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_NOT_PREEVENT );
			return;
		}
	}
	// 騷穫璃哱  蠕誑堣躂��
	if( EventNo >= 0 ){
		// 騷穫璃哱勗葡媃趙汔竣媃趙笢堎凝鏍攝蚗袲
		if( NPC_EventCheckFlg( talkerindex, EventNo ) == TRUE ){
			//   埱趙笢倜�梜蟦停撻灠�
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_ALREADY );
			return;
		}
	}

	// 鞳樊喫摯鏤畛騷媕↓毀猷覕敁婘埱趙笢倜凝
	if( CHAR_getWorkInt( talkerindex, CHAR_WORKTRADER ) == meindex ){

		// 踏�梇槸嬣釆�迶誑蠈  埬  媃趙笢倜��
		if( CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT )
		 >= CHAR_getWorkInt( meindex, BODYLAN_E_COMMANDNUM )
		){
			// 埬  ∪∪��    凝ㄑ
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_LAST_GOOD );
			return;
		}else{
			// 謄  
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_LAST_NG );
			return;
		}
	}else{
		// 疐誧趙埵摯ぁ斐窇堎
		// 鏤畛騷媕↓勗猷覕摯騷璃溢蚗袲筒禱瞀凎踏鞠堎
		CHAR_setWorkInt( talkerindex, CHAR_WORKTRADER, meindex );
		// で↓艇璃筒毀  疐凝�梇槻棑撘�
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1 );

		// 鼠摯傷掛惉啞敁凝婘堎迋拻凝埵堣＊＊＊
		NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_FIRST );
		return;
	}
}


/*=======================================
 * watch 窐  
 *======================================*/
void NPC_BodyLanWatch(
	int objmeindex,
	int objmoveindex,
    CHAR_ACTION act,
    int x,
    int y,
    int dir,
    int* opt,
    int optlen
)
{
	char szP[256], szArg[4096];
	char buf[256];
	int actindex;
	int meindex;
	int seqNo, needSeq;

	// す籤溘袲淏雄翻毀惉淏↓璃
	if( OBJECT_getType(objmoveindex) != OBJTYPE_CHARA ) return;
	actindex = OBJECT_getIndex(objmoveindex);
	// 鏤畛騷媕↓雄翻毀惉淏↓璃
	if( CHAR_getInt( actindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) return;

	// 猷覕摯騷璃溢蚗袲筒
	meindex = OBJECT_getIndex(objmeindex);

	// 猷覕摯INDEX禱瞀  媃趙笢堎凝ㄑ笢埵崹躂�屻魙�↓璃
	if( CHAR_getWorkInt( actindex, CHAR_WORKTRADER ) != meindex ){
		return;
	}

	// 汔喫竘ぁ囮袲で砫璃禱媃趙笢堎凝
	seqNo = CHAR_getWorkInt( actindex, CHAR_WORKSHOPRELEVANT );
	//   埵鳹譴毀  疐凝��
	if( seqNo < 1 )seqNo = 1;

	// 礎倳  棬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:羶衄竘杅(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }


	// で↓艇璃筒禱潺埰
	if( NPC_Util_GetStrFromStrWithDelim( szArg, "Act", szP, sizeof( szP ) ) == NULL ){
		print( "npc_bodylan:雄釬恅趼蹈奾帤扢隅(%s)\n",	szArg );
		return;
	}

	//   菺埵棹  
	if(getStringFromIndexWithDelim( szP, ",", seqNo, buf, sizeof( buf)) != FALSE ){
		needSeq = atoi(buf);
	}else{
		// 埵殽凝埵凝埱倜��  疐凝��
		CHAR_setWorkInt( actindex, CHAR_WORKSHOPRELEVANT, 1 );

//		print( "酕徹芛賸﹝隙善郔場﹝\n", seqNo, needSeq );
		// 笰凝婘堎迋拻凝＊＊＊
		return;
	}

	// ぽ樊  菺埵囮袲で砫璃誑郖窙媃倜
	if( needSeq == act ){
		// 郖窙媃倜�蛦蟤噱�    凝
//		print( "傖髡\(%d棒杅岆%d)\n", seqNo, needSeq );
		seqNo ++;
		if( seqNo >= CHAR_getWorkInt( meindex, BODYLAN_E_COMMANDNUM ) ){
//			print( "婓森賦旰﹝\n" );
		}
		// 喫摯痾  禱瞀凎
		CHAR_setWorkInt( actindex, CHAR_WORKSHOPRELEVANT, seqNo );
	}else{
		// 謄  媃倜鳹譴毀  疐凝�欶忍�  媃
		CHAR_setWorkInt( actindex, CHAR_WORKSHOPRELEVANT, 1 );
//		print( "(%d棒杅岆%d)\n", seqNo, needSeq );
	}

}



//********* 埬  鄹摯��     *********
static void NPC_BodyLan_Profit( int meindex, int playerindex )
{
	char szArg[4096], szP[256];
	int fl, x, y, pmode, i, subindex, parent;

	// 礎倳  棬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:羶衄竘杅(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }

	//*********************************************
	//
	//   ��    鼠摯兒�蛪央劗鯠珊�
	//
	//*********************************************
	if( NPC_Util_GetStrFromStrWithDelim( szArg, "Warp", szP, sizeof( szP ) ) != NULL ){
		// ��    勗極↓鏤�觛�  噁堎
		if( sscanf( szP, "%d,%d,%d", &fl, &x, &y ) == 3 ){
		}else{
			print( "npc_bodylan: 拸楊黍�▼梩駂饡�(%s)\n", szP );
			return;
		}
		// 蚕↓  贖蚻庌ぁ堎鳹譴碳潔忒極↓鏤
		pmode = CHAR_getWorkInt( playerindex, CHAR_WORKPARTYMODE );
		switch( pmode ){
		case 1: // 猷覕誑虮
			parent = playerindex;
			break;
		case 2: // 猷覕誑碳潔�袲岋思�  
			parent = CHAR_getWorkInt( playerindex, CHAR_WORKPARTYINDEX1 );
			break;
		default:
			// 蚕↓  贖啋崨埵凝埱倜�蛪葾郱笭篞央劗�
			CHAR_warpToSpecificPoint( playerindex, fl, x, y );
			return;
		}
		// 蠈劂極↓鏤
		for( i = 0; i < CHAR_PARTYMAX; i ++ ){
			subindex = CHAR_getWorkInt( parent, CHAR_WORKPARTYINDEX1+i );
			if( CHAR_CHECKINDEX( subindex ) == FALSE )continue;
			// 蚕↓  贖蠈劂極↓鏤
			CHAR_warpToSpecificPoint( subindex, fl, x, y );
		}
	}



}

#if 1
static void NPC_BodyLan_Window(
	int meindex,
	int talkerindex,
	int mode
)
{
	char token[1024];
	char escapedname[2048];
	char szArg[4096];
	char szP[256];
	int fd;
	int buttontype = 0, windowtype = 0, windowno = 0;

	if( CHAR_CHECKINDEX( talkerindex ) == FALSE )return;
	fd = getfdFromCharaIndex( talkerindex );

	// 礎倳  棬  
    if( NPC_Util_GetArgStr( meindex, szArg, sizeof( szArg ) ) == NULL ){
    	print( "npc_bodylan.c:羶衄竘杅(%s)\n",
    		CHAR_getChar(meindex,CHAR_NPCARGUMENT) );
    	return ;
    }

	szP[0] = 0;

	switch( mode ){
	  case BODYLAN_WIN_FIRST:
		//   疐勗  踏躂倜鳹譴摯掛惉啞
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "First", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:珨羲宎蔡趕腔恅趼羶衄怀��(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_YES|WINDOW_BUTTONTYPE_NO;
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_LAST_GOOD:
		//     ぁ諜搣煦埱倜��
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Good", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:湘勤奀佽腔恅趼羶衄怀��(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_YESNO;// YES|NO
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_LAST_NG:
		//     ぁ諜搣煦埱倜��
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Ng", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:湘渣奀佽腔恅趼羶衄怀��(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_GOOD_NO:
		// 諜搣鄹勗    禱す籤璃掛鳴
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Good_No", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:湘勤奀�＋�蔣こ腔恅趼羶衄怀��(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	  case BODYLAN_WIN_ALREADY:
		// 埰ぁ勗騷穫璃哱禱敖  媃趙笢倜鳹譴
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Good_No", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:岈璃賦旰奀腔恅趼羶衄怀��(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;	// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;

	case BODYLAN_WIN_NOT_PREEVENT:
		// 痀鞳勗  菺埵騷穫璃哱禱喫埵媃趙笢埵笢鳹譴
		if( NPC_Util_GetStrFromStrWithDelim( szArg, "Pre_Not", szP, sizeof( szP ) ) == NULL ){
			print( "npc_bodylan:?岈ヶ岈璃賦旰腔恅趼羶衄怀��(%s)\n",	szArg );
			return;
		}
		sprintf( token,"%s", szP );
		buttontype=WINDOW_BUTTONTYPE_OK;	// OK
	  	windowtype=WINDOW_MESSAGETYPE_MESSAGE;
	  	windowno=mode;
	  	break;
	  default:
	  	return;
	}

	makeEscapeString( token, escapedname, sizeof(escapedname));
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
void NPC_BodyLanWindowTalked(
	int meindex,
	int talkerindex,
	int seqno,
	int select,
	char *data
)
{


	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) return;

	switch( seqno){
	case BODYLAN_WIN_LAST_GOOD:	// 諜搣鄹勗OK棟踏躂倜��
		if(select==WINDOW_BUTTONTYPE_YES ){
			NPC_BodyLan_Profit( meindex, talkerindex );
			// 鏤畛騷媕↓勗猷覕摯騷璃溢蚗袲筒禱晲躂踏鞠堎
			CHAR_setWorkInt( talkerindex, CHAR_WORKTRADER, -1 );
			// で↓艇璃筒毀  疐凝�梇槻棑撘�
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1 );
		}else
		if( select == WINDOW_BUTTONTYPE_NO ){
			// 諜搣鄹勗す籤璃掛鳴踏躂倜��
			NPC_BodyLan_Window( meindex, talkerindex, BODYLAN_WIN_GOOD_NO );
			// 鏤畛騷媕↓勗猷覕摯騷璃溢蚗袲筒禱晲躂踏鞠堎
			CHAR_setWorkInt( talkerindex, CHAR_WORKTRADER, -1 );
			// で↓艇璃筒毀  疐凝�梇槻棑撘�
			CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 1 );
		}
		break;
	default:
		break;
	}

}



#endif

