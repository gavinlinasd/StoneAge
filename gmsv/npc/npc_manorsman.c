#include "version.h"
#include <time.h>
#include "char.h"
#include "object.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "handletime.h"
#include "npc_scheduleman.h"
#include "family.h"
#include "npc_manorsman.h"
#include "saacproto_cli.h"
#include "net.h"
#include "configfile.h"
#include "log.h"

/*
 * 庄园 PK 登记员
 *
 */
#define MAX_MANORSMAN 22
static int FMPK_ManorsmanList[MAX_MANORSMAN]={
-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,
-1,-1 };

enum {
	NPC_WORK_ID = CHAR_NPCWORKINT1,		// 登记员 ID, 从 0 开始
	NPC_WORK_MANORID = CHAR_NPCWORKINT2,	// 庄园编号
	NPC_WORK_CHALLENGEWAIT = CHAR_NPCWORKINT3,	// 挑战等待时间
	NPC_WORK_PEACEWAIT = CHAR_NPCWORKINT4,	// 休战时间
	NPC_WORK_PREVLOOPTIME = CHAR_NPCWORKINT5,	// 前一次处理 Loop 的时间
#ifdef _NEW_MANOR_LAW
	NPC_WORK_BETTLETIME = CHAR_NPCWORKINT6	// 记录开打时的日期
#endif
};

#define SCHEDULEFILEDIR		"./Schedule/"

#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 将可挑战庄园的由前十大改为前二十大
#define PK_LIMIT	20	       
#endif


extern struct FM_POINTLIST  fmpointlist;
extern void NPC_talkToFloor(int floor, int index1, int index2, char *data);
#ifdef _MANOR_PKRULE
extern struct  FMS_DPTOP fmdptop;
#endif

void NPC_ManorLoadPKSchedule(int meindex);	// Load schedule from disk
#ifdef _NEW_MANOR_LAW
void SortManorSchedule();
int SortManorScheduleMomentum(const void *indexa, const void *indexb);
extern int familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];
int g_iSortManor;
#endif

int NPC_getManorsmanListIndex( int ID)
{

	if( ID < 0 || ID >= MAX_MANORSMAN ){
		return	FMPK_ManorsmanList[ ID];
	}
	return -1;
}
BOOL NPC_ManorSmanInit( int meindex )
{
  char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
  int meid;
  int a;

  CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEMANORSCHEDULEMAN );

  // 参数
  NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));
  meid = NPC_Util_GetNumFromStrWithDelim(argstr, "id" );
  if ((meid<0) || (meid>=MAX_SCHEDULEMAN)) {
    print("MANORSMAN init error: invalid ID(%d)\n",meid);
    meid=0;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "loop" );
  if ((a<100) || (a>10000)) a=1000;
  CHAR_setInt(meindex, CHAR_LOOPINTERVAL, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "manorid" );
  if ((a<1) || (a>/*4*/MANORNUM)) {// CoolFish 2002/2/25
    print("MANORSMAN init error: invalid manor id(%d).", a);
    a=1;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_MANORID, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "challengewait");
  if ((a<1) || (a>259200)) {
    print("MANORSMAN init error: invalid challengewait(%d).",a);
    a=259200;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_CHALLENGEWAIT, a);

  a = NPC_Util_GetNumFromStrWithDelim(argstr, "peacewait");
  if ((a<1) || (a>432000)) {
    print("MANORSMAN init error: invalid peacewait(%d).",a);
    a=604800;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_PEACEWAIT, a);

  NPC_ManorLoadPKSchedule(meindex);

  CHAR_setWorkInt(meindex, NPC_WORK_PREVLOOPTIME, NowTime.tv_sec);
#ifdef _NEW_MANOR_LAW
	CHAR_setWorkInt(meindex,NPC_WORK_BETTLETIME,0);
#endif

  if( CHAR_getWorkInt( meindex, NPC_WORK_ID) >= 0 &&
	  CHAR_getWorkInt( meindex, NPC_WORK_ID) <MAX_MANORSMAN ){
		FMPK_ManorsmanList[ CHAR_getWorkInt( meindex, NPC_WORK_ID)] = meindex;
  }

  return TRUE;
}

#ifndef _NEW_MANOR_LAW
void NPC_ManorSmanTalked(int meindex, int talkerindex, char *msg, int color)
{
  char buf[4096];
  int fd;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  if (NPC_Util_CharDistance(meindex, talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;

  switch (fmpks[fmpks_pos+1].flag) {
  case FMPKS_FLAG_NONE:
    // 没有约定战斗，此时可以下战书
    saacproto_ACFMPointList_send(acfd);
#ifdef _MANOR_PKRULE
	sprintf(buf, "庄园所有权争夺战的挑战资格\n\n"
                 "一、没有拥有庄园的家族\n"
	#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 将可挑战庄园的由前十大改为前二十大
				 "二、家族排行必需为前二十大家族\n"
	#else
                 "二、家族排行必需为前十大家族\n"
	#endif
				 "叁、踢馆时间：下午六点至凌晨叁点\n\n"
                 "请稍待，我将确认你的资格。");
#else
    sprintf(buf, "庄园所有权争夺战的挑战资格\n\n"
                 "一、没有拥有庄园的家族\n"
                 "二、家族的声望高於庄园家族的声望\n\n"
                 "请稍待，我将确认你的资格。");
#endif
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	    		WINDOW_BUTTONTYPE_YESNO,
    			CHAR_WINDOWTYPE_CHECKMAN_START,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  case FMPKS_FLAG_MANOR_PREPARE:
    // 目前已经定好战斗，准备中
    {
      int timeleft=fmpks[fmpks_pos+1].dueltime-NowTime.tv_sec;
      int dd,hh,mm;
      char buf2[4096];
      dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;
      hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd*24;
      mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd*24*60 - hh*60;
      memset(buf2,0,sizeof(buf2));
      if (dd>0) sprintf(buf, " %d 天", dd); else strcpy(buf, "");
      strcat(buf2, buf);
      if (hh>0) sprintf(buf, " %d 小时", hh); else strcpy(buf, "");
      strcat(buf2, buf);
      if (mm>0) sprintf(buf, " %d 分锺", mm); else strcpy(buf, "");
      strcat(buf2, buf);
      if (strlen(buf2)==0)
        sprintf(buf, "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
                    "即将开始，请准备入场。",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name);
      else
        sprintf(buf, "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
                    "预定将在%s後开始。",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name,
    		buf2);
      lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    }
    break;
  case FMPKS_FLAG_MANOR_PEACE:
    // 战斗已经结束的和平时期
    {
      int timeleft=fmpks[fmpks_pos+1].dueltime-NowTime.tv_sec;
      int dd,hh,mm;
      char buf2[4096];
      dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;
      hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd*24;
      mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd*24*60 - hh*60;
      strcpy(buf2,"");
      if (dd>0) sprintf(buf, " %d 天", dd); else strcpy(buf, "");
      strcat(buf2, buf);
      if (hh>0) sprintf(buf, " %d 小时", hh); else strcpy(buf, "");
      strcat(buf2, buf);
      if (mm>0) sprintf(buf, " %d 分锺", mm); else strcpy(buf, "");
      strcat(buf2, buf);

      if (strlen(buf2)==0)
        strcpy(buf, "请稍等，让我准备一下申请踢馆的表格。");
      else
        sprintf(buf, "现在是休战时期，要踢馆的话\n请%s後再来申请。",buf2);
      lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
    }
    break;
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    // 目前正在进行踢馆
    sprintf(buf, "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
                 "开～打～罗～\n还没进场的人赶快进场吧。",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name);
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    // 在别的星球进行战斗
    sprintf(buf, "庄园所有权争夺战\n『%s ＶＳ %s』\n\n"
    		 "决斗地点在 %s 。",
              fmpks[fmpks_pos+1].guest_name,
              fmpks[fmpks_pos+1].host_name,
              fmpks[fmpks_pos+2].host_name);
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  }
}
#else
void NPC_ManorSmanTalked(int meindex, int talkerindex, char *msg, int color)
{
  char buf[4096];
  int fd;
  int fmpks_pos = CHAR_getWorkInt(meindex,NPC_WORK_ID)*MAX_SCHEDULE;
	int manorid = CHAR_getWorkInt(meindex,NPC_WORK_MANORID);

	if(CHAR_CHECKINDEX(meindex) == FALSE){
		printf("\nNPC_ManorSmanTalked error!(meindex:%d)",meindex);
		return;
	}
  if(NPC_Util_CharDistance(meindex,talkerindex) > 2) return;
  if(!NPC_Util_isFaceToFace(meindex,talkerindex,2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if(fd == -1) return;

	if(fmpks[fmpks_pos+1].flag == FMPKS_FLAG_CHALLENGE) fmpks[fmpks_pos+1].flag = FMPKS_FLAG_NONE;

  switch(fmpks[fmpks_pos+1].flag){
	// 挑战期 1800~2200
  case FMPKS_FLAG_NONE:
	case FMPKS_FLAG_WAIT:
    saacproto_ACFMPointList_send(acfd);
		sprintf(buf,"庄园所有权争夺战的挑战资格\n\n"
								"一、没有庄园的家族\n"
								"二、家族的气势高於庄园家族的气势\n"
								"叁、约战时间终止时，气势排名第一者获得挑战资格\n"
								"四、气势相同者以最先报名者获得资格\n"
								"按 OK 我将确认你的资格，按 NO 观看挑战排名");
    lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_YESNO,CHAR_WINDOWTYPE_CHECKMAN_START,
    								 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),buf);
		if(fmpointlist.fm_momentum[manorid-1] <= -1){
			int hadfmindex,index;
			char token[256];

			getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",5,token,sizeof(token));
			hadfmindex = atoi(token);
			if(hadfmindex != -1){
				for(index=0;index<FAMILY_MAXNUM;index++){
					// 找出守庄家族的索引
					if(fmdptop.fmtopid[index] == (hadfmindex - 1)) break;
				}
				if(index >= FAMILY_MAXNUM){
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",6,token,sizeof(token));
					printf("\nNPC_ManorSmanTalked():save fm_momentum error(%d:%s)",hadfmindex,token);
					break;
				}
				// 记录这个时刻守庄家族的气势
				else fmpointlist.fm_momentum[manorid-1] = fmdptop.fmMomentum[index];
			}
		}
		SortManorSchedule();
    break;
	// 挑战准备期 2200~取得挑战资格的家族当时挑战的时间(隔天的 1800~2200 之间)
  case FMPKS_FLAG_MANOR_PREPARE:
    // 目前已经定好战斗，准备中
    {
      int timeleft = fmpks[fmpks_pos+1].dueltime - NowTime.tv_sec;
      int mm,hh,dd;
      char buf2[4096];

			dd = timeleft / 86400;
			hh = timeleft / 3600 - dd * 24;
      mm = timeleft / 60 - dd * 24 * 60 - hh * 60;
      memset(buf2,0,sizeof(buf2));
			if(dd > 0) sprintf(buf," %d 天",dd); else strcpy(buf,"");
      strcat(buf2,buf);
			if(hh > 0) sprintf(buf," %d 小时",hh); else strcpy(buf,"");
      strcat(buf2,buf);
      if(mm > 0) sprintf(buf," %d 分锺",mm); else strcpy(buf,"");
      strcat(buf2,buf);
      if(strlen(buf2) == 0)
        sprintf(buf,"庄园所有权争夺战\n『%s ＶＳ %s』\n\n即将开始，请准备入场。",
    						fmpks[fmpks_pos+1].guest_name,fmpks[fmpks_pos+1].host_name);
      else
				sprintf(buf,"庄园所有权争夺战\n『%s ＶＳ %s』\n\n预定将在%s後开始。",
    						fmpks[fmpks_pos+1].guest_name,fmpks[fmpks_pos+1].host_name,buf2);
      lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
    									 CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX),buf);
    }
    break;
	// 休战期及准备期,五天
  case FMPKS_FLAG_MANOR_PEACE:
	case FMPKS_FLAG_MANOR_PEACE_SAVE:
    {
      int timeleft = fmpks[fmpks_pos+1].dueltime-NowTime.tv_sec;
      int dd,hh,mm;
      char buf2[4096];
      dd = timeleft / 86400;
      hh = timeleft / 3600 - dd*24;
      mm = timeleft / 60 - dd*24*60 - hh*60;
      memset(buf2,0,sizeof(buf2));
      if(dd > 0) sprintf(buf," %d 天",dd); else strcpy(buf,"");
      strcat(buf2,buf);
      if(hh > 0) sprintf(buf," %d 小时",hh); else strcpy(buf,"");
      strcat(buf2,buf);
      if(mm > 0) sprintf(buf," %d 分锺",mm); else strcpy(buf,"");
      strcat(buf2,buf);

      if(strlen(buf2) == 0) strcpy(buf, "请稍等，让我准备一下申请踢馆的表格。");
      else sprintf(buf, "现在非挑战时期，要挑战的话\n请%s後再来申请。",buf2);
      lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
											 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),buf);
    }
    break;
	// 挑战中
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    sprintf(buf,"庄园所有权争夺战\n『%s ＶＳ %s』\n\n开～打～罗～\n还没进场的人赶快进场吧。",
    				fmpks[fmpks_pos+1].guest_name,fmpks[fmpks_pos+1].host_name);
    lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
    								 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),buf);
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    // 在别的星球进行战斗
    sprintf(buf,"庄园所有权争夺战\n『%s ＶＳ %s』\n\n决斗地点在 %s 。",
            fmpks[fmpks_pos+1].guest_name,fmpks[fmpks_pos+1].host_name,fmpks[fmpks_pos+2].host_name);
    lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
    								 CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX),buf);
    break;
  }
}
#endif

void NPC_ManorSmanWindowTalked(int meindex, int talkerindex,
				int seqno, int select, char *data)
{
  int fd, fmpks_pos, manorid, tkfmindex, tkfmdp;
  char buf[4096],token[256];
  int hadfmindex;
#ifndef _ACFMPK_LIST
  char hadfmname[256];
#endif
  int hadfmpopular;
  
  if (NPC_Util_CharDistance(meindex,talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;
  fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
  tkfmindex = CHAR_getWorkInt(talkerindex, CHAR_WORKFMINDEXI);
  tkfmdp = CHAR_getWorkInt(talkerindex, CHAR_WORKFMDP);
  getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1], "|", 5, token, sizeof(token));
  hadfmindex = atoi(token);
  getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1], "|", 7, token, sizeof(token));
  hadfmpopular = atoi(token);
  
  switch (seqno){
  case CHAR_WINDOWTYPE_CHECKMAN_START:
    if (select==WINDOW_BUTTONTYPE_YES){
#ifdef _ACFMPK_NOFREE
		sprintf(buf, "庄园挑战会有一天的准备期\n"
				"你可以随时询问我以得知剩下的时间\n"
				"挑战结束後不论胜负将会有二天的休战期\n"
				"休战期间不能挑战\n\n"
				"你确定要挑战这个庄园吗？");
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
				WINDOW_BUTTONTYPE_YESNO,
				CHAR_WINDOWTYPE_CHECKMAN_MAIN,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
				return;
#endif

#ifdef _FMVER21      
		  if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
#else
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&
#endif      
				(CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)){
        if (hadfmindex != -1) {
          if (hadfmindex-1 != tkfmindex){
            int check=0,i;
            // Arminius 2.25 fix: fmpks 中第 1~"MANORNUM" 组一定要给 manorsman
            for (i=0; i<=/*3*/MANORNUM-1; i++) {	// 9个庄园
              getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, token, sizeof(token));
              if (tkfmindex==atoi(token)-1) check=1;
            }
            for (i=1; i<=/*4*/MANORNUM; i++) {	// 是否已经挑战其他庄园
              if ((fmpks[i*MAX_SCHEDULE+1].guest_index==tkfmindex) &&
								(strcmp(fmpks[i*MAX_SCHEDULE+1].guest_name,
								CHAR_getChar(talkerindex, CHAR_FMNAME))==0)
								) {
                check=2;
              }
            }
            if (check==0){
							int won1;
#ifdef _MANOR_PKRULE		    // WON ADD 2002/01/22
	#ifdef _NEW_MANOR_LAW
		//-----------------------------------------
							int index;
							won1 = 1;

							for(index=0;index<FAMILY_MAXNUM;index++){
								// 找出挑战家族的索引
								if(fmdptop.fmtopid[index] == tkfmindex) break;
							}
							if(index >= FAMILY_MAXNUM){
								printf("\nNPC_ManorSmanWindowTalked():find tkfmIndex error (%d)",tkfmindex);
							}
							else
							// 如果挑战家族气势不足守庄家族气势的九成,不能挑战
							if(fmdptop.fmMomentum[index] < fmpointlist.fm_momentum[manorid-1]*0.9) won1 = 0;
		//-----------------------------------------
	#else
		//-----------------------------------------
							int i;
							char won2[256];
							won1 = 0;
		#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 将可挑战庄园的由前十大改为前二十大		
							for(i=0; i< PK_LIMIT; i++){
		#else
							for(i=0; i<10; i++){
		#endif
								if( getStringFromIndexWithDelim( fmdptop.topmemo[i], "|", 3, won2, sizeof(won2)) == FALSE )	{
									print("err Get fmdptop.topmemo[%d] if FALSE !!\n", i);
									break;
								}
								if( strcmp(CHAR_getChar(talkerindex, CHAR_FMNAME) ,won2) == 0 ){
									won1= 1;
									break;
								}
		//-----------------------------------------
							}
	#endif

							if(won1 == 1){		
			// WON END
	#ifdef _NEW_MANOR_LAW
							sprintf(buf,"确定庄园挑战资格後会有一天的准备时间\n"
													"你可以随时询问我以得知剩下的时间\n"
													"挑战结束後至下次挑战期将会有一天的休战期\n"
													"和四天的准备期，在这五天期间内不能挑战\n\n"
													"你确定要挑战这个庄园吗？");
	#else
							sprintf(buf,"庄园挑战会有一天的准备期\n"
													"你可以随时询问我以得知剩下的时间\n"
													"挑战结束後不论胜负将会有二天的休战期\n"
													"休战期间不能挑战\n\n"
													"你确定要挑战这个庄园吗？");
	#endif
#else
              if(tkfmdp >= hadfmpopular){
								sprintf(buf,"庄园挑战会有一天的准备期\n"
                            "你可以随时询问我以得知剩下的时间\n"
												    "挑战结束後不论胜负将会有五天的休战期\n"
														"休战期间不能挑战\n\n"
														"你确定要挑战这个庄园吗？");
#endif
                lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    							WINDOW_BUTTONTYPE_YESNO,
    															CHAR_WINDOWTYPE_CHECKMAN_MAIN,
    															CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    															buf);
    					}
							else{
#ifdef _NEW_MANOR_LAW
								sprintf(buf, "你的家族气势尚未到达庄园家族气势的九成，请再加油。");
#else
	#ifdef _MANOR_PKRULE
		#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 将可挑战庄园的由前十大改为前二十大
                sprintf(buf, "你的家族声望尚未进入前二十大家族，请再加油。");
		#else
                sprintf(buf, "你的家族声望尚未进入前十大家族，请再加油。");
		#endif
	#else
                sprintf(buf, "你的家族声望尚不足以挑战\n"
                             "拥有这个庄园的家族，请再加油。");
	#endif
#endif
                lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						              	    	WINDOW_BUTTONTYPE_OK,
													   			CHAR_WINDOWTYPE_CHECKMAN_END,
												    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
													   			buf);
		   	      }
            }
						else if(check == 2){
              sprintf(buf, "你的家族正在挑战其他庄园，\n"
                           "请把机会留给其他家族吧！");
              lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
													    	WINDOW_BUTTONTYPE_OK,
											    			CHAR_WINDOWTYPE_CHECKMAN_END,
											    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
											    			buf);
            }
						else{
              sprintf(buf, "一个家族只能拥有一个庄园，\n"
                           "如果庄园家族之间需要切磋\n"
                           "请到家族ＰＫ场。");
              lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    						WINDOW_BUTTONTYPE_OK,
											    			CHAR_WINDOWTYPE_CHECKMAN_END,
											    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
										    				buf);
            }
          }
					else{
            sprintf(buf, "这个庄园已经是你的家族的喔。");
            lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              						  	WINDOW_BUTTONTYPE_OK,
									    				CHAR_WINDOWTYPE_CHECKMAN_END,
											   			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
										    			buf);
          }
        }
				else{
          sprintf(buf, "现在并没有庄园家族，\n"
                       "直接去申请迁入就可以了哟。");
          lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			              	    	WINDOW_BUTTONTYPE_OK,
    												CHAR_WINDOWTYPE_CHECKMAN_END,
									    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
									    			buf);
        }
      }
			else{
        sprintf(buf, "只有族长可以下战书喔。");
        lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    			WINDOW_BUTTONTYPE_OK,
									   			CHAR_WINDOWTYPE_CHECKMAN_END,
								    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
								    			buf);
      }
    }
#ifdef _NEW_MANOR_LAW
		else if(select == WINDOW_BUTTONTYPE_NO){
			int i,index,manorindex;
			char szMsg[3072];

			sprintf(buf,"              庄园挑战排名表\n名次 家族名称         时间          气势\n");
			manorindex = CHAR_getWorkInt(meindex,NPC_WORK_MANORID) - 1;
			// 送出挑战排名
			for(i=0;i<10;i++){
				index = ManorSchedule[manorindex].iSort[i];
				if(ManorSchedule[manorindex].iFmIndex[index] != -1){
					sprintf(szMsg,"%2d   %s\n",i+1,ManorSchedule[manorindex].szMemo[index]);
					strcat(buf,szMsg);
				}
			}
			lssproto_WN_send(fd,WINDOW_FMMESSAGETYPE_MANOR_SCHEDULE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
								    	 CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX),buf);
		}
#endif
    break;
  case CHAR_WINDOWTYPE_CHECKMAN_MAIN:	  
    if (select==WINDOW_BUTTONTYPE_YES) {

#ifdef _ACFMPK_NOFREE
	#ifndef _NEW_MANOR_LAW
		NPC_ManorSavePKSchedule(meindex, talkerindex, 0);
		sprintf( buf, "庄园挑战登陆确认中，请稍後。");
		CHAR_talkToCli( talkerindex, meindex, buf, CHAR_COLORYELLOW);
	#else
		NPC_ManorAddToSchedule(meindex,talkerindex);
	#endif
		return;
#endif

#ifdef _FMVER21    
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) && 
#else
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&     
#endif      
          (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1) &&   
          (hadfmindex != -1) &&
		  (hadfmindex - 1 != tkfmindex)    
#ifndef _MANOR_PKRULE
          && (tkfmdp >= hadfmpopular)
#endif
		  ){
#ifndef _ACFMPK_LIST
		  struct tm tm1; 
#endif

#ifndef _NEW_MANOR_LAW
	#ifdef _MANOR_PKRULE
		  time_t timep;
		  struct tm *p;		  
		  time(&timep);
		  p = gmtime(&timep);		
		  if( p->tm_hour+8 > 24)
		  	  p->tm_hour = p->tm_hour-16;
		  else
			  p->tm_hour = p->tm_hour+8;		 
		  if(p->tm_hour<18 && p->tm_hour>2){
			  sprintf(buf, "请於下午６：００至凌晨３：００再来约战吧！");
			  lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			        CHAR_WINDOWTYPE_CHECKMAN_END,
    			        CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			        buf);
			  break;
		  }		  
	#endif
			// WON ADD 修正家族pk场的约战问题
			if( fmpks[fmpks_pos+1].flag != FMPKS_FLAG_NONE ){
			     sprintf(buf, "这个庄园已经有人约战了喔。");
					 lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						 WINDOW_BUTTONTYPE_OK,
						 CHAR_WINDOWTYPE_CHECKMAN_END,
						 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
						 buf);
					 return;
			}
#endif
#ifdef _ACFMPK_LIST
	#ifndef _NEW_MANOR_LAW
		NPC_ManorSavePKSchedule(meindex, talkerindex, 0);
		sprintf( buf, "庄园挑战登陆确认中，请稍後。");
		CHAR_talkToCli( talkerindex, meindex, buf, CHAR_COLORYELLOW);
	#else
		NPC_ManorAddToSchedule(meindex,talkerindex);
	#endif
#else
	    memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
        getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1], "|", 6, token, sizeof(token));
        strcpy(hadfmname, token);

        fmpks[fmpks_pos+1].dueltime = CHAR_getWorkInt(meindex, NPC_WORK_CHALLENGEWAIT) + NowTime.tv_sec
                                      - tm1.tm_min*60 + 1800; // Arminius 11.1 改成一律 xx:30 开打
        fmpks[fmpks_pos+1].host_index = hadfmindex-1;
        strcpy(fmpks[fmpks_pos+1].host_name, hadfmname);
        fmpks[fmpks_pos+1].guest_index = tkfmindex;
        strcpy(fmpks[fmpks_pos+1].guest_name, CHAR_getChar(talkerindex, CHAR_FMNAME));        
        fmpks[fmpks_pos+1].prepare_time = 15;
        fmpks[fmpks_pos+1].max_player = 50;
        fmpks[fmpks_pos+1].win = -1;
        fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_OTHERPLANET;
        strcpy(fmpks[fmpks_pos+2].host_name, getGameserverID());

	#ifndef _NEW_MANOR_LAW
		NPC_ManorSavePKSchedule(meindex, talkerindex, 0);
		sprintf(buf, "庄园挑战已经设定完成，请好好准备。");
        lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
	#else
		NPC_ManorAddToSchedule(meindex,talkerindex);
	#endif
    fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_PREPARE;
#endif
        
      }
    }
    break;
  }
}

void NPC_CleanPkList( int ti)
{
// Terry fix 要清为-1不能清为0
/*	fmpks[ ti+1 ].host_index = 0;
	fmpks[ ti+1].guest_index=0;
	fmpks[ ti].host_index=0;
	fmpks[ ti].guest_index=0;*/
	fmpks[ ti+1 ].host_index = -1;
	fmpks[ ti+1].guest_index=-1;
	fmpks[ ti].host_index=-1;
	fmpks[ ti].guest_index=-1;

	strcpy(fmpks[ ti+1].host_name,"");
	strcpy(fmpks[ ti+1].guest_name,"");
	strcpy(fmpks[ ti].host_name,"");
	strcpy(fmpks[ ti].guest_name,"");
	strcpy(fmpks[ ti+2].host_name,"");
}

void NPC_ManorSmanLoop(int meindex)
{
	struct tm tm1;
	int fmpks_pos;
#ifdef _NEW_MANOR_LAW
	int iOffsetTime;
#endif

	if(CHAR_CHECKINDEX(meindex) == FALSE){
		printf("\nNPC_ManorSmanLoop error!(meindex:%d)",meindex);
		return;
	}

	fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));

  switch (fmpks[fmpks_pos+1].flag) {
  case FMPKS_FLAG_NONE:
#ifdef _NEW_MANOR_LAW
		{
			int hadfmindex,index,manorid;
			char token[256];
			//此时可以开始挑战
			manorid = CHAR_getWorkInt(meindex,NPC_WORK_MANORID);
			getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",5,token,sizeof(token));
			hadfmindex = atoi(token);
			if(hadfmindex != -1){
				for(index=0;index<FAMILY_MAXNUM;index++){
					// 找出守庄家族的索引
					if(fmdptop.fmtopid[index] == (hadfmindex - 1)) break;
				}
				if(index >= FAMILY_MAXNUM){
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",6,token,sizeof(token));
					printf("\nNPC_ManorSmanLoop():save fm_momentum error(%d:%s)",hadfmindex,token);
					break;
				}
				// 记录这个时刻守庄家族的气势
				else fmpointlist.fm_momentum[manorid-1] = fmdptop.fmMomentum[index];
				// 进入下一个状态
				fmpks[fmpks_pos+1].flag = FMPKS_FLAG_WAIT;
				// 挑战期共4小时(1800~2200)
				fmpks[fmpks_pos+1].dueltime = NowTime.tv_sec + 3600 * 4;
				NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_WAIT,fmpks[fmpks_pos+1].dueltime,tm1);
			}
			SortManorSchedule();
		}
#endif    
    break;
#ifdef _NEW_MANOR_LAW
	// 进入挑战期,已记录家族气势,等待挑战排程
	case FMPKS_FLAG_WAIT:
		{
			int manorid,i,iPlayerNum = CHAR_getPlayerMaxNum();
			char szMsg[256];

			manorid = CHAR_getWorkInt(meindex,NPC_WORK_MANORID);
			// 还没开打,气势改变要去向AC要即时资料
			fmpointlist.fm_inwar[manorid-1] = TRUE;
			if(fmpks[fmpks_pos+1].dueltime <= NowTime.tv_sec){
				// 挑战家族中气势最高的获得挑战权
				int manorindex = CHAR_getWorkInt(meindex,NPC_WORK_MANORID) - 1;
				int iNo1FmIndexSort = ManorSchedule[manorindex].iSort[0];
				int index = ManorSchedule[manorindex].iFmIndex[iNo1FmIndexSort],iFmIndex1,iCharindex;
				char token[256],fmname[256];
				// 没人挑战
				if(index < 0){
					// 直接进入到挑战结束
					fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_BATTLEEND;
					CHAR_setWorkInt(meindex,NPC_WORK_BETTLETIME,tm1.tm_mday);
					// 通知所有人
					for(i=0;i<iPlayerNum;i++){
						if(CHAR_getCharUse(i) != FALSE){
							CHAR_talkToCli(i,-1,"由於无家族挑战庄园，庄园进入休战时期",CHAR_COLORBLUE2);
						}
					}
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",5,token,sizeof(token));
					iFmIndex1 = atoi(token) - 1;
					if(iFmIndex1 != -1){
						getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",6,fmname,sizeof(token));
						saacproto_ACFixFMPoint_send(acfd,fmname,iFmIndex1 + 1,iFmIndex1,
							fmname,iFmIndex1 + 1,iFmIndex1,CHAR_getWorkInt(meindex,NPC_WORK_ID));
						// 增加庄园战胜负Log
						sprintf(token," (%d:%d) %d/%d/%d",tm1.tm_hour,tm1.tm_min,tm1.tm_year+1900,tm1.tm_mon+1,tm1.tm_mday);
						Logfmpk(fmname,iFmIndex1,-1,"无人挑战",-1,-1,token,"","",2);
						// 原家族守住了庄园,家族成员可得到石币
						for(i=0;i<FAMILY_MAXMEMBER;i++){
							iCharindex = familyMemberIndex[iFmIndex1][i];
							// 若在线上才给钱 
							if(iCharindex >= 0 && CHAR_getCharUse(iCharindex)){
								// 获得金钱 = 个人气势 * 5000
								int iAddGold = ((float)CHAR_getInt(iCharindex,CHAR_MOMENTUM)/100.0f) * 5000.0f;
								int iGold = CHAR_getInt(iCharindex,CHAR_BANKGOLD),iMaxGold;
								// 先放入个人银行
								if(iGold + iAddGold > CHAR_MAXBANKGOLDHAVE){
									CHAR_setInt(iCharindex,CHAR_BANKGOLD,CHAR_MAXBANKGOLDHAVE);
									// 个人银行放不下了,放到个人身上
									iAddGold = iGold + iAddGold - CHAR_MAXBANKGOLDHAVE;
									iGold = CHAR_getInt(iCharindex,CHAR_GOLD);
									iMaxGold = CHAR_getMaxHaveGold(iCharindex);
									if(iGold + iAddGold > iMaxGold) CHAR_setInt(iCharindex,CHAR_GOLD,iMaxGold);
									else CHAR_setInt(iCharindex,CHAR_GOLD,iGold + iAddGold);
									LogFMPKGetMomey(CHAR_getChar(iCharindex,CHAR_FMNAME),
																	CHAR_getChar(iCharindex,CHAR_CDKEY),
																	CHAR_getChar(iCharindex,CHAR_NAME),
																	CHAR_getInt(iCharindex,CHAR_MOMENTUM),iAddGold,0);
								}
								else{
									CHAR_setInt(iCharindex,CHAR_BANKGOLD,iGold + iAddGold);
									LogFMPKGetMomey(CHAR_getChar(iCharindex,CHAR_FMNAME),
																	CHAR_getChar(iCharindex,CHAR_CDKEY),
																	CHAR_getChar(iCharindex,CHAR_NAME),
																	CHAR_getInt(iCharindex,CHAR_MOMENTUM),iAddGold,1);
								}
								CHAR_talkToCli(iCharindex,-1,"辛苦了!守护住庄园的奖金已汇入你的个人银行",CHAR_COLORRED);
								// 家族的个人及家族气势都要归零
								CHAR_setInt(iCharindex,CHAR_MOMENTUM,0);
								CHAR_talkToCli(iCharindex,-1,"庄园战後个人及家族气势归零",CHAR_COLORRED);
							}
						}
					}
				}
				else{
					// 挑战时间已过,选出的挑战家族和守庄家族有约一天的时间可以准备
					NPC_ManorSavePKSchedule(meindex,index,0,-1,ManorSchedule[manorindex].tm1[iNo1FmIndexSort]);
					// 开打了,气势改变也不用去向AC要即时资料了
					fmpointlist.fm_inwar[manorid-1] = FALSE;
					// 通知所有人
					sprintf(szMsg,"%s 家族获得挑战庄园资格",ManorSchedule[manorindex].szFmName[iNo1FmIndexSort]);
					
					for(i=0;i<iPlayerNum;i++){
						if(CHAR_getCharUse(i) != FALSE){
							CHAR_talkToCli(i,-1,szMsg,CHAR_COLORBLUE2);
						}
					}
				}
			}
		}
		break;
#endif
  case FMPKS_FLAG_MANOR_PREPARE:
    if(fmpks[fmpks_pos+1].dueltime <= NowTime.tv_sec){
      memcpy(&fmpks[fmpks_pos], &fmpks[fmpks_pos+1], sizeof(FamilyPKSchedule));
      fmpks[fmpks_pos].dueltime = tm1.tm_hour * 100 + tm1.tm_min;
      fmpks[fmpks_pos].flag = FMPKS_FLAG_SCHEDULED;
      fmpks[fmpks_pos+1].dueltime = 0;
      fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_BATTLEBEGIN;
      NPC_talkToFloor(CHAR_getInt(meindex, CHAR_FLOOR) , fmpks[fmpks_pos].host_index,
         	fmpks[fmpks_pos].guest_index, "庄园争夺战已经开始，请尽快入场！");
#ifdef _NEW_MANOR_LAW
			CHAR_setWorkInt(meindex,NPC_WORK_BETTLETIME,tm1.tm_mday);
#endif
    }
    break;
  case FMPKS_FLAG_MANOR_PEACE:
    if (fmpks[fmpks_pos+1].dueltime <= NowTime.tv_sec) {
      fmpks[fmpks_pos+1].dueltime = 0;
      fmpks[fmpks_pos+1].flag = FMPKS_FLAG_NONE;
#ifndef _NEW_MANOR_LAW
      NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_NONE);
#else
			NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_NONE,fmpks[fmpks_pos+1].dueltime,tm1);
#endif
    }
    break;
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    break;
  case FMPKS_FLAG_MANOR_BATTLEEND:
		{
#ifdef _NEW_MANOR_LAW
			int manorindex = CHAR_getWorkInt(meindex,NPC_WORK_MANORID) - 1;
#endif

			fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
#ifndef _NEW_MANOR_LAW
			fmpks[fmpks_pos+1].dueltime = CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
#else
			// 打完後要再过五天才能再挑战
			// 若开打的日期和结束战斗的日期不一样表示打超过了一天以上,以一天计
			if(CHAR_getWorkInt(meindex,NPC_WORK_BETTLETIME) != tm1.tm_mday){
				iOffsetTime = tm1.tm_hour * 3600 * -1;	// 扣回00:00
			}
			else{	// 战斗结束时未超过一天
				iOffsetTime = (24 - tm1.tm_hour) * 3600;	// 补成00:00
			}
			//#ifdef _75_TEST
			fmpks[fmpks_pos+1].dueltime = NowTime.tv_sec - tm1.tm_min*60 + iOffsetTime + 172800 + (18 * 3600);	// 二天
			//#else
			//	fmpks[fmpks_pos+1].dueltime = NowTime.tv_sec - tm1.tm_min*60 + iOffsetTime + 432000 + (18 * 3600);
			//														|-----------取整点------------| |-补成00:00-| |-五天-|  |18:00开始报名|
			//#endif
#endif
			fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_PEACE_SAVE;
			NPC_CleanPkList( fmpks_pos);
#ifndef _NEW_MANOR_LAW
			NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_MANOR_PEACE_SAVE);
#else
			NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_MANOR_PEACE_SAVE,fmpks[fmpks_pos+1].dueltime,tm1);
			// 清空排程
			memset(&ManorSchedule[manorindex],0,sizeof(ManorSchedule[manorindex]));
			{
				int i;
				for(i=0;i<10;i++) ManorSchedule[manorindex].iSort[i] = ManorSchedule[manorindex].iFmIndex[i] = -1;
				for(i=0;i<FAMILY_MAXHOME;i++) fmpointlist.fm_momentum[i] = -1;
			}
		}
#endif
    break;
  case FMPKS_FLAG_MANOR_PEACE_SAVE:
    fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_PEACE;
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    break;
  case FMPKS_FLAG_MANOR_READYTOFIGHT:
  case FMPKS_FLAG_MANOR_CLEANFLAG:
    fmpks[fmpks_pos].flag = FMPKS_FLAG_NONE;
    fmpks[fmpks_pos+1].dueltime = CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
    fmpks[fmpks_pos+1].flag = FMPKS_FLAG_NONE;

		NPC_CleanPkList( fmpks_pos);
#ifndef _NEW_MANOR_LAW
    NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_NONE);
#else
		NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_NONE,fmpks[fmpks_pos+1].dueltime,tm1);
#endif
    break;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_PREVLOOPTIME, NowTime.tv_sec);
}

void NPC_ManorLoadPKSchedule(int meindex)
{
#ifdef _ACFMPK_LIST
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID);
	saacproto_ACLoadFmPk_send(acfd, fmpks_pos);
#else
	char filename[256], tmp[4096], token[256];
	FILE *f;
	int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

	snprintf(filename,sizeof(filename), "%s%d_%d_%d",
		       SCHEDULEFILEDIR, CHAR_getInt(meindex, CHAR_FLOOR),
			   CHAR_getInt(meindex, CHAR_X),
			CHAR_getInt(meindex, CHAR_Y) );

	if( ! (f=fopen( filename, "r" )) ){	// create new schedule file
		f = fopen( filename, "w" );
		if( !f ){
			print( "ERROR:Can't create Schedule file %s!\n",filename );
			return;
		}
		strcpy(tmp,"0|-1|-1|-1|-1|-1|-1|-1|-1\n");
		fwrite(tmp, strlen(tmp), 1, f);
	}

	fseek(f, 0, SEEK_SET);	// 档案开头
	fgets(tmp, sizeof(tmp), f);
	fmpks[fmpks_pos].flag=-1;

	// 时间
	if (getStringFromIndexWithDelim(tmp,"|",1,token,sizeof(token))) {
		fmpks[fmpks_pos+1].dueltime=atoi(token);
	}
	// 主队 familyindex
	if (getStringFromIndexWithDelim(tmp,"|",2,token,sizeof(token))) {
	    fmpks[fmpks_pos+1].host_index=atoi(token);
	}
	// 主队 家族名
	if (getStringFromIndexWithDelim(tmp,"|",3,token,sizeof(token))) {
		strcpy(fmpks[fmpks_pos+1].host_name,makeStringFromEscaped(token));
	}
	// 客队 familyindex
	if (getStringFromIndexWithDelim(tmp,"|",4,token,sizeof(token))) {
	    fmpks[fmpks_pos+1].guest_index=atoi(token);
	}
	// 客队 家族名
	if (getStringFromIndexWithDelim(tmp,"|",5,token,sizeof(token))) {
	    strcpy(fmpks[fmpks_pos+1].guest_name,makeStringFromEscaped(token));
	}
	// 准备时间
	if (getStringFromIndexWithDelim(tmp,"|",6,token,sizeof(token))) {
		fmpks[fmpks_pos+1].prepare_time=atoi(token);
	}
	// 最大人数
	if (getStringFromIndexWithDelim(tmp,"|",7,token,sizeof(token))) {
		fmpks[fmpks_pos+1].max_player=atoi(token);
	}
	// 旗标
	if (getStringFromIndexWithDelim(tmp,"|",8,token,sizeof(token))) {
		fmpks[fmpks_pos+1].flag=atoi(token);
	}
	// 对战星球
	if (getStringFromIndexWithDelim(tmp,"|",9,token,sizeof(token))) {
		strcpy(fmpks[fmpks_pos+2].host_name,makeStringFromEscaped(token));
	}
  
	// no schedule
	fmpks[fmpks_pos].flag=-1;
	// prepare time expired?
	if ((fmpks[fmpks_pos+1].flag==FMPKS_FLAG_MANOR_PREPARE) &&
		  (fmpks[fmpks_pos+1].dueltime<NowTime.tv_sec)) {
		fmpks[fmpks_pos].flag=-1;
// Terry fix 要清为-1不能清为0
//		fmpks[fmpks_pos].host_index=0;
		fmpks[fmpks_pos].host_index=-1;
		strcpy(fmpks[fmpks_pos].host_name,"");
// Terry fix 要清为-1不能清为0
//		fmpks[fmpks_pos].guest_index=0;
		fmpks[fmpks_pos].guest_index=-1;
		strcpy(fmpks[fmpks_pos].guest_name,"");
		fmpks[fmpks_pos+1].flag=-1;
		fmpks[fmpks_pos+1].dueltime=0;
	//    fmpks[fmpks_pos+1].dueltime=NowTime.tv_sec+30*60;	// add more 30 minutes
	}
	fclose(f);
#endif // end AC_SEND_FM_PK

}

#ifdef _ACFMPK_LIST
#ifndef _NEW_MANOR_LAW
void NPC_ManorSavePKSchedule(int meindex, int toindex, int flg)
#else
void NPC_ManorSavePKSchedule(int meindex, int toindex, int flg,int setTime,struct tm tm2)
#endif
{
	int fmpks_pos, manorid, dueltime;
	char msg[1024], n1[256], n2[256], n3[256];
	char token[256], hadfmname[256];
	int hadfmindex=0, hadfmpopular=0, tkfmindex=0;
	int PkFlg=0;

	if( toindex == -1 ){
#ifndef _NEW_MANOR_LAW
		dueltime = CHAR_getWorkInt(meindex, NPC_WORK_PEACEWAIT) + NowTime.tv_sec;
		sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s",
			dueltime, 0, "", 0, "", 0, 0, flg, "");
#else
		sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s",
			setTime, 0, "", 0, "", 0, 0, flg, "");
#endif
		PkFlg = 0;
	}else{
		struct tm tm1;
		int index;
#ifndef _NEW_MANOR_LAW
		tkfmindex = CHAR_getWorkInt(toindex, CHAR_WORKFMINDEXI);
#else
		// toindex 变成 家族的index
		tkfmindex = toindex;
#endif
		fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
		manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
		memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
#ifndef _NEW_MANOR_LAW
		dueltime = CHAR_getWorkInt(meindex, NPC_WORK_CHALLENGEWAIT) + NowTime.tv_sec - tm1.tm_min*60 + 1800;
#else
		// 以挑战时的时间为开打的时间,中间所经过的时间为准备时间
		dueltime = NowTime.tv_sec - tm1.tm_min * 60 + ((24 - tm1.tm_hour) * 3600) + 3600 * tm2.tm_hour;
		//				|-------------取整点-------------|  |--------加到0000时-------|  |挑战时的时间(算整点)| 
#endif
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[manorid-1], "|", 5, token, sizeof(token));
		hadfmindex = atoi( token);
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[manorid-1], "|", 7, token, sizeof(token));
		hadfmpopular = atoi( token);
		getStringFromIndexWithDelim( fmpointlist.pointlistarray[manorid-1], "|", 6, token, sizeof(token));
		strcpy( hadfmname, token);
		makeEscapeString( hadfmname, n1, sizeof(n1));
#ifndef _NEW_MANOR_LAW
		makeEscapeString( CHAR_getChar(toindex, CHAR_FMNAME), n2, sizeof(n2));		
#else
		// 找出挑战家族中气势最高家族索引
		index = ManorSchedule[manorid-1].iSort[0];
		makeEscapeString(ManorSchedule[manorid-1].szFmName[index],n2,sizeof(n2));
#endif
		makeEscapeString( getGameserverID(), n3, sizeof(n3));

		sprintf(msg, "%d|%d|%s|%d|%s|%d|%d|%d|%s",
			dueltime, hadfmindex-1, n1, tkfmindex, n2, 15, 50, FMPKS_FLAG_MANOR_OTHERPLANET, n3);
		PkFlg = 1;
		{
			char buf1[256];
			sprintf( buf1,"%d", CHAR_getWorkInt( meindex, NPC_WORK_ID) );
			Logfmpk(
				 n1, hadfmindex-1, 0,
				 n2, tkfmindex, 0,
				 "", buf1, n3, 1);
		}
	}
#ifndef _NEW_MANOR_LAW
	saacproto_ACSendFmPk_send( acfd, toindex, PkFlg, CHAR_getWorkInt( meindex, NPC_WORK_ID), msg);
#else
	// toindex 用不到设为 -1
	saacproto_ACSendFmPk_send( acfd, -1, PkFlg, CHAR_getWorkInt( meindex, NPC_WORK_ID), msg);
#endif
}

#endif

#ifdef _NEW_MANOR_LAW
// 加入挑战排程
void NPC_ManorAddToSchedule(int meindex,int charaindex)
{
	int i,j,iEmpty = -1,iFmIndex,manorindex = CHAR_getWorkInt(meindex,NPC_WORK_MANORID) - 1;
	struct tm tm1;

	iFmIndex = CHAR_getWorkInt(charaindex,CHAR_WORKFMINDEXI);
	// 检查是否已排入排程,检查所有的庄园约战
	for(j=0;j<MANORNUM;j++){
		for(i=0;i<10;i++){
			if(ManorSchedule[j].iFmIndex[i] != -1){
				if(ManorSchedule[j].iFmIndex[i] == iFmIndex){
					lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
						CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"你已经报名过挑战庄园了喔～");
					return;
				}
			}
			else{
				if(j == manorindex) iEmpty = i;	// 记录空的位置
			}
		}
	}
	// 如果程式执行到这里,表示没有加入排程,检查有没有空的位置
	if(iEmpty != -1){ // 有空位置,把报名者加入
		// 记录家族索引
		ManorSchedule[manorindex].iFmIndex[iEmpty] = iFmIndex;
		// 记录家族名称
		sprintf(ManorSchedule[manorindex].szFmName[iEmpty],"%s",CHAR_getChar(charaindex,CHAR_FMNAME));
		// 记录挑战时间
		memcpy(&tm1,localtime((time_t *)&NowTime.tv_sec),sizeof(tm1));
		memset(ManorSchedule[manorindex].szMemo[iEmpty],0,sizeof(ManorSchedule[manorindex].szMemo[iEmpty]));
		memcpy(&ManorSchedule[manorindex].tm1[iEmpty],&tm1,sizeof(tm1));
		lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"加入挑战排程");
	}
	// 满了,和挑战排程的最後一名作比较,如果後来的气势比最後一名高,替换掉最後一名,否则通知目前气势无法挤进排程
	else{
		// 取得最後一名在ManorSchedule的位置
		int index = ManorSchedule[manorindex].iSort[9];

		for(i=0;i<FAMILY_MAXNUM;i++){
			// 找出挑战家族的索引
			if(fmdptop.fmtopid[i] == iFmIndex) break;
		}
		// 找不到此家族
		if(i >= FAMILY_MAXNUM){
			lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"抱歉，找不到你的家族气势资料");
			return;
		}
		// 比较气势
		if(ManorSchedule[manorindex].iFmMomentum[index] < fmdptop.fmMomentum[i]){
			int fmpks_pos,iPlayerNum = CHAR_getPlayerMaxNum();
			char szMsg[256];

			// 通知所有人
			fmpks_pos = CHAR_getWorkInt(meindex,NPC_WORK_ID) * MAX_SCHEDULE;
			sprintf(szMsg,"%s 家族被 %s 家族挤下挑战排程",ManorSchedule[manorindex].szFmName[index],CHAR_getChar(charaindex,CHAR_FMNAME));
			for(i=0;i<iPlayerNum;i++){
				if(CHAR_getCharUse(i) != FALSE){
					CHAR_talkToCli(i,-1,szMsg,CHAR_COLORBLUE2);
				}
			}
			// 最後一名被挤掉
			ManorSchedule[manorindex].iFmIndex[index] = iFmIndex;
			// 记录家族名称
			sprintf(ManorSchedule[manorindex].szFmName[index],"%s",CHAR_getChar(charaindex,CHAR_FMNAME));
			// 记录挑战时间
			memcpy(&tm1,localtime((time_t *)&NowTime.tv_sec),sizeof(tm1));
			memcpy(&ManorSchedule[manorindex].tm1[index],&tm1,sizeof(tm1));
			lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"加入挑战排程");
		}
		else{
			lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"抱歉，你的家族气势不足以排进挑战排程");
			return;
		}
	}
	// 排序
	SortManorSchedule();
}

// 排序挑战排程
void SortManorSchedule()
{
	int i,j,k;

	// 先取得最新家族气势
	for(k=0;k<MANORNUM;k++){
		for(i=0;i<10;i++){
			ManorSchedule[k].iSort[i] = i;
			if(ManorSchedule[k].iFmIndex[i] != -1){
				for(j=0;j<FAMILY_MAXNUM;j++){
					// 找出挑战家族的索引
					if(fmdptop.fmtopid[j] == ManorSchedule[k].iFmIndex[i]){
						// 取得气势值
						ManorSchedule[k].iFmMomentum[i] = fmdptop.fmMomentum[j];
						// 作成memo
						sprintf(ManorSchedule[k].szMemo[i],"%-16s %2d:%2d %12d",
							ManorSchedule[k].szFmName[i],ManorSchedule[k].tm1[i].tm_hour,ManorSchedule[k].tm1[i].tm_min,
							ManorSchedule[k].iFmMomentum[i]/100);
						break;
					}
				}
			}
		}
	}
	// 排序
	for(i=0;i<MANORNUM;i++){
		g_iSortManor = i;
		qsort(&ManorSchedule[g_iSortManor].iSort,10,sizeof(int),&SortManorScheduleMomentum);
	}
}

int SortManorScheduleMomentum(const void *indexa, const void *indexb)
{
  int momentumA,momentumB;
  
  momentumA = ManorSchedule[g_iSortManor].iFmMomentum[*(int*)indexa];
  momentumB = ManorSchedule[g_iSortManor].iFmMomentum[*(int*)indexb];
  return (momentumA > momentumB) ? -1:1;
}
#endif
