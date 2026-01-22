#ifndef  __PETMAIL_H__
#define __PETMAIL_H__
#include "version.h"
/*
 * 妐蚗哱匴  摯优擭ぁ埰��
 */
#define PETMAIL_OFFMSG_MAX          10000
#define PETMAIL_OFFMSG_TIMEOUT      ( 3 * 24 * 3600 )
#define PETMAIL_CHECK_OFFMSG_EXPIRE_INTERVAL  3600
#define PETMAIL_OFFMSG_TEXTLEN 512

/* 妐蚗哱隍↓鳴摯③藯嗚堅摯倳��  傷摯倳源堇兒撙埵�彸彸糧蠁� */
#define		 PETMAIL_EFFECTMAX	1			

#define		PETMAIL_SPOOLFLOOR		777
#define		PETMAIL_SPOOLX			30
#define		PETMAIL_SPOOLY			30

#define		PETMAIL_LOOPINTERVAL1	500
#define		PETMAIL_LOOPINTERVAL2	5000

/* 酘啞溘騷璃摯す籤溘喜摯隍蚗掛↓喇禱斐窇堎 */
typedef struct
{
    int use;
    time_t send_tm;     /* 蝠↓勒誑隍蚗掛↓喇禱邞嚭媃倜鄹蹬+TIMEOUT�� */
    int color;							/*   す筒哱摯褗 */
    char text[PETMAIL_OFFMSG_TEXTLEN];     /*   す筒哱摯     */
    char destcd[CDKEYLEN];				/* 邞嚭袸CDKEY */
    char destcharname[CHARNAMELEN];		/* 邞嚭袸摯す籤溘   */
    char srccd[CDKEYLEN];				/* 邞嚭楁摯cdkey */
    char srccharname[CHARNAMELEN];		/* 邞嚭楁摯す籤溘   */
	
} PETMAIL_offmsg;

void PETMAIL_Loopfunc( int index);
BOOL PETMAIL_CheckPlayerExist( int index, int mode);

BOOL PETMAIL_initOffmsgBuffer( int count );
BOOL PETMAIL_addOffmsg( int fromindex, char *tocdkey, char *tocharaname,
                            char *text, int color );
PETMAIL_offmsg *PETMAIL_getOffmsg( int offmsgindex);
BOOL PETMAIL_deleteOffmsg( int offmsgindex);
void PETMAIL_proc( void );
BOOL storePetmail( void);
BOOL PETMAIL_sendPetMail( int cindex, int aindex, 
					int havepetindex, int haveitemindex, char* text , int color );

#ifdef _PETMAIL_DEFNUMS
void CHAR_AutoPickupMailPet( int charaindex, int petindex );
#endif
int PETMAIL_getPetMailTotalnums( void);
void PETMAIL_delPetMailTotalnums( int numflg);
void PETMAIL_setPetMailTotalnums( int numflg);
int PETMAIL_CheckIsMyOffmsg( int fromindex, char *tocdkey, char *tocharaname);

#endif
