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
 * 蚽埶 PK 腎暮埜
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
	NPC_WORK_ID = CHAR_NPCWORKINT1,		// 腎暮埜 ID, 植 0 羲宎
	NPC_WORK_MANORID = CHAR_NPCWORKINT2,	// 蚽埶晤瘍
	NPC_WORK_CHALLENGEWAIT = CHAR_NPCWORKINT3,	// 泔桵脹渾奀潔
	NPC_WORK_PEACEWAIT = CHAR_NPCWORKINT4,	// 倎桵奀潔
	NPC_WORK_PREVLOOPTIME = CHAR_NPCWORKINT5,	// 珨棒揭燴 Loop 腔奀潔
#ifdef _NEW_MANOR_LAW
	NPC_WORK_BETTLETIME = CHAR_NPCWORKINT6	// 暮翹羲湖奀腔
#endif
};

#define SCHEDULEFILEDIR		"./Schedule/"

#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 蔚褫泔桵蚽埶腔蚕坋湮蜊峈媼坋湮
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

  // 統杅
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
    // 羶衄埮隅桵須ㄛ森奀褫眕狟桵抎
    saacproto_ACFMPointList_send(acfd);
#ifdef _MANOR_PKRULE
	sprintf(buf, "蚽埶垀衄淰嗤桵腔泔桵訧跡\n\n"
                 "珨﹜羶衄茧衄蚽埶腔模逜\n"
	#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 蔚褫泔桵蚽埶腔蚕坋湮蜊峈媼坋湮
				 "媼﹜模逜齬俴斛剒峈媼坋湮模逜\n"
	#else
                 "媼﹜模逜齬俴斛剒峈坋湮模逜\n"
	#endif
				 "﹜杺奩奀潔ㄩ狟敁鞠萸祫錘鹵萸\n\n"
                 "尕渾ㄛ扂蔚斕腔訧跡﹝");
#else
    sprintf(buf, "蚽埶垀衄淰嗤桵腔泔桵訧跡\n\n"
                 "珨﹜羶衄茧衄蚽埶腔模逜\n"
                 "媼﹜模逜腔汒咡詢黺蚽埶模逜腔汒咡\n\n"
                 "尕渾ㄛ扂蔚斕腔訧跡﹝");
#endif
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	    		WINDOW_BUTTONTYPE_YESNO,
    			CHAR_WINDOWTYPE_CHECKMAN_START,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  case FMPKS_FLAG_MANOR_PREPARE:
    // 醴眒冪隅疑桵須ㄛ袧掘笢
    {
      int timeleft=fmpks[fmpks_pos+1].dueltime-NowTime.tv_sec;
      int dd,hh,mm;
      char buf2[4096];
      dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;
      hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd*24;
      mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd*24*60 - hh*60;
      memset(buf2,0,sizeof(buf2));
      if (dd>0) sprintf(buf, " %d 毞", dd); else strcpy(buf, "");
      strcat(buf2, buf);
      if (hh>0) sprintf(buf, " %d 苤奀", hh); else strcpy(buf, "");
      strcat(buf2, buf);
      if (mm>0) sprintf(buf, " %d 煦瀎", mm); else strcpy(buf, "");
      strcat(buf2, buf);
      if (strlen(buf2)==0)
        sprintf(buf, "蚽埶垀衄淰嗤桵\n◇%s 