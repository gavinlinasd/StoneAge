//Terry 2001/09/27
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_stoneserviceman.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "configfile.h"

#ifdef _SERVICE
#define LOOP_TIME 100		//0.1秒(进入loop function 的时间)
#define MAX_AP_MSG 128		// 接收AP送来讯息的最大数

// socket struct
struct socketaddr_in
{
  u_short sin_family;
  u_short sin_port;
  u_long  sin_addr;
  char    sin_zero[8];
};

// 接收AP传来的讯息
typedef struct _MSG
{
  int nAid;		// nAid => 流水号*10 + 讯息代号(_AP_CLIENT_MESSAGE)
  int nIndex;		// NPC的index值
  int nLen;		// 讯息长度
  LPList Data;		// 实际资料
  int bIsUse;		// 这个讯息是否有使用过
}Msg,*pMsg;
Msg g_APMsg[MAX_AP_MSG];
        
// 动作id        
enum _AP_CLIENT_MESSAGE
{
  Cli_CHECKPLAYER,  //要求AP对玩家是否有购买产品的确认  0
  AP_CHECKPLAYER,   //AP回应玩家是否有购买产品
  Cli_YES,          //玩家确定要将奖品拿走
  Cli_CANCEL,	    //通知AP把lock栏位清为0             
  Cli_GET,	    //通知AP给物品
  AP_GET,	    //AP回应物品			5
  Cli_CONNECT,      //测试是否有连到AP
  AP_CONNECT,       //AP回应
  AP_ERROR          //AP有错误				
};

// NPC的工作变数
enum
{
  NPC_WORK_FLAG1   	 = CHAR_NPCWORKINT2,	//flag1
  NPC_WORK_FLAG2   	 = CHAR_NPCWORKINT3,	//flag2
  NPC_WORK_FLAG3   	 = CHAR_NPCWORKINT4,	//flag3
  NPC_WORK_TIMEOUT 	 = CHAR_NPCWORKINT5,	//time out 时间
  NPC_WORK_STATE   	 = CHAR_NPCWORKINT6,	//目前执行状态
  NPC_WORK_START   	 = CHAR_NPCWORKINT7,	//开始计时flag
  NPC_WORK_LEAVE_COUNT   = CHAR_NPCWORKINT8,	//玩家talk时间计时
  NPC_WORK_TOINDEX 	 = CHAR_NPCWORKINT9,	//玩家的index值
  NPC_WORK_SERIALNUM     = CHAR_NPCWORKINT10,   //流水号
  NPC_WORK_ISUSE   	 = CHAR_NPCWORKINT11,	//目前NPC是否有玩家在使用
};

// 执行的状态
enum
{
  SERVICE_STATE_1,	// check 有没有和AP连线中 
  SERVICE_STATE_2,	// 向AP取得玩家有无买产品
  SERVICE_STATE_3,	// 取得要给玩家的东西的资料
  SERVICE_STATE_4	// end state
};
                                                                
extern int errno;

int g_nServiceSocket;
int g_EnableService = 0;
char g_ApID[32];
struct socketaddr_in server_sin;
struct sockaddr      temp;
unsigned short g_Port;

static void NPC_StoneServiceMan_selectWindow(int meindex,int toindex,int num,int select);
extern int GetSerialNum(void);
static int flag = 1;

/*********************************
* 初始化
*********************************/
BOOL NPC_StoneServiceManInit(int meindex )
{
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  
  if(NPC_Util_GetArgStr(meindex,npcarg,sizeof(npcarg)) == NULL){
    print("StoneServiceMan:GetArgStrErr");
    return FALSE;
  }

  CHAR_setInt(meindex,CHAR_LOOPINTERVAL,LOOP_TIME);
  CHAR_setInt(meindex,CHAR_WHICHTYPE,CHAR_TYPESTONESERVICEMAN);
  CHAR_setWorkInt(meindex,NPC_WORK_FLAG1,1);
  CHAR_setWorkInt(meindex,NPC_WORK_FLAG2,1);
  CHAR_setWorkInt(meindex,NPC_WORK_FLAG3,1);
  CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,0);
  CHAR_setWorkInt(meindex,NPC_WORK_STATE,SERVICE_STATE_2);
  CHAR_setWorkInt(meindex,NPC_WORK_START,0);  
  CHAR_setWorkInt(meindex,NPC_WORK_TOINDEX,-1);
  CHAR_setWorkInt(meindex,NPC_WORK_ISUSE,0);
  CHAR_setWorkInt(meindex,NPC_WORK_LEAVE_COUNT,0);
  CHAR_setWorkInt(meindex,NPC_WORK_SERIALNUM,-1);

  g_EnableService = getEnableService();
  
  //连线到AP在init时只做一次
  if(flag && g_EnableService){
    strcpy(g_ApID,getApID());
		g_Port = getApPort();
    bzero(g_APMsg,sizeof(g_APMsg));
    ConnectToAP();//连线到AP
    flag = 0;
  }
  
  return TRUE;
}

void NPC_StoneServiceManTalked(int meindex,int talkerindex,char *szMes,int color)
{
  if(CHAR_getInt(talkerindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;
  if(NPC_Util_isFaceToFace(talkerindex,meindex,1) == FALSE) return;
  NPC_StoneServiceMan_selectWindow(meindex,talkerindex,0,-1);
}

static void NPC_StoneServiceMan_selectWindow(int meindex,int toindex,int num,int select)
{
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  char token[1024],buf[128],*pUserid;
  int buttontype = 0, windowtype = 0, windowno = 0,nAid,nLen;
  int fd = getfdFromCharaIndex(toindex);
  char Data[3][32],talk[128];
  int i,j,k,Ret;
  LPList DataList = NULL,Kill = NULL;

	
  if(fd == -1) 
  {
    print("getfd err\n");
    return;
  }
	
  if(NPC_Util_GetArgStr(meindex,npcarg,sizeof(npcarg)) == NULL)
  {
    print("GetArgStrErr");
    return ;
  }
  
  token[0] = '\0';

  //如果不使用石器服务员的功能
  if(!g_EnableService)
  {
    sprintf(token,"\n 请到此星系的最後一颗星球使用此服务，谢谢！");
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_SERVICE_WAIT;
    
    lssproto_WN_send(fd,windowtype,buttontype,windowno,
                     CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX),
		     token);    
    return;
  }
  
  //如果这个玩家是第一个和NPC讲话
  if(CHAR_getWorkInt(meindex,NPC_WORK_ISUSE) == 0 && 
     CHAR_getWorkInt(meindex,NPC_WORK_TOINDEX) == -1)
  {
    //记住第一个和NPC讲话的是谁
    CHAR_setWorkInt(meindex,NPC_WORK_TOINDEX,toindex);
    //设定为NPC正在使用中
    CHAR_setWorkInt(meindex,NPC_WORK_ISUSE,1);
  }
  //如果是第二个以後和NPC讲话的人
  else if(CHAR_getWorkInt(meindex,NPC_WORK_ISUSE) == 1 &&
          CHAR_getWorkInt(meindex,NPC_WORK_TOINDEX) != toindex)
  {
    sprintf(token,"\n 正在服务其他玩家中，请稍等...");
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_SERVICE_WAIT;
    
    lssproto_WN_send(fd,windowtype,buttontype,windowno,
                     CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX),
		     token);    
    return;
  }
  if(flag) 
  {
    ConnectToAP();
	flag = 0;
  }
  switch(num)
  {
  case 0:
  case 1:
    //取得玩家的id
    pUserid = CHAR_getChar(toindex,CHAR_CDKEY);
    if(CHAR_getWorkInt(meindex,NPC_WORK_FLAG2) == 1)
    {
	  if(pUserid == NULL || SendToAP(Cli_CHECKPLAYER,meindex,1,pUserid,NULL) == 0)
      {
        if(NPC_Util_GetStrFromStrWithDelim(npcarg,"SysBusyMsg",buf,sizeof(buf)) == NULL) return;
        sprintf(token,"\n %s",buf);
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
        break;
      }
      CHAR_setWorkInt(meindex,NPC_WORK_FLAG2,0);
      CHAR_setWorkInt(meindex,NPC_WORK_START,1);
    }
    if(CHAR_getWorkInt(meindex,NPC_WORK_STATE) == SERVICE_STATE_2)
    {
			// 超过五秒time out
      if(CHAR_getWorkInt(meindex,NPC_WORK_TIMEOUT) >= 50)
      {
        close(g_nServiceSocket);
        flag = 1;
        sprintf(token,"\n 重新与系统取得连线...");
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_CONTINUE;
				CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,0);
				CHAR_setWorkInt(meindex,NPC_WORK_FLAG2,1);
				CHAR_setWorkInt(meindex,NPC_WORK_START,0);
        break;
      }
      if((Ret = RecvFromAP(&nAid,meindex,&nLen,&DataList)) == -1)
      {
        sprintf(token,"\n 等待回应...");
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_CONTINUE;
        break;
      }
      else 
      {
        for(i=0;i<nLen;i++)
        {
          strcpy(Data[i],DataList->Data);
          Kill = DataList;
          DataList = DataList->Next;
          if(Kill != NULL) free(Kill);
        }
        // 使用过的讯息清空
        bzero(&g_APMsg[Ret],sizeof(Msg));
      }
      if(nAid == AP_CHECKPLAYER)
      {
        switch(atoi(Data[0]))
        {
        case 0: //没买
          if(NPC_Util_GetStrFromStrWithDelim(npcarg,"ErrMsg",buf,sizeof(buf)) == NULL) return;
          sprintf(token,"\n %s",buf);
          buttontype = WINDOW_BUTTONTYPE_OK;
          windowtype = WINDOW_MESSAGETYPE_MESSAGE;
          windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;     
        break;
        case 1: //有买
          if(NPC_Util_GetStrFromStrWithDelim(npcarg,"MainMsg",buf,sizeof(buf)) == NULL) return;
          sprintf(token,"5\n %s"
                        "\n 你要将%s"
												"\n 的奖品给这个角色吗？\n"
                        "\n"
                        "\n                    《 确  定 》"
  	                    "\n"
        	              "\n                    《 取  消 》",
                        buf,Data[1]);
          buttontype = WINDOW_BUTTONTYPE_NONE;
          windowtype = WINDOW_MESSAGETYPE_SELECT;
          windowno = CHAR_WINDOWTYPE_SERVICE_START; 
        break;
        //另一个角色正在使用中
        case 2:
          sprintf(token,"\n 目前您的另一个角色正在领取.");
          buttontype = WINDOW_BUTTONTYPE_OK;
          windowtype = WINDOW_MESSAGETYPE_MESSAGE;
          windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
          break;
				//玩家的奖品已领完
				case 3:
					sprintf(token,"\n 您的奖品已经领完了!!");
          buttontype = WINDOW_BUTTONTYPE_OK;
          windowtype = WINDOW_MESSAGETYPE_MESSAGE;
          windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
          break;
        break;
        }
      }
      else
      {
        sprintf(token,"\n 系统讯息错误!!(%d)",nAid);
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
      }
      CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,0);
      CHAR_setWorkInt(meindex,NPC_WORK_FLAG2,1);
    }
    else
    {
      sprintf(token,"\n 等待回应...");
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_SERVICE_CONTINUE;
    }
  break;
  case 2:
    //取得玩家的id
    pUserid = CHAR_getChar(toindex,CHAR_CDKEY);
    if(CHAR_getWorkInt(meindex,NPC_WORK_FLAG3) == 1){
      if(pUserid == NULL || SendToAP(Cli_GET,meindex,1,pUserid,NULL) == 0){
        if(NPC_Util_GetStrFromStrWithDelim(npcarg,"SysBusyMsg",buf,sizeof(buf)) == NULL) return;
        sprintf(token,"\n %s",buf);
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
        break;
      }
			CHAR_setWorkInt(meindex,NPC_WORK_START,1);
      CHAR_setWorkInt(meindex,NPC_WORK_FLAG3,0);
    }
    if(CHAR_getWorkInt(meindex,NPC_WORK_STATE) == SERVICE_STATE_3){
			// 超过五秒time out
      if(CHAR_getWorkInt(meindex,NPC_WORK_TIMEOUT) >= 50){
        close(g_nServiceSocket);
        flag = 1;
        sprintf(token,"\n 重新与系统取得连线...");
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_CONTINUE;
				CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,0);
				CHAR_setWorkInt(meindex,NPC_WORK_FLAG3,1);
				CHAR_setWorkInt(meindex,NPC_WORK_START,0);
        break;
      }
      if((Ret = RecvFromAP(&nAid,meindex,&nLen,&DataList)) == -1){
        sprintf(token,"\n 等待回应...");
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_CONTINUE;
        break;
      }
      else
      {
        for(i=0;i<nLen;i++)
        {
          strcpy(Data[i],DataList->Data);
          Kill = DataList;
          DataList = DataList->Next;
          if(Kill != NULL) free(Kill);
        }
        // 使用过的讯息清空
        bzero(&g_APMsg[Ret],sizeof(Msg));
      }
      if(nAid == AP_GET)
      {
        //如果是给钱
        //确定玩家身上有没有空的栏位
        if(atoi(Data[2]) == 0){
          if(CHAR_getInt(toindex,CHAR_GOLD)+atoi(Data[1]) > CHAR_getMaxHaveGold(toindex) ){
            sprintf(token,"\n"
                          "\n 你身上的空间不足以放入那麽多钱！\n");
            buttontype = WINDOW_BUTTONTYPE_OK;
            windowtype = WINDOW_MESSAGETYPE_MESSAGE;
            windowno = CHAR_WINDOWTYPE_SERVICE_EXIT; 
            break;
          }else{
			CHAR_AddGold( toindex, atoi(Data[1]) );

            CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
            LogService(CHAR_getChar(toindex,CHAR_NAME),
                       CHAR_getChar(toindex,CHAR_CDKEY),
                       atoi(Data[1]),
                       "钱",
                       CHAR_getInt(toindex,CHAR_FLOOR),
                       CHAR_getInt(toindex,CHAR_X),
                       CHAR_getInt(toindex,CHAR_Y)
                      );
            sprintf(talk,"%s石币已经给你喽!!\n",Data[1]);
          }
        }
        //如果是给物品
        else if(atoi(Data[2]) == 1){
          //确定玩家身上有没有空的栏位
          int num = 0,itemindex = 0;

          for(i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++)
            if(CHAR_getItemIndex(toindex,i) != -1) num++;
          if(atoi(Data[1])+num > CHAR_MAXITEMNUM){
            sprintf(token,"\n"
                          "\n 你的道具栏空间不够！\n");
            buttontype = WINDOW_BUTTONTYPE_OK;
            windowtype = WINDOW_MESSAGETYPE_MESSAGE;
            windowno = CHAR_WINDOWTYPE_SERVICE_EXIT; 
            break;
          }
          //有空的栏位
          for(j=0;j<atoi(Data[1]);j++){
            for(i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++){
              if(CHAR_getItemIndex(toindex,i) == -1){
                itemindex = ITEM_makeItemAndRegist(atoi(Data[0]));
                CHAR_setItemIndex(toindex,i,itemindex);
                ITEM_setWorkInt(itemindex,ITEM_WORKOBJINDEX,-1);
                ITEM_setWorkInt(itemindex,ITEM_WORKCHARAINDEX,toindex);
                CHAR_sendItemDataOne(toindex,i);
                LogService(CHAR_getChar(toindex,CHAR_NAME),
                           CHAR_getChar(toindex,CHAR_CDKEY),
                           atoi(Data[0]),
                           "物品",
                           CHAR_getInt(toindex,CHAR_FLOOR),
                           CHAR_getInt(toindex,CHAR_X),
                           CHAR_getInt(toindex,CHAR_Y)
                          );
                break;
              }
            }
          }
          sprintf(talk,"%s %s个已经给你喽!!\n",ITEM_getAppropriateName(itemindex),Data[1]);
        }
        //如果是给宠物
        else if(atoi(Data[2]) == 2)
        {
          //确定玩家身上有没有空的栏位
          int num = 0,ret = 0;
          
          for(i=0;i<CHAR_MAXPETHAVE;i++)
            if(CHAR_getCharPet(toindex,i) != -1) num++;
          if(atoi(Data[1])+num > CHAR_MAXPETHAVE)
          {
            sprintf(token,"\n"
                          "\n 你的宠物栏空间不够！\n");
            buttontype = WINDOW_BUTTONTYPE_OK;
            windowtype = WINDOW_MESSAGETYPE_MESSAGE;
            windowno = CHAR_WINDOWTYPE_SERVICE_EXIT; 
            break;
          }
          //有空的栏位
          for(j=0;j<atoi(Data[1]);j++)
          {
            for(i=0;i<CHAR_MAXPETHAVE;i++)
            {
              if(CHAR_getCharPet(toindex,i) == -1)
              {
                int enemynum,count;
                char msgbuf[64];

                enemynum = ENEMY_getEnemyNum();
                for(k=0;k<enemynum;k++)
                {
                  if(ENEMY_getInt(k,ENEMY_ID) == atoi(Data[0])) break;
                }
                if(k == enemynum) break;
                
                ret = ENEMY_createPetFromEnemyIndex( toindex,k);
                CHAR_setCharPet(toindex,i,ret);
                snprintf(msgbuf,sizeof(msgbuf),"K%d",i);
                CHAR_sendStatusString(toindex,msgbuf);
                snprintf(msgbuf,sizeof(msgbuf),"W%d",i);
                CHAR_sendStatusString(toindex,msgbuf);

								for(count=0;count<CHAR_MAXPETHAVE;count++)
								{
								  if(CHAR_getCharPet(toindex,count) == ret)
									{	
								    CHAR_send_K_StatusString(toindex,count,CHAR_K_STRING_HP|CHAR_K_STRING_AI);
									}
								}
								//CHAR_setInt(petindex,CHAR_VARIABLEAI,CHAR_MAXVARIABLEAI);
                LogService(CHAR_getChar(toindex,CHAR_NAME),
                           CHAR_getChar(toindex,CHAR_CDKEY),
                           atoi(Data[0]),
                           "宠物",
                           CHAR_getInt(toindex,CHAR_FLOOR),
                           CHAR_getInt(toindex,CHAR_X),
                           CHAR_getInt(toindex,CHAR_Y)
                          );
                break;
              }
            }
          }
          sprintf(talk,"%s %s只已经给你喽!!\n",CHAR_getChar(ret,CHAR_NAME),Data[1]);
        }
        //玩家资料存档
        CHAR_charSaveFromConnect(CHAR_getWorkInt(toindex,CHAR_WORKFD),0);
       if(pUserid == NULL || SendToAP(Cli_YES,meindex,1,pUserid,NULL) == 0)
       {
          if(NPC_Util_GetStrFromStrWithDelim(npcarg,"SysBusyMsg",buf,sizeof(buf)) == NULL) return;
          sprintf(token,"\n %s",buf);
          buttontype = WINDOW_BUTTONTYPE_OK;
          windowtype = WINDOW_MESSAGETYPE_MESSAGE;
          windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
					LogService(CHAR_getChar(toindex,CHAR_NAME),
 										 CHAR_getChar(toindex,CHAR_CDKEY),
										 -1,
										 "玩家已领取宠物但Cli_YES没送到AP",
										 CHAR_getInt(toindex,CHAR_FLOOR),
										 CHAR_getInt(toindex,CHAR_X),
										 CHAR_getInt(toindex,CHAR_Y)
										 );
          break;
        }
        if(NPC_Util_GetStrFromStrWithDelim(npcarg , "ExitMsg" , buf , sizeof(buf)) == NULL) return;
        sprintf(token,"%s %s",talk,buf);
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
        CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,0);
        CHAR_setWorkInt(meindex,NPC_WORK_FLAG3,1);
        CHAR_setWorkInt(meindex,NPC_WORK_STATE,SERVICE_STATE_4);
      }
      else 
      {
        sprintf(token,"\n 系统讯息错误!!(%d)",nAid);
        buttontype = WINDOW_BUTTONTYPE_OK;
        windowtype = WINDOW_MESSAGETYPE_MESSAGE;
        windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
        break;
      }
    }
    else
    {
      sprintf(token,"\n 等待回应...");
      buttontype = WINDOW_BUTTONTYPE_OK;
      windowtype = WINDOW_MESSAGETYPE_MESSAGE;
      windowno = CHAR_WINDOWTYPE_SERVICE_CONTINUE;
    }
  break;
  case 3:
    pUserid = CHAR_getChar(toindex,CHAR_CDKEY);
    if(NPC_Util_GetStrFromStrWithDelim(npcarg,"CancelMsg",buf,sizeof(buf)) == NULL) return;
    sprintf(token,"\n %s",buf);
    buttontype = WINDOW_BUTTONTYPE_OK;
    windowtype = WINDOW_MESSAGETYPE_MESSAGE;
    windowno = CHAR_WINDOWTYPE_SERVICE_EXIT;
  break;
  }
  
  lssproto_WN_send(fd,windowtype,buttontype,windowno,
		   CHAR_getWorkInt(meindex,CHAR_WORKOBJINDEX),
		   token);

}

void NPC_StoneServiceManLoop(int meindex)
{
  int nLeaveCount,nTimeout;
  fd_set readfds;
  struct timeval tm;
  LPList kill = NULL;
	//static unsigned int dwLifeTime = 0;
  
  if(g_EnableService)
  {
    // 如果玩家和NPC讲话了就开始倒数
    if(CHAR_getWorkInt(meindex,NPC_WORK_ISUSE) == 1)
    {
      nLeaveCount = CHAR_getWorkInt(meindex,NPC_WORK_LEAVE_COUNT);
      // 如果玩家超过时间没完成动作(也有可能是玩家没有按下ok就离开对话框)
      // 这时要把NPC的工作变数清为零才能让下一个玩家talk
      if(nLeaveCount > getLoopTime())
      {
        int  talkerindex,i;
        char *pUserid;
       
        talkerindex = CHAR_getWorkInt(meindex,NPC_WORK_TOINDEX);
        pUserid = CHAR_getChar(talkerindex,CHAR_CDKEY);
        if(pUserid != NULL) SendToAP(Cli_CANCEL,meindex,1,pUserid,NULL);
        CHAR_setWorkInt(meindex,NPC_WORK_FLAG1,1);
        CHAR_setWorkInt(meindex,NPC_WORK_FLAG2,1);
        CHAR_setWorkInt(meindex,NPC_WORK_FLAG3,1);
        CHAR_setWorkInt(meindex,NPC_WORK_STATE,SERVICE_STATE_2);
        CHAR_setWorkInt(meindex,NPC_WORK_START,0);
        CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,0);
        CHAR_setWorkInt(meindex,NPC_WORK_TOINDEX,-1);
        CHAR_setWorkInt(meindex,NPC_WORK_ISUSE,0);
        CHAR_setWorkInt(meindex,NPC_WORK_LEAVE_COUNT,0);
        CHAR_setWorkInt(meindex,NPC_WORK_SERIALNUM,-1);
        //把之前收的有关这个NPC的讯息buf清空
        for(i=0;i<MAX_AP_MSG;i++)
        {
          // 检查讯息栏有没有被使用
          if(g_APMsg[i].bIsUse == 1)
          {
            // 检查是不是送给这个NPC的讯息
            if(g_APMsg[i].nIndex == meindex)
            {
              // 使用过的讯息清空
              while(g_APMsg[i].Data != NULL)
              {
                kill = g_APMsg[i].Data;
                g_APMsg[i].Data = g_APMsg[i].Data->Next;
                if(kill != NULL) free(kill);
              }
              bzero(&g_APMsg[i],sizeof(Msg));
            }
          }
        }
        print("Player leave NPC\n");
        return;
      }
      // 未超过时间则计数
      CHAR_setWorkInt(meindex,NPC_WORK_LEAVE_COUNT,++nLeaveCount);
    }

		/*if(dwLifeTime > 3000) 
		{
			SendToAP(Cli_CONNECT,meindex,0,NULL,NULL);
			dwLifeTime = 0;
			print("\nI'm life!!!!!");
		}
		else dwLifeTime++;*/
		 // NPC_WORK_START为1时计时五秒,超过五秒为time out
    if(CHAR_getWorkInt(meindex,NPC_WORK_START) == 1)
    {
			tm.tv_sec = 0;
			tm.tv_usec = 0;
			FD_ZERO(&readfds);
			FD_SET(g_nServiceSocket,&readfds);
			// 检查AP有没有送讯息过来
			if(!select(g_nServiceSocket+1,&readfds,NULL,NULL,&tm))
			{
				nTimeout = CHAR_getWorkInt(meindex,NPC_WORK_TIMEOUT);
        CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,++nTimeout);
				return;
			}
			print("\nAP send message.");
			
			// 有送讯息过来接收讯息
			switch(GetMsg(meindex))
			{
				//这个讯息不是给这个NPC
			case  0: 
				print("Msg not send to this npc!!\n");
				nTimeout = CHAR_getWorkInt(meindex,NPC_WORK_TIMEOUT);
				CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,++nTimeout);
				return;
			case -1: break; //重新connect to ap
			case -2: print("Msg buf is full!!\n"); break; 	 // 讯息buffer用光了
			case -3: print("Msg data is wrong!!\n"); break;  // 讯息内容错误
			case -4: print("calloc fail!!\n"); break;        // 记忆体配置错失败
			case -5: print("\nAP send get I'm life"); break; // AP收到Cli_CONNECT的回应
			}
			CHAR_setWorkInt(meindex,NPC_WORK_START,0);
		}
  }
}

void NPC_StoneServiceManWindowTalked(int meindex,int talkerindex,int seqno,int select,char *data)
{
  int datanum = -1,i;
  char *pUserid;
  LPList Kill = NULL;

  makeStringFromEscaped(data);
  datanum = atoi( data);
  switch(seqno)
  {
  case CHAR_WINDOWTYPE_SERVICE_START:
    if(datanum == 1) 
    {
      CHAR_setWorkInt(meindex,NPC_WORK_STATE,SERVICE_STATE_3);
      NPC_StoneServiceMan_selectWindow(meindex,talkerindex,2,-1);
    }
    else 
    if(datanum == 3) NPC_StoneServiceMan_selectWindow(meindex,talkerindex,3,-1);
  break;
  case CHAR_WINDOWTYPE_SERVICE_EXIT:
    pUserid = CHAR_getChar(talkerindex,CHAR_CDKEY);
    if(pUserid != NULL) SendToAP(Cli_CANCEL,meindex,1,pUserid,NULL);
    CHAR_setWorkInt(meindex,NPC_WORK_FLAG1,1);
    CHAR_setWorkInt(meindex,NPC_WORK_FLAG2,1);
    CHAR_setWorkInt(meindex,NPC_WORK_FLAG3,1);
    CHAR_setWorkInt(meindex,NPC_WORK_STATE,SERVICE_STATE_2);
    CHAR_setWorkInt(meindex,NPC_WORK_START,0);
    CHAR_setWorkInt(meindex,NPC_WORK_TIMEOUT,0);
    CHAR_setWorkInt(meindex,NPC_WORK_TOINDEX,-1);
    CHAR_setWorkInt(meindex,NPC_WORK_ISUSE,0);
    CHAR_setWorkInt(meindex,NPC_WORK_LEAVE_COUNT,0);
    CHAR_setWorkInt(meindex,NPC_WORK_SERIALNUM,-1);
    //把之前收的有关这个NPC的讯息buf清空
    for(i=0;i<MAX_AP_MSG;i++)
    {
      // 检查讯息栏有没有被使用
      if(g_APMsg[i].bIsUse == 1)
      {
        // 检查是不是送给这个NPC的讯息
        if(g_APMsg[i].nIndex == meindex)
        {
          // 使用过的讯息清空
          while(g_APMsg[i].Data != NULL)
          {
            Kill = g_APMsg[i].Data;
						if(g_APMsg[i].Data->Next != NULL) 
						{
							g_APMsg[i].Data = g_APMsg[i].Data->Next;
							if(Kill != NULL) free(Kill);
						}
						else
						{
							if(Kill != NULL) free(Kill);
							break;
						}
          }
          bzero(&g_APMsg[i],sizeof(Msg));
        }
      }
    }
  break;
  case CHAR_WINDOWTYPE_SERVICE_CONTINUE:
    if(CHAR_getWorkInt(meindex,NPC_WORK_STATE) == SERVICE_STATE_1)
    {
      NPC_StoneServiceMan_selectWindow(meindex,talkerindex,0,-1);
    }
    if(CHAR_getWorkInt(meindex,NPC_WORK_STATE) == SERVICE_STATE_2)
    {
      NPC_StoneServiceMan_selectWindow(meindex,talkerindex,1,-1);
    }
    if(CHAR_getWorkInt(meindex,NPC_WORK_STATE) == SERVICE_STATE_3)
    {
      NPC_StoneServiceMan_selectWindow(meindex,talkerindex,2,-1);
    }
  break;
  case CHAR_WINDOWTYPE_SERVICE_WAIT:
  break;
  default:
    print("ERROR type\n");
  break;
  }
}

//送讯息到AP

//nAid (Cli_????) nIndex (npc Index) nLen (後续资料笔数) Data1 (user cdkey)
int SendToAP(int nAid,int nIndex,int nLen,void *Data1,void *Data2)
{
  char		buf[128];
  char		*pData[2];
  int			i,j,bufnum;
  fd_set	fdset;
  struct  timeval tmv;
  int     ret;
 // Syu ADD 修正石器服务员星系确认
  char *sags="";
  if( strcmp ( getAccountservername() , "210.64.50.31") == 0 ) sags = "sags31"; //北斗
  if( strcmp ( getAccountservername() , "210.64.50.41") == 0 ) sags = "sags41"; //天神
  if( strcmp ( getAccountservername() , "210.64.50.51") == 0 ) sags = "sags51"; //太阳
  if( strcmp ( getAccountservername() , "210.64.50.61") == 0 ) sags = "sags61"; //紫微
  if( strcmp ( getAccountservername() , "210.64.50.71") == 0 ) sags = "sags71"; //苍龙
  if( strcmp ( getAccountservername() , "210.64.50.81") == 0 ) sags = "sags81"; //银河
  if( strcmp ( getAccountservername() , "210.64.50.91") == 0 ) sags = "sags91"; //天鹰
  if( strcmp ( getAccountservername() , "210.64.50.101") == 0 ) sags = "sags101"; //网路家庭
  if( strcmp ( getAccountservername() , "210.64.50.106") == 0 ) sags = "sags106"; //星乐园
  if( strcmp ( getAccountservername() , "210.64.50.111") == 0 ) sags = "sags111"; //圣兽
  if( strcmp ( getAccountservername() , "152.104.1.206") == 0 ) sags = "hkgs206"; //香港
  if( strcmp ( getAccountservername() , "202.134.122.130") == 0 ) sags = "hkgs130"; //新界

  
  bzero(buf,sizeof(buf));
  pData[0] = (char*)Data1;
  pData[1] = (char*)Data2;
  
  // 以下为编码
  // 取得流水号
  nAid = GetSerialNum()*10+nAid;
// Syu ADD 修正石器服务员星系确认
  nLen=2;	

  sprintf(buf,"&;%d;%d;%d;",nAid,nIndex,nLen);
  CHAR_setWorkInt(nIndex,NPC_WORK_SERIALNUM,nAid/10);
  if(nLen > 0)
  {
 // Syu ADD 修正石器服务员星系确认
    for(j = 0 ; j < 1 ; j++)

    {
      for(i = strlen(buf) ; i < 128 ; i++)
      {
      	if(*pData[j] == '\0')
      	  break;
      	else if(*pData[j] == ';')
      	{
      	  buf[i] = '\\';
      	  buf[++i] = ';';
      	}
      	else if(*pData[j] == '\\')
      	{
      	  buf[i] = '\\';
      	  buf[++i] = '\\';
      	}
      	else if(*pData[j] == '&')
      	{
      	  buf[i] = '\\';
      	  buf[++i] = '&';
      	}
      	else buf[i] = *pData[j];
      	pData[j]++;
      }
// Syu ADD 修正石器服务员星系确认
	  strcat(buf,";");
	  strcat(buf,sags);

      buf[strlen(buf)] = ';';

    }
  }
  else strcat(buf,"0;0;");


  memset(&buf[strlen(buf)],'&',sizeof(buf)-strlen(buf));

	// 检查网路可不可以传送讯息
	FD_ZERO(&fdset);
  FD_SET(g_nServiceSocket,&fdset);
  tmv.tv_sec = tmv.tv_usec = 0;	
  ret = select(g_nServiceSocket + 1,(fd_set*)NULL,&fdset,(fd_set*)NULL,&tmv);
  print("ret1=%d ",ret);


  if(ret > 0)
	{
		// 送讯息到AP
		print("\n**SendToAP**:[%s]",buf);  
		if((bufnum = write(g_nServiceSocket,buf,sizeof(buf))) < 0)
		{
			//与主机失去连线
			print("reconnect to server!\n");
			close(g_nServiceSocket);
			flag = 1;
			return 0;
		}
		print("\nSend %d bytes to AP.",bufnum);

	}
	else
	{
		print("reconnect to server!\n");
		close(g_nServiceSocket);
		flag = 1;
			
		return 0;
	}
	
  return 1;
}

//收到AP回应的讯息
int RecvFromAP(int *nAid,int nIndex,int *nLen,LPList *Data)
{
  int  i;
  
  for(i=0;i<MAX_AP_MSG;i++)
  {
    // 检查讯息栏有没有被使用
    if(g_APMsg[i].bIsUse == 1)
    {
      // 检查是不是送给这个NPC的讯息
      if(g_APMsg[i].nIndex == nIndex)
      {
        *nAid = g_APMsg[i].nAid;
        //检查流水号对不对
        if((*nAid)/10 == CHAR_getWorkInt(nIndex,NPC_WORK_SERIALNUM))
        {
          *nAid = *nAid%10;
          *nLen = g_APMsg[i].nLen;
          *Data = g_APMsg[i].Data;
          
          return i;
        }
      }
    }
  }
  return -1;
}

int GetMsg(int meindex)
{
  char buf[128];
  char temp[3][128];
  char *p;
  int  i = 0,j;
  pMsg Msg = NULL;
  LPList tempList = NULL;
  
	bzero(buf,sizeof(buf));
  // 接收AP传来的讯息
  if(read(g_nServiceSocket,buf,sizeof(buf)) <= 0)
  {
    print("can't read message!\n");
    //重连AP
	close(g_nServiceSocket);
    flag = 1;
    return -1;
  }
  print("**GetFromAP**:[%s]\n",buf);
  // 取得一个没用过的讯息栏来用
  for(j=0;j<MAX_AP_MSG;j++)
  {
    if(g_APMsg[j].bIsUse == 0) 
    {
      Msg = &g_APMsg[j];
      if((Msg->Data = (LPList)calloc( 1,sizeof(List))) != NULL)
      {
        Msg->Data->Next = NULL;
        tempList = Msg->Data;
        break;
      }
      else return -4;
    }
  }
  // 超过MAX_AP_MSG表示讯息栏都满了
  if(j == MAX_AP_MSG) return -2;
	Msg->bIsUse = 1;
  
  // 以下为解码
  if(buf[i] == '&')
  {
    if(buf[++i] == ';')
    {
      bzero(temp[0] , sizeof(temp[0]));
      bzero(temp[1] , sizeof(temp[1]));
      bzero(temp[2] , sizeof(temp[2]));
      for(j = 0 ; j < 3 ; j++)
      {
        p = strchr(&buf[i+1] , ';');
        strncpy(temp[j] , &buf[i+1] , p - (&buf[i+1]));
        i += strlen(temp[j]) + 1;
      }
      Msg->nAid   = atoi(temp[0]);
      Msg->nIndex = atoi(temp[1]);
      Msg->nLen   = atoi(temp[2]);
			// 如果收到的是AP_CONNECT不理这个讯息
			if(Msg->nAid%10 == AP_CONNECT)
			{
				free(Msg->Data);
				bzero(Msg,sizeof(Msg));
				return -5;
			}
      j = 0;
      
      bzero(temp[0] , sizeof(temp[0]));
      
      if(Msg->nLen > 0)
      {
        while(buf[++i] != '\0')
        {
          switch(buf[i])
          {
          case '\\':
            switch(buf[i + 1])
            {
            case ';':
            case '&':
            case '\\':
              temp[0][j++] = buf[++i];
              break;
            default:
              temp[0][j++] = buf[i++];
              break;
            }
          break;
          case ';':
            j = 0;
            strcpy(tempList->Data,temp[0]);
            tempList->Next = (LPList)calloc( 1,sizeof(List));
            tempList = tempList->Next;
            bzero(temp[0],sizeof(temp[0]));
          break;
          case '&':
            buf[i--] = '\0';
          break;
          default:
            temp[0][j++] = buf[i];
          break;
          }
        }
        tempList->Next = NULL;
        // 如果这个讯息是给这个NPC return 1
        if(Msg->nIndex == meindex) return 1;
        else 											 return 0;
      }
    }
  }
  else return -3;
  
  return 1;
}

int ConnectToAP(void)
{
  //make socket and connect to ap
  if((g_nServiceSocket = socket(PF_INET,SOCK_STREAM,0)) < 0)
  //if((g_nServiceSocket = socket(PF_INET,SOCK_DGRAM,0)) < 0)
  {
    print("can't create socket!\n");
    return 0;
  }
  bzero((char*)&server_sin , sizeof(server_sin));
  server_sin.sin_family = AF_INET;
  server_sin.sin_port = htons(g_Port);
  server_sin.sin_addr = inet_addr(g_ApID);
  memcpy(&temp , &server_sin , sizeof(server_sin));
  if(connect(g_nServiceSocket,&temp,sizeof(temp)) < 0)
  {
//	print("connect server error: %s\n" , sys_errlist[errno]);
	print("connect server error !!\n");
    print("ip:%s,port:%d\n",g_ApID,g_Port);
    return 0;
  }
  else print("connect AP server success!\n");
  
  return 1;
}

//取得流水号
int GetSerialNum(void)
{
  static int i = 0;
  
  if(++i > 65535)
  {
    i = 0;
  }
  return i;
}

#endif
