#ifndef  __PETMAIL_H__
#define __PETMAIL_H__
#include "version.h"
/*
 * 矢永玄楮  及涩烂匹允［
 */
#define PETMAIL_OFFMSG_MAX          10000
#define PETMAIL_OFFMSG_TIMEOUT      ( 3 * 24 * 3600 )
#define PETMAIL_CHECK_OFFMSG_EXPIRE_INTERVAL  3600
#define PETMAIL_OFFMSG_TEXTLEN 512

/* 矢永玄丢□伙及请蜇躲绊及醒［  端及醒方曰ㄠ剂卅仁允月仇午 */
#define		 PETMAIL_EFFECTMAX	1			

#define		PETMAIL_SPOOLFLOOR		777
#define		PETMAIL_SPOOLX			30
#define		PETMAIL_SPOOLY			30

#define		PETMAIL_LOOPINTERVAL1	500
#define		PETMAIL_LOOPINTERVAL2	5000

/* 左白仿奶件及平乓仿尺及丢永本□斥毛创尹月 */
typedef struct
{
    int use;
    time_t send_tm;     /* 交□扒互丢永本□斥毛霜耨仄凶凛棉+TIMEOUT［ */
    int color;							/*   平旦玄及缙 */
    char text[PETMAIL_OFFMSG_TEXTLEN];     /*   平旦玄及     */
    char destcd[CDKEYLEN];				/* 霜耨燮CDKEY */
    char destcharname[CHARNAMELEN];		/* 霜耨燮及平乓仿   */
    char srccd[CDKEYLEN];				/* 霜耨葭及cdkey */
    char srccharname[CHARNAMELEN];		/* 霜耨葭及平乓仿   */
	
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
