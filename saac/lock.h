#ifndef _LOCK_H_
#define _LOCK_H_

#include "version.h"

typedef struct tagLockNode {
  int use;
  char cdkey[128];
#ifdef _LOCK_ADD_NAME
	char name[128];
#endif
  char server[128];
  int process;
  struct tagLockNode *next,*prev;
#ifdef _TIMEOUTKICK
  char passwd[128];
  char deadline[128];
#endif
} LockNode;

extern LockNode **userlock;

void Lock_Init(void);
#ifdef _LOCK_ADD_NAME
	int InsertMemLock(int entry, char *cdkey, char *name, char *passwd, char *server, int process, char *deadline);
#else
	int InsertMemLock(int entry, char *cdkey, char *passwd, char *server, int process, char *deadline);
#endif
int DeleteMemLock(int entry, char *cdkey, int *process);
void DeleteMemLockServer(char *sname);
int isMemLocked(int entry, char *cdkey);
int GetMemLockState(int entry, char *cdkey, char *result);
int GetMemLockServer(int entry, char *cdkey, char *result);

int LockNode_getGname( int entries, char *id, char *gname);


#ifdef _WORKER_AUTO_LOGOUT     // WON ADD GS断线自动送LOGOUT给WORKER
void Worker_auto_logout(int ti, char *sname);
#endif

#ifdef _CHANGEGALAXY
int DeleteChangeGalaxyLock( void);
int isChangeGalaxyLocked( char *cdkey);
#endif

int ChangeMemDeadline(int entry, char *cdkey, char *deadline);

#ifdef _UNLOCKPOOL
typedef struct tagUnlockPoolNode {
  int use;
  int serverindex;
  char cdkey[128];
  char charname[128];
  char process[128];
  int mesgid;
} UnlockPoolNode;

#define	UNLOCKPOOLNUM	2000
extern UnlockPoolNode UnlockPool[UNLOCKPOOLNUM];
extern int iUnlockPoolAdd;

int addUnlockPool( int serverindex, char *cdkey, char *charname, char *process, int mesgid);
void init_UnlockPool( void);
int inUnlockPool( char* cdkey);

#endif

#endif

