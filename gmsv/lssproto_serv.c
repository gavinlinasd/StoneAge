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

#ifdef _FIX_DEL_MAP           // WON ADD 玩家抽地图送监狱
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

#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
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

#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
	if ( func==LSSPROTO_RESIST_RECV ) {
	    char buffer[2];
	    buffer[0] = '\0';
		lssproto_RESIST_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
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
 	潜卞爵  蔽    卞皿伊奶乩□及匏  毛  譬帮允月凶户卞银丹［
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
       弁仿奶失件玄反 CA 互  化｝公及平乓仿卞勾中化譬卅井匀凶凛反｝ C 
       匹仇及楮醒毛邰菲匹五月［扔□田反民尼永弁仄卅中［
       
 	string data
 		  白巧□穴永玄反 动票及3潘  及嫩  毛戊件穴匹卅日屯凶
 	    手及匹丐月.公木冗木及嫩  及  褥反今日卞凶化徇'|'匹仁亢日木
 	    化中月［
 	    
 	      凶化徇匹仁亢日木凶玄□弁件互12蜊及桦宁
 		  玄□弁件及    反
 
 		  WHICHTYPE|CHARINDEX|X|Y|DIR|BASEIMG|LEVEL|NAMECOLOR|NAME|SELFTITLE|WALKABLE|HEIGHT|POPUPNAMECOLOR
 		  仇及左皮斥尼弁玄反平乓仿弁正□匹丐月午中丹仇午匹丐月［
 		  
 		  WHICHTYPE 反］仇及平乓仿弁正□互升丹中匀凶潘  及手及
 		  匹丐月井［仇木反扔□田□匹反动票及方丹卞烂聒今木化中月［
 		  
 		typedef enum
 		{
 		    CHAR_TYPENONE,          窒匹手卅中
 		    CHAR_TYPEPLAYER,        皿伊奶乩□
 		    CHAR_TYPEENEMY,         衬
 		    CHAR_TYPEPET,           矢永玄
 		    CHAR_TYPEDOOR,          玉失
 		    CHAR_TYPEBOX ,              
 		    CHAR_TYPEMSG ,          枣  
 		    CHAR_TYPEWARP ,         伐□皿札□件
 		    CHAR_TYPESHOP ,         饕
 		    CHAR_TYPEHEALER ,       甲□仿□
 		    CHAR_TYPEOLDMAN ,       赢  
 		    CHAR_TYPEROOMADMIN,     尕  遣盒
 		    CHAR_TYPETOWNPEOPLE,    引切及夫午
 		    CHAR_TYPEDENGON,        鳗蜕  
 		    CHAR_TYPEADM,           鳗蜕  
 		    CHAR_TYPETEMPLE,        Temple master
 		    CHAR_TYPESTORYTELLER,   贿曰  
 		    CHAR_TYPERANKING,       尕  遣仿件平件弘  憎      
 		    CHAR_TYPEOTHERNPC,      公及职及腹绸覆擂卞卅日卅中NPC
 		    CHAR_TYPEPRINTPASSMAN,  玉失及由旦伐□玉  憎允月NPC
 		    CHAR_TYPENPCENEMY,      裔烂衬
 		    CHAR_TYPEACTION,        失弁扑亦件卞  杀允月NPC
 		    CHAR_TYPEWINDOWMAN,     它奴件玉它  憎允月NPC    旦玄井手)
 		    CHAR_TYPESAVEPOINT,     本□皮禾奶件玄
 		    CHAR_TYPEWINDOWHEALER,  它奶件玉它正奶皿及甲□仿□
 		    CHAR_TYPEITEMSHOP,	    云饕
 		    CHAR_TYPESTONESHOP,	    檗  盒  矢永玄及  盒  
 		    CHAR_TYPEDUELRANKING,   DUEL仿件平件弘NPC
 		    CHAR_TYPEWARPMAN,	    伐□皿穴件NPC
 		    CHAR_TYPEEVENT,	    奶矛件玄NPC
 		    CHAR_TYPEMIC,	    奶矛件玄NPC
 		    CHAR_TYPELUCKYMAN,	    奶矛件玄NPC
 		    CHAR_TYPEBUS,	    穴件乒旦田旦
 		    CHAR_TYPECHARM,	    奶矛件玄NPC
 		    CHAR_TYPENUM,
 		}CHAR_TYPE;
 		  
 		  公木反职及皿伊奶乩□支NPC支衬匹丐月［扔□田□互霜耨
 		  允月午五反苇尹月  区卞勾中化蝈  霜耨允月［勾引
 		  曰弁仿奶失件玄反｝仇及由弗永玄毛熬仃午匀凶午五卞仇及
 		  由弗永玄卞踏井木化中卅中平乓仿毛  匀化中凶日壅仄化仄
 		  引匀化方中午中丹仇午匹丐月［引凶｝穴它旦市□末伙毛丐
 		  歹六凶午五卞  憎允月树  反仇及树  及心卞湘中化中月［
 		  分井日｝穴它旦市□末伙毛丐歹六凶午五卞  憎允月仇午互
 		    凳今木凶桦宁反｝扔□田□反｝仇及楮醒毛    读卞裟太
 		  分今卅仃木壬卅日卅中［ SELFTITLE卞勾中化反｝犯伉立正
 		  匹丐月凶化徇毛孔仁引卅中方丹卞巨旦弗□皿今木化中月［
 		  弁仿奶失件玄反失弁扑亦件及戊穴件玉(CA)互仁月引匹反  
 		  切匹  憎允月［CHARINDEX扔□田□  及域啦卞平乓仿毛  
 		  烂匹五月  寞｝BASEIMG反  憎及凶户及  寞｝LEVEL反平乓
 		  仿及伊矛伙(0卅日  憎仄卅中［仇及袄反NPC卅升卞银丹［)
 		  WALKABLE反1及午五公及晓毛骚聊允月仇午互匹五｝0卅日骚
 		  聊允月仇午互匹五卅中［HEIGHT反嫖今毛手勾手及井公丹匹
 		  卅中及井及隙烂［
 		  
 		  平乓仿弁正□及  蟆午愤  惫寞反｝<a href="#escaping">
 		  巨旦弗□皿今木卅仃木壬卅日卅中［'|'匹玄□弁件毛
 		  潸曰分仄化井日巨旦弗□皿毛荸轮允月［巨旦弗□皿允月午'
 		  |'互幻井及  侬卞云五井歹月及匹｝  赓反签账卞'|'毛犯
 		  伉立正午仄化方中［霜耨允月  手｝  蟆午愤  惫寞毛巨旦
                   弗□皿仄化井日凶化徇匹勾卅中匹井日霜耨允月［
                   引凶］矢永玄及桦宁反愤  惫寞及医歹曰卞交□扒□互涩烂
                   仄凶矢永玄及  蟆互霜耨今木化仁月［
 		  
 	      凶化徇匹仁亢日木凶玄□弁件互6蜊及桦宁
 		  玄□弁件及    反
 		  INDEX|X|Y|BASEIMG|LEVEL|ITEM1LINEINFO
 		  匹哗  卞  切化中月失奶  丞卞勾中化及树  匹丐月［
 		  INDEX反平乓仿及奶件犯弁永旦午井少日卅中INDEX匹丐月［
 		  公及失奶  丞毛壅允凛卞银迕允月［X,Y反失奶  丞及弘夫□
 		  田伙匏  ［BASEIMG反  飓及  寞［ITEM1LINEINFO反1垫
 		  info卞  憎允月凶户及树  匹丐月［失奶  丞它奶件玉它  
 		  及  憎迕及树  反  及  芊匹迕啦允月［失奶  丞卞楮仄化
 		  反CA反  卅中［ITEM1LINEINFO反<a href="#escape">巨旦弗□
 		  皿今木月［仇及巨旦弗□皿及  芊反晓及嫩  毛辅寰［
 
 	      凶化徇匹仁亢日木凶玄□弁件互4蜊及桦宁
 		  玄□弁件及    反
 		  INDEX|X|Y|VALUE
 		  匹哗  卞  切化中月云嗯卞勾中化及树  匹丐月［    反允
 		  屯化醒侬［INDEX,X,Y 反失奶  丞午  元［VALUE 反升木分
 		  仃及汹井午中丹仪匹丐月［失奶  丞卞勾中化及树  匹丐月［
 	      凶化徇匹仁亢日木凶玄□弁件互1蜊及桦宁
 		  INDEX
 		  仇及平乓仿及C反项尹日木卅中［
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
       苇尹月  区卞中月平乓仿及失弁扑亦件橇谪毛凳蕙允月［
       扔□田□井日弁仿奶失件玄卞域  读卞霜耨允月［
       备平乓仿及1失弁扑亦件仍午卞霜耨［扔□田□反失弁扑亦件毛匹五月分
       仃做谅允月仇午［
       
 	string data
 	CHARINDEX|X|Y|ACTION|PARAM1|PARAM2|PARAM3|PARAM4|....毛戊
 	    件穴匹仁亢匀凶手及卞允月［PARAM反失弁扑亦件仍午卞蜊醒手
 	    银中井凶手仇午卅月［动票反失弁扑亦件域  ［X,Y反匏  匹升
 	    及失弁扑亦件匹手｝丐凶日仄中匏  毛隙烂允月［
 	    仇及  侬  反巨旦弗□皿今木卅中［
 	  
 	        ACTION  PARAM1  PARAM2 
 	       PARAM3  PARAM4  
 	        Stand:0    轾0~7        
 	        Walk:1    轾0~7        
 	        Attack:2    轾0~7        
 	        Throw:3    轾0~7        
 	        Damage:4    轾0~7      
 	        Dead:5    轾0~7        
 	        UseMagic:6    轾0~7  
 	        UseItem:7    轾0~7  
 	        Effect:8    轾0~7  巨白尼弁玄  寞  
 	        Down:10 (逦木月)    轾0~7  
 	        Sit:11 (甄月)    轾0~7  
 	        Hand:12 (澎毛蕊月)    轾0~7  
 	        Pleasure:13 (减少)    轾0~7  
 	        Angry:14 (变月)    轾0~7  
 	        Sad:15 (  仄戈)    轾0~7  
 	        Guard:16 (布□玉)    轾0~7  
 	        actionwalk:17 (失弁扑亦件迕汹五)    轾0~7  
 	        nod:18 (丹卅内仁)    轾0~7  
 	        actionstand:19 (失弁扑亦件迕  切禾□术)    轾0~7  
 	        Battle:20 (爵  树  )    轾0~7  BattleNo(-1 卅日  憎壅允    SideNo  HelpNo  ㄠ卅日踞仃毛裟少CA  憎］ㄟ卅日壅允］引凶反  仄    
 	        Leader:21 (伉□母□树  )    轾0~7  0:  憎壅允 1:  憎  
 	        Watch:22 (爵  棋爵)    轾0~7  0:  憎壅允 1:  憎  
 	        namecolor:23(  蟆及缙树  )    轾0~7    蟆及缙  寞  
 	        Turn:30(  轾  晶)    轾0~7    
 	        Warp:31(伐□皿)    轾0~7    
 	      
 	ACTION及袄反帮醒匹｝    反晓及  及尔羹及嫩  匹丐月［
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
 	data 反犯伉立正  ,  匹嗉濠日木凶奶件犯永弁旦［
 	  醒瘀霜月仪互请  月［
       仇及ID毛  匀化中月平乓仿弁正互壅尹凶凛卞扔□田井日弁仿奶失件玄尺
       午鳗歹月［
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
       伊□母□及    毛弁仿奶失件玄卞霜耨允月［弁仿奶失件玄反仇及霜耨毛
       邰菲允月仇午反卅中［扔□田□互赝癫卅正奶立件弘匹霜耨允月［
       凶午尹壬10汹丐月仁仍午午井｝1坌仍午午井［
 
 	string data
 	x ,y, kind ,及赐卞袄毛'|'匹仁亢匀化卅日屯凶手及毛
 	    今日卞'|'匹卅日屯凶手及［
 	    x,y反平乓仿井日及濮覆匏  ［kind反动票卞仄户允醒袄匹反卅中
 	    筏寞［伊□母□卞丹勾月  及潘  毛隙烂允月［霜日木化仁月手及
 	    及潘  反｝公木引匹卞儒尹凶  芊午井｝旦平伙匹  祭仄｝公木反
 	    扔□田□互哔尹月［
 	 
 	        kind及袄        
 	        E  衬  
 	        P  皿伊奶乩□  
 	        S  熔  
 	        G  云嗯  
 	        I  失奶  丞(熔动陆)  
   
   data及  “"12|22|E|13|24|P|14|28|P"
   仇及  侬  反巨旦弗□皿今木卅中［
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
       平乓仿及旦  □正旦毛霜耨允月［
       犯□正反 市  打伉筏寞  侬(域  侬)     午卅匀化中月［勾引曰  赓及
       1  侬毛苇木壬窒及旦  □正旦井坌月［    反2  侬  井日匹丐月［
           反动票及白巧□穴永玄卞仄凶互丹［凶化徇筏寞'|'互犯伉立正匹丐月［
       2蜊  动嫦及玄□弁件互    匹丐月［
       
 	P 蝈由仿丢□正
 	    kubun hp maxhp mp maxmp str tough exp maxexp exp 
 	    level attackpower deffencepower 
 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
 	    gold 尥仃化中月
 	    惫寞及index   蟆 愤裘惫寞 
 
 	    犯伉立正反 '|' 匹丐月［引凶  蟆午愤裘惫寞反｝
 	    <a href="#escaping">巨旦弗□皿仄凶手及毛
 	    凶化徇匹勾卅仆凶手及互骚耨今木月［
 	    公木冗木及袄及滇反动票［
 	    kubun 卞ㄠ互  匀化中月午蟆由仿丢□正毛霜耨］
 	    ㄡ动晓分午］备申永玄及  匀化中月手及及由仿丢□正互赐  卞  匀化霜耨今木化中月［
 	     尹壬］ㄢ互  凶日hp午Maxhp互kubun 动嫦及玄□弁件卞  户日木化中月［
 	    
 	        kunun  int  
 	        Hp(    )hp  int  
 	        MaxHp  int  
 	        Mp(热诸由伐□)  int  
 	        MaxMp  int  
 	        Vital(    )  int  
 	        Str(    )  int  
 	        Tough(暹屺今)  int  
 	        Dex(豳镀今)  int  
 	        Exp(烦董袄)exp  int  
 	        MaxExp(戚引匹及烦董袄)  int  
 	        Level(伊矛伙)  int  
 	        Attack(  猾  )  int  
      	        Defense(潮    )  int  
 	        fQuick(  蔽豳镀今)  int  
 	        fCharm(    )  int  
 	        fLuck(扪)  int  
 	        fEarth(哗)fixearthat  int  
 	        fWater(  )  int  
 	        fFire(绍)  int  
 	        fWid(氘)fixwindat  int  
 	        Gold(云嗯)  int  
 	        惫寞及index  int  
 	        犯亘巨伙禾奶件玄  int  
 	        鳖戏荚醒  int  
 	          蟆    侬    
 	        愤裘惫寞    侬    
 	    
 	    填    ( char 及    公及引引踏仁 )
 	    
 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|尺氏天|abc|def
 	
 	C 甄  
 	    floor maxx maxy x y
 	    毛  | 匹嗉濠匀化霜月［
 	    填    ( char 及    公及引引踏仁 )
 	    
 	    C1024|100|100|10|20
 	    
 	    晓筏及  匹反｝白夫失ID1024, 扔奶术100x100 X 10,Y 20及午仇
 	    欠卞平乓仿弁正□互中月午中丹    卞卅月［仇及穴奶平乓仿及白
 	    夫失手孔仁户凶甄  毛弁仿奶失件玄互襞月仇午互匹五月及反仇及
 	    戊穴件玉及心匹丐月［汹五及瑛绊反白夫失  及匏  仄井歹井日卅
 	    中［方匀化｝必□丞钒铵凛｝闭蟾银迕及午五｝伐□皿及午五卅升
 	    反｝仇及戊穴件玉毛勾井匀化恳仄中袄毛霜耨仄卅仁化反卅日卅中［
 	    
 	I 蝈失奶  丞
 	    (失奶  丞1)|(失奶  丞2)...(失奶  丞n)
 	    失奶  丞及    反动票及  芊匹由永弁允月［
 	    夫午勾及失奶  丞反｝井卅日内动票及6勾及玄□弁件及本永玄匹
 	    霜耨今木月［夫午勾夫午勾及    反｝
 	    
 	      蟆|  蟆2|旦  □斥|失奶  丞及    |失瓦丢  寞|银迕第  桦赭|银迕覆擂|失奶  丞伊矛伙|白仿弘  
 	    
 	      失奶  丞  反摹  伊矛伙匹愤  读卞  凳今木月［
 	        蟆2反摹  伊矛伙卞方匀化｝失奶  丞及    互  月  烂［
 		  弁仿奶失件玄及失奶  丞  及2垫  卞  木月仪［
 	      旦  □斥反  蟆及缙毛赘月及卞银丹［
 	      失奶  丞及    ［
 	      失瓦丢  寞反｝  飓  寞［
 	      银迕第  桦赭午反］仇及失奶  丞互升仇匹银迕允月仪互第  井互  匀化中月［
 	          仇木反］扔□田□匹反动票及方丹卞烂聒今木化中月［
 	          
 			typedef enum
 			{
 				ITEM_FIELD_ALL,			允屯化及桦赭匹银尹月
 				ITEM_FIELD_BATTLE,		爵    及心
 				ITEM_FIELD_MAP,			骚橘穴永皿晓及心
 
 			}ITEM_FIELDTYPE;
 	          
 	      银迕覆擂午反］仇及失奶  丞毛银迕允月仇午及请  月覆擂互  匀化中月［
 	          扔□田□匹反动票及方丹卞烂聒今木化中月［
 	          
 			typedef enum
 			{
 				ITEM_TARGET_MYSELF,		愤坌及心
 				ITEM_TARGET_OTHER,		职及谛  愤坌殖戈)
 				ITEM_TARGET_ALLMYSIDE,		    蝈  
 				ITEM_TARGET_ALLOTHERSIDE,	锹澎础蝈  
 				ITEM_TARGET_ALL,		蝈化
 			}ITEM_TARGETTYPE;
 		
   	        <font size=+1>仇及醒侬卞100毛箫允午］韶氏匹中月樊手覆擂午卅月［
 	      失奶  丞伊矛伙［仇及伊矛伙动晓及樊匹卅中午隶  请  卅中［
 	      白仿弘  ［缙氏卅白仿弘毛霜耨允月［备申永玄及覆杀反动票及骚曰［

 		      0bit    矢永玄丢□伙匹霜耨第  卅失奶  丞井升丹井［霜耨第  分午ㄠ［ㄟ分午霜耨匹五卅中［  
 		      1Bit    仇及失奶  丞互宁岳请  月井升丹井［  
 		      2Bit    仇及失奶  丞互    井升丹井［1分午    匹丐月［  
 	    
 	    仇仇及  匹反<a href="#escaping">巨旦弗□皿允月［
 	    填    ( char 及    公及引引踏仁 )
 	    
 	    I丐幻 卅 阂|str+2|1|丐幻\|中元|10|2|0
 	    
 	    旦  □斥及袄及啦  反｝帮醒袄匹
 	    
 	        1  旦  □斥A  
 	        2  旦  □斥B  	      
 	        3  旦  □斥C  
 	        4  旦  □斥D  
 	    
 	    午允月［引凶｝失奶  丞  及坞及  坌卞勾中化反｝5蜊及玄□弁件互
 	    坞匹｝勾引曰凶化徇互卅日氏分橇谪匹霜耨今木化仁月［
 	
 	S 蝈旦平伙
 	    (旦平伙0)|(旦平伙1)|(旦平伙2) ... (旦平伙n)
 	    惫寞午  元匹｝平乓仿互手化月    旦平伙醒坌毛霜耨允月［勾引
 	    曰卅中手及反 ||| 午卅月及匹丐月［
 	    备巨件玄伉及    反｝
 	    旦平伙及潘  戊□玉|伊矛伙
 	    匹丐月［
 	    填    ( char 及    公及引引踏仁 )
 	    
 	    S10|2|20|2|||
 	
 	T 惫寞
 	    (惫寞0)|(惫寞1)|(惫寞2) ... (惫寞n)
 	    惫寞及巨件玄伉互坞及桦宁反 '|' 互  粮允月及匹｝  粮仄凶手
 	    及毛    仄化旦平乓件仄化反卅日卅中［井卅日内    蜊醒坌霜耨
 	    允月［
 	    夫午勾｝夫午勾及    反｝
 	      蟆
 	    及心匹丐月［
 	    填    ( 霜日木化仁月 char 及    公及引引匹丐月 )
 	    
 	    T丐幻|方匀天日中
 	    
 	    仇木手<a href="#escaping">巨旦弗□皿允月［
 
 	M   坌由仿丢□正
 	    平乓仿弁正□及旦  □正旦及丹切｝    卞  凳今木月手及(蜇箕
 	    袄)午公丹匹卅中手及(    袄卅升)互丐月［升木井及旦  □正旦
 	    互  凳今木月凶太卞  荚蝈  及由仿丢□正毛霜耨允月及反生永玄
 	    伐□弁玄仿白奴永弁毛  蛲卞银丹仇午卞卅月及匹｝  卞HP ,
 	    MP,EXP及袄分仃卞仄廿匀凶由仿丢□正霜耨戊穴件玉毛迕啦允月［
 	    公木互仇及M戊穴件玉匹丐月［2  侬  动嫦及  域玄□弁件反HP及
 	    蜇箕袄 ,     玄□弁件反MP｝  赶玄□弁件反EXP匹丐月［
 	    动票卞填    毛仄户允［
 	    
 	    M54|210|8944909
 	    
 	    仇及  匹反HP及蜇箕袄互54｝MP及蜇箕袄互210｝烦董袄互8944909
 	    卞卅匀化中月及分［仇及3潘  反  卞  凳  蘸互嫖中午  歹木月
 	    凶户    仄凶戊穴件玉祭仄凶互｝手切欠氏仇及M戊穴件玉及井歹
 	    曰卞P戊穴件玉匹蝈由仿丢□正毛霜耨仄化手方中［凶分仄｝  赝
 	    卞生永玄伐□弁毛银丹凶户卞反｝仇及戊穴件玉毛银丹仇午互蹂俦
 	    今木月［
 	    
         D 平乓仿及ID
 	    夫弘奶件凛卞分仃霜月平乓仿及index］公木午凛棉毛霜月［
 	    D1000|912766409午井匹霜日木月［
 	
 	E 巨件市它件玄割    票蜃)|(晓蜃)
 	    巨件市它件玄  毛霜耨允月［割  反n/100
 	    弁仿奶失件玄反］票蜃及袄井日旦正□玄仄化］ㄠ汹汹仁  卞+1仄凶割  匹巨件市它件玄允月井毛  蝇允月［
 	    凶分仄］晓蜃及袄毛旋尹卅中方丹卞允月［
 	    戚荚卞仇木互霜日木化仁月引匹仇及割  匹巨件市它件玄毛煌遥允月仪［
 	
 	J0   J6 银尹月热诸及    
 	    银尹月热诸及    毛霜耨允月［
 	    0   6 反公木冗木失奶  丞及隶  组赭卞覆杀仄化中月［域  及隶  组赭    ］  ］澎］隶澡    
 	    仄井隶  请  卅中  烂分互］漆  傀舰及啃卞蝈化及隶  组赭及树  毛霜月
 	    J0|kubun|mp|field|target|name|comment
 	    午中丹白巧□穴永玄卞卅匀化中月［
 	    kubun 反公及  互丐月井  中井［ㄟ分午  中［公木动嫦卞玄□弁件允日卅中［ㄠ分午丐月［
 	    mp反壅  竣  毛  允［
 	    field反升及桦赭匹银尹月井［扔□田□匹反动票及  卞烂聒今木化中月［
 	    
 		typedef enum
 		{
 			MAGIC_FIELD_ALL,		允屯化及桦赭匹银尹月
 			MAGIC_FIELD_BATTLE,		爵    及心
 			MAGIC_FIELD_MAP,		骚橘穴永皿晓及心
 			
 		}MAGIC_FIELDTYPE;
 	    
 	    target反升木毛覆擂卞请  月井［扔□田□匹反动票及方丹卞烂聒今木化中月［
 	    
 		typedef enum
 		{
 			MAGIC_TARGET_MYSELF,		愤坌及心
 			MAGIC_TARGET_OTHER,		职及谛  愤坌殖戈)
 			MAGIC_TARGET_ALLMYSIDE,		    蝈  
 			MAGIC_TARGET_ALLOTHERSIDE,	锹澎础蝈  
 			MAGIC_TARGET_ALL,		蝈化
 			MAGIC_TARGET_NONE,		簿手蓟  请  卅中［  豢支凶户及凛
 			MAGIC_TARGET_OTHERWITHOUTMYSELF,职及谛  愤坌殖引卅中)
 			MAGIC_TARGET_WITHOUTMYSELFANDPET, 愤坌午矢永玄动陆
 			MAGIC_TARGET_WHOLEOTHERSIDE,        及扔奶玉蝈  
 		}MAGIC_TARGETTYPE;
 	    
 	    <font size=+1>仇及醒侬卞100毛箫允午］韶氏匹中月樊手覆擂午卅月［
 	    
 	    name反热诸  ［
 	    comment反仇及热诸及    ［
 	
 	N0    N3 醮棉及由仿丢□正
 	    醮棉卞卅匀化中月谛及树  毛霜耨允月［
 	    N0|kubun|level|charaindex|maxhp|hp|mp|name
 	    
 	    kubun 反公及  互丐月井  中井［ㄟ分午  中［公木动嫦卞玄□弁件允日卅中［ㄠ蝈由仿丢□正［
 	    2动晓分午］备申永玄及  匀化中月手及及由仿丢□正  1bit    level 2bit   charaindex卅升  
 	    互霜耨今木月［
 	    charaindex 反醮棉及charaindex
 	    level反公及谛及伊矛伙［
 	    maxhp反公及谛及MAXHP
 	    hp反公及谛及蜇箕及HP
 	    mp反公及谛及竣  
 	    name 反公及谛及  蟆［
 	
 	K0  K4 雯  由仿丢□正
 	    hp maxhp mp maxmp str tough exp 
 	    level attackpower deffencepower 
 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
 	      蟆 status
 	    K及  卞0  4毛隙烂仄化｝升及雯  井毛隙烂允月仇午［
 	    公及戚及玄□弁件卞0互  凶日公及矢永玄  反  中午蜕丹仪［
 	    丐月桦宁反ㄠ匹丐月［1分午蝈由仿丢□正［
 	    2动晓分午］备申永玄及  匀化中月手及及由仿丢□正  1bit    hp  2bit   maxhp卅升  
 	    互霜耨今木月［
 	    犯伉立正反 '|' 匹丐月［引凶  蟆午愤裘惫寞反｝
 	    <a href="#escaping">巨旦弗□皿仄凶手及毛
 	    凶化徇匹勾卅仆凶手及互骚耨今木月［
 	    公木冗木及袄及滇反动票［
 	    
 	        No.(  寞)  int  
 	        islive(戏韶)  int  
 	        GRA(  飓  寞)  int  
 	        Hp(觐菁  )  int  
 	        MaxHp(    觐菁  )  int  
 	        Mp  int  
 	        MapMp  int  
 	        Exp(烦董袄)  int  
 	        MaxExp(戚引匹及烦董袄)  int  
 	        Level(伊矛伙)  int  
 	        Attack(  猾  )  int  
 	        Defense(潮    )  int  
 	        Quick(豳镀今)  int  
 	        Ai(镝擦蘸)  int  
 	        fEarth(哗)  int  
 	        fWater(  )  int  
 	        fFire(绍)  int  
 	        fWid(氘)  int  
 	        Slot(    檗  醒)  int  
 	          蟆  凳袱第白仿弘  int  
 	          蟆    侬    
 	        交□扒□矢永玄      侬    
 	    
 	      蟆  凳袱第白仿弘午反］仇及矢永玄及  蟆毛  凳仄化方中井升丹井及白仿弘匹］
 	    1 分午  凳    ］ㄟ分午  凳尕第午卅月［
 	    
 	    填    ( char 及    公及引引踏仁 )
 	    
 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|尺氏天|PC
 
 	W0  W4 矢永玄及  犯□正
 	  W0|skillid|field|target|name|comment| x 7
 	  W0    W4 反公木冗木及矢永玄卞覆杀仄化中月［
 	  petskillid 反］矢永玄及  及  寞［pet_skillinfo.h卞烂聒今木化中月［
 	  field 反公及  互升仇匹银迕匹五月井［扔□田□匹反动票及方丹卞烂聒今木化中月［
 	  
 		typedef enum
 		{
 			PETSKILL_FIELD_ALL,		允屯化及桦赭匹银尹月
 			PETSKILL_FIELD_BATTLE,		爵    及心
 			PETSKILL_FIELD_MAP,		骚橘穴永皿晓及心
 
 		}PETSKILL_FIELDTYPE;
 	  
 	  target 反公及  及覆擂互升丹中丹手及井［扔□田□匹反戚及方丹卞烂聒今木化中月［
 	  
 		typedef enum
 		{
 			PETSKILL_TARGET_MYSELF,		愤坌及心
 			PETSKILL_TARGET_OTHER,		职及谛  愤坌殖戈)
 			PETSKILL_TARGET_ALLMYSIDE,	    蝈  
 			PETSKILL_TARGET_ALLOTHERSIDE,	锹澎础蝈  
 			PETSKILL_TARGET_ALL,		蝈化
 			PETSKILL_TARGET_NONE,		簿手蓟  请  卅中［  豢支凶户及凛
 			PETSKILL_TARGET_OTHERWITHOUTMYSELF,职及谛  愤坌殖引卅中) 
 			PETSKILL_TARGET_WITHOUTMYSELFANDPET, 愤坌午矢永玄动陆
 		}PETSKILL_TARGETTYPE;
 	  
 	  name 反  及  蟆［
 	  comment反公及  卞覆允月    ［
 	  target|name|comment| 反  及醒及坌ㄠ垫匹霜日木化仁月［
 	    反域杀ㄦ勾引匹［    匹  互  仃化中月  "|"及心互粮仁  桦宁
 	  反弁仿奶失件玄匹腴户化  憎允月仪［
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
           卞窒井  憎允月隙  ［
 
       
 	int category
 	窒毛  憎允月井［
 	    
 	        袄        
 	        1  愤坌动陆卞芨尹凶母丢□斥［data反  侬
 		    卞卅匀凶袄［  
 		    2  愤坌互丹仃凶母丢□斥［data反  侬  卞
 		      卅匀凶袄  
 		
 	int dx
 	穴奶平乓仿井日及弘伉永玉锹覆匏  X［域  读卞反奶矛件玄互  戏
 	    仄凶匏  ［弁仿奶失件玄反仇及袄井日赝给赝濠卅匏  毛煌遥仄化
 	        允月［
 	int dy
 	锹覆匏  Y
 	string data 
 	  憎允月    ［    反category卞方匀化瑁引月［
 	    仇及  侬  反<a href="#escaping">巨旦弗□皿今木卅仃木壬
 	    卅日卅中［
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
       (FlgSet)PS及及杀蚕［引凶反夫弘奶件凛卅升卞愤坌及橇谪午仄化霜日木化仁月［
       
         int flg
          0 bit  0: 醮棉Off                  1: 醮棉On
                蜇箕  银迕  1 bit  0: 爵      辅笛off          1: 爵      辅笛On    
              2 bit  0: DUEL off                 1: DUEL On
              3 bit  0:   骚及民乓永玄乒□玉     1: 由□  奴卞仄井民乓永玄互  壬卅中乒□玉
              4 bit  0:   铜跟晶OK               1:   铜跟晶蛐  
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
       (HeLp)HL及及杀蚕［引凶反由□  奴及醮棉互爵  及云踞仃乒□玉毛  凳仄凶桦宁卞霜日木化仁月［
       
         int flg
          0: 云踞仃乒□玉Off                  1: 云踞仃乒□玉On
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
       (PartyRequest)醮棉邰菲及杀蚕［PR毛弁仿奶失件玄互霜匀化中卅仁化手仇木毛熬仃潸月凛互丐月［
        由□  奴互  螺荸辣  褪互  仃凶  卅升及凛［
       
       	int request
       	0: 轮迩 1:  迩
         int result
         0: 撩   1: 岳  
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
       (PetSkill use result)矢永玄及  毛银匀凶瑛绊弁仿奶失件玄及PS卞覆杀仄化霜日木化仁月［
       result 动陆反弁仿奶失件玄及PS卞覆杀仄化中月［白奴□伙玉晓井日仄井裟壬木卅中［
       
 	int result
 	瑛绊［0: 撩   1:岳  
         int havepetindex
         窒    及矢永玄互银迕仄凶井［
         int havepetskill
         窒    及  毛银迕仄凶井［
 	int toindex
 	 簿卞  芊毛银迕仄凶井［仇木反左皮斥尼弁玄支平乓仿及index匹反卅中［动票及  卞卅匀化中月［
 	
 	  愤坌    = 0
 	  矢永玄  = 1   5
 	  醮棉    = 6   10   S N 及0  4卞覆杀［愤坌愤褥手殖引木化中月  
 	
 	覆擂互蝈够］午井坌井日卅中］午井及桦宁反-1匹霜耨允月［
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
       旦平伙失永皿互匹五月仪毛扔□田互骚襞允月［中仁勾晓仆日木月井毛隙烂允月［
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
       它奴件玉它毛  憎六方午扔□田□互弁仿奶失件玄卞骚襞允月［
       
         int windowtype
         它奴件玉它正奶皿［升及方丹卅溥挚及它奴件玉它毛  憎允月井［仇木午buttontype 及赚心宁歹六匹
             它奴件玉它互瑁烂允月［
             扔□田□匹反动票及  卞涩烂今木化中月［
           
 		typedef enum
 		{
 			WINDOW_MESSAGETYPE_MESSAGE,			丢永本□斥及心
 			WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,		丢永本□斥午域垫    
 			WINDOW_MESSAGETYPE_SELECT,			蓟  它奴件玉它
 			WINDOW_MESSAGETYPE_PETSELECT,			矢永玄蓟  它奴件玉它
 			WINDOW_MESSAGETYPE_PARTYSELECT,			醮棉蓟  它奴件玉它
 			WINDOW_MESSAGETYPE_PETANDPARTYSELECT,		矢永玄］醮棉蓟  它奴件玉它
 			WINDOW_MESSAGETYPE_ITEMSHOPMENU,		云饕及丢瓦亘□它奶件玉它
 			WINDOW_MWSSAGETYPE_ITEMSHOPYMAIN,		云饕及丢奶件它奶件玉它
 			WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,		  中潸曰毁  失奶  丞盒及丢奶件它奶件玉它
 			WINDOW_MESSAGETYPE_PETSKILLSHOP,		矢永玄及  盒今氏它奶件玉它
 			WINDOW_MESSAGETYPE_WIDEMESSAGE,			丢永本□斥及心    五中    
 			WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,	丢永本□斥午域垫        五中    
 
 		}WINDOW_MESSAGETYPE;
 
         int buttontype
         示正件及溥挚毛隙烂允月［扔□田□匹反动票及迕卞烂聒今木化中月［
             
 		#define		WINDOW_BUTTONTYPE_NONE		(0)
 		#define		WINDOW_BUTTONTYPE_OK		(1 << 0)
 		#define		WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
 		#define		WINDOW_BUTTONTYPE_YES		(1 << 2)
 		#define		WINDOW_BUTTONTYPE_NO		(1 << 3)
 		#define		WINDOW_BUTTONTYPE_PREV		(1 << 4)
 		#define		WINDOW_BUTTONTYPE_NEXT		(1 << 5)
             
             仇木日及赚心宁歹六匹霜耨允月［  尹壬］YES示正件午NO示正件互  仄中凛反
             WINDOW_BUTTONTYPE_YES | WINDOW_BUTTONTYPE_NO   (=12)
             匹霜月［
             
         int seqno
         仇及它奴件玉它及  寞毛憎允［扔□田□互栋  允月［
             弁仿奶失件玄反WN卞化仇及它奴件玉它及丑综瑛绊毛忒允及卞］仇及  寞毛骄尹化忒蚕允月［
             仇木卞方匀化扔□田□反升及NPC及升及桦  匹及它奴件玉它井毛  蝇请  月方丹卞允月［
         int objindex
         仇及它奴件玉它毛请六午蜕匀凶NPC卅升及index互伞  今木化中月［
             扑旦  丞互请六午蜕匀凶桦宁反-1卅升互  匀化中月［
             弁仿奶失件玄反］它奴件玉它      及WN皿夫玄戊伙匹仇及醒袄毛公及引引忒六壬  中［
         string data
         丢永本□斥    毛憎允［    反巨旦弗□皿允月［"\n"匹嗉濠月午荼垫午中丹啦  午允月［
             引凶］弁仿奶失件玄及它奴件玉它匹  憎请  月产汔毛旋尹凶端反愤  读卞荼垫今木月［
             蓟  饷及丐月它奴件玉它匹反］"\n"匹嗉濠日木凶  赓及玄□弁件互丢永本□斥及垫醒
             午卅曰］戚卞丢永本□斥及玄□弁件互  赓及玄□弁件匹隙烂今木凶蜊醒粮五］
             公及丐午及玄□弁件  互ㄠ勾内勾及蓟  饷午卅月［引凶蟆井日赐卞 1 井日
               寞毛喃曰癫化］蓟  仄凶桦宁及WN匹及忒蚕及凛卞忒允［
         string data(云饕迕  
         丢永本□斥    毛憎允［    反巨旦弗□皿允月［"\n"匹嗉濠月午荼垫午中丹啦  午允月［
             引凶］弁仿奶失件玄及它奴件玉它匹  憎请  月产汔毛旋尹凶端反愤  读卞荼垫今木月［
             犯□正  反嫩    卞"  "匹嗉濠日木化中引允［
    Ｕ  丹Ｖ
  	  中  月白仿弘    丹ㄟ“  月ㄠ    蟆及犯□正银丹井白仿弘  银丹ㄟ“银歹卅中“1    
 		饕及  蟆  丢永本□斥  饕丢永本□斥  蜊醒蓟  丢永本□斥  伊矛伙箫曰卅中丢永本□斥  割  丢永本□斥  
 		失奶  丞互中匀天中丢永本□斥  失奶  丞      尹月  尹卅中白仿弘    尹月“ㄟ  尹卅中“ㄠ    失奶  丞伊矛伙  
 		袄蟾    飓  寞        失奶  丞      尹月  尹卅中白仿弘  失奶  丞伊矛伙  袄蟾    飓  寞      
       		
       		0|1|扔丞幼伙及  湛盒|中日匀仄扎中｝窒及方丹分中|中日匀仄扎中升木卞允月|窒蜊  邰分中|公木毛隶  允月卞反伊矛伙互
       		箫曰卅中仃升中中及井中|  癫卞中中氏分友|云中云中失奶  丞互中匀天中元扎友尹井″  皮□丢仿件|
       		ㄟ|ㄠㄟㄟ|ㄠㄥㄟㄟㄟ|产域  卞  猾\n躲绊  歇|泔阕|ㄟ|ㄤㄟ|ㄠㄥㄟㄟㄡ|    ㄡㄟ荚汊\n  莴卞反勾五  \n兜中  
 
         Ｕ  月Ｖ
 		  中  月白仿弘    丹ㄟ“  月ㄠ    蟆及犯□正银丹井白仿弘  银丹ㄟ“银歹卅中“1    
 		饕及  蟆  丢永本□斥  饕丢永本□斥  云嗯互中匀天中卞卅匀切扎丹丢永本□斥|割  丢永本□斥  
 		失奶  丞      木月  木卅中白仿弘  袄蟾    飓  寞        失奶  丞      木月  木卅中白仿弘  
 		袄蟾    飓  寞        
       
       		ㄠ|1|扔丞幼伙及  填盒|中日匀仄扎中｝升月允月＂  中日匀仄扎中升木毛  匀化仁木月［|  癫卞中中氏分友|
       		公木分午｝云嗯互左□田□仄切扎丹友  皮□丢仿件|ㄟ|ㄡㄟ|ㄠㄥㄟㄟㄟ|
       		产域  卞  猾\n躲绊  歇|泔阕|ㄟ|ㄠㄟ|ㄠㄥㄟㄟㄡ|    ㄡㄟ荚汊\n  莴卞反勾五  \n兜中  
         Ｕ失奶  丞  仃月Ｖ
 		白仿弘    月=ㄟ    丐午窒蜊  仃日木月井  
 		饕及  蟆  
 		  仃月丢永本□斥  仇木动晓  仃日木卅中丢永本□斥  割  丢永本□斥  
 		  蟆    仃日木月井白仿弘  第  ＞ㄟ］尕第＞ㄠ    袄蟾    飓  寞  域垫奶件白巧  失奶  丞    寞  ㄠ井日  
 		  蟆    仃日木月井白仿弘  第  ＞ㄟ］尕第＞ㄠ    袄蟾    飓  寞  域垫奶件白巧  失奶  丞    寞  ㄠ井日  
         Ｕ失奶  丞娄五潸月Ｖ
 		白仿弘  娄五潸月=ㄠ    
 		饕及  蟆  娄五请允丢永本□斥  失奶  丞互域  及凛及丢永本□斥|割  丢永本□斥  
 		  蟆  娄五请六月井升丹井白仿弘  第  ＞ㄟ］尕第＞ㄠ    失奶  丞伊矛伙  袄蟾    飓  寞  域垫奶件白巧  
 		  蟆  娄五请六月井升丹井白仿弘  第  ＞ㄟ］尕第＞ㄠ    失奶  丞伊矛伙  袄蟾    飓  寞  域垫奶件白巧  
 
         Ｕ公及职Ｖ
 		饕及  蟆  丢永本□斥
       
       		扔丞幼伙及  填盒|中日匀仄扎中｝升月允月＂
 	
         string data(矢永玄及  盒今氏迕  
         丢永本□斥    毛憎允［    反巨旦弗□皿允月［"\n"匹嗉濠月午荼垫午中丹啦  午允月［
             引凶］弁仿奶失件玄及它奴件玉它匹  憎请  月产汔毛旋尹凶端反愤  读卞荼垫今木月［
             犯□正  反嫩    卞"  "匹嗉濠日木化中引允［
 	   Ｕ矢永玄及  潸  Ｖ
 		蟆及树  银丹井升丹井＂  银丹“0｛银歹卅中“1    饕及  蟆  丢奶件丢永本□斥  
 		      袄蟾                袄蟾        
 	
 		1  扔丞幼伙及  湛盒  中日匀仄扎中引匀六［允壬日仄中  壬匀井匹允方  
 		    及讽公及迭  500    猾  ㄢㄟ⊙    ｛  豢  ㄢㄟ⊙          
 		哗萄域葱  1000  蕙仄中  苇互丐月井手  赝癫    	
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
       湿支惚毛嫦日仄凶曰［弁仿奶失件玄卞蝈  读卅躲绊  蜇六方午霜月［
       
         int effect
         躲绊  寞［公木冗木毛箫仄遥允月午］    午手及躲绊互  日木引允［ㄢ卞允月午惚午湿互嫦匀凶曰午井仄引允［
           
             1:惚
             2:湿
             4:饼蹄湿    烂  
           
         int level
         躲绊及雄今［ㄟ反壅允［ㄠ  ㄤ反  蜇及雄今毛  允［
         string option
         漆  傀舰迕
*/
/* Arminius
  新增给丢骰子的动画
  int effect = 10   丢骰子
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
 	弁仿奶失件玄卞SE毛  日允方丹卞隙憎允月［
       
         int x,y
         SE毛  耨葭及甄  ［
         int senumber
             及  寞
         int sw
             毛  日允井鞅户月井［
           
             0:鞅户月
             1:  日允
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
       ClientLogin及忒蚕［
       
 	string result
 	"ok" 午中丹  侬  ［仇及  侬  反巨旦弗□皿今木卅中［
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
       CreateNewChar及忒蚕［
       
 	string result
 	"successful" 井 "failed" 及中内木井［仇及  侬  反巨旦弗□
 	    皿仄卅中［
 	string data
 	"failed" 及凛反    毛憎允谛棉及苇化坌月  侬
 	      匹丐月［失市它件玄扔□田井日及忒蚕公及引引匹丐月［
 	    动票及  侬  

 	    "failed bad parameter"

 	    及桦宁反｝平乓仿综岳及午五卞筋烂及由仿丢□正及  区毛旋尹化
 	    中月午中丹午毛啦  允月［仇木反必□丞扔□田□互请  允月丢永
 	    本□斥匹丐月［仇及  侬  反巨旦弗□皿仄卅中［
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
       CharDelete 及忒蚕［
       
 	string result
 	"successful" 井 "failed" 及中内木井［巨旦弗□皿仄卅中［
 	string data
 	"failed" 及凛反    毛憎允谛棉及苇化坌月  侬
 	      匹丐月［失市它件玄扔□田井日及忒蚕公及引引匹丐月［
 	    犯伉立正毛孔仁引卅中及匹｝巨旦弗□皿仄卅中［
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
       CharaLogin及忒蚕［
       
 	string result
 	"successful" 井 "failed" 及中内木井［巨旦弗□皿仄卅中［
 	string data
 	"failed" 及凛反 公及    及  侬  ［巨旦弗□皿仄卅中［
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
       CharList及忒蚕［
       
 	string result
 	"successful" 井 "failed" 及中内木井［巨旦弗□皿仄卅中
 	string data
 	result互"successful"及凛反｝失市它件玄扔□田□卞忡绣今木化
 	    中月允屯化及平乓仿及  蟆｝左皿扑亦件毛毛旦矢□旦匹嗉濠匀凶
 	    域蜊及  侬  ［仇及  侬  毛综匀化中月及反｝失市它件玄扔□田［
 	    result 互 "failed" 及凛反    毛憎允谛棉及苇化坌月  侬  匹
 	    丐月［岳  及凛及左皿扑亦件及  褥反动票及午云曰匹丐月［
 	    dataplace|faceimage|level|maxhp|atk|def|quick|charm|earth|water|fire|wind|logincount|name|place
 	    
 	      dataplace
 	      本□皮犯□正及窒    井［仇木匹平乓仿弁正□伉旦玄及窒    井毛瑁烂允月［
 	      faceimage
 	      涌及  飓  寞
 	      level
 	      平乓仿及伊矛伙
 	      maxhp,atk,def,quick,charm
 	      备由仿丢□正［
 	      earth.water,fire,wind
 	      备箪岭袄
 	      logincount
 	      夫弘奶件市它件玄
 	      name
 	      平乓仿及  蟆
 	      place
 	      平乓仿及中月桦赭
 	    
 	    "|" 匹嗉濠日木化中月［ 公木冗木及嫩  反｝<a
 	    href="#escaping">巨旦弗□皿今木化中月［公及丐午凶化徇
 	    匹勾卅仆月［
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
       Logout卞覆允月忒蚕［
       
 	string result
 	"successful" 井 "failed" 及中内木井［巨旦弗□皿仄卅中［
 	string data
 	"failed" 及凛卞及心啦  互丐曰｝撩  及    (橇谪)毛憎允谛棉
 	    及苇化坌月  侬  匹丐月［巨旦弗□皿仄卅中［
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
 	ProcGet及忒蚕［
       
 	string data
 	巨旦弗□皿允月［必□丞扔□田□及    树  毛霜耨允月［    反log/proc白央奶伙卞踏井木月    午  元［
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
 	PlayerNumGet及忒蚕［
       
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
       Echo卞覆允月忒蚕［
       
 	string test
 	今五幻升    今木凶  侬  ［巨旦弗□皿仄卅中［
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
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
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
#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
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
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
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
#ifdef _MAGIC_NOCAST             // 精灵:沉默
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
