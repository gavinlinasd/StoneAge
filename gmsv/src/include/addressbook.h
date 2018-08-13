#ifndef __ADDRESSBOOK_H__
#define __ADDRESSBOOK_H__

#include "common.h"
#include "util.h"
#include "net.h"

#define ADDRESSBOOK_MAX 40

typedef struct
{
    int use;
    BOOL online;					/* 锹澎平乓仿互扔□田  卞中凶日TRUE,
                                		公丹匹卅井匀凶日FALSE */
    int level;               		/* 锹澎平乓仿及伊矛伙 */
	int	duelpoint;					/* duelpoint*/
    int graphicsno;					/* 锹澎平乓仿及  飓  寞 */
    char cdkey[CDKEYLEN];			/* CD 瓜件田□ */
    char charname[CHARNAMELEN];		/* 锹澎平乓仿及  蟆 */
	int transmigration;             /* 鳖戏荚醒 */
	
} ADDRESSBOOK_entry;




char *ADDRESSBOOK_makeAddressbookString( ADDRESSBOOK_entry *a );
BOOL ADDRESSBOOK_makeAddressbookEntry( char *in , ADDRESSBOOK_entry *a );
BOOL ADDRESSBOOK_deleteEntry( int meindex ,int index );
BOOL ADDRESSBOOK_addEntry( int meindex );
BOOL ADDRESSBOOK_sendAddressbookTable( int cindex );
BOOL ADDRESSBOOK_sendAddressbookTableOne( int cindex, int num );
BOOL ADDRESSBOOK_sendMessage( int cindex, int aindex , char *text ,
                              int color );
BOOL ADDRESSBOOK_sendMessage_FromOther( char *fromcdkey, char *fromcharaname, 
										char *tocdkey, char *tocharaname,
										char* text , int color );

void ADDRESSBOOK_notifyLoginLogout( int cindex , int flg );
void ADDRESSBOOK_addAddressBook( int meindex, int toindex);
void ADDRESSBOOK_DispatchMessage( char *cd, char *nm, char *value, int mode);
int ADDRESSBOOK_getIndexInAddressbook(int cindex , char *cdkey, char *charname);
BOOL ADDRESSBOOK_AutoaddAddressBook( int meindex, int toindex);

#endif
