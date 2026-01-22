#include "version.h"
#include<string.h>
#include<stdlib.h>
#include"char.h"
#include"char_base.h"
#include "battle.h"
#include "pet_skill.h"
#include "battle_event.h"
#include "battle_command.h"
#include "lssproto_serv.h"
#include "magic.h"
#include "magic_base.h"
#include "handletime.h"

#ifdef _PROFESSION_SKILL			// WON ADD �冼攃曼絳敵�
#include "profession_skill.h"
#endif

int NowBattlerFd;
#if 1
int BATTLE_MpDown( int charaindex, int down )
{
	return 0;
}
#else
{
	int mp, battleindex;
	// 橋  毀葡硞橋凝��???橋凝
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return TRUE;
	// 葡硞橋啋崨埵凝埱倜��??
	if( BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P ){
		return TRUE;
	}

	// ??禱蚍�桷齉職恐�
	mp = CHAR_getInt( charaindex, CHAR_MP );
	if( mp < down ){
		// 蚍�桷�埵笢摯ぁ笰忒媃埵笢敁埰堎
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
		// 昡悃璃迶OK摯炩擭
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		return FALSE;
	}
	mp -= down;
	CHAR_setInt( charaindex, CHAR_MP, mp );
	return TRUE;
}
#endif


void BattleCommandDispach( int fd,	char *command )
{
	int endFlg = 0, battleindex;
	int EscapeFree = BATTLE_FLG_FREEDP;
	int charaindex = CONNECT_getCharaindex(fd);
	char szBuffer[256]="";
#ifdef _FIXWOLF	 // Syu ADD 党淏曖�丳韎檨ug
	int i , j , pindex;
	BATTLE_ENTRY	*pEntry;
#endif

	NowBattlerFd = fd;

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
		return;
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ||
		CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){

		return;
	}


	if( BattleArray[battleindex].flg & BATTLE_FLG_CHARALOST ){
		EscapeFree = 0;
	}

	if( strncmp( command, "U", 1 ) == 0 ){
		// shan 2001/12/25 begin
		EscapeFree = 0; 
		if (CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEWATCH )!=TRUE){
			CHAR_talkToCli( charaindex, -1,
                "貌砱湮藹卼妏堤隅旯楊ㄛ隅隅隅隅隅隅參斕隅婓桵須爵﹝", CHAR_COLORYELLOW );
            CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD );
            CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
            //sprintf( szBuffer, "Command(%s)(滅郘)", CHAR_getUseName( charaindex ) );
            endFlg = 1;
			BATTLE_MpDown( charaindex, 2 ); 
		}else{
			BATTLE_WatchStop( charaindex );
		}
		// shan end
	}else
	if( strncmp( command, "E", 1 ) == 0 ){

		if( checkErrorStatus( charaindex) ) {
			BattleCommandDispach( fd, "N");
			return;
		}

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ESCAPE );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		//sprintf( szBuffer, "Command(%s)(枅變)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

		BATTLE_MpDown( charaindex, 20 );

	}else
	if( strncmp( command, "H|", 2 ) == 0 ){
		int iNum = -1, At_SoubiIndex;
		if( sscanf( command+2, "%X", &iNum ) != 1
		|| ( iNum < 0 || iNum >= 20 )
		){
			iNum = -1;
		}

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, iNum );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );

		At_SoubiIndex =  CHAR_getItemIndex( charaindex, CHAR_ARM );
		if( ITEM_CHECKINDEX( At_SoubiIndex ) == FALSE ){

		}else{

			if( ITEM_getInt( At_SoubiIndex, ITEM_TYPE ) == ITEM_BOOMERANG ){
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_BOOMERANG );
#ifdef _PETSKILL_BECOMEFOX
				if( CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER)==101749
					|| CHAR_getWorkInt( charaindex, CHAR_WORKFOXROUND ) != -1 ){ //�襣Д銙奾◇�燥祥褫眕妏蚚隙薯梓
				    CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
				}
#endif
#ifdef _PETSKILL_BECOMEPIG
				if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){//曹傖拫薯賸
				    CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
				}
#endif
			}
		}


		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, 1 );
		//sprintf( szBuffer, "Command(%s)(桵須)", CHAR_getUseName( charaindex ) );

		endFlg = 1;

		BATTLE_MpDown( charaindex, 5 );

	}else
	if( strncmp( command, "G", 1 ) == 0 ){

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		sprintf( szBuffer, "Command(%s)(滅郘)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

		BATTLE_MpDown( charaindex, 2 );

	}else
	if( strncmp( command, "N", 1 ) == 0 ){

		EscapeFree = 0;

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
//		sprintf( szBuffer, "Command(%s)(server迖奪)", CHAR_getUseName( charaindex ) );
		//sprintf( szBuffer, "Command(%s)(渾儂)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

	}else
	if( strncmp( command, "T|", 2 ) == 0 ){
		int iNum = -1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		if( sscanf( command+2, "%X", &iNum ) != 1
		|| ( iNum < 0 || iNum >= 20 )
		){
			iNum = -1;
		}

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, iNum );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_CAPTURE );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		//sprintf( szBuffer, "Command(%s)(眸鳳)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

		BATTLE_MpDown( charaindex, 20 );

	}else
	if( strncmp( command, "S|", 2 ) == 0 ){
		int iNum = -1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		// shan 2002/01/08 begin
		if( sscanf( command+2, "%d", &iNum ) != 1
        || ( iNum < -1 || iNum >= CHAR_MAXPETHAVE )
        || ( CHAR_getInt(charaindex,CHAR_RIDEPET)==iNum)
		){
			iNum = -1;
		}
		// shan end

#ifdef _STANDBYPET
		if( iNum >= 0) {
			if( ! (CHAR_getWorkInt( charaindex, CHAR_WORKSTANDBYPET) & ( 1 << iNum)) ) {
				print("\n 蜊猾婦!請堤準渾儂唾!!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
				iNum = -1;
			}
		}
#endif

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, iNum );
		if( iNum < 0 ){
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_PETIN );
			//sprintf( szBuffer, "Command(%s)(遢隙唾昜)", CHAR_getUseName( charaindex ) );
		}else{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_PETOUT );
			//sprintf( szBuffer, "Command(%s)(請堤唾昜)", CHAR_getUseName( charaindex ) );
		}

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		endFlg = 1;

		BATTLE_MpDown( charaindex, 10 );

	}else
	if( strncmp( command, "W|", 2 ) == 0 ){
		int iNum = -1, petskillindex = -1, ToNo = -1, petnum, petindex;

		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) < 1 ){
			iNum = -1; ToNo = -1;
		}
		petnum = CHAR_getInt( charaindex, CHAR_DEFAULTPET );
		petindex = CHAR_getCharPet( charaindex, petnum );
		if( CHAR_CHECKINDEX( petindex ) == FALSE ){
		}else{

			if( iNum < 0 || iNum >= CHAR_MAXPETSKILLHAVE 
#ifdef _PETSKILLBUG  //Add By Syu 2002.0805
				|| (CHAR_getFlg( charaindex, CHAR_ISDIE ) == TRUE )||
				(CHAR_getInt( charaindex, CHAR_HP ) <= 0)
#endif
				|| checkErrorStatus( petindex)
				)
			{
				//sprintf( szBuffer, "Command(%s)(赻雄)", CHAR_getUseName( petindex ) );
				CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
#ifdef _PETSKILLBUG //Add By Syu 2002.0805
			}else if ((CHAR_getInt(petindex, CHAR_TRANSMIGRATION)<1)
				&& (  iNum>=CHAR_getInt(petindex, CHAR_SLOT) ) ) 
				{
				//sprintf( szBuffer, "Command(%s)(赻雄)", CHAR_getUseName( petindex ) );
				CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
#endif
			}else{
				petskillindex = PETSKILL_GetArray( petindex, iNum);
#ifdef _FIXWOLF	 // Syu ADD 党淏曖�丳韎檨ug
				if( (CHAR_getPetSkill( petindex , iNum )== 600) && (ToNo == -1))
				{
					for( j = 0; j < 2; j ++ )
					{
						pEntry = BattleArray[battleindex].Side[j].Entry;
						for( i = 0; i < BATTLE_ENTRY_MAX; i ++ )
						{
							pindex = pEntry[i].charaindex;
							if( pindex == petindex)
							{
								ToNo=i + ( j * 10 );
								break;
							}
						}
					}
				}
#endif
				if( petskillindex >= 0
				&& PETSKILL_Use( petindex, iNum, ToNo, NULL ) == TRUE
				){
					//sprintf( szBuffer, "Command(%s)(坒唳)", CHAR_getUseName( petindex ) );

					EscapeFree = 0;

				}else{
					//sprintf( szBuffer, "Command(%s)(赻雄)", CHAR_getUseName( petindex ) );
					CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
				}
			}
		}
		endFlg = 1;

		BATTLE_MpDown( charaindex, 10 );

	}else
	if( strncmp( command, "J|", 1 ) == 0 ){
		int iNum = -1, magicindex = -1, ToNo = -1;
		int valid = -1;
        int itemindex=-1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) != 2 ){
			iNum = -1; ToNo = -1;
		}
			
		if( iNum < CHAR_EQUIPPLACENUM && iNum >= CHAR_HEAD ){
			magicindex = MAGIC_GetArrayNo( charaindex, iNum );
			itemindex = CHAR_getItemIndex( charaindex, iNum );
			valid = MAGIC_isTargetValid( magicindex, ToNo);

			if ((valid <0 ) || ( magicindex < 0 ) || (itemindex < 0 )){
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			}else{
#ifdef _ITEM_ATTSKILLMAGIC
				char *magicarg;
				int magic_array = -1;
				magic_array = MAGIC_getMagicArray( magicindex );
				magicarg = MAGIC_getChar( magic_array, MAGIC_OPTION );
				if( magicarg != NULL && strstr( magicarg, "SKILL") != NULL )	{
					int mp = ITEM_getInt( itemindex, ITEM_MAGICUSEMP );
					if( MAGIC_AttSkill( charaindex, ToNo, magic_array, mp) == FALSE )	{
						CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
						CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
					}
				}else	{
#endif
					CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, ToNo );
					CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_JYUJYUTU );
					CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, magicindex );
					CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, iNum );
					CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
					//sprintf( szBuffer, "Command(%s)(紸扲)", CHAR_getUseName( charaindex ) );
#ifdef _ITEM_ATTSKILLMAGIC
				}
#endif
			}
		}else{
			EscapeFree = 0;
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			//sprintf( szBuffer, "Command(%s)(渾儂)", CHAR_getUseName( charaindex ) );
		}
		endFlg = 1;
	}else
	if( strncmp( command, "I|", 1 ) == 0 ){
		int iNum = -1, ToNo = -1;
		int valid = 0, itemindex = -1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) != 2 ){
			iNum = -1; ToNo = -1;
		}
		itemindex = CHAR_getItemIndex( charaindex, iNum );
		if ( !ITEM_CHECKINDEX( itemindex) ) valid=-1;
		if (valid >= 0) valid = ITEM_isTargetValid( charaindex, itemindex, ToNo );
		if (valid < 0 ) {
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		} else {
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, ToNo );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ITEM );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM3, iNum );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			//sprintf( szBuffer, "Command(%s)(耋撿)", CHAR_getUseName( charaindex ) );
			endFlg = 1;
		}
	}else
	if( strncmp( command, "@", 1 ) == 0 ){
		//sprintf( szBuffer, "Command(%s)(籵蚚)", CHAR_getUseName( charaindex ) );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		endFlg = 1;

		CHAR_setInt( charaindex, CHAR_MP,
			min( 100, CHAR_getInt( charaindex, CHAR_MP ) + 30 ) );

	}else
#ifdef _PROFESSION_SKILL			// WON ADD �冼攃曼絳敵�
	if( strncmp( command, "P", 1 ) == 0 ){
		int iNum=-1, ToNo=-1;
		int skillindex=-1;
		int char_pskill=-1, profession_skill=-1, Pskillid=-1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) < 1 ){
			iNum = -1; ToNo = -1;
		}

		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){
			print("\n\n 準楊猾婦(19001)(%s)(%s)\n\n", CHAR_getChar(charaindex, CHAR_CDKEY), CHAR_getChar(charaindex, CHAR_NAME) );
#ifdef _PROSKILL_ERR_KICK
			saacproto_ACKick_send( acfd, CHAR_getChar(charaindex,CHAR_CDKEY), getFdidFromCharaIndex( charaindex), 1);
#endif
			return;
		}
#ifdef _PROSKILL_OPTIMUM	// Robin fix cancel 森揭謹徹眥珛潰脤, 蜊婓 PROFESSION_SKILL_Use 笢潰脤
		// �冼齔齡曼�
		char_pskill = CHAR_getInt( charaindex, PROFESSION_CLASS );
		// 撮夔腔眥珛
		skillindex = PROFESSION_SKILL_GetArray( charaindex, iNum);
		Pskillid = PROFESSION_SKILL_getskillArray( skillindex);
		profession_skill = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);

		if( (char_pskill > 0) && (char_pskill == profession_skill) ){
#else
		if( 1 ) {
#endif
			if( PROFESSION_SKILL_Use( charaindex, iNum, ToNo, NULL ) == 1 ){
				//sprintf( szBuffer, "Command(%s)(眥珛撮夔)", CHAR_getUseName( charaindex ) );
				endFlg = 1;
			}else{
				print("\n眥珛撮夔囮啖!! ");
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
				//sprintf( szBuffer, "Command(%s)(渾儂)", CHAR_getUseName( charaindex ) );		
#ifdef _PROSKILL_ERR_KICK
			saacproto_ACKick_send( acfd, CHAR_getChar(charaindex,CHAR_CDKEY), getFdidFromCharaIndex( charaindex), 1);
#endif
			}
		}else{
			print("\n蜊猾婦??眥珛撮夔ID祥淏��:%s:%d:%d \n",
					CHAR_getChar( charaindex, CHAR_CDKEY), char_pskill, profession_skill);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			//sprintf( szBuffer, "Command(%s)(渾儂)", CHAR_getUseName( charaindex ) );
#ifdef _PROSKILL_ERR_KICK
			saacproto_ACKick_send( acfd, CHAR_getChar(charaindex,CHAR_CDKEY), getFdidFromCharaIndex( charaindex), 1);
#endif
		}

		EscapeFree = 0;		
	}else
#endif

	{
		//sprintf( szBuffer, "Command(%s)(囮啖)", CHAR_getUseName( charaindex ) );
		endFlg = 2;
	}


	if( endFlg ){
		BATTLE_ActSettingSend( battleindex );
		
		if( endFlg==1 ){
			int fd;
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE)==CHAR_TYPEPLAYER ){
				fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);	
				CONNECT_SetBattleRecvTime( fd, &NowTime);				
			}
		}
	}

	{
		//if( BATTLE_CHECKINDEX( battleindex ) == TRUE ){
		//	BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORWHITE ) ;
		//}		
	}

	if( BattleArray[battleindex].turn == 0 ){
		BattleArray[battleindex].flg |= EscapeFree;
	}else{
		BattleArray[battleindex].flg &= EscapeFree;
	}
}


void BATTLE_ActSettingSend( int battleindex )
{
	int pindex, k = 0, i, j, charaindex;
	int endBit = 0;
	char szBA[256];
	BATTLE_ENTRY	*pEntry;
	BATTLE	*pBattle;

	for( j = 0; j < 2; j ++ ){
		pEntry = BattleArray[battleindex].Side[j].Entry;
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			pindex = pEntry[i].charaindex;
			// 昡悃璃迶    ぁ拻趙笢倜��
			if( CHAR_getWorkInt( pindex, CHAR_WORKBATTLEMODE )
				== BATTLE_CHARMODE_C_OK
			){
				endBit |= 1 << k;	// 扠蚗哱  趙堎
			}else
			// 傍煦埱倜�涬孛�  凝埰
			if( CHAR_getInt( pindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )
			{
				endBit |= 1 << k;	// 扠蚗哱  趙堎
			}
			k ++;
		}
	}
	sprintf( szBA, "BA|%X|%X|", endBit, BattleArray[battleindex].turn );
//	print( "%s\n", szBA );
	//踏�梇殣撟遙鵅劗鵷し罟肴�
	for( j = 0; j < 2; j ++ ){
		pEntry = BattleArray[battleindex].Side[j].Entry;
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			pindex = pEntry[i].charaindex;
			if( CHAR_CHECKINDEX( pindex ) == FALSE )continue;
			// 落橋媃倜趙摯す籤溘勗毀邞�欷聿�
			if( CHAR_getWorkInt( pindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_RESCUE )continue;
			// 鏤畛騷媕↓煦埱倜��
			if( CHAR_getInt( pindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
			){	// 昡悃璃迶邞堎
				BATTLE_CommandSend( pindex, szBA );
			}
		}
	}


	// め橋媃趙笢堎硞勗忒邞堎
	pBattle = BattleArray[battleindex].pNext;
	for( ; pBattle ; pBattle = pBattle->pNext ){
		// 喫摯囮迶畛筒誑諜媃笢凝鏍攝蚗袲
		if( BATTLE_CHECKADDRESS( pBattle ) == FALSE ){
			fprint( "err:夤桵battle address渣昫(%p)\n", pBattle );
			break;
		}
		// 蠈劂勗邞嚭
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			charaindex = pBattle->Side[0].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			// 昡悃璃迶邞堎
			BATTLE_CommandSend( pindex, szBA );
		}
	}

}



BOOL BATTLE_IsHide( int charaindex ){
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_EARTHROUND0 
		){
		return TRUE;
	}
	return FALSE;
}





//*******************************************************
//
//  泬哱鳴摯昡悃璃迶禱袲溘騷囮璃哱勗邞堎
//
BOOL BATTLE_CommandSend( int charaindex, char *pszCommand )
//
//********************************************************
{
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;
	if(	getfdFromCharaIndex( charaindex ) < 0 )return FALSE;
	lssproto_B_send( getfdFromCharaIndex( charaindex ), pszCommand );	
	return TRUE;
}


BOOL BATTLE_MakeCharaString(
	int battleindex,
	char *pszCommand,
	int	size
)
{
	int i, j, iOffset, charaindex, flg;
	BATTLE_ENTRY	*pEntry;
	char szBuffer[4096]="", *pszTop, *pszLast;
	char szEscapeName[128], szEscapeTitle[128];
	char szEscapePetName[128];
	int rideflg = 0, petindex = -1, petlevel, pethp, petmaxhp;
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return FALSE;
	pszTop = pszCommand;
	pszLast = pszCommand+size-1;
#if 1
	sprintf( szBuffer, "BC|%X|", BattleArray[battleindex].field_att  );
#else
	sprintf( szBuffer, "BC|" );
#endif
	STRCPY_TAIL( pszTop, pszLast, szBuffer );

	for( j = 0; j < 2; j ++ ){
		if( j == 1 ){
			iOffset = SIDE_OFFSET;	// 澺喫竣插�蚅枅鬄啗踽�毀鏤溘筒埰堎
		}else{
			iOffset = 0;
		}
		pEntry = BattleArray[battleindex].Side[j].Entry;
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			charaindex = pEntry[i].charaindex;
			if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLG )
				& CHAR_BATTLEFLG_ULTIMATE )continue;
			flg = 0;
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
				== BATTLE_CHARMODE_INIT ) flg |= BC_FLG_NEW;
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
				== BATTLE_CHARMODE_RESCUE ) flg |= BC_FLG_NEW;
			if( CHAR_getFlg( charaindex, CHAR_ISDIE ) == TRUE ){
				flg |= BC_FLG_DEAD;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKPOISON ) > 0 ){
				flg |= BC_FLG_POISON;
			}else
#ifdef _MAGIC_WEAKEN      //   剞��			
			if( CHAR_getWorkInt( charaindex, CHAR_WORKWEAKEN ) > 0 ){
				flg |= BC_FLG_WEAKEN;
			}else 
#endif
#ifdef _MAGIC_DEEPPOISON  //   曄馮
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDEEPPOISON ) > 0 ){
				flg |= BC_FLG_DEEPPOISON;
			}else
#endif
#ifdef _MAGIC_NOCAST     //   麥蘇
			if( CHAR_getWorkInt( charaindex, CHAR_WORKNOCAST ) > 0 ){
				flg |= BC_FLG_NOCAST;
			}else
#endif
#ifdef _MAGIC_BARRIER	  //   藹梤
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBARRIER ) > 0 ){
				flg |= BC_FLG_BARRIER;
			}else 
#endif
			if( CHAR_getWorkInt( charaindex, CHAR_WORKPARALYSIS ) > 0 ){
				flg |= BC_FLG_PARALYSIS;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSLEEP ) > 0 ){
				flg |= BC_FLG_SLEEP;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSTONE ) > 0 ){
				flg |= BC_FLG_STONE;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDRUNK ) > 0 ){
				flg |= BC_FLG_DRUNK;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKCONFUSION ) > 0 ){
				flg |= BC_FLG_CONFUSION;
			}
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
				flg |= BC_FLG_PLAYER;
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_REVERSE ){
				flg |= BC_FLG_REVERSE;
			}
			if( BATTLE_IsHide( charaindex ) == TRUE ){
				flg |= BC_FLG_HIDE;
			}
#ifdef _PET_SKILL_SARS			// WON ADD 馮伢雞晊
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSARS ) > 0 ){
				flg |= BC_FLG_SARS;
			}
#endif
#ifdef _PROFESSION_SKILL			// WON ADD �冼攃曼絳敵�
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDIZZY ) > 0 ){
				flg |= BC_FLG_DIZZY;			// 婠悈	
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKENTWINE ) > 0 ){
				flg |= BC_FLG_ENTWINE;			// 攷跦莊��
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDRAGNET ) > 0 ){
				flg |= BC_FLG_DRAGNET;			// 毞蹕華厙	
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKICECRACK ) > 0 ){
			//	flg |= BC_FLG_ICECRACK;			// 梨惇扲	
			}			
			if( CHAR_getWorkInt( charaindex, CHAR_WORKOBLIVION ) > 0 ){
				flg |= BC_FLG_OBLIVION;			// 疻咭
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKICEARROW ) > 0 ){
				flg |= BC_FLG_ICEARROW;			// 梨璋
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBLOODWORMS ) > 0 ){
				flg |= BC_FLG_BLOODWORMS;		// 岓悛嘍
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSIGN ) > 0 ){
				flg |= BC_FLG_SIGN;				// 珨渀獗悛
			}		
			if( CHAR_getWorkInt( charaindex, CHAR_WORKINSTIGATE ) > 0 ){
				flg |= BC_FLG_CARY;				// 泔畢
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORK_F_ENCLOSE ) > 0 ){
				flg |= BC_FLG_F_ENCLOSE;		// 鳶蜇极
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORK_I_ENCLOSE ) > 0 ){
				flg |= BC_FLG_I_ENCLOSE;		// 梨蜇极
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORK_T_ENCLOSE ) > 0 ){
				flg |= BC_FLG_T_ENCLOSE;		// 濘蜇极
			}
#endif
#ifdef _PROFESSION_ADDSKILL
			if( CHAR_getWorkInt( charaindex, CHAR_WORKANNEX ) > 0 ){
				flg |= BC_FLG_T_ENCLOSE;		// 蜇旯扲
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKWATER ) > 0 ){
				flg |= BC_FLG_WATER;		// 阨蜇极
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKFEAR ) > 0 ){
				flg |= BC_FLG_FEAR;		// 謁曉
			}
#endif
#ifdef _PETSKILL_LER
			if(CHAR_getWorkInt(charaindex,CHAR_WORK_RELIFE) > 0){
				flg |= BC_FLG_CHANGE;		// 濘嫌曹旯
				CHAR_setWorkInt(charaindex,CHAR_WORK_RELIFE,0);
			}
#endif
			makeEscapeString( CHAR_getUseName( charaindex ),
				szEscapeName, 
				sizeof( szEscapeName ) );
			makeEscapeString( BATTLE_CharTitle( charaindex ),
				szEscapeTitle, 
				sizeof( szEscapeTitle ) );
 // Robin 0728 ride Pet
			petindex = BATTLE_getRidePet( charaindex );
			if( petindex != -1 ){
				rideflg = 1;
				makeEscapeString( CHAR_getUseName( petindex ),
					szEscapePetName, 
					sizeof( szEscapePetName ) );
				petlevel = CHAR_getInt( petindex, CHAR_LV);
				pethp = CHAR_getInt( petindex, CHAR_HP);
				petmaxhp = CHAR_getWorkInt( petindex, CHAR_WORKMAXHP);
				
			}else {
				if( CHAR_getWorkInt( charaindex, CHAR_WORKPETFALL ) == 1 ){
					rideflg = -1;
#ifdef _PETSKILL_BECOMEFOX // �趮釓佸躉鯞黧讕鞃簆掃巷剿５姜� Client
					if( CHAR_getWorkInt( charaindex, CHAR_WORKFOXROUND ) != -1 )
						rideflg = -2; //client蚚懂瓚剿珆尨邈鎮腔恅趼 (勤茼腔client曹杅峈p_party[ BattleMyNo ]->onRide)
#endif
#ifdef _PETSKILL_BECOMEPIG // �趮釓冼睋氖窄讕鞃簆掃巷剿５姜� Client
					if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > 120 )//曹傖拫薯賸
						rideflg = -3; //client蚚懂瓚剿珆尨邈鎮腔恅趼 (勤茼腔client曹杅峈p_party[ BattleMyNo ]->onRide)
#endif
					 CHAR_setWorkInt( charaindex, CHAR_WORKPETFALL, 0 );
				}
				else
					rideflg = 0;
				szEscapePetName[0] = 0;
				petlevel = 0;
				pethp = 0;
				petmaxhp = 0;
			}

			sprintf( szBuffer, "%X|%s|%s|%X|%X|%X|%X|%X|%X|%s|%X|%X|%X|",
				pEntry[i].bid,
				szEscapeName,
				szEscapeTitle,
				CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER ),
				CHAR_getInt( charaindex, CHAR_LV ),
				max( CHAR_getInt( charaindex, CHAR_HP ),0),
				CHAR_getWorkInt( charaindex, CHAR_WORKMAXHP ),
				flg,
				rideflg,
				szEscapePetName,
				petlevel,
				pethp,
				petmaxhp
			);
			STRCPY_TAIL( pszTop, pszLast, szBuffer );
			if( pszTop >= pszLast )return FALSE;// 祒窇倜�桷�  
		}
	}

	return TRUE;
}

void BATTLE_BpSendToWatch(
	BATTLE *pBattle, 	// め橋泬哱鳴淏筒袲摯睽騷璃淏
	char *pszBcString 	// BC  棬  
)
{

	char szBp[256];
	int flg = 0, i, charaindex;

//	for( ; pBattle ; pBattle = pBattle->pNext ){
	if( pBattle == NULL )return;
	// 喫摯囮迶畛筒誑諜媃笢凝鏍攝蚗袲
	if( BATTLE_CHECKADDRESS( pBattle ) == FALSE ){
		fprint( "err:夤桵battle address渣昫(%p)\n", pBattle );
		return;
	}

	// 蠈劂勗邞嚭
	for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
		charaindex = pBattle->Side[0].Entry[i].charaindex;
		if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) 
			!= CHAR_TYPEPLAYER ) continue;
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
			== BATTLE_CHARMODE_WATCHINIT){
			flg |= BP_FLG_JOIN;	// 犍踐落萃
		}
		flg |= BP_FLG_PLAYER_MENU_OFF;	// 鏤畛騷媕↓隍俓堥↓③踏埵笢
		// 鏤畛騷媕↓  蠕邞堎
		sprintf( szBp, "BP|%X|%X|%X", 20, flg, 0 );
		BATTLE_CommandSend( charaindex, szBp );
		// 蠈劂呬玊攷  邞堎
		BATTLE_CommandSend( charaindex, pszBcString );
	}
	// 昡悃璃迶輝з喜
	pBattle->mode = BATTLE_MODE_WATCHPRE;
//	}
}

void BATTLE_CharSendAll( int battleindex )
{
	int i, j, charaindex, flg, pet, At_SoubiIndex, pindex;
	char szBp[256];
	BATTLE	*pBattle;

	BATTLE_MakeCharaString(
		battleindex,
		szAllBattleString,
		sizeof(szAllBattleString)
	);

	for( j = 0; j < 2; j ++ ){
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			charaindex = BattleArray[battleindex].Side[j].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			flg = 0;
			if( BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE ){
				flg |= BP_FLG_ENEMY_SURPRISAL;
			}else
				if( BattleArray[battleindex].Side[1-j].flg & BSIDE_FLG_SURPRISE ){
					flg |= BP_FLG_PLAYER_SURPRISAL;
				}
				if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
					== BATTLE_CHARMODE_RESCUE){
					flg |= BP_FLG_JOIN;		// 落橋
				}
			At_SoubiIndex = CHAR_getItemIndex( charaindex, CHAR_ARM );
			if( ITEM_CHECKINDEX( At_SoubiIndex ) == FALSE ){
			}else{
				if( ITEM_getInt( At_SoubiIndex, ITEM_TYPE ) == ITEM_BOOMERANG ){
					flg |= BP_FLG_BOOMERANG;
				}
			}

			pet = CHAR_getInt( charaindex, CHAR_DEFAULTPET );
			if( pet >= 0 ){
				snprintf( szBp, sizeof( szBp ), "K%d", pet );
				CHAR_sendStatusString( charaindex, szBp );
				
				pindex = CHAR_getCharPet( charaindex, pet );

#ifdef _PETSKILL_DAMAGETOHP
	{
/*
		�藩�,�藩泠~~~~~~
                     
               成成  
             ×  ×  扂懂跤斕冞堎欲賸
*/
		char msg[32]={0};
		//print("\n唾昜id:%d",CHAR_getInt( pindex, CHAR_PETID)); 
		//print("\n唾昜靡:%s",CHAR_getChar( pindex, CHAR_NAME)); 
		//if( CHAR_getInt( pindex, CHAR_PETID) == 777 ){//唾昜ID 阨邧芛曖
		//	  || CHAR_getInt( pindex, CHAR_PETID) == 146 ) //鳶邧芛曖
		//	&& CHAR_getInt( pindex, CHAR_HP) ){
		    sprintf( msg, "o%d", pet );
		    CHAR_sendStatusString( charaindex, msg );
			
		//}
	} 
#endif

#ifdef _PETSKILL_BECOMEFOX // 癹秶笢賸藥鼻扲腔唾昜腔撮夔
		    //if( CHAR_getInt( pindex, CHAR_BASEIMAGENUMBER) == 101749 ){
	        if( CHAR_getWorkInt( pindex, CHAR_WORKFOXROUND ) != -1 ){ //�襣Д銙奾◇�燥
			    char msg[32]={0};
				sprintf( msg, "a%d", pet );
		        CHAR_sendStatusString( charaindex, msg );
				
			}
#endif

				if( CHAR_CHECKINDEX( pindex ) == TRUE ){
					if( BATTLE_IsCharge( pindex  ) == TRUE){
						flg |= BP_FLG_PET_MENU_OFF;
					}
					if( BATTLE_CanMoveCheck( pindex ) == FALSE ){
						flg |= BP_FLG_PET_MENU_OFF;
					}
					if( CHAR_getFlg( pindex, CHAR_ISDIE ) == TRUE ){
						flg |= BP_FLG_PET_MENU_OFF;
					}
				}
			}else{
				flg |= BP_FLG_PET_MENU_OFF;
			}

			if( BATTLE_CanMoveCheck( charaindex ) == FALSE
				|| CHAR_getFlg( charaindex, CHAR_ISDIE ) == TRUE
			){
				flg |= BP_FLG_PLAYER_MENU_OFF;
			}

			if( BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE ){
				flg |= BP_FLG_PLAYER_MENU_OFF;
				flg |= BP_FLG_PET_MENU_OFF;
			}
			sprintf( szBp, "BP|%X|%X|%X",
				i + SIDE_OFFSET * j,
				flg, CHAR_getInt( charaindex, CHAR_MP )
			);
			BATTLE_CommandSend( charaindex, szBp );
			BATTLE_CommandSend( charaindex, szAllBattleString );
		}
	}
	pBattle = BattleArray[battleindex].pNext;
	for( ; pBattle ; pBattle = pBattle->pNext ){
		// 喫摯囮迶畛筒誑諜媃笢凝鏍攝蚗袲
		if( BATTLE_CHECKADDRESS( pBattle ) == FALSE ){
			fprint( "err:夤桵battle address渣昫(%p)\n", pBattle );
			break;
		}

		BATTLE_BpSendToWatch( pBattle, szAllBattleString );

	}



}

void BattleEncountOut( int charaindex)
{

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEINDEX, -1 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_MODE_NONE );

    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX ) );
    CHAR_sendArroundCharaData( charaindex );

	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEWATCH ) == 0 ){
		CHAR_sendBattleEffect( charaindex, OFF);
	}else{
		CHAR_sendBattleWatch( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX ), OFF);
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEWATCH, 0 );
	}
   
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_CLIENT ){
		int pmode/*, battleindex*/;
		int pindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1 );
		pmode = CHAR_getWorkInt( pindex, CHAR_WORKBATTLEMODE );
		if( pmode > 0 && pmode != BATTLE_CHARMODE_FINAL ){
				BATTLE_RescueParentTry( charaindex, pindex );
				print( "誕鞣統樓桵須(%s)\n", CHAR_getUseName( charaindex ) );
		}
	}

}


//**************************************************
//
// 溢啞б鳴哱摯昡悃璃迶禱  躂堎妐蚗哱暵
//
BOOL	BATTLE_PetDefaultCommand( int petindex )
//
//**************************************************
{
	if( CHAR_CHECKINDEX( petindex ) == FALSE )return FALSE;
	// Ъ鱗摯炩擭
	CHAR_setWorkInt( petindex, CHAR_WORKBATTLECOM2, -1 );
	// 玊橖  賓摯炩擭
	CHAR_setWorkInt( petindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
	// 昡悃璃迶OK摯炩擭
	CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;
}

#if 0
{
	int Sign, Damage, i;
	Sign = (LifeUp >= 0)?(1):(0);
	Damage = ABS( LifeUp );

	for( i = 0; i < ; i ++ ){
	// (犍菲)喫喫ぁ昡悃璃迶禱笢躂堎
	snprintf( szCommand, sizeof(szCommand),	"BD|r%X|0|%X|d%X|", ToList[i], Sign, Damage );
	BATTLESTR_ADD( szCommand );
}

#endif

// Robin fix 潰脤坒趙餉阯祥褫桵須
int checkErrorStatus( int charaindex)
{
	//if( BATTLE_CanMoveCheck( charaindex) == FALSE) {

	if( 
		CHAR_getWorkInt( charaindex, CHAR_WORKPARALYSIS ) > 0	// 鎊敘
		|| CHAR_getWorkInt( charaindex, CHAR_WORKSTONE ) > 0	// 坒趙
		|| CHAR_getWorkInt( charaindex, CHAR_WORKSLEEP ) > 0	// 阯蹺
		//|| CHAR_getWorkInt( charaindex, CHAR_WORKBARRIER ) > 0	// 藹梤
		|| CHAR_getWorkInt( charaindex, CHAR_WORKDIZZY ) > 0	// 婠悈
		|| CHAR_getWorkInt( charaindex, CHAR_WORKDRAGNET ) > 0	// 毞蹕華厙
		//|| CHAR_getWorkInt( charaindex, CHAR_WORK_T_ENCLOSE ) > 0 // 濘蜇极
#ifdef _PROFESSION_ADDSKILL
//		|| CHAR_getWorkInt( charaindex, CHAR_DOOMTIME ) > 0 //岍賜藺�梩紜�
#endif
	)
	{

		char cdkey[128];
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
			strcpy( cdkey, CHAR_getChar( charaindex, CHAR_CDKEY));
		else
			strcpy( cdkey, CHAR_getChar( charaindex, CHAR_OWNERCDKEY) );
	
		print("\n 蜊猾婦!祥褫桵須腔袨怓!!:%s ", cdkey );

		return 1;
	}
	return 0;
}

