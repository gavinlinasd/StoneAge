#include "version.h"
#define __NPCTEMPLATE__

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "npctemplate.h"
#include "buf.h"
#include "char_data.h"
#include "util.h"
#include "handletime.h"
#include "item.h"
#include "anim_tbl.h"
#include "configfile.h"

/*É¬ÀÃÃ«¿ÃÇ©±åÔÊÔÂÐ×»§±åèúÐÑÀÃñø±¾ÓÀÐþÃ«×ÛÔÂ    */
typedef struct tagFunctionNameSet
{
    char*   id;
    char*   initfunc;
    char*   walkprefunc;
    char*   walkpostfunc;
    char*   preoverfunc;
    char*   postoverfunc;
    char*   watchfunc;
    char*   loopfunc;
    char*   dyingfunc;
    char*   talkedfunc;
    char*   preattackedfunc;
    char*   postattackedfunc;
    char*   offfunc;
    char*   lookedfunc;
    char*   itemputfunc;
    char*   specialtalkedfunc;
    char*   windowtalkedfunc;
}FunctionNameSet;


static FunctionNameSet  functionSet[]={

    { "Sample" , "", "", "",  "", "", "", "SampleLoop",
      "SampleDying" ,"","", "","","","","",""},

	/* ÓñÊ§ */
    { "Door" , "DoorInit" , "","",
      "","DoorPostOver","DoorWatch","",
      "","DoorTalked","", "",
      "DoorOff","DoorLooked","","",""},

	/* ÷Ò */
    { "SimpleShop" ,"SimpleShopInit","","",
      "","","","","",
      "SimpleShopTalked" , "","",
      "","","" ,"SimpleShopSpecialTalked" ,""},

	/* ¶ªÓÀ±¾¡õ³â */
    { "Msg" , "MsgInit" , "","",
      "","","","",
      "","","", "",
      "","MsgLooked","","",""},

	/* ·¥¡õÃóÔý¡õ¼þ */
    { "Warp","WarpInit","","",
      "","","","",
      "","","","",
      "","", "","",""},

	/*   ¼°ÚÐ */
    { "TownPeople" ,"TownPeopleInit","","",
      "","","","",
      "", "TownPeopleTalked" ,"","",
      "","","","",""},

	/* Ó®   */
    { "Oldman","OldmanInit","","",
      "","","","",
      "","OldmanTalked","","",
      "","", "","",""},


	/* ±¾¡õÆ¤ºÌÄÌ¼þÐþ */
    { "SavePoint","SavePointInit","","",
      "","","","",
      "","SavePointTalked","","",
      "","", "","","SavePointWindowTalked"},

	/* ¼×¡õ·Â¡õ */
    { "Healer","HealerInit","","",
      "","","","",
      "","HealerTalked","","",
      "","", "","",""},

        /* ¼Ò×åHealer add code by shan */  
    { "FmHealer","FmHealerInit","","",
      "","","","",
      "","FmHealerTalked","","",
      "","", "","",""},
    
        /* °×»¢¸øÓèÕß add code by shan */  
    { "PetMaker","PetMakerInit","","",
      "","","","",
      "","PetMakerTalked","","",
      "","", "","",""},    
    
	/* »ßÔ»   */
    { "StoryTeller","StoryTellerInit","","",
      "","","","",
      "","StoryTellerTalked","","",
      "","", "","",""},

	/* æØ  Ç²ºÐ */
    { "RoomAdminNew","RoomAdminNewInit","","",
      "","","","RoomAdminNewLoop",
      "","RoomAdminNewTalked","","",
      "","", "","",""},

	/* ÷©ÍÉ   */
    {"Dengon",
     "DengonInit", "", "", "", "", "", "", "", "",
     "", "", "", "DengonLooked", "", "","DengonWindowTalked"},
     
     	/* ¼Ò×åÁôÑÔ°å add code by shan */
    {"FmDengon",
     "FmDengonInit", "", "", "", "", "", "", "", "",
     "", "", "", "FmDengonLooked", "", "","FmDengonWindowTalked"},

	/*É¿í³Ü°µÑÊ§ÄÌ  Ø©ÄþÔÀ*/
    { "ItemCompo" , "ItemCompoInit", "", "",
      "", "", "", "",
      "" ,"ItemCompoTalked","",""
	  ,"","","" , "",""},

	/* ÒáÀÃ³Ä */
    { "NPCEnemy","NPCEnemyInit","","",
      "","","NPCEnemyWatch","",
      "","NPCEnemyTalked","","",
      "","", "","","NPCEnemyWindowTalked"},

	/* Ê§ÛÍÆËÒà¼þé¬ */
    { "Action","ActionInit","","",
      "","","ActionWatch","",
      "","ActionTalked","","",
      "","", "","",""},

	/*   åÃËüÅ«¼þÓñËüNPC */
    { "Windowman","WindowmanInit","","",
      "","","","",
      "","WindowmanTalked","","",
      "","WindowmanLooked", "","","WindowmanWindowTalked"},

	/*windowhealer  */
    { "WindowHealer","WindowHealerInit","","",
      "","","","",
      "","WindowHealerTalked","","",
      "","WindowHealerLooked", "","","WindowHealerWindowTalked"},


	/* ItemShop  */
    { "ItemShop","ItemShopInit","","",
      "","","","",
      "","ItemShopTalked","","",
      "","", "","","ItemShopWindowTalked"},

    {"Sysinfo",
     "SysinfoInit", "", "",
     "", "", "", "SysinfoLoop",
     "", "SysinfoTalked","", "",
     "", "", "", "",""},

	/* Duel·Â¼þÆ½¼þºë  Ô÷NPC */
    { "Duelranking","DuelrankingInit","","",
      "","","",
#ifdef _DEATH_CONTEND
	  "Duelrankingloop",
#else
	  "",
#endif
      "","","","",
      "","DuelrankingLooked", "","","DuelrankingWindowTalked"},


	/* PetSkillShop  */
    { "PetSkillShop","PetSkillShopInit","","",
      "","","","",
      "","PetSkillShopTalked","","",
      "","", "","","PetSkillShopWindowTalked"},

	/* PetShop  */
    { "PetShop","PetShopInit","","",
      "","","","",
      "","PetShopTalked","","",
      "","", "","","PetShopWindowTalked"},


	/* SignBoard  */
    { "SignBoard","SignBoardInit","","",
      "","","","",
      "","","","",
      "","SignBoardLooked", "","","SignBoardWindowTalked"},


	/* WarpMan  */
    { "WarpMan","WarpManInit","","",
      "","","WarpManWatch","WarpManLoop",
      "","WarpManTalked","","",
      "","", "","","WarpManWindowTalked"},


	/* ExChangeman  */
    { "ExChangeMan","ExChangeManInit","","",
      "","","","",
      "","ExChangeManTalked","","",
      "","", "","","ExChangeManWindowTalked"},

	/* timeman  */
    { "TimeMan","TimeManInit","","",
      "","","TimeManWatch","",
      "","TimeManTalked","","",
      "","", "","",""},

	/* Ê¾·¸Å«·Â¼þ±Ø¡õ³â */
    { "BodyLan","BodyLanInit","","",
      "","","BodyLanWatch","",
      "","BodyLanTalked","","",
      "","", "","","BodyLanWindowTalked"},

	/* Ê¾·¸Å«·Â¼þ±Ø¡õ³â */
    { "Mic","MicInit","","",
      "","","","",
      "","MicTalked","","",
      "","", "","",""},

	/* ·ÂÓÀÆ½¡õÑ¨¼þ */
    { "LuckyMan","LuckyManInit","","",
      "","","","",
      "","LuckyManTalked","","",
      "","", "","","LuckyManWindowTalked"},

	/* Ñ¨¼þÆ¹µ©Ìïµ©é¬ */
    { "Bus","BusInit","","",
      "","","","BusLoop",
      "","BusTalked","","",
      "","", "","",""},

	/* ¼ÓÃÀº½¿Õ */	// Arminius 7.7 Ariplane
    { "Airplane","AirInit","","",
      "","","","AirLoop",
      "","AirTalked","","",
      "","", "","",""},
	
	/*         é¬ */
    { "Charm","CharmInit","","",
      "","","","",
      "","CharmTalked","","",
      "","", "","","CharmWindowTalked"},

	/* ÛÍÄÌÊõé¬ */
    { "Quiz","QuizInit","","",
      "","","","",
      "","QuizTalked","","",
      "","", "","","QuizWindowTalked"},

	/* Ê§ÄÌ  Ø©  Ô»ºÐ½ñÊÏ  */
    { "PoolItemShop","PoolItemShopInit","","",
      "","","","PoolItemShopLoop",
      "","PoolItemShopTalked","","",
      "","", "","","PoolItemShopWindowTalked"},

	/* ÄÌÃ¬¼þÐþÃñÄáÓÀÛÍÑ¨¼þ */
    { "CheckMan","CheckManInit","","",
      "","","","",
      "","CheckManTalked","","",
      "","", "","","CheckManWindowTalked"},

	/* ÔªÔúÊÏØêÊÏé¬ */
    { "Janken","JankenInit","","",
      "","","","",
      "","JankenTalked","","",
      "","", "","","JankenWindowTalked"},

	/* ±îÏ·ÚÐ */
    { "Transmigration","TransmigrationInit","","",
      "","","","",
      "","TransmigrationTalked","","",
      "","", "","","TransmigrationWindowTalked"},

        /* Family Man */
    { "Familyman","FamilymanInit","","",
      "","","","",
      "","FamilymanTalked","","",
      "","FamilymanLooked", "","","FamilymanWindowTalked"},
             
	/* CoolFish: Family Warp Man 2001/6/6 */
    { "FMWarpMan","FMWarpManInit","","",
      "","","","FMWarpManLoop",
      "","FMWarpManTalked","","",
      "","", "","","FMWarpManWindowTalked"},

	/* CoolFish: Family PK Man 2001/7/4 */
    { "FMPKMan","FMPKManInit","","",
      "","","","",
      "","FMPKManTalked","","",
      "","", "","","FMPKManWindowTalked"},

	/* CoolFish: Family PK CallMan 2001/7/13 */
    { "FMPKCallMan","FMPKCallManInit","","",
      "","","","",
      "","FMPKCallManTalked","","",
      "","", "","","FMPKCallManWindowTalked"},

        /* Bank Man */
    { "Bankman","BankmanInit","","",
      "","","","",
      "","BankmanTalked","","",
      "","BankmanLooked", "","","BankmanWindowTalked"},

	/* Arminius 7.13 scheduleman */
    { "Scheduleman","SchedulemanInit","","",
      "","","","SchedulemanLoop",
      "","SchedulemanTalked","","",
      "","", "","","SchedulemanWindowTalked"},

	/* Arminius 7.24 manor scheduleman */
    { "ManorSman","ManorSmanInit","","",
      "","","","ManorSmanLoop",
      "","ManorSmanTalked","","",
      "","", "","","ManorSmanWindowTalked"},

	// Robin
    { "Riderman","RidermanInit","","",
      "","","","",
      "","RidermanTalked","","",
      "","RidermanLooked", "","","RidermanWindowTalked"},

    { "FmLetter","FmLetterInit","","",
      "","","","",
      "","FmLetterTalked","","",
      "","FmLetterLooked", "","","FmLetterWindowTalked"}
    // Terry 2001/08/30
    // Ê¯Æ÷·þÎñÔ± StoneSreviceMan
#ifdef _SERVICE    
    ,{
      "StoneServiceMan","StoneServiceManInit","","","","","",
      "StoneServiceManLoop","",
      "StoneServiceManTalked","","","","","","",
      "StoneServiceManWindowTalked"
    }
#endif
#ifdef _GAMBLE_BANK
	,{
      "NPC_GambleBank","GambleBankInit","","","","","",
      "GambleBankLoop","",
      "GambleBankTalked","","","","","","",
      "GambleBankWindowTalked"
    }
#endif 

#ifdef _GAMBLE_ROULETTE	//Gamble_Roulette
	,{
      "NPC_GambleRoulette","GambleRouletteInit","","","","","",
      "GambleRouletteLoop","",
      "GambleRouletteTalked","","","","","","",
	  "GambleRouletteWindowTalked"
	}
	//Ö÷³ÖÈË
	,{
      "NPC_GambleMaster","GambleMasterInit","","","","","",
      "GambleMasterLoop","",
      "GambleMasterTalked","","","","","","",
	  "GambleMasterWindowTalked"
	}

#endif

#ifdef _TRANSER_MAN
	/* TranserMan  */
    ,{ "TranserMan","TranserManInit","","",
      "","","","TranserManLoop",
      "","TranserManTalked","","",
      "","", "","","TranserManWindowTalked"
	}
#endif

#ifdef _NPC_SELLSTH
	/* SellsthMan  */
    ,{ "SellsthMan","SellsthManInit","","",
      "","","","SellsthManLoop",
      "","SellsthManTalked","","",
      "","", "","","SellsthManWindowTalked"
	}
#endif

#ifdef _NPC_MAKEPAIR
    ,{ "MakePair","MakePairManInit","","",
      "","","","MakePairManLoop",
      "","MakePairManTalked","","",
      "","", "","","MakePairManWindowTalked"
	}
#endif

#ifdef _NPC_FUSION
	,{ "PetFusion","PetFusionManInit","","",
      "","","","PetFusionManLoop",
      "","PetFusionManTalked","","",
      "","", "","","PetFusionManWindowTalked"
	}
#endif

#ifdef _PAUCTION_MAN
	/* PauctionMan  */
    ,{ "PauctionMan","PauctionManInit","","",
      "","","","PauctionManLoop",
      "","PauctionManTalked","","",
      "","", "","","PauctionManWindowTalked"
	}
#endif

#ifdef _ITEM_NPCCHANGE
    ,{ "ItemchangeMan","ItemchangeManInit","","",
      "","","","ItemchangeManLoop",
      "","ItemchangeManTalked","","",
      "","", "","","ItemchangeManWindowTalked"
	}
#endif

#ifdef _CFREE_petskill
	,{
      "NPC_FreePetSkill","FreePetSkillInit","","","","","",
      "","",
      "FreePetSkillTalked","","","","","","",
	  "FreePetSkillWindowTalked"
	}

#endif

#ifdef _PETRACE
	//Ö÷³ÖÈË
	,{
      "PetRaceMaster","PetRaceMasterInit","","","","","",
      "PetRaceMasterLoop","",
      "PetRaceMasterTalked","","","","","","",
	  "PetRaceMasterWindowTalked"
	}
	// ³èÎï
	,{
      "PetRacePet","PetRacePetInit","","","","","",
      "PetRacePetLoop","",
      "PetRacePetTalked","","","","","","",""
	}
#endif

#ifdef _NEW_WARPMAN
	,{
      "NPC_NewNpcMan","NewNpcManInit","","","","","",
      "NewNpcManLoop","",
      "NewNpcManTalked","","","","","","",
      "NewNpcManWindowTalked"
    }
#endif

#ifdef _MARKET_TRADE
	,{
      "NPC_MTradeNpcMan","MapTradeManInit","","","","","",
      "MapTradeManLoop","",
      "MapTradeManTalked","","","","","","",
      "MapTradeManWindowTalked"
    }
#endif

#ifdef _BLACK_MARKET	   
    ,{
		"BlackMarket",
		"BlackMarketInit","","","","","","","","BlackMarketTalked","","",
        "","", "","","BlackMarketWindowTalked"
	}
#endif

#ifdef _ALLDOMAN   // (²»¿É¿ª) Syu ADD ÅÅÐÐ°ñNPC
	,{
		"Alldoman","AlldomanInit","","",
      "","","","","","AlldomanTalked","","","","","","",
	  "AlldomanWindowTalked"}
#endif

#ifdef _NPC_WELFARE
	,{ "Welfare","WelfareInit","","",
      "","","","",
      "","WelfareTalked","","",
      "","", "","","WelfareWindowTalked"}
#endif

#ifdef _NPC_VERYWELFARE
	,{ "VeryWelfare","VeryWelfareInit","","",
      "","","","",
      "","VeryWelfareTalked","","",
      "","", "","","VeryWelfareWindowTalked"}
#endif

#ifdef _NPC_WELFARE_2				// WON ADD Ö°ÒµNPC-2
	,{ "Welfare2","WelfareInit2","","",
      "","","","",
      "","WelfareTalked2","","",
      "","", "","","WelfareWindowTalked2"}
#endif

#ifdef _RACEMAN
    ,{"Raceman","RacemanInit", "", "",
		"", "", "", "",
		"", "RacemanTalked", "", "",
		"", "", "", "","RacemanWindowTalked"},
#endif

};

static int NPC_searchFunctionSet( char* name, int* unuse)
{
    int     i;
    for( i=0 ; i <arraysizeof( functionSet ) ; i++ )
        if( strcasecmp( name, functionSet[i].id ) == 0 )
            return i;
    return -1;
}

BOOL NPC_copyFunctionSetToChar( int id, Char* ch )
{
    if( id<0 || id>=arraysizeof(functionSet) )return FALSE;

    strcpysafe( ch->charfunctable[CHAR_INITFUNC].string,
                sizeof( ch->charfunctable[CHAR_INITFUNC].string ),
                functionSet[id].initfunc);

    strcpysafe( ch->charfunctable[CHAR_WALKPREFUNC].string,
                sizeof(ch->charfunctable[CHAR_WALKPREFUNC].string),
                functionSet[id].walkprefunc);

    strcpysafe( ch->charfunctable[CHAR_WALKPOSTFUNC].string,
                sizeof(ch->charfunctable[CHAR_WALKPOSTFUNC].string),
                functionSet[id].walkpostfunc);

    strcpysafe( ch->charfunctable[CHAR_PREOVERFUNC].string,
                sizeof(ch->charfunctable[CHAR_PREOVERFUNC].string),
                functionSet[id].preoverfunc);

    strcpysafe( ch->charfunctable[CHAR_POSTOVERFUNC].string,
                sizeof(ch->charfunctable[CHAR_POSTOVERFUNC].string),
                functionSet[id].postoverfunc);

    strcpysafe( ch->charfunctable[CHAR_WATCHFUNC].string,
                sizeof(ch->charfunctable[CHAR_WATCHFUNC].string),
                functionSet[id].watchfunc);

    strcpysafe( ch->charfunctable[CHAR_LOOPFUNC].string,
                sizeof(ch->charfunctable[CHAR_LOOPFUNC].string),
                functionSet[id].loopfunc);

    strcpysafe( ch->charfunctable[CHAR_TALKEDFUNC].string,
                sizeof(ch->charfunctable[CHAR_TALKEDFUNC].string),
                functionSet[id].talkedfunc);

    strcpysafe( ch->charfunctable[CHAR_DYINGFUNC].string,
                sizeof(ch->charfunctable[CHAR_DYINGFUNC].string),
                functionSet[id].dyingfunc);

    strcpysafe( ch->charfunctable[CHAR_PREATTACKEDFUNC].string,
                sizeof(ch->charfunctable[CHAR_PREATTACKEDFUNC].string),
                functionSet[id].preattackedfunc);
    strcpysafe( ch->charfunctable[CHAR_POSTATTACKEDFUNC].string,
                sizeof(ch->charfunctable[CHAR_POSTATTACKEDFUNC].string),
                functionSet[id].postattackedfunc);

    strcpysafe( ch->charfunctable[CHAR_OFFFUNC].string,
                sizeof(ch->charfunctable[CHAR_OFFFUNC].string),
                functionSet[id].offfunc );

    strcpysafe( ch->charfunctable[CHAR_LOOKEDFUNC].string,
                sizeof(ch->charfunctable[CHAR_LOOKEDFUNC].string),
                functionSet[id].lookedfunc );

    strcpysafe( ch->charfunctable[CHAR_ITEMPUTFUNC].string,
                sizeof(ch->charfunctable[CHAR_ITEMPUTFUNC].string),
                functionSet[id].itemputfunc );

    strcpysafe( ch->charfunctable[CHAR_SPECIALTALKEDFUNC].string,
                sizeof(ch->charfunctable[CHAR_SPECIALTALKEDFUNC].string),
                functionSet[id].specialtalkedfunc );

    strcpysafe( ch->charfunctable[CHAR_WINDOWTALKEDFUNC].string,
                sizeof(ch->charfunctable[CHAR_WINDOWTALKEDFUNC].string),
                functionSet[id].windowtalkedfunc );

#ifdef _USER_CHARLOOPS
    strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
                sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP1].string), "");

    strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP2].string,
                sizeof(ch->charfunctable[CHAR_LOOPFUNCTEMP2].string), "");
#endif
    return TRUE;
}


typedef enum
{
    NPC_INTENTRY=0,
    NPC_CHARENTRY=1,
    NPC_INTFUNC=2,
    NPC_CHARFUNC=3,
}NPC_TYPECATEGORYATREADFILE;

/*------------------------------------------------------------
 * Template index ÎçØÆ»¯¿ÒØÆÖÐ¾®Éýµ¤¾®checkÔÊÔÂ
 * Â¦ÐÑ
 *  index       int             index
 * ß¯Ô»°À
 *  valid       TRUE
 *  invalid     FALSE
 ------------------------------------------------------------*/
INLINE int NPC_CHECKTEMPLATEINDEX(int index)
{
    if(  NPC_templatenum <= index || index < 0 )return FALSE;
    return TRUE;
}
/*------------------------------------------------------------
 * Template intdata index ÎçØÆ»¯¿ÒØÆÖÐ¾®Éýµ¤¾®checkÔÊÔÂ
 * Â¦ÐÑ
 *  index     int             index
 * ß¯Ô»°À
 *  valid       TRUE
 *  invalid     FALSE
 ------------------------------------------------------------*/
static INLINE int NPC_CHECKTEMPLATEINTINDEX(int index)
{
    if(  NPC_TEMPLATEINTNUM <= index || index < 0 )return FALSE;
    return TRUE;
}
/*------------------------------------------------------------
 * Template chardata index ÎçØÆ»¯¿ÒØÆÖÐ¾®Éýµ¤¾®checkÔÊÔÂ
 * Â¦ÐÑ
 *  index     int             index
 * ß¯Ô»°À
 *  valid       TRUE
 *  invalid     FALSE
 ------------------------------------------------------------*/
static INLINE int NPC_CHECKTEMPLATECHARINDEX(int index)
{
    if(  NPC_TEMPLATECHARNUM <= index || index < 0 )return FALSE;
    return TRUE;
}



/*------------------------------------------------------------
 * NPC_TemplateÃ«âÙÓå¼ÀÔÊÔÂ£Û
 * Â¦ÐÑ
 *  filenum    int      °×ÑëÄÌ»ïÐÑ
 * ß¯Ô»°À
 *  ÔÀ      TRUE(1)
 *  ÁÃ      FALSE(0)
 ------------------------------------------------------------*/
BOOL NPC_initTemplateArray( int templatenum )
{
    NPC_templatenum = templatenum;
    NPC_template = (NPC_Template*)allocateMemory( sizeof( NPC_Template ) *
                                                  NPC_templatenum );
    if( NPC_template == NULL )return FALSE;
    NPC_template_readindex = 0;
    return TRUE;
}

/*------------------------------------------------------------
 * ·¸°×ÇÉ»ïÐþ¼°NPC  ¼þÃóÒÁ¡õÐþÃ«Ò½  ÔÊÔÂ£Û
 * Â¦ÐÑ
 *  temp    NPC_Template*     ¼þÃóÒÁ¡õÐþ¼°ºÌÄÌ¼þÕý¡õ
 * ß¯Ô»°À
 *  Ø¦ØÆ
 ------------------------------------------------------------*/
void NPC_setDefaultNPCTemplate( NPC_Template*  temp )
{
    int i;
    if( temp == NULL )return;

    for( i=0 ; i< arraysizeof(temp->chardata) ; i ++ )
        memset( &temp->chardata[i], 0, sizeof(temp->chardata[i] ));

    temp->intdata[NPC_TEMPLATEMAKEATNOBODY]=0;
    temp->intdata[NPC_TEMPLATEMAKEATNOSEE]=0;
    temp->intdata[NPC_TEMPLATEIMAGENUMBER]=0;
    temp->intdata[NPC_TEMPLATETYPE]=-1;

    temp->intdata[NPC_TEMPLATEMINHP]=0;
    temp->intdata[NPC_TEMPLATEMINMP]=0;
    temp->intdata[NPC_TEMPLATEMINSTR]=0;
    temp->intdata[NPC_TEMPLATEMINTOUGH]=0;

    temp->intdata[NPC_TEMPLATEISFLYING]=0;

    temp->intdata[NPC_TEMPLATEITEMNUM]=0;
    temp->intdata[NPC_TEMPLATELOOPFUNCTIME]=-1;
    temp->intdata[NPC_TEMPLATEFUNCTIONINDEX]=-1;

    for( i=0; i<NPC_TEMPLATEINTNUM; i++){
        temp->randomdata[i] = 0;
    }

    temp->hash = 0;
    temp->haveitem=NULL;
}

BOOL NPC_IsNPCTemplateFile( char* filename )
{
    FILE*   f;
    char    line1[128];
    char*   ret;
    if( filename == NULL
        ||strlen( filename ) < 1
        ||filename[strlen(filename)-1] == '~'
        ||filename[0] == '#'
        ||strcmptail( filename, ".bak" ) == 0 )return FALSE;

    f = fopen( filename , "r" );
    if( f == NULL ) goto RETURNFALSE;
    ret = fgets( line1, sizeof( line1 ), f );
    if( ret == NULL )goto FCLOSERETURNFALSE;

    if( strcmp( NPC_TEMPLATEFILEMAGIC, line1 ) == 0 ){
        fclose(f);
        return TRUE;
    }

FCLOSERETURNFALSE:
    fclose(f);
RETURNFALSE:
    return FALSE;
}

void NPC_templateallocitemdata( NPC_Template* one )
{
    int     itemnum = one->intdata[NPC_TEMPLATEITEMNUM];
    if( itemnum > 8 ) itemnum = 8;
    else if( itemnum <= 0 ) itemnum = 0;
    /*  °À¼°  µÊ    */
    one->intdata[NPC_TEMPLATEITEMNUM] = itemnum;
    one->haveitem = allocateMemory( sizeof( NPC_haveItem ) * itemnum );

}


/*------------------------------------------------------------
 * NPC_getRandomValue Îç¼°Â¦ÐÑ¼°Ñ¯¾§Áë¼°Ð×»§¼°èúÐÑ
 * Â¦ÐÑ
 *  string      char*         Ù¯  
 *  unuse       int*        Òø´õØ¦ÖÐ£Û
 * ß¯Ô»°À
 *  ·Â¼þÄ¸Ø©¼°  ìµ¼°°À  ãà¼°É¬ÀÃ·´£ý2    ¼°Â¦ÐÑ±åâçÐåÔÊÔÂ£Û
 ------------------------------------------------------------*/
static int NPC_seekGraphicNumberFromString( char* string, int* unuse )
{
    return CHAR_seekGraphicNumberFromString( string );

}
/*------------------------------------------------------------
 * ·Â¼þÄ¸Ø©°ÀÃ«  ¸ê
 * Â¦ÐÑ
 *  string      char*         Ù¯  
 *  randomvalue int*        ·Â¼þÄ¸Ø©¼°  Îå½ñ
 * ß¯Ô»°À
 *  ·Â¼þÄ¸Ø©¼°  ìµ¼°°À  ãà¼°É¬ÀÃ·´£ý2    ¼°Â¦ÐÑ±åâçÐåÔÊÔÂ£Û
 ------------------------------------------------------------*/
static int NPC_getRandomValue( char*  string,int* randomwidth )
{
    int   minvalue;
    int   maxvalue;
    char*   startmax=NULL;

    minvalue = atoi( string );
    startmax = index( string,',' );
    if( startmax != NULL ){
        /*  ·Â¼þÄ¸Ø©É¬ÀÃ»¥½ñÄ¾»¯ÖÐÔÂ    */
        /*  +1 ·´ ","¼°Ð×»§  */
        maxvalue = atoi(startmax+1);

        /*  ÇÐÔúÊÏÎçØÆÐ×É¬ÀÃ·Ö  */
        *randomwidth = ABS(maxvalue - minvalue);
        return min(minvalue,maxvalue);
    }
    *randomwidth = 0;
    return minvalue;
}


/*------------------------------------------------------------
 * NPCTemplate °×ÑëÄÌ»ïÃ«·òÎç¹´  ¸ê£Û
 * Â¦ÐÑ
 *  filename   char*       É¬ÀÃ°×ÑëÄÌ»ïÃ«  ÐÄï§»§ÔÂÐþÓÀÃó·¸Å«ÒÁÛÍÐþØø
 * ß¯Ô»°À
 *  ÔÀ      TRUE(1)
 *  ·Ö»§(³ðÄ¾¶¯Ïþ°×ÑëÄÌ»ïÃ«  ÊÏÆ¥·´ÖÐØêØ¦ÖÐ)
 *  ÁÃ      FALSE(0)
 ------------------------------------------------------------*/
BOOL    NPC_readTemplateFile( char* filename )
{
    FILE*   f;
    char    line[512];
    int     linenum=0;
    int     start=OFF;
    NPC_Template    temp;
    int     itmreadindex=0;
    char*   ret;
    int     randomdata[NPC_TEMPLATEINTNUM];
    int     i;

    if( NPC_template_readindex >= NPC_templatenum ){
        print("Ä£¿é³¬¹ýÅäÖÃÊýÄ¿\n" );
        print("ÅäÖÃÄ£¿éÊýÄ¿ %d\n", NPC_templatenum);
        return FALSE;
    }

    for( i=0; i<NPC_TEMPLATEINTNUM ; i++ )
        randomdata[i] = 0;


    NPC_setDefaultNPCTemplate( &temp );

    f= fopen( filename ,"r");
    if( f == NULL )return FALSE;
    ret = fgets( line, sizeof( line ), f );
    if( ret == NULL )goto FCLOSERETURNFALSE;
    if( strcmp( NPC_TEMPLATEFILEMAGIC, line ) != 0 ){
        print( "Õâ²»ÊÇÄ£¿éÎÄ¼þ.\n" );
        goto FCLOSERETURNFALSE ;
    }
    linenum = 1;
    while( fgets( line , sizeof( line ) , f ) ){
        linenum++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );
        switch( line[0] ){
        case '{':
            if( start == ON ){
                fprint( "Find {. ÒÑ¾­¿ªÊ¼ÉùÃ÷. %s:%d\n",
                        filename, linenum);
                goto FCLOSERETURNFALSE;

            }else
                start = ON;
            break;
        case '}':
            if( start == ON ){
                int err=FALSE;
                if( strcmp(temp.chardata[NPC_TEMPLATENAME].string ,"")
                    == 0 ){
                    fprint( "ÕâÐ©Ã»ÓÐÃû×Ö %s:%d\n",
                            filename, linenum );
                    err  = TRUE;
                }else if( temp.intdata[NPC_TEMPLATETYPE] == -1 ){
					temp.intdata[NPC_TEMPLATETYPE] = SPR_pet001;
				}
                if( err ){
                    if( temp.intdata[NPC_TEMPLATEITEMNUM] != 0 )
                        freeMemory( temp.haveitem );

                }else{
                    temp.hash
                        = hashpjw(temp.chardata[NPC_TEMPLATENAME].string);
                    temp.intdata[NPC_TEMPLATEITEMNUM] = itmreadindex;
                    for( i=0 ; i<NPC_TEMPLATEINTNUM ; i++ ){
                        temp.randomdata[i] = randomdata[i];
                        randomdata[i]=0;
                    }
                    memcpy( &NPC_template[NPC_template_readindex],
                            &temp, sizeof( NPC_Template ) );
                    NPC_template_readindex++;
                    if( NPC_template_readindex >= NPC_templatenum ){
                        print("Ä£¿é³¬¹ýÅäÖÃÊýÄ¿\n" );
                        print("ÅäÖÃÄ£¿éÊýÄ¿ÊÇ %d\n",
                              NPC_templatenum);
                        goto FCLOSERETURNFALSE;
                    }
                }
                NPC_setDefaultNPCTemplate( &temp );
                itmreadindex=0;
                start=OFF;
            }else{
                fprint( "Î´½â¾ö '}' at %s:%d\n",filename,linenum);
                goto FCLOSERETURNFALSE;
            }
            break;
        default:
        {
            typedef struct  tagNPC_Readtemplate
            {
                char*   keyword;
                NPC_TYPECATEGORYATREADFILE type;
                int     index;
                void*   func;
            }NPC_Readtemplate;
            static NPC_Readtemplate NPC_readtemplate[NPC_TEMPLATECHARNUM+NPC_TEMPLATEINTNUM]=
            {
                {"templatename" , NPC_CHARENTRY, NPC_TEMPLATENAME,NULL},
                {"name"         , NPC_CHARENTRY, NPC_TEMPLATECHARNAME,
                 NULL},
                {"makeatnobody", NPC_INTENTRY,  NPC_TEMPLATEMAKEATNOBODY,
                 NULL},
                {"makeatnosee"  , NPC_INTENTRY, NPC_TEMPLATEMAKEATNOSEE,
                 NULL},
                {"graphicname", NPC_INTFUNC,   NPC_TEMPLATEIMAGENUMBER,
                 NPC_seekGraphicNumberFromString},
                {"type",    NPC_INTFUNC,    NPC_TEMPLATETYPE,
                 NPC_seekGraphicNumberFromString},

                {"hp",   NPC_INTFUNC,        NPC_TEMPLATEMINHP,
                 NPC_getRandomValue},
                {"mp",   NPC_INTFUNC,        NPC_TEMPLATEMINMP,
                 NPC_getRandomValue},
                {"str",   NPC_INTFUNC,       NPC_TEMPLATEMINSTR,
                 NPC_getRandomValue},
                {"tough",   NPC_INTFUNC,     NPC_TEMPLATEMINTOUGH,
                 NPC_getRandomValue},

                {"fly", NPC_INTENTRY,NPC_TEMPLATEISFLYING,NULL},

                {"itemnum",  NPC_INTENTRY,   NPC_TEMPLATEITEMNUM, NULL,},
                {"functionset",NPC_INTFUNC, NPC_TEMPLATEFUNCTIONINDEX,
                 NPC_searchFunctionSet,},
                {"initfunc", NPC_CHARENTRY,   NPC_TEMPLATEINITFUNC,NULL,},
                {"walkprefunc", NPC_CHARENTRY, NPC_TEMPLATEWALKPREFUNC,
                 NULL,},
                {"walkpostfunc",NPC_CHARENTRY, NPC_TEMPLATEWALKPOSTFUNC,
                 NULL,},
                {"preoverfunc", NPC_CHARENTRY, NPC_TEMPLATEPREOVERFUNC,
                 NULL,},
                {"postoverfunc",NPC_CHARENTRY, NPC_TEMPLATEPOSTOVERFUNC,
                 NULL,},
                {"watchfunc", NPC_CHARENTRY,   NPC_TEMPLATEWATCHFUNC,
                 NULL,},
                {"loopfunc", NPC_CHARENTRY, NPC_TEMPLATELOOPFUNC, NULL,},
                {"talkedfunc",NPC_CHARENTRY,NPC_TEMPLATETALKEDFUNC,NULL,},
                {"dyingfunc",NPC_CHARENTRY,NPC_TEMPLATEDYINGFUNC,NULL,},
                {"preattackedfunc",NPC_CHARENTRY,
                 NPC_TEMPLATEPREATTACKEDFUNC,NULL,},
                {"postattackedfunc",NPC_CHARENTRY,
                 NPC_TEMPLATEPOSTATTACKEDFUNC,NULL,},
                {"offfunc",NPC_CHARENTRY,NPC_TEMPLATEOFFFUNC,NULL,},
                {"lookedfunc",NPC_CHARENTRY,NPC_TEMPLATELOOKEDFUNC,NULL,},
                {"itemputfunc",NPC_CHARENTRY,NPC_TEMPLATEITEMPUTFUNC,
                 NULL,},
                {"specialtalkedfunc",NPC_CHARENTRY,
                 					NPC_TEMPLATESPECIALTALKEDFUNC, NULL,},

                {"windowtalkedfunc",NPC_CHARENTRY,
                					NPC_TEMPLATEWINDOWTALKEDFUNC, NULL,},

                {"loopfunctime",NPC_INTENTRY,NPC_TEMPLATELOOPFUNCTIME,
                 NULL,},
            };

            char    firstToken[256];
            int     ret,i;
            ret = getStringFromIndexWithDelim( line, "=",  1, firstToken, sizeof( firstToken ) );
            if( ret == FALSE ){
                print( "Find error at %s in line %d. Ignore\n", filename , linenum);
                break;
            }

            if( strcasecmp(firstToken,"itm") == 0 ){
                char    one[128];
                char    two[128];
                char    three[128];
                int     itemnumber=0,rate=0,amount=1;
                char    secondToken[256]={""};
                if( itmreadindex < temp.intdata[NPC_TEMPLATEITEMNUM] &&
                    temp.haveitem != NULL )
                    ;
                else
                    continue;
                ret = getStringFromIndexWithDelim( line, "=", 2,
                                                   secondToken, sizeof(secondToken ) );
                if( ret == FALSE ){
                    print( "Find error at %s in line %d. Ignore\n",
                           filename , linenum);
                    secondToken[0] = '\0';
                }
                ret = getStringFromIndexWithDelim( secondToken, "|", 1, one,sizeof( one ) );
                if( ret != FALSE )itemnumber=atoi(one);
                ret = getStringFromIndexWithDelim( secondToken, "|", 2, two,sizeof(two));
                if( ret != FALSE )rate=atoi(two);
                ret = getStringFromIndexWithDelim( secondToken, "|", 3, three,sizeof(three));
                if( ret != FALSE )amount=atoi(three);
                if( rate > 0 && amount > 0 ){
                    if( itemnumber == -1 ){
                        temp.haveitem[itmreadindex].itemnumber = -1;
                        temp.haveitem[itmreadindex].haverate   = rate;
                        temp.haveitem[itmreadindex].havenum    = amount;
                        itmreadindex++;
                    }else if( ITEM_CHECKITEMTABLE( itemnumber ) == TRUE ){
                        temp.haveitem[itmreadindex].itemnumber=itemnumber;
                        temp.haveitem[itmreadindex].haverate   = rate;
                        temp.haveitem[itmreadindex].havenum    = amount;
                        itmreadindex++;
                    }else
                        print("this item is not item %s:%d\n",
                              filename , linenum);
                }else
                    print("rate or amount is odd setting. rate=%d "
                          "amount=%d  %s:%d\n", rate,amount, filename,
                          linenum);
                goto NEXT;
            }else{
                for( i = 0 ; i < arraysizeof(NPC_readtemplate) ; i ++ ){
                    if( strcasecmp( NPC_readtemplate[i].keyword,
                                    firstToken ) == 0 ){
                        char    secondToken[256]={""};
                        ret = getStringFromIndexWithDelim(
                            line, "=", 2, secondToken, sizeof(secondToken)
                            );
                        if( ret == FALSE )
                            print( "Find error at %s in line %d. Ignore\n"
                                   ,filename , linenum);

                        switch( NPC_readtemplate[i].type ){
                        case NPC_INTENTRY:
                            temp.intdata[NPC_readtemplate[i].index]=atoi(secondToken);
                            if( NPC_readtemplate[i].index == NPC_TEMPLATEITEMNUM )
                                NPC_templateallocitemdata( &temp );
                            break;
                        case NPC_CHARENTRY:
                            strcpysafe( temp.chardata[NPC_readtemplate[i].index].string,
                                        sizeof(temp.chardata[NPC_readtemplate[i].index].string),
                                        secondToken);
                            break;
                        case NPC_INTFUNC:
                        {
                            int (*intfunction)(char*,int* );
                            intfunction = NPC_readtemplate[i].func;
                            temp.intdata[NPC_readtemplate[i].index]
                                = intfunction( secondToken, &randomdata[NPC_readtemplate[i].index]);
                            break;
                        }
                        case NPC_CHARFUNC:
                        {
                            char* (*charfunction)(char* );
                            charfunction = NPC_readtemplate[i].func;
                            strcpysafe( temp.chardata[NPC_readtemplate[i].index].string,
                                        sizeof(temp.chardata[NPC_readtemplate[i].index].string),
                                        charfunction(secondToken));
                            break;
                        }
                        default:
                            break;
                        }
                        goto NEXT;
                    }
                }
            }
            fprint("%s:%d There is no such entry %s\n" , filename,
                   linenum, firstToken );
        NEXT:
            break;
        }
            break;
        }
    }
    fclose(f);
    return TRUE;

FCLOSERETURNFALSE:
    fclose(f);
    return FALSE;
}

BOOL NPC_readNPCTemplateFiles( char* topdirectory ,int templatesize)
{
    STRING64    *filenames;
    int         filenum;
    int         i;
    filenames = (STRING64 *)allocateMemory( sizeof( STRING64 ) *
                                               (int)getFilesearchnum( ) );
	if( filenames == NULL ){
	    print( "¶ÁÈ¡NPC´´½¨ÎÄ¼þ... ÄÚ´æ³ö´í\n" );
		return FALSE;
	}
    filenum = rgetFileName( topdirectory ,filenames ,getFilesearchnum( ) );
    if( filenum == -1 ){
        fprint("ÎÞ·¨´Ó %s µÝ¹é»ñÈ¡ÎÄ¼þÃû\n",topdirectory);
	    freeMemory( filenames );
        return FALSE;
    }
	if( filenum >= getFilesearchnum( ) ){
		print( "ÎÄ¼þËÑË÷ÊýÄ¿Ê§°Ü %d\n", getFilesearchnum( ) );
		while( 1 );
	}else{
		print( "ËÑË÷µ½ÎÄ¼þ = %d\n", filenum );
	}
    if( !NPC_initTemplateArray( templatesize ) ){
        fprint( "¿ªÆôÄ£¿éÊý×é´íÎó\n" );
	    freeMemory( filenames );
        return FALSE;
    }
    print( "¶ÁÈ¡ÎÄ¼þÄ£¿éÊý×é´íÎó...." );
    for( i = 0 ; i < filenum ; i ++ )
        if( NPC_IsNPCTemplateFile( filenames[i].string ))
            NPC_readTemplateFile( filenames[i].string );
    print( "ÕýÈ·Ä£¿é %d ...\n" , NPC_template_readindex );
    NPC_templatenum=NPC_template_readindex;
    freeMemory( filenames );
    return TRUE;
}



/*------------------------------------------------------------
 *   Ù¯  ±å¸²É±ÔÊÔÂ template ¼° index Ã«ß¯ÔÊ£Û
 * Â¦ÐÑ
 *  templatename        char*         ¼þÃóÒÁ¡õÐþ¼°  ó¡
 * ß¯Ô»°À
 *  -1  ¼°ÁÝ¾Þ·Â¡õ
 ------------------------------------------------------------*/
int NPC_templateGetTemplateIndex( char* templatename )
{
    int     i;
    int     hash=hashpjw( templatename );

    for( i = 0 ; i < NPC_templatenum ; i ++ )
        if( hash == NPC_template[i].hash &&
            strcasecmp( templatename , NPC_template[i].chardata[NPC_TEMPLATENAME].string) ==  0 )
            return i;

    return -1;
}
