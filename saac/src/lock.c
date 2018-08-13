#include <stdio.h>
#include <stdlib.h>
#include "lock.h"
#include "main.h"
#include "char.h"

#ifdef _WORKER_AUTO_LOGOUT     // WON ADD GS断线自动送LOGOUT给WORKER
#include "recv.h"
#endif

LockNode **userlock;

#ifdef _UNLOCKPOOL
#define	UNLOCKPOOLNUM	2000
UnlockPoolNode UnlockPool[UNLOCKPOOLNUM];
int iUnlockPoolAdd =0;
//int iUnlockPoolClean =0;
#endif

void Lock_Init(void)
{
	int i;
 	userlock= (LockNode **) calloc( 1, sizeof(LockNode *) * 256);
	memset(userlock, 0, sizeof(userlock));
	for (i=0; i<256; i++) {
		userlock[i] = (LockNode *)calloc( 1,sizeof(LockNode));
		userlock[i]->use=0;
		userlock[i]->next=NULL;
		userlock[i]->prev=NULL;
		memset( userlock[i]->cdkey, 0, sizeof( userlock[i]->cdkey) );
		memset( userlock[i]->server, 0, sizeof( userlock[i]->server) );
#ifdef _LOCK_ADD_NAME
		memset( userlock[i]->name, 0, sizeof( userlock[i]->name) );
#endif
	}
	log("存贮器初始化");
}

LockNode *Creat_newNodes( void)
{
	LockNode *newln=NULL;
	newln = ( LockNode *)calloc( 1, sizeof( LockNode) );
	if( newln == NULL ){
		log( "err Can't calloc:%d lock nodes !!\n", sizeof(LockNode));
		return 0;
	}
	newln->use=0;
	newln->next=NULL;
	memset( newln->cdkey, 0, sizeof( newln->cdkey) );
	memset( newln->server, 0, sizeof( newln->server) );
#ifdef _LOCK_ADD_NAME
	memset( newln->name, 0, sizeof( newln->name) );
#endif
	return newln;
}

#ifdef _LOCK_ADD_NAME
int InsertMemLock(int entry, char *cdkey,char *name, char *passwd, char *server, int process, char *deadline)
#else
int InsertMemLock(int entry, char *cdkey, char *passwd, char *server, int process, char *deadline)
#endif
{
	int j;
	LockNode *ln = userlock[entry];
#ifdef _LOCK_ADD_NAME
		log("进入游戏:目录:char/0x%x 账号:%s 名称:%s 服务器:%s\n", entry, cdkey, name, server);
#else
		log("进入游戏:目录:%x 账号:%s 服务器:%s\n", entry, cdkey, server);
#endif
  
	while( (ln!=NULL) && (ln->use!=0)) ln=ln->next;

	if( ln == NULL ) {
		LockNode *fhead=NULL;
		LockNode *p = userlock[entry];
		log("Add more lock nodes.\n");
		while (p->next!=NULL) p=p->next;
		fhead = p;
		for( j=0; j<32; j++) {	// allocate more nodes
			if( (ln = Creat_newNodes() ) == NULL ) return 0;
			ln->prev=p;
			p->next=ln;
			p=ln;
		}
		while( (fhead!=NULL) && (fhead->use!=0)) fhead=fhead->next;
		ln = fhead;
	}

	if( ln->use !=0 ) return 0;
	ln->use = 1;
	strcpy( ln->cdkey, cdkey);
	strcpy( ln->server, server);
#ifdef _LOCK_ADD_NAME
	strcpy( ln->name, name);
#endif
	ln->process = process;
#ifdef _TIMEOUTKICK
	strcpy( ln->passwd, passwd);
	strcpy( ln->deadline, deadline);
#endif
	return 1;
}

int DeleteMemLock(int entry, char *cdkey, int *process)
{
	LockNode *ln = userlock[entry];

	log("删除内存信息 位置=%x 账号=%s ..\n", entry, cdkey);

	while (ln!=NULL) {
		if( ln->use != 0) {
#ifdef _CHANGEGALAXY
			if( !strcmp( ln->cdkey, cdkey ) && strcmp( ln->server, "移民") ) break;
#else
			if( strcmp( ln->cdkey, cdkey ) == 0 ) break;
#endif
		}
		ln=ln->next;
	}
	if( ln != NULL ) {
		ln->use=0;
		memset( ln->cdkey, 0, sizeof( ln->cdkey) );
		memset( ln->server, 0, sizeof( ln->server) );
#ifdef _LOCK_ADD_NAME
		memset( ln->name, 0, sizeof( ln->name) );
#endif
		*process = ln->process;
		log("删除成功\n");
		return 1;
	}
	log("删除失败!!\n");
	return 0;
}

void DeleteMemLockServer(char *sname)
{
	int i;
	LockNode *ln;
	for (i=0; i<256; i++) {
		ln = userlock[i];
		while (ln!=NULL) {
			if (ln->use != 0) {
				if( strcmp( ln->server, sname)==0) {
					ln->use=0;
				}
			}
			ln=ln->next;
		}
	}
}


#ifdef _WORKER_AUTO_LOGOUT     // WON ADD GS断线自动送LOGOUT给WORKER
void Worker_auto_logout(int ti, char *sname)
{
  int i;
  LockNode *ln;
  
  log("\nGMSV 退出或工作站退出!! \n");

  for (i=0; i<256; i++) {
    ln = userlock[i];
    while (ln!=NULL) {
      if (ln->use != 0) {
        if (strcmp(ln->server, sname)==0) {
#if 0
			if( addUnlockPool( ti, ln->cdkey, "", "", "") <0 ) {
				char id[20];
				strcpy(id ,ln->cdkey);
				log("工作站发送退出 ==> ti(%d) 账号(%s) 序号(%d)\n", ti, id, iUnlockPoolAdd );
				Worker_send_logout(ti, id);
			}
#else
			char id[20];
			strcpy(id ,ln->cdkey);
			log("工作站发送退出==>ti(%d) 账号(%s)\n", ti, id );
			Worker_send_logout(ti, id);
#endif
        }
      }
      ln=ln->next;
    }
  }
}
#endif




int isMemLocked(int entry, char *cdkey)
{
	LockNode *ln = userlock[entry];
	while (ln!=NULL) {
		if (ln->use != 0) {
			if (strcmp(ln->cdkey, cdkey)==0) {
				if( !strcmp(ln->server, "星系移民"))
					log(" 星系移民中 ");
				break;
			}
		}
		ln=ln->next;
	}
	if (ln!=NULL) return 1; else return 0;
}

int GetMemLockState(int entry, char *cdkey, char *result)
{
	LockNode *ln = userlock[entry];
  
	while (ln!=NULL) {
		if (ln->use != 0) {
			if (strcmp(ln->cdkey, cdkey)==0) {
				sprintf(result, "%s 是在 %s 被锁的.",cdkey, ln->server);
				return 1;
			}
		}
		ln=ln->next;
	}
	sprintf(result, "%s 没有被锁.", cdkey);
	return 0;
}

int GetMemLockServer(int entry, char *cdkey, char *result)
{
	LockNode *ln = userlock[entry];
  	while (ln!=NULL) {
		if (ln->use != 0) {
			if (strcmp(ln->cdkey, cdkey)==0) {
				strcpy(result, ln->server);
				return 1;
			}
		}
		ln=ln->next;
	}
	return 0;
}

int LockNode_getGname( int entries, char *id, char *gname)
{
	LockNode *ln = userlock[entries];
	while (ln!=NULL) {
		if (ln->use != 0) {
			if( !strcmp(ln->cdkey, id) ){
				sprintf( gname,"%s", ln->server );
				return 1;
			}
		}
		ln=ln->next;
	}
	return 0;
}

#ifdef _CHANGEGALAXY

int DeleteGalaxyChangeLock( void)
{
	int count =0;
	int entry =0;

	log(" DeleteGalaxyChangeLock ");

	for( entry =0; entry <256; entry++) {

		LockNode *ln = userlock[entry];

		while (ln!=NULL) {
			if( ln->use != 0) {
				if( strcmp( ln->server, "星系移民" ) == 0 ) {
					ln->use=0;
					memset( ln->cdkey, 0, sizeof( ln->cdkey) );
					memset( ln->server, 0, sizeof( ln->server) );
#ifdef _LOCK_ADD_NAME
					memset( ln->name, 0, sizeof( ln->name) );
#endif
					//*process = ln->process;
					count++;
				}
			}
			ln=ln->next;
		}
	}

	log("num=%d  ", count);

	return count;
}


int isChangeGalaxyLocked( char *cdkey)
{
	LockNode *ln;
	int entry;

	if (!cdkey[0]) return 1;	// invalid id: lock it

	entry = getHash(cdkey) & 0xff;
	ln = userlock[entry];

	while (ln!=NULL) {
		if (ln->use != 0) {
			if( !strcmp(ln->cdkey, cdkey) && !strcmp(ln->server, "星系移民") ) {
					log(" 星系移民中 ");
				break;
			}
		}
		ln=ln->next;
	}
	if (ln!=NULL) return 1; else return 0;
}

#endif


#ifdef _TIMEOUTKICK
int ChangeMemDeadline(int entry, char *cdkey, char *deadline)
{

	LockNode *ln = userlock[entry];
	while (ln!=NULL) {
		if (ln->use != 0) {
			if (strcmp(ln->cdkey, cdkey)==0) {
				log(" 修改Deadline ");
				strcpy( ln->deadline, deadline);
				break;
			}
		}
		ln=ln->next;
	}
	if (ln!=NULL) return 1; else return 0;	
}
#endif

#ifdef _UNLOCKPOOL
void UnlockPoolProc( void)
{
	static int unlockindex =0;
	static time_t lastcheck =0;
	int cleanCount =0;


	if( sys_time < lastcheck + 3 ) // 每xx秒检查
		return;
	lastcheck = sys_time;

	//log("\n Start send UnlockPool to worker!! ");
	
	//while( UnlockPool[unlockindex].use ) {
	for( unlockindex =0; unlockindex <UNLOCKPOOLNUM; unlockindex++) {
		
		if( UnlockPool[unlockindex].use ) {

			char id[128];
			char charname[128];
			int ti;
			char process[128];
			int mesgid;

			strcpy( id ,UnlockPool[unlockindex].cdkey);
			strcpy( charname ,UnlockPool[unlockindex].charname);
			ti = UnlockPool[unlockindex].serverindex;
			strcpy( process ,UnlockPool[unlockindex].process);
			mesgid = UnlockPool[unlockindex].mesgid;

			if( addWorkerTask( id , "dummy" , "3" , dummyCallback , ti ,
				id , "dummy" , charname , process ,"", mesgid , 0 ) < 0 ) {
				log(" UnlockPool_send_to_Worker_Err!!!!:%d ", unlockindex);
				//return;
				continue;
			}
			
			//log("\n Unlockpool_send:ti(%d),cdkey(%s)", ti, id );

			memset( &(UnlockPool[unlockindex]), '\0', sizeof( UnlockPoolNode) );
			//UnlockPool[unlockindex].use =0;
			//UnlockPool[unlockindex].cdkey[0] = NULL;

			//unlockindex++;
			//unlockindex = unlockindex % UNLOCKPOOLNUM;

			cleanCount++;

			//if( cleanCount >=30) // 每次最多送X个logout
			//	break;
		}
	}

	// 一次全清归零
	iUnlockPoolAdd =0;
	unlockindex =0;

	log("\n Unlockpool send Count:%d index:%d", cleanCount, unlockindex);

}


int addUnlockPool( int serverindex, char *cdkey, char *charname, char *process, int mesgid)
{
	int i =0;
	
	while( UnlockPool[iUnlockPoolAdd].use ) {
		iUnlockPoolAdd++;
		iUnlockPoolAdd = iUnlockPoolAdd % UNLOCKPOOLNUM;
		
		i++;
		if( i >= UNLOCKPOOLNUM ) {
			log(" UnlockPool_FULL!!!!:%d ", iUnlockPoolAdd);
			return -1;
		}
	}
	
	UnlockPool[iUnlockPoolAdd].use = 1;
	UnlockPool[iUnlockPoolAdd].serverindex = serverindex;
	strcpy( UnlockPool[iUnlockPoolAdd].cdkey, cdkey);
	strcpy( UnlockPool[iUnlockPoolAdd].charname, charname);
	strcpy( UnlockPool[iUnlockPoolAdd].process, process);
	UnlockPool[iUnlockPoolAdd].mesgid = mesgid;

	log("\n add_UnlockPool:i(%d),si(%d),cdkey(%s)", iUnlockPoolAdd, serverindex, UnlockPool[iUnlockPoolAdd].cdkey );

	iUnlockPoolAdd++;
	iUnlockPoolAdd = iUnlockPoolAdd % UNLOCKPOOLNUM;

	return 0;
}

void init_UnlockPool( void)
{
	int i;
	for( i =0; i < UNLOCKPOOLNUM; i++)
		memset( &(UnlockPool[i]), '\0', sizeof( UnlockPoolNode) );
}

int inUnlockPool( char* cdkey)
{
	int i;
	for( i=0; i <UNLOCKPOOLNUM; i++) {
		if( UnlockPool[i].use ) {
			if( !strcmp( UnlockPool[i].cdkey, cdkey) )
				return 1;
		}
		//else {
		//	break;
		//}
	}
	return 0;
}

#endif
