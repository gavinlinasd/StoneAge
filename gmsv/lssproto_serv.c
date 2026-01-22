#include "version.h"
#include "autil.h"
#include "lssproto_serv.h"
#include "common.h"	// for StoneAge
#include "log.h"	// for StoneAge
#include "char.h"
#include <stdio.h>
#include <time.h>
#include <net.h>


void lssproto_SetServerLogFiles( char *r , char *w )
{
	lssproto_strcpysafe( lssproto_writelogfilename , w , sizeof(lssproto_writelogfilename ));
	lssproto_strcpysafe( lssproto_readlogfilename , r , sizeof(lssproto_readlogfilename ));
}
int lssproto_InitServer(	int (*writefunc)(int,char*,int) ,	int worksiz )
{
	int i;
	if( (void*)writefunc == NULL){lssproto.write_func = lssproto_default_write_wrap;} else {lssproto.write_func = writefunc;}
	lssproto_AllocateCommonWork(worksiz);
	lssproto_stringwrapper = (char**)calloc( 1,sizeof(char*) * MAXLSRPCARGS);
	if(lssproto_stringwrapper ==NULL)return -1;
	memset( lssproto_stringwrapper , 0, sizeof(char*)*MAXLSRPCARGS);
	for(i=0;i<MAXLSRPCARGS;i++){
		lssproto_stringwrapper[i] = (char*)calloc( 1, worksiz );
		if( lssproto_stringwrapper[i] == NULL){
			for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);return -1;}
		}
	}
	lssproto.ret_work = (char*)calloc( 1,sizeof( worksiz ));
	if( lssproto.ret_work == NULL ){ return -1; }
	return 0;
}
void lssproto_CleanupServer( void )
{
	int i;
	free( lssproto.work );
	free( lssproto.arraywork);
	free( lssproto.escapework );
	free( lssproto.val_str);
	free( lssproto.token_list );
	for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);}
	free( lssproto_stringwrapper );
	free( lssproto.ret_work );
}

#define DME() print("<DME(%d)%d:%d>",fd,__LINE__,func)

int lssproto_ServerDispatchMessage(int fd, char *encoded)
{
	int func,fieldcount;
	char raw[65500];

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	
	util_DecodeMessage(raw,encoded);

	if( !util_SplitMessage(raw,SEPARATOR) ){
		print("\nDME1:package=%s\n",raw);
		DME(); return -1;
	}
	if (!util_GetFunctionFromSlice(&func, &fieldcount)) {
		logHack(fd,HACK_GETFUNCFAIL);
		DME(); return -1;
	}
	
	if (func==LSSPROTO_W_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[65500];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, direction);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_W_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_W2_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[65500];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, direction);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_W2_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EV_RECV) {
		int checksum=0, checksumrecv;
		int event;
		int seqno;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &event);
		checksum += util_deint(3, &seqno);
		checksum += util_deint(4, &x);
		checksum += util_deint(5, &y);
		checksum += util_deint(6, &dir);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_EV_recv(fd, event, seqno, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DU_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EO_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(2, &dummy);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_EO_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}
     
	if (func==LSSPROTO_BU_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(2, &dummy);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_BU_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_JB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_JB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_LB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_LB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_B_RECV) {
		int checksum=0, checksumrecv;
		char command[65500];

		checksum += util_destring(2, command);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_B_recv(fd, command);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKD_RECV) {
		int checksum=0, checksumrecv;
		int dir;
		int index;

		checksum += util_deint(2, &dir);
		checksum += util_deint(3, &index);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SKD_recv(fd, dir, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ID_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int haveitemindex;
		int toindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &haveitemindex);
		checksum += util_deint(5, &toindex);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_ID_recv(fd, x, y, haveitemindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &dir);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PI_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int itemindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &itemindex);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DI_recv(fd, x, y, itemindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DG_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int amount;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &amount);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DG_recv(fd, x, y, amount);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int petindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &petindex);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DP_recv(fd, x, y, petindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MI_RECV) {
		int checksum=0, checksumrecv;
		int fromindex;
		int toindex;

		checksum += util_deint(2, &fromindex);
		checksum += util_deint(3, &toindex);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_MI_recv(fd, fromindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		char message[65500];
		int color;

		checksum += util_deint(2, &index);
		checksum += util_destring(3, message);
		checksum += util_deint(4, &color);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_MSG_recv(fd, index, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PMSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		int petindex;
		int itemindex;
		char message[65500];
		int color;

		checksum += util_deint(2, &index);
		checksum += util_deint(3, &petindex);
		checksum += util_deint(4, &itemindex);
		checksum += util_destring(5, message);
		checksum += util_deint(6, &color);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PMSG_recv(fd, index, petindex, itemindex, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AB_RECV) {
		int checksum=0, checksumrecv;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_AB_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DAB_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(2, &index);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DAB_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AAB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_AAB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_L_RECV) {
		int checksum=0, checksumrecv;
		int dir;

		checksum += util_deint(2, &dir);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_L_recv(fd, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_TK_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char message[65500];
		int color;
		int area;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_destring(4, message);
		checksum += util_deint(5, &color);
		checksum += util_deint(6, &area);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_TK_recv(fd, x, y, message, color, area);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_M_RECV) {
		int checksum=0, checksumrecv;
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;

		checksum += util_deint(2, &fl);
		checksum += util_deint(3, &x1);
		checksum += util_deint(4, &y1);
		checksum += util_deint(5, &x2);
		checksum += util_deint(6, &y2);
		util_deint(7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_M_recv(fd, fl, x1, y1, x2, y2);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_C_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(2, &index);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_C_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_S_RECV) {
		int checksum=0, checksumrecv;
		char category[65500];

		checksum += util_destring(2, category);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_S_recv(fd, category);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FS_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(2, &flg);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_FS_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_HL_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(2, &flg);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_HL_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PR_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int request;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &request);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PR_recv(fd, x, y, request);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KS_RECV) {
		int checksum=0, checksumrecv;
		int petarray;
		checksum += util_deint(2, &petarray);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_KS_recv(fd, petarray);
		util_DiscardMessage();
		return 0;
	}

#ifdef _STANDBYPET
	if (func==LSSPROTO_SPET_RECV) {
		int checksum=0, checksumrecv;
		int standbypet;
		checksum += util_deint(2, &standbypet);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SPET_recv(fd, standbypet);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _RIGHTCLICK
	if (func==LSSPROTO_RCLICK_RECV) {
		int checksum=0, checksumrecv;
		int type;
		char data[1024];
		checksum += util_deint(2, &type);
		checksum += util_destring(3, data);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_RCLICK_recv(fd, type, data);
		util_DiscardMessage();
		return 0;
	}
#endif

	if (func==LSSPROTO_AC_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int actionno;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &actionno);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_AC_recv(fd, x, y, actionno);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int array;
		int toindex;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &array);
		checksum += util_deint(5, &toindex);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_MU_recv(fd, x, y, array, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PS_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		int havepetskill;
		int toindex;
		char data[65500];

		checksum += util_deint(2, &havepetindex);
		checksum += util_deint(3, &havepetskill);
		checksum += util_deint(4, &toindex);
		checksum += util_destring(5, data);
		util_deint(6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PS_recv(fd, havepetindex, havepetskill, toindex, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ST_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(2, &titleindex);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_ST_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DT_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(2, &titleindex);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_DT_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FT_RECV) {
		int checksum=0, checksumrecv;
		char data[65500];

		checksum += util_destring(2, data);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_FT_recv(fd, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKUP_RECV) {
		int checksum=0, checksumrecv;
		int skillid;

		checksum += util_deint(2, &skillid);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SKUP_recv(fd, skillid);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KN_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		char data[65500];

		checksum += util_deint(2, &havepetindex);
		checksum += util_destring(3, data);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_KN_recv(fd, havepetindex, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_WN_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int seqno;
		int objindex;
		int select;
		char data[65500];

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &seqno);
		checksum += util_deint(5, &objindex);
		checksum += util_deint(6, &select);
		checksum += util_destring(7, data);
		
		util_deint(8, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_WN_recv(fd, x, y, seqno, objindex, select, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(2, &x);
		checksum += util_deint(3, &y);
		checksum += util_deint(4, &dir);
		util_deint(5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_SP_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CLIENTLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char cdkey[65500];
		char passwd[65500];
		
		strcpy(PersonalKey, _DEFAULT_PKEY);

		checksum += util_destring(2, cdkey);
		checksum += util_destring(3, passwd);
		
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		
		lssproto_ClientLogin_recv(fd, cdkey, passwd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CREATENEWCHAR_RECV) {
		int checksum=0, checksumrecv;
		int dataplacenum;
		char charname[65500];
		int imgno;
		int faceimgno;
		int vital;
		int str;
		int tgh;
		int dex;
		int earth;
		int water;
		int fire;
		int wind;
		int hometown;

		checksum += util_deint(2, &dataplacenum);
		checksum += util_destring(3, charname);
		checksum += util_deint(4, &imgno);
		checksum += util_deint(5, &faceimgno);
		checksum += util_deint(6, &vital);
		checksum += util_deint(7, &str);
		checksum += util_deint(8, &tgh);
		checksum += util_deint(9, &dex);
		checksum += util_deint(10, &earth);
		checksum += util_deint(11, &water);
		checksum += util_deint(12, &fire);
		checksum += util_deint(13, &wind);
		checksum += util_deint(14, &hometown);
		util_deint(15, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CreateNewChar_recv(fd, dataplacenum, charname, imgno, faceimgno, vital, str, tgh, dex, earth, water, fire, wind, hometown);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARDELETE_RECV) {
		int checksum=0, checksumrecv;
		char charname[65500];

		checksum += util_destring(2, charname);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CharDelete_recv(fd, charname);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char charname[65500];

		checksum += util_destring(2, charname);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CharLogin_recv(fd, charname);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLIST_RECV) {
		int checksum=0, checksumrecv;
#ifdef _PKSEVER_VER
		int star = 0;
		util_deint(2, &checksumrecv);
		util_deint(3, &star);
#else
		util_deint(2, &checksumrecv);
#endif
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}

//#ifdef _PKSEVER_VER
//		lssproto_CharList_recv( fd, star);
//#else
		lssproto_CharList_recv( fd);
//#endif

		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGOUT_RECV) {
		int checksum=0, checksumrecv;
		int Flg=1;
#ifdef _CHAR_NEWLOGOUT
		checksum += util_deint(2, &Flg);
		util_deint(3, &checksumrecv);
#else
		util_deint(2, &checksumrecv);
#endif
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CharLogout_recv(fd, Flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PROCGET_RECV) {
		int checksum=0, checksumrecv;
		
		strcpy( PersonalKey, _DEFAULT_PKEY);

		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_ProcGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PLAYERNUMGET_RECV) {
		int checksum=0, checksumrecv;
		util_deint(2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_PlayerNumGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ECHO_RECV) {
		int checksum=0, checksumrecv;
		char test[65500];

		checksum += util_destring(2, test);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_Echo_recv(fd, test);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SHUTDOWN_RECV) {
		int checksum=0, checksumrecv;
		char passwd[65500];
		int min;

		checksum += util_destring(2, passwd);
		checksum += util_deint(3, &min);
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_Shutdown_recv(fd, passwd, min);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_TD_RECV) {
		int checksum=0, checksumrecv;
		char message[65500];

		checksum += util_destring(2, message);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_TD_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FM_RECV) {
		int checksum=0, checksumrecv;
		char message[65500];

		checksum += util_destring(2, message);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_FM_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}
    
	if (func==LSSPROTO_PETST_RECV) {
		int checksum=0, checksumrecv;
		int nPet;
		int sPet;		

		checksum += util_deint(2, &nPet);
		checksum += util_deint(3, &sPet);		
		util_deint(4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_PETST_recv(fd, nPet, sPet);		
		util_DiscardMessage();
		return 0;
	}
    
	// _BLACK_MARKET
	if (func==LSSPROTO_BM_RECV) {
		int checksum=0, checksumrecv;
		int iindex;		

		checksum += util_deint(2, &iindex);		
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_BM_recv(fd, iindex);		
		util_DiscardMessage();
		return 0;
	}

#ifdef _FIX_DEL_MAP           // WON ADD 俙模喲華芞冞潼郜
	if (func==LSSPROTO_DM_RECV) {
	    char buffer[2];
	    buffer[0] = '\0';
		lssproto_DM_recv( fd );
		util_DiscardMessage();
		return 0;
	}

#endif

#ifdef _CHECK_GAMESPEED
	if (func==LSSPROTO_CS_RECV) {
	    char buffer[2];
	    buffer[0] = '\0';
		lssproto_CS_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _TEAM_KICKPARTY
	if ( func == LSSPROTO_KTEAM_RECV ) {
		int checksum = 0, checksumrecv;
		int sindex;
		checksum += util_deint( 2, &sindex);
		util_deint( 3, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_KTEAM_recv( fd, sindex);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _MIND_ICON
	if(func==LSSPROTO_MA_RECV){
		int checksum = 0, checksumrecv;
		int nMind;
		int x, y;
		
		checksum += util_deint( 2, &nMind);
		checksum += util_deint( 3, &x);
		checksum += util_deint( 4, &y);
		util_deint( 5, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_MA_recv(fd, x, y, nMind);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _CHATROOMPROTOCOL			// (祥褫羲) Syu ADD 謐毞弅け耋
	if (func==LSSPROTO_CHATROOM_RECV) {
		int checksum=0, checksumrecv;
		char test[65500];

		checksum += util_destring(2, test);
		util_deint(3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			logHack(fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_CHATROOM_recv(fd, test);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _NEWREQUESTPROTOCOL			// (祥褫羲) Syu ADD 陔崝Protocol猁⑴牉砐
	if ( func==LSSPROTO_RESIST_RECV ) {
	    char buffer[2];
	    buffer[0] = '\0';
		lssproto_RESIST_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _OUTOFBATTLESKILL			// (祥褫羲) Syu ADD 準桵須奀撮夔Protocol
	if(func==LSSPROTO_BATTLESKILL_RECV){
		int checksum = 0, checksumrecv;
		int iNum;
		
		checksum += util_deint( 2, &iNum);
		util_deint( 3, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
			DME(); return -1;
		}		
		lssproto_BATTLESKILL_recv(fd, iNum);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _STREET_VENDOR
	if(func == LSSPROTO_STREET_VENDOR_RECV){
		int checksum = 0,checksumrecv;
		char message[65500];

		checksum += util_destring(2,message);
		util_deint(3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_STREET_VENDOR_recv(fd,message);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _JOBDAILY
	if(func == LSSPROTO_JOBDAILY_RECV){
		int checksum = 0,checksumrecv;
		char buffer[16384];

		buffer[0] = '\0';
		memset(buffer,0,16384); //kkkkkkkkk
		checksum += util_destring(2,buffer);
		util_deint(3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		print("\njobdaily:%s ",buffer);
		lssproto_JOBDAILY_recv(fd,buffer);
		util_DiscardMessage();
		return 0;

	}
#endif
#ifdef _TEACHER_SYSTEM
	if(func == LSSPROTO_TEACHER_SYSTEM_RECV){
		int checksum = 0,checksumrecv;
		char message[65500];

		checksum += util_destring(2,message);
		util_deint(3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
			DME(); return -1;
		}
		lssproto_TEACHER_SYSTEM_recv(fd,message);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _ADD_STATUS_2
	if(func == LSSPROTO_S2_RECV){
		return 0;
	}
#endif

	util_DiscardMessage();
	logHack(fd,HACK_NOTDISPATCHED);
	DME(); return -1;
}

/*
   servertoclient XYD( int x, int y, int dir );
 	Д勗橋  敖    勗鏤畛騷媕↓摯痾  禱  ぅ堆埰堎倜誧勗窅竣��
*/
void lssproto_XYD_send(int fd,int x,int y,int dir)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, dir);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_XYD_SEND, buffer);
}

void lssproto_EV_send(int fd,int seqno,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, seqno);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EV_SEND, buffer);
}

void lssproto_EN_send(int fd,int result,int field)
{
	char buffer[65500];
	int checksum=0;

	//print(" EN_send ");

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, result);
	checksum += util_mkint(buffer, field);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EN_SEND, buffer);
}


void lssproto_RS_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RS_SEND, buffer);
}


void lssproto_RD_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RD_SEND, buffer);
}

void lssproto_B_send(int fd,char* command)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, command);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_B_SEND, buffer);
}

void lssproto_I_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_I_SEND, buffer);
}

void lssproto_SI_send(int fd,int fromindex,int toindex)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fromindex);
	checksum += util_mkint(buffer, toindex);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SI_SEND, buffer);
}

void lssproto_MSG_send(int fd,int aindex,char* text,int color)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, aindex);
	checksum += util_mkstring(buffer, text);
	checksum += util_mkint(buffer, color);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MSG_SEND, buffer);
}


void lssproto_PME_send(int fd,int objindex,int graphicsno,int x,int y,int dir,int flg,int no,char* cdata)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, objindex);
	checksum += util_mkint(buffer, graphicsno);
	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, dir);
	checksum += util_mkint(buffer, flg);
	checksum += util_mkint(buffer, no);
	checksum += util_mkstring(buffer, cdata);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PME_SEND, buffer);
}


void lssproto_AB_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_AB_SEND, buffer);
}


void lssproto_ABI_send(int fd,int num,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, num);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ABI_SEND, buffer);
}

void lssproto_TK_send(int fd,int index,char* message,int color)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, index);
	checksum += util_mkstring(buffer, message);
	checksum += util_mkint(buffer, color);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TK_SEND, buffer);
}

void lssproto_MC_send(int fd,int fl,int x1,int y1,int x2,int y2,int tilesum,int objsum,int eventsum,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fl);
	checksum += util_mkint(buffer, x1);
	checksum += util_mkint(buffer, y1);
	checksum += util_mkint(buffer, x2);
	checksum += util_mkint(buffer, y2);
	checksum += util_mkint(buffer, tilesum);
	checksum += util_mkint(buffer, objsum);
	checksum += util_mkint(buffer, eventsum);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MC_SEND, buffer);
}


void lssproto_M_send(int fd,int fl,int x1,int y1,int x2,int y2,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, fl);
	checksum += util_mkint(buffer, x1);
	checksum += util_mkint(buffer, y1);
	checksum += util_mkint(buffer, x2);
	checksum += util_mkint(buffer, y2);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_M_SEND, buffer);
}

/*
   servertoclient C( string data );
       (Characters)
       袲溘騷囮璃哱毀 CA 誑  趙��鼠摯す籤溘勗僑笢趙ぅ埵凝埱倜鄹毀�� C 
       ぁ喫摯匴倳禱菺滑ぁ拻堎�袾荂懽儱棺鯆慪濛阹ぴ聿苺�
       
 	string data
 		  啞б↓悃蚗哱毀 雄き摯3攣  摯囂  禱昡璃悃ぁ埵�梠迖�
 	    忒摯ぁ堣堎.鼠躂�葸噯凰�  摯  �鼒晾鮵梇槱袘脊�'|'ぁ�尪瘓桫�
 	    趙笢堎��
 	    
 	      倜趙摲ぁ�尪瘓桫憶豝�↓袲璃誑12蠀摯鳹譴
 		  哱↓袲璃摯    毀
 
 		  WHICHTYPE|CHARINDEX|X|Y|DIR|BASEIMG|LEVEL|NAMECOLOR|NAME|SELFTITLE|WALKABLE|HEIGHT|POPUPNAMECOLOR
 		  喫摯酘々喇攝袲哱毀す籤溘袲淏↓ぁ堣堎敁笢竣喫敁ぁ堣堎��
 		  
 		  WHICHTYPE 毀�摀蟦副褐珝蹦邯�↓誑汔竣笢埱倜攣  摯忒摯
 		  ぁ堣堎凝�蛦蟤噸椅荂懽鵅嚾扑普紜掉健蔥仃敻鏍蠸鯆噱胱俶瞿�
 		  
 		typedef enum
 		{
 		    CHAR_TYPENONE,          笰ぁ忒埵笢
 		    CHAR_TYPEPLAYER,        鏤畛騷媕↓
 		    CHAR_TYPEENEMY,         傍
 		    CHAR_TYPEPET,           妐蚗哱
 		    CHAR_TYPEDOOR,          迶囮
 		    CHAR_TYPEBOX ,              
 		    CHAR_TYPEMSG ,          娹  
 		    CHAR_TYPEWARP ,         極↓鏤崥↓璃
 		    CHAR_TYPESHOP ,         壨
 		    CHAR_TYPEHEALER ,       樅↓溘↓
 		    CHAR_TYPEOLDMAN ,       荇  
 		    CHAR_TYPEROOMADMIN,     箾  Е碟
 		    CHAR_TYPETOWNPEOPLE,    竘з摯痲敁
 		    CHAR_TYPEDENGON,        魕虭  
 		    CHAR_TYPEADM,           魕虭  
 		    CHAR_TYPETEMPLE,        Temple master
 		    CHAR_TYPESTORYTELLER,   鞅堇  
 		    CHAR_TYPERANKING,       箾  Е溘璃す璃精  崚      
 		    CHAR_TYPEOTHERNPC,      鼠摯眥摯號喙葡濯勗埵�欷聿羑PC
 		    CHAR_TYPEPRINTPASSMAN,  迶囮摯蚕筒極↓迶  崚埰堎NPC
 		    CHAR_TYPENPCENEMY,      砡擭傍
 		    CHAR_TYPEACTION,        囮袲で砫璃勗  伀埰堎NPC
 		    CHAR_TYPEWINDOWMAN,     坳贖璃迶坳  崚埰堎NPC    筒哱凝忒)
 		    CHAR_TYPESAVEPOINT,     掛↓々睽騷璃哱
 		    CHAR_TYPEWINDOWHEALER,  坳騷璃迶坳淏騷鏤摯樅↓溘↓
 		    CHAR_TYPEITEMSHOP,	    堁壨
 		    CHAR_TYPESTONESHOP,	    歕  碟  妐蚗哱摯  碟  
 		    CHAR_TYPEDUELRANKING,   DUEL溘璃す璃精NPC
 		    CHAR_TYPEWARPMAN,	    極↓鏤悃璃NPC
 		    CHAR_TYPEEVENT,	    騷穫璃哱NPC
 		    CHAR_TYPEMIC,	    騷穫璃哱NPC
 		    CHAR_TYPELUCKYMAN,	    騷穫璃哱NPC
 		    CHAR_TYPEBUS,	    悃璃さ筒泬筒
 		    CHAR_TYPECHARM,	    騷穫璃哱NPC
 		    CHAR_TYPENUM,
 		}CHAR_TYPE;
 		  
 		  鼠躂毀眥摯鏤畛騷媕↓盓NPC盓傍ぁ堣堎�袾荂懽鵅齠瓦近�
 		  埰堎敁拻毀峟窇堎  ⑹勗僑笢趙蠈  邞嚭埰堎�蛫棍�
 		  堇袲溘騷囮璃哱毀��喫摯蚕艇蚗哱禱偏崹敁埱倜敁拻勗喫摯
 		  蚕艇蚗哱勗怳凝躂趙笢埵笢す籤溘禱  埱趙笢倜�殏欷し胰�
 		  竘埱趙源笢敁笢竣喫敁ぁ堣堎�覗�倜��悃坳筒庈↓藺鳴禱堣
 		  渦鞠倜敁拻勗  崚埰堎攷  毀喫摯攷  摯陑勗盻笢趙笢堎��
 		  煦凝�掁�悃坳筒庈↓藺鳴禱堣渦鞠倜敁拻勗  崚埰堎喫敁誑
 		    脾踏躂倜鳹譴毀���荂懽鵅黦提�喫摯匴倳禱    黍勗蠙怮
 		  煦踏埵崹躂�屻牟欷聿苺� SELFTITLE勗僑笢趙毀��溢惉蕾淏
 		  ぁ堣堎倜趙摲禱謂�弅�埵笢源竣勗操筒艇↓鏤踏躂趙笢堎��
 		  袲溘騷囮璃哱毀囮袲で砫璃摯昡悃璃迶(CA)誑�彸觴�ぁ毀  
 		  зぁ  崚埰堎�羡HARINDEX�荂懽鵅�  摯郖徽勗す籤溘禱  
 		  擭ぁ拻堎  蠕��BASEIMG毀  崚摯倜誧摯  蠕��LEVEL毀す籤
 		  溘摯畛穫鳴(0埵��  崚媃埵笢�蛦蟦偽懋就PC埵汔勗窅竣��)
 		  WALKABLE毀1摯敁拻鼠摯窀禱玊謐埰堎喫敁誑ぁ拻��0埵�桾�
 		  謐埰堎喫敁誑ぁ拻埵笢�胾EIGHT毀禜踏禱忒僑忒摯凝鼠竣ぁ
 		  埵笢摯凝摯炩擭��
 		  
 		  す籤溘袲淏↓摯  鞳敁猷  措蠕毀��<a href="#escaping">
 		  操筒艇↓鏤踏躂埵崹躂�屻牟欷聿苺�'|'ぁ哱↓袲璃禱
 		  噁堇煦媃趙凝�桴瑑弩央劗鬊待怏笪彸瞿蛨瑑弩央劗鯥彸蟲�'
 		  |'誑酵凝摯  棬勗堁拻凝渦堎摯ぁ��  疐毀ワ梖勗'|'禱溢
 		  惉蕾淏敁媃趙源笢�袷近鵋彸�  忒��  鞳敁猷  措蠕禱操筒
                   艇↓鏤媃趙凝�桸袘脊葅旦楮聿陊札旅梖近鵋彸瞿�
                   竘倜�楟該檗�摯鳹譴毀猷  措蠕摯瓟渦堇勗蝠↓勒↓誑优擭
                   媃倜妐蚗哱摯  鞳誑邞嚭踏躂趙�彸瞿�
 		  
 	      倜趙摲ぁ�尪瘓桫憶豝�↓袲璃誑6蠀摯鳹譴
 		  哱↓袲璃摯    毀
 		  INDEX|X|Y|BASEIMG|LEVEL|ITEM1LINEINFO
 		  ぁ貍  勗  з趙笢堎囮騷  堜勗僑笢趙摯攷  ぁ堣堎��
 		  INDEX毀す籤溘摯騷璃溢袲蚗筒敁凝屾�欷聿粁NDEXぁ堣堎��
 		  鼠摯囮騷  堜禱觛埰鄹勗窅暵埰堎�舼,Y毀囮騷  堜摯精痲↓
 		  泬鳴痾  �羠ASEIMG毀  鴢摯  蠕�胔TEM1LINEINFO毀1菜
 		  info勗  崚埰堎倜誧摯攷  ぁ堣堎�袼岌�  堜坳騷璃迶坳  
 		  摯  崚暵摯攷  毀  摯  傮ぁ暵徽埰堎�袼岌�  堜勗匴媃趙
 		  毀CA毀  埵笢�胔TEM1LINEINFO毀<a href="#escape">操筒艇↓
 		  鏤踏躂堎�蛦蟦偏瑑弩央劗騧�  傮毀窀摯囂  禱落敺��
 
 	      倜趙摲ぁ�尪瘓桫憶豝�↓袲璃誑4蠀摯鳹譴
 		  哱↓袲璃摯    毀
 		  INDEX|X|Y|VALUE
 		  ぁ貍  勗  з趙笢堎堁鉣勗僑笢趙摯攷  ぁ堣堎��    毀埰
 		  迋趙倳棬�胔NDEX,X,Y 毀囮騷  堜敁  啋�臹ALUE 毀汔躂煦
 		  崹摯倵凝敁笢竣痀ぁ堣堎�袼岌�  堜勗僑笢趙摯攷  ぁ堣堎��
 	      倜趙摲ぁ�尪瘓桫憶豝�↓袲璃誑1蠀摯鳹譴
 		  INDEX
 		  喫摯す籤溘摯C毀砐窇�桫撻聿苺�
*/
void lssproto_C_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_C_SEND, buffer);
}

/*
   servertoclient CA( string data );
       (CharacterAction)
       峟窇堎  ⑹勗笢堎す籤溘摯囮袲で砫璃Щ稃禱脾犍埰堎��
       �荂懽鵅齙旅殏芛薩昃尬�哱勗郖  黍勗邞嚭埰堎��
       掘す籤溘摯1囮袲で砫璃�婠蝐樆近韗袾荂懽鵅黦棵抄芢刳鉏�禱ぁ拻堎煦
       崹酕謝埰堎喫敁��
       
 	string data
 	CHARINDEX|X|Y|ACTION|PARAM1|PARAM2|PARAM3|PARAM4|....禱昡
 	    璃悃ぁ�尪碣�倜忒摯勗埰堎�腍ARAM毀囮袲で砫璃�婠蝐殣覕捗�
 	    窅笢凝倜忒喫敁埵堎�蛚紜捧棵抄芢刳鉏�郖  �舼,Y毀痾  ぁ汔
 	    摯囮袲で砫璃ぁ忒��堣倜�欷ひ党�  禱炩擭埰堎��
 	    喫摯  棬  毀操筒艇↓鏤踏躂埵笢��
 	  
 	        ACTION  PARAM1  PARAM2 
 	       PARAM3  PARAM4  
 	        Stand:0    澺0~7        
 	        Walk:1    澺0~7        
 	        Attack:2    澺0~7        
 	        Throw:3    澺0~7        
 	        Damage:4    澺0~7      
 	        Dead:5    澺0~7        
 	        UseMagic:6    澺0~7  
 	        UseItem:7    澺0~7  
 	        Effect:8    澺0~7  操啞攝袲哱  蠕  
 	        Down:10 (槸躂堎)    澺0~7  
 	        Sit:11 (淢堎)    澺0~7  
 	        Hand:12 (鱗禱�擸�)    澺0~7  
 	        Pleasure:13 (熬屾)    澺0~7  
 	        Angry:14 (曹堎)    澺0~7  
 	        Sad:15 (  媃資)    澺0~7  
 	        Guard:16 (票↓迶)    澺0~7  
 	        actionwalk:17 (囮袲で砫璃暵倵拻)    澺0~7  
 	        nod:18 (竣埵囀��)    澺0~7  
 	        actionstand:19 (囮袲で砫璃暵  з睽↓扲)    澺0~7  
 	        Battle:20 (橋  攷  )    澺0~7  BattleNo(-1 埵��  崚觛埰    SideNo  HelpNo  兒埵�桴戫篜拜馨愅A  崚�搢萶牟殏梊呁椹�倜毀  媃    
 	        Leader:21 (惉↓譫↓攷  )    澺0~7  0:  崚觛埰 1:  崚  
 	        Watch:22 (橋  め橋)    澺0~7  0:  崚觛埰 1:  崚  
 	        namecolor:23(  鞳摯褗攷  )    澺0~7    鞳摯褗  蠕  
 	        Turn:30(  澺  儒)    澺0~7    
 	        Warp:31(極↓鏤)    澺0~7    
 	      
 	ACTION摯偯毀堆倳ぁ��    毀窀摯  摯嫌輊摯囂  ぁ堣堎��
*/
void lssproto_CA_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CA_SEND, buffer);
}

/*
   servertoclient CD( string data );
       (CharacterDelete)
 	data 毀溢惉蕾淏  ,  ぁ鉏憍�桫憶袎抯�溢蚗袲筒��
 	  倳贀邞堎痀誑③  堎��
       喫摯ID禱  埱趙笢堎す籤溘袲淏誑觛窇倜鄹勗�荂懽嚝旅殏芛薩昃尬�哱喜
       敁魕渦堎��
*/
void lssproto_CD_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CD_SEND, buffer);
}

/*
   <a name="R">servertoclient R( string data );
       (Radar)
       畛↓譫↓摯    禱袲溘騷囮璃哱勗邞嚭埰堎�詗芛薩昃尬�哱毀喫摯邞嚭禱
       菺滑埰堎喫敁毀埵笢�袾荂懽鵅齠目剉紲耳�騷蕾璃精ぁ邞嚭埰堎��
       倜敁窇��10倵堣堎�岓婠覢蝢恐�1覕�婠覢蝢恐�
 
 	string data
 	x ,y, kind ,摯棹勗偯禱'|'ぁ�尪碣�趙埵�梠迖訧祤冕�
 	    踏�梇�'|'ぁ埵�梠迖訧祤除�
 	    x,y毀す籤溘凝�梩啣孛翎�  �菣ind毀雄き勗媃誧埰倳偯ぁ毀埵笢
 	    楔蠕�覗薄囃腹麙撋允椒�  摯攣  禱炩擭埰堎�袷糾桫噱紙彸薹祤�
 	    摯攣  毀��鼠躂竘ぁ勗�樠�倜  傮敁凝��筒す鳴ぁ  撬媃��鼠躂毀
 	    �荂懽鵅齠立棴�堎��
 	 
 	        kind摯偯        
 	        E  傍  
 	        P  鏤畛騷媕↓  
 	        S  ��  
 	        G  堁鉣  
 	        I  囮騷  堜(�蛚純�)  
   
   data摯  ※"12|22|E|13|24|P|14|28|P"
   喫摯  棬  毀操筒艇↓鏤踏躂埵笢��
*/
void lssproto_R_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_R_SEND, buffer);
}

/*
   servertoclient S( string data );
       (Status)
       す籤溘摯筒  ↓淏筒禱邞嚭埰堎��
       溢↓淏毀 庈  湖惉楔蠕  棬(郖  棬)     敁埵埱趙笢堎�蛫棍�堇  疐摯
       1  棬禱峟躂�屼牳做�  ↓淏筒凝覕堎��    毀2  棬  凝�梪目夭瞿�
           毀雄き摯啞б↓悃蚗哱勗媃倜誑竣�覕袘脊葽勾�'|'誑溢惉蕾淏ぁ堣堎��
       2蠀  雄禢摯哱↓袲璃誑    ぁ堣堎��
       
 	P 蠈蚕溘隍↓淏
 	    kubun hp maxhp mp maxmp str tough exp maxexp exp 
 	    level attackpower deffencepower 
 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
 	    gold 痹崹趙笢堎
 	    措蠕摯index   鞳 猷藽措蠕 
 
 	    溢惉蕾淏毀 '|' ぁ堣堎�覗�倜  鞳敁猷藽措蠕毀��
 	    <a href="#escaping">操筒艇↓鏤媃倜忒摯禱
 	    倜趙摲ぁ僑埵ど倜忒摯誑玊嚭踏躂堎��
 	    鼠躂�葸噯偽擘做廗普紜悵�
 	    kubun 勗兒誑  埱趙笢堎敁鞳蚕溘隍↓淏禱邞嚭��
 	    兕雄窀煦敁�搊補縎檗�摯  埱趙笢堎忒摯摯蚕溘隍↓淏誑棹  勗  埱趙邞嚭踏躂趙笢堎��
 	     窇�氿搢熐�  倜�掊p敁Maxhp誑kubun 雄禢摯哱↓袲璃勗  誧�桫噱胱俶瞿�
 	    
 	        kunun  int  
 	        Hp(    )hp  int  
 	        MaxHp  int  
 	        Mp(��絊蚕極↓)  int  
 	        MaxMp  int  
 	        Vital(    )  int  
 	        Str(    )  int  
 	        Tough(橀嶁踏)  int  
 	        Dex(搫傲踏)  int  
 	        Exp(歲雁偯)exp  int  
 	        MaxExp(べ竘ぁ摯歲雁偯)  int  
 	        Level(畛穫鳴)  int  
 	        Attack(  賓  )  int  
      	        Defense(陰    )  int  
 	        fQuick(  敖搫傲踏)  int  
 	        fCharm(    )  int  
 	        fLuck(痶)  int  
 	        fEarth(貍)fixearthat  int  
 	        fWater(  )  int  
 	        fFire(庄)  int  
 	        fWid(諿)fixwindat  int  
 	        Gold(堁鉣)  int  
 	        措蠕摯index  int  
 	        溢堥操鳴睽騷璃哱  int  
 	        梱牁樊倳  int  
 	          鞳    棬    
 	        猷藽措蠕    棬    
 	    
 	    沓    ( char 摯    鼠摯竘竘怳�� )
 	    
 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|喜庌毞|abc|def
 	
 	C 淢  
 	    floor maxx maxy x y
 	    禱  | ぁ鉏憍埱趙邞堎��
 	    沓    ( char 摯    鼠摯竘竘怳�� )
 	    
 	    C1024|100|100|10|20
 	    
 	    窀楔摯  ぁ毀��啞痲囮ID1024, �蚅昃�100x100 X 10,Y 20摯敁喫
 	    Й勗す籤溘袲淏↓誑笢堎敁笢竣    勗埵堎�蛦蟦參乖昄褐珝翹偽�
 	    痲囮忒謂�妡役赹�  禱袲溘騷囮璃哱誑蠐堎喫敁誑ぁ拻堎摯毀喫摯
 	    昡悃璃迶摯陑ぁ堣堎�覕硩撘啁灠窾敦袚藡�  摯痾  媃凝渦凝�欷�
 	    笢�蛪諂�趙��斛↓堜楣鴽鄹��敕騤窅暵摯敁拻��極↓鏤摯敁拻埵汔
 	    毀��喫摯昡悃璃迶禱僑凝埱趙諜媃笢偯禱邞嚭媃埵�妡窈楮牟欷聿苺�
 	    
 	I 蠈囮騷  堜
 	    (囮騷  堜1)|(囮騷  堜2)...(囮騷  堜n)
 	    囮騷  堜摯    毀雄き摯  傮ぁ蚕蚗袲埰堎��
 	    痲敁僑摯囮騷  堜毀��凝埵�桫痗紜掉�6僑摯哱↓袲璃摯掛蚗哱ぁ
 	    邞嚭踏躂堎�蛪藬蝜晰藬蝜智�    毀��
 	    
 	      鞳|  鞳2|筒  ↓喇|囮騷  堜摯    |囮俓隍  蠕|窅暵菴  鳹鐎|窅暵葡濯|囮騷  堜畛穫鳴|啞溘精  
 	    
 	      囮騷  堜  毀纂  畛穫鳴ぁ猷  黍勗  脾踏躂堎��
 	        鞳2毀纂  畛穫鳴勗源埱趙��囮騷  堜摯    誑  堎  擭��
 		  袲溘騷囮璃哱摯囮騷  堜  摯2菜  勗  躂堎痀��
 	      筒  ↓喇毀  鞳摯褗禱袑堎摯勗窅竣��
 	      囮騷  堜摯    ��
 	      囮俓隍  蠕毀��  鴢  蠕��
 	      窅暵菴  鳹鐎敁毀�摀蟦動岌�  堜誑汔喫ぁ窅暵埰堎痀誑菴  凝誑  埱趙笢堎��
 	          喫躂毀�朠荂懽鵅嚾扑普紜掉健蔥仃敻鏍蠸鯆噱胱俶瞿�
 	          
 			typedef enum
 			{
 				ITEM_FIELD_ALL,			埰迋趙摯鳹鐎ぁ窅窇堎
 				ITEM_FIELD_BATTLE,		橋    摯陑
 				ITEM_FIELD_MAP,			玊橖悃蚗鏤窀摯陑
 
 			}ITEM_FIELDTYPE;
 	          
 	      窅暵葡濯敁毀�摀蟦動岌�  堜禱窅暵埰堎喫敁摯③  堎葡濯誑  埱趙笢堎��
 	          �荂懽鵅嚾扑普紜掉健蔥仃敻鏍蠸鯆噱胱俶瞿�
 	          
 			typedef enum
 			{
 				ITEM_TARGET_MYSELF,		猷覕摯陑
 				ITEM_TARGET_OTHER,		眥摯硞  猷覕硈資)
 				ITEM_TARGET_ALLMYSIDE,		    蠈  
 				ITEM_TARGET_ALLOTHERSIDE,	Ъ鱗插蠈  
 				ITEM_TARGET_ALL,		蠈趙
 			}ITEM_TARGETTYPE;
 		
   	        <font size=+1>喫摯倳棬勗100禱鵿埰敁�楦婽玅皮俶繚晉硌窕痸諑耒瞿�
 	      囮騷  堜畛穫鳴�蛦蟦曼謄牴龠耆�摯榆ぁ埵笢敁薔  ③  埵笢��
 	      啞溘精  �貀て瓮旭袚繙踼屎近鵋彸瞿蛘補縎檗�摯葡伀毀雄き摯玊堇��

 		      0bit    妐蚗哱隍↓鳴ぁ邞嚭菴  埵囮騷  堜凝汔竣凝�袷近騊�  煦敁兒�菬葽笥褑近鮿用槾聿苺�  
 		      1Bit    喫摯囮騷  堜誑譴埬③  堎凝汔竣凝��  
 		      2Bit    喫摯囮騷  堜誑    凝汔竣凝��1煦敁    ぁ堣堎��  
 	    
 	    喫喫摯  ぁ毀<a href="#escaping">操筒艇↓鏤埰堎��
 	    沓    ( char 摯    鼠摯竘竘怳�� )
 	    
 	    I堣酵 埵 碳|str+2|1|堣酵\|笢啋|10|2|0
 	    
 	    筒  ↓喇摯偯摯徽  毀��堆倳偯ぁ
 	    
 	        1  筒  ↓喇A  
 	        2  筒  ↓喇B  	      
 	        3  筒  ↓喇C  
 	        4  筒  ↓喇D  
 	    
 	    敁埰堎�覗�倜��囮騷  堜  摯昶摯  覕勗僑笢趙毀��5蠀摯哱↓袲璃誑
 	    昶ぁ��僑竘堇倜趙摲誑埵�梛牁秷邁媃瓦近鬅鯆噱紙彸瞿�
 	
 	S 蠈筒す鳴
 	    (筒す鳴0)|(筒す鳴1)|(筒す鳴2) ... (筒す鳴n)
 	    措蠕敁  啋ぁ��す籤溘誑忒趙堎    筒す鳴倳覕禱邞嚭埰堎�蛫棍�
 	    堇埵笢忒摯毀 ||| 敁埵堎摯ぁ堣堎��
 	    掘操璃哱惉摯    毀��
 	    筒す鳴摯攣  昡↓迶|畛穫鳴
 	    ぁ堣堎��
 	    沓    ( char 摯    鼠摯竘竘怳�� )
 	    
 	    S10|2|20|2|||
 	
 	T 措蠕
 	    (措蠕0)|(措蠕1)|(措蠕2) ... (措蠕n)
 	    措蠕摯操璃哱惉誑昶摯鳹譴毀 '|' 誑  襄埰堎摯ぁ��  襄媃倜忒
 	    摯禱    媃趙筒す籤璃媃趙毀埵�欷聿苺蛨框牟桫�    蠀倳覕邞嚭
 	    埰堎��
 	    痲敁僑��痲敁僑摯    毀��
 	      鞳
 	    摯陑ぁ堣堎��
 	    沓    ( 邞�桫噱紙彸� char 摯    鼠摯竘竘ぁ堣堎 )
 	    
 	    T堣酵|源埱毞�欶�
 	    
 	    喫躂忒<a href="#escaping">操筒艇↓鏤埰堎��
 
 	M   覕蚕溘隍↓淏
 	    す籤溘袲淏↓摯筒  ↓淏筒摯竣з��    勗  脾踏躂堎忒摯(藯凜
 	    偯)敁鼠竣ぁ埵笢忒摯(    偯埵汔)誑堣堎�袶�躂凝摯筒  ↓淏筒
 	    誑  脾踏躂堎倜怮勗  樊蠈  摯蚕溘隍↓淏禱邞嚭埰堎摯毀汜蚗哱
 	    極↓袲哱溘啞贖蚗袲禱  藗勗窅竣喫敁勗埵堎摯ぁ��  勗HP ,
 	    MP,EXP摯偯煦崹勗媃堨埱倜蚕溘隍↓淏邞嚭昡悃璃迶禱暵徽埰堎��
 	    鼠躂誑喫摯M昡悃璃迶ぁ堣堎��2  棬  雄禢摯  郖哱↓袲璃毀HP摯
 	    藯凜偯 ,     哱↓袲璃毀MP��  裒哱↓袲璃毀EXPぁ堣堎��
 	    雄き勗沓    禱媃誧埰��
 	    
 	    M54|210|8944909
 	    
 	    喫摯  ぁ毀HP摯藯凜偯誑54��MP摯藯凜偯誑210��歲雁偯誑8944909
 	    勗埵埱趙笢堎摯煦�蛦蟦�3攣  毀  勗  脾  梣誑禜笢敁  渦躂堎
 	    倜誧    媃倜昡悃璃迶撬媃倜誑��忒зЙ庌喫摯M昡悃璃迶摯凝渦
 	    堇勗P昡悃璃迶ぁ蠈蚕溘隍↓淏禱邞嚭媃趙忒源笢�覕袚笭ゞ�  媏
 	    勗汜蚗哱極↓袲禱窅竣倜誧勗毀��喫摯昡悃璃迶禱窅竣喫敁誑籓棱
 	    踏躂堎��
 	    
         D す籤溘摯ID
 	    痲精騷璃鄹勗煦崹邞堎す籤溘摯index�搮孩憊衚暋甝屎肴瞿�
 	    D1000|912766409敁凝ぁ邞�桫戰瞿�
 	
 	E 操璃庈坳璃哱賃    き藦)|(窀藦)
 	    操璃庈坳璃哱  禱邞嚭埰堎�蛝�  毀n/100
 	    袲溘騷囮璃哱毀�暔梃袧偽擰旅桮拌�↓哱媃趙�搢閛硞硜�  勗+1媃倜賃  ぁ操璃庈坳璃哱埰堎凝禱  茯埰堎��
 	    倜煦媃�椿�藦摯偯禱唅窇埵笢源竣勗埰堎��
 	    べ樊勗喫躂誑邞�桫噱紙彸觴�ぁ喫摯賃  ぁ操璃庈坳璃哱禱銓猀埰堎痀��
 	
 	J0   J6 窅窇堎��絊摯    
 	    窅窇堎��絊摯    禱邞嚭埰堎��
 	    0   6 毀鼠躂�葸彊岌�  堜摯薔  郪鐎勗葡伀媃趙笢堎�觝�  摯薔  郪鐎    ��  �暙鴥暕疋�    
 	    媃凝薔  ③  埵笢  擭煦誑�暔�  錚耦摯諱勗蠈趙摯薔  郪鐎摯攷  禱邞堎
 	    J0|kubun|mp|field|target|name|comment
 	    敁笢竣啞б↓悃蚗哱勗埵埱趙笢堎��
 	    kubun 毀鼠摯  誑堣堎凝  笢凝�菬葽笥�  笢�蛫孩噤舀炱槱�↓袲璃埰�欷聿苺菬鈮笥諑夭瞿�
 	    mp毀觛  縈  禱  埰��
 	    field毀汔摯鳹鐎ぁ窅窇堎凝�袾荂懽鵅嚾扑普紜掉�  勗擭壛踏躂趙笢堎��
 	    
 		typedef enum
 		{
 			MAGIC_FIELD_ALL,		埰迋趙摯鳹鐎ぁ窅窇堎
 			MAGIC_FIELD_BATTLE,		橋    摯陑
 			MAGIC_FIELD_MAP,		玊橖悃蚗鏤窀摯陑
 			
 		}MAGIC_FIELDTYPE;
 	    
 	    target毀汔躂禱葡濯勗③  堎凝�袾荂懽鵅嚾扑普紜掉健蔥仃敻鏍蠸鯆噱胱俶瞿�
 	    
 		typedef enum
 		{
 			MAGIC_TARGET_MYSELF,		猷覕摯陑
 			MAGIC_TARGET_OTHER,		眥摯硞  猷覕硈資)
 			MAGIC_TARGET_ALLMYSIDE,		    蠈  
 			MAGIC_TARGET_ALLOTHERSIDE,	Ъ鱗插蠈  
 			MAGIC_TARGET_ALL,		蠈趙
 			MAGIC_TARGET_NONE,		移忒撒  ③  埵笢��  遛盓倜誧摯鄹
 			MAGIC_TARGET_OTHERWITHOUTMYSELF,眥摯硞  猷覕硈竘埵笢)
 			MAGIC_TARGET_WITHOUTMYSELFANDPET, 猷覕敁妐蚗哱雄翻
 			MAGIC_TARGET_WHOLEOTHERSIDE,        摯�蚅枅嚦�  
 		}MAGIC_TARGETTYPE;
 	    
 	    <font size=+1>喫摯倳棬勗100禱鵿埰敁�楦婽玅皮俶繚晉硌窕痸諑耒瞿�
 	    
 	    name毀��絊  ��
 	    comment毀喫摯��絊摯    ��
 	
 	N0    N3 黥蹬摯蚕溘隍↓淏
 	    黥蹬勗埵埱趙笢堎硞摯攷  禱邞嚭埰堎��
 	    N0|kubun|level|charaindex|maxhp|hp|mp|name
 	    
 	    kubun 毀鼠摯  誑堣堎凝  笢凝�菬葽笥�  笢�蛫孩噤舀炱槱�↓袲璃埰�欷聿苺菬僗槿伢繞炕灊���
 	    2雄窀煦敁�搊補縎檗�摯  埱趙笢堎忒摯摯蚕溘隍↓淏  1bit    level 2bit   charaindex埵汔  
 	    誑邞嚭踏躂堎��
 	    charaindex 毀黥蹬摯charaindex
 	    level毀鼠摯硞摯畛穫鳴��
 	    maxhp毀鼠摯硞摯MAXHP
 	    hp毀鼠摯硞摯藯凜摯HP
 	    mp毀鼠摯硞摯縈  
 	    name 毀鼠摯硞摯  鞳��
 	
 	K0  K4 鰫  蚕溘隍↓淏
 	    hp maxhp mp maxmp str tough exp 
 	    level attackpower deffencepower 
 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
 	      鞳 status
 	    K摯  勗0  4禱炩擭媃趙��汔摯鰫  凝禱炩擭埰堎喫敁��
 	    鼠摯べ摯哱↓袲璃勗0誑  倜�桯姨動該檗�  毀  笢敁虭竣痀��
 	    堣堎鳹譴毀兒ぁ堣堎��1煦敁蠈蚕溘隍↓淏��
 	    2雄窀煦敁�搊補縎檗�摯  埱趙笢堎忒摯摯蚕溘隍↓淏  1bit    hp  2bit   maxhp埵汔  
 	    誑邞嚭踏躂堎��
 	    溢惉蕾淏毀 '|' ぁ堣堎�覗�倜  鞳敁猷藽措蠕毀��
 	    <a href="#escaping">操筒艇↓鏤媃倜忒摯禱
 	    倜趙摲ぁ僑埵ど倜忒摯誑玊嚭踏躂堎��
 	    鼠躂�葸噯偽擘做廗普紜悵�
 	    
 	        No.(  蠕)  int  
 	        islive(牁屻)  int  
 	        GRA(  鴢  蠕)  int  
 	        Hp(膰敯  )  int  
 	        MaxHp(    膰敯  )  int  
 	        Mp  int  
 	        MapMp  int  
 	        Exp(歲雁偯)  int  
 	        MaxExp(べ竘ぁ摯歲雁偯)  int  
 	        Level(畛穫鳴)  int  
 	        Attack(  賓  )  int  
 	        Defense(陰    )  int  
 	        Quick(搫傲踏)  int  
 	        Ai(櫆笠梣)  int  
 	        fEarth(貍)  int  
 	        fWater(  )  int  
 	        fFire(庄)  int  
 	        fWid(諿)  int  
 	        Slot(    歕  倳)  int  
 	          鞳  脾舅菴啞溘精  int  
 	          鞳    棬    
 	        蝠↓勒↓妐蚗哱      棬    
 	    
 	      鞳  脾舅菴啞溘精敁毀�摀蟦動該檗�摯  鞳禱  脾媃趙源笢凝汔竣凝摯啞溘精ぁ��
 	    1 煦敁  脾    �搢葽笥�  脾箾菴敁埵堎��
 	    
 	    沓    ( char 摯    鼠摯竘竘怳�� )
 	    
 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|喜庌毞|PC
 
 	W0  W4 妐蚗哱摯  溢↓淏
 	  W0|skillid|field|target|name|comment| x 7
 	  W0    W4 毀鼠躂�葸噯動該檗�勗葡伀媃趙笢堎��
 	  petskillid 毀�楟該檗�摯  摯  蠕�菘et_skillinfo.h勗擭壛踏躂趙笢堎��
 	  field 毀鼠摯  誑汔喫ぁ窅暵ぁ拻堎凝�袾荂懽鵅嚾扑普紜掉健蔥仃敻鏍蠸鯆噱胱俶瞿�
 	  
 		typedef enum
 		{
 			PETSKILL_FIELD_ALL,		埰迋趙摯鳹鐎ぁ窅窇堎
 			PETSKILL_FIELD_BATTLE,		橋    摯陑
 			PETSKILL_FIELD_MAP,		玊橖悃蚗鏤窀摯陑
 
 		}PETSKILL_FIELDTYPE;
 	  
 	  target 毀鼠摯  摯葡濯誑汔竣笢竣忒摯凝�袾荂懽鵅嚾扑棘敯健蔥仃敻鏍蠸鯆噱胱俶瞿�
 	  
 		typedef enum
 		{
 			PETSKILL_TARGET_MYSELF,		猷覕摯陑
 			PETSKILL_TARGET_OTHER,		眥摯硞  猷覕硈資)
 			PETSKILL_TARGET_ALLMYSIDE,	    蠈  
 			PETSKILL_TARGET_ALLOTHERSIDE,	Ъ鱗插蠈  
 			PETSKILL_TARGET_ALL,		蠈趙
 			PETSKILL_TARGET_NONE,		移忒撒  ③  埵笢��  遛盓倜誧摯鄹
 			PETSKILL_TARGET_OTHERWITHOUTMYSELF,眥摯硞  猷覕硈竘埵笢) 
 			PETSKILL_TARGET_WITHOUTMYSELFANDPET, 猷覕敁妐蚗哱雄翻
 		}PETSKILL_TARGETTYPE;
 	  
 	  name 毀  摯  鞳��
 	  comment毀鼠摯  勗葡埰堎    ��
 	  target|name|comment| 毀  摯倳摯覕兒菜ぁ邞�桫噱紙彸瞿�
 	    毀郖伀冽僑竘ぁ��    ぁ  誑  崹趙笢堎  "|"摯陑誑襄��  鳹譴
 	  毀袲溘騷囮璃哱ぁ錁誧趙  崚埰堎痀��
*/
void lssproto_S_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_S_SEND, buffer);
}

/*
   servertoclient D( int category , int dx , int dy , string data );
       (Display)
           勗笰凝  崚埰堎炩  ��
 
       
 	int category
 	笰禱  崚埰堎凝��
 	    
 	        偯        
 	        1  猷覕雄翻勗僄窇倜譫隍↓喇�菤ata毀  棬
 		    勗埵埱倜偯��  
 		    2  猷覕誑竣崹倜譫隍↓喇�菤ata毀  棬  勗
 		      埵埱倜偯  
 		
 	int dx
 	悃騷す籤溘凝�梩偕醓驞歜鯁繡翎�  X�觝�  黍勗毀騷穫璃哱誑  牁
 	    媃倜痾  �詗芛薩昃尬�哱毀喫摯偯凝�欷芵鱦俜拂舌�  禱銓猀媃趙
 	        埰堎��
 	int dy
 	Ъ葡痾  Y
 	string data 
 	  崚埰堎    ��    毀category勗源埱趙鋆竘堎��
 	    喫摯  棬  毀<a href="#escaping">操筒艇↓鏤踏躂埵崹躂��
 	    埵�欷聿苺�
*/
void lssproto_D_send(int fd,int category,int dx,int dy,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, category);
	checksum += util_mkint(buffer, dx);
	checksum += util_mkint(buffer, dy);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_D_SEND, buffer);
}

/*
   servertoclient FS( int flg);
       (FlgSet)PS摯摯伀紮�覗�倜毀痲精騷璃鄹埵汔勗猷覕摯Щ稃敁媃趙邞�桫噱紙彸瞿�
       
         int flg
          0 bit  0: 黥蹬Off                  1: 黥蹬On
                藯凜  窅暵  1 bit  0: 橋      落萃off          1: 橋      落萃On    
              2 bit  0: DUEL off                 1: DUEL On
              3 bit  0:   玊摯鏍籤蚗哱さ↓迶     1: 蚕↓  贖勗媃凝鏍籤蚗哱誑  �屻聿陊飽灃�
              4 bit  0:   肣躲儒OK               1:   肣躲儒藸  
*/
void lssproto_FS_send(int fd,int flg)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FS_SEND, buffer);
}

/*
   servertoclient HL( int flg);
       (HeLp)HL摯摯伀紮�覗�倜毀蚕↓  贖摯黥蹬誑橋  摯堁擔崹さ↓迶禱  脾媃倜鳹譴勗邞�桫噱紙彸瞿�
       
         int flg
          0: 堁擔崹さ↓迶Off                  1: 堁擔崹さ↓迶On
*/
void lssproto_HL_send(int fd,int flg)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_HL_SEND, buffer);
}

/*
   servertoclient PR( int request, int result);
       (PartyRequest)黥蹬菺滑摯伀紮�腍R禱袲溘騷囮璃哱誑邞埱趙笢埵�妡缺硈蟤壅城撻翯�堎鄹誑堣堎��
        蚕↓  贖誑  蹟搣彌  虮誑  崹倜  埵汔摯鄹��
       
       	int request
       	0: 謫暷 1:  暷
         int result
         0: 謄   1: 埬  
*/
void lssproto_PR_send(int fd,int request,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, request);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PR_SEND, buffer);
}

#ifdef _PETS_SELECTCON
void lssproto_PETS_send(int fd,int petarray,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, petarray);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PETST_SEND, buffer);
}
#endif

void lssproto_KS_send(int fd,int petarray,int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, petarray);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_KS_SEND, buffer);
}

void lssproto_SPET_send(int fd, int standbypet, int result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, standbypet);
	checksum += util_mkint(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SPET_SEND, buffer);
}

/*
   servertoclient PS( int result, int havepetindex, int havepetskill, int toindex);
       (PetSkill use result)妐蚗哱摯  禱窅埱倜踕堅袲溘騷囮璃哱摯PS勗葡伀媃趙邞�桫噱紙彸瞿�
       result 雄翻毀袲溘騷囮璃哱摯PS勗葡伀媃趙笢堎�菾覂哄齠擽鵜�凝�欷ず氤饑厊撻聿苺�
       
 	int result
 	踕堅��0: 謄   1:埬  
         int havepetindex
         笰    摯妐蚗哱誑窅暵媃倜凝��
         int havepetskill
         笰    摯  禱窅暵媃倜凝��
 	int toindex
 	 目標  傮禱窅暵媃倜凝�蛦蟤噸棚鬷仇熗摛迖�盓す籤溘摯indexぁ毀埵笢�蛚紜掉�  勗埵埱趙笢堎��
 	
 	  猷覕    = 0
 	  妐蚗哱  = 1   5
 	  黥蹬    = 6   10   S N 摯0  4勗葡伀�蛪葾郱菙壖笱勞�躂趙笢堎  
 	
 	葡濯誑蠈劂�楱蝢桂郋旅欷聿苺楱蝢挺啗踽�毀-1ぁ邞嚭埰堎��
*/
void lssproto_PS_send(int fd,int result,int havepetindex,int havepetskill,int toindex)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, result);
	checksum += util_mkint(buffer, havepetindex);
	checksum += util_mkint(buffer, havepetskill);
	checksum += util_mkint(buffer, toindex);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PS_SEND, buffer);
}

/*
   servertoclient SKUP( int point );
       (SKillUP)
       筒す鳴囮蚗鏤誑ぁ拻堎痀禱�荂懽嚜玉束籥彸瞿詎倞妏棲�ど�桫戰職捌峙黃譎彸瞿�
*/
void lssproto_SKUP_send(int fd,int point)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, point);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SKUP_SEND, buffer);
}

/*
   servertoclient WN( int windowtype, int buttontype, int seqno, int objindex, string data );
       (Window)
       坳贖璃迶坳禱  崚鞠源敁�荂懽鵅齠石芛薩昃尬�哱勗玊蠐埰堎��
       
         int windowtype
         坳贖璃迶坳淏騷鏤�袶�摯源竣埵魠祪摯坳贖璃迶坳禱  崚埰堎凝�蛦蟤憊蓫uttontype 摯蚻陑譴渦鞠ぁ
             坳贖璃迶坳誑鋆擭埰堎��
             �荂懽鵅嚾扑普紜掉�  勗优擭踏躂趙笢堎��
           
 		typedef enum
 		{
 			WINDOW_MESSAGETYPE_MESSAGE,			隍蚗掛↓喇摯陑
 			WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,		隍蚗掛↓喇敁郖菜    
 			WINDOW_MESSAGETYPE_SELECT,			撒  坳贖璃迶坳
 			WINDOW_MESSAGETYPE_PETSELECT,			妐蚗哱撒  坳贖璃迶坳
 			WINDOW_MESSAGETYPE_PARTYSELECT,			黥蹬撒  坳贖璃迶坳
 			WINDOW_MESSAGETYPE_PETANDPARTYSELECT,		妐蚗哱�歆棺瑆�  坳贖璃迶坳
 			WINDOW_MESSAGETYPE_ITEMSHOPMENU,		堁壨摯隍俓堥↓坳騷璃迶坳
 			WINDOW_MWSSAGETYPE_ITEMSHOPYMAIN,		堁壨摯隍騷璃坳騷璃迶坳
 			WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,		  笢噁堇障  囮騷  堜碟摯隍騷璃坳騷璃迶坳
 			WINDOW_MESSAGETYPE_PETSKILLSHOP,		妐蚗哱摯  碟踏庌坳騷璃迶坳
 			WINDOW_MESSAGETYPE_WIDEMESSAGE,			隍蚗掛↓喇摯陑    拻笢    
 			WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,	隍蚗掛↓喇敁郖菜        拻笢    
 
 		}WINDOW_MESSAGETYPE;
 
         int buttontype
         尨淏璃摯魠祪禱炩擭埰堎�袾荂懽鵅嚾扑普紜掉啣簽敻鏍蠸鯆噱胱俶瞿�
             
 		#define		WINDOW_BUTTONTYPE_NONE		(0)
 		#define		WINDOW_BUTTONTYPE_OK		(1 << 0)
 		#define		WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
 		#define		WINDOW_BUTTONTYPE_YES		(1 << 2)
 		#define		WINDOW_BUTTONTYPE_NO		(1 << 3)
 		#define		WINDOW_BUTTONTYPE_PREV		(1 << 4)
 		#define		WINDOW_BUTTONTYPE_NEXT		(1 << 5)
             
             喫躂�梩啞疣闡�渦鞠ぁ邞嚭埰堎��  窇�氿幎ES尨淏璃敁NO尨淏璃誑  媃笢鄹毀
             WINDOW_BUTTONTYPE_YES | WINDOW_BUTTONTYPE_NO   (=12)
             ぁ邞堎��
             
         int seqno
         喫摯坳贖璃迶坳摯  蠕禱崚埰�袾荂懽鵅齠扒�  埰堎��
             袲溘騷囮璃哱毀WN勗趙喫摯坳贖璃迶坳摯堯軘踕堅禱蒍埰摯勗�摀蟦�  蠕禱蝨窇趙蒍紮埰堎��
             喫躂勗源埱趙�荂懽鵅黦棟�摯NPC摯汔摯鳹  ぁ摯坳贖璃迶坳凝禱  茯③  堎源竣勗埰堎��
         int objindex
         喫摯坳贖璃迶坳禱③鞠敁虭埱倜NPC埵汔摯index誑氶  踏躂趙笢堎��
             で筒  堜誑③鞠敁虭埱倜鳹譴毀-1埵汔誑  埱趙笢堎��
             袲溘騷囮璃哱毀�椸�贖璃迶坳      摯WN鏤痲哱昡鳴ぁ喫摯倳偯禱鼠摯竘竘蒍鞠��  笢��
         string data
         隍蚗掛↓喇    禱崚埰��    毀操筒艇↓鏤埰堎��"\n"ぁ鉏憍堎敁搊菜敁笢竣徽  敁埰堎��
             竘倜�椳芛薩昃尬�哱摯坳贖璃迶坳ぁ  崚③  堎莉蜬禱唅窇倜傷毀猷  黍勗搊菜踏躂堎��
             撒  熁摯堣堎坳贖璃迶坳ぁ毀��"\n"ぁ鉏憍�桫憶�  疐摯哱↓袲璃誑隍蚗掛↓喇摯菜倳
             敁埵堇�暔搊撊肩彌鴃麚熉匾�↓袲璃誑  疐摯哱↓袲璃ぁ炩擭踏躂倜蠀倳襄拻��
             鼠摯堣敁摯哱↓袲璃  誑兒僑囀僑摯撒  熁敁埵堎�覗�倜鞳凝�桭舠� 1 凝��
               蠕禱鄎堇騍趙�敯�  媃倜鳹譴摯WNぁ摯蒍紮摯鄹勗蒍埰��
         string data(堁壨暵  
         隍蚗掛↓喇    禱崚埰��    毀操筒艇↓鏤埰堎��"\n"ぁ鉏憍堎敁搊菜敁笢竣徽  敁埰堎��
             竘倜�椳芛薩昃尬�哱摯坳贖璃迶坳ぁ  崚③  堎莉蜬禱唅窇倜傷毀猷  黍勗搊菜踏躂堎��
             溢↓淏  毀囂    勗"  "ぁ鉏憍�桫噱胱倷�埰��
    ��  竣��
  	  笢  堎啞溘精    竣兔※  堎兒    鞳摯溢↓淏窅竣凝啞溘精  窅竣兔※窅渦埵笢※1    
 		壨摯  鞳  隍蚗掛↓喇  壨隍蚗掛↓喇  蠀倳撒  隍蚗掛↓喇  畛穫鳴鵿堇埵笢隍蚗掛↓喇  賃  隍蚗掛↓喇  
 		囮騷  堜誑笢埱毞笢隍蚗掛↓喇  囮騷  堜      窇堎  窇埵笢啞溘精    窇堎※兔  窇埵笢※兒    囮騷  堜畛穫鳴  
 		偯騤    鴢  蠕        囮騷  堜      窇堎  窇埵笢啞溘精  囮騷  堜畛穫鳴  偯騤    鴢  蠕      
       		
       		0|1|�衯承袘嚗�  梲碟|笢�梊�媃崨笢��笰摯源竣煦笢|笢�梊�媃崨笢汔躂勗埰堎|笰蠀  菺煦笢|鼠躂禱薔  埰堎勗毀畛穫鳴誑
       		鵿堇埵笢崹汔笢笢摯凝笢|  騍勗笢笢庌煦衭|堁笢堁笢囮騷  堜誑笢埱毞笢啋崨衭窇凝∪  々↓隍溘璃|
       		兔|兒兔兔|兒典兔兔兔|莉郖  勗  賓\n嗚堅  衁|蜧蜨|兔|其兔|兒典兔兔兕|    兕兔樊蜾\n  搦勗毀僑拻  \n項笢  
 
         ��  堎��
 		  笢  堎啞溘精    竣兔※  堎兒    鞳摯溢↓淏窅竣凝啞溘精  窅竣兔※窅渦埵笢※1    
 		壨摯  鞳  隍蚗掛↓喇  壨隍蚗掛↓喇  堁鉣誑笢埱毞笢勗埵埱з崨竣隍蚗掛↓喇|賃  隍蚗掛↓喇  
 		囮騷  堜      躂堎  躂埵笢啞溘精  偯騤    鴢  蠕        囮騷  堜      躂堎  躂埵笢啞溘精  
 		偯騤    鴢  蠕        
       
       		兒|1|�衯承袘嚗�  沓碟|笢�梊�媃崨笢��汔堎埰堎ㄑ  笢�梊�媃崨笢汔躂禱  埱趙�岉戰瞿萆  騍勗笢笢庌煦衭|
       		鼠躂煦敁��堁鉣誑酘↓泬↓媃з崨竣衭  々↓隍溘璃|兔|兕兔|兒典兔兔兔|
       		莉郖  勗  賓\n嗚堅  衁|蜧蜨|兔|兒兔|兒典兔兔兕|    兕兔樊蜾\n  搦勗毀僑拻  \n項笢  
         �梛岌�  堜  崹堎��
 		啞溘精    堎=兔    堣敁笰蠀  崹�桫戰職�  
 		壨摯  鞳  
 		  崹堎隍蚗掛↓喇  喫躂雄窀  崹�桫撻聿迠肩彌鴃麚�  賃  隍蚗掛↓喇  
 		  鞳    崹�桫戰職扇袚繙�  菴  ˇ兔�楘媯琭儘�    偯騤    鴢  蠕  郖菜騷璃啞б  囮騷  堜    蠕  兒凝��  
 		  鞳    崹�桫戰職扇袚繙�  菴  ˇ兔�楘媯琭儘�    偯騤    鴢  蠕  郖菜騷璃啞б  囮騷  堜    蠕  兒凝��  
         �梛岌�  堜礎拻噁堎��
 		啞溘精  礎拻噁堎=兒    
 		壨摯  鞳  礎拻③埰隍蚗掛↓喇  囮騷  堜誑郖  摯鄹摯隍蚗掛↓喇|賃  隍蚗掛↓喇  
 		  鞳  礎拻③鞠堎凝汔竣凝啞溘精  菴  ˇ兔�楘媯琭儘�    囮騷  堜畛穫鳴  偯騤    鴢  蠕  郖菜騷璃啞б  
 		  鞳  礎拻③鞠堎凝汔竣凝啞溘精  菴  ˇ兔�楘媯琭儘�    囮騷  堜畛穫鳴  偯騤    鴢  蠕  郖菜騷璃啞б  
 
         �桯姨啄除�
 		壨摯  鞳  隍蚗掛↓喇
       
       		�衯承袘嚗�  沓碟|笢�梊�媃崨笢��汔堎埰堎ㄑ
 	
         string data(妐蚗哱摯  碟踏庌暵  
         隍蚗掛↓喇    禱崚埰��    毀操筒艇↓鏤埰堎��"\n"ぁ鉏憍堎敁搊菜敁笢竣徽  敁埰堎��
             竘倜�椳芛薩昃尬�哱摯坳贖璃迶坳ぁ  崚③  堎莉蜬禱唅窇倜傷毀猷  黍勗搊菜踏躂堎��
             溢↓淏  毀囂    勗"  "ぁ鉏憍�桫噱胱倷�埰��
 	   �梛該檗�摯  噁  ��
 		鞳摯攷  窅竣凝汔竣凝ㄑ  窅竣※0��窅渦埵笢※1    壨摯  鞳  隍騷璃隍蚗掛↓喇  
 		      偯騤                偯騤        
 	
 		1  �衯承袘嚗�  梲碟  笢�梊�媃崨笢竘埱鞠�觚岓圮欷ひ�  �尕�凝ぁ埰源  
 		    摯當鼠摯詞  500    賓  兩兔×    ��  遛  兩兔×          
 		貍曶郖棣  1000  犍媃笢  峟誑堣堎凝忒  媏騍    	
*/
void lssproto_WN_send(int fd,int windowtype,int buttontype,int seqno,int objindex,char* data)
{
	char buffer[65500];
	int checksum=0;

#ifdef _NO_WARP
	// shan hanjj add Begin
    CONNECT_set_seqno(fd,seqno);
    CONNECT_set_selectbutton(fd,buttontype);    
	// shan End
#endif

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, windowtype);
	checksum += util_mkint(buffer, buttontype);
	checksum += util_mkint(buffer, seqno);
	checksum += util_mkint(buffer, objindex);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WN_SEND, buffer);
	
}

/*
   servertoclient EF( int effect, int level, string option );
       (EFfect)
       坁盓蓎禱禢�欷ぬ赻誨詗芛薩昃尬�哱勗蠈  黍埵嗚堅  藯鞠源敁邞堎��
       
         int effect
         嗚堅  蠕�蛫孩寰葸壅扁斔の�埰堎敁��    敁忒摯嗚堅誑  �桫懊�埰�菬漹槬彸蟲誽敖褖盂交珆�倜堇敁凝媃竘埰��
           
             1:蓎
             2:坁
             4:欲枃坁    擭  
           
         int level
         嗚堅摯倯踏�菬葽欺梊呁菬�  其毀  藯摯倯踏禱  埰��
         string option
         ぽ  錚耦暵
*/
/* Arminius
  陔崝跤隍鷋赽腔雄賒
  int effect = 10   隍鷋赽
      level dont care
      option = "pet1x|pet1y|dice1|pet2x|pet2y|dice2" (no makeEscapeString)
*/
void lssproto_EF_send(int fd,int effect,int level,char* option)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, effect);
	checksum += util_mkint(buffer, level);
	checksum += util_mkstring(buffer, option);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EF_SEND, buffer);
}

/*
   servertoclient SE( int x, int y, int senumber, int sw );
       (SoundEffect)
 	袲溘騷囮璃哱勗SE禱  �梊妢蔥仃樕勤奲彸瞿�
       
         int x,y
         SE禱  嚭楁摯淢  ��
         int senumber
             摯  蠕
         int sw
             禱  �梊尨涕掩忍職恐�
           
             0:鰼誧堎
             1:  �梊�
*/
void lssproto_SE_send(int fd,int x,int y,int senumber,int sw)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	checksum += util_mkint(buffer, senumber);
	checksum += util_mkint(buffer, sw);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SE_SEND, buffer);
}

/*
   servertoclient ClientLogin(string result);
       ClientLogin摯蒍紮��
       
 	string result
 	"ok" 敁笢竣  棬  �蛦蟦�  棬  毀操筒艇↓鏤踏躂埵笢��
*/
void lssproto_ClientLogin_send(int fd,char* result)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);
}

/*
   servertoclient CreateNewChar(string result,string data);
       CreateNewChar摯蒍紮��
       
 	string result
 	"successful" 凝 "failed" 摯笢囀躂凝�蛦蟦�  棬  毀操筒艇↓
 	    鏤媃埵笢��
 	string data
 	"failed" 摯鄹毀    禱崚埰硞蹬摯峟趙覕堎  棬
 	      ぁ堣堎�袼廷倓�璃哱�荂懽嚝旅梩唯租牊姨曼�竘ぁ堣堎��
 	    雄き摯  棬  

 	    "failed bad parameter"

 	    摯鳹譴毀��す籤溘軘埬摯敁拻勗踐擭摯蚕溘隍↓淏摯  ⑹禱唅窇趙
 	    笢堎敁笢竣敁禱徽  埰堎�蛦蟤噸敢堙灅怕荂懽鵅齠犯�  埰堎隍蚗
 	    掛↓喇ぁ堣堎�蛦蟦�  棬  毀操筒艇↓鏤媃埵笢��
*/
void lssproto_CreateNewChar_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CREATENEWCHAR_SEND, buffer);
}

/*
   servertoclient CharDelete(string result,string data);
       CharDelete 摯蒍紮��
       
 	string result
 	"successful" 凝 "failed" 摯笢囀躂凝�蛨瑑弩央劗鵷ぴ聿苺�
 	string data
 	"failed" 摯鄹毀    禱崚埰硞蹬摯峟趙覕堎  棬
 	      ぁ堣堎�袼廷倓�璃哱�荂懽嚝旅梩唯租牊姨曼�竘ぁ堣堎��
 	    溢惉蕾淏禱謂�弅�埵笢摯ぁ��操筒艇↓鏤媃埵笢��
*/
void lssproto_CharDelete_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARDELETE_SEND, buffer);
}

/*
   servertoclient CharLogin(string result,string data);
       CharaLogin摯蒍紮��
       
 	string result
 	"successful" 凝 "failed" 摯笢囀躂凝�蛨瑑弩央劗鵷ぴ聿苺�
 	string data
 	"failed" 摯鄹毀 鼠摯    摯  棬  �蛨瑑弩央劗鵷ぴ聿苺�
*/
void lssproto_CharLogin_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGIN_SEND, buffer);
}

/*
   servertoclient CharList(string result,string data);
       CharList摯蒍紮��
       
 	string result
 	"successful" 凝 "failed" 摯笢囀躂凝�蛨瑑弩央劗鵷ぴ聿�
 	string data
 	result誑"successful"摯鄹毀��囮庈坳璃哱�荂懽鵅麙槻覣敶鯆噱�
 	    笢堎埰迋趙摯す籤溘摯  鞳��酘鏤で砫璃禱禱筒妐↓筒ぁ鉏憍埱倜
 	    郖蠀摯  棬  �蛦蟦�  棬  禱軘埱趙笢堎摯毀��囮庈坳璃哱�荂懽鵃�
 	    result 誑 "failed" 摯鄹毀    禱崚埰硞蹬摯峟趙覕堎  棬  ぁ
 	    堣堎�觚�  摯鄹摯酘鏤で砫璃摯  �鼒普紜掉匿諏ば酴目夭瞿�
 	    dataplace|faceimage|level|maxhp|atk|def|quick|charm|earth|water|fire|wind|logincount|name|place
 	    
 	      dataplace
 	      掛↓々溢↓淏摯笰    凝�蛦蟤嬴氾褐珝蹦邯�↓惉筒哱摯笰    凝禱鋆擭埰堎��
 	      faceimage
 	      蚇摯  鴢  蠕
 	      level
 	      す籤溘摯畛穫鳴
 	      maxhp,atk,def,quick,charm
 	      掘蚕溘隍↓淏��
 	      earth.water,fire,wind
 	      掘鶂鍛偯
 	      logincount
 	      痲精騷璃庈坳璃哱
 	      name
 	      す籤溘摯  鞳
 	      place
 	      す籤溘摯笢堎鳹鐎
 	    
 	    "|" ぁ鉏憍�桫噱胱俶瞿� 鼠躂�葸噯凰�  毀��<a
 	    href="#escaping">操筒艇↓鏤踏躂趙笢堎�蛫姨啡及覣袘脊�
 	    ぁ僑埵ど堎��
*/
void lssproto_CharList_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLIST_SEND, buffer);
}

/*
   servertoclient CharLogout(string result , string data);
       Logout勗葡埰堎蒍紮��
       
 	string result
 	"successful" 凝 "failed" 摯笢囀躂凝�蛨瑑弩央劗鵷ぴ聿苺�
 	string data
 	"failed" 摯鄹勗摯陑徽  誑堣堇��謄  摯    (Щ稃)禱崚埰硞蹬
 	    摯峟趙覕堎  棬  ぁ堣堎�蛨瑑弩央劗鵷ぴ聿苺�
*/
void lssproto_CharLogout_send(int fd,char* result,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, result);
	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGOUT_SEND, buffer);
}

/*
   servertoclient ProcGet( string data);
 	ProcGet摯蒍紮��
       
 	string data
 	操筒艇↓鏤埰堎�蛘堙灅怕荂懽鵅齞�    攷  禱邞嚭埰堎��    毀log/proc啞栝騷鳴勗怳凝躂堎    敁  啋��
*/
void lssproto_ProcGet_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");
	
	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PROCGET_SEND, buffer);
}

/*
   servertoclient PlayerNumGet( int logincount, int player);
 	PlayerNumGet摯蒍紮��
       
 	int logincount,player
*/
void lssproto_PlayerNumGet_send(int fd,int logincount,int player)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, logincount);
	checksum += util_mkint(buffer, player);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PLAYERNUMGET_SEND, buffer);
}

/*
   servertoclient Echo( string test );
       Echo勗葡埰堎蒍紮��
       
 	string test
 	踏拻酵汔    踏躂倜  棬  �蛨瑑弩央劗鵷ぴ聿苺�
*/
void lssproto_Echo_send(int fd,char* test)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, test);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ECHO_SEND, buffer);
}

// CoolFish: Trade 2001/4/18

void lssproto_TD_send(int fd, int index, char* message)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TD_SEND, buffer);
}
#ifdef _CHATROOMPROTOCOL			// (祥褫羲) Syu ADD 謐毞弅け耋
void lssproto_CHATROOM_send ( int fd , char* message )
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHATROOM_SEND, buffer);
}
#endif
#ifdef _NEWREQUESTPROTOCOL			// (祥褫羲) Syu ADD 陔崝Protocol猁⑴牉砐
void lssproto_RESIST_send ( int fd , char* message )
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RESIST_SEND, buffer);
}
#endif
#ifdef _OUTOFBATTLESKILL			// (祥褫羲) Syu ADD 準桵須奀撮夔Protocol
void lssproto_BATTLESKILL_send ( int fd , char* message )
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_BATTLESKILL_SEND, buffer);
}
#endif
void lssproto_NU_send(int fd, int nu)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, nu);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NU_SEND, buffer);
}


void lssproto_FM_send(int fd, char* message)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, message);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FM_SEND, buffer);
}


void lssproto_WO_send(int fd,int effect)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, effect);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WO_SEND, buffer);
}
#ifdef _ITEM_CRACKER
void lssproto_IC_send(int fd, int x, int y)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, x);
	checksum += util_mkint(buffer, y);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_IC_SEND, buffer);
}
#endif
#ifdef _MAGIC_NOCAST             // 儕鍾:麥蘇
void lssproto_NC_send(int fd,int flg)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, flg);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NC_SEND, buffer);
}
#endif

#ifdef _CHECK_GAMESPEED
void lssproto_CS_send( int fd, int deltimes)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint( buffer, deltimes);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CS_SEND, buffer);
}
#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_send(int fd,char *message)
{
	char buffer[65500];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,_RUNNING_KEY);

	checksum += util_mkstring(buffer,message);
	util_mkint(buffer,checksum);
	util_SendMesg(fd,LSSPROTO_STREET_VENDOR_SEND,buffer);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_send(int fd, int type, char* data)
{
	char buffer[65500];
	int checksum=0;

	print("\n RCLICK_send( type=%d data=%s) ", type, data );

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkint(buffer, type);
	checksum += util_mkint(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RCLICK_SEND, buffer);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_send(int fd,char *data)
{
	char buffer[65500];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,_RUNNING_KEY);

	//print("tocli_data:%s ",data);
	checksum += util_mkstring(buffer,data);
	util_mkint(buffer,checksum);
	//print("tocli_buffer:%s ",buffer);
	util_SendMesg(fd,LSSPROTO_JOBDAILY_SEND,buffer);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_send(int fd,char *data)
{
	char buffer[65500];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,_RUNNING_KEY);

	checksum += util_mkstring(buffer,data);
	util_mkint(buffer,checksum);
	util_SendMesg(fd,LSSPROTO_TEACHER_SYSTEM_SEND,buffer);
}
#endif

#ifdef _ADD_STATUS_2
void lssproto_S2_send(int fd,char* data)
{
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, _RUNNING_KEY);

	checksum += util_mkstring(buffer, data);
	util_mkint(buffer, checksum);
	util_SendMesg(fd, LSSPROTO_S2_SEND, buffer);
}
#endif
