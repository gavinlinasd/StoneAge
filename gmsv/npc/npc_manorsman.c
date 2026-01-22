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
	NPC_WORK_PREVLOOPTIME = CHAR_NPCWORKINT5,	// ヶ珨棒揭燴 Loop 腔奀潔
#ifdef _NEW_MANOR_LAW
	NPC_WORK_BETTLETIME = CHAR_NPCWORKINT6	// 暮翹羲湖奀腔�梪�
#endif
};

#define SCHEDULEFILEDIR		"./Schedule/"

#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 蔚褫泔桵蚽埶腔蚕ヶ坋湮蜊峈ヶ媼坋湮
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
	sprintf(buf, "蚽埶垀衄�佰鬤慖蔥騷譭論妐鎪n\n"
                 "珨﹜羶衄茧衄蚽埶腔模逜\n"
	#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 蔚褫泔桵蚽埶腔蚕ヶ坋湮蜊峈ヶ媼坋湮
				 "媼﹜模逜齬俴斛剒峈ヶ媼坋湮模逜\n"
	#else
                 "媼﹜模逜齬俴斛剒峈ヶ坋湮模逜\n"
	#endif
				 "��﹜杺奩奀潔ㄩ狟敁鞠萸祫錘鹵��萸\n\n"
                 "③尕渾ㄛ扂蔚�溜狦蒫儷妐鞢�");
#else
    sprintf(buf, "蚽埶垀衄�佰鬤慖蔥騷譭論妐鎪n\n"
                 "珨﹜羶衄茧衄蚽埶腔模逜\n"
                 "媼﹜模逜腔汒咡詢黺蚽埶模逜腔汒咡\n\n"
                 "③尕渾ㄛ扂蔚�溜狦蒫儷妐鞢�");
#endif
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	    		WINDOW_BUTTONTYPE_YESNO,
    			CHAR_WINDOWTYPE_CHECKMAN_START,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  case FMPKS_FLAG_MANOR_PREPARE:
    // 醴ヶ眒冪隅疑桵須ㄛ袧掘笢
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
        sprintf(buf, "蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n"
                    "撈蔚羲宎ㄛ③袧掘�貐﹛�",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name);
      else
        sprintf(buf, "蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n"
                    "啎隅蔚婓%s摽羲宎﹝",
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
    // 桵須眒冪賦旰腔睿す奀ぶ
    {
      int timeleft=fmpks[fmpks_pos+1].dueltime-NowTime.tv_sec;
      int dd,hh,mm;
      char buf2[4096];
      dd = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 86400;
      hh = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 3600 - dd*24;
      mm = /*fmpks[fmpks_pos+1].dueltime*/ timeleft / 60 - dd*24*60 - hh*60;
      strcpy(buf2,"");
      if (dd>0) sprintf(buf, " %d 毞", dd); else strcpy(buf, "");
      strcat(buf2, buf);
      if (hh>0) sprintf(buf, " %d 苤奀", hh); else strcpy(buf, "");
      strcat(buf2, buf);
      if (mm>0) sprintf(buf, " %d 煦瀎", mm); else strcpy(buf, "");
      strcat(buf2, buf);

      if (strlen(buf2)==0)
        strcpy(buf, "③尕脹ㄛ�襞眢摹詫銨薺糒輲葙搧覺竀鞢�");
      else
        sprintf(buf, "珋婓岆倎桵奀ぶㄛ猁杺奩腔趕\n③%s摽婬懂扠③﹝",buf2);
      lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX), buf);
    }
    break;
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    // 醴ヶ淏婓輛俴杺奩
    sprintf(buf, "蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n"
                 "羲‵湖‵蹕‵\n遜羶輛部腔�佶狤儠瓥※氶�",
    		fmpks[fmpks_pos+1].guest_name,
    		fmpks[fmpks_pos+1].host_name);
    lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			CHAR_WINDOWTYPE_CHECKMAN_END,
    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			buf);
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    // 婓梗腔陎⑩輛俴桵須
    sprintf(buf, "蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n"
    		 "樵須華萸婓 %s ﹝",
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
	// 泔桵ぶ 1800~2200
  case FMPKS_FLAG_NONE:
	case FMPKS_FLAG_WAIT:
    saacproto_ACFMPointList_send(acfd);
		sprintf(buf,"蚽埶垀衄�佰鬤慖蔥騷譭論妐鎪n\n"
								"珨﹜羶衄蚽埶腔模逜\n"
								"媼﹜模逜腔ァ岊詢黺蚽埶模逜腔ァ岊\n"
								"��﹜埮桵奀潔笝砦奀ㄛァ岊齬靡菴珨氪鳳腕泔桵訧跡\n"
								"侐﹜ァ岊眈肮氪眕郔珂惆靡氪鳳腕訧跡\n"
								"偌 OK 扂蔚�溜狦蒫儷妐韗為� NO 夤艘泔桵齬靡");
    lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_YESNO,CHAR_WINDOWTYPE_CHECKMAN_START,
    								 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),buf);
		if(fmpointlist.fm_momentum[manorid-1] <= -1){
			int hadfmindex,index;
			char token[256];

			getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",5,token,sizeof(token));
			hadfmindex = atoi(token);
			if(hadfmindex != -1){
				for(index=0;index<FAMILY_MAXNUM;index++){
					// 梑堤忐蚽模逜腔坰竘
					if(fmdptop.fmtopid[index] == (hadfmindex - 1)) break;
				}
				if(index >= FAMILY_MAXNUM){
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",6,token,sizeof(token));
					printf("\nNPC_ManorSmanTalked():save fm_momentum error(%d:%s)",hadfmindex,token);
					break;
				}
				// 暮翹涴跺奀覦忐蚽模逜腔ァ岊
				else fmpointlist.fm_momentum[manorid-1] = fmdptop.fmMomentum[index];
			}
		}
		SortManorSchedule();
    break;
	// 泔桵袧掘ぶ 2200~�△襟譭論妐騊躁眢撋捩捺譭蔥騫掉�(路毞腔 1800~2200 眳潔)
  case FMPKS_FLAG_MANOR_PREPARE:
    // 醴ヶ眒冪隅疑桵須ㄛ袧掘笢
    {
      int timeleft = fmpks[fmpks_pos+1].dueltime - NowTime.tv_sec;
      int mm,hh,dd;
      char buf2[4096];

			dd = timeleft / 86400;
			hh = timeleft / 3600 - dd * 24;
      mm = timeleft / 60 - dd * 24 * 60 - hh * 60;
      memset(buf2,0,sizeof(buf2));
			if(dd > 0) sprintf(buf," %d 毞",dd); else strcpy(buf,"");
      strcat(buf2,buf);
			if(hh > 0) sprintf(buf," %d 苤奀",hh); else strcpy(buf,"");
      strcat(buf2,buf);
      if(mm > 0) sprintf(buf," %d 煦瀎",mm); else strcpy(buf,"");
      strcat(buf2,buf);
      if(strlen(buf2) == 0)
        sprintf(buf,"蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n撈蔚羲宎ㄛ③袧掘�貐﹛�",
    						fmpks[fmpks_pos+1].guest_name,fmpks[fmpks_pos+1].host_name);
      else
				sprintf(buf,"蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n啎隅蔚婓%s摽羲宎﹝",
    						fmpks[fmpks_pos+1].guest_name,fmpks[fmpks_pos+1].host_name,buf2);
      lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
    									 CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX),buf);
    }
    break;
	// 倎桵ぶ摯袧掘ぶ,拻毞
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
      if(dd > 0) sprintf(buf," %d 毞",dd); else strcpy(buf,"");
      strcat(buf2,buf);
      if(hh > 0) sprintf(buf," %d 苤奀",hh); else strcpy(buf,"");
      strcat(buf2,buf);
      if(mm > 0) sprintf(buf," %d 煦瀎",mm); else strcpy(buf,"");
      strcat(buf2,buf);

      if(strlen(buf2) == 0) strcpy(buf, "③尕脹ㄛ�襞眢摹詫銨薺糒輲葙搧覺竀鞢�");
      else sprintf(buf, "珋婓準泔桵奀ぶㄛ猁泔桵腔趕\n③%s摽婬懂扠③﹝",buf2);
      lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
											 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),buf);
    }
    break;
	// 泔桵笢
  case FMPKS_FLAG_MANOR_BATTLEBEGIN:
    sprintf(buf,"蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n羲‵湖‵蹕‵\n遜羶輛部腔�佶狤儠瓥※氶�",
    				fmpks[fmpks_pos+1].guest_name,fmpks[fmpks_pos+1].host_name);
    lssproto_WN_send(fd,WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
    								 CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),buf);
    break;
  case FMPKS_FLAG_MANOR_OTHERPLANET:
    // 婓梗腔陎⑩輛俴桵須
    sprintf(buf,"蚽埶垀衄�佰鬤慖穀n◇%s �痑� %s◆\n\n樵須華萸婓 %s ﹝",
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
		sprintf(buf, "蚽埶泔桵頗衄珨毞腔袧掘ぶ\n"
				"斕褫眕呴奀戙恀扂眕腕眭呁狟腔奀潔\n"
				"泔桵賦旰摽祥蹦吨蛹蔚頗衄媼毞腔倎桵ぶ\n"
				"倎桵ぶ潔祥夔泔桵\n\n"
				"斕�毓例者譭調漈囆耽兜艞�");
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
            // Arminius 2.25 fix: fmpks 笢菴 1~"MANORNUM" 郪珨隅猁跤 manorsman
            for (i=0; i<=/*3*/MANORNUM-1; i++) {	// 9跺蚽埶
              getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, token, sizeof(token));
              if (tkfmindex==atoi(token)-1) check=1;
            }
            for (i=1; i<=/*4*/MANORNUM; i++) {	// 岆瘁眒冪泔桵む坻蚽埶
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
								// 梑堤泔桵模逜腔坰竘
								if(fmdptop.fmtopid[index] == tkfmindex) break;
							}
							if(index >= FAMILY_MAXNUM){
								printf("\nNPC_ManorSmanWindowTalked():find tkfmIndex error (%d)",tkfmindex);
							}
							else
							// �蝜�泔桵模逜ァ岊祥逋忐蚽模逜ァ岊腔嬝傖,祥夔泔桵
							if(fmdptop.fmMomentum[index] < fmpointlist.fm_momentum[manorid-1]*0.9) won1 = 0;
		//-----------------------------------------
	#else
		//-----------------------------------------
							int i;
							char won2[256];
							won1 = 0;
		#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 蔚褫泔桵蚽埶腔蚕ヶ坋湮蜊峈ヶ媼坋湮		
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
							sprintf(buf,"�毓侈耽匏譭論妐鼁廙慪倷鉻黖儷摹裘掉踊n"
													"斕褫眕呴奀戙恀扂眕腕眭呁狟腔奀潔\n"
													"泔桵賦旰摽祫狟棒泔桵ぶ蔚頗衄珨毞腔倎桵ぶ\n"
													"睿侐毞腔袧掘ぶㄛ婓涴拻毞ぶ潔囀祥夔泔桵\n\n"
													"斕�毓例者譭調漈囆耽兜艞�");
	#else
							sprintf(buf,"蚽埶泔桵頗衄珨毞腔袧掘ぶ\n"
													"斕褫眕呴奀戙恀扂眕腕眭呁狟腔奀潔\n"
													"泔桵賦旰摽祥蹦吨蛹蔚頗衄媼毞腔倎桵ぶ\n"
													"倎桵ぶ潔祥夔泔桵\n\n"
													"斕�毓例者譭調漈囆耽兜艞�");
	#endif
#else
              if(tkfmdp >= hadfmpopular){
								sprintf(buf,"蚽埶泔桵頗衄珨毞腔袧掘ぶ\n"
                            "斕褫眕呴奀戙恀扂眕腕眭呁狟腔奀潔\n"
												    "泔桵賦旰摽祥蹦吨蛹蔚頗衄拻毞腔倎桵ぶ\n"
														"倎桵ぶ潔祥夔泔桵\n\n"
														"斕�毓例者譭調漈囆耽兜艞�");
#endif
                lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    							WINDOW_BUTTONTYPE_YESNO,
    															CHAR_WINDOWTYPE_CHECKMAN_MAIN,
    															CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    															buf);
    					}
							else{
#ifdef _NEW_MANOR_LAW
								sprintf(buf, "斕腔模逜ァ岊奾帤善湛蚽埶模逜ァ岊腔嬝傖ㄛ③婬樓蚐﹝");
#else
	#ifdef _MANOR_PKRULE
		#ifdef _FIX_FAMILY_PK_LIMIT	   // WON ADD 蔚褫泔桵蚽埶腔蚕ヶ坋湮蜊峈ヶ媼坋湮
                sprintf(buf, "斕腔模逜汒咡奾帤輛�踿偉�坋湮模逜ㄛ③婬樓蚐﹝");
		#else
                sprintf(buf, "斕腔模逜汒咡奾帤輛�踿動捎騧眢憯甭郺椇衶矷�");
		#endif
	#else
                sprintf(buf, "斕腔模逜汒咡奾祥逋眕泔桵\n"
                             "茧衄涴跺蚽埶腔模逜ㄛ③婬樓蚐﹝");
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
              sprintf(buf, "斕腔模逜淏婓泔桵む坻蚽埶ㄛ\n"
                           "③參儂頗隱跤む坻模逜勘ㄐ");
              lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
													    	WINDOW_BUTTONTYPE_OK,
											    			CHAR_WINDOWTYPE_CHECKMAN_END,
											    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
											    			buf);
            }
						else{
              sprintf(buf, "珨跺模逜硐夔茧衄珨跺蚽埶ㄛ\n"
                           "�蝜�蚽埶模逜眳潔剒猁з渲\n"
                           "③善模逜�苺佼﹛�");
              lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    						WINDOW_BUTTONTYPE_OK,
											    			CHAR_WINDOWTYPE_CHECKMAN_END,
											    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
										    				buf);
            }
          }
					else{
            sprintf(buf, "涴跺蚽埶眒冪岆斕腔模逜腔鉊﹝");
            lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              						  	WINDOW_BUTTONTYPE_OK,
									    				CHAR_WINDOWTYPE_CHECKMAN_END,
											   			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
										    			buf);
          }
        }
				else{
          sprintf(buf, "珋婓甜羶衄蚽埶模逜ㄛ\n"
                       "眻諉�玉糒踿亞踾芨奿埡剆插�");
          lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			              	    	WINDOW_BUTTONTYPE_OK,
    												CHAR_WINDOWTYPE_CHECKMAN_END,
									    			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
									    			buf);
        }
      }
			else{
        sprintf(buf, "硐衄逜酗褫眕狟桵抎鉊﹝");
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

			sprintf(buf,"              蚽埶泔桵齬靡桶\n靡棒 模逜靡備         奀潔          ァ岊\n");
			manorindex = CHAR_getWorkInt(meindex,NPC_WORK_MANORID) - 1;
			// 冞堤泔桵齬靡
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
		sprintf( buf, "蚽埶泔桵腎翻�溜珋苺甭輶婛寣�");
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
			  sprintf(buf, "③黺狟敁ㄥㄩㄟㄟ祫錘鹵ㄢㄩㄟㄟ婬懂埮桵勘ㄐ");
			  lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
              	    	WINDOW_BUTTONTYPE_OK,
    			        CHAR_WINDOWTYPE_CHECKMAN_END,
    			        CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
    			        buf);
			  break;
		  }		  
	#endif
			// WON ADD 党淏模逜pk部腔埮桵恀枙
			if( fmpks[fmpks_pos+1].flag != FMPKS_FLAG_NONE ){
			     sprintf(buf, "涴跺蚽埶眒冪衄�刱槳蝌咈腹�");
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
		sprintf( buf, "蚽埶泔桵腎翻�溜珋苺甭輶婛寣�");
		CHAR_talkToCli( talkerindex, meindex, buf, CHAR_COLORYELLOW);
	#else
		NPC_ManorAddToSchedule(meindex,talkerindex);
	#endif
#else
	    memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
        getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1], "|", 6, token, sizeof(token));
        strcpy(hadfmname, token);

        fmpks[fmpks_pos+1].dueltime = CHAR_getWorkInt(meindex, NPC_WORK_CHALLENGEWAIT) + NowTime.tv_sec
                                      - tm1.tm_min*60 + 1800; // Arminius 11.1 蜊傖珨薺 xx:30 羲湖
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
		sprintf(buf, "蚽埶泔桵眒冪扢隅俇傖ㄛ③疑疑袧掘﹝");
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
// Terry fix 猁ь峈-1祥夔ь峈0
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
			//森奀褫眕羲宎泔桵
			manorid = CHAR_getWorkInt(meindex,NPC_WORK_MANORID);
			getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",5,token,sizeof(token));
			hadfmindex = atoi(token);
			if(hadfmindex != -1){
				for(index=0;index<FAMILY_MAXNUM;index++){
					// 梑堤忐蚽模逜腔坰竘
					if(fmdptop.fmtopid[index] == (hadfmindex - 1)) break;
				}
				if(index >= FAMILY_MAXNUM){
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",6,token,sizeof(token));
					printf("\nNPC_ManorSmanLoop():save fm_momentum error(%d:%s)",hadfmindex,token);
					break;
				}
				// 暮翹涴跺奀覦忐蚽模逜腔ァ岊
				else fmpointlist.fm_momentum[manorid-1] = fmdptop.fmMomentum[index];
				// 輛�輴觴遘囆棧�
				fmpks[fmpks_pos+1].flag = FMPKS_FLAG_WAIT;
				// 泔桵ぶ僕4苤奀(1800~2200)
				fmpks[fmpks_pos+1].dueltime = NowTime.tv_sec + 3600 * 4;
				NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_WAIT,fmpks[fmpks_pos+1].dueltime,tm1);
			}
			SortManorSchedule();
		}
#endif    
    break;
#ifdef _NEW_MANOR_LAW
	// 輛�輲譭複�,眒暮翹模逜ァ岊,脹渾泔桵齬最
	case FMPKS_FLAG_WAIT:
		{
			int manorid,i,iPlayerNum = CHAR_getPlayerMaxNum();
			char szMsg[256];

			manorid = CHAR_getWorkInt(meindex,NPC_WORK_MANORID);
			// 遜羶羲湖,ァ岊蜊曹猁�甩懻C猁撈奀訧蹋
			fmpointlist.fm_inwar[manorid-1] = TRUE;
			if(fmpks[fmpks_pos+1].dueltime <= NowTime.tv_sec){
				// 泔桵模逜笢ァ岊郔詢腔鳳腕泔桵��
				int manorindex = CHAR_getWorkInt(meindex,NPC_WORK_MANORID) - 1;
				int iNo1FmIndexSort = ManorSchedule[manorindex].iSort[0];
				int index = ManorSchedule[manorindex].iFmIndex[iNo1FmIndexSort],iFmIndex1,iCharindex;
				char token[256],fmname[256];
				// 羶�佮譭�
				if(index < 0){
					// 眻諉輛�賮諒譭蝸慲�
					fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_BATTLEEND;
					CHAR_setWorkInt(meindex,NPC_WORK_BETTLETIME,tm1.tm_mday);
					// 籵眭垀衄��
					for(i=0;i<iPlayerNum;i++){
						if(CHAR_getCharUse(i) != FALSE){
							CHAR_talkToCli(i,-1,"蚕黺拸模逜泔桵蚽埶ㄛ蚽埶輛�遶楙褊排�",CHAR_COLORBLUE2);
						}
					}
					getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",5,token,sizeof(token));
					iFmIndex1 = atoi(token) - 1;
					if(iFmIndex1 != -1){
						getStringFromIndexWithDelim(fmpointlist.pointlistarray[manorid-1],"|",6,fmname,sizeof(token));
						saacproto_ACFixFMPoint_send(acfd,fmname,iFmIndex1 + 1,iFmIndex1,
							fmname,iFmIndex1 + 1,iFmIndex1,CHAR_getWorkInt(meindex,NPC_WORK_ID));
						// 崝樓蚽埶桵吨蛹Log
						sprintf(token," (%d:%d) %d/%d/%d",tm1.tm_hour,tm1.tm_min,tm1.tm_year+1900,tm1.tm_mon+1,tm1.tm_mday);
						Logfmpk(fmname,iFmIndex1,-1,"拸�佮譭�",-1,-1,token,"","",2);
						// 埻模逜忐蛂賸蚽埶,模逜傖埜褫腕善坒啟
						for(i=0;i<FAMILY_MAXMEMBER;i++){
							iCharindex = familyMemberIndex[iFmIndex1][i];
							// �譧硨葋炰顫醾� 
							if(iCharindex >= 0 && CHAR_getCharUse(iCharindex)){
								// 鳳腕踢ヴ = 跺�侔鑫� * 5000
								int iAddGold = ((float)CHAR_getInt(iCharindex,CHAR_MOMENTUM)/100.0f) * 5000.0f;
								int iGold = CHAR_getInt(iCharindex,CHAR_BANKGOLD),iMaxGold;
								// 珂溫�賳鶲刳靇�
								if(iGold + iAddGold > CHAR_MAXBANKGOLDHAVE){
									CHAR_setInt(iCharindex,CHAR_BANKGOLD,CHAR_MAXBANKGOLDHAVE);
									// 跺�刳靇郱鑑銨臏�,溫善跺�侂篽�
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
								CHAR_talkToCli(iCharindex,-1,"釓賴賸!忐誘蛂蚽埶腔蔣踢眒颯�踽蒫譫鶲刳靇�",CHAR_COLORRED);
								// 模逜腔跺�佷側眢暲鑫げ模疚橧�
								CHAR_setInt(iCharindex,CHAR_MOMENTUM,0);
								CHAR_talkToCli(iCharindex,-1,"蚽埶桵摽跺�佷側眢暲鑫さ橧�",CHAR_COLORRED);
							}
						}
					}
				}
				else{
					// 泔桵奀潔眒徹,恁堤腔泔桵模逜睿忐蚽模逜衄埮珨毞腔奀潔褫眕袧掘
					NPC_ManorSavePKSchedule(meindex,index,0,-1,ManorSchedule[manorindex].tm1[iNo1FmIndexSort]);
					// 羲湖賸,ァ岊蜊曹珩祥蚚�甩懻C猁撈奀訧蹋賸
					fmpointlist.fm_inwar[manorid-1] = FALSE;
					// 籵眭垀衄��
					sprintf(szMsg,"%s 模逜鳳腕泔桵蚽埶訧跡",ManorSchedule[manorindex].szFmName[iNo1FmIndexSort]);
					
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
         	fmpks[fmpks_pos].guest_index, "蚽埶淰嗤桵眒冪羲宎ㄛ③鴃辦�貐﹝�");
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
			// 湖俇摽猁婬徹拻毞符夔婬泔桵
			// �蘄玫繺饑梪睆芺慲魙蓿殿饑梪痦閡銑饡簆噹繷牲�賸珨毞眕奻,眕珨毞數
			if(CHAR_getWorkInt(meindex,NPC_WORK_BETTLETIME) != tm1.tm_mday){
				iOffsetTime = tm1.tm_hour * 3600 * -1;	// 諶隙00:00
			}
			else{	// 桵須賦旰奀帤閉徹珨毞
				iOffsetTime = (24 - tm1.tm_hour) * 3600;	// 硃傖00:00
			}
			//#ifdef _75_TEST
			fmpks[fmpks_pos+1].dueltime = NowTime.tv_sec - tm1.tm_min*60 + iOffsetTime + 172800 + (18 * 3600);	// 媼毞
			//#else
			//	fmpks[fmpks_pos+1].dueltime = NowTime.tv_sec - tm1.tm_min*60 + iOffsetTime + 432000 + (18 * 3600);
			//														|-----------�＜�萸------------| |-硃傖00:00-| |-拻毞-|  |18:00羲宎惆靡|
			//#endif
#endif
			fmpks[fmpks_pos+1].flag = FMPKS_FLAG_MANOR_PEACE_SAVE;
			NPC_CleanPkList( fmpks_pos);
#ifndef _NEW_MANOR_LAW
			NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_MANOR_PEACE_SAVE);
#else
			NPC_ManorSavePKSchedule(meindex, -1, FMPKS_FLAG_MANOR_PEACE_SAVE,fmpks[fmpks_pos+1].dueltime,tm1);
			// ь諾齬最
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

	fseek(f, 0, SEEK_SET);	// 紫偶羲芛
	fgets(tmp, sizeof(tmp), f);
	fmpks[fmpks_pos].flag=-1;

	// 奀潔
	if (getStringFromIndexWithDelim(tmp,"|",1,token,sizeof(token))) {
		fmpks[fmpks_pos+1].dueltime=atoi(token);
	}
	// 翋勦 familyindex
	if (getStringFromIndexWithDelim(tmp,"|",2,token,sizeof(token))) {
	    fmpks[fmpks_pos+1].host_index=atoi(token);
	}
	// 翋勦 模逜靡
	if (getStringFromIndexWithDelim(tmp,"|",3,token,sizeof(token))) {
		strcpy(fmpks[fmpks_pos+1].host_name,makeStringFromEscaped(token));
	}
	// 諦勦 familyindex
	if (getStringFromIndexWithDelim(tmp,"|",4,token,sizeof(token))) {
	    fmpks[fmpks_pos+1].guest_index=atoi(token);
	}
	// 諦勦 模逜靡
	if (getStringFromIndexWithDelim(tmp,"|",5,token,sizeof(token))) {
	    strcpy(fmpks[fmpks_pos+1].guest_name,makeStringFromEscaped(token));
	}
	// 袧掘奀潔
	if (getStringFromIndexWithDelim(tmp,"|",6,token,sizeof(token))) {
		fmpks[fmpks_pos+1].prepare_time=atoi(token);
	}
	// 郔湮�侕�
	if (getStringFromIndexWithDelim(tmp,"|",7,token,sizeof(token))) {
		fmpks[fmpks_pos+1].max_player=atoi(token);
	}
	// よ梓
	if (getStringFromIndexWithDelim(tmp,"|",8,token,sizeof(token))) {
		fmpks[fmpks_pos+1].flag=atoi(token);
	}
	// 勤桵陎⑩
	if (getStringFromIndexWithDelim(tmp,"|",9,token,sizeof(token))) {
		strcpy(fmpks[fmpks_pos+2].host_name,makeStringFromEscaped(token));
	}
  
	// no schedule
	fmpks[fmpks_pos].flag=-1;
	// prepare time expired?
	if ((fmpks[fmpks_pos+1].flag==FMPKS_FLAG_MANOR_PREPARE) &&
		  (fmpks[fmpks_pos+1].dueltime<NowTime.tv_sec)) {
		fmpks[fmpks_pos].flag=-1;
// Terry fix 猁ь峈-1祥夔ь峈0
//		fmpks[fmpks_pos].host_index=0;
		fmpks[fmpks_pos].host_index=-1;
		strcpy(fmpks[fmpks_pos].host_name,"");
// Terry fix 猁ь峈-1祥夔ь峈0
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
		// toindex 曹傖 模逜腔index
		tkfmindex = toindex;
#endif
		fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID) * MAX_SCHEDULE;
		manorid = CHAR_getWorkInt(meindex, NPC_WORK_MANORID);
		memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
#ifndef _NEW_MANOR_LAW
		dueltime = CHAR_getWorkInt(meindex, NPC_WORK_CHALLENGEWAIT) + NowTime.tv_sec - tm1.tm_min*60 + 1800;
#else
		// 眕泔桵奀腔奀潔峈羲湖腔奀潔,笢潔垀冪徹腔奀潔峈袧掘奀潔
		dueltime = NowTime.tv_sec - tm1.tm_min * 60 + ((24 - tm1.tm_hour) * 3600) + 3600 * tm2.tm_hour;
		//				|-------------�＜�萸-------------|  |--------樓善0000奀-------|  |泔桵奀腔奀潔(呾淕萸)| 
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
		// 梑堤泔桵模逜笢ァ岊郔詢模逜坰竘
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
	// toindex 蚚祥善扢峈 -1
	saacproto_ACSendFmPk_send( acfd, -1, PkFlg, CHAR_getWorkInt( meindex, NPC_WORK_ID), msg);
#endif
}

#endif

#ifdef _NEW_MANOR_LAW
// 樓�輲譭褐鑒�
void NPC_ManorAddToSchedule(int meindex,int charaindex)
{
	int i,j,iEmpty = -1,iFmIndex,manorindex = CHAR_getWorkInt(meindex,NPC_WORK_MANORID) - 1;
	struct tm tm1;

	iFmIndex = CHAR_getWorkInt(charaindex,CHAR_WORKFMINDEXI);
	// 潰脤岆瘁眒齬�蹁鑒�,潰脤垀衄腔蚽埶埮桵
	for(j=0;j<MANORNUM;j++){
		for(i=0;i<10;i++){
			if(ManorSchedule[j].iFmIndex[i] != -1){
				if(ManorSchedule[j].iFmIndex[i] == iFmIndex){
					lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
						CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"斕眒冪惆靡徹泔桵蚽埶賸鉊‵");
					return;
				}
			}
			else{
				if(j == manorindex) iEmpty = i;	// 暮翹諾腔弇离
			}
		}
	}
	// �蝜�最宒硒俴善涴爵,桶尨羶衄樓�蹁鑒�,潰脤衄羶衄諾腔弇离
	if(iEmpty != -1){ // 衄諾弇离,參惆靡氪樓��
		// 暮翹模逜坰竘
		ManorSchedule[manorindex].iFmIndex[iEmpty] = iFmIndex;
		// 暮翹模逜靡備
		sprintf(ManorSchedule[manorindex].szFmName[iEmpty],"%s",CHAR_getChar(charaindex,CHAR_FMNAME));
		// 暮翹泔桵奀潔
		memcpy(&tm1,localtime((time_t *)&NowTime.tv_sec),sizeof(tm1));
		memset(ManorSchedule[manorindex].szMemo[iEmpty],0,sizeof(ManorSchedule[manorindex].szMemo[iEmpty]));
		memcpy(&ManorSchedule[manorindex].tm1[iEmpty],&tm1,sizeof(tm1));
		lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"樓�輲譭褐鑒�");
	}
	// 雛賸,睿泔桵齬最腔郔摽珨靡釬掀誕,�蝜�摽懂腔ァ岊掀郔摽珨靡詢,杸遙裁郔摽珨靡,瘁寀籵眭醴ヶァ岊拸楊撥輛齬最
	else{
		// �△譙鍕愻酵�婓ManorSchedule腔弇离
		int index = ManorSchedule[manorindex].iSort[9];

		for(i=0;i<FAMILY_MAXNUM;i++){
			// 梑堤泔桵模逜腔坰竘
			if(fmdptop.fmtopid[i] == iFmIndex) break;
		}
		// 梑祥善森模逜
		if(i >= FAMILY_MAXNUM){
			lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"惕Кㄛ梑祥善斕腔模逜ァ岊訧蹋");
			return;
		}
		// 掀誕ァ岊
		if(ManorSchedule[manorindex].iFmMomentum[index] < fmdptop.fmMomentum[i]){
			int fmpks_pos,iPlayerNum = CHAR_getPlayerMaxNum();
			char szMsg[256];

			// 籵眭垀衄��
			fmpks_pos = CHAR_getWorkInt(meindex,NPC_WORK_ID) * MAX_SCHEDULE;
			sprintf(szMsg,"%s 模逜掩 %s 模逜撥狟泔桵齬最",ManorSchedule[manorindex].szFmName[index],CHAR_getChar(charaindex,CHAR_FMNAME));
			for(i=0;i<iPlayerNum;i++){
				if(CHAR_getCharUse(i) != FALSE){
					CHAR_talkToCli(i,-1,szMsg,CHAR_COLORBLUE2);
				}
			}
			// 郔摽珨靡掩撥裁
			ManorSchedule[manorindex].iFmIndex[index] = iFmIndex;
			// 暮翹模逜靡備
			sprintf(ManorSchedule[manorindex].szFmName[index],"%s",CHAR_getChar(charaindex,CHAR_FMNAME));
			// 暮翹泔桵奀潔
			memcpy(&tm1,localtime((time_t *)&NowTime.tv_sec),sizeof(tm1));
			memcpy(&ManorSchedule[manorindex].tm1[index],&tm1,sizeof(tm1));
			lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"樓�輲譭褐鑒�");
		}
		else{
			lssproto_WN_send(getfdFromCharaIndex(charaindex),WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,CHAR_WINDOWTYPE_CHECKMAN_END,
				CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),"惕Кㄛ斕腔模逜ァ岊祥逋眕齬輛泔桵齬最");
			return;
		}
	}
	// 齬唗
	SortManorSchedule();
}

// 齬唗泔桵齬最
void SortManorSchedule()
{
	int i,j,k;

	// 珂�△譙鍤翹眢暲鑫�
	for(k=0;k<MANORNUM;k++){
		for(i=0;i<10;i++){
			ManorSchedule[k].iSort[i] = i;
			if(ManorSchedule[k].iFmIndex[i] != -1){
				for(j=0;j<FAMILY_MAXNUM;j++){
					// 梑堤泔桵模逜腔坰竘
					if(fmdptop.fmtopid[j] == ManorSchedule[k].iFmIndex[i]){
						// �△蟻鑫ひ�
						ManorSchedule[k].iFmMomentum[i] = fmdptop.fmMomentum[j];
						// 釬傖memo
						sprintf(ManorSchedule[k].szMemo[i],"%-16s %2d:%2d %12d",
							ManorSchedule[k].szFmName[i],ManorSchedule[k].tm1[i].tm_hour,ManorSchedule[k].tm1[i].tm_min,
							ManorSchedule[k].iFmMomentum[i]/100);
						break;
					}
				}
			}
		}
	}
	// 齬唗
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
