#include "version.h"
#include <string.h>
#include "char.h"
#include "char_base.h"
#include "battle.h"
#include "battle_event.h"
#include "battle_item.h"
#include "battle_magic.h"
#include "item_event.h"
#include "log.h"
#include "anim_tbl.h"
#include "npcutil.h"
#include "magic_base.h"
#include "lssproto_serv.h"


int BATTLE_ItemUseDelete(
	int charaindex,
	int haveitemindex
)
{
	int itemindex;

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( ITEM_CHECKINDEX( itemindex ) == FALSE ) return 0;
	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 婓item腔log笢崝樓item靡備
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"BattleUse(桵須笢妏蚚裁腔耋撿)",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	CHAR_DelItemMess( charaindex, haveitemindex, 0);

	return 0;
}

#ifdef _IMPRECATE_ITEM
void ITEM_useImprecate( int charaindex, int toNo, int haveitemindex )
{
	int i;
	int battleindex, attackNo,itemindex;
	char *arg;
	char buf[256];

	struct tagImprecate {
		char fun[256];
		int intfun;
	};
	struct tagImprecate ImList[3] ={
		{"紸",BD_KIND_CURSE},{"塋",BD_KIND_BESTOW},{"蛅",BD_KIND_WISHES} };

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	if( (attackNo = BATTLE_Index2No( battleindex, charaindex )) < 0 ){
		print( "ANDY attackNo=%d\n", attackNo);
		return;
	}
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == NULL ){
		print( "ANDY ITEM id:%d=>arg err\n", ITEM_getInt( itemindex, ITEM_ID));
		return;
	}

	for( i=0; i<3; i++)	{
		if( strstr( arg, ImList[i].fun ) != 0 )	{
			char buf1[256];
			int kind,powers, rounds, HealedEffect;

			if( NPC_Util_GetStrFromStrWithDelim( arg, ImList[i].fun, buf, sizeof( buf)) == NULL )continue;
			kind = ImList[i].intfun;
			if( getStringFromIndexWithDelim( buf,"x", 1, buf1, sizeof( buf1)) == FALSE ) continue;
			powers = atoi( buf1);
			if( getStringFromIndexWithDelim( buf,"x", 2, buf1, sizeof( buf1)) == FALSE ) continue;
			rounds = atoi( buf1);
			HealedEffect = SPR_hoshi;
			BATTLE_ImprecateRecovery(
				battleindex, attackNo, toNo, kind, powers,
				rounds, SPR_item3, HealedEffect );
			CHAR_setItemIndex(charaindex, haveitemindex ,-1);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
			ITEM_endExistItemsOne( itemindex );
			break;
		}
	}
}
#endif

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery_Battle( int charaindex, int toNo, int haveitemindex )
{
	int battleindex, attackNo, itemindex;
	int turn=0, magicid, marray;
	char buf1[256];
	char *arg=NULL;

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return;
	if( (attackNo =  BATTLE_Index2No( battleindex, charaindex )) == -1 ) return;

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar( itemindex, ITEM_ARGUMENT );


	if( getStringFromIndexWithDelim( arg, ":", 2, buf1, sizeof(buf1)) ==FALSE ) return;
	turn = atoi( buf1);
	if( getStringFromIndexWithDelim( arg, ":", 1, buf1, sizeof(buf1)) ==FALSE ) return;

	if( strstr( buf1, "��" ) != NULL ){
	}else{
	}

	magicid = ITEM_getInt( itemindex, ITEM_MAGICID);
	marray = MAGIC_getMagicArray( magicid);
	if( marray == -1 ) return;


	if( IsBATTLING( charaindex ) == TRUE ){
		int i, status=-1;
		char *magicarg=NULL, *pszP=NULL;
#ifdef _PREVENT_TEAMATTACK //嫖噩忐..祥腕妏蚚菩源
		int battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
			//&& BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P 
			){
			if( BATTLE_CheckSameSide( charaindex, toNo) == 0 ){//祥肮晚
				int battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
				BATTLE_NoAction( battleindex, BATTLE_Index2No( battleindex, charaindex) );
				CHAR_talkToCli( charaindex, -1, "嫖噩忐..祥腕囥軑準俙模菩源﹝", CHAR_COLORYELLOW);
				return;
			}
		}
#endif
		
		magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
		pszP = magicarg;
		for( ;status == -1 && pszP[0] != 0; pszP++ ){
			for( i = 1; i < BATTLE_MD_END; i ++ ){
				if( strncmp( pszP, aszMagicDef[i], 2 ) == 0 ){
					status = i;
					pszP +=2;
					break;
				}
			}
		}
		if( status == -1 ) return;

		BATTLE_MultiMagicDef( battleindex, attackNo, toNo,
			status, turn, MAGIC_EFFECT_USER, SPR_difence );

		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 婓item腔log笢崝樓item靡備
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"BattleUse(桵須笢妏蚚裁腔耋撿)",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
		);
	}

	CHAR_DelItemMess( charaindex, haveitemindex, 0);
}
#endif

#ifdef _ITEM_USEMAGIC
void ITEM_useMagic_Battle( int charaindex, int toNo, int haveitemindex )
{
	int itemindex,itemmaxuse;
    char *arg=NULL;
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar( itemindex, ITEM_ARGUMENT );

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_JYUJYUTU );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, atoi(arg) );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, 0 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	MAGIC_DirectUse(
				charaindex,
				CHAR_GETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3 ),
				CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM2 ),
				CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3 )
	);

	itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
	if( itemmaxuse > 0 )
	    ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse-1 );
    else{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 婓item腔log笢崝樓item靡備
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"BattleUse(桵須笢妏蚚裁腔耋撿)",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
		);

	    CHAR_DelItemMess( charaindex, haveitemindex, 0);
	}
}
#endif

void ITEM_useRecovery_Battle( int charaindex, int toNo, int haveitemindex )
{
#ifdef _CHANGEITEMUSE	 // Syu ADD 覃淕桵須笢妏蚚蹋燴扢隅
	int power1 = 0;
#endif
	int power = 0, per = 0, HealedEffect=0;
	int battleindex, attackNo,itemindex, kind = BD_KIND_HP;
	char *p = NULL, *arg;
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
#ifdef _CHANGEITEMUSE	 // Syu ADD 覃淕桵須笢妏蚚蹋燴扢隅
	if( ((p = strstr( arg, "极" )) != NULL) && ((p = strstr( arg,"ァ")) != NULL))
	{
		kind = BD_KIND_HP_MP;
		p = strstr( arg,"ァ");
		if( sscanf( p+2, "%d", &power1 ) != 1 )
		{
			power1 = 0;
		}
		p = strstr( arg,"极");
	}
	else if( (p = strstr( arg, "极" )) != NULL )
#else
	if( (p = strstr( arg, "极" )) != NULL )
#endif
	{
		kind = BD_KIND_HP;
	}
	else if( (p = strstr( arg, "ァ" )) != NULL )
	{
		kind = BD_KIND_MP;
	}
	else
#ifdef _ITEM_UNBECOMEPIG
    if( (p = strstr( arg, "賤紿" )) != NULL ){
        kind = BD_KIND_UNBECOMEPIG;
		HealedEffect = 100608; //杻虴晤瘍
	}
	else
#endif
#ifdef _ITEM_LVUPUP
	if( (p = strstr( arg, "LVUPUP" )) != NULL ){
	    return;
	}
	else
#endif
#ifdef _ITEM_PROPERTY
    if( (p = strstr( arg, "PROPERTY" )) != NULL ){
	    kind = BD_KIND_PROPERTY;
		HealedEffect = 100608; //杻虴晤瘍
		if( strstr( arg, "+" ) )
			power = 1;
		else if( strstr( arg, "-" ) )
			power = 2;
	}
	else
#endif
#ifdef _ITEM_ADDPETEXP
    if( (p = strstr( arg, "GETEXP" )) != NULL ){
        return;
	}
	else
#endif
		return;

#ifdef _ITEM_UNBECOMEPIG
    if( HealedEffect != 100608 ){
#endif
	    if( sscanf( p+2, "%d", &power ) != 1 )
		    power = 0;
	    if( power <= 100 )
		    HealedEffect = SPR_heal;//SPR_hoshi
		else if( power <= 300 )
		    HealedEffect = SPR_heal2;
	    else
		    HealedEffect = SPR_heal3;
#ifdef _ITEM_UNBECOMEPIG
	}
#endif

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );
	if( attackNo < 0 )return;

#ifdef _CHANGEITEMUSE	 // Syu ADD 覃淕桵須笢妏蚚蹋燴扢隅
	BATTLE_MultiRecovery( battleindex, attackNo, toNo,
		kind, power, per, SPR_item3, HealedEffect , power1);
#else
	BATTLE_MultiRecovery( battleindex, attackNo, toNo,
		kind, power, per, SPR_item3, HealedEffect );
#endif
	{
		LogItem(
			CHAR_getChar( charaindex, CHAR_NAME ),
			CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 婓item腔log笢崝樓item靡備
			itemindex,
#else
       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"BattleUse(桵須笢妏蚚裁腔耋撿)",
	       	CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
        	CHAR_getInt( charaindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
		);
	}
	CHAR_DelItemMess( charaindex, haveitemindex, 0);
}

void ITEM_useStatusChange_Battle(
	int charaindex,
	int toNo,
	int haveitemindex
){
	int turn = 0, i;
	int battleindex, attackNo,itemindex, status = -1, Success = 15;
	int ReceveEffect;
	char *pszP = NULL, *arg;
	char szTurn[] = "turn";
	char szSuccess[] = "傖";

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	pszP = arg;
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 0; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	if( status == -1 ) return ;
	if( ( pszP = strstr( arg, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	if( ( pszP = strstr( arg, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;
	}else{
		ReceveEffect = SPR_hoshi;
	}
	BATTLE_MultiStatusChange( battleindex, attackNo, toNo,
		status, turn, SPR_item3, ReceveEffect, Success );
	BATTLE_ItemUseDelete( charaindex, haveitemindex );
}

void ITEM_useStatusRecovery_Battle(
	int charaindex, 	// 窅埱倜硞摯騷璃溢蚗袲筒
	int toNo, 			// 窅渦躂堎硞摯  蠕
	int haveitemindex 	// 窅竣硞摯囮騷  堜摯    蠕
){
	int i;
	int battleindex, attackNo,itemindex, status = -1;
	int ReceveEffect;
	char *pszP = NULL, *arg;

	// 囮騷  堜誑堣堎凝汔竣凝
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	// 蚕溘隍↓淏禱噁  
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	pszP = arg;
	// 嗚堅禱噁  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// 蠈塙忒堣堎摯ぁ兔凝�楖像�
		for( i = 0; i < BATTLE_ST_END; i ++ ){
			// 嗚堅鼀蚗淏惉凝ㄑ
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 嗚堅埵笢摯ぁ謄  
	if( status == -1 ) return ;

	//------- 喫喫凝�梩媎甂�   -----------
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	ReceveEffect = SPR_tyusya;	//   堎敁拻毀喫躂

	// 蠈劂
	BATTLE_MultiStatusRecovery( battleindex, attackNo, toNo,
		status, SPR_item3, ReceveEffect );

	// 橋    勗囮騷  堜誑觛窇倜鳹譴摯呬玊窐  
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}






void ITEM_useMagicDef_Battle(
	int charaindex,
	int toNo,
	int haveitemindex
)
{
	int turn = 0, i;
	int battleindex, attackNo,itemindex, status = -1;
	char *pszP = NULL, *arg;

	char szTurn[] = "turn";

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	pszP = arg;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			if( strncmp( pszP, aszMagicDef[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return ;

	if( ( pszP = strstr( arg, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	BATTLE_MultiMagicDef( battleindex, attackNo, toNo,
		status, turn, SPR_item3, SPR_difence );

	BATTLE_ItemUseDelete( charaindex, haveitemindex );


}






//--------------------------------------------------------------
//  蚕溘隍↓淏  脾囮騷  堜禱窅埱倜鳹譴摯窐  
//--------------------------------------------------------------
// 橋    摯鳹譴
void ITEM_useParamChange_Battle(
	int charaindex, 	// 窅埱倜硞摯騷璃溢蚗袲筒
	int toNo, 			// 窅渦躂堎硞摯  蠕
	int haveitemindex 	// 窅竣硞摯囮騷  堜摯    蠕
)
{
	int i;
	int battleindex, attackNo,itemindex;
	int pow, par = 0;
	int kind = -1;
	char *pszP = NULL, *arg;

	// 囮騷  堜誑堣堎凝汔竣凝
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	// 蚕溘隍↓淏禱噁  
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	pszP = arg;
	// 嗚堅禱噁  
	for( ;kind == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			// 嗚堅鼀蚗淏惉凝ㄑ
			if( strncmp( pszP, aszParamChange[i], 2 ) == 0 ){
				kind = i;
				pszP +=2;
				break;
			}
		}
	}
	// 嗚堅埵笢摯ぁ謄  
	if( kind == -1 ) return ;


	if( strstr( pszP, "%" ) ){	// 喫摯鳹譴毀×銓猀
		par = 1;
	}

	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 笰睽騷璃哱堣ど堎凝
		pow = 30;
	}

	//------- 喫喫凝�欶�   -----------
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	// 蠈劂
	BATTLE_MultiParamChange( battleindex, attackNo, toNo,
		kind, pow, par, SPR_item3, SPR_hoshi );


	// 橋    勗囮騷  堜誑觛窇倜鳹譴摯呬玊窐  
	BATTLE_ItemUseDelete( charaindex, haveitemindex );


}






//--------------------------------------------------------------
//  啞贖↓鳴迶鶂鍛  脾囮騷  堜禱窅埱倜鳹譴摯窐  
//--------------------------------------------------------------
// 橋    摯鳹譴
void ITEM_useFieldChange_Battle(
	int charaindex, 	// 窅埱倜硞摯騷璃溢蚗袲筒
	int toNo, 			// 窅渦躂堎硞摯  蠕
	int haveitemindex 	// 窅竣硞摯囮騷  堜摯    蠕
)
{
	int itemindex;
	char *pArg;

	// 囮騷  堜誑堣堎凝汔竣凝
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- 喫喫凝�欶�   -----------
	// 蚕溘隍↓淏禱噁  
	pArg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	// 蚕溘隍↓淏  笢摯ぁ謄  
	if( pArg == NULL )return ;

	BATTLE_FieldAttChange( charaindex, pArg );

	// 橋    勗囮騷  堜誑觛窇倜鳹譴摯呬玊窐  
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}


//--------------------------------------------------------------
//  鶂鍛  梱囮騷  堜禱窅埱倜鳹譴摯窐  
//--------------------------------------------------------------
// 橋    摯鳹譴
void ITEM_useAttReverse_Battle(
	int charaindex, 	// 窅埱倜硞摯騷璃溢蚗袲筒
	int toNo, 			// 窅渦躂堎硞摯  蠕
	int haveitemindex 	// 窅竣硞摯囮騷  堜摯    蠕
)
{
	int itemindex, battleindex, attackNo;


	// 囮騷  堜誑堣堎凝汔竣凝
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- 喫喫凝�欶�   -----------
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	// 蠈劂
	BATTLE_MultiAttReverse( battleindex, attackNo, toNo,
		SPR_item3, SPR_kyu );

	// 橋    勗囮騷  堜誑觛窇倜鳹譴摯呬玊窐  
	BATTLE_ItemUseDelete( charaindex, haveitemindex );


}


//--------------------------------------------------------------
//  縈憪凝�涳獌棆帝髕�倜鳹譴摯窐  
//--------------------------------------------------------------
// 橋    摯鳹譴
void ITEM_useRessurect(
	int charaindex, 	// 窅埱倜硞摯騷璃溢蚗袲筒
	int toNo, 			// 窅渦躂堎硞摯  蠕
	int haveitemindex 	// 窅竣硞摯囮騷  堜摯    蠕
)
{
	int itemindex, battleindex, attackNo, par = 0, pow = 0, ReceveEffect;
	char *pszP = NULL;


	// 囮騷  堜誑堣堎凝汔竣凝
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- 喫喫凝�欶�   -----------
	pszP = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	if( strstr( pszP, "%" ) ){	// 喫摯鳹譴毀×銓猀
		par = 1;
	}

	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 笰睽騷璃哱樊蜾埰堎凝ㄑ
		pow = 0;	// 兔摯鳹譴毀嗟蠈樊蜾
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	if( pow <= 0 ){
		ReceveEffect = SPR_fukkatu3;
	}else
	if( pow <= 100 ){
		ReceveEffect = SPR_fukkatu1;
	}else
	if( pow <= 300 ){
		ReceveEffect = SPR_fukkatu2;
	}else{
		ReceveEffect = SPR_fukkatu3;
	}

	// 蠈劂
	BATTLE_MultiRessurect( battleindex, attackNo, toNo,
		pow, par, SPR_item3, ReceveEffect );


	// 橋    勗囮騷  堜誑觛窇倜鳹譴摯呬玊窐  
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}



//--------------------------------------------------------------
//    翮  臚諜禱窅埱倜鳹譴摯窐  
//--------------------------------------------------------------
// 橋    摯鳹譴
void ITEM_useCaptureUp_Battle(
	int charaindex, 	// 窅埱倜硞摯騷璃溢蚗袲筒
	int toNo, 			// 窅渦躂堎硞摯  蠕
	int haveitemindex 	// 窅竣硞摯囮騷  堜摯    蠕
)
{
	int itemindex, battleindex, attackNo, pow = 5, ReceveEffect;
	char *pArg;

	// 囮騷  堜誑堣堎凝汔竣凝
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- 喫喫凝�欶�   -----------
	// 蚕溘隍↓淏禱噁  
	pArg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	// 蚕溘隍↓淏  笢摯ぁ謄  
	if( pArg == NULL )return ;

	if( sscanf( pArg, "%d", &pow ) != 1 ){
		// 笰睽騷璃哱樊蜾埰堎凝ㄑ
		pow = 5;
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	ReceveEffect = SPR_hoshi;

	// 蠈劂
	BATTLE_MultiCaptureUp( battleindex, attackNo, toNo,
		pow, SPR_item3, ReceveEffect );

	// 橋    勗囮騷  堜誑觛窇倜鳹譴摯呬玊窐  
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}
#ifdef _ITEM_CRACKER
void ITEM_useCracker_Effect( charaindex, toindex, haveitemindex)
{
	int itemindex,x,y,tofd;

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	// 潰脤昜こ
	if(!ITEM_CHECKINDEX(itemindex)) return;

    x = CHAR_getInt( charaindex, CHAR_X);
    y = CHAR_getInt( charaindex, CHAR_Y);
	
	CHAR_setMyPosition( charaindex, x, y, TRUE);
	CHAR_setWorkInt( charaindex, CHAR_WORKITEM_CRACKER, 1);
	CHAR_sendCrackerEffect( charaindex, 101125);
	//to client
	tofd = getfdFromCharaIndex( charaindex );
	lssproto_IC_send(tofd, x, y);
	//ь壺耋撿
	BATTLE_ItemUseDelete(charaindex,haveitemindex);
	CHAR_talkToCli(charaindex,-1,"耋撿秏囮賸﹝",CHAR_COLORYELLOW);

}
#endif
#ifdef _ITEM_REFRESH //vincent 賤壺祑都袨怓耋撿
void ITEM_useRefresh_Effect( charaindex, toindex, haveitemindex)
{
	int i,itemindex,ReceveEffect;
	char  *arg;
//	char szBuffer[128]="";
	int status = -1,attackNo,index2;
	int battleindex;
	char *pszP;

//print("\nvincent--ITEM_useRefresh_Effect");
	// 潰脤昜こ
	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	pszP = arg;
	// 嗚堅禱噁  
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// 兒凝�楖像�
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			// 嗚堅鼀蚗淏惉凝ㄑ
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
//print("\nvincent-->status:%d",status);
	// 嗚堅埵笢摯ぁ謄  
	if( status == -1 ) return;

	// 泬哱鳴  蠕
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	index2 = BATTLE_No2Index( battleindex, toindex);//掩僻笢醴梓眳index
	attackNo = BATTLE_Index2No( battleindex, charaindex );
//    defNo = BATTLE_Index2No( battleindex, index2 );
//print("\nvincent-->charaindex:%d,attackNo:%d,index2:%d,defNo:%d,",charaindex,attackNo,index2,defNo);

    /* す籤溘袲淏↓摯鐎    惉筒哱凝�殏梬� */
    CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);/* 囮騷  堜脾犍 */
//	if( CHAR_getWorkInt( charaindex, StatusTbl[status] ) > 0 ){
//		ReceveEffect = SPR_tyusya;
//	}else{
//		ReceveEffect = SPR_hoshi;
//	}
	ReceveEffect = SPR_tyusya;//湖渀
	BATTLE_MultiStatusRecovery( battleindex, attackNo, toindex,
		status, MAGIC_EFFECT_USER, ReceveEffect );
	/* 觛埰 */
	ITEM_endExistItemsOne( itemindex );
//////////////////////////
//	BATTLE_MultiList( battleindex, defNo, ToList );
//		 for( i = 0; ToList[i] != -1; i ++ ){
//		toindex = BATTLE_No2Index( battleindex, ToList[i] );
//
//			}

		

////////////////////////////
}
#endif
#ifdef _ITEM_ADDEXP	//vincent 冪桄枑汔
void ITEM_useAddexp_Effect( charaindex, toindex, haveitemindex)
{
	int itemindex,power,vtime;
	//,pindex
	char *p = NULL, *arg;
	char szBuffer[1024]="";

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);

	// 潰脤昜こ
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	if( (p = strstr( arg, "崝" )) != NULL ){
        sscanf( p+2, "%d", &power );
	}
	if( (p = strstr( arg, "煦" )) != NULL ){
		sscanf( p+2, "%d", &vtime );
	}
	if( p == NULL )return;
#ifdef _ITEM_ADDEXP2 // 秷彆奀潔褫眕濛樓
	{
		int point;
		point = CHAR_getWorkInt( charaindex, CHAR_WORKITEM_ADDEXP)
					* (int)(CHAR_getWorkInt( charaindex, CHAR_WORKITEM_ADDEXPTIME)/60);
		if( point >= 72000) {
			CHAR_talkToCli( charaindex, -1, "秷雌眳彆濛數奀潔摯虴彆眒湛奻癹﹝", CHAR_COLORYELLOW);
			return;
		}
		point += (power * vtime);
		point = min( point, 72000);
		vtime = (int)(point / power);
#ifdef _LOG_OTHER
		sprintf( szBuffer, "妏蚚秷雌眳彆 %d\t濛儅虴彆= 煦杅%d 夔薯%dㄔ 奀潔%d煦 ",
				ITEM_getInt( itemindex, ITEM_ID ), point, power, vtime);
		LogOther( 
			CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME),
			szBuffer );
#endif
		//CHAR_setInt( charaindex, CHAR_ADDEXPPOWER, power);
		//CHAR_setInt( charaindex, CHAR_ADDEXPTIME,vtime*60 );

		//sprintf(szBuffer, "聆彸捅洘ㄩ醴ヶ煦杅%d 奀潔%d鏃﹝", point, vtime*60);
		//CHAR_talkToCli(charaindex,-1,szBuffer,CHAR_COLORRED);
	}
#endif
	CHAR_setWorkInt( charaindex, CHAR_WORKITEM_ADDEXP, power);
	CHAR_setWorkInt( charaindex, CHAR_WORKITEM_ADDEXPTIME,vtime*60 );

	//ь壺耋撿
	BATTLE_ItemUseDelete(charaindex,haveitemindex);
	//sprintf(szBuffer, "悝炾冪桄腔夔薯枑汔賸%dㄔ", power);
	sprintf(szBuffer, "悝炾冪桄腔夔薯枑汔賸%dㄔㄛ奀虴呁牄%d煦笘﹝", power, vtime);
	CHAR_talkToCli(charaindex,-1,szBuffer,CHAR_COLORYELLOW);

}
#endif
//Terry add 2001/12/24
#ifdef _ITEM_FIRECRACKER
void ITEM_useFirecracker_Battle( charaindex, toindex, haveitemindex)
{
	int itemindex, battleindex, masteridx=-1, index2;
	
	char szWork[128];
#ifdef _FIX_FIRECRACKER
	int petid=-1, i=1;
	BOOL FINDPET=FALSE;
	char *buff1;
	char token[256], buf1[256];
#else
	int PetEscape = 0;
#endif

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	// 潰脤昜こ
	if(!ITEM_CHECKINDEX(itemindex)) return;

	battleindex = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEINDEX);
#ifdef _FIX_FIRECRACKER
	index2 = BATTLE_No2Index( battleindex, toindex);
	if( !CHAR_CHECKINDEX( index2) ) return;
	{
		int attackNo = BATTLE_Index2No( battleindex, charaindex );
		int safeSide = 0;
		int MySide_start, MySide_end;
		if( attackNo >= 10 )
			safeSide = 1;

		MySide_start = safeSide*SIDE_OFFSET;
		MySide_end = ((safeSide*SIDE_OFFSET) + SIDE_OFFSET);

		if( (toindex >= MySide_start) && (toindex<MySide_end) ){	//肮晚
			return;
		}
	}

	if( BATTLE_Index2No( battleindex, charaindex ) == toindex ){
		BATTLE_ItemUseDelete(charaindex,haveitemindex);//蚚婓赻旯
		return;
	}

	if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		petid = -1;//釬蚚婓�冼�
	}else if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEPET ) {
		masteridx = BATTLE_No2Index(battleindex,toindex-5);//釬蚚婓唾昜

		if( !CHAR_CHECKINDEX( masteridx) ) return;

		petid = CHAR_getInt(index2,CHAR_PETID);//唾昜id
	}else if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEENEMY ) {
		petid = CHAR_getInt(index2,CHAR_PETID);
	}

	if( (buff1 = ITEM_getChar( itemindex, ITEM_ARGUMENT)) == NULL ) return;

	memset( token, 0, sizeof( token));
	if( NPC_Util_GetStrFromStrWithDelim( buff1, "KPET", token, sizeof( token) ) == NULL) {
		print( "Can't get KPET: %s!!\n", buff1);
		return;
	}

	// won fix
	while( getStringFromIndexWithDelim( token,"_", i, buf1, sizeof( buf1)) != FALSE )	{
	//while( getStringFromIndexWithDelim( token,"|", i, buf1, sizeof( buf1)) != FALSE )	{
		i++;
		if( petid == atoi( buf1) ){//爛忤瓚梗
			FINDPET = TRUE;
			break;
		}
	}

	sprintf(szWork,"BB|a%X|w3|r%X|f0|d0|p0|FF|",BATTLE_Index2No(battleindex,charaindex),toindex);
	BATTLESTR_ADD(szWork);//隍堤晝蘿腔雄賒
//冞捅洘祫俙模
	if( FINDPET == TRUE )	{//�譠秈糐�
		char buf4[255];
		sprintf( buf4, "%s掩狣變賸ㄐ", CHAR_getChar( index2, CHAR_NAME));

		BATTLE_Exit(index2,battleindex);//燭羲桵須
		if( CHAR_CHECKINDEX( masteridx) ){
			CHAR_setInt(masteridx,CHAR_DEFAULTPET,-1);//拸統桵唾
			CHAR_talkToCli( masteridx,-1, buf4, CHAR_COLORYELLOW);
		}

		sprintf(szWork,"BQ|e%X|",toindex);//枅變雄賒
		BATTLESTR_ADD(szWork);
		CHAR_talkToCli( charaindex,-1, buf4, CHAR_COLORYELLOW);
	}else	{
		CHAR_talkToCli( charaindex, -1, "妦欒岈飲羶楷汜﹝", CHAR_COLORYELLOW);
	}
	BATTLE_ItemUseDelete( charaindex, haveitemindex);//刉壺晝蘿
#else

	index2 = BATTLE_No2Index(battleindex,toindex);
	// 掩旍善腔岆瘁峈唾昜
	if(CHAR_getInt(index2,CHAR_WHICHTYPE) == CHAR_TYPEPET) {
		// �蝜�掩旍腔岆爛忤
		if(CHAR_getInt(index2,CHAR_PETID) >= 901 && CHAR_getInt(index2,CHAR_PETID) <= 904){
			// �△藥膨窱齣�摽源腔褒伎腔index
			masteridx = BATTLE_No2Index(battleindex,toindex-5);
			// �蝜�勤桵腔岆俙模
			if(CHAR_getInt(masteridx,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) PetEscape = 1;
			else																									       print("\n爛忤婓郣菩奀堤珋");
		}
	}
	
	// �蝜�祥岆旍赻撩
	if(BATTLE_Index2No(battleindex,charaindex) != toindex)
	{
		sprintf(szWork,"BB|a%X|w3|r%X|f0|d0|p0|FF|",BATTLE_Index2No(battleindex,charaindex),toindex);
		BATTLESTR_ADD(szWork);
		if(PetEscape)
		{
			BATTLE_Exit(index2,battleindex);
			CHAR_setInt(masteridx,CHAR_DEFAULTPET,-1);
			sprintf(szWork,"BQ|e%X|",toindex);
			BATTLESTR_ADD(szWork);
			CHAR_talkToCli(masteridx,-1,"爛忤掩狣變賸ㄐ",CHAR_COLORWHITE);
			CHAR_talkToCli(charaindex,-1,"爛忤掩狣變賸ㄐ",CHAR_COLORWHITE);
		}
	}
	CHAR_talkToCli(charaindex,-1,"妦欒岈飲羶楷汜﹝",CHAR_COLORWHITE);
	CHAR_talkToCli(charaindex,-1,"耋撿秏囮賸﹝",CHAR_COLORWHITE);
	// 妏蚚摽耋撿秏囮
	BATTLE_ItemUseDelete(charaindex,haveitemindex);
#endif
}
#endif

//Terry end




//ァ橈袨怓隙葩騵薯,葩樓葩魂(嫖,噩,忐)儕鍾 統杅肮藹楊(嫖,噩,忐)儕鍾,筍硐夔婓桵須眳妏蚚,蚚俇憩羶賸 
//統杅 瞰��:騵薯硉 柲 turn 1
#ifdef  _FEV_ADD_NEW_ITEM			// FEV ADD 崝樓葩魂忐儕
void ITEM_ResAndDef( int charaindex, int toindex, int haveitemindex )
{
	int itemindex, battleindex, attackNo, par = 0, pow = 0, ReceveEffect;
	char *buffer = NULL;
	char *magicarg = NULL;
	char *magicarg2 = NULL;
	char *magicarg3 = NULL;
	char magicarg4[200];

    char *pszP = NULL; 
	char delim[] = " ";//衪翑煦賤趼揹腔⑹路趼啋

	int status = -1, i, turn = 3;
	char szTurn[] = "turn";

	// 囮騷  堜誑堣堎凝汔竣凝
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	buffer = ITEM_getChar(itemindex, ITEM_ARGUMENT );//淕揹趼揹
	pszP = strtok(buffer, delim);//菴珨跺統杅
    magicarg = strtok(NULL, delim);//菴媼跺統杅
	magicarg2 = strtok(NULL, delim);//菴��跺統杅
	magicarg3 = strtok(NULL, delim);//菴侐跺統杅
    sprintf(magicarg4,"%s %s %s",magicarg,magicarg2,magicarg3);
	magicarg = (char*)magicarg4;

	//嫖噩忐..祥腕妏蚚菩源
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
		//&& BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P 
		){
		if( BATTLE_CheckSameSide( charaindex, toindex) == 0 ){//祥肮晚
			battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
			BATTLE_NoAction( battleindex, BATTLE_Index2No( battleindex, charaindex) );
			CHAR_talkToCli( charaindex, -1, "嫖噩忐..祥腕囥軑準俙模菩源﹝", CHAR_COLORYELLOW);
			return;
		}
	}

	//婓ァ橈袨怓隙葩騵薯
	if( strstr( pszP, "%" ) ){
		par = 1;
	}
	
	if( sscanf( pszP, "%d", &pow ) != 1 ){
		pow = 0;
	}
      
	attackNo = BATTLE_Index2No( battleindex, charaindex );
   
	//恁寁測桶腔杻虴
	if( pow <= 0 ){
		ReceveEffect = SPR_fukkatu3;
	}else
	if( pow <= 100 ){
		ReceveEffect = SPR_fukkatu1;
	}else
	if( pow <= 300 ){
		ReceveEffect = SPR_fukkatu2;
	}else{
		ReceveEffect = SPR_fukkatu3;
	}

	BATTLE_MultiRessurect( battleindex, attackNo, toindex,
		pow, par, SPR_item3, ReceveEffect );
    

	//(嫖,噩,忐)儕鍾

	// 嗚堅禱噁  
	for( ;status == -1 && magicarg[0] != 0; magicarg++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			// 嗚堅鼀蚗淏惉凝ㄑ
			if( strncmp( magicarg, aszMagicDef[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 嗚堅埵笢摯ぁ謄  
	if( status == -1 ) return;

	// 笰樊  脾踏鞠堎凝ㄑ
	if( ( magicarg = strstr( magicarg, szTurn ) ) != NULL){
		magicarg += sizeof( szTurn );
		sscanf( magicarg, "%d", &turn );
	}

	//print("棒杅:%d",turn);

	// 泬哱鳴  蠕
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 蠈劂
	BATTLE_MultiMagicDef( battleindex, attackNo, toindex,
		status, turn, MAGIC_EFFECT_USER, SPR_difence );


	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}

#endif 





