#ifndef __ENEMY_H__
#define __ENEMY_H__

enum
{
    E_T_SIZE_NORMAL,
    E_T_SIZE_BIG,
};

typedef enum
{
    E_T_TEMPNO,
    E_T_INITNUM,
    E_T_LVUPPOINT,
    E_T_BASEVITAL,
    E_T_BASESTR,
    E_T_BASETGH,
    E_T_BASEDEX,
    E_T_MODAI,
    E_T_GET,
    E_T_EARTHAT,
    E_T_WATERAT,
    E_T_FIREAT,
    E_T_WINDAT,
    E_T_POISON,               /*   汹仁凶太卞母丢□斥          */
    E_T_PARALYSIS,            /* 仄太木｝1  及垫  互匹五卅中［ */
    E_T_SLEEP,                /* 戽曰［垫  匹五卅中            */
    E_T_STONE,                /* 檗［垫  匹五卅中              */
    E_T_DRUNK,                /* 办丹［      互票互月     */
    E_T_CONFUSION,            /* 渔刭［  猾    毛赀月     */
    E_T_PETSKILL1,
    E_T_PETSKILL2,
    E_T_PETSKILL3,
    E_T_PETSKILL4,
    E_T_PETSKILL5,
    E_T_PETSKILL6,
    E_T_PETSKILL7,
    E_T_RARE,
    E_T_CRITICAL,
    E_T_COUNTER,
    E_T_SLOT,
    E_T_IMGNUMBER,
    E_T_PETFLG,
    E_T_SIZE,
	E_T_ATOMBASEADD1,
	E_T_ATOMFIXMIN1,
	E_T_ATOMFIXMAX1,
	E_T_ATOMBASEADD2,
	E_T_ATOMFIXMIN2,
	E_T_ATOMFIXMAX2,
	E_T_ATOMBASEADD3,
	E_T_ATOMFIXMIN3,
	E_T_ATOMFIXMAX3,
	E_T_ATOMBASEADD4,
	E_T_ATOMFIXMIN4,
	E_T_ATOMFIXMAX4,
	E_T_ATOMBASEADD5,
	E_T_ATOMFIXMIN5,
	E_T_ATOMFIXMAX5,
    E_T_LIMITLEVEL,	// Arminius 7.30 limit level
#ifdef _PET_FUSION
	E_T_FUSIONCODE,
#endif
    E_T_DATAINTNUM,
}ENEMYTEMP_DATAINT;

typedef enum
{
    E_T_NAME,
    E_T_ATOMFIXNAME1,
    E_T_ATOMFIXNAME2,
    E_T_ATOMFIXNAME3,
    E_T_ATOMFIXNAME4,
    E_T_ATOMFIXNAME5,
    E_T_DATACHARNUM,

}ENEMYTEMP_DATACHAR;

typedef enum
{
    ENEMY_ID,
    ENEMY_TEMPNO,
    ENEMY_LV_MIN,
    ENEMY_LV_MAX,
    ENEMY_CREATEMAXNUM,
    ENEMY_CREATEMINNUM,
    ENEMY_TACTICS,
	ENEMY_EXP,
    ENEMY_DUELPOINT,
	ENEMY_STYLE,
    ENEMY_PETFLG,               /* 矢永玄卞卅月井升丹井 */

    ENEMY_ITEM1,
    ENEMY_ITEM2,
    ENEMY_ITEM3,
    ENEMY_ITEM4,
    ENEMY_ITEM5,
    ENEMY_ITEM6,
    ENEMY_ITEM7,
    ENEMY_ITEM8,
    ENEMY_ITEM9,
    ENEMY_ITEM10,
    ENEMY_ITEMPROB1,
    ENEMY_ITEMPROB2,
    ENEMY_ITEMPROB3,
    ENEMY_ITEMPROB4,
    ENEMY_ITEMPROB5,
    ENEMY_ITEMPROB6,
    ENEMY_ITEMPROB7,
    ENEMY_ITEMPROB8,
    ENEMY_ITEMPROB9,
    ENEMY_ITEMPROB10,

    ENEMY_DATAINTNUM,

}ENEMY_DATAINT;

typedef enum
{
    ENEMY_NAME,
    ENEMY_TACTICSOPTION,
#ifdef _BATTLENPC_WARP_PLAYER
	ENEMY_ACT_CONDITION,
#endif
    ENEMY_DATACHARNUM,
}ENEMY_DATACHAR;

typedef enum
{
    GROUP_ID,
	GROUP_APPEARBYITEMID,		/* 仇及失奶  丞毛  匀化中凶日请蜇允月 -1 反  骰*/
	GROUP_NOTAPPEARBYITEMID,	/* 仇及失奶  丞毛  匀化中凶日请蜇仄卅中 -1 反  骰*/
    ENEMY_ID1,
    ENEMY_ID2,
    ENEMY_ID3,
    ENEMY_ID4,
    ENEMY_ID5,
    ENEMY_ID6,
    ENEMY_ID7,
    ENEMY_ID8,
    ENEMY_ID9,
    ENEMY_ID10,
    CREATEPROB1,
    CREATEPROB2,
    CREATEPROB3,
    CREATEPROB4,
    CREATEPROB5,
    CREATEPROB6,
    CREATEPROB7,
    CREATEPROB8,
    CREATEPROB9,
    CREATEPROB10,
    GROUP_DATAINTNUM,
}GROUP_DATAINT;

typedef enum
{
    GROUP_NAME,
    GROUP_DATACHARNUM,

}GROUP_DATACHAR;


typedef struct tagENEMY_EnemyTable
{
    int         intdata[ENEMY_DATAINTNUM];
    STRING64    chardata[ENEMY_DATACHARNUM];
    int         enemytemparray;
}ENEMY_EnemyTable;

typedef struct tagENEMYTEMP_Table
{
    int         intdata[E_T_DATAINTNUM];
	//ANDY_EDIT
    STRING64    chardata[E_T_DATACHARNUM];

}ENEMYTEMP_Table;

typedef struct tagGROUP_Table
{
    int         intdata[GROUP_DATAINTNUM];
    STRING32    chardata[GROUP_DATACHARNUM];
    int         enemyarray[CREATEPROB1 - ENEMY_ID1];
}GROUP_Table;



INLINE BOOL ENEMY_CHECKINDEX( int index);
INLINE int ENEMY_setInt( int index, ENEMY_DATAINT element, int data);
INLINE int ENEMY_getInt( int index, ENEMY_DATAINT element);

INLINE int *ENEMY_getIntdata( int index);

INLINE BOOL ENEMY_setChar( int index ,ENEMY_DATACHAR element, char* new );
INLINE char *ENEMY_getChar( int index, ENEMY_DATACHAR element);
int ENEMY_getEnemyNum( void);
BOOL ENEMY_initEnemy( char* filename );
BOOL ENEMY_reinitEnemy( void );
int ENEMY_createEnemy( int array, int baselevel );
int *ENEMY_getEnemy( int charaindex, int x, int y);
int ENEMY_createPetFromEnemyIndex( int charaindex, int array);

int ENEMY_getEnemyArrayFromId( int EnemyId);
int ENEMY_getEnemyArrayFromTempNo( int EnemyTempNo);
int ENEMY_getEnemyIdFromTempNo( int EnemyTempNo);
int ENEMY_getEnemyTempNoFromId( int EnemyId);

#ifdef _TEST_PETCREATE
int ENEMY_TEST_createPetIndex( int array);
#endif

int ENEMYTEMP_getEnemyNum( void);//krynn 2001/12/13
INLINE BOOL ENEMYTEMP_CHECKINDEX( int index);
INLINE int ENEMYTEMP_setInt( int index, ENEMYTEMP_DATAINT element, int data);
INLINE int ENEMYTEMP_getInt( int index, ENEMYTEMP_DATAINT element);
INLINE char *ENEMYTEMP_getChar( int index, ENEMYTEMP_DATACHAR element);
INLINE BOOL ENEMYTEMP_getInt_setChar( int index ,ENEMYTEMP_DATACHAR element, char* new );
INLINE char *ENEMYTEMP_getInt_getChar( int index, ENEMYTEMP_DATACHAR element);
int ENEMYTEMP_getInt_getEnemyNum( void);
BOOL ENEMYTEMP_getInt_initEnemy( char* filename );
BOOL ENEMYTEMP_getInt_reinitEnemy( void );
int ENEMYTEMP_getEnemyTempArray( int enemyindex);
int ENEMYTEMP_getEnemyTempArrayFromTempNo( int EnemyTempNo);
int ENEMYTEMP_getEnemyTempArrayFromInitnum( int EnemyTempNo);

INLINE int GROUP_setInt( int index, GROUP_DATAINT element, int data);
INLINE int GROUP_getInt( int index, GROUP_DATAINT element);
INLINE BOOL GROUP_setChar( int index ,GROUP_DATACHAR element, char* new );
INLINE char *GROUP_getChar( int index, GROUP_DATACHAR element);
int GROUP_getEnemyNum( void);

BOOL ENEMYTEMP_initEnemy( char* filename );
BOOL ENEMYTEMP_reinitEnemy( void );
BOOL GROUP_initGroup( char* filename );
BOOL GROUP_reinitGroup( void );

#ifdef _PET_EVOLUTION
int EVOLUTION_createPetFromEnemyIndex( int charaindex, int baseindex, int flg);
int PET_CheckIncubate( int charaindex);
BOOL PETFUSION_getIndexForChar( int toindex, int *MainIndex, int *Subindex1,int *Subindex2, char *data);
int NPC_getFusionTableForBase( int charaindex, int petindex1, int petindex2 );
int NPC_getPetArrayForNo( int PetCode);
BOOL PET_getEvolutionAns( int petindex, int *base);
#endif

int PETFUSION_SetNewEgg( int toindex , int petindex, int array, int *work, int *skill1, int *skill2);
BOOL PETFUSION_AddEgg(int toindex, int petID, int PetCode);

#ifdef _PET_TRANS
int GetNewPet( int toindex , int petindex, int array, int *work);
#endif

#endif
