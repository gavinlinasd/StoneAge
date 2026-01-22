#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   // shan
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "common.h"
#include "char_base.h"
#include "char_data.h"
#include "char.h"
#include "lssproto_serv.h"
#include "item.h"
#include "item_event.h"
#include "buf.h"
#include "object.h"
#include "map_deal.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "handletime.h"
#include "char_event.h"
#include "npccreate.h"
#include "addressbook.h"
#include "item_event.h"
#include "magic_base.h"
#include "magic.h"
#include "chatmagic.h"
#include "configfile.h"
#include "log.h"
#include "anim_tbl.h"
#include "encount.h"
#include "battle.h"
#include "pet_skill.h"
#include "util.h"
#include "enemy.h"
#include "npcutil.h"
#include "pet.h"
#include "family.h"
#include "defend.h"
#ifdef _NPCSERVER_NEW
#include "npcserver.h"
#endif
#ifdef _DEATH_CONTEND
#include "deathcontend.h"
#endif
#include "correct_bug.h"
#ifdef _JOBDAILY
#include "npc_checkman.h"
#endif

#ifdef _CHAR_PROFESSION			// WON ADD �冼攃曼耋裕�
#include "profession_skill.h"
#endif
#ifdef _CHATROOMPROTOCOL			// (祥褫羲) Syu ADD 謐毞弅け耋
#include "chatroom.h"
#endif

extern int channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];	
extern int familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

extern  tagRidePetTable ridePetTable[296];
extern	int BATTLE_getRidePet( int charaindex );

#ifdef _CHANNEL_MODIFY
extern int InitOccChannel(void);
#endif

#ifdef _ANGEL_SUMMON
extern int checkIfAngelByName( char* nameinfo);
extern char* getMissionNameInfo( int charaindex, char* nameinfo);
extern void CHAR_sendAngelMark( int objindex, int flag);
extern int checkIfOnlyAngel( int charaindex);
extern void selectAngel( int charaindex, int heroindex, int mission, int gm_cmd);
#endif

extern void GOLD_DeleteTimeCheckOne( int objindex);

// WON ADD 党淏耋撿腔扢隅恀枙
void fix_item_bug(int charaindex, int i);


/*====================す籤溘摯軘埬勗匴埰堎匴倳====================*/
/*------------------------------------------------------------
 * 軘埬埰堎鄹摯酘鏤で砫璃摯偯禱优擭媃源竣敁埰堎��
 * 礎倳
 *  ch      Char*       优擭袸摯す籤溘摯溢↓淏
 *  option  char*       袲溘騷囮璃哱凝�桱彸繚腹灊�
 * 蒍堇偯
 *  埬      TRUE
 *  謄      FALSE
 ------------------------------------------------------------*/
static BOOL CHAR_makeCharFromOptionAtCreate( Char* ch ,
		int	vital, int str, int tgh, int dex,
		int earth, int water, int fire, int wind)
{

	/*    嗚鍛摯號擂禱菜竣  */
	int		para[4];
	int		attr[4];
	int		i;
	int		parasum,attrsum;
	int		cnt;

	para[0] = vital;
	para[1] = str;
	para[2] = tgh;
	para[3] = dex;

	attr[0] = earth;
	attr[1] = water;
	attr[2] = fire;
	attr[3] = wind;

#define MAXPARAMETER 20
#define PKMAXPARAMETER 300
	parasum = 0;
	/*
	 * 埰迋趙摯溢↓淏摯  勗 - 毀堣埱趙毀埵�欷聿倠�
	 * 閬堣謂躂摯痀禱萳窇趙堁笢趙    摯偯忒祒窇埵笢
	 */

	for( i = 0; i < arraysizeof( para); i ++ ) {
		if( para[i] < 0 || para[i] > MAXPARAMETER )return FALSE;
		parasum += para[i];
	}
#ifdef _NEW_PLAYER_CF	//陔忒堤汜饜离
	if( parasum > MAXPARAMETER ) return FALSE;
#else
	if( parasum != MAXPARAMETER ) return FALSE;
#endif

/*#ifdef _NEW_TESTSERVER	//聆彸侜督
	if( parasum > MAXPARAMETER ) return FALSE;
#else
	if( parasum != MAXPARAMETER ) return FALSE;
#endif*/


#undef MAXPARAMETER


#define MAXATTRIBUTE 10
	cnt = 0;
	attrsum = 0;
	/* 鶂鍛摯鏍攝蚗袲禱埰堎 */
	for( i = 0; i < arraysizeof( para); i ++ ) {
		if( attr[i] < 0 || attr[i] > MAXATTRIBUTE )return FALSE;
		attrsum += attr[i];
		if( attr[i] > 0 ) cnt ++;
	}
	/* 譴銓偯毀10 */
	if( attrsum != MAXATTRIBUTE ) return FALSE;

	/* 2僑源堇蘗�忺帟鳳祥紙桫噱胱俴觕淜�   */
	if( cnt > 2 ) return FALSE;

	/* 楪鶂鍛毀鄎堇騍趙�桫撻聿� */
	if( attr[0] > 0 && attr[2] > 0 ) return FALSE;
	if( attr[1] > 0 && attr[3] > 0 ) return FALSE;

#undef MAXATTRIBUTE

	/*  昡鼀↓  */
	ch->data[CHAR_VITAL] = vital*100;
	ch->data[CHAR_STR]   = str*100;
	ch->data[CHAR_TOUGH] = tgh*100;
	ch->data[CHAR_DEX]   = dex*100;

	ch->data[CHAR_EARTHAT] = attr[0] * 10;
	ch->data[CHAR_WATERAT] = attr[1] * 10;
	ch->data[CHAR_FIREAT]  = attr[2] * 10;
	ch->data[CHAR_WINDAT]  = attr[3] * 10;
#ifdef _NEWOPEN_MAXEXP
	ch->data[CHAR_OLDEXP] = 0;
#endif
	ch->data[CHAR_EXP] = 0;
	
	
#ifdef _NEW_PLAYER_CF	//陔忒堤汜饜离
	ch->data[CHAR_TRANSMIGRATION] = getNewplayertrans();
	ch->data[CHAR_GOLD] = getNewplayergivegold();;
	ch->data[CHAR_LV] = getNewplayerlv();//120;
//	ch->data[CHAR_LEARNRIDE] = 200;
//	ch->data[CHAR_LASTTALKELDER] = (rand()%10)>5?35:36;//34;
//	ch->data[CHAR_SKILLUPPOINT] = 616;//600;
#ifdef _75_TEST
	ch->data[CHAR_TRANSEQUATION] = (100 << 16) + 650;
#endif
#ifdef _NEW_RIDEPETS
	//ch->data[CHAR_LOWRIDEPETS] = 0xffffffff;
#endif
#endif

/*#ifdef _NEW_TESTSERVER	//聆彸侜督
	ch->data[CHAR_SKILLUPPOINT] = "";//600;
	ch->data[CHAR_TRANSMIGRATION] = 5;
	ch->data[CHAR_GOLD] = CHAR_MAXGOLDHAVE;
	ch->data[CHAR_LV] = 140;//120;
	ch->data[CHAR_LEARNRIDE] = 200;
//	ch->data[CHAR_LASTTALKELDER] = (rand()%10)>5?35:36;//34;
	ch->data[CHAR_SKILLUPPOINT] = 616;//600;
#ifdef _75_TEST
	ch->data[CHAR_TRANSEQUATION] = (100 << 16) + 650;
#endif
#ifdef _NEW_RIDEPETS
	//ch->data[CHAR_LOWRIDEPETS] = 0xffffffff;
#endif
#endif*/

#ifdef _PETSKILL_BECOMEPIG
    ch->data[CHAR_BECOMEPIG] = -1;
	ch->data[CHAR_BECOMEPIG_BBI] = 100250;
#endif

	ch->data[CHAR_LASTLEAVETIME] = 0;

	return TRUE;
}

#ifdef _HELP_NEWHAND
void CHAR_loginAddItemForNew( int charindex )
{
    int emptyitemindexinchara, itemindex;
	int	i;

	char	msgbuf[128];
	
	for( i = 0; i < 15; i ++ ) {
	    emptyitemindexinchara = CHAR_findEmptyItemBox( charindex );

		if( emptyitemindexinchara < 0 ){
			sprintf( msgbuf, "諾潔戲弇祥逋﹝" );

			CHAR_talkToCli( charindex, -1, msgbuf, CHAR_COLORYELLOW);
			return;
		}
		if(getNewplayergiveitem(i)==-1)continue;
		itemindex = ITEM_makeItemAndRegist( getNewplayergiveitem(i) );

	    if( itemindex != -1 ){
	        CHAR_setItemIndex( charindex, emptyitemindexinchara, itemindex );
	        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
	        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charindex);
	       	CHAR_sendItemDataOne( charindex, emptyitemindexinchara);
		}
	}
	print("Get New Hand Item");
}
#endif

void CHAR_createNewChar( int clifd, int dataplacenum, char* charname ,
						  int imgno,int faceimgno,
						  int vital,int str,int tgh,int dex,
						  int earth,int water,int fire,int wind,
						  int hometown , char *cdkey )
{

	Char    ch, *chwk;
	int		charaindex, petindex;
	int		enemyarray;
	char szKey[256];

/*#ifdef _NEW_TESTSERVER	//聆彸侜督
	int add_pet[]={0,0,0,0,0};
#endif*/

#ifdef _NEW_PLAYER_CF	//陔忒堤汜饜离
	int add_pet[]={0,0,0,0,0};
#endif

#ifdef _DELBORNPLACE //Syu ADD 6.0 苀珨堤汜黺陔忒游
	int BornPet = hometown;
	if( getMuseum() )
		hometown = 1;
#endif
	memset(&ch,0,sizeof(Char));

	if( !CHAR_checkPlayerImageNumber( imgno)) {
		lssproto_CreateNewChar_send( clifd ,FAILED, "" );
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	if( !CHAR_checkFaceImageNumber( imgno, faceimgno)) {
		lssproto_CreateNewChar_send( clifd ,FAILED, "" );
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	CHAR_getDefaultChar(&ch,imgno);
	ch.data[CHAR_WALKINTERVAL] = getWalksendinterval();

	if( CHAR_getInitElderPosition( &ch ,hometown) == FALSE ){
		lssproto_CreateNewChar_send( clifd ,FAILED, "" );
        CONNECT_setState( clifd, NOTLOGIN );
		return;

	}else if( MAP_checkCoordinates( ch.data[CHAR_FLOOR], ch.data[CHAR_X],
									ch.data[CHAR_Y] ) == FALSE ){
		lssproto_CreateNewChar_send( clifd ,FAILED, "");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	strcpysafe( ch.string[CHAR_NAME].string ,
				sizeof(ch.string[CHAR_NAME].string),
				charname);

	if( CHAR_getNewImagenumberFromEquip( imgno,0) == -1 ){
		lssproto_CreateNewChar_send( clifd ,FAILED, "");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	ch.data[CHAR_BASEIMAGENUMBER] = imgno;
	ch.data[CHAR_BASEBASEIMAGENUMBER] = imgno;
	ch.data[CHAR_FACEIMAGENUMBER] = faceimgno;
	ch.data[CHAR_DATAPLACENUMBER] = dataplacenum;
	
#ifdef _FM_JOINLIMIT
	ch.data[CHAR_FMTIMELIMIT] = 0;
#endif

#ifdef _NEWSAVE
	ch.data[CHAR_SAVEINDEXNUMBER] = -1;
#endif
	ch.data[CHAR_HP] = 0x7fffffff;

	ch.data[CHAR_CHARM] = 60;
	ch.data[CHAR_MAXMP] = ch.data[CHAR_MP] = 100;
	strcpysafe( ch.string[CHAR_CDKEY].string,
                sizeof( ch.string[CHAR_CDKEY].string), cdkey );
                
        // Robin 0724
        ch.data[CHAR_RIDEPET] = -1;
        ch.data[CHAR_LEARNRIDE] = 0;

#ifdef _NEW_RIDEPETS
		ch.data[CHAR_LOWRIDEPETS] = 0;
#endif

#ifdef _PERSONAL_FAME	// Arminius: 模逜跺�侂驫�
	ch.data[CHAR_FAME] = 0;
#endif
#ifdef _NEW_MANOR_LAW
	ch.data[CHAR_MOMENTUM] = 0;	// 跺�侔鑫�
#endif
#ifdef _TEACHER_SYSTEM
	ch.data[CHAR_TEACHER_FAME] = 0;	// 絳呇鍰絳汒咡
#endif
#ifdef _RACEMAN
	ch.data[CHAR_CHECKIN] = 0;  // 唾昜腎暮
	ch.data[CHAR_CATCHCNT1] = 0; // 陔忒癹 轂唾棒杅
	ch.data[CHAR_CATCHCNT2] = 0; // 橾忒癹 轂唾棒杅
	ch.data[CHAR_CATCHCNT3] = 0; // 模逜癹 轂唾棒杅
	ch.data[CHAR_CATCHCNT4] = 0; // 橾忒祥癹 轂唾棒杅
	ch.data[CHAR_CATCHCNT5] = 0; // 模逜祥癹 轂唾棒杅
	ch.data[CHAR_KINDCNT1] = 0;
	ch.data[CHAR_KINDCNT2] = 0;
	ch.data[CHAR_KINDCNT3] = 0;
	ch.data[CHAR_KINDCNT4] = 0;
	ch.data[CHAR_KINDCNT5] = 0;
	ch.data[CHAR_KINDCNT6] = 0;
	ch.data[CHAR_KINDCNT7] = 0;
	ch.data[CHAR_KINDCNT8] = 0;
	ch.data[CHAR_KINDCNT9] = 0;
	ch.data[CHAR_KINDCNT10] = 0;
#endif
#ifdef _CHAR_PROFESSION			// WON ADD �冼攃曼�
	ch.data[PROFESSION_CLASS] = 0;
	ch.data[PROFESSION_LEVEL] = 0;
//	ch.data[PROFESSION_EXP] = 0;
	ch.data[PROFESSION_SKILL_POINT] = 0;
	ch.data[ATTACHPILE] = 0;
#endif

#ifdef _GM_IDENTIFY
  sprintf(ch.string[CHAR_GMIDENTIFY].string,"%s",""); //gm靡備ь峈諾趼揹
#endif
#ifdef _TEACHER_SYSTEM
	memset(ch.string[CHAR_TEACHER_ID].string,0,sizeof(STRING128));
	memset(ch.string[CHAR_TEACHER_NAME].string,0,sizeof(STRING128));
#endif

#ifdef _ITEM_SETLOVER
	memset(ch.string[CHAR_LOVERID].string,0,sizeof(STRING128));
	memset(ch.string[CHAR_LOVERNAME].string,0,sizeof(STRING128));
#endif

#ifdef _PETSKILL_BECOMEPIG
    ch.data[CHAR_BECOMEPIG] = -1;
	ch.data[CHAR_BECOMEPIG_BBI] = 100250;
#endif

	strcpysafe( ch.string[CHAR_FMNAME].string,
                sizeof( ch.string[CHAR_FMNAME].string), "\0" );
	ch.data[CHAR_FMINDEX] = -1;

	if( CHAR_makeCharFromOptionAtCreate( &ch,
										vital,str,tgh,dex,
										earth,water,fire,wind ) == FALSE )
	{
		lssproto_CreateNewChar_send( clifd ,FAILED,
									 "option data is invalid\n");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	{
		int mp=ch.data[CHAR_MAXMP];
		int str=ch.data[CHAR_STR];
		int tough=ch.data[CHAR_TOUGH];

		ch.data[CHAR_INITDATA] = 2*100*100*100*100 /*+ hp*100*100*100*/
			+ mp*100*100 + str* 100 + tough;
	}

	charaindex =  CHAR_initCharOneArray( &ch );
	if( charaindex == -1 ) {
		lssproto_CreateNewChar_send( clifd ,FAILED, "");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	
/*#ifdef _NEW_TESTSERVER	//畛菟聆彸侜督
	{
		int petTemp[]={ 2258, 1610, 353, 2474, -1}; //紾�腹�
		int petNum=0;
		int k=0;
		int i=0,j=0;

#ifdef _75_TEST
		if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 1 )//鎖璨饑佪
			petTemp[4] = 2;//翮掀
		else if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 2 )//樓樓
			petTemp[4]  = 3;//親親嫌
		else if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 3 )//縐糧坳饒
			petTemp[4]  = 4;//哏皎
		else 
			petTemp[4] = 1;//拫薯
#else
		j=2;
		for( i=0; i< arraysizeof(ridePetTable) ; i++ ){
			if( CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].charNo ){
				petTemp[j]= ridePetTable[i].petId;
				j++;
				if( j >= arraysizeof( petTemp) )
					break;
			}
		}
#endif

		j=0;
		for( petNum=0; petNum<arraysizeof( petTemp); petNum++)	{
			enemyarray = ENEMY_getEnemyArrayFromId( petTemp[ petNum]); //啞誥
			petindex = ENEMY_createPetFromEnemyIndex( charaindex, enemyarray);
			if( petindex == -1 ) {
				continue;
			}
			add_pet[j]=petindex;
			j++;
			for( k = 1; k < 140; k ++ ){	//汔撰
				CHAR_PetLevelUp( petindex );
				CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
				CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
			}
			CHAR_complianceParameter( petindex );
			CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );
#ifdef _75_TEST
			CHAR_setInt( petindex , CHAR_TRANSMIGRATION , 1 );
#endif
		}
//		CHAR_setInt( charaindex, CHAR_FLOOR, 130);
//		CHAR_setInt( charaindex, CHAR_X, 20);
//		CHAR_setInt( charaindex, CHAR_Y, 55);
	}
#else
#ifdef _DELBORNPLACE //Syu ADD 6.0 苀珨堤汜黺陔忒游
	if( getMuseum() ) {
		if ( BornPet > 3 ) BornPet = 3;
		if ( BornPet < 0 ) BornPet = 0;
		enemyarray = ENEMY_getEnemyArrayFromId( BornPet + 2076);
	}
	else {
		enemyarray = ENEMY_getEnemyArrayFromId( hometown + 1);
	}
#else
	enemyarray = ENEMY_getEnemyArrayFromId( hometown + 1);
#endif
	petindex = ENEMY_createPetFromEnemyIndex( charaindex, enemyarray);
	if( !CHAR_CHECKINDEX( petindex )){
		CHAR_endCharOneArray( charaindex);
		lssproto_CreateNewChar_send( clifd ,FAILED, "");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	CHAR_setMaxExpFromLevel( petindex, 1);

#endif*/

#ifdef _NEW_PLAYER_CF	//陔忒堤汜饜离
		int petTemp[]={ 2258, 1610, 353, 2474, -1}; //紾�腹�
		int petNum=0;
		int k=0;
		int i=0,j=0;
		int mylevel,level;
		if(getNewplayergivepet(0)==-1){
			if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 1 )//鎖璨饑佪
				setNewplayergivepet(0,2);
			else if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 2 )//樓樓
				setNewplayergivepet(0,3);
			else if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 3 )//縐糧坳饒
				setNewplayergivepet(0,4);
			else 
				setNewplayergivepet(0,1);
		}
		
		for( petNum=0; petNum<5; petNum++)	{
			if(getNewplayergivepet(petNum)==-1)continue;
			enemyarray = ENEMY_getEnemyArrayFromId( getNewplayergivepet(petNum)); //啞誥
			petindex = ENEMY_createPetFromEnemyIndex( charaindex, enemyarray);
			if( petindex == -1 ) {
				continue;
			}
			add_pet[j]=petindex;
			mylevel = CHAR_getInt( petindex, CHAR_LV);
			level = getNewplayerpetlv()-mylevel;
			if(level<1)level=1;
			for( k = 1; k < level; k ++ ){	//汔撰
				CHAR_PetLevelUp( petindex );
				CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
				CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
			}
			CHAR_complianceParameter( petindex );
			CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );
		}
#endif

#ifdef _HELP_NEWHAND
	CHAR_loginAddItemForNew(charaindex); 
#endif

#if 1
	if( CHAR_makeDBKey(  charaindex, szKey, sizeof( szKey ) ) != FALSE ){
		int dp;
		char info[512];
		snprintf( info, sizeof( info),
					"%d,%d,%d,%d,%d,%d",
					CHAR_getInt( charaindex, CHAR_LV),
					CHAR_getInt( charaindex, CHAR_DUELBATTLECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELLOSECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELSTWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELMAXSTWINCOUNT));
		dp = CHAR_getInt( charaindex, CHAR_DUELPOINT );
		saacproto_DBUpdateEntryInt_send(
			acfd,
			DB_DUELPOINT,
			szKey,
			(max(dp,0)),
			info,
            CONNECT_getFdid(clifd ),0 );
	}
#endif
    chwk = CHAR_getCharPointer( charaindex);
#ifdef _AUTO_ADDADDRESS
	{
		int playernum = CHAR_getPlayerMaxNum();
		int i, k, oldnum=0, newnum=0;
		i = charaindex;
		for( k=0 ; k< playernum ; k++ ){
			char token[256];
			if( ++i >= playernum ) i = 0;
			if( i < 0 ) i =0;
			if( !CHAR_getCharUse(i) ) continue;
			if( charaindex == i ) continue;
 			if( !CHAR_getFlg( i, CHAR_ISTRADECARD) ) continue;
			if( oldnum < 3 && CHAR_getInt( i, CHAR_LV) > 100 ){
				if( ADDRESSBOOK_AutoaddAddressBook( charaindex, i) == TRUE ){
					sprintf( token, "陔忒-%s 樓�輮紜驐炬Ｅ踽蓐遙酵�え﹝", CHAR_getUseName( charaindex));
					CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
					oldnum++;
				}
			}else if( newnum < 5 && CHAR_getInt( i, CHAR_LV) < 30 ){
				if( ADDRESSBOOK_AutoaddAddressBook( charaindex, i) == TRUE ){
					sprintf( token, "陔忒-%s 樓�輮紜驐炬Ｅ踽蓐遙酵�え﹝", CHAR_getUseName( charaindex));
					CHAR_talkToCli( i, -1, token, CHAR_COLORYELLOW);
					newnum++;
				}
			}
			if( newnum >= 5 && oldnum >= 3 )
				break;
		}
	}
#endif

#ifdef _NEW_PLAYER_CF	//陔忒堤汜饜离

	CHAR_charSaveFromConnectAndChar( clifd,chwk ,FALSE );
	CHAR_endCharData(&ch);
	CHAR_endCharOneArray( charaindex);
	{
		int k=0;
		for( k=0;k<arraysizeof( add_pet); k++)	{
			CHAR_endCharOneArray( add_pet[k]);
		}
	}
    CONNECT_setState( clifd, WHILECREATE );
#endif

/*#ifdef _NEW_TESTSERVER	//畛菟聆彸侜督
	{
#ifdef _75_TEST
		int event_end[] = { 4, 81, 48, 49, 50, 51, 52, 63, 69, 70, 71, 72, 81, 88, 89, 109, 110, 111, 112, 113, 114, 115, 116, 118, 122, 127, 131, 136, 158, 159, 174, 102};
		int event_now[] = { 71};
		int i;
		for( i=0; i < arraysizeof( event_end); i++)	{
			NPC_EventSetFlg( charaindex, event_end[i]);
		}
		for( i=0; i < arraysizeof( event_now); i++)	{
			NPC_NowEventSetFlg( charaindex, event_now[i]);
		}
#else
		int event_end[] = { 4, 69, 70, 71, 72, 81, 48, 49, 50, 51, 52};
		int event_now[] = { 71};
		int i;
		for( i=0; i < arraysizeof( event_end); i++)	{
			NPC_EventSetFlg( charaindex, event_end[i]);
		}
		for( i=0; i < arraysizeof( event_now); i++)	{
			NPC_NowEventSetFlg( charaindex, event_now[i]);
		}
#endif


//7.0聆彸儂祥扢堤汜萸
		if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 35 )	{
			CHAR_setInt( charaindex, CHAR_FLOOR, 7407);
			CHAR_setInt( charaindex, CHAR_X, 27);
			CHAR_setInt( charaindex, CHAR_Y, 6);
		}else	{
			CHAR_setInt( charaindex, CHAR_LASTTALKELDER, 36);
			CHAR_setInt( charaindex, CHAR_FLOOR, 7305);
			CHAR_setInt( charaindex, CHAR_X, 21);
			CHAR_setInt( charaindex, CHAR_Y, 10);
		}

	}
	CHAR_charSaveFromConnectAndChar( clifd,chwk ,FALSE );
	CHAR_endCharData(&ch);
	CHAR_endCharOneArray( charaindex);
	{
		int k=0;
		for( k=0;k<arraysizeof( add_pet); k++)	{
			CHAR_endCharOneArray( add_pet[k]);
		}
	}
    CONNECT_setState( clifd, WHILECREATE );

#else
	CHAR_charSaveFromConnectAndChar( clifd,chwk ,FALSE );
	CHAR_endCharData(&ch);
	CHAR_endCharOneArray( charaindex);
	CHAR_endCharOneArray( petindex);
    CONNECT_setState( clifd, WHILECREATE );
#endif*/

}

static void CHAR_setCharFuncTable( Char *ch)
{
	int i;
	char *tmp[CHAR_FUNCTABLENUM] = {
		"",                 /*  CHAR_INITFUNC */
		"core_PreWalk",     /*  CHAR_WALKPREFUNC    */
		"core_PostWalk",    /*  CHAR_WALKPOSTFUNC   */
		"",                 /*  CHAR_PREOVERFUNC    */
		"",                 /*  CHAR_PREOVERFUNC    */
		"core_PlayerWatch", /*  CHAR_WATCHFUNC  */
		"core_Loop",        /*  CHAR_LOOPFUNC */
		"core_Dying",       /*  CHAR_DYINGFUNC */
		"core_PlayerTalked",/*  CHAR_TALKEDFUNC */
		"",                 /*  CHAR_PREATTACKEDFUNC    */
		"",                 /*  CHAR_POSTATTACKEDFUNC    */
		"",                 /*  CHAR_OFFFUNC    */
		"",                 /*  CHAR_LOOKEDFUNC */
		"",                 /*  CHAR_ITEMPUTFUNC    */
		"",                 /*  CHAR_SPECIALTALKEDFUNC    */
		"",                 /*  CHAR_WINDOWTALKEDFUNC    */
#ifdef _USER_CHARLOOPS
		"",					//	CHAR_LOOPFUNCTEMP1,
		"",					//	CHAR_LOOPFUNCTEMP2,
		"",					//CHAR_BATTLEPROPERTY,
#endif
	};
	for( i = 0; i < CHAR_FUNCTABLENUM; i ++ ) {
		strcpysafe( ch->charfunctable[i].string,
					sizeof( ch->charfunctable[i]),
					tmp[i]);
	}
}

void CHAR_loginAddItem( int charaindex )
{
/*
	int emptyindex, itemindex, id;
	int itemID[10]={	20128, 20130, 20171, 20176, 20296,
					20287, 20288, 20289, 20290, 20291};

	{
		int trn_num=0;
		trn_num=CHAR_getInt(charaindex,CHAR_TRANSMIGRATION);
		CHAR_setInt( charaindex, CHAR_MAXPOOLITEMHAVELIMIT, 10 + (trn_num * 4) )	;	
		CHAR_setInt( charaindex, CHAR_MAXPOOLPETHAVELIMIT , 5 + (trn_num * 2) );
	}
	while( 1){
		emptyindex = CHAR_findEmptyItemBox( charaindex );
		if( emptyindex == -1 ) break;
		id = rand()%10;
		itemindex = ITEM_makeItemAndRegist( itemID[id] );
		if( !ITEM_CHECKINDEX( itemindex) ) break;
		CHAR_setItemIndex( charaindex, emptyindex, itemindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charaindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);
		//andy_log
		print("@:%d.", emptyindex);
	}
	while( 1){
		emptyindex = CHAR_findEmptyPoolItemBox( charaindex );
		if( emptyindex == -1 ) break;
		id = rand()%10;
		itemindex = ITEM_makeItemAndRegist( itemID[id] );
		if( !ITEM_CHECKINDEX( itemindex) ) break;

		CHAR_setPoolItemIndex( charaindex, emptyindex, itemindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charaindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);
		//andy_log
		print("&:%d.", emptyindex);
	}
*/
}

void CHAR_loginCheckUserItem( int charaindex)
{
	typedef void (*ATTACHFUNC)(int,int);
	int i, itemindex;
	for( i=0; i<CHAR_STARTITEMARRAY; i++){ //潰脤蚾掘笢耋撿
		int ti;
		itemindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex)){
			continue;
		}
		ITEM_reChangeItemName( itemindex);
#ifdef _ITEM_PILENUMS
		ITEM_reChangeItemToPile( itemindex);
#endif
		ITEM_setItemUniCode( itemindex);

#ifdef _ANGEL_SUMMON
		if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM 
			|| ITEM_getInt( itemindex, ITEM_ID) == HEROITEM ) {
			int mindex, mission;
			// 潰脤陓昜垀衄�侕Й鮵恄鵖�
			mindex = checkIfAngelByName( ITEM_getChar( itemindex, ITEM_ANGELINFO));
			mission = atoi( ITEM_getChar( itemindex, ITEM_ANGELMISSION));
			if( mindex < 0 || mission != missiontable[mindex].mission ) {
				CHAR_setItemIndex( charaindex, i, -1 );
				ITEM_endExistItemsOne( itemindex);
				CHAR_sendItemDataOne( charaindex, i);
				continue;
			}

			if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM ) {
				char nameinfo[128];
				getMissionNameInfo( charaindex, nameinfo);
				// 潰脤妏氪陓昜蚾掘笢
				if( !strcmp( ITEM_getChar( itemindex, ITEM_ANGELINFO), nameinfo) ) {
					//CHAR_setWorkInt( charaindex, CHAR_WORKANGELMODE, TRUE);
					CHAR_sendAngelMark( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX), 1);
				}
			}
		}
#endif

#ifdef _CHECK_ITEM_MODIFY
		ITEM_checkItemModify( charaindex, itemindex);
#endif

		//Change fix 2004/07/05
		//迵�冼攃曼結遣炵警擰葸譁�//////////////////
		if( ITEM_getInt( charaindex, ITEM_NEEDPROFESSION) != 0 ){
			if( CHAR_getInt( charaindex, PROFESSION_CLASS ) != ITEM_getInt( itemindex, ITEM_NEEDPROFESSION) 
				&& ITEM_getInt( itemindex, ITEM_NEEDPROFESSION) > 0 ){
				if( (ti = CHAR_findEmptyItemBox( charaindex )) != -1 ){
					CHAR_setItemIndex( charaindex , i, -1);
					CHAR_setItemIndex( charaindex , ti, itemindex);
				}
				continue;
			}
		}
		////////////////////////////////////////////


		if( (i == ITEM_getEquipPlace( charaindex, itemindex )) ||
			i == CHAR_DECORATION1 || i == CHAR_DECORATION2 ){
			char category[256];
			ATTACHFUNC atf;
			atf=(ATTACHFUNC)ITEM_getFunctionPointer(itemindex,ITEM_ATTACHFUNC);
			if( atf )	atf( charaindex, itemindex );
			if( ITEM_canuseMagic( itemindex)) {
				snprintf( category, sizeof( category), "J%d",i);
				CHAR_sendStatusString( charaindex, category );
			}
		}else{
			if( (ti = CHAR_findEmptyItemBox( charaindex )) == -1 ) continue;
			CHAR_setItemIndex( charaindex , i, -1);
			CHAR_setItemIndex( charaindex , ti, itemindex);
		}
	}
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){ //潰脤觓湍耋撿
		itemindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex)) continue;
		ITEM_reChangeItemName( itemindex);
#ifdef _ITEM_PILENUMS
		ITEM_reChangeItemToPile( itemindex);
#endif

#ifdef _ANGEL_SUMMON
		if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM 
			|| ITEM_getInt( itemindex, ITEM_ID) == HEROITEM ) {
			int mindex, mission;
			// 潰脤陓昜垀衄�侕Й鮵恄鵖�
			mindex = checkIfAngelByName( ITEM_getChar( itemindex, ITEM_ANGELINFO));
			mission = atoi( ITEM_getChar( itemindex, ITEM_ANGELMISSION));
			if( mindex < 0 || mission != missiontable[mindex].mission ) {
				CHAR_setItemIndex( charaindex, i, -1 );
				ITEM_endExistItemsOne( itemindex);
				CHAR_sendItemDataOne( charaindex, i);
				continue;
			}
		}
#endif

		ITEM_setItemUniCode( itemindex);

#ifdef _DEATH_FAMILY_LOGIN_CHECK   // WON ADD 模逜桵腎�趧麮�
		if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_WARES ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_DISH ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_OTHER ){
			CHAR_setItemIndex( charaindex, i, -1 );
			ITEM_endExistItemsOne( itemindex);
			CHAR_sendItemDataOne( charaindex, i);
		}		

#endif

#ifdef _DEATH_CONTEND
		if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_WARES ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_DISH ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_OTHER ){
			CHAR_setItemIndex( charaindex, i, -1 );
			ITEM_endExistItemsOne( itemindex);
			CHAR_sendItemDataOne( charaindex, i);
		}
#endif

	}
	//潰脤敵溫虛耋撿
	for( i = 0 ; i < CHAR_MAXPOOLITEMHAVE ; i++ ){
		itemindex = CHAR_getPoolItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex) ) continue;

		ITEM_reChangeItemName( itemindex);
#ifdef _ITEM_PILENUMS
		ITEM_reChangeItemToPile( itemindex);
#endif

#ifdef _ANGEL_SUMMON
		if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM 
			|| ITEM_getInt( itemindex, ITEM_ID) == HEROITEM ) {
			int mindex, mission;
			// 潰脤陓昜垀衄�侕Й鮵恄鵖�
			mindex = checkIfAngelByName( ITEM_getChar( itemindex, ITEM_ANGELINFO));
			mission = atoi( ITEM_getChar( itemindex, ITEM_ANGELMISSION));
			if( mindex < 0 || mission != missiontable[mindex].mission ) {
				CHAR_setPoolItemIndex( charaindex, i, -1 );
				ITEM_endExistItemsOne( itemindex);
				continue;
			}
		}
#endif
		
		ITEM_setItemUniCode( itemindex);

#ifdef _DEATH_FAMILY_LOGIN_CHECK   // WON ADD 模逜桵腎�趧麮�
		if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_WARES ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_DISH ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_OTHER ){
			CHAR_setPoolItemIndex( charaindex, i, -1 );
			ITEM_endExistItemsOne( itemindex);
		}
#endif

#ifdef _DEATH_CONTEND
		if( ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_WARES ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_DISH ||
			ITEM_getInt( itemindex, ITEM_TYPE) == ITEM_OTHER ){
			CHAR_setPoolItemIndex( charaindex, i, -1 );
			ITEM_endExistItemsOne( itemindex);
		}
#endif
	}

	// WON ADD 党淏耋撿腔扢隅恀枙
	if(!NPC_EventCheckFlg( charaindex,134)){
		for (i = 0; i < CHAR_MAXITEMHAVE; i ++) {
			itemindex = CHAR_getItemIndex(charaindex, i);
			fix_item_bug(charaindex, itemindex);
		}	
		for (i = 0; i < CHAR_MAXPOOLITEMHAVE; i++) {
			itemindex = CHAR_getPoolItemIndex(charaindex, i);
			fix_item_bug(charaindex, itemindex);
		}
		NPC_EventSetFlg(charaindex, 134);
	}

	Check_P_I_UniCode( charaindex);
}
void CHAR_LoginBesideSetWorkInt( int charaindex, int clifd)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX1, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX2, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX3, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX4, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX5, -1);

	CHAR_setWorkInt( charaindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMINDEXI, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMCHANNEL, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMCHANNELQUICK, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMFLOOR, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMDP, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMPKFLAG, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMSETUPFLAG, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMMANINDEX, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMCHARINDEX, -1);
	
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEFLAG, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET0_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET1_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET2_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET3_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET4_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORKACTION, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFD, clifd);


	if( CHAR_getInt( charaindex, CHAR_SILENT) > 100000000 )
		CHAR_setInt( charaindex, CHAR_SILENT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec);
	CHAR_setWorkInt( charaindex, CHAR_WORKTALKCOUNT, 0 );
	CHAR_setWorkInt( charaindex, CHAR_WORKTALKTIME, (int)NowTime.tv_sec);
	CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1);
#ifdef _ITEM_ADDEXP	//vincent 冪桄枑汔
	CHAR_setWorkInt(charaindex,CHAR_WORKITEM_ADDEXP, 0);
	CHAR_setWorkInt(charaindex,CHAR_WORKITEM_ADDEXPTIME, 0);
#endif
#ifdef _BULL_FIXPLAYER
	if( NPC_EventCheckFlg( charaindex, 131 ) != TRUE )	{
		if( CHAR_getInt( charaindex, CHAR_ABULLSCORE) > 100 )
			CHAR_setInt( charaindex, CHAR_ABULLSCORE, 100);
		CHAR_setInt( charaindex, CHAR_ABULLTIME, 0);
		NPC_EventSetFlg( charaindex, 131);
		NPC_NowEndEventSetFlgCls( charaindex, 130);
	}
#endif
#ifdef _STATUS_WATERWORD //阨岍賜袨怓
	if( MAP_getMapFloorType( CHAR_getInt( charaindex, CHAR_FLOOR)) == 1 ){
		CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 1);
	}else{
		CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 0);
	}
#endif
#ifdef _NEWOPEN_MAXEXP
	if( CHAR_getInt( charaindex, CHAR_OLDEXP) > 0 || CHAR_getInt( charaindex, CHAR_OLDEXP) < 0 ){
		CHAR_ChangeExp( charaindex);
	}
#endif
#ifdef _PETSKILL_BECOMEFOX
    CHAR_setWorkInt( charaindex, CHAR_WORKFOXROUND, -1 );
#endif
#ifdef _MAP_TIME
	CHAR_setWorkInt(charaindex,CHAR_WORK_MAP_TIME,0);	
#endif
#ifdef _PETSKILL_LER
	CHAR_setWorkInt(charaindex,CHAR_WORK_RELIFE,0);
#endif

#ifdef _ITEM_ADDEXP2
	CHAR_setWorkInt( charaindex, CHAR_WORKITEM_ADDEXP,
			CHAR_getInt( charaindex, CHAR_ADDEXPPOWER) );
	CHAR_setWorkInt( charaindex, CHAR_WORKITEM_ADDEXPTIME ,
			CHAR_getInt( charaindex, CHAR_ADDEXPTIME) );
#endif

}

void CHAR_CheckProfessionSkill( int charaindex)
{
	int i, Pclass, skpoint=0, mynum=0;
	CHAR_HaveSkill *pSkil;

	if( NPC_EventCheckFlg( charaindex, 155) ) return;

	Pclass = CHAR_getInt( charaindex, PROFESSION_CLASS );
	skpoint = CHAR_getInt( charaindex, PROFESSION_SKILL_POINT);
	if( Pclass == PROFESSION_CLASS_NONE )return;


	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
			
		// 撮夔ID
		int skillID = CHAR_getCharSkill( charaindex, i);

		if( skillID < 0 ) continue;
		
		switch( skillID ){
		case 63:
			mynum = CHAR_getInt( charaindex, CHAR_TOUGH);
			if( Pclass == PROFESSION_CLASS_FIGHTER ) mynum = mynum-25;
			if( Pclass == PROFESSION_CLASS_WIZARD )	mynum = mynum-10;
			if( Pclass == PROFESSION_CLASS_HUNTER ) mynum = mynum-15;
			if( mynum < 0 ) mynum = 0;
			CHAR_setInt( charaindex, CHAR_TOUGH, mynum);
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, skpoint+1 );		
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
			pSkil->use = 0;
			break;
		case 64:
			CHAR_setInt( charaindex, CHAR_MAXMP, 100);
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, skpoint+1 );
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
			pSkil->use = 0;
			break;
		case 65:
			CHAR_setInt( charaindex, ATTACHPILE, 0);
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, skpoint+1 );
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
			pSkil->use = 0;
			break;
		}
	}
	{
		int myskillpoint, myskillnum=0, myskilllevel;
		int mysknum=0;

		myskillpoint= CHAR_getInt( charaindex, PROFESSION_SKILL_POINT );
		myskilllevel= CHAR_getInt( charaindex, PROFESSION_LEVEL );

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			if( CHAR_getCharSkill( charaindex, i) <= 0 ) continue;
			myskillnum++;
		}
		mysknum = myskilllevel - (myskillpoint + myskillnum);

		if( mysknum > 0 ){
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, myskillpoint + mysknum );
		}else if( mysknum < 0 ){
			int defsknum = (myskillpoint + myskillnum)-myskilllevel;
			while( 1){
				if( defsknum<= 0 ||
					(myskillpoint= CHAR_getInt( charaindex, PROFESSION_SKILL_POINT )) <= 0 ) break;
				CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, myskillpoint-1);
				defsknum--;
			}
		}
	}

	NPC_EventSetFlg( charaindex, 155);
}

void CHAR_ResetPoolPetEgg( int charaindex)
{
	int levelup, vital, str, tgh, dex;
	int raise, petindex, i;


	if( NPC_EventCheckFlg( charaindex, 157) ) return;

	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		petindex = CHAR_getCharPoolPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;

		if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) != 1 ||
			CHAR_getInt( petindex, CHAR_FUSIONRAISE) <= 0 ) continue;

		raise = CHAR_getInt( petindex, CHAR_FUSIONRAISE);
		raise--;
		CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time(NULL)+PETFEEDTIME);
		if( raise < 1 ) raise = 1;
		if( raise >= 40 ) raise = 30;
		CHAR_setInt( petindex, CHAR_FUSIONRAISE, raise);

		levelup = CHAR_getInt( petindex, CHAR_ALLOCPOINT);
		vital = ((levelup>>24) & 0xFF) + 4;
		str   = ((levelup>>16) & 0xFF) + 4;
		tgh   = ((levelup>> 8) & 0xFF) + 4;
		dex   = ((levelup>> 0) & 0xFF) + 4;
		if( vital < 0 ) vital = 0;
		if( str < 0 ) str = 0;
		if( tgh < 0 ) tgh = 0;
		if( dex < 0 ) dex = 0;
		//諶扽俶
		levelup = (vital<<24) + (str<<16) + (tgh<<8) + (dex<<0);
		CHAR_setInt( petindex, CHAR_ALLOCPOINT, levelup);
	}
	NPC_EventSetFlg( charaindex, 157);
}

// Robin add 笭扢唾粥  欱奀潔
void CHAR_ResetPetEggFusionTime(int charaindex)
{
	int i, petindex;
	int nowTime =0;
	//int leaveTime =0;
	//int lastFeedTime =0;
	//int anhour = PETFEEDTIME;

	//if( NPC_EventCheckFlg( charaindex, 157) ) return;
	
	nowTime = (int)time(NULL);
	//leaveTime = CHAR_getInt( charaindex, CHAR_LASTLEAVETIME);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
		petindex = CHAR_getCharPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;

		if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) != 1 ||
			CHAR_getInt( petindex, CHAR_FUSIONRAISE) <= 0 ) continue;

		//lastFeedTime = CHAR_getInt( petindex, CHAR_FUSIONTIMELIMIT);
		//if( leaveTime > 0) {
		//	CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, nowTime- (leaveTime-lastFeedTime));
		//}
		//else
		//	CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, nowTime);

		CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, nowTime);

	}

	//NPC_EventSetFlg( charaindex, 157);

}


BOOL CHAR_CheckProfessionEquit( int toindex)
{
	int i, j, itemindex, newindex;
	int FixItem[16]={
		2028,2029,2030,2031,2032,2033,2034,2035,
		2036,2037,2038,2039,2040,2041,2042,2043
	};
	CHAR_HaveSkill *pSkil;

	if(!NPC_EventCheckFlg( toindex, 149)){
		for (i = 0; i < CHAR_MAXITEMHAVE; i ++) {
			itemindex = CHAR_getItemIndex( toindex, i);
			if( !ITEM_CHECKINDEX( itemindex)) continue;

			for( j=0; j<16; j++){
				if( ITEM_getInt( itemindex, ITEM_ID) == FixItem[j] ){
					CHAR_setItemIndex( toindex, i, -1 );
					ITEM_endExistItemsOne( itemindex);
					newindex = ITEM_makeItemAndRegist( FixItem[j]);
					if( !ITEM_CHECKINDEX( newindex) ) break;
					CHAR_setItemIndex( toindex , i, newindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, toindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);

					CHAR_sendItemDataOne( toindex, i);

				}
			}
		}	

		for (i = 0; i < CHAR_MAXPOOLITEMHAVE; i++){
			int itemindex = CHAR_getPoolItemIndex(toindex, i);
			if( !ITEM_CHECKINDEX( itemindex)) continue;
			for( j=0; j<16; j++){
				if( ITEM_getInt( itemindex, ITEM_ID) == FixItem[j] ){
					CHAR_setPoolItemIndex( toindex, i, -1);
					ITEM_endExistItemsOne( itemindex);

					newindex = ITEM_makeItemAndRegist( FixItem[j]);
					if( !ITEM_CHECKINDEX( newindex) ) break;
					CHAR_setPoolItemIndex( toindex , i, newindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, toindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);

				}
			}
		}

		NPC_EventSetFlg( toindex, 149);
	}


	if( NPC_EventCheckFlg( toindex, 156) ) return TRUE;

	// WON ADD
	for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
		if( CHAR_getIntPSkill( toindex, i, SKILL_IDENTITY) > 0 ) continue;

		pSkil = CHAR_getCharHaveSkill( toindex, i );
		SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
		pSkil->use = 0;
	}
	{
		int myskillpoint= CHAR_getInt( toindex, PROFESSION_SKILL_POINT );
		int myskilllevel= CHAR_getInt( toindex, PROFESSION_LEVEL );
		int myskillnum = 0, mysknum = 0;

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			if( CHAR_getCharSkill( toindex, i) <= 0 ) continue;
			myskillnum++;
		}

		mysknum = myskilllevel - (myskillpoint + myskillnum);

		if( mysknum > 0 ){
			CHAR_setInt( toindex, PROFESSION_SKILL_POINT, myskillpoint + mysknum );
		}
	}

	{
		Skill work1[PROFESSION_MAX_LEVEL];
		int cnt=0;
		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			work1[i].data[SKILL_IDENTITY]=-1;
			work1[i].data[SKILL_LEVEL]=0;
		}

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			if( CHAR_getIntPSkill( toindex, i, SKILL_IDENTITY) <= 0 ) continue;
			work1[cnt].data[SKILL_IDENTITY] = CHAR_getIntPSkill( toindex, i, SKILL_IDENTITY);
			work1[cnt].data[SKILL_LEVEL] = CHAR_getIntPSkill( toindex, i, SKILL_LEVEL);
			cnt++;
		}
		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			print("%d,", work1[i].data[SKILL_IDENTITY] );
		}

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			pSkil = CHAR_getCharHaveSkill( toindex, i );
			if( pSkil == NULL  ) continue;
			pSkil->use = 0;
			if( work1[i].data[SKILL_IDENTITY] == -1 ) continue;
			SKILL_makeSkillData( &pSkil->skill, work1[i].data[SKILL_IDENTITY], work1[i].data[SKILL_LEVEL] );
			pSkil->use = 1;
		}
	}

	NPC_EventSetFlg( toindex, 156);

	return TRUE;
}

static void CHAR_setLuck( int charaindex);

#ifdef _CHANNEL_MODIFY
extern int *piOccChannelMember;
#endif

#ifdef _BAD_PLAYER             // WON ADD 冞輓俙模�旦�
void CHAR_login( int clifd, char* data, int saveindex, int badplayer )
#else
void CHAR_login( int clifd, char* data, int saveindex )
#endif
{
	int charaindex,objindex;
	Char    ch;
	int per;
DebugPoint=50;
//	char c_temp2[4096];
	if( CHAR_makeCharFromStringToArg( data , &ch ) == FALSE ){
		fprint ("make char error\n");
		goto MAKECHARDATAERROR;
	}
DebugPoint=70;
	CHAR_setCharFuncTable( &ch);
DebugPoint=71;
	charaindex =  CHAR_initCharOneArray( &ch );
DebugPoint=72;
	if( charaindex == -1 ){
		fprint ("make char error\n");
		CHAR_endCharData(&ch);
		goto MAKECHARDATAERROR;
	}
DebugPoint=80;
#ifdef _NEWSAVE
	CHAR_setInt( charaindex, CHAR_SAVEINDEXNUMBER, saveindex);
	print("湔紫蚾婥坰竘:%d\n", CHAR_getInt( charaindex, CHAR_SAVEINDEXNUMBER) );
#endif
	if( CHAR_getFlg( charaindex, CHAR_ISDIE)){
		print( "?data? ISDIE flg is standing.\n");
		CHAR_setFlg( charaindex, CHAR_ISDIE, FALSE);
	}
#ifdef _75_TEST
	{
		int event_end[] = { 4, 32, 33, 34, 81, 86, 87, 105, 83};
		int i;
		for( i=0; i < arraysizeof(event_end); i++)	{
			NPC_EventSetFlg( charaindex, event_end[i]);
		}
	}
#endif
#ifndef _FIX_UNNECESSARY
	// Arminius 7.9 airplane logout
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
		int oyaindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
		if( CHAR_CHECKINDEX( oyaindex)) {
			if (CHAR_getInt( oyaindex, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) {
				if ((CHAR_getInt( oyaindex, CHAR_BASEIMAGENUMBER) !=100355) &&
					(CHAR_getInt( oyaindex, CHAR_BASEIMAGENUMBER) !=100461))	{

					if(CHAR_getWorkInt(oyaindex,CHAR_NPCWORKINT5)==1) {
					  if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER)>=0){
						int fl,x,y;
						CHAR_getElderPosition( CHAR_getInt( charaindex, CHAR_LASTTALKELDER),
												&fl, &x, &y );
						CHAR_setInt(charaindex,CHAR_FLOOR,fl);
						CHAR_setInt(charaindex,CHAR_X,x);
						CHAR_setInt(charaindex,CHAR_Y,y);
					  }
					}
				}
			}
		}
	}
#endif
// Nuke 20040420: CHECK MAX POINT
#if 1 
  {
    int lv,vi,str,tou,dx,skup,trn,teq,quest,level,total,max;
    float table[]={437,490,521,550,578,620}; //跪蛌郔詢萸杅(熬10)
    //float table[]={620,660,700,740,780,820}; //跪蛌郔詢萸杅(熬10)
    lv = CHAR_getInt(charaindex,CHAR_LV);
  	vi = CHAR_getInt(charaindex,CHAR_VITAL);
	str = CHAR_getInt(charaindex,CHAR_STR);
	tou = CHAR_getInt(charaindex,CHAR_TOUGH);
	dx = CHAR_getInt(charaindex,CHAR_DEX);
    trn = CHAR_getInt(charaindex,CHAR_TRANSMIGRATION);
    teq = CHAR_getInt(charaindex,CHAR_TRANSEQUATION);
    skup = CHAR_getInt(charaindex,CHAR_SKILLUPPOINT);
    quest=(teq >> 16)& 0xFFFF;
    level=teq & 0xFFFF;
    total=(vi+str+tou+dx)/100+skup;
    max=(trn==0)?(lv-1)*3+20+10: // 0蛌447
      (lv-1)*3+table[trn-1]/12.0+quest/4.0+(level-trn*85)/4.0+10+1+10+trn*10;

		
/* 1蛌眕奻=汔撰萸杅+樟創萸扲+盪妢�恄�+盪妢脹撰+蛌ヶ蛅腦+昫船1萸+蛌摽蛅腦+蛌汜綻瞳 */
	if (trn==6) max=max-20; /* 鞠蛌奀羶衄蛌ヶ蛅腦迵蛌摽蛅腦 */
  if (total>max)
  {
	  print("\n笭覃萸杅[%s:%s]:%d->%d ",
	  		CHAR_getChar(charaindex,CHAR_CDKEY),
			  CHAR_getChar(charaindex,CHAR_NAME),
	  		total,max);
    CHAR_setInt(charaindex,CHAR_VITAL,1000);
    CHAR_setInt(charaindex,CHAR_STR,0);
    CHAR_setInt(charaindex,CHAR_TOUGH,0);
    CHAR_setInt(charaindex,CHAR_DEX,0);
    CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,max-10);
  }
	// 硃逋鞠蛌萸杅祥逋眳俙模
	if ((trn==6) && (total < max))
	{
   		print("\n硃逋萸杅[%s:%s]:%d->%d ",
		  CHAR_getChar(charaindex,CHAR_CDKEY),
		  CHAR_getChar(charaindex,CHAR_NAME),
		  total,max);
		CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,
			CHAR_getInt(charaindex,CHAR_SKILLUPPOINT)+(max-total));
	}
  }
#endif
	{
		int EQ_BBI=-1, EQ_ARM=-1, EQ_NUM=-1, EQ_BI=-1;
		int CH_BI = CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER);

		EQ_BBI = CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		EQ_ARM = CHAR_getItemIndex(charaindex,CHAR_ARM);
		EQ_NUM=ITEM_FIST;
		if ( ITEM_CHECKINDEX( EQ_ARM))	{
			EQ_NUM=ITEM_getInt(EQ_ARM,ITEM_TYPE);
		}

		EQ_BI=CHAR_getNewImagenumberFromEquip( EQ_BBI, EQ_NUM);

		if( CHAR_getInt( charaindex, CHAR_RIDEPET) != -1 )	{	//る唾

		}else	{	//準る唾
			if( EQ_BI != CH_BI )	{
				print("\n 準る唾倛砓 %d [%d=>%d]", charaindex,
					CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER),
					EQ_BI );
				CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, EQ_BI);
			}
		}
	}
	{
	  int x,y;
	  if( CHAR_isAppearPosition( CHAR_getInt( charaindex, CHAR_FLOOR),&x, &y ))	{
	  	int flelder, xelder, yelder;
			CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
	  			&flelder, &xelder, &yelder);
			   CHAR_setInt(charaindex, CHAR_FLOOR, flelder);
			   CHAR_setInt(charaindex, CHAR_X, xelder);
			   CHAR_setInt(charaindex, CHAR_Y, yelder);
	  }
	}

#ifdef _FIX_GAMBLENUM		
	{		
        if( CHAR_getInt(charaindex, CHAR_GAMBLENUM) < 0){
			CHAR_setInt(charaindex, CHAR_GAMBLENUM, 0);
		}
	}
#endif

#ifdef _DEATH_CONTEND
	{
		char cdkey[CDKEYLEN];
		memset( cdkey, 0, sizeof(cdkey));
		CONNECT_getCdkey( clifd, cdkey, sizeof( cdkey ));
		if( strlen(cdkey) > 0 ) {
			if( strcmp( cdkey, CHAR_getChar( charaindex, CHAR_CDKEY) ) ){
				print("梖瘍渣昫 %s->%s\n", CHAR_getChar( charaindex, CHAR_CDKEY), cdkey);
				CHAR_setChar( charaindex, CHAR_CDKEY, cdkey);
			}
		}else{
			print("梖瘍渣昫:%d\n", clifd);
		}
	}
	CHAR_setInt( charaindex, CHAR_PKLISTTEAMNUM, -1);
	CHAR_setInt( charaindex, CHAR_PKLISTLEADER, -1);
	//CHAR_setInt( charaindex, CHAR_GOLD, 1000000);
	PKLIST_InsertTeamNum( charaindex);
	if( CHAR_getInt( charaindex, CHAR_PKLISTTEAMNUM ) < 0 ||
		CHAR_getInt( charaindex, CHAR_PKLISTLEADER ) < 0 ){
		PKLIST_InsertTeamNum( charaindex);
	}
#endif

#ifdef _MAP_NOEXIT
	{
		int exfloor=-1,ex_X=-1,ex_Y=-1;
		int map_type=0;
		unsigned int point;
		point = MAP_getExFloor_XY( CHAR_getInt(charaindex, CHAR_FLOOR) , &map_type);
		if( point > 0 )	{
			if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) >= 0 )	{
				CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
					&exfloor, &ex_X, &ex_Y);
			}
			if( map_type >= 0 )	{	// map_type >= 0 蚥珂隙槨翹萸
				if( exfloor == map_type && exfloor >= 0 )	{//隙槨翹萸
				}else	{
					exfloor = (point>>16)&0xffffff;
					ex_X = (point>>8)&0xff;
					ex_Y = (point>>0)&0xff;
				}
			}
			if( CHECKFLOORID( exfloor) )	{	//潰脤滇潔瘍
				CHAR_setInt(charaindex, CHAR_FLOOR, exfloor);
				CHAR_setInt(charaindex, CHAR_X, ex_X);
				CHAR_setInt(charaindex, CHAR_Y, ex_Y);
			}
		}

#ifdef _BAD_PLAYER             // WON ADD 冞輓俙模�旦�

		NPC_NowEndEventSetFlgCls( charaindex, 135);			      // よ梓
		if( badplayer ){
			NPC_EventSetFlg(charaindex, 135);					  // よ梓
			CHAR_setInt(charaindex, CHAR_FLOOR, 887);
			CHAR_setInt(charaindex, CHAR_X, 56);
			CHAR_setInt(charaindex, CHAR_Y, 14);		
		
		}else if( CHAR_getInt(charaindex, CHAR_FLOOR) == 887 ){   // �蝜�婓888ㄛ隙暮翹萸
			CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER), &exfloor, &ex_X, &ex_Y);
			CHAR_setInt(charaindex, CHAR_FLOOR, exfloor);
			CHAR_setInt(charaindex, CHAR_X, ex_X);
			CHAR_setInt(charaindex, CHAR_Y, ex_Y);	

		} 
#endif
#ifdef _DEATH_FAMILY_LOGIN_CHECK
		CHAR_setInt( charaindex, CHAR_FLOOR, 130);
		CHAR_setInt( charaindex, CHAR_X, 56);
		CHAR_setInt( charaindex, CHAR_Y, 12);
#endif

#ifdef _NEW_INSERVERPOINT
		CHAR_setInt( charaindex, CHAR_FLOOR, 8250);
		CHAR_setInt( charaindex, CHAR_X, 15);
		CHAR_setInt( charaindex, CHAR_Y, 15);
#endif
	}
#endif
	if( CHAR_getInt(charaindex,CHAR_X) < 0 || CHAR_getInt(charaindex,CHAR_Y) < 0 ){
		CHAR_setInt(charaindex,CHAR_X,1);
		CHAR_setInt(charaindex,CHAR_Y,1);
	}
#ifdef _MAP_TIME
	// �蝜�腎�賮警媏暱м瑱店鉭霾躉�,隙善�踸�
	if(CHAR_getInt(charaindex,CHAR_FLOOR) >= 30017 && CHAR_getInt(charaindex, CHAR_FLOOR) <= 30021){
		CHAR_setInt(charaindex,CHAR_FLOOR,30008);
		CHAR_setInt(charaindex,CHAR_X,39);
		CHAR_setInt(charaindex,CHAR_Y,38);
	}
#endif

	objindex = initObjectFromObjectMember(OBJTYPE_CHARA,charaindex,
										  CHAR_getInt(charaindex,CHAR_X),
										  CHAR_getInt(charaindex,CHAR_Y),
										  CHAR_getInt(charaindex,CHAR_FLOOR));
	if( objindex == -1 ){
		fprint ("init obj error\n");
		goto DELETECHARDATA;
	}

	CHAR_setWorkInt( charaindex,CHAR_WORKOBJINDEX,objindex );
	CONNECT_setState( clifd, LOGIN );
	CONNECT_setCharaindex( clifd, charaindex );
	CHAR_LoginBesideSetWorkInt( charaindex, clifd);

#ifdef _CHANGEGOATMETAMO		// (祥褫羲) Syu  栺爛忤載遙陔芞
	if( NPC_EventCheckFlg( charaindex, 141 ) != TRUE )	{
		int z;
		int PETID1;
		int PETBASE1;
		for ( z = 0 ; z < CHAR_MAXPETHAVE; z ++ ) {
			int petindex = CHAR_getCharPet(charaindex,z);
			if( CHAR_CHECKINDEX( petindex) ) {
				PETID1 = CHAR_getInt( petindex, CHAR_PETID);
				PETBASE1 = CHAR_getInt( petindex , CHAR_BASEBASEIMAGENUMBER );
				if ( PETID1 == 1056 && PETBASE1 == 101497 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101607 ) ; 
				if ( PETID1 == 1057 && PETBASE1 == 101498 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101608 ) ; 
				if ( PETID1 == 1058 && PETBASE1 == 101499 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101609 ) ; 
				if ( PETID1 == 1059 && PETBASE1 == 101500 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101610 ) ; 
			}
		}
		for ( z = 0 ; z < CHAR_MAXPOOLPETHAVE; z ++ ) {
			int petindex = CHAR_getCharPoolPet(charaindex,z);
			if( CHAR_CHECKINDEX( petindex) ) {
				PETID1 = CHAR_getInt( petindex, CHAR_PETID);
				PETBASE1 = CHAR_getInt( petindex , CHAR_BASEBASEIMAGENUMBER );
				if ( PETID1 == 1056 && PETBASE1 == 101497 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101607 ) ; 
				if ( PETID1 == 1057 && PETBASE1 == 101498 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101608 ) ; 
				if ( PETID1 == 1058 && PETBASE1 == 101499 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101609 ) ; 
				if ( PETID1 == 1059 && PETBASE1 == 101500 ) 
					CHAR_setInt( petindex , CHAR_BASEBASEIMAGENUMBER , 101610 ) ; 
			}
		}
	}		
	NPC_EventSetFlg( charaindex, 141);
#endif
	
	CHAR_complianceParameter( charaindex );
	//潰脤�冼懭篽�
	{
		int i;
//		char *petstring;
		int ID1;
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			int petindex = CHAR_getCharPet(charaindex,i);
			if( CHAR_CHECKINDEX( petindex) ) {

#ifdef _CHECK_ENEMY_PET
				{
					int j;
					for( j=0;j<ENEMYTEMP_getEnemyNum();j++ ) {//ENEMY_getEnemyNum()
						if( CHAR_getInt( petindex, CHAR_PETID) == ENEMYTEMP_getInt( j, E_T_TEMPNO ) ) {//ENEMY_getInt( j, ENEMY_TEMPNO)
							if( ENEMYTEMP_getInt( j, E_T_PETFLG ) == 0 ){//ENEMY_getInt( j, ENEMY_PETFLG) 
								CHAR_setCharPet( charaindex, i, -1);//ь壺祥夔絞唾昜腔褒伎
								LogPet( CHAR_getChar( charaindex, CHAR_NAME ),
										CHAR_getChar( charaindex, CHAR_CDKEY ),
										CHAR_getChar( petindex, CHAR_NAME),
										CHAR_getInt( petindex, CHAR_LV),
										"login(ь壺祥夔絞唾昜腔褒伎)",
										CHAR_getInt( charaindex,CHAR_FLOOR),
										CHAR_getInt( charaindex,CHAR_X ),
										CHAR_getInt( charaindex,CHAR_Y ),
										CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
										);
								 CHAR_talkToCli(charaindex,-1,"炵苀ь壺斕旯奻腔準楊唾昜",CHAR_COLORWHITE);
							}
						}
					}
				}
#endif

#ifdef _UNIQUE_P_I			    
			    // CoolFish: 2001/10/11 Set Pet Unicode 
			    CHAR_setPetUniCode(petindex);
#endif
#ifdef _NEWOPEN_MAXEXP
				if( CHAR_getInt( petindex, CHAR_OLDEXP) > 0 || CHAR_getInt( charaindex, CHAR_OLDEXP) < 0 ){
					CHAR_ChangeExp( petindex);
				}
#endif
				ID1=CHAR_getInt( petindex, CHAR_PETID);
//				petstring = CHAR_makePetStringFromPetIndex( petindex);
#ifdef _REEDIT_PETBBIS
				if( ID1 == 1133 ){
					CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, 101578);
					CHAR_setInt( petindex, CHAR_BASEBASEIMAGENUMBER, 101578);
					print("PET:%s fix BBI\n", CHAR_getUseName(petindex));
				}
#endif
				// WON ADD 党淏啞誥ほ撮
				if( ID1 == 777 )CHAR_setInt( petindex, CHAR_SLOT, 7);
				CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, charaindex);
				// 党淏唾撮郔湮硉曹-1  Robin 20040803
				if( CHAR_getInt( petindex, CHAR_SLOT) < 0 ) {
					int j;
					for( j=0; j<CHAR_MAXPETSKILLHAVE; j++) {
						if( CHAR_getPetSkill( petindex, j) >= 0)
							CHAR_setInt( petindex, CHAR_SLOT, j+1);
					}
					//print(" 唾撮MAX曹-1蜊6 ");
				}

				CHAR_complianceParameter( petindex );
#ifdef _FIX_GP_PET_SKILL	 // WON ADD 党淏gp唾頗邈鎮扲
				if( ID1 == 1133){
					int j;
					for( j=0; j<CHAR_MAXPETSKILLHAVE; j++){
						if( CHAR_getPetSkill( petindex, j) == 210 ){ // 邈鎮扲
							CHAR_setPetSkill( petindex, j, -1);
						}
					}
				}
#endif
			}
		}
	}
	{
		int i;
//		char *petstring;//ttom
//		char *c_ptr_index;
//		char c_temp[4096];
//		char c_temp1[4096];
		int ID1;
//		int tran_no;
		BOOL b_find=FALSE;
		//ttom
		for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
			int petindex = CHAR_getCharPoolPet(charaindex,i);
			if( CHAR_CHECKINDEX( petindex) ) {
#ifdef _CHECK_ENEMY_PET
				{
					int j;
					for( j=0;j<ENEMYTEMP_getEnemyNum();j++ ) {//ENEMY_getEnemyNum()
						if( CHAR_getInt( petindex, CHAR_PETID) == ENEMYTEMP_getInt( j, E_T_TEMPNO ) ) {//ENEMY_getInt( j, ENEMY_TEMPNO)
							if( ENEMYTEMP_getInt( j, E_T_PETFLG ) == 0 ){//ENEMY_getInt( j, ENEMY_PETFLG) 
								CHAR_setCharPoolPet( charaindex, i, -1);//ь壺祥夔絞唾昜腔褒伎
								LogPet( CHAR_getChar( charaindex, CHAR_NAME ),
										CHAR_getChar( charaindex, CHAR_CDKEY ),
										CHAR_getChar( petindex, CHAR_NAME),
										CHAR_getInt( petindex, CHAR_LV),
										"login(ь壺祥夔絞唾昜腔褒伎)",
										CHAR_getInt( charaindex,CHAR_FLOOR),
										CHAR_getInt( charaindex,CHAR_X ),
										CHAR_getInt( charaindex,CHAR_Y ),
										CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
										);
								 CHAR_talkToCli(charaindex,-1,"炵苀ь壺斕旯奻腔準楊唾昜",CHAR_COLORWHITE);
							}
						}
					}
				}
#endif
#ifdef _UNIQUE_P_I			    
			    // CoolFish: 2001/10/11 Set Pet Unicode 
			    CHAR_setPetUniCode(petindex);
#endif
#ifdef _NEWOPEN_MAXEXP
				if( CHAR_getInt( petindex, CHAR_OLDEXP) > 0 ||
					CHAR_getInt( charaindex, CHAR_OLDEXP) < 0 ){
					CHAR_ChangeExp( petindex);
				}
#endif
				b_find=FALSE;
				ID1=CHAR_getInt( petindex, CHAR_PETID);
#ifdef _REEDIT_PETBBIS
				if( ID1 == 1133 ){
					CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, 101578);
					CHAR_setInt( petindex, CHAR_BASEBASEIMAGENUMBER, 101578);
				}
#endif
/* 
			    tran_no=CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
			    petstring = CHAR_makePetStringFromPetIndex( petindex);
				c_ptr_index=strstr(petstring,"name:");
			    getStringFromIndexWithDelim(c_ptr_index,":",2,c_temp,sizeof(c_temp));
			    c_ptr_index=c_temp;
			    getStringFromIndexWithDelim(c_ptr_index,"|",1,c_temp1,sizeof(c_temp1));
                                                                                                                                                                                                                                                                                                       
			    if(b_find){//ttom
			          CHAR_setCharPoolPet( charaindex, i, -1);
			          print("\nKill pool pet  ID=%d name=%s",ID1,c_temp1);
			          sprintf(c_temp2,"Kill pool pet  ID=%d name=%s",ID1,c_temp1);
			          LogKill(CHAR_getChar( charaindex, CHAR_NAME ),
			                  CHAR_getChar( charaindex, CHAR_CDKEY ),
			                  c_temp2);
			    }else{//ttom
*/			    
						CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, charaindex);
						CHAR_complianceParameter( petindex );				    
//				}//ttom
			}
		}
	}

//	CHAR_ResetPoolPetEgg( charaindex);
//	CHAR_CheckProfessionEquit( charaindex);
	CHAR_CheckProfessionSkill( charaindex);
	{
		int i=0;
		int skidx[]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
					1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
					3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,3,3,3,3,3,3,3};
		//20040702 Change fix
		//潰脤岆瘁衄眥珛祥蜆衄腔撮夔甜刉壺	
		for( i = CHAR_SKILLMAXHAVE-1; i >= 0; i -- ){
			CHAR_HaveSkill *pSkil;
			Skill*          skill;
			
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			skill = &pSkil->skill;
			if( pSkil == NULL )continue;
			if( pSkil->use == 0 )continue;	
			if( skidx[skill->data[SKILL_IDENTITY]] != CHAR_getInt( charaindex, PROFESSION_CLASS )
				&& skidx[skill->data[SKILL_IDENTITY]] != 4 ){//僕籵撮祥刉
				SKILL_makeSkillData( &pSkil->skill, 0, 0 );
				pSkil->use = 0;
			}
		}
	}
#ifdef _LOGIN_ADDITEM
	CHAR_loginAddItem( charaindex);
#endif
	CHAR_loginCheckUserItem( charaindex);
#ifdef _FIX_TSKILLCAN
	CHAR_PETSKILLCAN( charaindex);
#endif
	CHAR_complianceParameter( charaindex );

	// Robin add 笭扢唾粥  欱奀潔
	CHAR_ResetPetEggFusionTime( charaindex);

#ifdef _PROFESSION_FIX_LEVEL		// WON ADD 党淏眥珛冪桄硉
	{
		int i=0;
		for( i = 0 ; i < CHAR_SKILLMAXHAVE ; i ++ ){
			CHAR_HaveSkill* hskill;

			hskill = CHAR_getCharHaveSkill( charaindex, i );

			if( hskill != NULL && hskill->use == TRUE ){
				int skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);
				if( skill_level < 10 ){
					SKILL_setInt( &hskill->skill, SKILL_LEVEL, 10* 100 );
				}
			}
		}
	}
#endif

#ifdef _CHANNEL_MODIFY
	if(CHAR_getInt(charaindex,PROFESSION_CLASS) > 0){
		int i,pclass = CHAR_getInt(charaindex,PROFESSION_CLASS) - 1,Empty = -1;
		// �藶伂齡曼腑紫擘Ъ樓邾Ъ樊睅芩躆�,�藽閨邾蚧踰紫�
		for(i=0;i<getFdnum();i++){
			if(*(piOccChannelMember + (pclass * getFdnum()) + i) == charaindex) break;
			else if(*(piOccChannelMember + (pclass * getFdnum()) + i) == -1 && Empty == -1) Empty = i;
		}
		// 梑祥善導暮翹
		if(i == getFdnum()){
			if(Empty != -1) *(piOccChannelMember + (pclass * getFdnum()) + Empty) = charaindex;
		}
	}
#endif

#ifdef _STREET_VENDOR
	CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
	CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
	{
		int i;
		for(i=0;i<MAX_SELL_ITEM;i++) CHAR_clearStreetVendor(charaindex,i);
	}
#endif
#ifdef _NEW_MANOR_LAW
	if(CHAR_getInt(charaindex,CHAR_MOMENTUM) < 0) CHAR_setInt(charaindex,CHAR_MOMENTUM,0);
	if(CHAR_getInt(charaindex,CHAR_MOMENTUM) > MAX_PERSONAL_MOMENTUM) CHAR_setInt(charaindex,CHAR_MOMENTUM,MAX_PERSONAL_MOMENTUM);
#endif

#ifdef _PETSKILL_BECOMEPIG
	if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){
        CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( charaindex, CHAR_BECOMEPIG_BBI ) );
	}
	//change fix 揭燴蛹竭湮腔拫薯奀潔
	else{
		CHAR_setInt( charaindex, CHAR_BECOMEPIG, -1 );
	}
#endif


	lssproto_CharLogin_send( clifd, SUCCESSFUL,"" );
	per = ENCOUNT_getEncountPercentMin( charaindex,
									CHAR_getInt( charaindex, CHAR_FLOOR),
									CHAR_getInt( charaindex, CHAR_X),
									CHAR_getInt( charaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, per);
	}
	per = ENCOUNT_getEncountPercentMax( charaindex,
									CHAR_getInt( charaindex, CHAR_FLOOR),
									CHAR_getInt( charaindex, CHAR_X),
									CHAR_getInt( charaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, per);
	}

	CHAR_setLuck( charaindex);
	{
		int i;
		char*   statuscat[]={"C","P","I","S","D","E","F"};
		for( i=0 ; i<arraysizeof(statuscat) ; i++ )
			CHAR_sendStatusString( charaindex, statuscat[i] );
	}
	{
		int		i;
		char	category[3];
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			snprintf( category, sizeof( category), "K%d",i);
			CHAR_sendStatusString( charaindex, category );
			snprintf( category, sizeof( category), "W%d",i);
			CHAR_sendStatusString( charaindex, category );
		}
	}
	{
		int petarray = CHAR_getInt( charaindex, CHAR_DEFAULTPET);
		if( CHAR_CHECKPETINDEX( petarray)) {
			if( !CHAR_CHECKINDEX( CHAR_getCharPet( charaindex, petarray))) {
				petarray = -1;
			}
		}else {
			petarray = -1;
		}
		lssproto_KS_send( clifd, petarray, TRUE);
	}
	{
		int flg = 0;
		CHAR_setFlg( charaindex, CHAR_ISSHOWBATTLEMSG, 0);
		CHAR_setFlg( charaindex, CHAR_ISDUEL, 0);
		CHAR_setFlg( charaindex, CHAR_ISTRADECARD, 0);
		if( CHAR_getFlg( charaindex, CHAR_ISPARTY)) flg |= CHAR_FS_PARTY;
		if( CHAR_getFlg( charaindex, CHAR_ISDUEL)) flg |= CHAR_FS_DUEL;
		if( CHAR_getFlg( charaindex, CHAR_ISPARTYCHAT)) flg |= CHAR_FS_PARTYCHAT;
		if( CHAR_getFlg( charaindex, CHAR_ISTRADECARD)) flg |= CHAR_FS_TRADECARD;
#ifdef _CHANNEL_MODIFY
		CHAR_setFlg(charaindex,CHAR_ISTELL,0);
		CHAR_setFlg(charaindex,CHAR_ISFM,0);
		CHAR_setFlg(charaindex,CHAR_ISOCC,0);
		CHAR_setFlg(charaindex,CHAR_ISCHAT,0);
		CHAR_setFlg(charaindex,CHAR_ISSAVE,0);
#endif
        // CoolFish: 2001/4/18
		CHAR_setFlg( charaindex, CHAR_ISTRADE, 0);
#ifdef _AUCPROTOCOL				// (祥褫羲) Syu ADD 鼴闖け耋羲壽Protocol
		CHAR_setFlg( charaindex , CHAR_ISAUC , 0 ) ; 
#endif
		lssproto_FS_send( clifd, flg);

	}

	MAP_sendArroundCharNeedFD( clifd, charaindex );
	CHAR_setInt( charaindex, CHAR_LOGINCOUNT, CHAR_getInt( charaindex, CHAR_LOGINCOUNT) + 1  );
	CHAR_sendCToArroundCharacter( objindex );
	CHAR_sendArroundCharaData( charaindex );
	CHAR_sendWatchEvent(objindex,CHAR_ACTSTAND,NULL,0,TRUE);
#ifndef _DEATH_CONTEND
	ADDRESSBOOK_notifyLoginLogout(charaindex,1);
#endif
	CHAR_setWorkInt(charaindex,CHAR_WORKLASTATTACKCHARAINDEX,-1);
	if(CHAR_getInt(charaindex,CHAR_HP) <= 0 ){
		typedef int (*DYINGFUNC)(int);
		DYINGFUNC   dyingfunc=NULL;
		dyingfunc = (DYINGFUNC)CHAR_getFunctionPointer(charaindex, CHAR_DYINGFUNC );
		CHAR_setInt(charaindex,CHAR_HP,1);    // HP 扢峈 1
		logHack( CHAR_getWorkInt(charaindex,CHAR_WORKFD), HACK_HP);
	}
	if( CHAR_getInt( charaindex,CHAR_SKILLUPPOINT ) >=1 )
		CHAR_Skillupsend(charaindex);
	CHAR_checkEffect( charaindex);

#ifdef _DEATH_FAMILY_LOGIN_CHECK   // WON ADD 模逜桵腎�趧麮�
//	if( NPC_EventCheckFlg( charaindex, 150 ) != TRUE )	{
	{
		char *char_id, *char_name;
		// ь壺模逜よ梓
		SetFMVarInit(charaindex);
		char_id = CHAR_getChar( charaindex, CHAR_CDKEY );
		char_name = CHAR_getChar( charaindex, CHAR_NAME );		
		saacproto_new_ACFM_Login_send(acfd, charaindex, char_id, char_name );
	}
//		NPC_EventSetFlg( charaindex, 150);
//	}
#else

	// CoolFish: Family 2001/5/30
	if ((CHAR_getChar(charaindex, CHAR_FMNAME)!=NULL) &&
		(CHAR_getInt(charaindex, CHAR_FMINDEX) >= 0) &&
		(strcmp(CHAR_getChar(charaindex, CHAR_FMNAME), "") != 0))
	{
#ifdef _FMVER21
		int eventflag = 0;
		// 岆瘁睫磁逜酗腕訧跡
		if (CheckLeaderQ(charaindex) >= 0)
			eventflag = 1;
		if (CHAR_getInt(charaindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
			SetFMPetVarInit(charaindex);
	   	saacproto_ACFMCharLogin_send(
				 acfd,
	   	   CHAR_getChar(charaindex, CHAR_FMNAME),
	   	   CHAR_getInt(charaindex, CHAR_FMINDEX),
	   	   CHAR_getChar(charaindex, CHAR_NAME),
	   	   CHAR_getChar(charaindex, CHAR_CDKEY),
	   	   CHAR_getInt(charaindex, CHAR_LV), eventflag,
	   	   CONNECT_getFdid(clifd)
#ifdef _FM_MODIFY
				 ,getServernumber()
#endif
				 );
#endif
	}
#ifdef _FMVER21
	else
		SetFMVarInit(charaindex);
#endif	
	
#endif

	AnnounceToPlayerWN( clifd );

#ifdef _ANGEL_SUMMON
	{
		int mindex;
		mindex = checkIfOnlyAngel( charaindex);
		if( mindex != -1 ) {
			if( missiontable[mindex].flag == MISSION_WAIT_ANSWER ) {
				lssproto_WN_send( clifd, WINDOW_MESSAGETYPE_ANGELMESSAGE, 
				WINDOW_BUTTONTYPE_YESNO, 
				CHAR_WINDOWTYPE_ANGEL_ASK,
				-1,
				"醴ヶ藹逜佹酈ㄛ儕鍾蠅剒猁斕腔堆疆ㄛヶ厘扆梑蚋氪懂秏鏢涴虳藹逜ㄛ斕岆瘁堋砩堆疆ˋ" );
			}
		}
	}
#endif

	print("\n腎翻�冼屪�備:%s ", CHAR_getChar(charaindex, CHAR_NAME ) );
	
	{
		unsigned long ip;
		char ipstr[512];

		ip=CONNECT_get_userip(clifd);
		sprintf(ipstr,"%d.%d.%d.%d",
        	((unsigned char *)&ip)[0],
        	((unsigned char *)&ip)[1],
        	((unsigned char *)&ip)[2],
        	((unsigned char *)&ip)[3]);

		LogLogin(
			CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME),
			saveindex, ipstr
			);
	}

#ifdef _TEACHER_SYSTEM
	// 悝汜奻盄籵眭絳呇
	if(strlen(CHAR_getChar(charaindex,CHAR_TEACHER_ID)) > 0 && strlen(CHAR_getChar(charaindex,CHAR_TEACHER_NAME)) > 0){
		int iPlayernum = CHAR_getPlayerMaxNum(),i;
		char szMsg[128];

		for(i=0;i<iPlayernum;i++){
			if(CHAR_getCharUse(i) == FALSE) continue;
			if(strcmp(CHAR_getChar(charaindex,CHAR_TEACHER_ID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
				 strcmp(CHAR_getChar(charaindex,CHAR_TEACHER_NAME),CHAR_getChar(i,CHAR_NAME)) == 0){
				sprintf(szMsg,"悝汜 %s 眒奻盄",CHAR_getChar(charaindex,CHAR_NAME));
				CHAR_talkToCli(i,-1,szMsg,CHAR_COLORYELLOW);
				break;
			}
		}
	}
#endif

#ifdef _ITEM_SETLOVER
	// 痲ぺ奻盄籵眭勤源
		if(strlen( CHAR_getChar( charaindex, CHAR_LOVE)) > 0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERID))>0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERNAME))>0){
		int iPlayernum = CHAR_getPlayerMaxNum(),i;
		char szMsg[128];
		for(i=0;i<iPlayernum;i++){
			if(CHAR_getCharUse(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_LOVE), "YES") &&
        	!strcmp(CHAR_getChar(charaindex,CHAR_LOVERID),CHAR_getChar(i,CHAR_CDKEY)) &&
        	!strcmp(CHAR_getChar(charaindex,CHAR_LOVERNAME),CHAR_getChar(i,CHAR_NAME))){
				sprintf(szMsg,"斕腔乾�� %s 奻盄賸",CHAR_getChar(charaindex,CHAR_NAME));
				CHAR_talkToCli(i,-1,szMsg,CHAR_COLORYELLOW);
				break;
			}
		}
	}
#endif


	return;

DELETECHARDATA:
	CHAR_endCharOneArray( charaindex );
MAKECHARDATAERROR:
	{
		int i;
		char cdkey[CDKEYLEN];
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			if( CHAR_CHECKINDEX(ch.unionTable.indexOfPet[i] ) ) {
				CHAR_endCharOneArray( ch.unionTable.indexOfPet[i] );
			}
		}
        CONNECT_getCdkey( clifd, cdkey, sizeof(cdkey ));
        saacproto_ACLock_send( acfd, cdkey, UNLOCK, CONNECT_getFdid(clifd) );
    }
    CONNECT_setState(clifd, WHILECANNOTLOGIN );
    CONNECT_setCharaindex( clifd, -1 );
	lssproto_CharLogin_send( clifd, FAILED, "Download data ok,but cannot make chara");
}

BOOL CHAR_charSaveFromConnectAndChar( int fd, Char* ch, BOOL unlock )
{
	char*   chardata;
    char cdkey[CDKEYLEN];

	chardata = CHAR_makeStringFromCharData( ch );
	if( chardata == NULL )return FALSE;
    CONNECT_getCdkey( fd, cdkey, sizeof(cdkey));

#ifdef _NEWSAVE
//	print("saveindex_save:%d\n",ch->data[CHAR_SAVEINDEXNUMBER]);
	saacproto_ACCharSave_send( acfd, cdkey,
							   ch->string[CHAR_NAME].string,
							   CHAR_makeOptionString( ch ),  chardata,
							   unlock, CONNECT_getFdid(fd), ch->data[CHAR_SAVEINDEXNUMBER] );
#else
	saacproto_ACCharSave_send( acfd, cdkey,
							   ch->string[CHAR_NAME].string,
							   CHAR_makeOptionString( ch ),  chardata,
							   unlock, CONNECT_getFdid(fd) );
#endif
	return TRUE;
}

BOOL CHAR_charSaveFromConnect( int fd , BOOL unlock)
{
	Char*   ch;
	int charaindex = CONNECT_getCharaindex(fd);
	if( CHAR_CHECKINDEX( charaindex ) == FALSE)return FALSE;
	ch = CHAR_getCharPointer( charaindex );
	if( !ch )return FALSE;
#ifdef _CHAR_POOLITEM
	if( CHAR_SaveDepotItem( charaindex) == TRUE ){
		print("saveDepotItem:%d \n", charaindex);
	}
#endif
#ifdef _CHAR_POOLPET
	if( CHAR_SaveDepotPet( charaindex) == TRUE ){
		print("saveDepotPet:%d \n", charaindex);
	}
#endif

	return CHAR_charSaveFromConnectAndChar( fd, ch, unlock );
}


/*------------------------------------------------------------
 * 痲精囮坳哱埰堎鄹勗  埱倜竘竘痲精囮坳哱ぁ拻埵笢囮騷  堜禱
 *   埰
 * 礎倳
 *  charaindex  int     す籤溘袲淏↓摯騷璃溢蚗袲筒
 * 蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
static void CHAR_dropItemAtLogout( int charaindex )
{
	int i;
	for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
		int     itemindex;
		itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_CHECKINDEX(itemindex) == FALSE )continue;
		if( ITEM_getInt(itemindex,ITEM_DROPATLOGOUT ) == TRUE ) {
			{
				LogItem(
					CHAR_getChar( charaindex, CHAR_NAME ),
					CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 婓item腔log笢崝樓item靡備
					itemindex,
#else
		       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
					"LogoutDel(腎堤摽秏囮腔耋撿)",
					CHAR_getInt( charaindex,CHAR_FLOOR),
					CHAR_getInt( charaindex,CHAR_X ),
 		      		CHAR_getInt( charaindex,CHAR_Y ),
					ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
				);
			}
			CHAR_setItemIndex( charaindex, i ,-1);
	       		CHAR_sendItemDataOne( charaindex, i);
			ITEM_endExistItemsOne(itemindex);
		}
	}
}

BOOL _CHAR_logout( char *file, int line, int clifd, BOOL save )
{
	int     charindex, battleindex;
#ifdef _STREET_VENDOR
	int toindex;
#endif
	BOOL	ret = TRUE;
	int	fmindexi, channel, i;
	charindex = CONNECT_getCharaindex( clifd );
	if( !CHAR_CHECKINDEX( charindex)) {
		print( "logout err : clifd[%d] charaindex[%d] %s:%d from %s:%d \n", 
				clifd, charindex, __FILE__, __LINE__, file, line);
		ret = FALSE;
	}

#ifdef _DEATH_CONTEND
//andy_log
print("logout\n");
	if( CHAR_getWorkInt( charindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
//andy_log
print("BATTLE NONE\n");
		if( CHAR_getInt( charindex, CHAR_FLOOR) != 8250 &&
			CHAR_getInt( charindex, CHAR_PKLISTLEADER) == 1 ){
//andy_log
print("PlayerLogout_Exit()\n");
			NPC_PKLIST_PlayerLogout_Exit( charindex );
		}
	}
#endif
	battleindex = CHAR_getWorkInt( charindex, CHAR_WORKBATTLEINDEX );
	if( battleindex >= 0 ){
		BATTLE_EscapeDpSend( battleindex, charindex );
		CHAR_setInt( charindex, CHAR_DUELPOINT,
			CHAR_getInt( charindex, CHAR_DUELPOINT )
			+ CHAR_getWorkInt( charindex, CHAR_WORKGETEXP ) );
		BATTLE_Exit( charindex, battleindex );
	}

	CHAR_dropItemAtLogout( charindex );
	CHAR_DischargeParty( charindex, 0);
	if(CHAR_getChar(charindex, CHAR_FMNAME) != NULL )	{
		if(strlen(CHAR_getChar(charindex, CHAR_FMNAME)) > 0 &&
			strlen(CHAR_getChar(charindex, CHAR_FMNAME)) < sizeof( STRING32) )
		if( (CHAR_getInt(charindex, CHAR_FMINDEX) >= 0)	&&                
				(strcmp(CHAR_getChar(charindex, CHAR_FMNAME), "") != 0) )	{
	   		saacproto_ACFMCharLogout_send(acfd,
	   		   CHAR_getChar(charindex, CHAR_FMNAME),
	   		   CHAR_getInt(charindex, CHAR_FMINDEX),
	   		   CHAR_getChar(charindex, CHAR_NAME),
	   		   CHAR_getChar(charindex, CHAR_CDKEY),
	   		   CHAR_getInt(charindex, CHAR_LV),
	   		   CHAR_getWorkInt(charindex, CHAR_WORKFMINDEXI),
	   		   CONNECT_getFdid(clifd));
		}
	}

#ifdef _CHATROOMPROTOCOL			// (祥褫羲) Syu ADD 謐毞弅け耋
	if ( CHAR_getWorkInt ( charindex , CHAR_WORKCHATROOMTYPE ) != 0 ) {
		ChatRoom_Leave ( charindex ) ;
	}
#endif
#ifdef _STREET_VENDOR
	// �觷瞍疺Ъ艩褒眙�婓蝠眢笢
	if(CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR) == 2){
		// �○鷅豖�蝠眢腔��
		toindex = CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR_WHO);
		if(toindex > -1 && CHAR_CHECKINDEX(toindex)){
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
			lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
			CHAR_talkToCli(toindex,-1,"虛模�＋�蝠眢",CHAR_COLORYELLOW);
		}
	}
	// 俙模岆鎗源
	else if(CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR) == 3){
		// �○鷈艩�
		toindex = CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR_WHO);
		if(toindex > -1 && CHAR_CHECKINDEX(toindex)){
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,1);
		}
	}
#endif

	// Robin 0606 channel	
	fmindexi = CHAR_getWorkInt(charindex, CHAR_WORKFMINDEXI);
	channel = CHAR_getWorkInt(charindex, CHAR_WORKFMCHANNEL);
	i = 0;
	if( (fmindexi > 0) && (channel >= 0) && (fmindexi < FAMILY_MAXNUM) && (channel < FAMILY_MAXCHANNEL)) {
		while( i < FAMILY_MAXMEMBER ) {
		    if( channelMember[fmindexi][channel][i] == charindex ) {
		    	channelMember[fmindexi][channel][i] = -1;
		    	break;
		    }
		    i++;
		}
	}
#ifdef _CHANNEL_MODIFY
	if(CHAR_getInt(charindex,PROFESSION_CLASS) > 0){
		int i,pclass = CHAR_getInt(charindex,PROFESSION_CLASS) - 1;
		for(i=0;i<getFdnum();i++){
			if(*(piOccChannelMember + (pclass * getFdnum()) + i) == charindex){
				*(piOccChannelMember + (pclass * getFdnum()) + i) = -1;
				break;
			}
		}
	}
#endif
	// Robin 0707 petFollow
	CHAR_pickupFollowPet( charindex, -1 );
	for( i=0; i<FAMILY_MAXMEMBER; i++ ){
		if ((fmindexi > 0) && (fmindexi < FAMILY_MAXNUM)){
			if( familyMemberIndex[fmindexi][i] == charindex )
				familyMemberIndex[fmindexi][i] = -1;
		}
	}
	// Robin 0629 silent
	if( CHAR_getInt( charindex, CHAR_SILENT) > 0 ){
		int silent_t, login_t;
		login_t = CHAR_getWorkInt(charindex, CHAR_WORKLOGINTIME);
		silent_t = CHAR_getInt( charindex, CHAR_SILENT);
		silent_t = silent_t - ( (int)NowTime.tv_sec-login_t);
		if( silent_t<0 ) silent_t = 0;
		CHAR_setInt( charindex, CHAR_SILENT, silent_t);
	}

#ifdef _ITEM_ADDEXP2
	if( CHAR_getWorkInt( charindex, CHAR_WORKITEM_ADDEXP) > 0 ) {
		// 腎�輮敔Л�虴彆赻雄熬90鏃
		CHAR_setWorkInt( charindex, CHAR_WORKITEM_ADDEXPTIME,
			CHAR_getWorkInt( charindex, CHAR_WORKITEM_ADDEXPTIME) - 90 );
		if( CHAR_getWorkInt( charindex, CHAR_WORKITEM_ADDEXPTIME) <= 0 ) {
			CHAR_setWorkInt( charindex, CHAR_WORKITEM_ADDEXPTIME, 0);
			CHAR_setWorkInt( charindex, CHAR_WORKITEM_ADDEXP, 0);
		}
	}
	CHAR_setInt( charindex, CHAR_ADDEXPPOWER,
			CHAR_getWorkInt( charindex, CHAR_WORKITEM_ADDEXP) );
	CHAR_setInt( charindex, CHAR_ADDEXPTIME,
			CHAR_getWorkInt( charindex, CHAR_WORKITEM_ADDEXPTIME) );
#endif

#ifdef _TIME_TICKET
	if( check_TimeTicketMap( CHAR_getInt( charindex, CHAR_FLOOR)) ) {
		CHAR_warpToSpecificPoint( charindex, 7001, 41, 6);
	}
#endif

	// Robin add
	CHAR_setInt( charindex, CHAR_LASTLEAVETIME, (int)time(NULL));

	if( save ){
		CHAR_charSaveFromConnect( clifd ,TRUE);
	}

#ifndef _DEATH_CONTEND
	ADDRESSBOOK_notifyLoginLogout(charindex,0);
#endif
	CHAR_CharaDeleteHavePet( charindex);
	CHAR_CharaDelete( charindex );

	return ret;
}
/*------------------------------------------------------------
 * watch騷穫璃哱禱邞堎凝汔竣凝鏍攝蚗袲埰堎
 * 礎倳
 *  objindex    int             騷穫璃哱禱堁喫媃倜す籤溘摯酘々喇攝袲哱
 *                                  騷璃溢蚗袲筒
 *	index		int				騷穫璃哱禱堁喫媃倜す籤溘摯騷璃溢蚗袲筒
 *  recvindex   int				騷穫璃哱禱偏崹敁堎す籤溘摯index
 *	chac		int				囮袲で砫璃摯攣  
 * 蒍堇偯 		TRUE:	邞埱趙笢笢源
 *				FALSE: 	藗  
 ------------------------------------------------------------*/
static BOOL CHAR_sendWatchEvent_sendCheck( int objindex, int index,
											int recvindex, int chac)
{
	/* 猷覕誑NPCぁЪ鱗忒NPC摯鄹  崹堎 */
	if( OBJECT_getType( objindex) == OBJTYPE_CHARA &&
		CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER &&
		CHAR_getInt(recvindex, CHAR_WHICHTYPE )
		!= CHAR_TYPEPLAYER )
	{
		return FALSE;
	}
	/* す籤溘雄翻摯watch 毀鏍攝蚗袲  媃勗犎 */
	if( OBJECT_getType( objindex) != OBJTYPE_CHARA) {
		return TRUE;
	}
	/*
	 * 猷覕喜摯倵拻摯    毀  媃勗埰堎
	 */
	if( index == recvindex && chac == CHAR_ACTWALK ) {
		return FALSE;
	}

	if( CHAR_getInt( recvindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		/* 橋    摯硞勗毀  藗煦凝�梖近鵗ぴ聿� */
		if( CHAR_getWorkInt( recvindex,CHAR_WORKBATTLEMODE)
			!= BATTLE_CHARMODE_NONE)
		{
			return FALSE;
		}
		/* 囮袲で砫璃啋崨  笢鄹 */
		if( CHAR_getWorkInt( index, CHAR_WORKACTION) == -1 ) {
			if( OBJECT_getType(objindex) == OBJTYPE_CHARA) {
				/*   啋蚕↓  贖煦埱倜��
				 * 虮雄翻毀眥摯隍璃泬↓勗倵拻禱邞嚭媃埵笢
				 */
				if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE )
						== CHAR_PARTY_CLIENT &&
					CHAR_getWorkInt( recvindex, CHAR_WORKPARTYMODE)
						!= CHAR_PARTY_NONE &&
					chac == CHAR_ACTWALK )
				{
					int found = FALSE;
					int loop;
					/* 碳禱ぅ迋堎 */
					for( loop = 1; loop < CHAR_PARTYMAX; loop ++ ) {
						int partycharaindex = CHAR_getPartyIndex( recvindex, loop);
						/*   笢倜す籤溘誑猷覕摯蚕↓  贖摯碳煦埱倜 */
						if( partycharaindex == index ) {
							found = TRUE;
							break;
						}
					}
					if( found ) return FALSE;
				}
				/* 虮毀碳摯STAND禱邞嚭媃埵笢 */
				if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE)
					== CHAR_PARTY_CLIENT &&
					CHAR_getWorkInt( recvindex, CHAR_WORKPARTYMODE)
						== CHAR_PARTY_LEADER &&
					chac == CHAR_ACTSTAND)
				{
					int found = FALSE;
					int loop;
					/* 碳禱ぅ迋堎 */
					for( loop = 1; loop < CHAR_PARTYMAX; loop ++ ) {
						int partycharaindex
							= CHAR_getWorkInt( recvindex,
											CHAR_WORKPARTYINDEX1+loop);
						/* STAND媃倜す籤溘誑猷覕摯蚕↓  贖摯碳煦埱倜 */
						if( partycharaindex == index ) {
							found = TRUE;
							break;
						}
					}
					if( found ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}

void CHAR_sendWatchEvent( int objindex, int chac, int* opt,
						  int optlen,BOOL myflg )
{
	int     i;
	int j;
	int     x=0,y=0,dir=0;
	int     floor;
	int     watchopt[32];
	int     watchoptlen=0;
	typedef void (*WATCHF)(int,int,CHAR_ACTION,int,int,int,int*,int);
	int		index;

	if( !CHECKOBJECTUSE(objindex) )return;
	index = OBJECT_getIndex( objindex);
	switch( OBJECT_getType(objindex) ){
	case OBJTYPE_CHARA:
		for( i=0; i<optlen ; i++ ) {
			watchopt[i] = opt[i];
		}
		watchoptlen = optlen;
		dir = CHAR_getInt(index,CHAR_DIR);
		break;
	case OBJTYPE_ITEM:
		for( i=0; i<optlen ; i++ ) {
			watchopt[i] = opt[i];
		}
		watchoptlen = optlen;
		dir = 0;
		break;
	case OBJTYPE_GOLD:
		watchopt[0] = index;
		watchoptlen = 1;
		dir = 0;
		break;
	default:
		return;
	}
	x = OBJECT_getX(objindex);
	y = OBJECT_getY(objindex);
	floor = OBJECT_getFloor(objindex);

	for( i=x-CHAR_DEFAULTSEESIZ/2 ; i<=x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j=y-CHAR_DEFAULTSEESIZ/2 ; j<=y+CHAR_DEFAULTSEESIZ/2 ; j ++ ){
			OBJECT object;
			for( object = MAP_getTopObj(floor,i,j) ; object ;
				 object = NEXT_OBJECT(object) )
			{
				int receiveobjindex = GET_OBJINDEX(object);
				int	recvindex = OBJECT_getIndex( receiveobjindex);
				WATCHF watchfunc=NULL;

				switch( OBJECT_getType(receiveobjindex) ){
				  case OBJTYPE_CHARA:
					/* 猷覕堣趙ぁ毀邞嚭媃埵笢啞溘精誑  埱趙笢堎鄹  崹堎 */
					if( index == recvindex && myflg == FALSE ) {
						break;
					}
					/* watch 騷穫璃哱禱厔喫埰凝汔竣凝眥勗鏍攝蚗袲埰堎 */
					if( !CHAR_sendWatchEvent_sendCheck( objindex,
														index, recvindex ,
														chac
														))
					{
						break;
					}

					watchfunc =(WATCHF)CHAR_getFunctionPointer(
						OBJECT_getIndex(receiveobjindex),
						CHAR_WATCHFUNC);
					break;
				  case OBJTYPE_ITEM:
					watchfunc = (WATCHF)ITEM_getFunctionPointer(
						OBJECT_getIndex(receiveobjindex),
						ITEM_WATCHFUNC);
					break;
				  case OBJTYPE_NOUSE:
				  case OBJTYPE_GOLD:
#ifdef _DEL_DROP_GOLD
					GOLD_DeleteTimeCheckOne( receiveobjindex);
#endif
				  default:
					break;
				}

				if( watchfunc == NULL )continue;
				watchfunc( receiveobjindex, objindex, chac, x, y, dir,
						   watchopt, watchoptlen );
			}
		}
	}
}


/*====================筒す鳴====================*/
/*------------------------------------------------------------
 * 筒す鳴囮蚗鏤ぁ拻堎源鏤痲哱昡鳴禱媃崨迋堎��
 * 礎倳
 *  charaindex      int     す籤溘騷璃溢蚗袲筒
 * 蒍堇偯
 *  埬      TRUE(1)
 *  謄  (  傷勗邞�欷汙朗�倜�桴瑀癒巏笱勞�躂堎)    FALSE(0)
 ------------------------------------------------------------*/
BOOL CHAR_Skillupsend(int charaindex )
{
#if 1
	// 筒哱↓璃操騷喇暵勗  脾媃倜忒摯
	int point;
	int fd;
	if( !CHAR_CHECKINDEX(charaindex) )return FALSE;
	// 笢�妏景昉抯�哱囮蚗鏤ぁ拻堎凝
	point = CHAR_getInt( charaindex, CHAR_SKILLUPPOINT );
	fd = getfdFromCharaIndex(charaindex);
	lssproto_SKUP_send(fd, point );
	return TRUE;
#else
	char    sendbuf[128];

	// 喫з�梫尊S2鄹摯忒摯
	if(SKILL_getUpableSkillID(charaindex,sendbuf,sizeof(sendbuf))){
		int fd;
		fd = getfdFromCharaIndex(charaindex);
		if( fd != -1 && sendbuf[0] != '\0' ){
			lssproto_SKUP_send(fd,sendbuf);
			return TRUE;
		}
	}
#endif
	return FALSE;
}

/*------------------------------------------------------------
 * 筒す鳴囮蚗鏤埰堎
 * 礎倳
 *  charaindex          int         す籤溘騷璃溢蚗袲筒
 *  skillid             int         筒す鳴ID
 * 蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
void CHAR_SkillUp(  int charaindex, int skillid )
{
	if( !CHAR_CHECKINDEX(charaindex) )return;

	/*屻庌ぁ笢倜�梪用槾聿�  */
	if( CHAR_getFlg(charaindex,CHAR_ISDIE) )return;

	{ 
		int SkUpTbl[] = { CHAR_VITAL, CHAR_STR, CHAR_TOUGH, CHAR_DEX };
		int	SendTbl[] = { CHAR_P_STRING_VITAL|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF,
						  CHAR_P_STRING_STR|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK,
						  CHAR_P_STRING_TOUGH|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF,
						  CHAR_P_STRING_DEX|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK|CHAR_P_STRING_QUICK
						};
		int cnt;
		//   ⑹鏍攝蚗袲
		if( skillid < 0 || skillid >= 4 ){
			return;
		}
		// 竘煦蚕溘隍↓淏囮蚗鏤ぁ拻堎凝ㄑ
		cnt = CHAR_getInt(charaindex,CHAR_SKILLUPPOINT);
		if( cnt <= 0 )return;

#ifdef _CHAR_PROFESSION			// WON ADD �冼攃曼耋裕�
		{
			int p_class = CHAR_getInt( charaindex, PROFESSION_CLASS );
			int fd=-1;

			switch( p_class ){
				case PROFESSION_CLASS_FIGHTER:
					// 蚋尪坶鏗200
					if( SkUpTbl[skillid] == CHAR_DEX ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "斕腔夔薯硉眒湛奻癹", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );								
							return;
						}
					}						
					break;
				case PROFESSION_CLASS_WIZARD:
					// 拵呇坶馴200
					if( SkUpTbl[skillid] == CHAR_STR ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "斕腔夔薯硉眒湛奻癹", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					// 拵呇坶滅200
					if( SkUpTbl[skillid] == CHAR_TOUGH ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "斕腔夔薯硉眒湛奻癹", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					break;
				case PROFESSION_CLASS_HUNTER:
					// 轂�佫纗�200
					if( SkUpTbl[skillid] == CHAR_STR ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "斕腔夔薯硉眒湛奻癹", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					// 轂�佫禶�200
					if( SkUpTbl[skillid] == CHAR_TOUGH ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "斕腔夔薯硉眒湛奻癹", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					// 轂�佫躞�400
					if( SkUpTbl[skillid] == CHAR_DEX ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 400 * 100 ){
							CHAR_talkToCli(charaindex, -1, "斕腔夔薯硉眒湛奻癹", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					break;
				default: break;
			}

			fd = getfdFromCharaIndex(charaindex);
			if( fd != -1 )	lssproto_SKUP_send( fd, cnt-1 );
		}
#endif

		CHAR_setInt( charaindex,CHAR_SKILLUPPOINT,cnt-1);
		CHAR_setInt( charaindex, SkUpTbl[skillid], CHAR_getInt( charaindex, SkUpTbl[skillid] ) + 1*100 );

		CHAR_complianceParameter(charaindex);
		CHAR_send_P_StatusString( charaindex, SendTbl[skillid]);

#ifdef _ANGEL_SUMMON // 樓萸奀潰脤訧跡
		if( cnt-1 == 0 )
			//selectAngel( charaindex);
			selectAngel( charaindex, -1, -1 , FALSE);

#endif
	}
	CHAR_PartyUpdate( charaindex, CHAR_N_STRING_MAXHP );

}

INLINE void CHAR_getDXDY( int dir , int* dx, int* dy )
{
	static POINT CHAR_dxdy[8]=
	{
		{ 0,-1},    /*  */
		{ 1,-1},    /*    */
		{ 1, 0},    /*  */
		{ 1, 1},    /*    */
		{ 0, 1},    /*  */
		{-1, 1},    /*    */
		{-1, 0},    /*  */
		{-1,-1},    /*    */
	};
	VALIDATEDIR(dir);

	if( dx )*dx = CHAR_dxdy[dir].x;
	if( dy )*dy = CHAR_dxdy[dir].y;
}
INLINE int CHAR_getDX( int dir )
{
	int tmp;
	CHAR_getDXDY( dir,&tmp,NULL);
	return tmp;
}
INLINE int CHAR_getDY( int dir )
{
	int tmp;
	CHAR_getDXDY( dir,NULL,&tmp);
	return tmp;
}

/*------------------------------------------------------------
 *   啋悃筒摯す籤溘袲淏↓禱  堎��
 * 礎倳
 *  objbuf      int*        object     ぁ摯 index 禱瓟  埰堎
 *                              泬蚗啞栝
 *  siz         int         objbuf 摯 �蚅昃�
 *  ff          int         啞痲囮
 *  fx          int         x
 *  fy          int         y
 * 蒍堇偯
 *  笰蠀す籤溘誑笢堎凝��
 ------------------------------------------------------------*/
int CHAR_getSameCoordinateObjects(int* objbuf, int siz,int ff, int fx,
								  int fy)
{
	OBJECT  object;
	int     findobjnum=0;

	for( object = MAP_getTopObj(ff,fx,fy) ; object ;
		 object = NEXT_OBJECT(object ) ){
		int objindex = GET_OBJINDEX(object);

		if( siz <= findobjnum )break;
		if( OBJECT_getType(objindex) != OBJTYPE_NOUSE )
			objbuf[findobjnum++] = objindex;
	}
	return findobjnum;
}


/*====================す籤溘摯攷  禱  堎捈摯匴倳====================*/
/*酘鏤で砫璃溢↓淏摯  棬  禱瞀凎埰堎匴倳*/
static char    CHAR_optiondataString[STRINGBUFSIZ];
/*------------------------------------------------------------
 * 酘鏤で砫璃暵摯  棬  禱軘堎��
 *  level, showstring , �蛩匾� ,   鴢  蠕�� 笰樊痲精騷璃媃倜凝��
 *  袲溘筒��
 * 礎倳
 *  ch      Char*       す籤溘溢↓淏
 * 蒍堇偯
 *  char*   static 埵    禱蒍埰摯ぁべ樊摯蠙怮③媃摯鄹勗毀    誑
 *    脾踏躂趙笢堎�衕羽捙姘姨曼�竘瞀凎媃埵笢痀��
 ------------------------------------------------------------*/
char* CHAR_makeOptionString( Char* ch )
{
	char    escapeshowstring[256];
	char *showstr = MAP_getfloorShowstring(ch->data[CHAR_FLOOR]);
	if( ! showstr ){
		CHAR_optiondataString[0] = '\0';
		return CHAR_optiondataString;
	} else {

#if 0  /* 雄き毀LS2蹄  ぁ堣堎 */
		snprintf( CHAR_optiondataString,
			  sizeof( CHAR_optiondataString ),
			  "%d|%s|1|%d|%d",
			  ch->data[CHAR_LV],
			  makeEscapeString( showstr ,
				  escapeshowstring,sizeof(escapeshowstring) ),
			  ch->data[CHAR_BASEIMAGENUMBER],
			  ch->data[CHAR_LOGINCOUNT]);
#endif

        /* 雄き毀StoneAge 蹄  ぁ堣堎��

           dataplace|faceimg|level|hp|str|def|dex|charm|dp|earth|water|fire|wind|login|Name|Place
           敁埵堎��
         */
        {
            snprintf( CHAR_optiondataString,
                      sizeof( CHAR_optiondataString ),
                      "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%s|%s",
                      ch->data[CHAR_DATAPLACENUMBER],
                      ch->data[CHAR_FACEIMAGENUMBER],
                      ch->data[CHAR_LV],
                      ch->workint[CHAR_WORKMAXHP],
                      ch->workint[CHAR_WORKATTACKPOWER],
                      ch->workint[CHAR_WORKDEFENCEPOWER],
                      ch->workint[CHAR_WORKQUICK],
                      ch->workint[CHAR_WORKFIXCHARM],
                      ch->data[CHAR_DUELPOINT],    /* dp, FIXME */
                      ch->workint[CHAR_WORKFIXEARTHAT],
                      ch->workint[CHAR_WORKFIXWATERAT],
                      ch->workint[CHAR_WORKFIXFIREAT],
                      ch->workint[CHAR_WORKFIXWINDAT],
                      ch->data[CHAR_LOGINCOUNT],
                      ch->string[CHAR_NAME].string,
                      makeEscapeString( showstr ,
                                        escapeshowstring,
                                        sizeof(escapeshowstring) )
                      );
        }
    }
	return CHAR_optiondataString;
}

static char CHAR_statusSendBuffer[STRINGBUFSIZ];

char* CHAR_makeStatusString( int index, char* category )
{
	char    c = tolower( category[0] );
	int     strlength=0;

	if( !CHAR_CHECKINDEX( index ) ) return NULL;

	if( category == NULL ){
		CHAR_statusSendBuffer[0] = '\0';
		return CHAR_statusSendBuffer;
	}
DebugPoint = 100;
	switch( c ){
	case 'p':
	{
		int i;
		int getCharDataArray[]= {
			CHAR_NAME,
			CHAR_OWNTITLE,
			//CHAR_FMNAME,
		};
		int maxexp = 200;
		int exp;
		int	attr[4];
		{
			exp = CHAR_getInt(index,CHAR_EXP );
			maxexp = CHAR_GetLevelExp( index, CHAR_getInt( index, CHAR_LV ) + 1);
		}

		for( i = 0; i < 4; i ++ ) {
			attr[i] = CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT + i);
			if( attr[i] < 0 ) attr[i] = 0;
		}

		snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
			"P1|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
				 CHAR_getInt(index,CHAR_HP),
				 CHAR_getWorkInt(index,CHAR_WORKMAXHP),
				 CHAR_getInt(index,CHAR_MP),
				 CHAR_getWorkInt(index,CHAR_WORKMAXMP),
				 CHAR_getInt(index,CHAR_VITAL)/100,
				 CHAR_getInt(index,CHAR_STR)/100,
				 CHAR_getInt(index,CHAR_TOUGH)/100,
				 CHAR_getInt(index,CHAR_DEX)/100,
				 exp, maxexp,
				 CHAR_getInt(index,CHAR_LV),
				 CHAR_getWorkInt(index,CHAR_WORKATTACKPOWER),
				 CHAR_getWorkInt(index,CHAR_WORKDEFENCEPOWER),
				 CHAR_getWorkInt( index, CHAR_WORKQUICK),
				 CHAR_getWorkInt( index, CHAR_WORKFIXCHARM),
				 CHAR_getWorkInt( index, CHAR_WORKFIXLUCK),
				 attr[0],
				 attr[1],
				 attr[2],
				 attr[3],
				 CHAR_getInt(index,CHAR_GOLD),
				 CHAR_getInt(index,CHAR_INDEXOFEQTITLE),
				 CHAR_getInt(index,CHAR_DUELPOINT),
				 CHAR_getInt( index, CHAR_TRANSMIGRATION),
				 CHAR_getInt(index,CHAR_RIDEPET),
				 CHAR_getInt(index,CHAR_LEARNRIDE),
				 CHAR_getInt(index,CHAR_BASEBASEIMAGENUMBER)
#ifdef _NEW_RIDEPETS
				 ,CHAR_getInt( index, CHAR_LOWRIDEPETS)
#endif

				);
		strlength = strlen( CHAR_statusSendBuffer );
		for( i = 0 ; i < arraysizeof( getCharDataArray ) ; i ++ ){
			char    token[256];
			char    escapebuffer[128];
			snprintf( token,
					  sizeof( token ),
					  "%s" STATUSSENDDELIMITER,
					  makeEscapeString( CHAR_getChar(index, getCharDataArray[i]),
					  escapebuffer,sizeof(escapebuffer)
					));
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof( CHAR_statusSendBuffer ) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= sizeof( CHAR_statusSendBuffer ) )
				return CHAR_statusSendBuffer;
		}
		return CHAR_statusSendBuffer;
		break;
	}
#ifdef _NEW_RIDEPETS
	case 'x':
		{
		snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer), "X0|%d",
				CHAR_getInt( index, CHAR_LOWRIDEPETS));
		return CHAR_statusSendBuffer;
			break;
		}
#endif
	case 'f':
	{
		int big4fm =0;
#ifndef _NEW_RIDEPETS
		switch( CHAR_getWorkInt( index, CHAR_WORKFMFLOOR) )
		{
			case 1041:
				big4fm = 1;
				break;
			case 2031:
				big4fm = 2;
				break;
			case 3031:
				big4fm = 3;
				break;
			case 4031:
				big4fm = 4;
				break;
			default:
				big4fm = 0;
		}
#endif		
#ifdef _FIX_FMNAME_RULE	 // WON ADD 模逜帤傖蕾ㄛ祥珆尨靡備			
		if( CHAR_getWorkInt(index, CHAR_WORKFMSETUPFLAG)==1 ){
			snprintf( CHAR_statusSendBuffer,
				sizeof( CHAR_statusSendBuffer ),
				"F%s|%d|%d|%d|%d",	
				"(渾机)",
				CHAR_getInt( index, CHAR_FMLEADERFLAG),
				CHAR_getWorkInt( index, CHAR_WORKFMCHANNEL),
				CHAR_getInt( index, CHAR_FMSPRITE),	big4fm );		
		}else
#endif	

		snprintf( CHAR_statusSendBuffer,
			sizeof( CHAR_statusSendBuffer ),
			"F%s|%d|%d|%d|%d",
			CHAR_getChar( index, CHAR_FMNAME),
			CHAR_getInt( index, CHAR_FMLEADERFLAG),
			CHAR_getWorkInt( index, CHAR_WORKFMCHANNEL),
			CHAR_getInt( index, CHAR_FMSPRITE),	big4fm );
		return CHAR_statusSendBuffer;
		break;			
	}
	case 'c':
	{
		int floor,xsiz,ysiz;
		floor = CHAR_getInt(index,CHAR_FLOOR);
		xsiz = MAP_getfloorX(floor);
		ysiz = MAP_getfloorY(floor);
		snprintf( CHAR_statusSendBuffer,
				  sizeof( CHAR_statusSendBuffer ),
				  "C%d|%d|%d|%d|%d",
				  CHAR_getInt(index,CHAR_FLOOR),
				  xsiz, ysiz,
				  CHAR_getInt(index,CHAR_X),
				  CHAR_getInt(index,CHAR_Y) );
		return CHAR_statusSendBuffer;
		break;
	}
	case 'i':
	{
		int     i;
		CHAR_statusSendBuffer[0] = 'I';
		strlength = 1;
		for( i = 0 ; i < CHAR_MAXITEMHAVE ; i ++ ){
			char    token[512];
			int     itemindex;
			itemindex = CHAR_getItemIndex( index, i );
			if( itemindex >= 0 ) {
				snprintf( token ,sizeof(token),"%s|" ,ITEM_makeItemStatusString( -1, itemindex ) );
			}else {
				snprintf( token ,sizeof(token),"%s|" , ITEM_makeItemFalseString() );
			}
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof(CHAR_statusSendBuffer) - strlength, token );
			strlength += strlen( token );
			if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) return CHAR_statusSendBuffer;
		}
		dchop( CHAR_statusSendBuffer , "|" );
		return CHAR_statusSendBuffer;
		break;
	}
	case 's':
	{
		int  i;
		CHAR_statusSendBuffer[0] = 'S';
		strlength = 1;

		for( i = 0 ; i < CHAR_SKILLMAXHAVE ; i ++ ){
			char    token[256];
			CHAR_HaveSkill* hskill;

			hskill = CHAR_getCharHaveSkill( index, i );

			if( hskill != NULL && hskill->use == TRUE ){
#ifdef _CHAR_PROFESSION			// WON ADD �冼攃曼�
				snprintf( token , sizeof(token), "%s|", SKILL_makeSkillStatusString(&hskill->skill, index, i) );
#else
				snprintf( token , sizeof(token), "%s|", SKILL_makeSkillStatusString(&hskill->skill) );
#endif
			}else{
				snprintf( token , sizeof(token), "%s|", SKILL_makeSkillFalseString() );
			}

			strcpysafe( CHAR_statusSendBuffer + strlength, sizeof(CHAR_statusSendBuffer)-strlength, token );

			strlength += strlen( token );
			if( strlength >= arraysizeof(CHAR_statusSendBuffer) )
				return CHAR_statusSendBuffer;
		}

		dchop( CHAR_statusSendBuffer , "|" );
		return CHAR_statusSendBuffer;
		break;
	}
	case 't':
	{
		int     i;
		CHAR_statusSendBuffer[0] = 'T';
		strlength = 1;
		for( i = 0 ; i < CHAR_TITLEMAXHAVE ; i ++ ){
			char    token[256];
			char    escape[256];
			snprintf( token , sizeof(token), "%s|" ,
					  makeEscapeString( TITLE_makeTitleStatusString(index,
																	i ),
										escape,sizeof(escape) ) );

			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof(CHAR_statusSendBuffer) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= arraysizeof(CHAR_statusSendBuffer) )
				return CHAR_statusSendBuffer;
		}
		dchop( CHAR_statusSendBuffer , "|" );
		return CHAR_statusSendBuffer;
		break;
	}
	case 'd':
		snprintf( CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				  "D%d|%ld",CHAR_getWorkInt(index,CHAR_WORKOBJINDEX),
				  NowTime.tv_sec );
		return CHAR_statusSendBuffer;
		break;
	case 'm':
		snprintf( CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				  "M%d|%d|%d",
				  CHAR_getInt( index,CHAR_HP ),
				  CHAR_getInt( index,CHAR_MP ),
				  CHAR_getInt( index,CHAR_EXP ) );
		return CHAR_statusSendBuffer;
		break;
	case 'e':
		return NULL;

	case 'n':
	{
		int num;

		int i, nindex = -1;
		int getCharDataArray[]= {
			CHAR_NAME
		};
		num = tolower( category[1] ) - '0';

		if( num < 0 || num >= CHAR_PARTYMAX ){
			print( "攬衭耀宒囮啖 (%c)%d \n", num, num );
			break;
		}

		nindex = CHAR_getPartyIndex( index, num);

		if( nindex == -1 ){
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "N%d|0|", num );
			return  CHAR_statusSendBuffer;
		}else{
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "N%d|1|%d|%d|%d|%d|%d|",
				 num,
				 CHAR_getWorkInt( nindex, CHAR_WORKOBJINDEX),
				 CHAR_getInt( nindex, CHAR_LV),
				 CHAR_getWorkInt( nindex, CHAR_WORKMAXHP),
				 CHAR_getInt( nindex, CHAR_HP),
				 CHAR_getInt( nindex, CHAR_MP)

			);
		}
		strlength = strlen( CHAR_statusSendBuffer );

		for( i = 0 ; i < arraysizeof( getCharDataArray ) ; i ++ ){
			char    token[256];
			char    escapebuffer[128];
			snprintf( token,
					  sizeof( token ),
					  "%s" STATUSSENDDELIMITER,
					  makeEscapeString( CHAR_getChar(nindex,
													 getCharDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
						  ));
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof( CHAR_statusSendBuffer ) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= sizeof( CHAR_statusSendBuffer ) )
				return CHAR_statusSendBuffer;
		}

		return CHAR_statusSendBuffer;
		break;
	}
	case 'k':
	{
		int num;
		int i, pindex;
		int getCharDataArray[]= {
			CHAR_NAME,
			CHAR_USERPETNAME,
		};
		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "唾昜耀宒囮啖 (%c)%d \n", num, num );
			break;
		}
		pindex = CHAR_getCharPet( index, num );
		if( pindex == -1 ){
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "K%d|0|", num );
			return  CHAR_statusSendBuffer;
		}else{
			int		attr[4];
			BOOL	changenameflg = FALSE;
            char    *mycdkey;
			char	*cdkey = NULL;
			for( i = 0; i < 4; i ++ ) {
				attr[i] = CHAR_getWorkInt( pindex, CHAR_WORKFIXEARTHAT + i);
				if( attr[i] < 0 ) attr[i] = 0;
			}
			mycdkey = CHAR_getChar( index, CHAR_CDKEY);
			{
				cdkey = CHAR_getChar( pindex, CHAR_CDKEY);
				if( cdkey == NULL ) {
					print( "can't get CDKEY\n");
				}else {

					if( strlen( cdkey) == 0 ||
						strcmp( cdkey, mycdkey) == 0 ){
						changenameflg = TRUE;
					}
				}
			}
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
// Syu ADD 蜊曹蝠眢奀唾昜蛌汜晇伎
#ifdef _SHOW_FUSION
				 "K%d|1|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
#else
 				 "K%d|1|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
#endif
				 num,
				 CHAR_getInt(pindex,CHAR_BASEIMAGENUMBER),
				 CHAR_getInt(pindex,CHAR_HP),
				 CHAR_getWorkInt(pindex,CHAR_WORKMAXHP),
				 CHAR_getInt(pindex,CHAR_MP),
				 CHAR_getWorkInt(pindex,CHAR_WORKMAXMP),
				 CHAR_getInt(pindex,CHAR_EXP ),
				 CHAR_GetLevelExp( pindex,CHAR_getInt( pindex, CHAR_LV ) + 1),
				 CHAR_getInt(pindex,CHAR_LV),
				 CHAR_getWorkInt(pindex,CHAR_WORKATTACKPOWER),
				 CHAR_getWorkInt(pindex,CHAR_WORKDEFENCEPOWER),
				 CHAR_getWorkInt( pindex, CHAR_WORKQUICK),
				 CHAR_getWorkInt( pindex, CHAR_WORKFIXAI),
				 attr[0],
				 attr[1],
				 attr[2],
				 attr[3],
				 CHAR_getInt( pindex, CHAR_SLOT),
// Syu ADD 蜊曹蝠眢奀唾昜蛌汜晇伎
				 changenameflg,
				 CHAR_getInt( pindex,CHAR_TRANSMIGRATION)
#ifdef _SHOW_FUSION
				,CHAR_getInt ( pindex, CHAR_FUSIONBEIT)
#endif
			);
		}
		strlength = strlen( CHAR_statusSendBuffer );
		for( i = 0 ; i < arraysizeof( getCharDataArray ) ; i ++ ){
			char    token[256];
			char    escapebuffer[128];
			snprintf( token,
					  sizeof( token ),
					  "%s" STATUSSENDDELIMITER,
					  makeEscapeString( CHAR_getChar(pindex,
													 getCharDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
						  ));
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof( CHAR_statusSendBuffer ) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= sizeof( CHAR_statusSendBuffer ) )
				return CHAR_statusSendBuffer;
		}
		return CHAR_statusSendBuffer;
		break;
	}

	case 'j':
{

		int num;
		int i, itemindex;
		int getMagicDataArray[]= {
			MAGIC_NAME,
			MAGIC_COMMENT,
		};
		num = tolower( category[1] ) - '0';

		if( num < 0 || num >= CHAR_EQUIPPLACENUM ){
			break;
		}

		itemindex = CHAR_getItemIndex( index, num );
		if( !ITEM_CHECKINDEX( itemindex) ) {
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "J%d|0|", num );
			return  CHAR_statusSendBuffer;
		}else{
			int	magicindex = MAGIC_getMagicArray( ITEM_getInt( itemindex, ITEM_MAGICID));
			if( magicindex == -1 ) {
				snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
					 "J%d|0|", num );
				return  CHAR_statusSendBuffer;
			}
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "J%d|1|%d|%d|%d|",
				 num,
				 ITEM_getInt( itemindex, ITEM_MAGICUSEMP),
				 MAGIC_getInt( magicindex, MAGIC_FIELD),
				 MAGIC_getInt( magicindex, MAGIC_TARGET));
			strlength = strlen( CHAR_statusSendBuffer );

			for( i = 0 ; i < arraysizeof( getMagicDataArray ) ; i ++ ){
				char    token[256];
				char    escapebuffer[128];
				snprintf( token,
						  sizeof( token ),
						  "%s" STATUSSENDDELIMITER,
						  makeEscapeString( MAGIC_getChar(magicindex,
														 getMagicDataArray[i]),
											escapebuffer,sizeof(escapebuffer)
							  ));
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof( CHAR_statusSendBuffer ) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= sizeof( CHAR_statusSendBuffer ) )
					return CHAR_statusSendBuffer;
			}
			return CHAR_statusSendBuffer;
		}
 }
		break;

	case 'w':
{
		int num;
		int i,petskillloop, pindex;
		char	tmp[16];
		int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "唾昜杻撮訧蹋囮啖 (%c)%d \n", num, num );
			break;
		}

		pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return NULL;
		}
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}
		for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				snprintf( token, sizeof( token),
					 "%d|%d|%d|",

					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 PETSKILL_getInt( petskillindex, PETSKILL_FIELD),
					 PETSKILL_getInt( petskillindex, PETSKILL_TARGET)
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}

#ifdef _PROFESSION_SKILL			// WON ADD �冼攃曼絳敵�
	case 'y':
{
		int num = 0, f_num = 0;
		int i,petskillloop, pindex;
		char	tmp[16];
		int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "唾昜杻撮訧蹋囮啖 (%c)%d \n", num, num );
			break;
		}

		pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return NULL;
		}
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}

		if( CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION) > 0 )
			f_num = CHAR_getWorkInt( pindex, CHAR_WORKMODOBLIVION ) + 1;// 疻咭唾撮杅講

		for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				int field = -1, target = -1;
				int oblivion = CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION );
				int p_id = PETSKILL_getInt( petskillindex, PETSKILL_ID);
				int rand_num = RAND( 0, 100 );
						
				// 笢賸疻咭
				if( (oblivion > 0) && ( f_num > 0 ) && ( rand_num <= 60 ) && ( p_id != 1 ) ){
					field = PETSKILL_FIELD_MAP;
					target = PETSKILL_TARGET_NONE;
					f_num --;
				}else{
					field = PETSKILL_getInt( petskillindex, PETSKILL_FIELD);
					target = PETSKILL_getInt( petskillindex, PETSKILL_TARGET);
				}

				snprintf( token, sizeof( token),"%d|%d|%d|",
//					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 p_id,
					 field,	target
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}
#endif

#ifdef _PETSKILL_DAMAGETOHP
    case 'o':
{
	    int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};
		int i,petskillloop, pindex=index;
        int num = 0;//, f_num = 0;
        char	tmp[16];

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "唾昜杻撮訧蹋囮啖 (%c)%d \n", num, num );
			break;
		}

        pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return NULL;
		}
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}

        for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				int field = -1, target = -1;
				//int oblivion = CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION );
				int p_id = PETSKILL_getInt( petskillindex, PETSKILL_ID);
			
				//汜韜湮黺50%й撮夔靡備�譠� 唌悛遼炷 撮夔祥夔恁
				if( ( CHAR_getInt( pindex, CHAR_HP ) > CHAR_getWorkInt( pindex, CHAR_WORKMAXHP )*0.5 
					&& strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"唌悛遼炷" ) == 0 )
#ifdef _PETSKILL_EXPLODE
					|| ( CHAR_getInt( pindex, CHAR_HP ) < (CHAR_getWorkInt( pindex, CHAR_WORKMAXHP ) >> 1 )
					&& strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"惇蹊馴僻" ) == 0 )
#endif
					){
					field = PETSKILL_FIELD_MAP;
					target = PETSKILL_TARGET_NONE;
				}else{
					field = PETSKILL_getInt( petskillindex, PETSKILL_FIELD);
					target = PETSKILL_getInt( petskillindex, PETSKILL_TARGET);
				}

				snprintf( token, sizeof( token),"%d|%d|%d|",
//					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 p_id,
					 field,	target
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}
#endif

#ifdef _PETSKILL_BECOMEFOX		
    case 'a':
{
	    int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};
		int i,petskillloop, pindex=index;
        int num = 0;//, f_num = 0;
        char	tmp[16];

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "唾昜杻撮訧蹋囮啖 (%c)%d \n", num, num );
			break;
		}
		print( "唾昜杻撮訧蹋" );
        pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return NULL;
		}
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}        
		
		for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				int field = -1, target = -1;
				//int oblivion = CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION );
				int p_id = PETSKILL_getInt( petskillindex, PETSKILL_ID);
			
				//撮夔靡備�譠� 渾儂 馴僻 滅郘 撮夔符夔恁
				print("\n撮夔靡備:%s",PETSKILL_getChar( petskillindex, PETSKILL_NAME));
				if( strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"渾儂" ) == 0 
					|| strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"馴僻" ) == 0
					|| strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"滅郘" ) == 0){
					field = PETSKILL_getInt( petskillindex, PETSKILL_FIELD);
					target = PETSKILL_getInt( petskillindex, PETSKILL_TARGET);
				}else{
					field = PETSKILL_FIELD_MAP;
					target = PETSKILL_TARGET_NONE;
				}

				snprintf( token, sizeof( token),"%d|%d|%d|",
//					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 p_id,
					 field,	target
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}
#endif

	default:
		break;
	}

	CHAR_statusSendBuffer[0] = '\0';
	return CHAR_statusSendBuffer;
}

//BOOL CHAR_sendStatusString( int charaindex, char* category )
BOOL _CHAR_sendStatusString( int charaindex, char* category, char* file, int line )
{
	char*   string;

	//print("\nsend_S:%s:%d ", file, line);

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}
	string = CHAR_makeStatusString( charaindex, category );

	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}

void CHAR_inputOwnTitle( int index ,char* name )
{
	if( !CHAR_CHECKINDEX( index ) ) return;

	if( CHAR_getFlg( index,CHAR_ISDIE ) )return;

	CHAR_setChar( index, CHAR_OWNTITLE ,name);

	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( index, CHAR_WORKOBJINDEX) );

	CHAR_send_P_StatusString( index, CHAR_P_STRING_OWNTITLE );
}


void CHAR_selectTitle( int index, int titleindex )
{
	if( !CHAR_CHECKINDEX( index ) )return;

	if( CHAR_getFlg( index,CHAR_ISDIE ) )return;

	if( titleindex == -1 ){
		CHAR_setInt( index, CHAR_INDEXOFEQTITLE, titleindex);
	}else if( titleindex < 0 || titleindex >= CHAR_TITLEMAXHAVE ) {
		return;
	}
	else{

		int title = CHAR_getCharHaveTitle( index,titleindex );

		if( TITLE_getTitleIndex( title) != -1 ){
			CHAR_setInt( index, CHAR_INDEXOFEQTITLE, titleindex);
		}
	}

	CHAR_sendStatusString( index, "T" );
	CHAR_send_P_StatusString( index, CHAR_P_STRING_TITLE );
}


void CHAR_deleteTitle( int index, int titleindex )
{
	int title;

	if( !CHAR_CHECKINDEX( index ) )return;
	/* 屻庌ぁ笢堎鄹毀煦誧 */
	if( CHAR_getFlg( index,CHAR_ISDIE ) )return;

	if( titleindex < 0 || titleindex >= CHAR_TITLEMAXHAVE )return;
	title = CHAR_getCharHaveTitle( index,titleindex );
	/*if( TITLE_CHECKTABLEINDEX( title ) == FALSE )*/
	if( TITLE_getTitleIndex( title) == -1 ) {
		return;
	}
	CHAR_setCharHaveTitle( index,titleindex,-1 );

	/*  猷覕誑窅埱趙笢倜盓僑埵�桱氿�鼠躂忒埵媃勗埰堎    */
	if( CHAR_getInt(index, CHAR_INDEXOFEQTITLE) == titleindex )
		CHAR_setInt(index, CHAR_INDEXOFEQTITLE, -1 );

	/*
	 * 筒  ↓淏筒攷  禱邞堎��
	 */
	CHAR_sendStatusString( index, "T" );
	CHAR_send_P_StatusString( index, CHAR_P_STRING_TITLE );
}

static void CHAR_initcharWorkInt( index )
{
	float hp;
	static struct substitutionTable{
		CHAR_WORKDATAINT    workindex;
		CHAR_DATAINT        intindex;
	}subtbl[]={
		{ CHAR_WORKMAXMP,		CHAR_MAXMP  },
		{ CHAR_WORKFIXFIREAT,   CHAR_FIREAT },
		{ CHAR_WORKFIXWATERAT,  CHAR_WATERAT},
		{ CHAR_WORKFIXEARTHAT,  CHAR_EARTHAT},
		{ CHAR_WORKFIXWINDAT,   CHAR_WINDAT },
		{ CHAR_WORKFIXCHARM,    CHAR_CHARM  },
		{ CHAR_WORKFIXLUCK,     CHAR_LUCK   },
		{ CHAR_WORKFIXLUCK,     CHAR_LUCK   },
		{ CHAR_WORKMODPOISON,   CHAR_POISON },
		{ CHAR_WORKMODPARALYSIS,CHAR_PARALYSIS },
		{ CHAR_WORKMODSLEEP,	CHAR_SLEEP },
		{ CHAR_WORKMODSTONE,	CHAR_STONE },
		{ CHAR_WORKMODDRUNK,	CHAR_DRUNK },
		{ CHAR_WORKMODCONFUSION, CHAR_CONFUSION }
	};
	int i;
	for( i=0 ; i<arraysizeof(subtbl) ; i++ )
		CHAR_setWorkInt(index,subtbl[i].workindex, 	CHAR_getInt(index,subtbl[i].intindex));

	CHAR_setWorkInt( index, CHAR_WORKMODWEAKEN, 0);
	CHAR_setWorkInt( index, CHAR_WORKMODDEEPPOISON, 0);
	CHAR_setWorkInt( index, CHAR_WORKMODBARRIER, 0);
	CHAR_setWorkInt( index, CHAR_WORKMODNOCAST, 0);

	for( i = 0; i < 4; i ++ ){
		int attr = CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT + i);
		if( attr > 0 ){
			CHAR_setWorkInt( index, CHAR_WORKFIXEARTHAT + (i + 2) % 4, attr * -1);
		}
	}

#ifdef _MAGIC_RESIST_EQUIT			// WON ADD 眥珛蕨俶蚾掘
	{
		// 鳶梨萇蕨俶
		//for( i=0; i<3; i++)
		//	CHAR_setWorkInt( index, CHAR_WORK_F_RESIST+i, CHAR_getInt( index, PROFESSION_FIRE_R+i ) );
	}
#endif


	CHAR_setWorkInt( index, CHAR_WORKFIXDEX,
		CHAR_getInt( index, CHAR_DEX) * 0.01 );
	CHAR_setWorkInt( index, CHAR_WORKFIXVITAL,
		CHAR_getInt( index, CHAR_VITAL) * 0.01 );

	CHAR_setWorkInt( index, CHAR_WORKFIXSTR,
		CHAR_getInt( index, CHAR_STR) * 0.01 * 1.0
//#ifdef _BATTLE_NEWPOWER
//		+ CHAR_getInt( index, CHAR_DEX) * 0.01 * 0.05
//		+ CHAR_getInt( index, CHAR_VITAL) * 0.01 * 0.05	); // 騵薯
//#else
		+ CHAR_getInt( index, CHAR_TOUGH) * 0.01 * 0.1
		+ CHAR_getInt( index, CHAR_VITAL) * 0.01 * 0.1
		+ CHAR_getInt( index, CHAR_DEX) * 0.01 * 0.05 );
//#endif
	CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH,
		CHAR_getInt( index, CHAR_TOUGH) * 0.01 * 1.0
//#ifdef _BATTLE_NEWPOWER
//		+ CHAR_getInt( index, CHAR_DEX) * 0.01 * 0.05
//		+ CHAR_getInt( index, CHAR_VITAL) * 0.01 * 0.05	);	// 薯講
//#else
		+ CHAR_getInt(index,CHAR_STR) * 0.01 * 0.1
		+ CHAR_getInt(index,CHAR_VITAL) * 0.01 * 0.1
		+ CHAR_getInt(index,CHAR_DEX) * 0.01 * 0.05 );
//#endif

#ifdef _ITEMSET5_TXT
	{
		CHAR_setWorkInt( index, CHAR_WORKFIXARRANGE, 0);
		CHAR_setWorkInt( index, CHAR_WORKFIXSEQUENCE, 0);
		CHAR_setWorkInt( index, CHAR_WORKATTACHPILE, 0);
		CHAR_setWorkInt( index, CHAR_WORKHITRIGHT, 0);
	}
#endif



#ifdef _ITEMSET6_TXT
	{
		CHAR_setWorkInt( index, CHAR_WORKNEGLECTGUARD, 0);
	}
#endif

	CHAR_setWorkInt( index, CHAR_WORKATTACKPOWER,
		CHAR_getWorkInt(index,CHAR_WORKFIXSTR) );
	CHAR_setWorkInt( index, CHAR_WORKDEFENCEPOWER,
		CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH) );
	CHAR_setWorkInt( index, CHAR_WORKQUICK,
		CHAR_getWorkInt( index, CHAR_WORKFIXDEX));

	hp = ( CHAR_getInt(index,CHAR_VITAL) * 4
		+    CHAR_getInt(index,CHAR_STR) * 1
		+    CHAR_getInt(index,CHAR_TOUGH) * 1
		+    CHAR_getInt(index,CHAR_DEX) * 1 ) * 0.01;

	 CHAR_setWorkInt( index, CHAR_WORKMAXHP, (int)hp );
	 CHAR_setWorkInt( index, CHAR_WORKMAXMP, CHAR_getInt( index, CHAR_MAXMP));

#ifdef _ADD_DEAMGEDEFC
	CHAR_setWorkInt( index, CHAR_WORKOTHERDMAGE, 0);
	CHAR_setWorkInt( index, CHAR_WORKOTHERDEFC, 0);
#endif

	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
		int		hostindex;
		CHAR_setWorkInt( index, CHAR_WORKFIXAI, 0);
		hostindex =  CHAR_getWorkInt( index, CHAR_WORKPLAYERINDEX);
		if( CHAR_CHECKINDEX( hostindex)) {
			int		ai;
			int 	modai = CHAR_getInt( index, CHAR_MODAI);
			double	R;
			if( modai <= 0 ) modai = 100;
			R = 1.10;
			ai =  ( (CHAR_getInt( hostindex, CHAR_LV)
				    * CHAR_getWorkInt( hostindex, CHAR_WORKFIXCHARM )
				    * R)
				   /
				   (CHAR_getInt( index, CHAR_LV)* modai) * 100 ) ;
			if( ai > 100 ) ai = 100;

			ai += CHAR_getInt( index, CHAR_VARIABLEAI) * 0.01;				
			if( CHAR_getInt( hostindex, CHAR_TRANSMIGRATION ) > 0 ){
				// shan   120 -> 140				
				ai += ( CHAR_GetLevel() - CHAR_getInt( hostindex, CHAR_LV ) ) / 2;
			}
			if( ai < 0 ) ai = 0;
			if( ai > 100 ) ai = 100;
			CHAR_setWorkInt( index, CHAR_WORKFIXAI, ai);
		}
	}
}

int _CHAR_complianceParameter( int index, char *FILE, int LINE)
{
	//print("\ncompPara:%s:%d", FILE, LINE);
	
	if( !CHAR_CHECKINDEX( index)) return 0;

	CHAR_initcharWorkInt( index );
	ITEM_equipEffect( index );

	CHAR_setWorkInt(index, CHAR_WORKATTACKPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXSTR ) );
	CHAR_setWorkInt(index, CHAR_WORKDEFENCEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH ) );
	CHAR_setWorkInt(index, CHAR_WORKQUICK,
		CHAR_getWorkInt( index, CHAR_WORKFIXDEX ) );

#ifdef _ITEMSET5_TXT
	CHAR_setWorkInt(index, CHAR_WORKARRANGEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXARRANGE ) );
	CHAR_setWorkInt(index, CHAR_WORKSEQUENCEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXSEQUENCE ) );
#endif

	Other_DefcharWorkInt( index);
	CHAR_setInt( index, CHAR_HP, min( CHAR_getInt(index,CHAR_HP), CHAR_getWorkInt( index, CHAR_WORKMAXHP)));
	CHAR_setInt( index, CHAR_MP, min( CHAR_getInt(index,CHAR_MP), CHAR_getWorkInt( index, CHAR_WORKMAXMP)));

	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){
		if( CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW ) != -1 ){
			int petindex = CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW );
			if( !CHAR_CHECKINDEX( petindex) ){
				CHAR_setWorkInt( index, CHAR_WORKPETFOLLOW, -1);
			}else{
				if( CHAR_getInt( petindex, CHAR_WHICHTYPE) == CHAR_TYPEPET && 
					!CHAR_CHECKINDEX( CHAR_getWorkInt( petindex, CHAR_WORKPLAYERINDEX)) ){
						CHAR_setWorkInt( index, CHAR_WORKPETFOLLOW, petindex);
						CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, index);
					}
			}
		}
		
#ifdef _PROFESSION_SKILL			// WON ADD �冼攃曼絳敵�		
		{
			int i, add_pile = 0;
			int old_pile = CHAR_getWorkInt( index, CHAR_WORKATTACHPILE);
			
			for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
				int skillID = CHAR_getIntPSkill( index, i, SKILL_IDENTITY );
				
				if( skillID <= 0 ) continue;
				
				// 蛹笭崝樓
				if( skillID == 65 ){
					int p_class = CHAR_getInt( index, PROFESSION_CLASS );

					if( p_class == PROFESSION_CLASS_FIGHTER )	add_pile = 2;
					if( p_class == PROFESSION_CLASS_WIZARD )	add_pile = 2;
					if( p_class == PROFESSION_CLASS_HUNTER )	add_pile = 6;

					CHAR_setWorkInt( index, CHAR_WORKATTACHPILE, old_pile + add_pile );

				}
			}
		}
#endif
	}

	if( CHAR_getFlg(index,CHAR_ISDIE) == FALSE ){
		int oldimagenumber = CHAR_getInt(index,CHAR_BASEIMAGENUMBER);
		int itemindex = CHAR_getItemIndex(index,CHAR_ARM);
		int category;
		int newimagenumber;
		int basebaseimagenumber = CHAR_getInt(index,CHAR_BASEBASEIMAGENUMBER);
		if( !ITEM_CHECKINDEX(itemindex) )
			category = ITEM_FIST;
		else
			category = ITEM_getInt(itemindex,ITEM_TYPE );

		newimagenumber = CHAR_getNewImagenumberFromEquip( basebaseimagenumber, category);
#ifdef _ITEM_METAMO
		if( CHAR_getWorkInt( index, CHAR_WORKITEMMETAMO) > NowTime.tv_sec 
#ifdef _NPCCHANGE_PLAYERIMG
			|| CHAR_getWorkInt( index, CHAR_WORKNPCMETAMO) > 0
#endif
#ifdef _PETSKILL_BECOMEPIG
			|| CHAR_getInt( index, CHAR_BECOMEPIG) > -1
#endif
			){
			newimagenumber = oldimagenumber; //祥閥葩傖埻掛腔芞
		}
#endif
		// Arminius 8.9 airplane
		if (oldimagenumber==100259) newimagenumber=100259;
#ifdef _VARY_WOLF		
		if( (CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPET) &&
			(CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) && 			
			oldimagenumber == 101428 ){
			newimagenumber = 101428;
		}
#endif

#ifdef _ENEMY_FALLGROUND
		if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEENEMY )	{
			return 0;
		}else	{
#endif
			if( newimagenumber == -1 )	{
				CHAR_setInt(index,CHAR_BASEIMAGENUMBER, basebaseimagenumber);
			}else	{
				CHAR_setInt(index,CHAR_BASEIMAGENUMBER,newimagenumber);				
			}

#ifdef _ENEMY_FALLGROUND
		}
#endif
#ifdef _NEW_RIDEPETS
		if( (CHAR_getWorkInt(index, CHAR_WORKITEMMETAMO)>NowTime.tv_sec)
			|| (CHAR_getWorkInt(index,CHAR_WORKNPCMETAMO)>0)){
			return 0;
		}
#endif
		// Robin 0725
		if( CHAR_getInt(index, CHAR_RIDEPET) != -1 ){
			int i;
#ifndef _NEW_RIDEPETS
			int big4fm =0;
#endif
#ifdef _NEW_RIDEPETS
			BOOL FindGraNo = FALSE;
#endif
			int petindex = CHAR_getCharPet( index,  CHAR_getInt(index, CHAR_RIDEPET) );
#ifndef _NEW_RIDEPETS
			int leaderGraNo = 100700
				+ ((CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER)-100000)/20)*10
				+ CHAR_getInt( index, CHAR_FMSPRITE)*5;
			switch( CHAR_getWorkInt( index, CHAR_WORKFMFLOOR) ){

				case 1041:
					big4fm = 1;
					break;
				case 2031:
					big4fm = 2;
					break;
				case 3031:
					big4fm = 3;
					break;
				case 4031:
					big4fm = 4;
					break;
				default:
					big4fm = 0;
			}
#endif
			for( i=0; i< arraysizeof(ridePetTable) ; i++ )	{
#ifdef _NEW_RIDEPETS
				if( (( CHAR_getInt( index, CHAR_BASEIMAGENUMBER) == ridePetTable[i].charNo ) ||
					( CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].charNo ))
#else
				if( ( CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].charNo )
#endif
					&& ( CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].petNo ) )	{
					CHAR_setInt( index, CHAR_BASEIMAGENUMBER, ridePetTable[i].rideNo );
#ifdef _NEW_RIDEPETS
					FindGraNo = TRUE;
#endif
					break;
				}
#ifndef _NEW_RIDEPETS
				else if( ( leaderGraNo == ridePetTable[i].charNo )
					&& ( CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].petNo )
#ifdef _EVERYONE_RIDE
					&& big4fm != 0 ){
#else
					&& CHAR_getInt( index, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER ){
#endif					
					CHAR_setInt( index, CHAR_BASEIMAGENUMBER, ridePetTable[i].rideNo );
					break;
				}
#endif
			}

#ifdef _NEW_RIDEPETS
			{
				int ti=-1, Noindex, image=-1;
				int petNo = CHAR_getInt( petindex, CHAR_BASEBASEIMAGENUMBER);
				int playerNo = CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER);

				int playerlowsride = CHAR_getInt( index, CHAR_LOWRIDEPETS);
				if( (ti = RIDEPET_getPETindex( petNo, playerlowsride )) >= 0 )	{
					if( (Noindex = RIDEPET_getNOindex( playerNo)) >= 0 ){
						if( (image = RIDEPET_getRIDEno( Noindex,ti)) >= 0 )	{
							FindGraNo = TRUE;
							CHAR_setInt( index, CHAR_BASEIMAGENUMBER, image );
						}
					}
				}
			}
			if( FindGraNo == FALSE ){
				CHAR_setInt( index , CHAR_RIDEPET, -1 );
				CHAR_setInt( index , CHAR_BASEIMAGENUMBER , CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER) );
				CHAR_sendCToArroundCharacter( CHAR_getWorkInt( index, CHAR_WORKOBJINDEX ));
				CHAR_send_P_StatusString( index, CHAR_P_STRING_RIDEPET );
			}
#endif
		}
	}
	return 1;
}

/*======================================================================
  す籤溘摯囮騷  堜禱翩竣匴倳
  ======================================================================*/

/*====================峟堎====================*/
/*------------------------------------------------------------
 *   澺炩擭ぁ��鼠摯  澺摯す籤溘袲淏勗峟堎騷穫璃哱禱  牁踏鞠堎
 * 礎倳
 *  charaindex      int     す籤溘騷璃溢蚗袲筒
 *  dir             int       澺
 * 蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
void CHAR_Look( int charaindex, int dir )
{
	int fl ,x,y;
	OBJECT  object;

	/*  騷璃溢蚗袲筒摯鏍攝蚗袲  */
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;

	if( dir < 0 ) {
		dir = CHAR_getInt(charaindex,CHAR_DIR);
	}
	VALIDATEDIR(dir);
	if( CHAR_getInt( charaindex, CHAR_DIR) != dir ) {
		CHAR_setInt(charaindex,CHAR_DIR,dir);
		/*    澺  儒媃倜囮袲で砫璃禱邞嚭埰堎  */
		CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX),
							CHAR_ACTTURN,NULL,0,TRUE);
	}

	fl = CHAR_getInt(charaindex,CHAR_FLOOR);
	CHAR_getCoordinationDir( CHAR_getInt(charaindex,CHAR_DIR),
							 CHAR_getInt(charaindex,CHAR_X),
							 CHAR_getInt(charaindex,CHAR_Y),
							 1, &x, &y );

	//CHAR_talkToCli( charaindex, -1, "覃脤侐笚遠噫﹝", CHAR_COLORWHITE );

	for( object=MAP_getTopObj(fl,x,y) ; object ;
		 object = NEXT_OBJECT(object) ){
		int objindex = GET_OBJINDEX(object);
		if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
			typedef void (*LOOKF)(int,int);
				LOOKF lf;
				lf = (LOOKF)CHAR_getFunctionPointer(
					OBJECT_getIndex(objindex),CHAR_LOOKEDFUNC);
				if( lf ) lf(OBJECT_getIndex(objindex),charaindex);
		}
	}
}

BOOL _CHAR_makeObjectCString( char *file, int line, int objindex,char* buf, int buflen )
{
	char    objindexbuf[64];
	if( CHECKOBJECT(objindex) == FALSE )    return FALSE;

	switch( OBJECT_getType(objindex) ){
	case OBJTYPE_NOUSE:
		return FALSE;
		break;
	case OBJTYPE_CHARA:
	{
		int     charaindex = OBJECT_getIndex(objindex);
		char    escapename[256];
		char    escapetitle[256];
		// shan add +1
		char    tmp[128];
		int namecolor;
#ifdef _TRANS_6
#ifdef _TRANS_7
		int namecolortbl[] = { CHAR_COLORWHITE, CHAR_COLORYELLOW, CHAR_COLORGREEN,
					CHAR_COLORCYAN, CHAR_COLORRED, CHAR_COLORPURPLE, CHAR_COLORBLUE2,CHAR_COLORGRAY};//蛌汜摽腔晇伎
#else
		int namecolortbl[] = { CHAR_COLORWHITE, CHAR_COLORYELLOW, CHAR_COLORGREEN,
					CHAR_COLORCYAN, CHAR_COLORRED, CHAR_COLORPURPLE, CHAR_COLORBLUE2};//蛌汜摽腔晇伎
#endif
#else
		int namecolortbl[] = { CHAR_COLORWHITE, CHAR_COLORYELLOW, CHAR_COLORGREEN,
					CHAR_COLORCYAN, CHAR_COLORRED, CHAR_COLORPURPLE};
#endif
		int ridepet = BATTLE_getRidePet( charaindex );
		char petname[64] = "";
		char escapepetname[256];
		int petlevel = 0;

		if( !CHAR_getFlg(charaindex,CHAR_ISVISIBLE) )return FALSE;
		namecolor = CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
#ifdef _TRANS_6
#ifdef _TRANS_7
		if( namecolor > 7 ) namecolor = 7;
#else
		if( namecolor > 6 ) namecolor = 6;
#endif
#else
		if( namecolor > 5 ) namecolor = 5;
#endif
		if( namecolor < 0 ) namecolor = 0;
		
		// shan add begin
		if( CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI) >= 0 
			&& CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI) < FAMILY_MAXNUM
			&& CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER		
			&& CHAR_getWorkInt(charaindex, CHAR_WORKFMSETUPFLAG)!=1)
#ifdef _ITEM_SETLOVER
		if(strlen( CHAR_getChar( charaindex, CHAR_LOVE)) > 0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERID))>0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERNAME))>0)
        	sprintf(tmp, "%s∫﹥%s﹦",CHAR_getChar( charaindex, CHAR_FMNAME),CHAR_getChar( charaindex, CHAR_LOVERNAME));
    else
    			sprintf(tmp, "%s",CHAR_getChar( charaindex, CHAR_FMNAME));
#else
   		    sprintf(tmp, "%s",CHAR_getChar( charaindex, CHAR_FMNAME));
#endif
		else
#ifdef _ITEM_SETLOVER
		if(strlen( CHAR_getChar( charaindex, CHAR_LOVE)) > 0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERID))>0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERNAME))>0)
        	sprintf(tmp, "﹥%s﹦",CHAR_getChar( charaindex, CHAR_LOVERNAME));
    else
    			strcpy(tmp, "");
#else
   		    strcpy(tmp, "");
#endif

   		// Robin 0730
   		if( ridepet >= 0 ){
   			if( ! strcmp( CHAR_getChar( ridepet, CHAR_USERPETNAME), "") )
	   			strcpy( petname, CHAR_getChar( ridepet, CHAR_NAME ));
	   		else
	   			strcpy( petname, CHAR_getChar( ridepet, CHAR_USERPETNAME ));
	   		petlevel = CHAR_getInt( ridepet, CHAR_LV);
	   	}
#ifdef _OBJSEND_C
  #ifdef _CHAR_PROFESSION			// WON ADD �冼攃曼�
    #ifdef _ALLDOMAN // (祥褫羲) Syu ADD 齬俴埤NPC
		  snprintf( buf, buflen, "%d|%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d|%d|%d|%d|%d",
    #else
		  snprintf( buf, buflen, "%d|%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d|%d|%d|%d",
    #endif
  #else
		snprintf( buf, buflen, "%d|%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d",
  #endif
			OBJTYPE_CHARA,
#else
  #ifdef _GM_IDENTIFY
    snprintf( buf, buflen, "%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d|%s", //嗣堤賸gm靡備涴跺戲弇
  #else
		snprintf( buf, buflen, "%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d",
  #endif
#endif
				  CHAR_getInt( charaindex, CHAR_WHICHTYPE), 
				  cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
				  OBJECT_getX(objindex), 
				  OBJECT_getY(objindex),
				  CHAR_getInt( charaindex, CHAR_DIR),
				  CHAR_getInt( charaindex,CHAR_BASEIMAGENUMBER ),
				  CHAR_getInt( charaindex,CHAR_LV ),
				  namecolortbl[namecolor],
				  makeEscapeString( CHAR_getChar( charaindex,CHAR_NAME ), escapename, sizeof(escapename)),
				  makeEscapeString(CHAR_getChar(charaindex,CHAR_OWNTITLE), escapetitle,sizeof(escapetitle)),
				  CHAR_getFlg( charaindex,CHAR_ISOVERED ),
				  CHAR_getFlg( charaindex,CHAR_HAVEHEIGHT ),
				  CHAR_getInt( charaindex, CHAR_POPUPNAMECOLOR),
				  tmp,
				  makeEscapeString( petname, escapepetname, sizeof(escapepetname)),
				  petlevel
#ifdef _CHAR_PROFESSION			// WON ADD �冼攃曼�
				  ,CHAR_getInt( charaindex, PROFESSION_CLASS)	// 眥珛梗
				  ,CHAR_getInt( charaindex, PROFESSION_LEVEL)	// 眥珛脹撰
//				  ,CHAR_getInt( charaindex, PROFESSION_EXP)		// 眥珛冪桄硉
				  ,CHAR_getInt( charaindex, PROFESSION_SKILL_POINT)	// 撮夔萸杅
#endif
#ifdef _ALLDOMAN // (祥褫羲) Syu ADD 齬俴埤NPC
				  ,CHAR_getInt( charaindex, CHAR_HEROFLOOR )	// 荎倯桵部瞼脯
#endif
#ifdef _GM_IDENTIFY
                  ,CHAR_getChar(charaindex, CHAR_GMIDENTIFY) //gm腔靡備
#endif
#ifdef _PETSKILL_BECOMEPIG
    //              ,CHAR_getInt(charaindex, CHAR_BECOMEPIG)
#endif
			);
		break;
	}
	case OBJTYPE_ITEM:
	{
		int     itemindex = OBJECT_getIndex(objindex);
		int     nameint;
		int     leaklevel;
		int		color;
		char bufbuf[256];
		char *temp;
		leaklevel = ITEM_getInt( itemindex,ITEM_LEAKLEVEL );

		nameint = ITEM_SECRETNAME;
//		if( leaklevel >=1 ) {
//			nameint = ITEM_SECRETNAME;
//		}else {
//			nameint = ITEM_NAME;
//		}
		color = CHAR_COLORWHITE;
		if (ITEM_getChar( itemindex, ITEM_CDKEY) == NULL){
			return FALSE;
			break;
		}
		temp=ITEM_getChar( itemindex, ITEM_CDKEY);
		if( temp && strlen( temp ) ) {
			color = CHAR_COLORGREEN;
		}else {
			if( ITEM_getInt( itemindex, ITEM_MERGEFLG) ) {
				color = CHAR_COLORYELLOW;
			}
		}
#ifdef _OBJSEND_C
		snprintf( buf, buflen, "%d|%s|%d|%d|%d|%d|%s",
					OBJTYPE_ITEM,
#else
		snprintf( buf, buflen, "%s|%d|%d|%d|%d|%s",
#endif
				cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
				OBJECT_getX(objindex), OBJECT_getY(objindex),
				ITEM_getInt( itemindex,ITEM_BASEIMAGENUMBER ),
				color,
				makeEscapeString( ITEM_getChar( itemindex,nameint), bufbuf, sizeof(bufbuf))
		);
		break;
	}
	case OBJTYPE_GOLD:
#ifdef _OBJSEND_C
		snprintf( buf, buflen, "%d|%s|%d|%d|%d",
					OBJTYPE_GOLD,
#else
		snprintf( buf, buflen, "%s|%d|%d|%d",
#endif
				cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
				OBJECT_getX(objindex),OBJECT_getY(objindex),
				OBJECT_getIndex(objindex) );
		break;
#ifdef _OBJSEND_C
#ifdef _NPCSERVER_NEW
	case OBJTYPE_NPCSCHARA:
		{
			char bufbuf[256];
			snprintf( buf, buflen, "%d|%s|%s|%d|%d|%d|%d",
						OBJTYPE_NPCSCHARA,
						cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
						makeEscapeString( OBJECT_getName( objindex), bufbuf, sizeof(bufbuf)),
						OBJECT_getNpcdir( objindex),
						OBJECT_getNpcImage( objindex),
						OBJECT_getX(objindex),
						OBJECT_getY(objindex));
		}
		break;
#endif
#endif
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

void CHAR_sendCSpecifiedObjindex( int fd, int index)
{
	char    introduction[512];	
	if( CHECKOBJECTUSE( index )
		&& CHAR_makeObjectCString( index, introduction,
								   sizeof(introduction) ) ){
		lssproto_C_send( fd,introduction );
	}else{
		char    mesg[32];
		cnv10to62( index, mesg, sizeof( mesg));
		lssproto_C_send( fd, mesg );
	}
}

void CHAR_sendSpecifiedobjindexCToCharaindex(int charaindex,int objindex)
{
	int fd;
	fd = getfdFromCharaIndex(charaindex);
	if( fd == -1)return;
	CHAR_sendCSpecifiedObjindex(fd,objindex);
}

void CHAR_sendCToArroundCharacter( int objindex )
{
	char    introduction[512];
	int     x,y,fl;
	int     i,j;

	if( CHAR_makeObjectCString( objindex,introduction, sizeof(introduction) ) == FALSE ) return;

	x = OBJECT_getX(objindex);
	y = OBJECT_getY(objindex);
	fl =OBJECT_getFloor(objindex);

	for( i = x - CHAR_DEFAULTSEESIZ/2 ;
		 i <= x +CHAR_DEFAULTSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_DEFAULTSEESIZ/2;
			 j <= y + CHAR_DEFAULTSEESIZ/2; j ++ ){
			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j);
				 object;
				 object = NEXT_OBJECT(object) ){
				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex(objindexwk);
					if( CHAR_getInt( index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
						int fd;
						if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) continue;
						fd = getfdFromCharaIndex( index);
						if( fd == -1 )continue;
						lssproto_C_send( fd,introduction );
					}
				}
			}
		}
	}
}

void CHAR_sendCDArroundChar_Main( int fl, int x, int y, int objindex, BOOL mode )
{
#define CHAR_CDSEESIZ CHAR_DEFAULTSEESIZ+10

	int     i,j;
	char    cdmsg[64*2];
	cnv10to62( objindex,cdmsg, sizeof(cdmsg));
	for( i = x - CHAR_CDSEESIZ/2 ; i <= x +CHAR_CDSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_CDSEESIZ/2; j <= y + CHAR_CDSEESIZ/2; j ++ ){
			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j); object;
				 object = NEXT_OBJECT(object) ){

				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex( objindexwk);
					if( CHAR_getInt( index,CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER) {
						if( objindexwk != objindex ) {
							int fd;
							fd = getfdFromCharaIndex(index);
							if( fd == -1 )continue;
							if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE)
								== BATTLE_CHARMODE_NONE )
							{
								CONNECT_appendCDbuf( fd, cdmsg, strlen( cdmsg));
								CDflush( index);
							}
						}
					}
				}
			}
		}
	}
}

void CHAR_sendCDArroundChar( int fl, int x, int y, int objindex )
{
	CHAR_sendCDArroundChar_Main( fl,x,y,objindex,FALSE);
}

void CHAR_sendArroundCharaData( int charaindex )
{
	int fd;
	int     i,j,x, y,fl;
	char    introduction[512*2];
	char    c_msg[1024*96];
	int     strpos = 0;
	char	cabuf[128*2];
	fd = getfdFromCharaIndex( charaindex );
	if( fd == -1 )return;
	c_msg[0] = '\0';
	fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	x = CHAR_getInt( charaindex, CHAR_X);
	y = CHAR_getInt( charaindex, CHAR_Y);
							
	for( i = x-CHAR_DEFAULTSEESIZ/2 ; i <= x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j = y-CHAR_DEFAULTSEESIZ/2 ; j <= y+CHAR_DEFAULTSEESIZ/2 ;	 j ++ )	{
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ; object ; object = NEXT_OBJECT(object ) ){
				int introlen;
				int objindex = GET_OBJINDEX(object);
				int c_index = OBJECT_getIndex( objindex);

				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA &&
					!CHAR_getFlg(OBJECT_getIndex(objindex),	CHAR_ISVISIBLE) ){
					continue;
				}
				if( OBJECT_getType( objindex) == OBJTYPE_CHARA) {
					if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
			 	   		/* 峟窇倜す籤溘誑黥蹬倵拻摯虮埵�桱屻齱囃腹�  崚CA禱邞堎 */
						if( CHAR_getWorkInt( c_index, CHAR_WORKPARTYMODE )== CHAR_PARTY_LEADER ){
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),	CHAR_ACTLEADER,1 ))	{
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
						// Robin 0611 trade
						/* 褒伎蝠眢笢 */
						if( CHAR_getWorkInt( c_index, CHAR_WORKTRADEMODE ) != CHAR_TRADE_FREE )	{
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),	CHAR_ACTTRADE,1 )){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}

#ifdef _ANGEL_SUMMON
						if( CHAR_getWorkInt( c_index, CHAR_WORKANGELMODE ) == TRUE )	{
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),	CHAR_ACTANGEL,1 )){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
#endif

#ifdef _MIND_ICON
						//print("\nshan--->(batlmode)->%d", CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE ));
						if(CHAR_getWorkInt( c_index, CHAR_MIND_NUM) &&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	== BATTLE_CHARMODE_NONE){
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
														CHAR_MIND, CHAR_getWorkInt( c_index, CHAR_MIND_NUM))){
								CONNECT_appendCAbuf( fd, cabuf, strlen(cabuf));
								//print("\nshan---->(1)cabuf-->%s", cabuf);
							}
						}
#endif

#ifdef _STREET_VENDOR
						if(CHAR_getWorkInt(c_index,CHAR_WORKSTREETVENDOR) == 1 && 
							 CHAR_getWorkInt(c_index,CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
							if(CHAR_makeCAOPTString(objindex,cabuf,sizeof(cabuf),
								 CHAR_STREETVENDOR_OPEN,CHAR_getWorkChar(c_index,CHAR_STREETVENDOR_NAME))){
								CONNECT_appendCAbuf(fd,cabuf,strlen(cabuf));
							}
						}
#endif

#ifdef _ITEM_CRACKER
						if(CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER)&&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	== BATTLE_CHARMODE_NONE)
						{
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
														CHAR_ITEM_CRACKER, CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER)))
							{
								CONNECT_appendCAbuf( fd, cabuf, strlen(cabuf));
							}
						}
#endif						
						if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	!= BATTLE_CHARMODE_NONE ){		
							if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEWATCH) == TRUE ) {
								if( CHAR_makeCAOPT1String( objindex, cabuf,	sizeof( cabuf),	CHAR_ACTBATTLEWATCH, 1))	{
									CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
								}
							}else {
								int	battleno = CHAR_getWorkInt( c_index, CHAR_WORKBATTLEINDEX);
								int	sideno   = CHAR_getWorkInt( c_index, CHAR_WORKBATTLESIDE);
								int	helpno = FALSE;
								if( BATTLE_CHECKINDEX( battleno ) == FALSE ){
								}else	{
									helpno = (BattleArray[ battleno].Side[
										CHAR_getWorkInt( c_index, CHAR_WORKBATTLESIDE)].flg & BSIDE_FLG_HELP_OK)?TRUE:FALSE;
								}
								if( CHAR_makeCAOPT3String( objindex, cabuf,	sizeof( cabuf),	CHAR_ACTBATTLE,
															battleno,sideno,helpno)){
									CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));

								}
							}
						}
						if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 )	{
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));

							}
						}
					}else if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPET )	{
						if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 ){
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
					}
#ifdef _ADD_ACTION
					else {	//npc
			
						if( CHAR_CHECKINDEX( c_index) )	{
			 	   			if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) > 0 ) {
								if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
												CHAR_getWorkInt( c_index, CHAR_WORKACTION))) {
									CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));

								}
							}
						}
					}
#endif
				}
				if( c_index == charaindex) continue;
				if( CHAR_makeObjectCString( objindex, introduction,	sizeof( introduction))){
					introlen = strlen( introduction);
					introduction[introlen] = ',';
					introduction[introlen+1] = '\0';

					strcpysafe( &c_msg[strpos], sizeof( c_msg) - strpos,
								introduction);
					strpos += strlen( introduction);
				}
			}
		}
	}
	dchop( c_msg, "," );
	if( strlen( c_msg) == 0 ) return;
	lssproto_C_send( fd, c_msg);
}

BOOL _CHAR_warpToSpecificPoint( char *file, int line, int charaindex, int floor, int x, int y)
{
	int     objindex;
	int     per;
	objindex = CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX);
	if( !MAP_IsValidCoordinate( floor, x, y )) {
		print( "error: invalid Coordinate fl[%d] x[%d] y[%d] %s:%d from %s:%d\n", 
					floor, x, y, __FILE__, __LINE__, file, line);
		return FALSE;
	}
	CHAR_sendCDArroundChar_Main( OBJECT_getFloor(objindex),
							OBJECT_getX(objindex),
							OBJECT_getY(objindex),
							objindex, TRUE );

	CHAR_setInt( charaindex,CHAR_FLOOR,floor);
	CHAR_setInt( charaindex,CHAR_X,x);
	CHAR_setInt( charaindex,CHAR_Y,y);
	{
		int of,ox,oy;
		of = OBJECT_setFloor(objindex,floor);
		ox = OBJECT_setX(objindex,x);
		oy = OBJECT_setY(objindex,y);
		if( !MAP_objmove( objindex,of,ox,oy,floor,x,y ) ){
			fprint( "ERROR MAP_OBJMOVE objindex=%d\n",objindex );
		}
	}
	per = ENCOUNT_getEncountPercentMin( charaindex, floor,x,y);
	if( per != -1 ) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, per);
	}
	per = ENCOUNT_getEncountPercentMax( charaindex, floor,x,y);
	if( per != -1 ) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, per);
	}

#ifdef _PROFESSION_SKILL			// WON ADD �冼攃曼絳敵�
	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_FIX, 0);
#endif

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		CAflush( charaindex );
		{
			int i;
			char*   statuscat[]={"C","E",};
			for( i=0 ; i<arraysizeof(statuscat) ; i++ )
				CHAR_sendStatusString( charaindex, statuscat[i] );

			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
				CHAR_sendStatusString( charaindex, "P" );

			}
		}
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
			CHAR_sendArroundCharaData( charaindex );
		}
	}

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPET ||
		( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
		  CHAR_getInt( charaindex, CHAR_MAILMODE) == CHAR_PETMAIL_NONE)){
		CHAR_sendCToArroundCharacter( objindex );
	}
	else {
		CHAR_sendPMEToArroundCharacterFLXY( charaindex,
										CHAR_getInt( charaindex, CHAR_FLOOR),
										CHAR_getInt( charaindex, CHAR_X),
										CHAR_getInt( charaindex, CHAR_Y),
										0,1,CHAR_getInt( charaindex, CHAR_PETMAILEFFECT)
										 );
	}

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ) {
			CHAR_sendLeader( objindex, 1);
		}
		MAP_sendArroundChar( charaindex );
		CHAR_setWorkChar(charaindex,CHAR_WORKWALKARRAY,"");
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) != CHAR_PARTY_CLIENT ){
			CHAR_setFlg( charaindex, CHAR_ISWARP, 1);
		}
		CHAR_checkEffect( charaindex);
		// Robin 0706 petFollow
		{
			int petindex = CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW );
			if( (petindex > -1) && (CHAR_CHECKINDEX( petindex )) )
          
			CHAR_warpToSpecificPoint( petindex, floor, x, y );
		}
#ifdef _STATUS_WATERWORD //阨岍賜袨怓
		if( MAP_getMapFloorType( floor) == 1 ){
			CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 1);
		}else {
			CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 0);
		}
#endif
#ifdef _MAP_TIME
		// �蝜�輛�賮警媏暱м瑱店鉭霾躉�,扢隅給杅
		if(floor >= 30017 && floor <= 30021){
			if(CHAR_getWorkInt(charaindex,CHAR_WORK_MAP_TIME) <= 0) CHAR_setWorkInt(charaindex,CHAR_WORK_MAP_TIME,60);
		}
		// �蝜�輛�鄳�都腔華芞
		else CHAR_setWorkInt(charaindex,CHAR_WORK_MAP_TIME,0);
#endif

#ifdef _ANGEL_SUMMON
		if( CHAR_getWorkInt( charaindex, CHAR_WORKANGELMODE ) == TRUE ) {
			CHAR_sendAngelMark( objindex, 1);
		}
#endif

	}else if( OBJECT_getType( objindex) == OBJTYPE_CHARA ){
		MAP_sendArroundChar( charaindex );
	}
	
	return TRUE;
}

static void CHAR_walk_check( int charaindex )
{
	if( strlen( CHAR_getWorkChar( charaindex ,CHAR_WORKWALKARRAY ) ) != 0 ){
		struct timeval old;
		old.tv_sec  = CHAR_getWorkInt(charaindex,CHAR_WORKWALKSTARTSEC);
		old.tv_usec = CHAR_getWorkInt(charaindex,CHAR_WORKWALKSTARTMSEC);
		if( time_diff_us(NowTime,old) >= CHAR_getInt(charaindex,CHAR_WALKINTERVAL)*1000.0 )	{
			CHAR_walkcall( charaindex );
		}else	{
		}
	}
}

static BOOL CHAR_callLoop( int charaindex )
{
	unsigned int timediff_us;
	struct timeval old;
	int     loopinterval, iRet = FALSE;
	loopinterval = CHAR_getInt(charaindex,CHAR_LOOPINTERVAL);
	if( loopinterval <= 0 )return FALSE;

	//print("\n CHAR_callLoop:%d", charaindex);

	old.tv_sec  = CHAR_getWorkInt(charaindex,CHAR_WORKLOOPSTARTSEC);
	old.tv_usec = CHAR_getWorkInt(charaindex,CHAR_WORKLOOPSTARTMSEC);
	timediff_us = time_diff_us( NowTime , old );

	if( timediff_us >= loopinterval*1000.0 ){
		int (*loopfunc)(int)=NULL;
		loopfunc =(int(*)(int))CHAR_getFunctionPointer( charaindex, CHAR_LOOPFUNC);
		if( loopfunc ){
			loopfunc(charaindex);
			iRet = TRUE;
		}
#ifdef _USER_CHARLOOPS
		loopfunc =(int(*)(int))CHAR_getFunctionPointer( charaindex, CHAR_LOOPFUNCTEMP1);
		if( loopfunc ){
			loopfunc(charaindex);
			iRet = TRUE;
		}
#endif
		if( iRet == FALSE ){
			CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 0);
		}

		CHAR_setWorkInt(charaindex,CHAR_WORKLOOPSTARTSEC, NowTime.tv_sec);
		CHAR_setWorkInt(charaindex,CHAR_WORKLOOPSTARTMSEC, NowTime.tv_usec);
	}
	return iRet;
}


int EnemyMoveNum = 10;
void CHAR_Loop( void )
{
	int charnum=CHAR_getCharNum();
	int playernum = CHAR_getPlayerMaxNum();
	int petnum = CHAR_getPetMaxNum();
	static int  charcnt = 0;
	int i, movecnt=0;
	

	if( charcnt == -1 ) {
		charcnt = CHAR_getCharNum();
	}

	// 俙模蚚腔loop
	for( i=0 ; i< playernum ; i++ ){
		if( CHAR_getCharUse(i) == FALSE )continue;
		CHAR_walk_check( i );
		CHAR_callLoop( i );
#ifdef _TEACHER_SYSTEM
		// 衄鳳腕絳呇汒咡
		if(CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME) > 0){
			int iAddTFame = CHAR_getInt(i,CHAR_TEACHER_FAME) + CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME);
			int iAddFame = CHAR_getInt(i,CHAR_FAME) + CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME);

			// 樓絳呇汒咡(絳呇汒咡郔湮硉睿跺�侂驫�郔湮硉珨欴)
			if(iAddTFame > MAX_PERSONALFAME) iAddTFame = MAX_PERSONALFAME;
			else if(iAddTFame < 0) iAddTFame = 0;
			CHAR_setInt(i,CHAR_TEACHER_FAME,iAddTFame);
			// 樓汒咡
			if(iAddFame > MAX_PERSONALFAME) iAddFame = MAX_PERSONALFAME;
			else if(iAddFame < 0) iAddFame = 0;
			CHAR_setInt(i,CHAR_FAME,iAddFame);
			// ь峈 0
			CHAR_setWorkInt(i,CHAR_WORK_GET_TEACHER_FAME,0);
			// �譥邾眢�,奻換跤ac悵厥媼晚訧蹋肮祭
			if(CHAR_getInt(i,CHAR_FMLEADERFLAG) > 0 && CHAR_getInt(i,CHAR_FMLEADERFLAG) != FMMEMBER_APPLY){
				char tmpbuf1[16];

				sprintf(tmpbuf1,"%d",iAddFame);
				saacproto_ACFixFMData_send(acfd,
					CHAR_getChar(i,CHAR_FMNAME),
					CHAR_getInt(i,CHAR_FMINDEX),
					CHAR_getWorkInt(i,CHAR_WORKFMINDEXI),
					FM_FIX_FMFEED,
					"0",			// 硐岆峈賸肮祭訧蹋,垀眕扢峈0,祥雄善模逜腔訧蹋
					tmpbuf1,	// 肮祭跺�侂驫�訧蹋
					CHAR_getWorkInt(i,CHAR_WORKFMCHARINDEX),
					CONNECT_getFdid(getfdFromCharaIndex(i))
				);
			}
		}
#endif
	}

	// 俙模眕俋腔Object蚚腔loop
	for( i = 0, movecnt = 0 ; i < (petnum/2) ; i++, charcnt++ ){
		if( charcnt >= charnum )charcnt = playernum ;
		if( CHAR_getCharUse(charcnt) == FALSE )continue;
		if( CHAR_callLoop(charcnt) == TRUE ){
			if( ++movecnt >= EnemyMoveNum )break;
		}
	}

}

char* CHAR_appendNameAndTitle( int charaindex, char* src, char* buf,
							   int buflen )
{
	if( buflen > 0 ){
		if( CHAR_CHECKINDEX(charaindex) == FALSE )
			snprintf( buf,buflen,"%s",src );
		else{
			int titleindex = CHAR_getInt(charaindex,CHAR_INDEXOFEQTITLE);
			if( titleindex == -1 )
				snprintf( buf,buflen, "%sㄩ%s",
						  CHAR_getChar(charaindex,CHAR_NAME), src );
			else
				snprintf( buf,buflen, "%s(%s)ㄩ%s",
						  CHAR_getChar(charaindex,CHAR_NAME),
						  TITLE_makeTitleStatusString(charaindex,
													  titleindex), src );
		}
	}
	return buf;
}

void CHAR_getCoordinationDir( int dir , int x, int y ,int c,
							  int *xout , int *yout )
{
	*xout = x + CHAR_getDX(dir) * c;
	*yout = y + CHAR_getDY(dir) * c;
}

BOOL CHAR_createCharacter( int type, int floor, int x, int y,int dir,
						   int* charaindex, int* objindex, BOOL seemap )
{
	Char   ch;
	Object  ob;
	if( !CHAR_getDefaultChar( &ch,type ) )return FALSE;

	ch.data[CHAR_FLOOR] = floor;
	ch.data[CHAR_X] = x;
	ch.data[CHAR_Y] = y;
	ch.data[CHAR_DIR] = dir;

	*charaindex = CHAR_initCharOneArray( &ch );
	if( *charaindex == -1 )return FALSE;

	ob.type     =   OBJTYPE_CHARA;
	ob.index    =   *charaindex;
	ob.floor    =   floor;
	ob.x        =   x;
	ob.y        =   y;

	if( seemap && MAP_walkAble( *charaindex, floor,x,y ) == FALSE ){
		CHAR_endCharOneArray( *charaindex );
		return FALSE;
	}

	*objindex = initObjectOne( &ob );
	if( *objindex == -1 ){
		CHAR_endCharOneArray(*charaindex);
		return FALSE;
	}
	CHAR_setWorkInt( *charaindex,CHAR_WORKOBJINDEX,*objindex);

	/*  汜蚗哱極↓袲禱菜竣  */

	CHAR_sendWatchEvent( *objindex,CHAR_ACTSTAND,NULL,0,TRUE);

	return TRUE;
}

void CHAR_ObjectDelete( int objindex )
{
	CHAR_sendCDArroundChar( OBJECT_getFloor(objindex),
							OBJECT_getX(objindex),
							OBJECT_getY(objindex),
							objindex );
#ifdef _MAP_WARPPOINT
	if( OBJECT_getType( objindex) == OBJTYPE_WARPPOINT ){
		print("Delete OBJTYPE_WARPPOINT endObjectOne( %d) [%d,%d,%d] \n ", objindex,
							OBJECT_getFloor(objindex), OBJECT_getX(objindex),
							OBJECT_getY(objindex) );
	}
#endif
	endObjectOne(objindex);
}

/*------------------------------------------------------------
 * す籤溘禱朝謫埰堎�詍鬷仇熗摛迖�忒朝謫埰堎��
 * 汜蚗哱極↓袲摯盓堇呫媃禱埵�彸庈袘尬動祤�
 * 礎倳
 *  charaindex      int     す籤溘騷璃溢蚗蚗袲筒
 * 蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
void CHAR_CharaDelete( int charaindex )
{
	/*  酘々喇攝袲哱禱朝謫  */
	CHAR_ObjectDelete( CHAR_getWorkInt( charaindex,CHAR_WORKOBJINDEX ) );
	/*  す籤溘摯朝謫    */
	CHAR_endCharOneArray( charaindex );
}
/*------------------------------------------------------------
 * 鏤畛騷媕↓摯  埱趙笢堎妐蚗哱禱觛埰
 ------------------------------------------------------------*/
void CHAR_CharaDeleteHavePet( int charaindex)
{
	int     i;
	int     pindex;

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
		pindex = CHAR_getCharPet( charaindex, i);
		if( CHAR_CHECKINDEX( pindex)) {
			CHAR_endCharOneArray( pindex );
		}
	}
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		pindex = CHAR_getCharPoolPet( charaindex, i);
		if( CHAR_CHECKINDEX( pindex)) {
			CHAR_endCharOneArray( pindex );
		}
	}
}
/*------------------------------------------------------------
 * 熬曹汔杽埵汔禱  藯埰堎��
 * mode		int			0: 猷覕勗毀CA邞�欷聿�
 *						1: 猷覕勗忒CA邞堎
 ------------------------------------------------------------*/
int CHAR_sendAction( int charaindex, int action, int mode)
{
	/* 邞�桫噱�  倜囮袲で砫璃敁CA禱郖窙踏鞠堎  ↓々鳴 */
	static int table[] = {
		CHAR_ACTATTACK,CHAR_ACTDAMAGE,CHAR_ACTDOWN,CHAR_ACTACTIONSTAND,
		CHAR_ACTACTIONWALK,CHAR_ACTSIT,CHAR_ACTHAND,CHAR_ACTPLEASURE,
		CHAR_ACTANGRY,CHAR_ACTSAD,CHAR_ACTGUARD, CHAR_ACTNOD, CHAR_ACTTHROW
	};
	if( action < 0 || action >= arraysizeof( table)) return FALSE;

	/* 薛堇勗囮袲で砫璃禱邞嚭埰堎 */
	CHAR_sendWatchEvent(
			CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
			table[action],
			NULL,0,mode);

	/* 囮袲で砫璃摯頞   */
	CHAR_setWorkInt( charaindex, CHAR_WORKACTION, table[action]);

	return TRUE;
}
/*------------------------------------------------------------
 * 猷覕誑橋    ぁ堣堎敁笢竣痀禱邞嚭埰堎��
 ------------------------------------------------------------*/
void CHAR_sendBattleEffect( int charaindex, int onoff)
{
	int		opt[3];

	if( onoff == 1)	{
		opt[0] = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX);
		opt[1] = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLESIDE);
		opt[2] = (BattleArray[CHAR_getWorkInt( charaindex,
				CHAR_WORKBATTLEINDEX)].Side[
				CHAR_getWorkInt( charaindex,
				CHAR_WORKBATTLESIDE)].flg & BSIDE_FLG_HELP_OK) ? TRUE:FALSE;
	}
	else {
		opt[0] = -1;
		opt[1] = -1;
		opt[2] = 0;
	}

	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ACTBATTLE,opt,arraysizeof(opt),TRUE);
}

/*------------------------------------------------------------
 * 蝠眢笢換冞捅洘      add code Shan 2001/06/30
 ------------------------------------------------------------*/
void CHAR_sendTradeEffect( int charaindex, int onoff)
{
	int opt[1];

	if( onoff == 1)	{
		opt[0] = 1;
	}
	else {
		opt[0] = -1;
	}
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ACTTRADE,opt,arraysizeof(opt),TRUE);
}

#ifdef _ANGEL_SUMMON
void CHAR_sendAngelEffect( int charaindex, int onoff)
{
	int opt[1];

	if( onoff == 1)	{
		opt[0] = 1;
	}
	else {
		opt[0] = -1;
	}
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ACTANGEL,opt,arraysizeof(opt),TRUE);
}
#endif

#ifdef _MIND_ICON
void CHAR_sendMindEffect( int charaindex, int onoff)
{
	int opt[1];

	if(onoff>0){
		opt[0] = onoff;
	}
	else{
		opt[0] = 0;
	}
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_MIND, opt, arraysizeof(opt), FALSE);
}
#endif
#ifdef _ITEM_CRACKER
void CHAR_sendCrackerEffect( int charaindex, int onoff)
{
	int opt[1];
		opt[0] = onoff;
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ITEM_CRACKER, opt, arraysizeof(opt), FALSE);
}
#endif

void CHAR_inputUserPetName( int index , int havepetindex, char* name )
{
	int		petindex;
	//char	category[3];
	char	*mycdkey=NULL, *cdkey = NULL;
	BOOL	changenameflg = FALSE;
	
	if( !CHAR_CHECKINDEX( index ) ) return;

	petindex = CHAR_getCharPet( index, havepetindex);
	if( !CHAR_CHECKINDEX( petindex)) return;

	if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){
		CHAR_talkToCli(index, -1, "模逜忐誘忤拸楊党蜊靡趼ㄐ", CHAR_COLORYELLOW);
		return;
	}

	mycdkey = CHAR_getChar( index, CHAR_CDKEY);
	cdkey = CHAR_getChar( petindex, CHAR_CDKEY);
	if( cdkey == NULL ) {
		print( "can't get CDKEY\n");
	}else {
		if( strlen( cdkey) == 0 || strcmp( cdkey, mycdkey) == 0 ) {
			changenameflg = TRUE;
		}
	}
	if( !changenameflg) {
		CHAR_talkToCli( index, -1, "拸楊峈唾昜韜靡﹝", CHAR_COLORYELLOW );
		return;
	}

	CHAR_setChar( petindex, CHAR_USERPETNAME ,name);

	if( strlen( name ) == 0 ||
		strcmp( CHAR_getChar( petindex, CHAR_NAME), name) == 0 ){
		CHAR_setChar( petindex, CHAR_CDKEY, "");
	}else {
		CHAR_setChar( petindex, CHAR_CDKEY, mycdkey);
	}
	CHAR_send_K_StatusString( index, havepetindex, CHAR_K_STRING_USERPETNAME);
}

static void CHAR_setLuck( int charaindex)
{
	LSTIME		nowlstime;
	LSTIME		mylstime;
	struct {
		int	prob;
		int	luck;
	}lucktbl[] = {
		{ 60, 1 },		/* 40% */
		{ 30, 2 },		/* 30% */
		{ 10, 3 },		/* 20% */
		{  3, 4 },		/* 7% */
		{  0, 5 }		/* 3% */
	};

	if( !CHAR_CHECKINDEX( charaindex)) return;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;

	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
	RealTimeToLSTime( CHAR_getInt( charaindex, CHAR_LASTTIMESETLUCK), &mylstime);

	/* LS鄹蹬ぁ兒  雄窀歲埱趙笢躂�匚HAR_LUCK禱脾犍埰堎 */
	if( nowlstime.day != mylstime.day || nowlstime.year != mylstime.year ) {
		int		i,r;
		r = RAND( 0, 99);
		for( i = 0; i < arraysizeof( lucktbl); i ++ ) {
			if( r >= lucktbl[i].prob ) {
				CHAR_setInt( charaindex, CHAR_LUCK, lucktbl[i].luck);
				CHAR_setInt( charaindex, CHAR_LASTTIMESETLUCK, NowTime.tv_sec);
				return;
			}
		}
	}
}
void CHAR_JoinBattle_WindowResult( int charaindex, int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);

#if 1	// 党淏瞳蚚統桵笭葩樓�鄳蓿�	Robin
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) {
		CHAR_talkToCli( charaindex, -1, " 笭葩樓�鄳蓿�! ", CHAR_COLORRED);
		print("\n 蜊猾婦!!笭葩樓�鄳蓿�!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
		//lssproto_EN_send( fd, FALSE, 0 );
		return;
	}
#endif
	if( select != WINDOW_BUTTONTYPE_CANCEL &&
	    CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
		while( 1 ) {
			int		selected;
			int		bcharaindex;
			if( fd == -1 ) break;
			selected  = atoi( data)-1;

			if( !CHAR_CHECKINDEX( CONNECT_getBattlecharaindex(fd,selected))) {
				break;
			}
        	if( CHAR_getWorkInt( CONNECT_getBattlecharaindex(fd,selected),
        						CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
				break;
			}
			if( BattleArray[ CHAR_getWorkInt(
                CONNECT_getBattlecharaindex( fd,selected),
							CHAR_WORKBATTLEINDEX)].type	!= BATTLE_TYPE_P_vs_E ){
				break;
			}
			bcharaindex = CONNECT_getBattlecharaindex( fd,selected);
			if( !(BattleArray[CHAR_getWorkInt( bcharaindex,
						CHAR_WORKBATTLEINDEX)].Side[
						CHAR_getWorkInt( bcharaindex,
						CHAR_WORKBATTLESIDE)].flg
				& BSIDE_FLG_HELP_OK)){
				break;
			}
			if(NPC_Util_CharDistance( charaindex,
                                      CONNECT_getBattlecharaindex(
                                          fd,selected) )> 1){
				break;
			}
			if( BATTLE_RescueEntry( charaindex,
                                    CONNECT_getBattlecharaindex(fd,selected) )){
	        	CHAR_talkToCli( charaindex, -1, "拸楊統桵﹝",
	        									CHAR_COLORYELLOW);
			}else {
				ret = TRUE;
			}

			break;
		}
	}
	if( ret == FALSE ) {
		if( fd != -1 ) {
			lssproto_EN_send( fd, FALSE, 0 );
		}
	}
}
void CHAR_JoinDuel_WindowResult( int charaindex, int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);
	/* 忒竣郖梣鏍攝蚗袲埰堎 */
	if( select != WINDOW_BUTTONTYPE_CANCEL &&
	    CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT )
	{
		/* 汜筒哱誑竣埱敁堁媃笢摯ぁ兒梣煦崹鳴↓鏤禱窅竣 */
		while( 1 ) {
			int		selected;
			int		enemyindex;

			if( fd == -1 ) break;
			selected  = atoi( data)-1;
			/*   嗚埵index凝 */
			if( !CHAR_CHECKINDEX( CONNECT_getDuelcharaindex(fd,selected)) ) {
				break;
			}
        	/* 橋    凝汔竣凝 */
        	if( CHAR_getWorkInt( CONNECT_getDuelcharaindex(fd,selected),
        						CHAR_WORKBATTLEMODE)
				!= BATTLE_CHARMODE_NONE)
			{
				break;
			}
			/* duel菴凝 */
			if( !CHAR_getFlg( CONNECT_getDuelcharaindex(fd,selected),
                              CHAR_ISDUEL))
			{
				break;
			}
			/* 兒倵雄  勗笢堎凝(坳贖璃迶坳ぁ  笢譴渦鞠媃趙堎蹬勗  崹堎諱) */
			if(NPC_Util_CharDistance( charaindex,
									CONNECT_getDuelcharaindex(fd,selected) )
				> 1)
			{
				break;
			}
			//   摯鞳摯す籤溘摯騷璃溢蚗袲筒
			enemyindex = CONNECT_getDuelcharaindex( fd,selected);
			// Ъ鱗誑虮埵�桯姨曼�竘操璃庈坳璃哱踏鞠堎誑
			// 碳潔埵�梠岋拜騫玅玄彸�
			if( CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYMODE )
				== CHAR_PARTY_CLIENT )
			{
				enemyindex = CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYINDEX1 );
				// 埵殽凝虮誑笢埵笢
				if( enemyindex < 0 ) break;
			}
			/* Ъ鱗誑鏤畛騷媕↓ぁ埵笢痀忒堣堎 */
			if( CHAR_getWorkInt( enemyindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
				break;
			}
			ret = BATTLE_CreateVsPlayer( charaindex, enemyindex );
			if( ret != 0 ){
				CHAR_talkToCli( charaindex, -1,
						"婈郣囮啖ㄐ", CHAR_COLORYELLOW);
				ret = FALSE;
			}
			else {
				ret = TRUE;
			}
			break;
		}
	}
	if( ret == FALSE ) {
		/* 踕堅邞嚭 */
		if( fd != -1 ) {
			lssproto_EN_send( fd, FALSE, 0 );
		}
	}
}
void CHAR_SelectCard_WindowResult( int charaindex, int select, char *data)
{
	if( select != WINDOW_BUTTONTYPE_CANCEL ){
		while( 1 ) {
			int		selected;
			int		fd;
			fd = getfdFromCharaIndex( charaindex);
			if( fd == -1 ) break;
			selected  = atoi( data)-1;
			if( !CHAR_CHECKINDEX(
                CONNECT_getTradecardcharaindex(fd,selected))) {
				break;
			}
	  		if( CHAR_getWorkInt( CONNECT_getTradecardcharaindex(fd,selected),
	  							CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE){
	  			break;
	  		}
	  		if(!CHAR_getFlg( CONNECT_getTradecardcharaindex(fd,selected),
	  						CHAR_ISTRADECARD)){
	  			break;
	  		}
			if( NPC_Util_CharDistance( charaindex,
									CONNECT_getTradecardcharaindex
                                       (fd,selected) ) > 1){
				break;
			}
			ADDRESSBOOK_addAddressBook( charaindex,
                                        CONNECT_getTradecardcharaindex(
                                            fd,selected));
			break;
		}
	}
}

void CHAR_JoinParty_WindowResult( int charaindex , int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);
	/* 忒竣郖梣鏍攝蚗袲埰堎 */
	if( select != WINDOW_BUTTONTYPE_CANCEL &&
	    /* 猷覕誑蚕↓  贖蚻庌ぁ倜�淜�   */
	    CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE )
	{
		/* 汜筒哱誑竣埱敁堁媃笢摯ぁ兒梣煦崹鳴↓鏤禱窅竣 */
		while( 1 ) {
			int		parray;
			int		selected;
			int		toindex = -1;
			if( fd == -1 ) break;
			selected  = atoi( data)-1;

			/*   嗚埵index凝
			 * (福菺※坳贖璃迶坳ぁ  笢譴渦鞠媃趙堎蹬勗
			 *    嗚埵index勗埵堎菴  鍛誑堣堎)
			 */
			if( !CHAR_CHECKINDEX(
                CONNECT_getJoinpartycharaindex(fd,selected))) {
				break;
			}
			/* 虮誑笢倜�梒耒�耦堇③埰 */
			if( CHAR_getWorkInt( CONNECT_getJoinpartycharaindex(
                fd,selected),	CHAR_WORKPARTYMODE)
			== CHAR_PARTY_NONE )
			{
				toindex = CONNECT_getJoinpartycharaindex( fd,selected );
			}
			else {
				toindex = CHAR_getPartyIndex(
						CONNECT_getJoinpartycharaindex(fd,selected), 0);
				if( !CHAR_CHECKINDEX( toindex) ) {
					print( " %s:%d err\n", __FILE__, __LINE__);
					break;
				}
			}
			/* 兒倵雄  勗笢堎凝(坳贖璃迶坳ぁ  笢譴渦鞠媃趙堎蹬勗  崹堎諱) */
			if( NPC_Util_CharDistance( charaindex, toindex ) > 1) {
				break;
			}
        	/* 橋    毀煦誧 */
        	if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLEMODE)
				!= BATTLE_CHARMODE_NONE)
			{
				break;
			}
			/* 黥蹬舅菴さ↓迶凝 */
			if( !CHAR_getFlg( toindex, CHAR_ISPARTY)) {
				break;
			}

			/* Ъ鱗蚕↓  贖摯硞倳毀    凝ㄑ */
			parray = CHAR_getEmptyPartyArray( toindex) ;
			if( parray == -1 ) break;

#ifdef _DEATH_CONTEND
			if(CHAR_getInt(toindex,CHAR_PKLISTTEAMNUM) == -1 && CHAR_getInt(charaindex,CHAR_PKLISTTEAMNUM) == -1){
			}else if(CHAR_getInt(charaindex,CHAR_PKLISTLEADER) > 0 ||
					CHAR_getInt(toindex, CHAR_PKLISTTEAMNUM) < 0 ||
					CHAR_getInt(charaindex, CHAR_PKLISTTEAMNUM) < 0 ||
					CHAR_getInt(toindex,CHAR_PKLISTTEAMNUM) != CHAR_getInt(charaindex,CHAR_PKLISTTEAMNUM) ||
					CHAR_getInt(toindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
					CHAR_talkToCli( charaindex, -1, "勦斪祥肮ㄛ拸楊樓�輹韃荂�", CHAR_COLORYELLOW);
					ret = FALSE;
					break;
				}
#endif

			/* 蚕↓  贖勗  �桷齉� */
			CHAR_JoinParty_Main( charaindex, toindex);

			ret = TRUE;

			break;
		}
	}
	if( ret == FALSE ) {
		CHAR_talkToCli( charaindex, -1, "拸楊樓�輹韃荂�",
						CHAR_COLORYELLOW);
	}
	/* 踕堅邞嚭 */
	if( fd != -1 ) {
		lssproto_PR_send( fd, 1, ret);
	}
}
void CHAR_JoinBattleWatch_WindowResult( int charaindex, int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);
	/* 忒竣郖梣鏍攝蚗袲埰堎 */
	if( select != WINDOW_BUTTONTYPE_CANCEL ) {
		/* 汜筒哱誑竣埱敁堁媃笢摯ぁ兒梣煦崹鳴↓鏤禱窅竣 */
		while( 1 ) {
			int		selected;
			if( fd == -1 ) break;
			selected  = atoi( data)-1;

			/*   嗚埵index凝
			 * (福菺※坳贖璃迶坳ぁ  笢譴渦鞠媃趙堎蹬勗
			 *    嗚埵index勗埵堎菴  鍛誑堣堎)
			 */
			if( !CHAR_CHECKINDEX( CONNECT_getBattlecharaindex(fd,selected))) {
				break;
			}
        	/* 橋    凝汔竣凝 */
        	if( CHAR_getWorkInt( CONNECT_getBattlecharaindex(fd,selected),
        						CHAR_WORKBATTLEMODE)
				== BATTLE_CHARMODE_NONE)
			{
				break;
			}
			/* 兒倵雄  勗笢堎凝(坳贖璃迶坳ぁ  笢譴渦鞠媃趙堎蹬勗  崹堎諱) */
			if(NPC_Util_CharDistance( charaindex,
                                      CONNECT_getBattlecharaindex(
                                          fd,selected) )
				> 1)
			{
				break;
			}
			/* 操璃哱惉↓踏鞠堎 */
			if( BATTLE_WatchEntry( charaindex,
                                    CONNECT_getBattlecharaindex(fd,selected)
					 ))
			{
	        	CHAR_talkToCli( charaindex, -1, "拸楊夤桵﹝",
	        									CHAR_COLORYELLOW);
			}
			else {
				ret = TRUE;
			}

			break;
		}
	}
	if( ret == FALSE ) {
		/* 踕堅邞嚭 */
		if( fd != -1 ) {
			lssproto_EN_send( fd, FALSE, 0 );
		}
	}
}

#ifdef _FM_MODIFY
extern void NPC_FmDengonWindowTalked( int index, int talker, int seqno, int select, char *data);
#endif

void CHAR_processWindow(int charaindex, int seqno, int select,
						int objindex, char* data )
{
	if( CHECKOBJECTUSE(objindex)) {
		if( OBJECT_getType(objindex) == OBJTYPE_CHARA ) {
			typedef void (*WINDF)(int,int,int, int,char*);
			WINDF   windowtalkedfunc=NULL;
			windowtalkedfunc = (WINDF)CHAR_getFunctionPointer(
							OBJECT_getIndex(objindex),CHAR_WINDOWTALKEDFUNC);
			if( windowtalkedfunc ) {
				windowtalkedfunc( OBJECT_getIndex(objindex), charaindex,
								seqno, select, data);
			}
		}
#ifdef _NPCSERVER_NEW //CHAR_WINDOWTALKEDFUNC
		else if( OBJECT_getType(objindex) == OBJTYPE_NPCSCHARA ) {
			int npcindex = OBJECT_getNpcIndex( objindex);
			if( npcfd != -1 )
				NPCS_NpcWinMess_send( objindex, npcindex, charaindex, data, seqno, select);
		}
#endif
	}else {
		if( seqno == CHAR_WINDOWTYPE_SELECTBATTLE) {
			CHAR_JoinBattle_WindowResult( charaindex, select, data);
		}else if( seqno == CHAR_WINDOWTYPE_SELECTDUEL) {
			CHAR_JoinDuel_WindowResult( charaindex, select, data);
		}else if( seqno == CHAR_WINDOWTYPE_SELECTTRADECARD) {
			CHAR_SelectCard_WindowResult( charaindex, select, data);
		}else{
			if( seqno == CHAR_WINDOWTYPE_SELECTPARTY) {
				CHAR_JoinParty_WindowResult( charaindex, select, data);
			}else {
				if( seqno == CHAR_WINDOWTYPE_SELECTBATTLEWATCH) {
					CHAR_JoinBattleWatch_WindowResult( charaindex, select, data);
				}else if( seqno >= CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1 && 
						 seqno <= CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME_ATTENTION ){
					ITEM_useRenameItem_WindowResult( charaindex, seqno, select, data);
				}
#ifdef _PETSKILL_CANNEDFOOD
				else if( seqno == ITEM_WINDOWTYPE_SELECTPETSKILL_SELECT ){
					ITEM_usePetSkillCanned_WindowResult( charaindex, seqno, select, data);
				}
#endif
			}
		}
#ifdef _FM_MODIFY
		if(seqno >= CHAR_WINDOWTYPE_FM_DENGON || seqno <= CHAR_WINDOWTYPE_FM_DPSELECT)
			NPC_FmDengonWindowTalked(0,charaindex,seqno,select,data);
#endif
	}
}

void CHAR_PartyUpdate( int charaindex, int senddata ){
	int i, ownerindex, mode, pindex, pno;
	mode = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE );
	if( mode == CHAR_PARTY_NONE )return;
	if( mode == CHAR_PARTY_LEADER ){
		ownerindex = charaindex;
	}else{
		ownerindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1 );
	}

	for( pno = 0; pno < CHAR_PARTYMAX; pno ++ ){
		pindex = CHAR_getWorkInt( ownerindex, CHAR_WORKPARTYINDEX1+pno );
		if( pindex == charaindex )break;
	}
	if( pno == CHAR_PARTYMAX )return;
	for( i = 0; i < CHAR_PARTYMAX; i ++ ){
		pindex = CHAR_getWorkInt( ownerindex, CHAR_WORKPARTYINDEX1+i );
		if( pindex == charaindex )continue;
		if( pindex < 0 )continue;
		CHAR_send_N_StatusString( pindex, pno, senddata );
	}
}

static int CHAR_getObjectByPosition( int myobjindex, int fl, int x, int y,
									  int *objbuf, int objbuflen)
{
	int     i,j;
	int		cnt = 0;

	for( i = x-CHAR_DEFAULTSEESIZ/2 ; i <= x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j = y-CHAR_DEFAULTSEESIZ/2 ; j <= y+CHAR_DEFAULTSEESIZ/2 ;
			 j ++ )
		{
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ;
				 object ;
				 object = NEXT_OBJECT(object ) )
			{
				int objindex = GET_OBJINDEX(object);
				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType( objindex) == OBJTYPE_CHARA &&
					!CHAR_getFlg( OBJECT_getIndex( objindex), CHAR_ISVISIBLE)) 
				{
					continue;
				}
				/* 猷覕毀笢笢源 */
				if( objindex == myobjindex) continue;

				objbuf[cnt] = objindex;
				cnt ++;
				if( cnt >= objbuflen) return cnt;
			}
		}
	}
	return cnt;
}
static void CHAR_setMyPosition_sendData( int charaindex,int prev_x, int prev_y, int x, int y)
{
	int		CurrentObjCollection[CHAR_DEFAULTSEESIZ*CHAR_DEFAULTSEESIZ*10];
	int		PrevObjCollection[CHAR_DEFAULTSEESIZ*CHAR_DEFAULTSEESIZ*10];
	int		curcnt,prevcnt;
	int		objindex;
	int		fl;
	char    introduction[512];
	char    c_msg[1024*96];
	char	buf[64];
	int     strpos;
	int		fd;
	int		i,j;
	char	cabuf[128];
	char	myintroduction[512];
	char	cd_msg[128];

	fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	if( fd == -1 )return;

	objindex = CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX);
	fl = CHAR_getInt( charaindex, CHAR_FLOOR);

	curcnt = CHAR_getObjectByPosition( objindex, fl,x,y, CurrentObjCollection, sizeof(CurrentObjCollection));
	prevcnt = CHAR_getObjectByPosition( objindex, fl, prev_x, prev_y, PrevObjCollection, sizeof(PrevObjCollection));
	strpos = 0;
	c_msg[0] = '\0';
	if( !CHAR_makeObjectCString( objindex, myintroduction,sizeof( myintroduction))){
		myintroduction[0] = '\0';
	}

	for( i = 0 ; i < curcnt; i ++ ) {
		int	findflg = FALSE;
		for( j = 0; j < prevcnt; j ++ ) {
			if( CurrentObjCollection[i] == PrevObjCollection[j] ) {
				findflg = TRUE;
				break;
			}
		}
		if( !findflg ) {
			int introlen;
			int c_index = OBJECT_getIndex( CurrentObjCollection[i]);
			if( CHAR_makeObjectCString( CurrentObjCollection[i], introduction,
											sizeof( introduction)))
			{
				introlen = strlen( introduction);
				introduction[introlen] = ',';
				introduction[introlen+1] = '\0';

				strcpysafe( &c_msg[strpos], sizeof( c_msg) - strpos,
							introduction);
				strpos += strlen( introduction);
				if( strpos >= sizeof( c_msg)) break;
			}
			if( strlen( myintroduction) != 0 ) {
				if( OBJECT_getType( CurrentObjCollection[i] ) == OBJTYPE_CHARA ) {
					if( CHAR_getInt( c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
						int		tofd = getfdFromCharaIndex( c_index );
						CDsend( tofd);
						lssproto_C_send( tofd, myintroduction);
					}
				}
			}
			if( OBJECT_getType( CurrentObjCollection[i]) == OBJTYPE_CHARA ) {
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKPARTYMODE )
						== CHAR_PARTY_LEADER ){
					if( CHAR_makeCAOPT1String( CurrentObjCollection[i], cabuf, sizeof( cabuf),
												CHAR_ACTLEADER,1 )){
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}
				
				// Robin 0611 trade
				/* 褒伎蝠眢笢 */
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKTRADEMODE ) != CHAR_TRADE_FREE )	{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf), CHAR_ACTTRADE,1 ))
					{
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}				

#ifdef _ANGEL_SUMMON
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKANGELMODE ) == TRUE )	{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf), CHAR_ACTANGEL,1 ))
					{
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}				
#endif

#ifdef _MIND_ICON
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE &&
					CHAR_getWorkInt( c_index, CHAR_MIND_NUM))
				{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
										CHAR_MIND, CHAR_getWorkInt( c_index, CHAR_MIND_NUM)))
					{
						CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
						//print("\nshan---->(2)cabuf-->%s", cabuf);
					}
				}				
#endif
#ifdef _STREET_VENDOR
				if(CHAR_getWorkInt(c_index,CHAR_WORKSTREETVENDOR) == 1 && 
					CHAR_getWorkInt(c_index,CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
					if(CHAR_makeCAOPTString(objindex,cabuf,sizeof(cabuf),
						CHAR_STREETVENDOR_OPEN,CHAR_getWorkChar(c_index,CHAR_STREETVENDOR_NAME)))
						CONNECT_appendCAbuf(fd,cabuf,strlen(cabuf));
				}
#endif
#ifdef _ITEM_CRACKER
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE &&
					CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER))
				{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
										CHAR_ITEM_CRACKER, CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER)))
					{
						CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
					}
				}				
#endif				
				/* 猷覕誑虮ぁЪ鱗誑橋    ぁ埵笢埵�梀藍麇槾齱囃腹�  崚CA禱邞堎 */
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE )
						== CHAR_PARTY_LEADER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE)
						== BATTLE_CHARMODE_NONE)
				{
					int		tofd = getfdFromCharaIndex( c_index );
					if( tofd != -1 ) {
						if( CHAR_makeCAOPT1String( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
													cabuf, sizeof( cabuf),
													CHAR_ACTLEADER,1 ))
						{
							CONNECT_appendCAbuf( tofd,cabuf,strlen(cabuf));
						}
					}
				}
				/* 峟窇倜す籤溘誑橋    埵�桴�  囮騷昡璃  崚    禱邞堎 */
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )
					!= BATTLE_CHARMODE_NONE )
				{
					/* め橋CA */
					if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEWATCH) == TRUE ) {
						if( CHAR_makeCAOPT1String( CurrentObjCollection[i], cabuf,
													sizeof( cabuf),
													CHAR_ACTBATTLEWATCH,
													1))
						{
							CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
						}
					}
					else {
						int	battleno = CHAR_getWorkInt( c_index,
															CHAR_WORKBATTLEINDEX);
						int	sideno   = CHAR_getWorkInt( c_index,
															CHAR_WORKBATTLESIDE);
						int	helpno = (BattleArray[CHAR_getWorkInt( c_index,
								CHAR_WORKBATTLEINDEX)].Side[
								CHAR_getWorkInt( c_index,
								CHAR_WORKBATTLESIDE)].flg & BSIDE_FLG_HELP_OK) ? TRUE:FALSE;
						/* HelpNo = 1 毀痄 */
						if( CHAR_makeCAOPT3String( CurrentObjCollection[i], cabuf, sizeof( cabuf),
													CHAR_ACTBATTLE,
													battleno,sideno,helpno
													))
						{
							CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
						}
					}
				}
	 	   		/* 峟窇倜す籤溘誑笰凝摯囮袲で砫璃禱媃趙笢躂�夼肴瞿� */
				if( ( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ||
					  CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPET ) &&
					CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 )
				{
					if( CHAR_makeCADefaultString( CurrentObjCollection[i], cabuf, sizeof( cabuf),
									CHAR_getWorkInt( c_index, CHAR_WORKACTION)))
					{
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}
			}
		}
	}
	dchop( c_msg, "," );
	if( strlen( c_msg) != 0 ) {
		lssproto_C_send( fd, c_msg);
	}

	/* 猷覕摯CD軘埬 */
	cnv10to62( objindex,cd_msg, sizeof(cd_msg));

	for( i = 0 ; i < prevcnt; i ++ ) {
		int	findflg = FALSE;
		/* 鞳摯痾  勗笢堎酘々喇攝袲哱ぁ�椯婸�摯淢  勗笢埵笢
		 * 酘々喇攝袲哱埵�殏梊庋迓樝鬷仇熗摛迖�煦
		 */
		for( j = 0; j < curcnt; j ++ ) {
			if( PrevObjCollection[i] == CurrentObjCollection[j] ) {
				findflg = TRUE;
				break;
			}
		}
		if( !findflg ) {
			cnv10to62( PrevObjCollection[i], buf, sizeof( buf));
			CONNECT_appendCDbuf( fd, buf, strlen( buf));
			
			/* Ъ鱗勗忒    邞堎 */
			if( OBJECT_getType( PrevObjCollection[i]) == OBJTYPE_CHARA ) {
				int tocharaindex = OBJECT_getIndex( PrevObjCollection[i]);
				if( CHAR_getInt( tocharaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
					int tofd = CHAR_getWorkInt( tocharaindex, CHAR_WORKFD);
					if( tofd != -1 ) {
						CONNECT_appendCDbuf( tofd, cd_msg, strlen( cd_msg));
						CDsend( tofd);
					}
				}
			}
		}
	}
	CDsend( fd);


}
BOOL CHAR_setMyPosition( int index, int x, int y, BOOL CAFlg)
{
    return CHAR_setMyPosition_main( index,x,y,-1,CAFlg);
}

BOOL CHAR_setMyPosition_main( int index, int x, int y, int setdir, BOOL CAFlg)
{
	int     objindex;
	int		prev_x,prev_y;
	int		fl;

	prev_x = CHAR_getInt( index, CHAR_X);
	prev_y = CHAR_getInt( index, CHAR_Y);
	if( prev_x == x && prev_y == y ) {
		return FALSE;
	}

	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
		return FALSE;
	}

	if( CHAR_getFlg( index, CHAR_ISWARP)) return FALSE;

	objindex = CHAR_getWorkInt(index,CHAR_WORKOBJINDEX);
	fl = CHAR_getInt( index, CHAR_FLOOR);

	if( !MAP_IsValidCoordinate( fl, x, y )) {
		print( "error: invalid Coordinate fl[%d] x[%d] y[%d] %s:%d\n", 
					fl, x, y, __FILE__, __LINE__);
                CONNECT_setCloseRequest( CHAR_getWorkInt( index, CHAR_WORKFD), 1);
		return FALSE;
	}

	if( ABS( x - prev_x) > CHAR_DEFAULTSEESIZ/2 ||
		ABS( y - prev_y) > CHAR_DEFAULTSEESIZ/2 )
	{
		CHAR_sendCDArroundChar( fl,prev_x,prev_y,objindex );
	}

	if( ABS( x - prev_x) <= 1 && ABS( y - prev_y ) <= 1){
		POINT start,end;
		int	dir;
		start.x = CHAR_getInt( index, CHAR_X);
		start.y = CHAR_getInt( index, CHAR_Y);
		end.x = x;
		end.y = y;
		dir = NPC_Util_getDirFromTwoPoint( &start, &end);
		if( dir != -1 ) {
			CHAR_walk( index, dir, 0);
		}
	}
	else {
		CHAR_setInt( index, CHAR_X,x);
		CHAR_setInt( index, CHAR_Y,y);
        if( setdir >= 0 && setdir < 8) CHAR_setInt( index, CHAR_DIR, setdir);
		{
			int of,ox,oy;
			of = OBJECT_setFloor(objindex,fl);
			ox = OBJECT_setX(objindex,x);
			oy = OBJECT_setY(objindex,y);

			if( !MAP_objmove( objindex,of,ox,oy,fl,x,y ) ){
				fprint( "ERROR MAP_OBJMOVE objindex=%d\n",objindex );
				return FALSE;
			}
		}
		if( ABS( x - prev_x) > CHAR_DEFAULTSEESIZ/2 ||
			ABS( y - prev_y) > CHAR_DEFAULTSEESIZ/2 )
		{
			CHAR_sendWatchEvent(objindex,CHAR_ACTWARP,NULL,0,FALSE);
		}
		else if( CAFlg == TRUE ) {
			CHAR_sendWatchEvent(objindex,CHAR_ACTWARP,NULL,0,FALSE);
		}
	}
	
	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) {
		if( ABS( x - prev_x) <= 1 && ABS( y - prev_y ) <= 1){
			int		i;
			int prevparty_x = prev_x;
			int prevparty_y = prev_y;
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int toindex = CHAR_getWorkInt( index, i + CHAR_WORKPARTYINDEX1);
				if( CHAR_CHECKINDEX(toindex) ) {
					int client_x = prevparty_x;
					int client_y = prevparty_y;
					int	dir;
					POINT	start, end;

					prevparty_x = CHAR_getInt( toindex, CHAR_X);
					prevparty_y = CHAR_getInt( toindex, CHAR_Y);
					start.x = prevparty_x;
					start.y = prevparty_y;

					end.x = client_x;
					end.y = client_y;

					dir = NPC_Util_getDirFromTwoPoint( &start, &end);
					if( dir != - 1 ) {
						CHAR_walk( toindex, dir, 0);
					}
				}
			}
		}

		else if( ABS( x - prev_x) > CHAR_DEFAULTSEESIZ/2 ||
				ABS( y - prev_y) > CHAR_DEFAULTSEESIZ/2 )
		{
			int i;
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int toindex = CHAR_getWorkInt( index, i + CHAR_WORKPARTYINDEX1);
				if( CHAR_CHECKINDEX(toindex) ) {
					CHAR_warpToSpecificPoint( toindex,fl , x,y);
				}
			}
		}
	}

	if( ABS( x - prev_x) <= CHAR_DEFAULTSEESIZ/2 &&
		ABS( y - prev_y) <= CHAR_DEFAULTSEESIZ/2 )
	{
		CHAR_setMyPosition_sendData( index, prev_x, prev_y, x, y);
	}

	return TRUE;
}


static char CHAR_P_statusSendBuffer[STRINGBUFSIZ];



static char *CHAR_make_P_StatusString( int charaindex, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_P_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_P_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_P_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_P_STRING_MAXMP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_P_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_P_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_P_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_P_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_P_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_P_STRING_CHARM, 	CHAR_WORKFIXCHARM,		1 },
		{ CHAR_P_STRING_LUCK, 	CHAR_WORKFIXLUCK,		1 },
		{ CHAR_P_STRING_GOLD, 	CHAR_GOLD,				0 },
		{ CHAR_P_STRING_TITLE, 	CHAR_INDEXOFEQTITLE,	0 },
		{ CHAR_P_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_P_STRING_OWNTITLE, CHAR_OWNTITLE,			2 },
		{ CHAR_P_STRING_VITAL, 	CHAR_VITAL,				10 },
		{ CHAR_P_STRING_STR, 	CHAR_STR,				10 },
		{ CHAR_P_STRING_TOUGH, 	CHAR_TOUGH,				10 },
		{ CHAR_P_STRING_DEX, 	CHAR_DEX,				10 },
		{ CHAR_P_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11 },
		{ CHAR_P_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11 },
		{ CHAR_P_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11 },
		{ CHAR_P_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11 },
		{ CHAR_P_STRING_DUELPOINT, 	CHAR_DUELPOINT,		0 },
		{ CHAR_P_STRING_TRANSMIGRATION, CHAR_TRANSMIGRATION, 0},
		{ CHAR_P_STRING_RIDEPET, CHAR_RIDEPET, 0},
		{ CHAR_P_STRING_LEARNRIDE, CHAR_LEARNRIDE, 0},
		{ CHAR_P_STRING_BASEBASEIMAGENUMBER, CHAR_BASEBASEIMAGENUMBER, 0},
		{ CHAR_P_STRING_SKYWALKER, CHAR_WORKSKYWALKER, 1},
		{ CHAR_P_STRING_DEBUGMODE, CHAR_WORKDEBUGMODE, 1},
/*
#ifdef _CHAR_PROFESSION
#ifdef _SEND_PROF_DATA
		{ CHAR_P_STRING_PROFESSION, PROFESSION_CLASS, 20},
#endif
#endif
*/
	};

	snprintf( CHAR_P_statusSendBuffer, sizeof( CHAR_P_statusSendBuffer),
				"P%s|", cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_P_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( charaindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( charaindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( charaindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}
/*
#ifdef _CHAR_PROFESSION
#ifdef _SEND_PROF_DATA
					else if( chk[j].gettype == 20 ) {
						int k;
						char tempbuf[256];
						strcpy( tmp, "");
						for( k=0; k<11; k++){
							ret = CHAR_getInt( charaindex, chk[j].intdataindex + k );
							snprintf( tempbuf, sizeof( tempbuf), "%d|", ret);
							strcat( tmp, tempbuf);
						}
					}
#endif
#endif
*/
					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_P_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( charaindex, CHAR_getInt( charaindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
			}
			if( !found) {
				print( "梑祥善硌隅腔統杅[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_P_statusSendBuffer[ strlength],
							sizeof( CHAR_P_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_P_statusSendBuffer ) ) {
					return CHAR_P_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_P_statusSendBuffer;
}



BOOL CHAR_send_P_StatusString( int charaindex, unsigned int indextable )
{
	char*   string;
	//if( indextable >= ( CHAR_P_STRING_BASEBASEIMAGENUMBER << 1)) {
	if( indextable >= ( CHAR_P_STRING_SKYWALKER << 1)) {
	//if( indextable >= ( CHAR_P_STRING_DEBUGMODE << 1)) {
		print( "send_P invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}

	string = CHAR_make_P_StatusString( charaindex, indextable );

	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}
static char CHAR_N_statusSendBuffer[STRINGBUFSIZ];
/*------------------------------------------------------------
 * 袲溘騷囮璃哱勗邞堎  ��  摯  棬  禱軘埬埰堎��
 * 礎倳
 *  charaindex      int     す籤溘騷璃溢袲蚗筒
 *  indextable		int 	P 禱邞堎蚕溘隍↓淏摯襛譴  ��
 *------------------------------------------------------------*/
static char *CHAR_make_N_StatusString( int charaindex, int num, unsigned int indextable )
{
	int		i,j;
	int 	nindex = -1;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_N_STRING_OBJINDEX, 	CHAR_WORKOBJINDEX,	1 },
		{ CHAR_N_STRING_LV,		CHAR_LV, 				0 },
		{ CHAR_N_STRING_MAXHP, 	CHAR_WORKMAXHP,			1 },
		{ CHAR_N_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_N_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_N_STRING_NAME, 	CHAR_NAME,				2 },


	};
	/*  黥蹬筒  ↓淏筒溢↓淏    */
	/*       覽鏍攝蚗袲 */
	if( num < 0 || num >= CHAR_PARTYMAX ){
		print( "攬衭耀宒囮啖 (%c)%d \n", num, num );
		return NULL;
	}
	/* 黥蹬摯騷璃溢蚗袲筒禱噁   */
	nindex = CHAR_getPartyIndex( charaindex, num);

	if( nindex == -1 ){
		/*    寤盓崹汔 snprintf 埰堎    */
		snprintf( CHAR_N_statusSendBuffer,sizeof(CHAR_N_statusSendBuffer),
			 "N%d|0|", num );
		return  CHAR_N_statusSendBuffer;
	}

	snprintf( CHAR_N_statusSendBuffer, sizeof( CHAR_N_statusSendBuffer),
				"N%d|%s|", num,cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_N_statusSendBuffer );

	/* indextable摯  ぁ  埱趙笢堎扠蚗哱禱棹  勗ぅ迋堎 */
	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			/*   埱趙笢堎扠蚗哱敁�徯hk勗樅蚗哱埰堎凝ぅ迋堎 */
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					/* 郖窙媃倜摯ぁ偯禱噁  埰堎 */
					/* 倳偯捈摯溢↓淏 */
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( nindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( nindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( nindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( nindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					/*   棬  捈摯溢↓淏 */
					else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( nindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}
					found = TRUE;
					break;
				}
			}
			/* 眥摯淏騷鏤摯溢↓淏凝忒媃躂埵笢 */
			if( !found) {
				/* 喫喫凝�梫捶鉆捶鉆� */
			}
			if( !found) {
				print( "梑祥善硌隅腔統杅[%d]\n",indextable);
			}
			else {
				strcpysafe( &CHAR_N_statusSendBuffer[ strlength],
							sizeof( CHAR_N_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_N_statusSendBuffer ) ) {
					return CHAR_N_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_N_statusSendBuffer;
}
/*------------------------------------------------------------
 * S P 筒  ↓淏筒禱邞堎�袺鯆慪濛邴彸翹副奴�窀ぁ毀鏍攝蚗袲媃埵�妡窈誰苺�
 * 礎倳
 *  charaindex      int     す籤溘騷璃溢袲蚗筒
 *  num				int		黥蹬  蠕
 *  indextable		int 	P 禱邞堎蚕溘隍↓淏摯襛譴  ��
 * 蒍堇偯
 *  邞埱倜�臷RUE(1)
 *  邞躂埵凝埱倜�聏ALSE(0)
 *------------------------------------------------------------*/
BOOL CHAR_send_N_StatusString( int charaindex, int num, unsigned int indextable )
{
	char*   string;

	if( indextable >= (CHAR_N_STRING_NAME << 1)) {
		print( "send_N invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}
	string = CHAR_make_N_StatusString( charaindex, num, indextable );
	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}

static char CHAR_K_statusSendBuffer[STRINGBUFSIZ];
static char *CHAR_make_K_StatusString( int charaindex, int num, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	int 	pindex;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_K_STRING_BASEIMAGENUMBER, 	CHAR_BASEIMAGENUMBER,	0 },
		{ CHAR_K_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_K_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_K_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_K_STRING_MAXHP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_K_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_K_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_K_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_K_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_K_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_K_STRING_AI, 	CHAR_WORKFIXAI,			1 },
		{ CHAR_K_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11},
		{ CHAR_K_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11},
		{ CHAR_K_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11},
		{ CHAR_K_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11},
		{ CHAR_K_STRING_SLOT,	CHAR_SLOT,				0 },
		{ CHAR_K_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_K_STRING_USERPETNAME, CHAR_USERPETNAME,  2 },

	};
	/*       覽鏍攝蚗袲 */
	if( num < 0 || num >= CHAR_MAXPETHAVE ){
		print( "唾昜耀宒囮啖 (%c)%d \n", num, num );
		return NULL;
	}
	/* 妐蚗哱摯騷璃溢蚗袲筒禱噁   */
	pindex = CHAR_getCharPet( charaindex, num );
	if( pindex == -1 ){
		/*    寤盓崹汔 snprintf 埰堎    */
		snprintf(CHAR_K_statusSendBuffer,sizeof(CHAR_K_statusSendBuffer),
			 "K%d|0|", num );
		return  CHAR_K_statusSendBuffer;
	}

	snprintf( CHAR_K_statusSendBuffer, sizeof( CHAR_K_statusSendBuffer),
				"K%d|%s|", num, cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_K_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( pindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( pindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( pindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( pindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					/*   棬  捈摯溢↓淏 */
					else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( pindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}
					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_K_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( pindex, CHAR_getInt( pindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
				if( indextable & CHAR_K_STRING_CHANGENAMEFLG ) {
					BOOL	changenameflg = FALSE;
					char	*mycdkey, *cdkey = NULL;
					mycdkey = CHAR_getChar( charaindex, CHAR_CDKEY);
					{
						cdkey = CHAR_getChar( pindex, CHAR_CDKEY);
						if( cdkey == NULL ) {
							print( "can't get CDKEY\n");
						}else {
							if( strlen( cdkey) == 0 ||
								strcmp( cdkey, mycdkey) == 0 ){
								changenameflg = TRUE;
							}
						}
					}
					snprintf( tmp, sizeof( tmp), "%d|", changenameflg);
					found = TRUE;
				}
			}
			if( !found) {
				print( "梑祥善硌隅腔統杅[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_K_statusSendBuffer[ strlength],
							sizeof( CHAR_K_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_K_statusSendBuffer ) ) {
					return CHAR_K_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_K_statusSendBuffer;
}
/*------------------------------------------------------------
 * S K 筒  ↓淏筒禱邞堎�袺鯆慪濛邴彸翹副奴�窀ぁ毀鏍攝蚗袲媃埵�妡窈誰苺�
 * 礎倳
 *  charaindex      int     す籤溘騷璃溢袲蚗筒
 *  indextable		int 	P 禱邞堎蚕溘隍↓淏摯襛譴  ��
 * 蒍堇偯
 *  邞埱倜�臷RUE(1)
 *  邞躂埵凝埱倜�聏ALSE(0)
 *------------------------------------------------------------*/
BOOL CHAR_send_K_StatusString( int charaindex, int num, unsigned int indextable )
{
	char*   string;

	if( indextable >= (CHAR_K_STRING_USERPETNAME << 1)) {
		print( "send_K invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}
	string = CHAR_make_K_StatusString( charaindex, num, indextable );
	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}



//------------------------------------------------------------
// 鏤畛騷媕↓摯    禱萃猀
//------------------------------------------------------------
void CHAR_AddCharm( int charaindex, int iValue )
{
	int iWork;
	iWork = CHAR_getInt( charaindex, CHAR_CHARM ) + iValue;
	iWork = max( 0, iWork );	//   覽兔
	iWork = min( 100, iWork );	//     兒兔兔
	CHAR_setInt( charaindex, CHAR_CHARM, iWork );
}

//------------------------------------------------------------
// 妐蚗哱摯櫆笠梣  撬偯禱萃猀埰堎��
//------------------------------------------------------------
void CHAR_PetAddVariableAi( int petindex, int iValue )
{
	int iWork;
	iWork = CHAR_getInt( petindex, CHAR_VARIABLEAI ) + iValue;
	iWork = min( CHAR_MAXVARIABLEAI, iWork );
	iWork = max( CHAR_MINVARIABLEAI, iWork );
	CHAR_setInt( petindex, CHAR_VARIABLEAI, iWork );

}

//------------------------------------------------------------
// 溢↓淏穫↓筒勗頞  埰堎す↓    鞳  禱軘埬埰堎��
//------------------------------------------------------------
int CHAR_makeDBKey( int charaindex, char *pszBuffer, int size ){

	char *cdkey;
	char buff[512];
	char escapebuf[1024];

	pszBuffer[0] = 0;	// 疐趵撬
	cdkey = CHAR_getChar( charaindex, CHAR_CDKEY);
	if( cdkey == NULL )return FALSE; // 頞  ぁ拻埵笢
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;

	// す↓毀 cdkey_  鞳
	snprintf( buff, sizeof(buff),
		"%s_%s", cdkey, CHAR_getChar( charaindex, CHAR_NAME ) );
	makeEscapeString( buff, escapebuf, sizeof(escapebuf));
	strcpysafe( pszBuffer, size, escapebuf);

	return TRUE;
}


//------------------------------------------------------------
//
//  溢↓淏穫↓筒摯囂  禱脾犍
//
// DuelPoint 摯 DBUpdateEntry禱邞堎
//------------------------------------------------------------
BOOL CHAR_send_DpDBUpdate( int charaindex )
{
	int fd,dp;
	char szKey[256];
	if( CHAR_makeDBKey(  charaindex, szKey, sizeof( szKey ) ) == FALSE ){
		return FALSE;
	}
	dp = CHAR_getInt( charaindex, CHAR_DUELPOINT );
	fd = getfdFromCharaIndex( charaindex);
	if( fd != -1 ) {
		char info[512];
		//char escapebuff[1024];
		/* "脹撰��撓桵��撓吨��撓啖��撓蟀吨��郔詢蟀吨杅" */
		snprintf( info, sizeof( info),
					"%d,%d,%d,%d,%d,%d",
					CHAR_getInt( charaindex, CHAR_LV),
					CHAR_getInt( charaindex, CHAR_DUELBATTLECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELLOSECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELSTWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELMAXSTWINCOUNT));
		saacproto_DBUpdateEntryInt_send(
			acfd, DB_DUELPOINT, szKey, max(dp,0), info,
			CONNECT_getFdid( fd ) ,0 );
	}

	return TRUE;
}

char *CHAR_getUseName( int charaindex )
{
	char *pName=NULL;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
		pName = CHAR_getChar( charaindex, CHAR_USERPETNAME );
		if( pName[0] == 0 ){
			pName = CHAR_getChar( charaindex, CHAR_NAME );
		}
	}else{
//		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )
//			pName = CHAR_getChar( charaindex, CHAR_OWNTITLE );
//		if( pName == NULL || strlen(pName) <= 0 )
			pName = CHAR_getChar( charaindex, CHAR_NAME );
	}
	return pName;
}

// Robin extend_of_CHAR_getUseName() 2001/02/15  
char *CHAR_getUseID( int charaindex )
{
	char *pName;
	// 妐蚗哱煦埱倜�桹趹齱巋�↓堜勗埰堎
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
		pName = "NO_USER";
	}else{
		pName = CHAR_getChar( charaindex, CHAR_CDKEY );
	}
	return pName;
}

BOOL CHAR_send_DpDBUpdate_AddressBook( int charaindex, int mode )
{
	int fd;
	char szKey[256];
	if( CHAR_makeDBKey(  charaindex, szKey, sizeof( szKey ) ) == FALSE ){
		return FALSE;
	}

	fd = getfdFromCharaIndex( charaindex);
	if( fd != -1 ) {
		char msgbuf[512];
		snprintf( msgbuf, sizeof( msgbuf),
					"%d|%d|%d|%d|%d|",
					(mode == 0 ) ? 0 : getServernumber(),
					CHAR_getInt( charaindex, CHAR_LV),
					CHAR_getInt( charaindex, CHAR_DUELPOINT),
					CHAR_getInt( charaindex, CHAR_FACEIMAGENUMBER),
					CHAR_getInt( charaindex, CHAR_TRANSMIGRATION) );

		saacproto_DBUpdateEntryString_send(
			acfd, DB_ADDRESSBOOK, szKey, msgbuf, CONNECT_getFdid( fd ),0 );

		saacproto_Broadcast_send( acfd, CHAR_getChar( charaindex, CHAR_CDKEY),
									CHAR_getChar( charaindex, CHAR_NAME), "param", 0);

	}
	return TRUE;
}

void CHAR_sendBattleWatch( int objindex, int onoff)
{
	int		opt[1];
	opt[0] = onoff;
	CHAR_sendWatchEvent( objindex,CHAR_ACTBATTLEWATCH,opt,1,TRUE);
}
/*------------------------------------------------------------
 * 炩擭踏躂倜  禱蘆堇摯硞勗妐蚗哱隍↓鳴摯奿③禱埰堎��
 * 礎倳
 * charaindex        int     chara ぁ摯騷璃溢袲蚗筒
 * 蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
void CHAR_sendPMEToArroundCharacterFLXY( int petindex, int fl, int x, int y,
										int dir, int flg, int no )
{
	int     i,j;
	int		objindex;
	int		graphicsno;
	char	buff[2048];

	objindex = CHAR_getWorkInt( petindex, CHAR_WORKOBJINDEX);
	graphicsno = CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER);
	if( CHAR_makeObjectCString( objindex, buff, sizeof( buff)) == FALSE ) {
		buff[0] = '\0';
	}

	for( i = x - CHAR_DEFAULTSEESIZ/2 ;
		 i <= x +CHAR_DEFAULTSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_DEFAULTSEESIZ/2;
			 j <= y + CHAR_DEFAULTSEESIZ/2; j ++ ){

			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j);
				 object;
				 object = NEXT_OBJECT(object) ){
				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex(objindexwk);
					if( CHAR_getInt( index,CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
						int fd;
						if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE)
							!= BATTLE_CHARMODE_NONE) continue;
						fd = getfdFromCharaIndex( index);
						if( fd == -1 )continue;
						lssproto_PME_send( fd, objindex, graphicsno,
											x,y, dir,flg,no,buff
						 );
					}
				}
			}
		}
	}
}
/*------------------------------------------------------------
 * charaindex ぁ炩擭踏躂倜  禱蘆堇摯硞勗妐蚗哱隍↓鳴摯奿③禱埰堎��
 * 礎倳
 * charaindex        int     chara ぁ摯騷璃溢袲蚗筒
 * 蒍堇偯
 *  埵媃
 ------------------------------------------------------------*/
void CHAR_sendPMEToArroundCharacter( int charaindex, int petindex, int flg, int no )
{
    int dirx[9],diry[9];
    int		i;
    int		fl,x,y;
	int		dir;
    for( i  = 0 ; i < 7 ; i  ++ ){
        dirx[i+2] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
        diry[i+2] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
    }
    dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
    diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
    dirx[1] = 0;
    diry[1] = 0;

    fl = CHAR_getInt( charaindex,CHAR_FLOOR );
	dir = CHAR_getInt( charaindex, CHAR_DIR);

    /*   ��  澺噁   */
    for( i = 0 ; i < 9 ; i ++ ){
        int x = CHAR_getInt( charaindex, CHAR_X) + dirx[i];
        int y = CHAR_getInt( charaindex, CHAR_Y) + diry[i];
        /* 妐蚗哱禱  崹堎鳹鐎凝ぅ迋堎 */
        if( PET_isPutPoint( fl, x, y ) == TRUE ) {
            break;
        }
    }
    /* 埵崹躂�机櫅� */
    if( i == 9 ) i = 1;

    x = CHAR_getInt(charaindex,CHAR_X)+dirx[i];
    y = CHAR_getInt(charaindex,CHAR_Y)+diry[i];

	CHAR_sendPMEToArroundCharacterFLXY( petindex,
										fl, x, y, dir, flg, no);
}
/*------------------------------------------------------------
 * SE禱  �梊呁�
 *   嚭楁禱  刳勗�楱倡�堎  ⑹覕邞嚭埰堎
 * 礎倳
 * fl,x,y   嚭楁摯淢  
 * senumber     摯  蠕�菋ndcnf.h禱落敺摯喫敁��
 * sw   �梊尨涕掩忍職� 0 : 鰼誧堎 1 :   �梊�
 ------------------------------------------------------------*/
void CHAR_sendSEoArroundCharacter( int fl, int x, int y, int senumber, int sw )
{
	int     i,j;

	for( i = x - CHAR_DEFAULTSEESIZ/2 ;
		 i <= x +CHAR_DEFAULTSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_DEFAULTSEESIZ/2;
			 j <= y + CHAR_DEFAULTSEESIZ/2; j ++ ){

			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j);
				 object;
				 object = NEXT_OBJECT(object) ){
				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex(objindexwk);
					if( CHAR_getInt( index,CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
						int fd;
						if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE)
							!= BATTLE_CHARMODE_NONE) continue;
						fd = getfdFromCharaIndex( index);
						if( fd == -1 )continue;
						lssproto_SE_send( fd,x, y, senumber, sw );
					}
				}
			}
		}
	}
}
/*====================奿③优擭====================*/


#ifndef _SEND_EFFECT	   	        // WON ADD AC冞狟悕﹜狟迾脹杻虴

#define		CHAR_EFFECT_SETTINGBUFFER	256
typedef struct tagCHAR_effectsetting
{
    int     floor;								//	啞痲囮
	int		effect;								//	奿③  蠕
	int		level;								//	奿③摯畛穫鳴��  藯摯倯踏��
	int		sendflg;							//	  崚操啞攝袲哱禱邞埱倜凝汔竣凝��
	char	month[CHAR_EFFECT_SETTINGBUFFER];	//	  菜埰堎儈
	char	day[CHAR_EFFECT_SETTINGBUFFER];		//	  菜埰堎  
	char	hour[CHAR_EFFECT_SETTINGBUFFER];	//	  菜埰堎鄹蹬
	char	min[CHAR_EFFECT_SETTINGBUFFER];		//	  菜埰堎覕
	char	expire[CHAR_EFFECT_SETTINGBUFFER];	//	  菜媃趙笢堎荇踏��(    
	
}CHAR_effectsetting;

CHAR_effectsetting*    CHAR_effect;
int                     CHAR_effectnum;

#endif



/*------------------------------------------------------------
 * 奿③优擭摯疐趵撬禱埰堎��
 * 礎倳
 *  filename        char*       优擭啞栝騷鳴  
 * 蒍堇偯
 *  埬      TRUE(1)
 *  謄      FALSE(0)
 *------------------------------------------------------------*/
BOOL CHAR_initEffectSetting( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;

    int effectreadlen=0;

    f = fopen(filename,"r");
    if( f == NULL ){
        errorprint;
        return FALSE;
    }

    CHAR_effectnum=0;

    /*  竘囀  嗚埵菜誑笰菜堣堎凝汔竣凝ぅ迋堎    */
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        CHAR_effectnum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

    CHAR_effect = allocateMemory( sizeof(CHAR_effectsetting)
                                   * CHAR_effectnum );
    if( CHAR_effect == NULL ){
        fprint( "Can't allocate Memory %d\n" ,
                sizeof(CHAR_effectsetting)*CHAR_effectnum);
        fclose( f );
        return FALSE;
    }
    /* 疐趵撬 */
{
    int     i;
    for( i = 0; i < CHAR_effectnum; i ++ ) {
        CHAR_effect[i].floor = 0;
        CHAR_effect[i].effect = 0;
        CHAR_effect[i].level = 0;
		CHAR_effect[i].sendflg = 0;
        CHAR_effect[i].month[0] = '\0';
        CHAR_effect[i].day[0] = '\0';
        CHAR_effect[i].hour[0] = '\0';
        CHAR_effect[i].min[0] = '\0';
        CHAR_effect[i].expire[0] = '\0';
		
    }
}

    /*  竘倜  陑  埰    */
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        /*  菜禱堆魠埰堎    */
        /*  竘囀 tab 禱 " " 勗  拻儒窇堎    */
        replaceString( line, '\t' , ' ' );
        /*    襄媃倜 " " 禱痲敁僑勗埰堎 */
        deleteSequentChar( line, " " );

        /*  喫喫竘ぁ堆魠媃趙��袸  誑 筒妐↓筒煦埱倜�� */
        /*    竘埵笢    */
        if( line[0] == ' ' )continue;

        {
            char    token[256];
            int     ret;

            /*  痲敁僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",1,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }
            CHAR_effect[effectreadlen].floor = atoi(token);

            /*  2僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",2,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }

            CHAR_effect[effectreadlen].effect = atoi( token );

            /*  3僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",3,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }

            CHAR_effect[effectreadlen].level = atoi( token );
            
            /*  4僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",4,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].month, 
						sizeof( CHAR_effect[effectreadlen].month),
						token); 
            
            /*  5僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",5,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].day, 
						sizeof( CHAR_effect[effectreadlen].day),
						token); 
            
            /*  6僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",6,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].hour, 
						sizeof( CHAR_effect[effectreadlen].hour),
						token); 

            /*  7僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",7,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].min, 
						sizeof( CHAR_effect[effectreadlen].min),
						token); 
            
            /*  8僑誧摯哱↓袲璃禱峟堎    */
            ret = getStringFromIndexWithDelim( line," ",8,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("恅璃窏唗渣昫:%s 菴%d俴\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].expire, 
						sizeof( CHAR_effect[effectreadlen].expire),
						token); 

            effectreadlen ++;
        }
    }
    fclose(f);

    CHAR_effectnum = effectreadlen;

    print( "衄虴扢离軞杅 %d...", CHAR_effectnum );
#ifdef DEBUG

    {
        int i;
        for( i=0; i <CHAR_effectnum ; i++ )
            print( "effect fl[%d] effect[%d] level[%d] month[%d] day[%d] hour[%d] min[%d] expire[%d]\n",
                   CHAR_effect[i].floor,
                   CHAR_effect[i].x,
                   CHAR_effect[i].y,
                   CHAR_effect[i].month,
                   CHAR_effect[i].day,
                   CHAR_effect[i].hour,
                   CHAR_effect[i].min,
                   CHAR_effect[i].expire
                    );
    }
#endif
    return TRUE;
}

//-------------------------------------------------------------------------
//	藯凜鄹勤誑�椿黃羹匙紨鰡槤來擭鵗し胱俶職捌宦怛迗�埰��
//	譴埱趙笢躂�仈RUE禱蒍埰��
//-------------------------------------------------------------------------
static BOOL CHAR_checkEffectTime( int num)
{
	BOOL returnflg = FALSE;
	struct	tm	t;
	
	memcpy( &t, localtime( &NowTime.tv_sec), sizeof( struct tm));
	
	while( 1 ) {
		int	i;
		int ret;
		char token[256];
		
		//	"*"毀�椯撱紐峙勤呁�
		
		// 儈誑郖窙媃趙笢堎凝
		if( strcmp( CHAR_effect[num].month, "*" ) != 0 ) {
			BOOL flg = FALSE;
			for( i = 1; ; i ++) {
	            ret = getStringFromIndexWithDelim( CHAR_effect[num].month, 
	            									",", i, 
	            									token, sizeof( token));
	            if( ret == FALSE ) break;
				//	兒僑ぁ忒郖窙媃趙笢躂�伄孩嬴皮倗�
				if( t.tm_mon +1 == atoi( token)) {
					flg = TRUE;
					break;
				}
			}
			if( !flg ) break;
		}
		
		//   誑郖窙媃趙笢堎凝
		if( strcmp( CHAR_effect[num].day, "*" ) != 0 ) {
			BOOL flg = FALSE;
			for( i = 1; ; i ++) {
	            ret = getStringFromIndexWithDelim( CHAR_effect[num].day, 
	            									",", i, 
	            									token, sizeof( token));
	            if( ret == FALSE ) break;
				//	兒僑ぁ忒郖窙媃趙笢躂�伄孩嬴皮倗�
				if( t.tm_mday == atoi( token)) {
					flg = TRUE;
					break;
				}
			}
			if( !flg ) break;
		}
		// 鄹蹬誑郖窙媃趙笢堎凝
		if( strcmp( CHAR_effect[num].hour, "*" ) != 0 ) {
			BOOL flg = FALSE;
			for( i = 1; ; i ++) {
	            ret = getStringFromIndexWithDelim( CHAR_effect[num].hour, 
	            									",", i, 
	            									token, sizeof( token));
	            if( ret == FALSE ) break;
				//	兒僑ぁ忒郖窙媃趙笢躂�伄孩嬴皮倗�
				if( t.tm_hour == atoi( token)) {
					flg = TRUE;
					break;
				}
			}
			if( !flg ) break;
		}
		// 覕誑郖窙媃趙笢堎凝
		//	з踝埱敁煦崹  壓窐  
		if( strcmp( CHAR_effect[num].min, "*" ) != 0 ) {
			// 趵蹬誑↘埵�梛硉力K
			if( strcmp( CHAR_effect[num].expire, "*" ) != 0 ) {
				BOOL flg = FALSE;
				for( i = 1; ; i ++) {
		            struct tm tm_work;
		            time_t tim;
		            ret = getStringFromIndexWithDelim( CHAR_effect[num].min, 
		            									",", i, 
		            									token, sizeof( token));
		            if( ret == FALSE ) break;
					//	兒僑ぁ忒郖窙媃趙笢躂�伄孩嬴皮倗�

					//	优擭媃倜覕凝�掁�  嗚趵蹬摯  ⑹勗  埱趙笢堎凝ㄑ
					tm_work = t;
					tm_work.tm_sec = 0;
					tm_work.tm_min = atoi( token);
					tim = mktime( &tm_work);
					
					if( NowTime.tv_sec > tim && NowTime.tv_sec  < tim+atoi(CHAR_effect[num].expire)) {
						flg = TRUE;
						break;
					}
				}
				if( !flg ) break;
			}
		}
		// 覕誑炩擭  媃煦埱倜�梛硉力K
		
		returnflg = TRUE;
		break;
	}
	return returnflg;
}
//-------------------------------------------------------------------------
//	す籤溘袲淏↓摯痾  敁鄹蹬禱楁勗��
//	操啞攝袲哱禱邞堎  菺誑堣躂�夼肴�
//-------------------------------------------------------------------------
void CHAR_checkEffect( int charaindex)
{
	int	i;
	int fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	int fd = getfdFromCharaIndex( charaindex);
	
	// 操啞攝袲哱郖秠觛埰
	lssproto_EF_send( fd, 0, 0, "");
	CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, 0);

	for( i = 0; i < CHAR_effectnum; i ++){
		//	啞痲囮誑郖窙媃倜鄹煦崹
		if( CHAR_effect[i].floor == fl) {
			// 邳贀禱ぅ迋竘埰��
			if( CHAR_checkEffectTime( i)){
				// 操啞攝袲哱禱邞堎
				lssproto_EF_send( fd, CHAR_effect[i].effect, CHAR_effect[i].level, "");
				if( CHAR_effect[i].level == 0 ) {
					CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, 
									CHAR_getWorkInt( charaindex, CHAR_WORKEFFECT) &
										~CHAR_effect[i].effect);
				}
				else {
					CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, 
									CHAR_getWorkInt( charaindex, CHAR_WORKEFFECT) | 
										CHAR_effect[i].effect);
				}
			}
		}
	}
}

void CHAR_checkEffectLoop( void)
{
	int	i,j;
	int     playernum = CHAR_getPlayerMaxNum();
	int		player;
	int		efnum = 0;

	struct {
		int floor;
		int effect;
		BOOL on;
		BOOL off;
	} ef[2048];

	memset( &ef, 0, sizeof( ef));

	for( i = 0; i < CHAR_effectnum; i ++){

		if( CHAR_checkEffectTime( i)){
			if( !CHAR_effect[i].sendflg) {

				for( player = 0 ; player < playernum; player ++ ) {
					if( CHAR_getInt( player, CHAR_FLOOR) == CHAR_effect[i].floor ) {
						int fd = getfdFromCharaIndex( player);
						lssproto_EF_send( fd, CHAR_effect[i].effect, CHAR_effect[i].level, "");
						if( CHAR_effect[i].level == 0 ) {
							CHAR_setWorkInt( player, CHAR_WORKEFFECT, 
											CHAR_getWorkInt( player, CHAR_WORKEFFECT) &
												~CHAR_effect[i].effect);
						}
						else {					
							CHAR_setWorkInt( player, CHAR_WORKEFFECT, 
											CHAR_getWorkInt( player, CHAR_WORKEFFECT) | 
												CHAR_effect[i].effect);
						}
					}
				}
				CHAR_effect[i].sendflg = TRUE;
				
			}

			for( j = 0; j < efnum; j ++ ) {
				if( ef[j].floor == CHAR_effect[i].floor && 
					ef[j].effect == CHAR_effect[i].effect) 
				{
					ef[j].on = TRUE;
					break;
				}
			}
			if( j == efnum ) {
				ef[j].floor = CHAR_effect[i].floor;
				ef[j].effect = CHAR_effect[i].effect;
				ef[j].on = TRUE;
				efnum++;
				if( efnum >= arraysizeof( ef)) {
					print( "err buffer over %s:%d\n", __FILE__, __LINE__);
					efnum --;
				}
			}
		}
		else {
			if( CHAR_effect[i].sendflg) {

				for( j = 0; j < efnum; j ++ ) {
					if( ef[j].floor == CHAR_effect[i].floor && 
						ef[j].effect == CHAR_effect[i].effect) 
					{
						ef[j].off = TRUE;
						break;
					}
				}
				if( j == efnum ) {
					ef[j].floor = CHAR_effect[i].floor;
					ef[j].effect = CHAR_effect[i].effect;
					ef[j].off = TRUE;
					efnum++;
					if( efnum >= arraysizeof( ef)) {
						print( "err buffer over %s:%d\n", __FILE__, __LINE__);
						efnum --;
					}
				}
				CHAR_effect[i].sendflg = FALSE;
			}
		}
	}

	for( i = 0; i < efnum; i ++ ) {
		if( ef[i].on == FALSE && ef[i].off == TRUE ) {

			for( player = 0 ; player < playernum; player ++ ) {
				if( CHAR_getInt( player, CHAR_FLOOR) == ef[i].floor ) {
					int fd = getfdFromCharaIndex( player);
					lssproto_EF_send( fd, 0, 0, "");
					CHAR_setWorkInt( player, CHAR_WORKEFFECT, 0);
				}
			}
		}
	}
}

float GetRecoveryRate( int charaindex ){
	float fBai = 1.0;

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		fBai = 1.0 + 0.00010 * CHAR_getInt( charaindex, CHAR_VITAL );
	}else{

		fBai = 1.0 + 0.00005 * CHAR_getInt( charaindex, CHAR_VITAL );
	}
	return fBai;

}


#define CHARDATASIZE (1024*256)
#define SPACESTRING "|"

static int makeSaveCharString( char *out , int outlen ,
                               char *nm , char *opt , char *info )
{
    char nmwork[256];
    char optwork[512];
    char infowork[CHARDATASIZE];
    char outwork[CHARDATASIZE];
    char *nmwork_p , *optwork_p , *infowork_p;
    int l;
    
    strncpy( nmwork , nm , sizeof( nmwork ));
    nmwork[strlen( nm)]=0;

    strncpy( optwork , opt , sizeof( optwork ));
    optwork[strlen(opt)]=0;

    strncpy( infowork , info , sizeof( infowork ));
    infowork[strlen(info)]=0;

    nmwork_p = makeEscapeString( nm , nmwork ,sizeof( nmwork ));
    optwork_p = makeEscapeString( opt , optwork , sizeof( optwork ));
    infowork_p = makeEscapeString( info , infowork , sizeof( infowork ));

    snprintf( outwork ,sizeof( outwork ) ,
              "%s" SPACESTRING "%s" SPACESTRING "%s" ,
              nmwork_p , optwork_p , infowork_p );

    l = strlen( outwork );
	outwork[ strlen( outwork)+1] = 0;
    if( l >= (outlen-1)){
        return -1;
    }

    memcpy( out , outwork , l + 1 );
    
    return 0;
}

#ifdef _STORECHAR
int storeCharaData( void ){
	int i, charamax;
	FILE	*fp;
	struct tm *pLtime;
	char szFileName[256], *chardata;
	char outbuff[CHARDATASIZE];
	Char*   ch;

	print("\n悵湔堍俴笢腔杅擂\n");
	
	pLtime = localtime( &NowTime.tv_sec );

	charamax = getFdnum();

	for( i = 0; i < charamax; i ++ ){
		int hash, dir, j;
		char charId[32];
		char pathname[128];
		
		if( CHAR_getCharUse( i ) == FALSE )continue;

		strcpy( charId, CHAR_getChar( i, CHAR_CDKEY ) );
		print("梖瘍:%s", charId);
		
		hash = 0;
		for( j=0; j<strlen(charId); j++) {
			hash += (int)charId[j];
			hash = hash % 256;
		}

//		sprintf( pathname, "%s/char/0x%x", getStoredir(), hash);
		sprintf( pathname, "%s/0x%x", getStoredir(), hash);
//		print("恅璃繚噤:%s\n", pathname);
		dir = mkdir( pathname, -1);
		
		if( dir != 0 && errno != EEXIST )
			continue;
		//print("dir:%d\n", dir);

		sprintf( szFileName,
			"%s/%s.%d.char",
			pathname,
			CHAR_getChar( i, CHAR_CDKEY ),	// ID
			CHAR_getInt( i, CHAR_SAVEINDEXNUMBER )
		);
		
		print("\n湔揣:%s\n", szFileName);

		fp = fopen( szFileName, "w" );
		if( fp == NULL )continue;

		ch = CHAR_getCharPointer( i );
		if( !ch )continue;

		chardata = CHAR_makeStringFromCharData( ch );

		if( makeSaveCharString( outbuff , sizeof( outbuff ),
			 CHAR_getChar( i, CHAR_NAME ),
			 CHAR_makeOptionString( ch ), 
			 chardata ) == 0 
		){

			fprintf( fp, outbuff );
		}else{
//			fprintf( fp, "掛↓々謄  \n" );
		}

		fclose( fp );
#ifdef _CHAR_POOLITEM
	{
		if( !CHAR_CheckDepotItem( i) ) continue; //累踱帤湔婓
		sprintf( szFileName, "%s/%s.item", pathname, CHAR_getChar( i, CHAR_CDKEY ));
		if( (fp=fopen( szFileName, "w" )) == NULL )continue;
		print(" 湔揣:%s\n", szFileName);
		chardata = CHAR_makeDepotItemFromCharIndex( i);
		fprintf( fp, chardata );
		fclose( fp );
		CHAR_removeDepotItem( i);
	}
#endif

#ifdef _CHAR_POOLPET
	{
		if( !CHAR_CheckDepotPet( i) ) continue; //累踱帤湔婓
		sprintf( szFileName, "%s/%s.pet", pathname, CHAR_getChar( i, CHAR_CDKEY ));
		if( (fp=fopen( szFileName, "w" )) == NULL )continue;
		print(" 湔揣:%s\n", szFileName);
		chardata = CHAR_makeDepotPetFromCharIndex( i);
		fprintf( fp, chardata );
		fclose( fp );
		CHAR_removeDepotPet( i);
	}
#endif
	}

	/*
	if( execlp( getStorechar(), "" ) == -1 ) {
		print( " run %s error!:%d\n", getStorechar(), errno );
	}else {
		print( " run %s\n", getStorechar());
	}
	*/
	
	return 0;
}

#else

int storeCharaData( void ){
	int i, charamax;
	FILE	*fp;
	struct tm *pLtime;
	char szFileName[256], *chardata;
	char outbuff[CHARDATASIZE];
	Char*   ch;

	print(" run_storeCharaData ");
	
	pLtime = localtime( &NowTime.tv_sec );
  
	charamax = getFdnum();

	for( i = 0; i < charamax; i ++ ){

		if( CHAR_getCharUse( i ) == FALSE )continue;

		sprintf( szFileName, 
			"%s/%s_%d%02d%02d_%02d%02d", 
			getStoredir(),
			CHAR_getChar( i, CHAR_CDKEY ),	// ID
			pLtime->tm_year+1900,
			pLtime->tm_mon+1,
			pLtime->tm_mday,
			pLtime->tm_hour,
			pLtime->tm_min
		);

		fp = fopen( szFileName, "w" );
		if( fp == NULL )continue;

		ch = CHAR_getCharPointer( i );
		if( !ch )continue;

		chardata = CHAR_makeStringFromCharData( ch );

		if( makeSaveCharString( outbuff , sizeof( outbuff ),
			 CHAR_getChar( i, CHAR_NAME ),
			 CHAR_makeOptionString( ch ), 
			 chardata ) == 0 
		){

			fprintf( fp, outbuff );
		}else{
			fprintf( fp, "掛↓々謄  \n" );
		}

		fclose( fp );
	}
	return 0;
}

#endif

#ifdef _LASTERR_FUNCTION
DebugBreakPoint LastFunction[10];
static int lastfunctionNum = 0;
void LastFunction_Init()
{
	int i;
	for( i=0; i<10; i++)	{
		memset( LastFunction[i].FILE, 0, sizeof( char)*256);
		memset( LastFunction[i].funName, 0, sizeof( char)*256);
		LastFunction[i].line = -1;
	}
}
void LastFunction_Add( char *file, int line, char *funName)
{
	lastfunctionNum ++;
	if( lastfunctionNum >= 10 )
		lastfunctionNum = 0;
	
	strcpy( LastFunction[ lastfunctionNum].FILE, file);
	LastFunction[ lastfunctionNum].line = line;
	strcpy( LastFunction[ lastfunctionNum].funName, funName);
}

int LastFunction_Get( int lasts, char *file, int *line, char *funName)
{
	if( lasts < 0 || lasts >= 10 )
		return -1;
	strcpy( file, LastFunction[ lasts].FILE);
	*line = LastFunction[ lasts].line = line;
	strcpy( funName, LastFunction[ lasts].funName);
	return lastfunctionNum;
}
#endif



#ifdef _FIX_METAMORIDE
BOOL CHAR_CHECKJOINENEMY( int index)
{
	int masterindex=-1;
	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
		masterindex = CHAR_getWorkInt( index, CHAR_WORKPARTYINDEX1);
		if( !CHAR_CHECKINDEX( masterindex) ) {
			print( "err masterindex:%d %s:%d !!\n", masterindex,__FILE__, __LINE__);
			return FALSE;
		}
		if( CHAR_getInt( masterindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
			return TRUE;	//JOIN ENEMY
		}
	}

	return FALSE;
}
#endif

 // WON ADD 党淏耋撿腔扢隅恀枙
void fix_item_bug(int charaindex, int itemindex)
{

	if (ITEM_CHECKINDEX(itemindex)){		
		switch( ITEM_getInt(itemindex,ITEM_ID) ){
			case 19700:	ITEM_setChar( itemindex, ITEM_ARGUMENT, "KPET:901_902_903_904"); break; // 晝蘿
			case 20242:	ITEM_setChar( itemindex, ITEM_ARGUMENT, "KPET:957_958_959_960"); break; // 倯酴嬴
			case 1292:  ITEM_setInt( itemindex, ITEM_MAXDAMAGECRUSHE, 0); ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, 0); break; // 鍾薯霟
			case 19646: ITEM_setInt( itemindex, ITEM_MAXDAMAGECRUSHE, 0); ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, 0); break; // 鍾薯霟
			case 1478:	ITEM_setChar( itemindex, ITEM_ARGUMENT, "HP:30");	break;  // 醫親督
			case 20282: ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "馴+25 夼+25 鏗-4 黰+5");	ITEM_setInt( itemindex, ITEM_MODIFYQUICK, -4 );		break;  // 沺Л1
			case 20283: ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "馴+50 夼+50 鏗-10 黰+10");   ITEM_setInt( itemindex, ITEM_MODIFYQUICK, -10 );	break;  // 沺Л2
			case 20284: ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "馴+100 夼+100 鏗-20 黰+15"); ITEM_setInt( itemindex, ITEM_MODIFYQUICK, -20 );	break;  // 沺Л3
			case 1210:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "馴 +4 滅 -4 鏗 +4 黰 +2");		break;  // 捺迶腔坒忒遠
			case 1203:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "馴 +2 滅 +2 鏗 -2 黰 +1");		break;  // 々腔忒遠(綻)
			case 1204:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "馴 -2 滅 +2 鏗 +2 黰 +1");		break;  // 々腔忒遠(蟯)
			case 1205:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "馴 +2 滅 -2 鏗 +2 黰 +1");		break;  // 々腔忒遠(ч)
			case 1483:  ITEM_setInt( itemindex, ITEM_MODIFYCHARM, 5);	break;  // 踱伈湛瘍褒
			case 20147: ITEM_setChar(itemindex,ITEM_INGNAME0,"躂");	ITEM_setChar(itemindex,ITEM_INGNAME1,"嘎");	ITEM_setInt(itemindex,ITEM_INGVALUE0, 1125 );	ITEM_setInt(itemindex,ITEM_INGVALUE1, 1125 );	break;  // 邧芛脫
			default:
				break;
		}
	}
}


#ifdef _NPCSERVER_NEW
BOOL NPCSERVER_CreateObjindexFromServer( int fd, int npcindex, char *Name, int image,
										int dir, int floor, int x, int y)
{
        int     objindex;
        Object  obj;

        obj.type = OBJTYPE_NPCSCHARA;
        obj.index= -1;
		memset( obj.objname, 0, sizeof( obj.objname));
		memcpy( obj.objname, Name, strlen( Name)+1);
        obj.npcsindex = npcindex;
		obj.imagenum = image;
		obj.dir		= dir;
        obj.x       = x;
        obj.y       = y;
        obj.floor   = floor;
        objindex = initObjectOne( &obj );
        if( objindex == -1 ){
            return FALSE;
        }

        return TRUE;
}
#endif

#ifdef _PET_LOSTPET
BOOL CHAR_CharSaveLostPet( int petindex, int type)//華奻0 闊唾 1 唾蚘 2
{
	int lv, cost=0;
    char* CdKey=NULL;
	char* Uniquecode;
	char* petname;

	char lostpetstring[1024*2];
	char	*petstring;

	if( !CHAR_CHECKINDEX( petindex) ) return FALSE;
//湔��
	petstring = CHAR_makePetStringFromPetIndex( petindex);
	if( petstring == NULL ) return FALSE;
	CdKey = CHAR_getChar( petindex, CHAR_OWNERCDKEY);
	if( CdKey == NULL ) return FALSE;
	Uniquecode = CHAR_getChar( petindex, CHAR_UNIQUECODE);
	if( Uniquecode == NULL ) return FALSE;
	lv = CHAR_getInt( petindex, CHAR_LV);
	petname = CHAR_getChar( petindex, CHAR_NAME);

	if( type == 1 ){
		cost = lv*300;
	}else if( type == 2){
		cost = lv*1000;
	}else{
		cost = lv*10000;
	}

	sprintf( lostpetstring, "%s|%s|%d|%d|%s|%d#%s#%d",
		CdKey, petname, lv, cost, Uniquecode, (int)time( NULL), petstring, 
		type);

	print("悵湔郔綴唾昜:%s-%d\n", lostpetstring, strlen(lostpetstring));
	{
		char buf[10][2048]={"","","","","","","","","",""};
		char line[2048];
		int count=0, i, start=0;
		FILE *fp=NULL;
		char filename[256];

/*
		sprintf( filename, "lostpet/%s.txt", CdKey);
		if( (fp=fopen( filename, "a+")) != NULL ){
			fprintf( fp, "%s\n", petstring);
			fclose( fp);
		}else{
			print("Can't Add %s \n", filename);
		}

*/


		sprintf( filename, "lostpet/%s.txt", CdKey);
		if( (fp=fopen( filename, "r")) != NULL ){
			while( fgets( line , sizeof( line ) , fp ) && count < 7){
				if( strlen(line) <= 0 ) continue;
				strcpy( buf[ count++] , line);
			}
			fclose( fp);
		}else{
			print("Can't Read %s \n", filename);
		}

		if( (fp=fopen( filename, "w+")) == NULL ){
			print("Can't Write %s \n", filename);
			return FALSE;
		}
		if( strlen( buf[ 6]) > 0  ){
			start=2;
		}
		for( i=start; i<7; i++){
			if( strlen( buf[i]) > 0 ) {
				print("Write %s \n", buf[i]);
				fprintf( fp, "%s", buf[i]);
			}
		}
		fprintf( fp, "%s\n", lostpetstring);

		fclose( fp);
	}
	return TRUE;
}
/*
while( fgets( line , sizeof( line ) , fp ) && count < 7){
*/
#endif

#ifdef _ALLDOMAN
void InitHeroList( void)
{
	saacproto_UpdataStele_send ( acfd , "FirstLoad", "LoadHerolist" , "貌砱" , 0 , 0 , 0 , 999 ) ; 
}
#endif

#ifdef _STREET_VENDOR
void CHAR_sendStreetVendor(int charaindex,char *message)
{
	char szAction[2],szTemp[21],szMessage[4];
	int count = 0,i,j,iItemIndex = 0,iPetIndex = 0,iPileNum = 0,iMaxPileNum = 0;
	int ix,iy,iPlayerNum = 0,tofd = -1,objbuf[16];

	if(!getStringFromIndexWithDelim(message,"|",1,szAction,sizeof(szAction))) return;
	// 羲ゐ啊怉賡醱
	if(szAction[0] == 'O'){
		CHAR_getCoordinationDir(CHAR_getInt(charaindex,CHAR_DIR),CHAR_getInt(charaindex,CHAR_X),
														CHAR_getInt(charaindex,CHAR_Y),1,&ix,&iy);
		// �△蠅健談瞍疺�講
		iPlayerNum = CHAR_getSameCoordinateObjects(objbuf,arraysizeof(objbuf),CHAR_getInt(charaindex,CHAR_FLOOR),ix,iy);
		// 羶衄��
		if(iPlayerNum == 0){
			// 潰脤赻撩桴腔釱梓衄羶衄�刳紳痚确�
			iPlayerNum = CHAR_getSameCoordinateObjects(objbuf,arraysizeof(objbuf),
																								 CHAR_getInt(charaindex,CHAR_FLOOR),
																								 CHAR_getInt(charaindex,CHAR_X),
																								 CHAR_getInt(charaindex,CHAR_Y));
			// 祥硐赻撩珨跺��
			if(iPlayerNum > 1){
				// 潰脤梗�剆陏閨訄确�
				for(i=0;i<iPlayerNum;i++){
					int	objindex = objbuf[i];
					int	index = OBJECT_getIndex(objindex);
					
					if(OBJECT_getType(objindex) != OBJTYPE_CHARA) continue;
					if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) continue;
					if(index == charaindex) continue;
					// 勤源衄羶衄啊怉
					if(CHAR_getWorkInt(index,CHAR_WORKSTREETVENDOR) != 1) continue;
					// 衄�刱痚确�
					else{
						CHAR_talkToCli(charaindex,-1,"斕垀桴腔弇离眒冪衄�刱痚焮怉賸",CHAR_COLORYELLOW);
						return;
					}
				}
			}
			// 俙模羶衄婓蝠眢
			if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) return;
			// 俙模羶衄婓桵須
			if(CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;
			// 俙模羶衄郪勦
			if(CHAR_getWorkInt(charaindex,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) return;

			if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == -1){
				sprintf(szMessage,"O|");
				lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMessage);
			}
			// 眒啊怉筍猁党蜊啊怉囀��
			else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 1){
				// 党蜊囀�楺倅鞈妅敦确�
				CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
				CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
			}
			// 眒婓蝠眢笢筍猁蜊囀��,壽敕勤源弝敦
			else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 2){
				int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

				if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
					lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
					CHAR_talkToCli(toindex,-1,"虛模�＋�蝠眢",CHAR_COLORYELLOW);
					CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
					// ь壺蝠眢芞尨
					CHAR_sendTradeEffect(charaindex,0);
					CHAR_sendTradeEffect(toindex,0);
				}
				// 党蜊囀�楺�,扢傖帤啊怉
				CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
				CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
			}
		}
		// 衄��
		else{
			int	objindex,index,toindex;

			for(i=0;i<iPlayerNum;i++){
				objindex = objbuf[i];
				index = OBJECT_getIndex(objindex);
				
				if(OBJECT_getType(objindex) != OBJTYPE_CHARA) continue;
				if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) continue;
				if(index == charaindex) continue;
				// 勤源郪勦袨怓
				if(CHAR_getWorkInt(index,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) continue;
				// 勤源蝠眢袨怓
				if(CHAR_getWorkInt(index,CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) continue;
				tofd = getfdFromCharaIndex(index);
				if(tofd == -1) continue;
				toindex = CONNECT_getCharaindex(tofd);
				// 勤源衄啊怉
				if(CHAR_getWorkInt(toindex,CHAR_WORKSTREETVENDOR) == 1){
					// 赻撩婓啊怉笢麼啊怉蝠眢笢
					if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) > 0){
						if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 1){
							// 党蜊囀�楺倅鞈妅敦确�
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
						// 眒婓蝠眢笢筍猁蜊囀��,壽敕勤源弝敦
						else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 2){
							int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);
							
							if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
								CHAR_talkToCli(toindex,-1,"虛模�＋�蝠眢",CHAR_COLORYELLOW);
								CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								// ь壺蝠眢芞尨
								CHAR_sendTradeEffect(charaindex,0);
								CHAR_sendTradeEffect(toindex,0);
							}
							// 党蜊囀�楺�,扢傖帤啊怉
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
						return;
					}
					// 潰脤勤源岆瘁闖俇
					for(j=0;j<MAX_SELL_ITEM;j++)
						if(CHAR_getStreetVendor(toindex,j,SV_USAGE) == TRUE) break;
					// 闖俇賸
					if(j == MAX_SELL_ITEM){
						CHAR_talkToCli(charaindex,-1,"虛模陲昹闖俇賸",CHAR_COLORYELLOW);
						return;
					}
					// 邧源扢隅峈啊怉蝠眢笢(闖源蝠眢笢扢峈2,鎗源扢峈3)
					CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,3);
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,2);
					// 邧源扢隅蝠眢勤砓
					CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,toindex);
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,charaindex);
					// 扢隅峈蝠眢芞尨
					CHAR_sendTradeEffect(charaindex,1);
					CHAR_sendTradeEffect(toindex,1);
					// 參勤源毽闖囀�搥姜雥瞍�
					CHAR_sendStreetVendorDataToCli(charaindex,toindex);
				}
				// 勤源羶衄啊怉麼勤源淏婓啊怉蝠眢笢
				else{
					// 赻撩婓啊怉笢麼啊怉蝠眢笢
					if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) > 0){
						if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 1){
							// 党蜊囀�楺倅鞈妅敦确�
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
						// 眒婓蝠眢笢筍猁蜊囀��,壽敕勤源弝敦
						else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 2){
							int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);
							
							if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
								CHAR_talkToCli(toindex,-1,"虛模�＋�蝠眢",CHAR_COLORYELLOW);
								CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								// ь壺蝠眢芞尨
								CHAR_sendTradeEffect(charaindex,0);
								CHAR_sendTradeEffect(toindex,0);
							}
							// 党蜊囀�楺�,扢傖帤啊怉
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
					}
					// 扢隅傖啊怉
					else{
						sprintf(szMessage,"O|");
						lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMessage);
					}
					return;
				}
			}
		}
	}
	// 啊怉
	else if(szAction[0] == 'S'){
		int price;

		// ь壺垀衄導訧蹋
		for(i=0;i<MAX_SELL_ITEM;i++) CHAR_clearStreetVendor(charaindex,i);

		if(!getStringFromIndexWithDelim(message,"|",2,szTemp,sizeof(szTemp))) return;
		count = atoi(szTemp);
		
		for(i=0;i<count;i++){
			if(!getStringFromIndexWithDelim(message,"|",3+i*3,szTemp,sizeof(szTemp))) continue;
			CHAR_setStreetVendor(charaindex,i,SV_KIND,atoi(szTemp));
			if(!getStringFromIndexWithDelim(message,"|",4+i*3,szTemp,sizeof(szTemp))) continue;
			CHAR_setStreetVendor(charaindex,i,SV_INDEX,atoi(szTemp));
			if(!getStringFromIndexWithDelim(message,"|",5+i*3,szTemp,sizeof(szTemp))) continue;
			price = atoi(szTemp);
			if(price > 10000000) price = 10000000;
			CHAR_setStreetVendor(charaindex,i,SV_PRICE,price);
		}
		if(!getStringFromIndexWithDelim(message,"|",6+(i-1)*3,szTemp,sizeof(szTemp))) return;

		// 潰脤cli傷冞懂腔訧蹋
		for(i=0;i<MAX_SELL_ITEM;i++){
			if(CHAR_getStreetVendor(charaindex,i,SV_USAGE) == TRUE){
				// �襣З擰�
				if(CHAR_getStreetVendor(charaindex,i,SV_KIND) == 0){
					iItemIndex = CHAR_getItemIndex(charaindex,CHAR_getStreetVendor(charaindex,i,SV_INDEX));
					// 耋撿祥湔婓
					if(!ITEM_CHECKINDEX(iItemIndex)){
						CHAR_clearStreetVendor(charaindex,i);
						printf("\nCHAR_sendStreetVendor(S):梑祥善耋撿!!!!!!!\n");
						continue;
					}
					// 隍婓華奻頗秏囮腔耋撿祥褫毽闖
					if(ITEM_getInt(iItemIndex,ITEM_VANISHATDROP) == 1){
						CHAR_clearStreetVendor(charaindex,i);
						CHAR_talkToCli(charaindex,-1,"隍婓華奻頗秏囮腔耋撿祥褫毽闖ㄛ蜆恁砐�＋�",CHAR_COLORYELLOW);
						continue;
					}
					iPileNum = ITEM_getInt(iItemIndex,ITEM_USEPILENUMS);
					CHAR_setStreetVendor(charaindex,i,SV_PILE,iPileNum);
				}
				// �襣Ё駘�
				else if(CHAR_getStreetVendor(charaindex,i,SV_KIND) == 1){
					iPetIndex = CHAR_getCharPet(charaindex,CHAR_getStreetVendor(charaindex,i,SV_INDEX));
					// 唾昜祥湔婓
					if(!CHAR_CHECKINDEX(iPetIndex)){
						CHAR_clearStreetVendor(charaindex,i);
						printf("\nCHAR_sendStreetVendor(S):梑祥善唾昜!!!!!!!\n");
						continue;
					}
					if(CHAR_getInt(iPetIndex,CHAR_PETFAMILY) == 1){
						CHAR_talkToCli(charaindex,-1,"模逜忐誘忤拸楊毽闖ㄛ蜆恁砐�＋�",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
					if(CHAR_getInt(charaindex,CHAR_RIDEPET) == CHAR_getStreetVendor(charaindex,i,SV_INDEX)){
						CHAR_talkToCli(charaindex,-1,"る傚笢腔唾昜拸楊毽闖ㄛ蜆恁砐�＋�",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
					CHAR_setStreetVendor(charaindex,i,SV_PILE,1);
				}
				// 渣腔囀��
				else{
					CHAR_clearStreetVendor(charaindex,i);
					printf("\nCHAR_sendStreetVendor(S):渣腔囀��!!!!!!!\n");
				}
			}
		}
		
		CHAR_setWorkChar(charaindex,CHAR_STREETVENDOR_NAME,szTemp);
		CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,1);
		CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX),CHAR_STREETVENDOR_OPEN,NULL,0,TRUE);
	}
	// 彶怉
	else if(szAction[0] == 'E'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		// 彶怉奀�譥倞劼�婓鎗,籵眭�＋�
		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
			CHAR_talkToCli(toindex,-1,"虛模�＋�蝠眢",CHAR_COLORYELLOW);
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			// ь壺蝠眢芞尨
			CHAR_sendTradeEffect(charaindex,0);
			CHAR_sendTradeEffect(toindex,0);
		}
		CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
		CHAR_setWorkChar(charaindex,CHAR_STREETVENDOR_NAME,"");
		for(i=0;i<MAX_SELL_ITEM;i++) CHAR_clearStreetVendor(charaindex,i);
		CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX),CHAR_STREETVENDOR_CLOSE,NULL,0,TRUE);
	}
	// 鎗源祥鎗賸
	else if(szAction[0] == 'N'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){

			// 勤源扢隅峈啊怉笢
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,1);
			// ь壺勤源淏婓蝠眢笢腔靡等
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			// ь壺蝠眢芞尨
			CHAR_sendTradeEffect(charaindex,0);
			CHAR_sendTradeEffect(toindex,0);
		}
		CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
	}
	// 鎗源猁艘議跺毽闖昜腔砆牉訧蹋
	else if(szAction[0] == 'D'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
			int index;
			if(!getStringFromIndexWithDelim(message,"|",2,szTemp,sizeof(szTemp))) return;
			index = atoi(szTemp);
			CHAR_sendStreetVendorOneDataToCli(charaindex,toindex,index);
		}
		else printf("\nCHAR_sendStreetVendor():toindex error:%d",toindex);
	}
	// 鎗源冞懂猁鎗腔陲昹腔砐醴
	else if(szAction[0] == 'B'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		// 梑善闖源
		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
			int iBuyIndex = 0,iKind,iFieldIndex,iPrice,iRet;
			char szMsg[64];
			BOOL bPutToBank = FALSE;

			if(!getStringFromIndexWithDelim(message,"|",2,szTemp,sizeof(szTemp))) return;
			count = atoi(szTemp);
			for(i=0;i<count;i++){
				if(!getStringFromIndexWithDelim(message,"|",3+i,szTemp,sizeof(szTemp))) return;
				iBuyIndex = atoi(szTemp);
				iKind = CHAR_getStreetVendor(toindex,iBuyIndex,SV_KIND);
				iFieldIndex = CHAR_getStreetVendor(toindex,iBuyIndex,SV_INDEX);
				iPrice = CHAR_getStreetVendor(toindex,iBuyIndex,SV_PRICE);

				// 潰脤俙模旯奻ヴ劂祥劂
				if(CHAR_getInt(charaindex,CHAR_GOLD) < iPrice){
					CHAR_talkToCli(charaindex,-1,"斕旯奻坒啟祥劂﹝",CHAR_COLORRED);
					break;
				}
				bPutToBank = FALSE;
				// 潰脤闖源旯奻ヴ劂祥劂溫
				if(CHAR_getInt(toindex,CHAR_GOLD) + iPrice > CHAR_getMaxHaveGold(toindex)){
					// 旯奻祥劂溫,湔�賳鶲刳靇�
					if(CHAR_getInt(toindex,CHAR_BANKGOLD) + iPrice > CHAR_MAXBANKGOLDHAVE){
						CHAR_talkToCli(charaindex,-1,"虛模溫祥狟坒啟賸ㄛ蝠眢�＋�﹝",CHAR_COLORRED);
						CHAR_talkToCli(toindex,-1,"斕旯奻摯跺�刳靇迡瞈鍇捔�",CHAR_COLORRED);
						break;
					}
					CHAR_talkToCli(toindex,-1,"毽闖垀腕眒湔�賳鶲刳靇�",CHAR_COLORRED);
					bPutToBank = TRUE;
				}
				// �襣З擰�
				if(iKind == 0){
					iItemIndex = CHAR_getItemIndex(toindex,iFieldIndex);
					// 耋撿祥湔婓
					if(!ITEM_CHECKINDEX(iItemIndex)){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						printf("\nCHAR_sendStreetVendor(B):梑祥善耋撿!!!!!!!\n");
						continue;
					}
					// 隍婓華奻頗秏囮腔耋撿祥褫毽闖
					if(ITEM_getInt(iItemIndex,ITEM_VANISHATDROP) == 1){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						CHAR_talkToCli(charaindex,-1,"隍婓華奻頗秏囮腔耋撿祥褫毽闖ㄛ蜆恁砐�＋�",CHAR_COLORYELLOW);
						CHAR_talkToCli(toindex,-1,"淈聆善衄隍婓華奻頗秏囮腔耋撿婓毽闖ㄛ梛瘍眒暮翹ㄐ",CHAR_COLORRED);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(闖源蜊猾婦)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE)
						);
						continue;
					}
					// 祜歎昜こ祥夔闖
					if(iPrice == 0){
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(鎗源蜊猾婦)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE)
							);
						continue;
					}
					// 潰脤昜こ剽詁
					iPileNum = ITEM_getInt(iItemIndex,ITEM_USEPILENUMS);
					// 鎗源郔湮剽詁杅
					iMaxPileNum = CHAR_getMyMaxPilenum(charaindex);
					// �譠幮殿譯捄�杅掀旯奻腔郔湮剽詁杅屾,參闖源腔昜こь裁,參昜こ陔崝祫鎗源
					if(iPileNum - iMaxPileNum <= 0){
						if(!ITEM_CHECKINDEX(iItemIndex)){
							CHAR_talkToCli(charaindex,-1,"蝠眢囮啖(1)﹝",CHAR_COLORRED);
							break;
						}
#ifdef _ITEM_PILENUMS
						// 耋撿杅講峈0
						if(ITEM_getInt(iItemIndex,ITEM_USEPILENUMS) <= 0){
							CHAR_talkToCli(charaindex,-1,"蝠眢囮啖(2)﹝",CHAR_COLORRED);
							break;
						}
#endif
						// 陔崝鎗源腔
						if((iRet = CHAR_addItemSpecificItemIndex(charaindex,iItemIndex)) >= CHAR_MAXITEMHAVE){
							CHAR_talkToCli(charaindex,-1,"耋撿戲雛賸﹝",CHAR_COLORRED);
							break;
						}
						CHAR_sendItemDataOne(charaindex,iRet);
						// ь裁闖源腔
						CHAR_setItemIndex(toindex,iFieldIndex,-1);
						CHAR_sendItemDataOne(toindex,iFieldIndex);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						// 諶ヴ
						CHAR_setInt(charaindex,CHAR_GOLD,CHAR_getInt(charaindex,CHAR_GOLD) - iPrice);
						CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
						// 樓ヴ
						if(bPutToBank) CHAR_setInt(toindex,CHAR_BANKGOLD,CHAR_getInt(toindex,CHAR_BANKGOLD) + iPrice);
						else CHAR_setInt(toindex,CHAR_GOLD,CHAR_getInt(toindex,CHAR_GOLD) + iPrice);
						CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
						sprintf(szMsg,"耋撿 %s 蝠眢俇傖",ITEM_getChar(iItemIndex,ITEM_NAME));
						CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
						sprintf(szMsg,"%s 鎗軗賸耋撿 %s",CHAR_getUseName(charaindex),ITEM_getChar(iItemIndex,ITEM_NAME));
						CHAR_talkToCli(toindex,-1,szMsg,CHAR_COLORWHITE);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(耋撿)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE)
							);
					}
					else CHAR_talkToCli(charaindex,-1,"旯奻剽詁杅奻癹祥逋﹝",CHAR_COLORRED);
				}
				// �襣Ё駘�
				else if(iKind == 1){
					int iEmptyPetField;

					iPetIndex = CHAR_getCharPet(toindex,iFieldIndex);
					// 唾昜祥湔婓
					if(!CHAR_CHECKINDEX(iPetIndex)){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						printf("\nCHAR_sendStreetVendor(B):梑祥善唾昜!!!!!!!\n");
						continue;
					}
					if(CHAR_getInt(iPetIndex,CHAR_PETFAMILY) == 1){
						CHAR_talkToCli(charaindex,-1,"模逜忐誘忤拸楊毽闖ㄛ蜆恁砐�＋�",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(闖源蜊猾婦)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE)
						);
						continue;
					}
					if(CHAR_getInt(toindex,CHAR_RIDEPET) == CHAR_getStreetVendor(toindex,iBuyIndex,SV_INDEX)){
						CHAR_talkToCli(charaindex,-1,"る傚笢腔唾昜拸楊毽闖ㄛ蜆恁砐�＋�",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(闖源蜊猾婦)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE)
						);
						continue;
					}
						
					// 祜歎昜こ祥夔闖
					if(iPrice == 0){
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(鎗源蜊猾婦)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE)
							);
						continue;
					}
					// 潰脤俙模衄拸蚾掘拲忤賭硌,潰脤俙模衄羶衄蛌汜徹
					if(CHAR_getWorkInt(charaindex,CHAR_PickAllPet) == FALSE && CHAR_getInt(charaindex,CHAR_TRANSMIGRATION) < 1){
						// 潰脤俙模脹撰衄羶衄掀唾昜詢
						if(CHAR_getInt(iPetIndex,CHAR_LV) > (CHAR_getInt(charaindex,CHAR_LV) + 5)){
							CHAR_talkToCli(charaindex,-1,"斕拸楊桽嘈蜆唾昜﹝",CHAR_COLORRED);
							continue;
						}
					}
					// 眕狟岆唾昜蝠遙
					iEmptyPetField = CHAR_getCharPetElement(charaindex);
					// 旯奻衄諾戲弇
					if(iEmptyPetField > -1){
						// ь闖源
						CHAR_setCharPet(toindex,iFieldIndex,-1);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						sprintf(szMsg,"K%d",iFieldIndex);
						CHAR_sendStatusString(toindex,szMsg);
						// 扢隅鎗源
						CHAR_setCharPet(charaindex,iEmptyPetField,iPetIndex);
						CHAR_setWorkInt(iPetIndex,CHAR_WORKPLAYERINDEX,charaindex);
						CHAR_setChar(iPetIndex,CHAR_OWNERCDKEY,CHAR_getChar(charaindex,CHAR_CDKEY));
						CHAR_setChar(iPetIndex,CHAR_OWNERCHARANAME,CHAR_getChar(charaindex,CHAR_NAME));
						CHAR_complianceParameter(iPetIndex);
						sprintf(szMsg,"K%d",iEmptyPetField);
						CHAR_sendStatusString(charaindex,szMsg);
						sprintf(szMsg,"W%d",iEmptyPetField);
						CHAR_sendStatusString(charaindex,szMsg);
						// 諶ヴ
						CHAR_setInt(charaindex,CHAR_GOLD,CHAR_getInt(charaindex,CHAR_GOLD) - iPrice);
						CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
						// 樓ヴ
						if(bPutToBank) CHAR_setInt(toindex,CHAR_BANKGOLD,CHAR_getInt(toindex,CHAR_BANKGOLD) + iPrice);
						else CHAR_setInt(toindex,CHAR_GOLD,CHAR_getInt(toindex,CHAR_GOLD) + iPrice);
						CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
						sprintf(szMsg,"%s 鎗軗賸唾昜 %s",CHAR_getUseName(charaindex),CHAR_getChar(iPetIndex,CHAR_NAME));
						CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
						sprintf(szMsg,"唾昜 %s 蝠眢俇傖ㄐ",CHAR_getChar(iPetIndex,CHAR_NAME));
						CHAR_talkToCli(toindex,-1,szMsg,CHAR_COLORWHITE);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							CHAR_getChar(iPetIndex,CHAR_NAME),
							CHAR_getInt(iPetIndex,CHAR_LV),
							iPrice,
							"StreetVendor(唾昜)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE)
							);
					}
					// 俙模唾昜戲弇雛賸
					else CHAR_talkToCli(charaindex,-1,"旯奻唾昜戲戲弇祥逋ㄐ",CHAR_COLORRED);
				}
				// 渣腔囀��
				else{
					CHAR_clearStreetVendor(toindex,iBuyIndex);
					printf("\nCHAR_sendStreetVendor(B):渣腔囀��!!!!!!!\n");
				}
			}
			// 闖源扢隅峈啊怉,ь壺鎗源袨怓
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,1);
			// ь壺蝠眢芞尨
			CHAR_sendTradeEffect(charaindex,0);
			CHAR_sendTradeEffect(toindex,0);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
		}
		else printf("StreetVendor蝠眢梑祥善勤源(%s)\n",CHAR_getChar(charaindex,CHAR_NAME));
	}
}

void CHAR_sendStreetVendorDataToCli(int charaindex,int toindex)
{
	int i,count = 0;

	// 冞毽闖囀�搛雥瞍眢埮�,猁党蜊毽闖囀��
	if(charaindex == toindex){
		char szMsg[512],szTemp[512],szTemp1[16];

		memset(szTemp,0,sizeof(szTemp));
		for(i=0;i<MAX_SELL_ITEM;i++){
			if(CHAR_getStreetVendor(charaindex,i,SV_USAGE) == TRUE){
				sprintf(szTemp1,"%d|%d|%d|",
								CHAR_getStreetVendor(charaindex,i,SV_KIND),
								CHAR_getStreetVendor(charaindex,i,SV_INDEX),
								CHAR_getStreetVendor(charaindex,i,SV_PRICE));
				strcat(szTemp,szTemp1);
				count++;
			}
		}
		sprintf(szMsg,"S|%d|%s",count,szTemp);
		lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMsg);
	}
	// 冞毽闖湮砐睿菴珨跺毽闖昜腔牉砐囀�搛讕羆�
	else{
		int kind,index,itempetindex,firstindex = -1;
		char szName[32],szFreeName[32];
		char szMsg[1024],szTemp[1024],szTemp1[128];

		memset(szTemp,0,sizeof(szTemp));
		for(i=0;i<MAX_SELL_ITEM;i++){
			if(CHAR_getStreetVendor(toindex,i,SV_USAGE) == TRUE){
				kind = CHAR_getStreetVendor(toindex,i,SV_KIND);
				index = CHAR_getStreetVendor(toindex,i,SV_INDEX);
				// �襣З擰�
				if(kind == 0){
					itempetindex = CHAR_getItemIndex(toindex,index);
					// 耋撿祥湔婓
					if(!ITEM_CHECKINDEX(itempetindex)){
						CHAR_clearStreetVendor(toindex,i);
						printf("\nCHAR_sendStreetVendorDataToCli():梑祥善耋撿!!!!!!!\n");
						continue;
					}
					sprintf(szName,"%s",ITEM_getChar(itempetindex,ITEM_NAME));
					// szFreeName 婓耋撿奀岆蚚懂湔毽闖腔杅講
					sprintf(szFreeName,"%d",CHAR_getStreetVendor(toindex,i,SV_PILE));
				}
				// 唾昜
				else if(kind == 1){
					itempetindex = CHAR_getCharPet(toindex,index);
					// 唾昜祥湔婓
					if(!CHAR_CHECKINDEX(itempetindex)){
						CHAR_clearStreetVendor(toindex,i);
						printf("\nCHAR_sendStreetVendorDataToCli():梑祥善唾昜!!!!!!!\n");
						continue;
					}
					sprintf(szName,"%s",CHAR_getChar(itempetindex,CHAR_NAME));
					sprintf(szFreeName,"%s",CHAR_getChar(itempetindex,CHAR_USERPETNAME));
				}
				// 渣腔囀��
				else{
					CHAR_clearStreetVendor(toindex,i);
					printf("\nCHAR_sendStreetVendorDataToCli():渣昫囀��!!!!!!!\n");
					continue;
				}
				if(firstindex == -1) firstindex = i;

				sprintf(szTemp1,"%d|%d|%s|%s|%d|",kind,CHAR_getStreetVendor(toindex,i,SV_PRICE),szName,szFreeName,i);
				strcat(szTemp,szTemp1);
				count++;
			}
		}
		sprintf(szMsg,"B|%d|%s",count,szTemp);
		lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMsg);
		if(firstindex == -1) firstindex = 0;
		CHAR_sendStreetVendorOneDataToCli(charaindex,toindex,firstindex);
	}
}

void CHAR_sendStreetVendorOneDataToCli(int charaindex,int toindex,int sendindex)
{
	int kind,index,itempetindex,i,count = 0;
	char szMsg[512],szTemp[512],szTemp1[256];

	if(CHAR_getStreetVendor(toindex,sendindex,SV_USAGE) == TRUE){
		kind = CHAR_getStreetVendor(toindex,sendindex,SV_KIND);
		index = CHAR_getStreetVendor(toindex,sendindex,SV_INDEX);

		memset(szTemp,0,sizeof(szTemp));
		memset(szTemp1,0,sizeof(szTemp1));
		// �襣З擰�
		if(kind == 0){
			int crushe,maxcrushe,itemcolor;

			itempetindex = CHAR_getItemIndex(toindex,index);
			if(ITEM_CHECKINDEX(itempetindex)){
				// 佽隴
				sprintf(szTemp1,"%s|",ITEM_getChar(itempetindex,ITEM_EFFECTSTRING));
				strcat(szTemp,szTemp1);
				// 騵壅僅
				crushe = ITEM_getInt(itempetindex,ITEM_DAMAGECRUSHE);
				maxcrushe = ITEM_getInt(itempetindex,ITEM_MAXDAMAGECRUSHE);
				
				if(crushe < 1) crushe = 1;
				if(maxcrushe < 1) sprintf(szTemp1,"祥頗囷輓|");
				else{
					maxcrushe = maxcrushe/1000;
					crushe = crushe/1000;
					if(maxcrushe <= 0) maxcrushe = 1;
					sprintf(szTemp1,"%d%%|",(int)((crushe*100)/maxcrushe));
				}
				strcat(szTemp,szTemp1);
				// 恅趼晇伎
				itemcolor = CHAR_COLORWHITE;
				if(strlen(ITEM_getChar(itempetindex,ITEM_CDKEY)) != 0) itemcolor = CHAR_COLORGREEN;
				else if(ITEM_getInt(itempetindex,ITEM_MERGEFLG)) itemcolor = CHAR_COLORYELLOW;
				sprintf(szTemp1,"%d|",itemcolor);
				strcat(szTemp,szTemp1);
				// 芞瘍
				sprintf(szTemp1,"%d|",ITEM_getInt(itempetindex,ITEM_BASEIMAGENUMBER));
				strcat(szTemp,szTemp1);
			}
			// 梑祥善耋撿
			else{
				CHAR_clearStreetVendor(toindex,sendindex);
				printf("\nCHAR_sendStreetVendorOneDataToCli():梑祥善耋撿!!!!!!!\n");
				return;
			}
		}
		// 唾昜
		else if(kind == 1){
			int skillid,petskillindex;

			itempetindex = CHAR_getCharPet(toindex,index);
			if(CHAR_CHECKINDEX(itempetindex)){
				// 唾撮
				for(i=0;i<CHAR_MAXPETSKILLHAVE;i++){
					skillid = CHAR_getPetSkill(itempetindex,i);
					petskillindex = PETSKILL_getPetskillArray(skillid);
					if(PETSKILL_CHECKINDEX(petskillindex)){
						sprintf(szTemp,"%s|",PETSKILL_getChar(petskillindex,PETSKILL_NAME));
						strcat(szTemp1,szTemp);
						count++;
					}
				}
				sprintf(szTemp,"%d|%s",count,szTemp1);
				// 脹撰,悛,馴,滅,鏗,侐扽俶,笳剴僅
				sprintf(szTemp1,"%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
					CHAR_getInt(itempetindex,CHAR_LV),CHAR_getWorkInt(itempetindex,CHAR_WORKMAXHP),CHAR_getWorkInt(itempetindex,CHAR_WORKATTACKPOWER),
					CHAR_getWorkInt(itempetindex,CHAR_WORKDEFENCEPOWER),CHAR_getWorkInt(itempetindex,CHAR_WORKQUICK),CHAR_getInt(itempetindex,CHAR_EARTHAT),
					CHAR_getInt(itempetindex,CHAR_WATERAT),CHAR_getInt(itempetindex,CHAR_FIREAT),CHAR_getInt(itempetindex,CHAR_WINDAT),
					CHAR_getWorkInt(itempetindex,CHAR_WORKFIXAI));
				strcat(szTemp,szTemp1);
				// 芞瘍
				sprintf(szTemp1,"%d|",CHAR_getInt(itempetindex,CHAR_BASEBASEIMAGENUMBER));
				strcat(szTemp,szTemp1);
			}
			// 梑祥善唾昜
			else{
				CHAR_clearStreetVendor(toindex,sendindex);
				printf("\nCHAR_sendStreetVendorOneDataToCli():梑祥善唾昜!!!!!!!\n");
				return;
			}
		}
		sprintf(szMsg,"D|%d|%s",sendindex,szTemp);
		lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMsg);
		printf("\nCHAR_sendStreetVendorOneDataToCli():%s",szMsg);
	}
	else printf("\nCHAR_sendStreetVendorOneDataToCli():諾腔囀��!!!!!!!\n");
}
#endif


BOOL checkUnlawWarpFloor(int floor) // 潰脤輦砦俙模誑眈換冞華⑹
{
	
	if( floor == 887 // 桸渾垀
		|| floor == 117 // 潼郜
		|| floor == 1042 || floor == 2032 || floor == 3032
		|| floor == 4032 || floor == 5032 || floor == 6032
		|| floor == 7032 || floor == 8032 || floor == 9032 // 蚽埶桵
		|| floor == 8015 || floor == 8027
		|| floor == 8028 || floor == 8029 // 濡眳漆�恄�
		|| floor == 154 || floor == 155 || floor == 156 || floor == 157
		|| floor == 8519 || floor == 8520 
		|| floor == 8513 //華檣 
#ifdef _TIME_TICKET
		|| check_TimeTicketMap( floor)
#endif
#ifdef _UNLAW_WARP_FLOOR
		|| floor == getUnlawwarpfloor(0)
		|| floor == getUnlawwarpfloor(1)
		|| floor == getUnlawwarpfloor(2)
		|| floor == getUnlawwarpfloor(3)
		|| floor == getUnlawwarpfloor(4)
#endif
	)
	{ 
		return TRUE;
	}

	return FALSE;
}

#ifdef _JOBDAILY
int JobDailyEventCheck( int meindex, int talker, char *buff1);
BOOL JobDailyRuleCheck(int talker, char* buf);
DailyFileType dailyfile[MAXDAILYLIST];
//羲傷
void CHAR_JobDaily(int charaindex,char *data)
{
	int nowflag[MAXMISSIONFLAG];
	int endflag[MAXMISSIONFLAG];
	int nowtotal = 0;
	int endtotal = 0;
	int i,j=0,k=0;
	char szMsg[16384];
	char szToken[1024];
	int evch,temp,lens;
	char tmpbuf[50],tmpbuf1[50];

	memset(nowflag,0,sizeof(nowflag));
	memset(endflag,0,sizeof(endflag));
	memset(szMsg,0,sizeof(szMsg));
	memset(tmpbuf,0,sizeof(tmpbuf));
	memset(tmpbuf1,0,sizeof(tmpbuf1));

	if(strcmp(data,"dyedye")!=0){
		CHAR_talkToCli( charaindex, -1, "賤鎢渣昫", CHAR_COLORYELLOW);
		return;
	}

	for(i=0;i<1000;i++)
	{
		temp = atoi(dailyfile[i].jobid);
		if(temp<=0) break;

		evch = JobDailyEventCheck( 0 , charaindex, dailyfile[i].rule );
		if(evch != -1){
			szToken[0] = '\0';
			print("\n lens:%d",strlen(dailyfile[i].explain));
			if(strlen(dailyfile[i].explain)>=48){   
				for(j=0;j<strlen(dailyfile[i].explain);j++){
					if(j<48){
						tmpbuf[j] =	dailyfile[i].explain[j];
					}else{
						tmpbuf1[j-48] = dailyfile[i].explain[j];
					}
				}
				print("tb1:%s",tmpbuf);
				print("tb2:%s",tmpbuf1);
				sprintf( szToken,"%s|%s|%s|#|%s||#",dailyfile[i].jobid,tmpbuf,dailyfile[i].state,tmpbuf1);
				strcat(szMsg,szToken);
			}else{
				sprintf( szToken,"%s|%s|%s|#",dailyfile[i].jobid,dailyfile[i].explain,dailyfile[i].state);
				strcat(szMsg,szToken);
			}
		}
	}
	print("\n szMsg:%s",szMsg);
	lssproto_JOBDAILY_send(getfdFromCharaIndex(charaindex),szMsg);
}

//黍堤沭璃宒
int JobDailyEventCheck( int meindex, int talker, char *buff1)
{
	char buff2[512];
	char buff3[128];
	int  i=1,j=1;
	BOOL endcheck = TRUE;

	while( getStringFromIndexWithDelim(buff1, ",", i, buff2,sizeof( buff2))
		!=FALSE )
	{
		i++;
		if(strstr( buff2, "&")!=NULL){
			j = 1;
			endcheck = TRUE;
			while(getStringFromIndexWithDelim(buff2, "&", j, buff3, sizeof( buff3))
			 != FALSE )
			{
				j++;
				if(JobDailyRuleCheck( talker, buff3) == FALSE)
				{
					endcheck = FALSE;
					break;
				}
			}
			if(endcheck){
				i--;
				return i;
			}
		}else{
			if(JobDailyRuleCheck( talker, buff2) == TRUE){
				i--;
				return i;
			}
		}
	}
	return -1;
}

//瓚剿�恄鮿麇縎倛�
BOOL JobDailyRuleCheck(int talker, char* buf)
{
	char buf1[20]="";
	char buf2[20]="";
	int  eventNo;

	getStringFromIndexWithDelim( buf, "=", 2, buf1, sizeof( buf1));
	eventNo = atoi(buf1);
	if(eventNo >= 256){
		print("�恄鮵欶噩饑恄鮿麇糗頞來倛�%d\n",eventNo);
		return FALSE;
	}
	print("\n eventNo:%d ",eventNo);
	print("buf:%s ",buf); 
	getStringFromIndexWithDelim( buf, "=", 1, buf2, sizeof( buf2));

	if(strcmp( buf2, "ENDEV") == 0) {
		print("seee");
		if(NPC_EventCheckFlg( talker , eventNo) == TRUE){
			return TRUE;
		}
	}

	if(strcmp( buf2, "NOWEV") == 0) {
		print("seee");
		if(NPC_NowEventCheckFlg( talker , eventNo) == TRUE){
			return TRUE;
		}
	}
	
	return FALSE;
}
#endif

#ifdef _TEACHER_SYSTEM
void CHAR_Teacher_system(int charaindex,char *data)
{
	char szAction[2];
	int i,ix,iy,iPlayerNum,objbuf[16];
	BOOL bHasTeacher = FALSE;
	char szMsg[1024];

	if(!getStringFromIndexWithDelim(data,"|",1,szAction,sizeof(szAction))) return;
	switch(szAction[0]){
		case 'P':
			{
				// 潰脤赻撩岆瘁眒冪衄絳呇賸
				if(strlen(CHAR_getChar(charaindex,CHAR_TEACHER_ID)) > 0 && strlen(CHAR_getChar(charaindex,CHAR_TEACHER_NAME)) > 0) bHasTeacher = TRUE;
				// 潰脤淏ヶ源衄羶衄��
				CHAR_getCoordinationDir(CHAR_getInt(charaindex,CHAR_DIR),CHAR_getInt(charaindex,CHAR_X),
					CHAR_getInt(charaindex,CHAR_Y),1,&ix,&iy);
				// �△蠅健談瞍疺�講
				iPlayerNum = CHAR_getSameCoordinateObjects(objbuf,arraysizeof(objbuf),CHAR_getInt(charaindex,CHAR_FLOOR),ix,iy);
				// 羶衄��
				if(iPlayerNum == 0){
					// 眒衄絳呇,珆尨絳呇訧蹋
					if(bHasTeacher){
						// 砃 ac 猁訧蹋
						saacproto_ACCheckCharacterOnLine_send(acfd,charaindex,CHAR_getChar(charaindex,CHAR_TEACHER_ID),
																									CHAR_getChar(charaindex,CHAR_TEACHER_NAME),
																									R_F_TEACHER_SYSTEM);
					}
					// 羶衄絳呇,珆尨佽隴
					else lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),"M|");
				}
				// 衄��
				else{
					// 眒衄絳呇
					if(bHasTeacher){
						// ③俙模珂�＋�埻掛腔絳呇婬輛俴梑陔絳呇雄釬
						CHAR_talkToCli(charaindex,-1,"③珂�＋�埻掛腔絳呇婬梑む坻�匋龕秈蒫警暱�",CHAR_COLORRED);
					}
					else{
						int	objindex,index;
						char szFindName[256];
						
						if(iPlayerNum == 1){
							// 戙恀岆瘁猁勤源絞斕腔絳呇
							objindex = objbuf[0];
							index = OBJECT_getIndex(objindex);
							
							if(OBJECT_getType(objindex) != OBJTYPE_CHARA) break;
							if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) break;
							if(index == charaindex) break;
							// 潰脤勤源岆瘁峈赻撩腔悝汜
							if(strcmp(CHAR_getChar(index,CHAR_TEACHER_ID),CHAR_getChar(charaindex,CHAR_CDKEY)) == 0 &&
								 strcmp(CHAR_getChar(index,CHAR_TEACHER_NAME),CHAR_getChar(charaindex,CHAR_NAME)) == 0){
								CHAR_talkToCli(charaindex,-1,"勤源峈斕腔悝汜ㄛ拸楊�繹埸蔔妅秈蒫警暱�",CHAR_COLORRED);
								break;
							}
							sprintf(szMsg,"C|%s|%d",CHAR_getChar(index,CHAR_NAME),index);
							lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),szMsg);
						}
						// 閉徹珨��,戙恀猁梑阰絞絳呇
						else{
							int iGetNum = 0;
							char szBuf[1024];

							memset(szBuf,0,sizeof(szBuf));
							for(i=0;i<iPlayerNum && i<5;i++){
								objindex = objbuf[i];
								index = OBJECT_getIndex(objindex);
								
								if(OBJECT_getType(objindex) != OBJTYPE_CHARA) continue;
								if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) continue;
								if(index == charaindex) continue;
								sprintf(szFindName,"%s|%d|",CHAR_getChar(index,CHAR_NAME),index);
								strcat(szBuf,szFindName);
								iGetNum++;
							}
							sprintf(szMsg,"A|%d|%s",iGetNum,szBuf);
							lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),szMsg);
						}
					}
				}
			}
			break;
		// �毓乖傀冼玥暱�
		case 'O':
			{
				char szBuf[16];
				int index;

				getStringFromIndexWithDelim(data,"|",2,szBuf,sizeof(szBuf));
				index = atoi(szBuf);
				if(!CHAR_CHECKINDEX(index)){
					printf("\nCHAR_Teacher_system:error index (%s:%d)\n",CHAR_getChar(charaindex,CHAR_NAME),index);
					sprintf(szMsg,"蜆俙模祥湔婓(%d)",index);
					CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORRED);
					break;
				}
				CHAR_setChar(charaindex,CHAR_TEACHER_ID,CHAR_getChar(index,CHAR_CDKEY));
				CHAR_setChar(charaindex,CHAR_TEACHER_NAME,CHAR_getChar(index,CHAR_NAME));
				sprintf(szMsg,"%s 眒傖峈斕腔悝汜ㄐ",CHAR_getChar(charaindex,CHAR_NAME));
				CHAR_talkToCli(index,-1,szMsg,CHAR_COLORWHITE);
				sprintf(szMsg,"%s 眒傖峈斕腔絳呇ㄐ",CHAR_getChar(index,CHAR_NAME));
				CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
			}
			break;
		// �＋�絳呇訧跡
		case 'C':
			{
				int iCharm = CHAR_getInt(charaindex,CHAR_CHARM) - 30;

				CHAR_setChar(charaindex,CHAR_TEACHER_ID,"");
				CHAR_setChar(charaindex,CHAR_TEACHER_NAME,"");
				// 諶黰薯硉
				CHAR_setInt(charaindex,CHAR_CHARM,iCharm < 0 ? 0:iCharm);
				CHAR_setWorkInt(charaindex,CHAR_WORKFIXCHARM,iCharm < 0 ? 0:iCharm);
				CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_CHARM);
			}
			break;
		default:printf("\nCHAR_Teacher_system:error command (%s)\n",&szAction[0]);
	}
}

void CHAR_Teacher_system_View(int charaindex,int iOnLine,char *data)
{
	char szMsg[1024];

	// V|絳呇俷靡|婓祥婓盄奻|垀婓陎炵
	sprintf(szMsg,"V|%s|%d|%s",CHAR_getChar(charaindex,CHAR_TEACHER_NAME),iOnLine,data);
	lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),szMsg);
}
#endif

#ifdef _TIME_TICKET

int check_TimeTicketMap(int floor)
{
	if( floor == 7025) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void check_TimeTicket()
{
	int	i;
	int tickettime;
	int	playernum = CHAR_getPlayerMaxNum();
	int nowtime = time(NULL);
	char msg[1024];

	for( i =0; i <playernum; i++) {
		
		if( !CHAR_CHECKINDEX(i) )	continue;

		if( check_TimeTicketMap( CHAR_getInt( i, CHAR_FLOOR)) == FALSE) {
			CHAR_setWorkInt( i, CHAR_WORKTICKETTIME, 0);
			CHAR_setWorkInt( i, CHAR_WORKTICKETTIMESTART, 0);
			continue;
		}
		
		tickettime = CHAR_getWorkInt( i, CHAR_WORKTICKETTIME);
		// 試�貐�
		if( tickettime == 0 ) {
			//CHAR_talkToCli( i, -1, "③婓ㄠㄟ鏃囀妏蚚藷き﹝", CHAR_COLORYELLOW);
			//CHAR_setWorkInt( i, CHAR_WORKTICKETTIME, nowtime+10);
		}
		else
		// 奀癹20鏃眕奻
		if( tickettime > 0 && tickettime > nowtime+20 ) {
			continue;
		}
		// 奀癹祥善20鏃
		else if( tickettime < nowtime+20 && tickettime >= nowtime ) {
			sprintf( msg, "奀潔き呁牄奀潔%d鏃﹝", tickettime - nowtime);
			CHAR_talkToCli( i, -1, msg, CHAR_COLORYELLOW);
		}

		// 閉徹奀癹
		else if( tickettime < nowtime ) {
			int floor, x, y;
			int totaltime;
			char msg[1024];
			if( CHAR_getWorkInt( i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
				CHAR_talkToCli( i, -1, "奀潔眒善ㄛ郅郅嫖嘈﹝", CHAR_COLORYELLOW);
				if( CHAR_getWorkInt( i, CHAR_WORKTICKETTIMESTART) > 0 ) {
					totaltime = nowtime - CHAR_getWorkInt( i, CHAR_WORKTICKETTIMESTART);
					sprintf( msg, "軞�貐﹋掉�%d煦%d鏃﹝", totaltime/60, totaltime%60);
					CHAR_talkToCli( i, -1, msg, CHAR_COLORYELLOW);
				}
				CHAR_setWorkInt( i, CHAR_WORKTICKETTIME, 0);
				CHAR_setWorkInt( i, CHAR_WORKTICKETTIMESTART, 0);
				/*
				if( CHAR_getElderPosition( CHAR_getInt( i, CHAR_LASTTALKELDER), &floor, &x, &y ) != TRUE ) {
					CHAR_warpToSpecificPoint( i, 1006, 15, 15);
				}
				else {
					CHAR_warpToSpecificPoint( i, floor, x, y);
				}
				*/
				CHAR_warpToSpecificPoint( i, 7001, 41, 6);
			}
			else {
				CHAR_talkToCli( i, -1, "奀潔眒善ㄛ蔚黺桵須賦旰奀燭羲﹝", CHAR_COLORYELLOW);
				// 痄祫 BATTLE_Exit() 揭燴
			}
		}
	}

}
#endif
