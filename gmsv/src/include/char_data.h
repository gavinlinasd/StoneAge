#ifndef __CHAR_DATA_H__
#define __CHAR_DATA_H__

#include "item.h"
#include "char.h"


#define CHAR_CLASS01_PASSLV     20

#define BATEBAN	2
#define FIRSTPOSITIONMAXINDEX   1
#define ELDERINDEXSTART     FIRSTPOSITIONMAXINDEX+BATEBAN+1

typedef struct tagLevelUpPattern
{
#if 1
    struct  Exptbl{
        int origin;
        int multi;
    }exptbl[3];
    int     hpupminpoint;   /*  hp 及丐互月   斓袄*/
    int     hpupmaxpoint;   /*  hp 及丐互月   斓袄*/

    int     needexp;        /*    邰卅烦董袄    */
#else
    int     uppoint[5];
#endif
}LevelUpPattern;

/*====================赓渝袄｝伊皮伙失永皿楮溢====================*/
BOOL CHAR_getDefaultChar( Char*  nc, int imagenumber );

/*====================隶    ｝  飓  寞赘尹楮溢====================*/
int CHAR_getNewImagenumberFromEquip( int basebaseimagenumber,
                                     ITEM_CATEGORY category );
/*====================  飓  寞  侬   ->   寞楮溢====================*/
void CHAR_initSeekGraphicNumberFromString();
int CHAR_seekGraphicNumberFromString( char* string );



/*====================平乓仿及赓渝袄楮溢====================*/
BOOL CHAR_getInitElderPosition( Char* ch,int hometown);
BOOL CHAR_getElderPosition( int elderindex, int* fl, int* x, int* y );

void CHAR_setInitValues( Char* ch );


/*====================  衬匏  楮溢====================*/
typedef enum
{
    CHAR_INVAREA,
    CHAR_CANNOTDROPAREA,
    CHAR_CANNOTMAGICAREA,
	CHAR_AREAKIND_NONE,
}CHAR_AREAKIND;

BOOL CHAR_initInvinciblePlace( char* filename );
BOOL CHAR_isCannotMagicArea( int floor , int x, int y, int magicnum );
BOOL CHAR_initAppearPosition( char* filename );
BOOL CHAR_isInvincibleArea( int floor , int x, int y );
BOOL CHAR_isAppearPosition( int floor, int *x, int *y);
BOOL CHAR_isCannotDropArea( int floor , int x, int y );


/* 赢  楮溢 */
BOOL CHAR_ElderSetPosition( int elderindex ,int fl,int x ,int y);



/*************************************************
酵烂伊矛伙失永皿质  
*************************************************/


int GetEnemyExp( int level );

int CHAR_GetLevel();
int CHAR_GetLevelExp( int charaindex, int level );
int CHAR_GetOldLevelExp( int level);

int CHAR_LevelUpCheck( int charaindex , int toindex);

int CHAR_PetLevelUp( int petindex );

BOOL CHAR_checkFaceImageNumber( int imagenumber, int faceimagenumber);
BOOL CHAR_checkPlayerImageNumber( int imagenumber);

int CHAR_PetTakeLevelUp( int petindex, int lv);	//强制宠物升级
int CHAR_PetTakeDrop( int petindex, int floor, int ox, int oy);	//强制丢弃宠物升级

#ifdef _NPC_FUSION
int PETFUSION_FusionPetSub( int charaindex, int Subindex1, int Subindex2, int *work, int *skill);
BOOL PETFUSION_FusionPetMain( int charaindex, int Mainindex, int *work, int *skill);
BOOL PETFUSION_DelPet( int toindex, int Mainindex, int Subindex1, int Subindex2, int flg);
int PETFUSION_Evolution( int charaindex, int petindex);
#endif
int PETTRANS_PetTransManStatus( int toindex, int petindex1, int petindex2);
#ifdef _PET_2TRANS
int NPC_PetTransManGetAns( int total1, int total2, int LV, int rank, int tran);
#else
int NPC_PetTransManGetAns( int total1, int total2, int LV, int rank);
#endif //_PET_2TRANS

#ifdef _CHIKULA_STONE
void CHAR_AutoChikulaStone( int charaindex, int Dflg);
#endif

#ifdef _STATUS_WATERWORD //水世界状态
void CHAR_CheckWaterStatus( int charaindex);
#endif

int CHAR_findSurplusPetBox( int charaindex );

#ifdef _USER_EXP_CF
void setNeedLevelUpTbls(int level,int exp);
void setMaxLevel(int level);
#endif 

#endif 