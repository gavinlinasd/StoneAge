#include "version.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "npcutil.h"
#include "item.h"
#include "readmap.h"
#include "enemy.h"
#include "chatmagic.h"
#include "log.h"

#define EVFLG 118                 // 58->啞誥  118->ч韓
  
void pet_make( int charaindex, char* message );
  
BOOL NPC_PetMakerInit( int meindex )
{
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEPETMAKER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    return TRUE;
}

void NPC_PetMakerTalked( int meindex , int talker , char *msg ,int color )
{
    char* npcarg;
    char  token[32],tmpbuf[256];
    int   msgNo/*,translv*/;
    int   point,ckpoint,array,shift; 
    int   i,num=0;

    array   = EVFLG / 32;
    shift   = EVFLG % 32;
    point   = CHAR_getInt( talker, CHAR_ENDEVENT + array);
    ckpoint = point;

    npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

    getStringFromIndexWithDelim( npcarg, "|", 1, token,sizeof( token));
    msgNo  = atoi( token );

    if( CHAR_getInt( talker , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )	
        return;

    if( NPC_Util_isFaceToFace( talker, meindex , 2 ) == FALSE ) 
    {
    	if( NPC_Util_isFaceToFace( talker, meindex , 1 ) == FALSE ) 
        	return;
    }
    
    //translv = CHAR_getInt( talker, CHAR_TRANSMIGRATION);    // 啞誥
    
    for(i=0; i<CHAR_MAXPETHAVE; i++)
        if(CHAR_getCharPet(talker, i) != -1) 
            num++;

    // 啞誥
    /*if( translv < 5 || CHAR_getInt( talker, CHAR_LV ) < 125){
        sprintf(tmpbuf, "惕К韃‵脹善蛌汜ㄤ棒摯脹撰ㄠㄡㄤ撰奀婬懂梑扂勘ㄐ");
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;
    }*/   
	// ч韓	
    if( CHAR_getInt( talker, CHAR_FAME) < 200000){
        sprintf(tmpbuf, "惕К韃‵斕醴ヶ跺�侂驫� %d 萸ㄛ③脹善閉徹媼ロ萸奀婬懂梑扂勘ㄐ", (CHAR_getInt( talker, CHAR_FAME)/100));
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;
    }
    else if( ckpoint & (1 << shift) ){
        sprintf(tmpbuf, "韃‵扂暮腕扂疑砉跤徹斕賸勘ㄐ");
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;
    }    
    else if( num>=CHAR_MAXPETHAVE ){
        sprintf(tmpbuf, "惕К‵羶嗣牄腔諾潔褫溫离唾昜﹝");
        CHAR_talkToCli( talker, meindex,
                        tmpbuf,CHAR_COLORWHITE);
        return;    
    }
    
    pet_make( talker, "1845" );
    
    point = point | (1 << shift);
    CHAR_setInt( talker, CHAR_ENDEVENT + array, point);
}

void pet_make( int charaindex, char* message )
{
    int  ret;
    char msgbuf[64];
    int  enemynum;
    int  enemyid;
    int  i;

    enemyid  = atoi(message);
    enemynum = ENEMY_getEnemyNum();

    for( i=0; i<enemynum; i++ ){
        if( ENEMY_getInt( i, ENEMY_ID) == enemyid){
            break;
        }
    }
    if(i==enemynum) return;

    ret = ENEMY_createPetFromEnemyIndex( charaindex, i);
    snprintf( msgbuf, sizeof( msgbuf), "韃‵眕斕腔夔薯逋眕饜腕奻珨硐換佽唾昜ㄛ疑勘ㄐ憩岆斕賸﹝");
    CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);
                
    for( i=0; i<CHAR_MAXPETHAVE; i++ ){
        if( CHAR_getCharPet( charaindex, i) == ret )break;
    }
  
    if( i==CHAR_MAXPETHAVE) i = 0;
    if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
    }
                                                                                                
    snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
    CHAR_sendStatusString( charaindex, msgbuf );
                     
    snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
    CHAR_sendStatusString( charaindex, msgbuf );
	
	for( i=0; i<CHAR_MAXPETHAVE; i++ )
		if( CHAR_getCharPet(charaindex, i) == ret )
			CHAR_send_K_StatusString(charaindex, i, CHAR_K_STRING_HP|CHAR_K_STRING_AI);
	
	LogPet(
		CHAR_getChar( charaindex, CHAR_NAME ), /* す籤溘   */
		CHAR_getChar( charaindex, CHAR_CDKEY ),
		"翮塋",1,"PetMaker",
		CHAR_getInt( charaindex,CHAR_FLOOR),
		CHAR_getInt( charaindex,CHAR_X ),
		CHAR_getInt( charaindex,CHAR_Y ),
		CHAR_getChar( ret, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
}
