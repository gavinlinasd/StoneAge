#define __CONFIGFILE_C__
#include "version.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "util.h"
//#include "configfile.h"
//ttom
#include "lssproto_util.h"
#include "configfile.h"
#include "net.h"
//ttom end
#include "npcutil.h"
// Arminius 7.12 login announce
#include "char.h"
#include "char_data.h"
// CoolFish: add 
#include "lssproto_serv.h"

#include "npc_raceman.h"

/* 涩烂毛忡  允月厌瞻   */
typedef struct tagConfig
{
    /*皿夫弘仿丞  (愤  读卞菲户凶中仃升引分蛲  */
    char    progname[8];

    char    configfilename[32]; /* config白央奶伙   */
    unsigned char debuglevel;   /* 犯田永弘伊矛伙 */

    unsigned int  usememoryunit;    /*丢乒伉及交瓦永玄扔奶术 */
    unsigned int  usememoryunitnum; /*丢乒伉及交瓦永玄醒 */

    char    asname[32];         /*失市它件玄扔□田及  蟆*/
    unsigned short  acservport; /*失市它件玄扔□田及禾□玄  */
    char    acpasswd[32];       /*失市它件玄扔□田尺及由旦伐□玉*/
    char    gsnamefromas[32];   /*
                                 * 失市它件玄扔□田井日苇尹月
                                 * 必□丞扔□田午仄化及  蟆
                                 */
                                 
    // Arminius 7.24 manor pk
    char gsid[32];	// game server chinese id
#ifdef _SERVICE    
    // Terry 2001/10/03
    char apid[32];      // service ap id
		unsigned short apport;	//service ap port
    int  looptime;	// 几秒後设定离开
    int  enableservice;	// 是否使用石器服务员功能
#endif    
    unsigned short allowmanorpk;	// is this server allow manor pk
                                 
    unsigned short port;        /* 扔□田及谨切熬仃禾□玄 */
	
	int				servernumber;	/* 必□丞扔□田及  寞 */
	int				reuseaddr;	/* Address already used... 互鞅引日卅中凛及凶户卞 */
    int             do_nodelay;     /* TCP_NODELAY 卞允月井升丹井 */
    int             log_write_time; /* 踏五仇心夫弘毛允月井升丹井［ */
    int             log_io_time;    /* I/O蝈  及凛棉反井月井升丹井［ */
    int             log_game_time;  /* 必□丞及质  蝈  及凛棉毛反井月 */
    int             log_netloop_faster; /* netloop_faster 及夫弘 */
	int				saacwritenum;	/* 失市它件玄扔□田尺及窒谛  卞write允月井 */
	int				saacreadnum;	/* 失市它件玄扔□田井日及dispatch 毛窒荚允月井 */

    unsigned short fdnum;           /*升木分仃戊生弁扑亦件毛忡  允月井 */
    unsigned int   othercharnum;    /*  公及职及平乓仿及醒  */

    unsigned int objnum;            /* 左皮斥尼弁玄及    醒*/
    unsigned int   petcharnum;   	/* 矢永玄及醒    */
    unsigned int itemnum;           /* 失奶  丞及    醒*/
    unsigned int battlenum;         /* 田玄伙及    */
    unsigned int battleexp;         /* 田玄伙及    */
    
    char    topdir[64];         	/* 玄永皿犯奴伊弁玄伉  */

    char    mapdir[64];         	/* 穴永皿犯奴伊弁玄伉  */
    char    maptilefile[64];    	/* 穴永皿涩烂白央奶伙  */
    char    battlemapfile[64];    	/* 田玄伙穴永皿涩烂白央奶伙  */
    char    itemfile[64];       	/* 失奶  丞涩烂白央奶伙  */
    char    invfile[64];        	/*   衬涩烂白央奶伙  */
    char    appearfile[64];     	/* 请蜇匏  涩烂白央奶伙  */
	char	titlenamefile[64];		/* 惫寞白央奶伙   */
	char	titleconfigfile[64];	/* 惫寞涩烂白央奶伙   */
	char	encountfile[64];		/* 巨件市它件玄涩烂白央奶伙   */
	char	enemybasefile[64];		/* 衬湘  涩烂白央奶伙   */
	char	enemyfile[64];			/* 衬涩烂白央奶伙   */
	char	groupfile[64];			/* 弘伙□皿涩烂白央奶伙   */
	char	magicfile[64];			/* 热诸涩烂白央奶伙   */
	        #ifdef __ATTACK_MAGIC


        char    attmagicfile[64];       // 攻击性咒术

        #endif

	char	petskillfile[64];		/* 矢永玄  热诸涩烂白央奶伙   */
    char    itematomfile[64];       /* 失奶  丞及笺    白央奶伙 */
    char    effectfile[64];     	/* 梢请涩烂白央奶伙  */
    char    quizfile[64];     		/* 弁奶术涩烂白央奶伙  */


    char    lsgenlog[64];       /*扔□田及lsgen 失它玄皿永玄白央奶伙  */

    char    storedir[64];       /*旦玄失犯奴伊弁玄伉    */
    char    npcdir[64];         /*NPC及涩烂白央奶伙毛  仁犯奴伊弁玄伉   */

    char    logdir[64];         /*
                                 * 夫弘犯奴伊弁玄伉
                                 */
    char    logconfname[64];    /*
                                 * 夫弘涩烂白央奶伙  
                                 */
    char	chatmagicpasswd[64];	/* 民乓永玄  芊由旦伐□玉 */

#ifdef _STORECHAR
    char	storechar[64];
#endif
	
    unsigned int 	chatmagiccdkeycheck;	/* 民乓永玄  芊匹CDKEY毛民尼永弁允月井 */
    
    unsigned int    filesearchnum;     /*白央奶伙毛腹绸匹五月白央奶伙及醒*/
    unsigned int    npctemplatenum;     /*NPC及  件皿伊□玄白央奶伙及醒*/
    unsigned int    npccreatenum;       /*NPC及戏遣白央奶伙及醒*/
    unsigned int    walksendinterval;   /* 汹仁及毛霜月棉厥 */
    unsigned int    CAsendinterval_ms;     /* CA毛霜月棉厥 (ms)*/
    unsigned int    CDsendinterval_ms;     /* CD毛霜月棉厥 (ms)*/
    unsigned int    Onelooptime_ms;     	/* 1伙□皿卞井仃月凛棉 */
	unsigned int	Petdeletetime;		/* 矢永玄互壅  允月凛棉 */
	unsigned int	Itemdeletetime;		/* 失奶  丞互壅  允月凛棉 */
    /* 夫弘奶件  及平乓仿及本□皮毛允月棉厥 */
    unsigned int    CharSavesendinterval;
    unsigned int    addressbookoffmsgnum;  /*
                                            * 失玉伊旦皮永弁卞左白仿奶件
                                            * 丢永本□斥毛
                                            * 窒丢永本□斥酸六月井
                                            */

    unsigned int    protocolreadfrequency;  /*
                                             * 皿夫玄戊伙毛窒立伉    
                                             * 卞  戈井
                                             */
    unsigned int    allowerrornum;          /*
                                             * 巨仿□毛窒蜊引匹袱允井
                                             */
    unsigned int    loghour;          		/*
                                             * 夫弘毛忡绣允月凛对  ＂凛  
                                             */
    unsigned int    battledebugmsg;    		/*
                                             * 田玄伙  及犯田永弘丢永本□斥毛请允井［ㄟ卅日请今卅中
                                             */
    //ttom add this because the second had this                                         
    unsigned int    encodekey;              
    unsigned int    acwbsize;             
    unsigned int    acwritesize;
    unsigned int    ErrUserDownFlg;
    //ttom end
#ifdef _GMRELOAD
	char	gmsetfile[64];				/* GM帐号、权限设定档 */
#endif

#ifdef _AUCTIONEER
  char auctiondir[256];   // 拍卖资料目录
#endif
#ifdef _BLACK_MARKET 
  char blackmarketfile[256];
#endif
#ifdef _M_SERVER
    char    msname[32];         
    unsigned short  msport;
#endif
#ifdef _NPCSERVER_NEW
	char    nsaddress[64];         
	unsigned short  nsport;
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	char profession[64];
#endif

#ifdef _ITEM_QUITPARTY
    char itemquitparty[64];
#endif

#ifdef _MUSEUM
	int museum;
#endif

#ifdef _DEL_DROP_GOLD	
	unsigned int	Golddeletetime;
#endif

#ifdef _NEW_PLAYER_CF
int	newplayertrans;
int	newplayerlv;
int	newplayergivepet[5];
int newplayergiveitem[15];
int	newplayerpetlv;
unsigned int newplayergivegold;
#endif

#ifdef _USER_EXP_CF
char	expfile[64];
#endif

#ifdef _UNLAW_WARP_FLOOR
int	unlawwarpfloor[5];
#endif

#ifdef _UNREG_NEMA
char	unregname[5][16];
#endif

#ifdef _TRANS_LEVEL_CF
int chartrans;
int pettrans;
int yblevel;
#endif

#ifdef _LOCK_IP
char lockip[64];
#endif
}Config;
Config config;

#ifdef _USER_EXP_CF
int NeedLevelUpTbls[160];
int MaxLevel=0;
#endif

#ifdef _LOCK_IP
typedef struct slockip
{
    int			a,b,c,d;
    int			locktime;
    int			logintime;
}sLockip;
sLockip lockip[100];

int maxlockip=0;
#endif
/*
 *  戊件白奴弘白央奶伙毛  戈凛卞银丹厌瞻  
 *  xxxx=yyyy 心凶中卅及毛  戈
 */

typedef struct tagReadConf
{
    char    name[32];       /*xxxx卞丐凶月袄*/

    /*戚及2勾反NULL毛  木月午窒手质  仄卅中*/
    char  *charvalue;      /*yyyy毛公及引引医  允月凛及医  燮*/
    size_t  charsize;       /*charvalue及扔奶术*/

    /*
     *     酷  午仄化=及  互 "ON"分匀凶日 intvalue 卞反1毛医  允月
     * 公木动陆反 atoi 及瑛绊
     */
    void*    value;       /*yyyy毛  晶允月医  允月凛及医  燮*/
    CTYPE    valuetype;
}ReadConf;

ReadConf readconf[]=
{
    { "debuglevel"      , NULL ,0 , (void*)&config.debuglevel      ,CHAR},

    { "usememoryunit"   , NULL ,0 , (void*)&config.usememoryunit   ,INT},
    { "usememoryunitnum", NULL ,0 , (void*)&config.usememoryunitnum,INT},

    { "acserv",			config.asname,sizeof(config.asname) ,NULL , 0},
    { "acservport",		NULL ,0 , (void*)&config.acservport     ,SHORT},
    { "acpasswd",		config.acpasswd,sizeof( config.acpasswd),NULL,0},
    { "gameservname",	config.gsnamefromas,sizeof(config.gsnamefromas),
     NULL,0},

    // Arminius 7.24 manor pk
    { "gameservid", config.gsid, sizeof(config.gsid), NULL, 0},
#ifdef _SERVICE    
    // Terry 2001/10/03 service ap
    { "apid", config.apid, sizeof(config.apid), NULL, 0},
		{ "apport",	NULL ,0 ,(void*)&config.apport ,SHORT},
    { "looptime",NULL,0,(void*)&config.looptime,INT},
    { "enableservice",NULL,0,(void*)&config.enableservice,INT},
#endif    
    { "allowmanorpk", NULL, 0, (void*)&config.allowmanorpk, SHORT},

    { "port",			NULL ,0 , (void*)&config.port           ,SHORT},
    { "servernumber",	NULL ,0 , (void*)&config.servernumber           ,INT},

    { "reuseaddr",			NULL ,0 , (void*)&config.reuseaddr  ,		INT},
    { "nodelay",			NULL , 0 , (void*)&config.do_nodelay ,		INT},
    { "log_write_time", 	NULL, 0 , (void*)&config.log_write_time,	INT},
    { "log_io_time", 		NULL, 0 , (void*)&config.log_io_time, 		INT},
    { "log_game_time",		NULL, 0 , (void*)&config.log_game_time,		INT},
    { "log_netloop_faster", NULL,0,(void*)&config.log_netloop_faster,	INT},
    { "saacwritenum",		NULL,0,(void*)&config.saacwritenum, 		INT},
    { "saacreadnum",		NULL,0,(void*)&config.saacreadnum, 			INT},
    { "fdnum",				NULL ,0 , (void*)&config.fdnum,				SHORT},
    { "petnum",				NULL ,0 , (void*)&config.petcharnum,		INT},
    { "othercharnum",		NULL ,0 , (void*)&config.othercharnum,		INT},

    { "objnum",			NULL ,0 , (void*)&config.objnum,				INT},
    { "itemnum",		NULL ,0 , (void*)&config.itemnum,				INT},
    { "battlenum",		NULL ,0 , (void*)&config.battlenum,				INT},
    { "battleexp",		NULL ,0 , (void*)&config.battleexp,				INT},
    { "topdir"          , config.topdir,sizeof(config.topdir),NULL,0},
    { "mapdir"          , config.mapdir,sizeof(config.mapdir),NULL,0},
    { "maptilefile"     , config.maptilefile,sizeof(config.maptilefile),NULL,0},
    { "battlemapfile"   , config.battlemapfile,sizeof(config.battlemapfile),NULL,0},

#ifdef _ITEMSET6_TXT
	{ "itemset6file",	config.itemfile,	sizeof(config.invfile),	NULL,	0},
#else
#ifdef _ITEMSET5_TXT
	{ "itemset5file",	config.itemfile,	sizeof(config.invfile),	NULL,	0},
#else
#ifdef _ITEMSET4_TXT
	{ "itemset4file"  , config.itemfile,sizeof(config.invfile),NULL,0},
#else
#ifdef _ITEMSET3_ITEM
	{ "itemset3file"  , config.itemfile,sizeof(config.invfile),NULL,0},
#endif
#endif
#endif
#endif
  { "invinciblefile"  , config.invfile,sizeof(config.invfile),NULL,0},
  { "appearpositionfile"  , config.appearfile,sizeof(config.appearfile),NULL,0},
	{ "titlenamefile", config.titlenamefile, sizeof( config.titlenamefile),NULL,0},
	{ "titleconfigfile", config.titleconfigfile, sizeof( config.titleconfigfile),NULL,0},
	{ "encountfile", config.encountfile, sizeof( config.encountfile),NULL,0},
	{ "enemyfile", config.enemyfile, sizeof( config.enemyfile),NULL,0},
	{ "enemybasefile", config.enemybasefile, sizeof( config.enemybasefile),NULL,0},
	{ "groupfile", config.groupfile, sizeof( config.groupfile),NULL,0},
	{ "magicfile", config.magicfile, sizeof( config.magicfile),NULL,0},
#ifdef __ATTACK_MAGIC
	{ "attmagicfile" , config.attmagicfile , sizeof( config.attmagicfile )  , NULL , 0 },
#endif

#ifdef _PETSKILL2_TXT
	{ "petskillfile2", config.petskillfile, sizeof( config.petskillfile),NULL,0},
#else
	{ "petskillfile1", config.petskillfile, sizeof( config.petskillfile),NULL,0},
#endif

    { "itematomfile" , config.itematomfile, sizeof( config.itematomfile),NULL,0},
    { "effectfile"  , config.effectfile,sizeof(config.effectfile),NULL,0},
    { "quizfile"  , config.quizfile,sizeof(config.quizfile),NULL,0},

    { "lsgenlogfilename", config.lsgenlog,sizeof(config.lsgenlog),NULL,0},
#ifdef _GMRELOAD
	{ "gmsetfile", config.gmsetfile, sizeof( config.gmsetfile),NULL,0},
#endif

    { "storedir"        ,config.storedir,sizeof(config.storedir),NULL,0},
    { "npcdir"          ,config.npcdir,sizeof(config.npcdir),NULL,0},
    { "logdir"          ,config.logdir,sizeof(config.logdir),NULL,0},
    { "logconfname"     ,config.logconfname,sizeof(config.logconfname),NULL,0},
    { "chatmagicpasswd", config.chatmagicpasswd, sizeof( config.chatmagicpasswd),NULL,0},
#ifdef _STORECHAR
    { "storechar", config.storechar, sizeof( config.storechar),NULL,0},
#endif
    { "chatmagiccdkeycheck",  NULL,0, &config.chatmagiccdkeycheck,INT},
    { "filesearchnum",  NULL,0, &config.filesearchnum,INT},
    { "npctemplatenum",  NULL,0, &config.npctemplatenum,INT},
    { "npccreatenum",    NULL,0, &config.npccreatenum,INT},
    { "walkinterval" ,NULL,0,(void*)&config.walksendinterval,INT},
    { "CAinterval" ,NULL,0,(void*)&config.CAsendinterval_ms,INT},
    { "CDinterval" ,NULL,0,(void*)&config.CDsendinterval_ms,INT},
    { "CharSaveinterval" ,NULL,0,(void*)&config.CharSavesendinterval,INT},
    { "Onelooptime" ,NULL,0,(void*)&config.Onelooptime_ms,INT},
    { "Petdeletetime" ,NULL,0,(void*)&config.Petdeletetime,INT},
    { "Itemdeletetime" ,NULL,0,(void*)&config.Itemdeletetime,INT},
	 { "addressbookoffmesgnum" ,NULL,0,
      (void*)&config.addressbookoffmsgnum,INT},

    { "protocolreadfrequency" ,NULL,0,
      (void*)&config.protocolreadfrequency,INT},

    { "allowerrornum" ,NULL,0,(void*)&config.allowerrornum,INT},
    { "loghour" ,NULL,0,(void*)&config.loghour,INT},
    { "battledebugmsg" ,NULL,0,(void*)&config.battledebugmsg,INT},
    //ttom add because the second had
    { "encodekey" ,NULL,0,(void*)&config.encodekey,INT},
    { "acwritesize" ,NULL,0,(void*)&config.acwritesize,INT},
    { "acwbsize" ,NULL,0,(void*)&config.acwbsize,INT},
    { "erruser_down" ,NULL,0,(void*)&config.ErrUserDownFlg,INT},    
    //ttom end
#ifdef _AUCTIONEER
    { "auctiondir" , config.auctiondir, sizeof(config.auctiondir),NULL,0},
#endif
#ifdef _BLACK_MARKET
	{ "blackmarketfile", config.blackmarketfile, sizeof(config.blackmarketfile), NULL, 0},
#endif
#ifdef _M_SERVER
    { "msname",		config.msname,sizeof(config.msname) ,NULL , 0},
    { "msport",		NULL ,0 , (void*)&config.msport     ,SHORT},
#endif
#ifdef _NPCSERVER_NEW
    { "npcaddress",		config.nsaddress, sizeof(config.nsaddress) ,NULL , 0},
    { "nsport",		NULL, 0 , (void*)&config.nsport     ,SHORT},
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
    { "profession",		config.profession, sizeof(config.profession) ,NULL , 0},
#endif

#ifdef _ITEM_QUITPARTY
    { "itemquitparty",	config.itemquitparty, sizeof(config.itemquitparty) ,NULL , 0},
#endif

#ifdef _MUSEUM
    { "museum",			NULL , 0 , (void*)&config.museum ,		INT},
#endif

#ifdef _DEL_DROP_GOLD
	{ "Golddeletetime" ,NULL,0,(void*)&config.Golddeletetime,	INT},
#endif

#ifdef _NEW_PLAYER_CF
	{ "TRANS" ,NULL,0,(void*)&config.newplayertrans,	INT},
	{ "LV" ,NULL,0,(void*)&config.newplayerlv,	INT},	
	{ "PET1" ,NULL,0,(void*)&config.newplayergivepet[1],	INT},
	{ "PET2" ,NULL,0,(void*)&config.newplayergivepet[2],	INT},	
	{ "PET3" ,NULL,0,(void*)&config.newplayergivepet[3],	INT},	
	{ "PET4" ,NULL,0,(void*)&config.newplayergivepet[4],	INT},	
	{ "ITEM1" ,NULL,0,(void*)&config.newplayergiveitem[0],	INT},
	{ "ITEM2" ,NULL,0,(void*)&config.newplayergiveitem[1],	INT},	
	{ "ITEM3" ,NULL,0,(void*)&config.newplayergiveitem[2],	INT},	
	{ "ITEM4" ,NULL,0,(void*)&config.newplayergiveitem[3],	INT},	
	{ "ITEM5" ,NULL,0,(void*)&config.newplayergiveitem[4],	INT},
	{ "ITEM6" ,NULL,0,(void*)&config.newplayergiveitem[5],	INT},	
	{ "ITEM7" ,NULL,0,(void*)&config.newplayergiveitem[6],	INT},	
	{ "ITEM8" ,NULL,0,(void*)&config.newplayergiveitem[7],	INT},	
	{ "ITEM9" ,NULL,0,(void*)&config.newplayergiveitem[8],	INT},
	{ "ITEM10" ,NULL,0,(void*)&config.newplayergiveitem[9],	INT},	
	{ "ITEM11" ,NULL,0,(void*)&config.newplayergiveitem[10],	INT},	
	{ "ITEM12" ,NULL,0,(void*)&config.newplayergiveitem[11],	INT},	
	{ "ITEM13" ,NULL,0,(void*)&config.newplayergiveitem[12],	INT},	
	{ "ITEM14" ,NULL,0,(void*)&config.newplayergiveitem[13],	INT},	
	{ "ITEM15" ,NULL,0,(void*)&config.newplayergiveitem[14],	INT},
	{ "PETLV" ,NULL,0,(void*)&config.newplayerpetlv,	INT},	
	{ "GOLD" ,NULL,0,(void*)&config.newplayergivegold,	INT},
#endif

#ifdef _USER_EXP_CF
	{ "USEREXP", config.expfile, sizeof( config.expfile),NULL,0},
#endif

#ifdef _UNLAW_WARP_FLOOR
	{ "FLOOR1" ,NULL,0,(void*)&config.unlawwarpfloor[0],	INT},
	{ "FLOOR2" ,NULL,0,(void*)&config.unlawwarpfloor[1],	INT},	
	{ "FLOOR3" ,NULL,0,(void*)&config.unlawwarpfloor[2],	INT},	
	{ "FLOOR4" ,NULL,0,(void*)&config.unlawwarpfloor[3],	INT},	
	{ "FLOOR5" ,NULL,0,(void*)&config.unlawwarpfloor[4],	INT},
#endif
#ifdef _UNREG_NEMA
	{ "NAME1" ,config.unregname[0], sizeof( config.unregname[0]),NULL,0},
	{ "NAME2" ,config.unregname[1], sizeof( config.unregname[1]),NULL,0},
	{ "NAME3" ,config.unregname[2], sizeof( config.unregname[2]),NULL,0},
	{ "NAME4" ,config.unregname[3], sizeof( config.unregname[3]),NULL,0},
	{ "NAME5" ,config.unregname[4], sizeof( config.unregname[4]),NULL,0},
#endif
#ifdef _TRANS_LEVEL_CF
	{ "CHARTRANS" ,NULL,0,(void*)&config.chartrans,	INT},
	{ "PETTRANS" ,NULL,0,(void*)&config.pettrans,	INT},	
	{ "LEVEL" ,NULL,0,(void*)&config.yblevel,	INT},	
#endif
#ifdef _LOCK_IP
	{ "LOCKIP" ,config.lockip, sizeof( config.lockip),NULL,0},
#endif

};

// Arminius 7.12 login announce
char announcetext[8192];
void AnnounceToPlayer(int charaindex)
{
  char *ptr,*qtr;
  
  ptr=announcetext;
  while ((qtr=strstr(ptr,"\n"))!=NULL) {
    qtr[0]='\0';
//    printf("ptr=%s\n",ptr);
    CHAR_talkToCli(charaindex, -1, ptr, CHAR_COLORYELLOW);
    qtr[0]='\n';
    ptr=qtr+1;
  }
  CHAR_talkToCli(charaindex, -1, ptr, CHAR_COLORYELLOW);
  
}

// Robin 0720
void AnnounceToPlayerWN(int fd)
{
	char buf[8192];
	lssproto_WN_send( fd , WINDOW_MESSAGETYPE_LOGINMESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( announcetext, buf, sizeof(buf))
		);
}


void LoadAnnounce(void)
{
    FILE *f;

    memset(announcetext, 0, sizeof(announcetext));
    if ((f=fopen("./announce.txt","r"))!=NULL) {
      fread(announcetext, sizeof(announcetext), 1, f);
      announcetext[sizeof(announcetext)-1]='\0';
      fclose(f);
    }
}
#ifdef _PET_TALKPRO
PTALK pettalktext[PETTALK_MAXID];

void LoadPetTalk(void)
{
	FILE *fp;
	char fn[256];
	char line[ 4096];
	char talkmem[4096];
	int maxid=0;
	char buf1[256], buf2[256], buf3[256];
	int talkNO=-1, mark=-1, i;
	int len = sizeof( talkmem);
	
	memset(talkmem, 0, sizeof(talkmem));
	sprintf(fn, "%s/pettalk/pettalk.menu", getNpcdir());

	for( i=0;i<PETTALK_MAXID;i++)	{
		pettalktext[i].ID = -1;
		strcpy( pettalktext[i].DATA, "\0");
	}

	print("\n装载宠物对话文件:%s...", fn);
	fp = fopen( fn, "r");
    if( fp != NULL ) {
		while( fgets( line, sizeof( line), fp)) {
			if( strlen( talkmem) != 0 ) {
				if( talkmem[strlen( talkmem) -1] != '|' ) {
					strcatsafe( talkmem, len, "|");		
				}
			}
			chompex( line);
			strcatsafe( talkmem,len,  line);
		}
		fclose( fp);
    }else	{
		print("错误:找不到文件!");
	}

	talkNO=1;
	while( getStringFromIndexWithDelim( talkmem,"END",talkNO, buf1, sizeof( buf1)) != FALSE	){
		talkNO++;
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "PETTEMPNO", buf2, sizeof( buf2)) == NULL  )
			continue;
		mark=1;
		strcpy( fn,"\0");

		if( getStringFromIndexWithDelim( buf2,",", mark+1,buf3,sizeof( buf3)) != FALSE )	{
			pettalktext[maxid].ID = atoi( buf3);
			if( getStringFromIndexWithDelim( buf2,",", mark,buf3,sizeof( buf3)) != FALSE )	{
				sprintf(fn, "%s/pettalk/%s", getNpcdir(), buf3);
				//print("\n ...file:%s", fn);
				fp = fopen( fn, "r");
				if( fp != NULL )	{
					char line[4096];
					while( fgets( line, sizeof( line), fp ) ) {
						if( strlen( pettalktext[maxid].DATA) != 0 ) {
							if( pettalktext[maxid].DATA[strlen( pettalktext[maxid].DATA) -1] != '|' ) {
								strcatsafe( pettalktext[maxid].DATA, sizeof( pettalktext[maxid].DATA), "|");
							}
						}
						chompex( line);
						strcatsafe( pettalktext[maxid].DATA, sizeof( pettalktext[maxid].DATA), line);
					}
					maxid++;
					fclose( fp);
				}else	{
					print("错误:[%s] 找不到!", fn);
					pettalktext[maxid].ID=-1;
				}
			}else	{
				pettalktext[maxid].ID=-1;
			}
		}
		print(".");
		if( maxid >= PETTALK_MAXID )
			break;
	}
	print("最大ID=%d...", maxid);
	{
		int haveid=0;
		for( i=0;i<PETTALK_MAXID;i++)	{
			if( pettalktext[i].ID >= 0 )	{
				haveid++;
			}
		}
		print("载入总数=%d", haveid);
	}

}

#else
char pettalktext[4096];
void LoadPetTalk(void)
{
  FILE *fp;
  char fn[256];
  char	line[ 4096];
  int len = sizeof( pettalktext);
  
  memset(pettalktext, 0, sizeof(pettalktext));
  sprintf(fn, "%s/pettalk/pettalk.mem", getNpcdir());
  
  fp = fopen( fn, "r");
    if( fp != NULL ) {
		print("\n\n 读取 pettalk.mem");
		while( fgets( line, sizeof( line), fp)) {
			if( strlen( pettalktext) != 0 ) {
				if( pettalktext[strlen( pettalktext) -1] != '|' ) {
					strcatsafe( pettalktext, len, "|");		
				}
			}
			chompex( line);
			strcatsafe( pettalktext,len,  line);
		}
		fclose( fp);
		print("\n %s", pettalktext);
    }else	{
		print("\n 不能找到 pettalk.mem");
	}
}
#endif

#ifdef _GAMBLE_BANK
GAMBLEBANK_ITEMS GB_ITEMS[GAMBLEBANK_ITEMSMAX];

void Load_GambleBankItems( void)
{

	FILE *fp;
	char filename[256];
	char buf1[256];
	char name[128];
	int num,ID,type;
	int i=0;
	sprintf(filename, "./data/gambleitems.txt" );
	print("\n加载赌博物品文件 %s ...", filename);
	fp = fopen( filename, "r");
    if( fp != NULL ) {
		while( fgets( buf1, sizeof( buf1), fp) != NULL )	{
			if( strstr( buf1, "#") != 0 ) continue;
			sscanf( buf1,"%s %d %d %d", name, &ID, &num , &type);
			strcpy( GB_ITEMS[i].name, name);
			GB_ITEMS[i].Gnum = num;
			GB_ITEMS[i].ItemId = ID;
			GB_ITEMS[i].type = type;
			i++;
		}
		print("最大ID: %d ", i);
		fclose( fp);
    }else	{
		print("错误 找不到文件 %s", filename);
	}

}
#endif


#ifdef _CFREE_petskill
PETSKILL_CODES Code_skill[PETSKILL_CODE];
void Load_PetSkillCodes( void)
{
	FILE *fp;
	char filename[256];
	char buf1[256];
	char name[128];
	char type[256];
	int num,ID;
	int i=0;
	sprintf(filename, "./data/skillcode.txt" );
	print("\n加载宠物技能编码文件:%s...", filename);
	fp = fopen( filename, "r");
    if( fp != NULL ) {
		while( fgets( buf1, sizeof( buf1), fp) != NULL )	{
			sscanf( buf1,"%s %d %d %s", name, &num, &ID, type);
			strcpy( Code_skill[i].name, name);
			Code_skill[i].TempNo = num;
			Code_skill[i].PetId = ID;
			strcpy( Code_skill[i].Code, type);
			//print("\n %s|%d|%d|%s|", Code_skill[i].name, Code_skill[i].TempNo, 
			//	Code_skill[i].PetId, Code_skill[i].Code);
			i++;
			if( i >= PETSKILL_CODE ) break;
		}
		fclose( fp);
    }else	{
		print("打不到文件 %s", filename);
	}
	print("完成\n");
}
#endif

#ifdef _BLACK_MARKET
BOOL LoadBMItem( char* filename)
{
	FILE *fp;	
	int  i, j, k;
	char line[512]="", cTmp[256]="";
	char *ip=NULL, *gp=NULL;	
    
	for(i=0; i<BMIMAX; i++){		
		BMItem[i].iGraphicsNum      = 0;
		BMItem[i].GCondition        = 0;
		for(j=0; j<4; j++){			
		    BMItem[i].iCondition[j] = 0;
			for(k=0; k<3; k++){			
				BMItem[i].iId[j][k] = 0;
			}
		}
		strcpy( BMItem[i].iName, "");
	}	
	for(i=0; i<12; i++) BMSellList[i] = -1;

	fp = fopen( filename, "r");

	if(fp==NULL){
		print("\n故障!! 不能打开文件:%s ...\n", filename);
		return FALSE;
	}else{
		while(fgets( line, sizeof(line), fp)!=NULL){			
			char cTmp1[256]="", cTmp2[256]="", cTmp3[256]="";
			char iTmp1[128]="", iTmp2[128]="", iTmp3[128]="", iTmp4[128]="", iTmp5[128]="";

			if(BMINum>=BMIMAX){
				print("\n警告!! 物口数目超越范围(%d).", BMIMAX);
				break;
			}
			sscanf( line, "%s %d %s %s %s %s %s", 
				    BMItem[BMINum].iName,
					&BMItem[BMINum].iGraphicsNum,
				    iTmp1, iTmp2, iTmp3, iTmp4,	cTmp);
			
            for(i=0; i<3; i++){
				if(getStringFromIndexWithDelim( iTmp1, ",", i+1, iTmp5, sizeof( iTmp5))!=FALSE)
					BMItem[BMINum].iId[0][i] = atoi(iTmp5);
				if(getStringFromIndexWithDelim( iTmp2, ",", i+1, iTmp5, sizeof( iTmp5))!=FALSE)
					BMItem[BMINum].iId[1][i] = atoi(iTmp5);
				if(getStringFromIndexWithDelim( iTmp3, ",", i+1, iTmp5, sizeof( iTmp5))!=FALSE)
					BMItem[BMINum].iId[2][i] = atoi(iTmp5);
				if(getStringFromIndexWithDelim( iTmp4, ",", i+1, iTmp5, sizeof( iTmp5))!=FALSE)
					BMItem[BMINum].iId[3][i] = atoi(iTmp5);
			}

			ip = strstr( cTmp, "I");
			gp = strstr( cTmp, "G");	

			if( ip && gp && gp>ip){
				strncpy( cTmp1, ip+1, gp-ip-1);				
				for(i=0; i<4; i++)
					if(getStringFromIndexWithDelim( cTmp1, ",", i+1, cTmp3, sizeof( cTmp3))!=FALSE)
						BMItem[BMINum].iCondition[i] = atoi(cTmp3);			
				strcpy( cTmp2, gp+1);
				BMItem[BMINum].GCondition = atoi(cTmp2);	
			}else if( ip && gp && gp<ip){
				strcpy( cTmp1, ip+1);				
				for(i=0; i<4; i++)
					if(getStringFromIndexWithDelim( cTmp1, ",", i+1, cTmp3, sizeof( cTmp3))!=FALSE)
						BMItem[BMINum].iCondition[i] = atoi(cTmp3);				
				strncpy( cTmp2, gp+1, ip-gp-1);
				BMItem[BMINum].GCondition = atoi(cTmp2);
			}else if( gp && !ip){
				strcpy( cTmp2, gp+1);
				BMItem[BMINum].GCondition = atoi(cTmp2);
			}else if( !gp && ip){
				strcpy( cTmp1, ip+1);
				for(i=0; i<4; i++)
					if(getStringFromIndexWithDelim( cTmp1, ",", i+1, cTmp3, sizeof( cTmp3))!=FALSE)
						BMItem[BMINum].iCondition[i] = atoi(cTmp3);
			}else{
				print("\n警告!! 这些不是物品和石币条件...");
				continue;
			}
			BMINum++;
		}
	}
	for(i=0; i<12; i++)	BMSellList[i] = RAND(0, BMINum-1);		
	fclose(fp);
	return TRUE;
}
#endif

#ifdef _GMRELOAD
BOOL LoadGMSet( char* filename )
{
	FILE* fp;
	int i = 0, gm_num = 0;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	for (i = 0; i < GMMAXNUM; i++)
	{
		strcpy(gminfo[i].cdkey, "");
		gminfo[i].level = 0;
	}
	while(1){
		char	line[64], cdkey[64], level[64];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);
		//change 使gmset.txt可以增加注解*******
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		gm_num = gm_num + 1;
		if (gm_num > GMMAXNUM)	break;
		easyGetTokenFromString(line, 1, cdkey, sizeof(cdkey));
		if (strcmp(cdkey, "") == 0)	break;
		strncpy(gminfo[gm_num].cdkey, cdkey, sizeof(gminfo[gm_num].cdkey));
		easyGetTokenFromString(line, 2, level, sizeof(level));
		if (strcmp(level, "") == 0)	break;
		gminfo[gm_num].level = atoi(level);
//		print("\ncdkey:%s, level:%d", gminfo[gm_num].cdkey, gminfo[gm_num].level);
	}
	fclose(fp);
	return TRUE;
}
#endif

/*------------------------------------------------------------
 * 皿夫弘仿丞  毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  cahr*
 ------------------------------------------------------------*/
char* getProgname( void )
{
    return config.progname;
}
/*------------------------------------------------------------
 * configfilename 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getConfigfilename( void )
{
    return config.configfilename;
}
/*------------------------------------------------------------
 * configfilename 毛涩烂允月［
 * 娄醒
 *  newv    char*   蕙仄中袄
 * 忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
void setConfigfilename( char* newv )
{
    strcpysafe( config.configfilename, sizeof( config.configfilename ),
                newv );
}

/*------------------------------------------------------------
 * 犯田永弘伊矛伙毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getDebuglevel( void )
{
    return config.debuglevel;
}
/*------------------------------------------------------------
 * 犯田永弘伊矛伙毛涩烂允月
 * 娄醒
 *  newv    int     蕙仄中袄
 * 忒曰袄
 *  unsigned int    樯及袄
 ------------------------------------------------------------*/
unsigned int setDebuglevel( unsigned int newv )
{
    int old;
    old = config.debuglevel;
    config.debuglevel = newv;
    return old;
}
/*------------------------------------------------------------
 * memoryunit 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getMemoryunit( void )
{
    return config.usememoryunit;
}
/*------------------------------------------------------------
 * memoryunitnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getMemoryunitnum( void )
{
    return config.usememoryunitnum;
}

/*------------------------------------------------------------
 * 失市它件玄扔□田及失玉伊旦毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char*   getAccountservername( void )
{
    return config.asname;
}
/*------------------------------------------------------------
 * 失市它件玄扔□田及禾□玄毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
unsigned short   getAccountserverport( void )
{
    return config.acservport;
}
/*------------------------------------------------------------
 * 失市它件玄扔□田尺及由旦伐□玉毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
char*   getAccountserverpasswd( void )
{
    return config.acpasswd;
}
/*------------------------------------------------------------
 * 失市它件玄扔□田井日苇尹月必□丞扔□田午仄化及  蟆毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
char*   getGameservername( void )
{
    return config.gsnamefromas;
}

// Arminius 7.24 manor pk
char* getGameserverID( void )
{
    if (config.gsid[strlen(config.gsid)-1]=='\n')
      config.gsid[strlen(config.gsid)-1]='\0';
      
    return config.gsid;
}

#ifdef _SERVICE
// Terry 2001/10/03
char* getApID(void)
{
  return config.apid;
}

unsigned short getApPort(void)
{
  return config.apport; 
}

int getLoopTime(void)
{
  return config.looptime;
}

int getEnableService(void)
{
  return config.enableservice;
}
#endif
  
unsigned short getAllowManorPK( void )
{
    return config.allowmanorpk;
}

unsigned short getPortnumber( void )
{
    return config.port;
}
/*------------------------------------------------------------
 * 必□丞扔□田及  寞   寞毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
int getServernumber( void )
{
    return config.servernumber;
}
/*------------------------------------------------------------
 * reuseaddr 及袄毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
int getReuseaddr( void )
{
    return config.reuseaddr;
}

int getNodelay( void )
{
    return config.do_nodelay;
}
int getLogWriteTime(void)
{
    return config.log_write_time;
}
int getLogIOTime( void)
{
    return config.log_io_time;
}
int getLogGameTime(void)
{
    return config.log_game_time;
}
int getLogNetloopFaster(void)
{
    return config.log_netloop_faster;
}

/*------------------------------------------------------------
 * saacwritenum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
int getSaacwritenum( void )
{
    return config.saacwritenum;
}
/*------------------------------------------------------------
 * saacwritenum 毛涩烂允月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
void setSaacwritenum( int num )
{
    config.saacwritenum = num;
}
/*------------------------------------------------------------
 * saacreadnum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
int getSaacreadnum( void )
{
    return config.saacreadnum;
}
/*------------------------------------------------------------
 * saacreadnum 毛涩烂允月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
void setSaacreadnum( int num )
{
    config.saacreadnum = num;
}
/*------------------------------------------------------------
 * fdnum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getFdnum( void )
{
    return config.fdnum;
}
/*------------------------------------------------------------
 * petcharanum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getPetcharnum( void )
{
    return config.petcharnum;
}


/*------------------------------------------------------------
 * othercharnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getOtherscharnum( void )
{
    return config.othercharnum;
}

/*------------------------------------------------------------
 * objnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getObjnum( void )
{
    return config.objnum;
}

/*------------------------------------------------------------
 * itemnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getItemnum( void )
{
    return config.itemnum;
}


/*------------------------------------------------------------
 * battlenum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getBattlenum( void )
{
    return config.battlenum;
}

#ifdef _GET_BATTLE_EXP
unsigned int getBattleexp( void )
{
    return config.battleexp;
}
#endif
/*------------------------------------------------------------
 * topdir 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getTopdir( void )
{
    return config.topdir;
}
/*------------------------------------------------------------
 * mapdir 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getMapdir( void )
{
    return config.mapdir;
}
/*------------------------------------------------------------
 * maptilefile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getMaptilefile( void )
{
    return config.maptilefile;
}
/*------------------------------------------------------------
 * battlemapfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getBattleMapfile( void )
{
    return config.battlemapfile;
}
/*------------------------------------------------------------
 * itemfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getItemfile( void )
{
    return config.itemfile;
}
/*------------------------------------------------------------
 * invfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getInvfile( void )
{
    return config.invfile;
}
/*------------------------------------------------------------
 * appearfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getAppearfile( void )
{
    return config.appearfile;
}
/*------------------------------------------------------------
 * effectfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEffectfile( void )
{
    return config.effectfile;
}
/*------------------------------------------------------------
 * titlenamefile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getTitleNamefile( void )
{
    return config.titlenamefile;
}
/*------------------------------------------------------------
 * titleconfigfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getTitleConfigfile( void )
{
    return config.titleconfigfile;
}
/*------------------------------------------------------------
 * encountfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEncountfile( void )
{
    return config.encountfile;
}
/*------------------------------------------------------------
 * enemyfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEnemyfile( void )
{
    return config.enemyfile;
}
/*------------------------------------------------------------
 * enemybasefile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEnemyBasefile( void )
{
    return config.enemybasefile;
}
/*------------------------------------------------------------
 * groupfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getGroupfile( void )
{
    return config.groupfile;
}
/*------------------------------------------------------------
 * magicfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getMagicfile( void )
{
    return config.magicfile;
}

#ifdef __ATTACK_MAGIC

/*------------------------------------------------------------
 * 取得攻击性的咒术
 * 参数
 * None
 * 返回值
 * char*
 ------------------------------------------------------------*/
char* getAttMagicfileName( void )
{
    return config.attmagicfile;
}

#endif


char* getPetskillfile( void )
{
    return config.petskillfile;
}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
char* getProfession( void )
{
    return config.profession;
}
#endif

#ifdef _ITEM_QUITPARTY
char* getitemquitparty( void )
{
    return config.itemquitparty;
}
#endif

char *getItematomfile( void )
{
    return config.itematomfile;
}


char* getQuizfile( void )
{
    return config.quizfile;
}

/*------------------------------------------------------------
 * lsgenlogfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getLsgenlogfilename( void )
{
    return config.lsgenlog;
}

#ifdef _BLACK_MARKET
char* getBMItemFile(void){
	return config.blackmarketfile;
}
#endif

#ifdef _GMRELOAD
char* getGMSetfile( void )
{
    return config.gmsetfile;
}
#endif

/*------------------------------------------------------------
 * storedir 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getStoredir( void )
{
    return config.storedir;
}
#ifdef _STORECHAR
/*------------------------------------------------------------
 ------------------------------------------------------------*/
char* getStorechar( void )
{
    return config.storechar;
}
#endif

#ifdef _AUCTIONEER
char* getAuctiondir(void)
{
  return config.auctiondir;
}
#endif

/*------------------------------------------------------------
 * NPC 迕及犯奴伊弁玄伉毛  化仁月楮醒
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getNpcdir( void )
{
    return config.npcdir;
}
/*------------------------------------------------------------
 * 夫弘犯奴伊弁玄伉毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getLogdir( void )
{
    return config.logdir;
}

/*------------------------------------------------------------
 * 夫弘涩烂白央奶伙  毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getLogconffile( void )
{
    return config.logconfname;
}
/*------------------------------------------------------------
 * 民乓永玄  芊由旦伐□玉 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getChatMagicPasswd( void )
{
    return config.chatmagicpasswd;
}
/*------------------------------------------------------------
 * 犯田永弘民乓永玄  芊匹及CDKEY民尼永弁毛允月井升丹井毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
unsigned getChatMagicCDKeyCheck( void )
{
    return config.chatmagiccdkeycheck;
}

/*------------------------------------------------------------
 * filesearchnum毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getFilesearchnum( void )
{
    return config.filesearchnum;
}
/*------------------------------------------------------------
 * npctemplatenum毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getNpctemplatenum( void )
{
    return config.npctemplatenum;
}
/*------------------------------------------------------------
 * npccreatenum毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getNpccreatenum( void )
{
    return config.npccreatenum;
}

/*------------------------------------------------------------
 * walksendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getWalksendinterval( void )
{
    return config.walksendinterval;
}
/*------------------------------------------------------------
 * walksendinterval毛本永玄允月［
 * 娄醒
 *  unsigned int 	interval	凛棉  立伉    
 * 忒曰袄
 *  void
 ------------------------------------------------------------*/
void setWalksendinterval( unsigned int interval )
{
    config.walksendinterval = interval;
}
/*------------------------------------------------------------
 * CAsendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCAsendinterval_ms( void )
{
    return config.CAsendinterval_ms;
}
/*------------------------------------------------------------
 * CAsendinterval毛本永玄允月［
 * 娄醒
 *  unsigned int interval
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
void setCAsendinterval_ms( unsigned int interval_ms )
{
    config.CAsendinterval_ms = interval_ms;
}
/*------------------------------------------------------------
 * CDsendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCDsendinterval_ms( void )
{
    return config.CDsendinterval_ms;
}
/*------------------------------------------------------------
 * CDsendinterval毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setCDsendinterval_ms( unsigned int interval_ms )
{
	config.CDsendinterval_ms = interval_ms;
}
/*------------------------------------------------------------
 * Onelooptime毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getOnelooptime_ms( void )
{
    return config.Onelooptime_ms;
}
/*------------------------------------------------------------
 * Onelooptime毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setOnelooptime_ms( unsigned int interval_ms )
{
	config.Onelooptime_ms = interval_ms;
}
/*------------------------------------------------------------
 * Petdeletetime毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getPetdeletetime( void )
{
    return config.Petdeletetime;
}
/*------------------------------------------------------------
 * Petdeletetime毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setPetdeletetime( unsigned int interval )
{
	config.Petdeletetime = interval;
}
/*------------------------------------------------------------
 * Itemdeletetime毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getItemdeletetime( void )
{
    return config.Itemdeletetime;
}
/*------------------------------------------------------------
 * Itemdeletetime毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setItemdeletetime( unsigned int interval )
{
	config.Itemdeletetime = interval;
}

/*------------------------------------------------------------
 * CharSavesendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCharSavesendinterval( void )
{
    return config.CharSavesendinterval;
}
/*------------------------------------------------------------
 * CharSavesendinterval毛本永玄允月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
void setCharSavesendinterval( unsigned int interval)
{
	config.CharSavesendinterval = interval;
}

/*------------------------------------------------------------
 * Addressbookoffmsgnum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getAddressbookoffmsgnum( void )
{
    return config.addressbookoffmsgnum;
}
/*------------------------------------------------------------
 * Protocolreadfrequency 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getProtocolreadfrequency( void )
{
    return config.protocolreadfrequency;
}

/*------------------------------------------------------------
 * Allowerrornum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getAllowerrornum( void )
{
    return config.allowerrornum;
}

/*------------------------------------------------------------
 * 夫弘毛潸  允月凛对毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getLogHour( void )
{
    return config.loghour;
}

/*------------------------------------------------------------
 * 田玄伙  及犯田永弘丢永本□斥毛请允井［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int 1卅日请允
 ------------------------------------------------------------*/
unsigned int getBattleDebugMsg( void )
{
    return config.battledebugmsg;
}
/*------------------------------------------------------------
 * 田玄伙  及犯田永弘丢永本□斥毛请允井［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int 1卅日请允
 ------------------------------------------------------------*/
void setBattleDebugMsg( unsigned int num )
{
    config.battledebugmsg = num;
}



/*
 * Config及犯白巧伙玄袄毛瑁户月楮醒
 * 娄醒
 *  argv0   char*   戊穴件玉仿奶件娄醒及  赓
 */
void  defaultConfig( char* argv0 )
{
    char* program;                  /* program  毛菲户月及卞银丹 */

    /* 犯白巧伙玄袄毛  木月 */

    /*皿夫弘仿丞  */
    program = rindex(argv0, '/');
    if (program == NULL)
        program = argv0;
    else
        program++;   /* "/"及戚井日卞仄凶中及匹++允月*/
    strcpysafe( config.progname , sizeof( config.progname ) ,program );

    /*涩烂白央奶伙  */
    strcpysafe( config.configfilename,
                sizeof( config.configfilename ),"setup.cf" );

}

/*
 * 涩烂白央奶伙  毛  氏分  匹及质  毛垫丹［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  卅仄
 */
void lastConfig( void )
{
    char    entry[256];
    /*  穴永皿犯奴伊弁玄伉及涩烂    */
    snprintf(entry, sizeof(entry), "%s/%s", config.topdir, config.mapdir);
    strcpysafe(config.mapdir, sizeof(config.mapdir), entry);

    /*  穴永皿涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry),"%s/%s",
             config.topdir,config.maptilefile);
    strcpysafe(config.maptilefile, sizeof(config.maptilefile), entry);

    /*  田玄伙穴永皿涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry),"%s/%s",
             config.topdir,config.battlemapfile);
    strcpysafe(config.battlemapfile, sizeof(config.battlemapfile), entry);

    /*  失奶  丞涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.itemfile);
    strcpysafe(config.itemfile, sizeof(config.itemfile), entry);

    /*    衬涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.invfile);
    strcpysafe(config.invfile, sizeof(config.invfile), entry);

    /*  请蜇匏  涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.appearfile);
    strcpysafe(config.appearfile, sizeof(config.appearfile), entry);

    /*  梢请涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.effectfile);
    strcpysafe(config.effectfile, sizeof(config.effectfile), entry);

    /*  弁奶术涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.quizfile);
    strcpysafe(config.quizfile, sizeof(config.quizfile), entry);

    /*  惫寞  白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.titlenamefile);
    strcpysafe(config.titlenamefile, sizeof(config.titlenamefile), entry);

    /*  lsgen 失它玄皿永玄白央奶伙      */
    snprintf(entry,sizeof(entry),"%s/%s", config.topdir,config.lsgenlog);
    strcpysafe(config.lsgenlog, sizeof(config.lsgenlog), entry);

    /*  旦玄失犯奴伊弁玄伉及涩烂    */
/*
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.storedir);
    strcpysafe(config.storedir, sizeof(config.storedir), entry);
*/
    /*  NPC涩烂玄永皿犯奴伊弁玄伉及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.npcdir);
    strcpysafe(config.npcdir, sizeof(config.npcdir), entry);

#ifdef _STORECHAR
    /*   */
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.storechar);
    strcpysafe(config.storechar, sizeof(config.storechar), entry);
#endif

}


/*
 * 禾奶件正□午赝濠卅滇树  毛  匀化医  毛允月楮醒
 * 娄醒
 *  to      void*   袄毛医  允月禾奶件正
 *  type    CTYPE   to及滇毛瑁户月
 *  value   double  to卞医  允月袄
 * 忒曰袄
 *  卅仄
 */
void substitutePointerFromType( void* to , CTYPE type ,double value)
{
    switch( type  ){
    case CHAR:
        *(char*)to = (char)value;
        break;
    case SHORT:
        *(short*)to = (short)value;
        break;
    case INT:
        *(int*)to = (int)value;
        break;
    case DOUBLE:
        *(double*)to = (double)value;
        break;
    }
}


/*------------------------------------------------------------
 * 涩烂白央奶伙毛  戈
 * 娄醒
 *      filename            白央奶伙  
 * 忒曰袄
 *      TRUE(1)     岳  
 *      FALSE(0)    撩      -> 白央奶伙及左□皿件卞撩  仄凶
 ------------------------------------------------------------*/
BOOL readconfigfile( char* filename )
{
    FILE* f=NULL;
    char linebuf[256];                  /* 域垫  心  戈田永白央 */
    int linenum=0;                      /* 垫醒毛醒尹月 */
    char    realopenfilename[256];      /*    端卞open 允月白央奶伙  */

    char    hostname[128];

    /*  石旦玄  毛菲户月    */
    if( gethostname( hostname, sizeof(hostname) ) != -1 ){
        char*   initdot;
        initdot = index( hostname, '.' );
        if( initdot != NULL )
            *initdot = '\0';
        snprintf( realopenfilename, sizeof(realopenfilename),
                  "%s.%s" , filename, hostname);

        /* 白央奶伙及左□皿件 */
        f=fopen( realopenfilename, "r" );
        if( f == NULL )
            print( "Can't open %s.  use %s instead\n", realopenfilename,
                   filename );
    }
    if( f == NULL ){
        f=fopen( filename , "r" );          /* 白央奶伙及左□皿件 */
        if( f == NULL ){
            print( "Can't open %s\n", filename );
            return FALSE;
        }
    }

    /* 域垫勿勾  心  戈 */
    while( fgets( linebuf , sizeof( linebuf ), f ) ){
        char firstToken[256];       /*1    及  侬  */
        int i;                      /*伙□皿  醒*/
        int ret;                    /*伉正□件戊□玉*/

        linenum ++;

        deleteWhiteSpace(linebuf);          /* remove whitespace    */

        if( linebuf[0] == '#' )continue;        /* comment */
        if( linebuf[0] == '\n' )continue;       /* none    */

        chomp( linebuf );                    /* remove tail newline  */

        /* delim "=" 匹  赓(1)及玄□弁件毛  月*/
        ret = getStringFromIndexWithDelim( linebuf , "=",  1, firstToken,
                                           sizeof(firstToken) );
        if( ret == FALSE ){
            print( "Find error at %s in line %d. Ignore\n",
                     filename , linenum);
            continue;
        }

        /* readconf 及扔奶术匹伙□皿 */
        for( i = 0 ; i < arraysizeof( readconf ) ; i ++ ){
            if( strcmp( readconf[i].name ,firstToken ) == 0 ){
                /* match */
                char secondToken[256];      /*2    及  侬  */
                /* delim "=" 匹2    及玄□弁件毛  月*/
                ret = getStringFromIndexWithDelim( linebuf , "=" , 2
                                                   , secondToken ,
                                                   sizeof(secondToken) );

                /* NULL  侬井升丹井毛譬屯月 */
                if( ret == FALSE ){
                    print( "Find error at %s in line %d. Ignore",
                           filename , linenum);
                    break;
                }


                /*NULL分匀凶日医  仄卅中*/
                if( readconf[i].charvalue != NULL )
                    strcpysafe( readconf[i].charvalue
                                ,readconf[i].charsize, secondToken);

                /*NULL分匀凶日医  仄卅中*/
                if( readconf[i].value != NULL ) {
                    if( strcmp( "ON" ,secondToken ) == 0 ) {
                        /*ON分匀凶日1毛  木月*/
                        substitutePointerFromType( readconf[i].value,
                                                   readconf[i].valuetype,
                                                   1.0);

                    }else if( strcmp( "OFF" ,secondToken ) == 0 ) {
                        /*OFF分匀凶日1毛  木月*/
                        substitutePointerFromType( readconf[i].value,
                                                   readconf[i].valuetype,
                                                   1.0);
                    }else {
                        strtolchecknum(secondToken,
                                       (int*)readconf[i].value,
                                       10, readconf[i].valuetype);
					}
				}
                break;
            }
        }
    }
    fclose( f );
    lastConfig();
    return TRUE;
}
//ttom add this becaus the second had this function 
/*------------------------------------------------------------
 * 巨件戊□玉平□毛涩烂允月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int 平□毛忒允
------------------------------------------------------------*/
unsigned int setEncodeKey( void )
{
   JENCODE_KEY = config.encodekey;
   return JENCODE_KEY;
}
/*------------------------------------------------------------
* 失市它件玄扔□田□卞踏五  戈田永白央及扔奶术毛涩烂允月
* 娄醒
*  卅仄
* 忒曰袄
*  unsigned int 平□毛忒允
------------------------------------------------------------*/
unsigned int setAcWBSize( void )
{
    AC_WBSIZE = config.acwbsize;
    return AC_WBSIZE;
}
unsigned int getAcwriteSize( void )
{
    return config.acwritesize;
}
unsigned int getErrUserDownFlg( void )
{
    return config.ErrUserDownFlg;
}
    
    
//tom end


#ifdef _M_SERVER
char* getmservername(void)
{
	return config.msname;
}
void setmservername( char *msname)
{
	sprintf( config.msname, "%s", msname);
}
unsigned int getmserverport(void)
{
	return config.msport;
}
void setmserverport( int port)
{
	config.msport = port;
}
#endif

#ifdef _NPCSERVER_NEW
char *getnpcserveraddr(void)
{
	return config.nsaddress;
}

unsigned int getnpcserverport(void)
{
	return config.nsport;
}
void NS_setAddressAndPort( char *address, int nport)
{
	sprintf( config.nsaddress, "%s", address);
	config.nsport = nport;
}
#endif

#ifdef _MUSEUM
int   getMuseum( void )
{
    return config.museum;
}
#endif

#ifdef _DEL_DROP_GOLD
unsigned int getGolddeletetime( void )
{
    if( config.Golddeletetime > 0 )
		return config.Golddeletetime;
	else
		return config.Itemdeletetime;
}
void setIGolddeletetime( unsigned int interval )
{
	config.Golddeletetime = interval;
}
#endif

#ifdef _NEW_PLAYER_CF
int getNewplayertrans( void )
{
  if(config.newplayertrans > 7)
		return 7;
	else if(config.newplayertrans >= 0)
		return config.newplayertrans;
	else
		return 0;
}
int getNewplayerlv( void )
{
  if(config.newplayerlv > 160)
		return 160;
	else if(config.newplayerlv >0)
		return config.newplayerlv;
	else
		return 0;
}
int getNewplayerpetlv( void )
{
  if(config.newplayerpetlv > 160)
		return 160;
	else if(config.newplayerpetlv > 0)
		return config.newplayerpetlv;
	else
		return 0;
}

int getNewplayergivepet( unsigned int index )
{
  if(config.newplayergivepet[index] > 0 )
		return config.newplayergivepet[index];
	else
		return -1;
}

int getNewplayergiveitem( unsigned int index )
{
  if(config.newplayergiveitem[index] > 0 )
		return config.newplayergiveitem[index];
	else
		return -1;
}

void setNewplayergivepet( unsigned int index ,unsigned int interval)
{
	config.newplayergivepet[index] = interval;
}

unsigned int getNewplayergivegold( void )
{
  if(config.newplayergivegold > 1000000)
		return 1000000;
	else if(config.newplayergivegold >= 0)
		return config.newplayergivegold;
	else
		return 0;
}

#endif

#ifdef _UNLAW_WARP_FLOOR
int getUnlawwarpfloor( unsigned int index )
{
  if(config.unlawwarpfloor[index] > 0 )
		return config.unlawwarpfloor[index];
	else
		return -1;
}
#endif


#ifdef _ANGEL_SUMMON

extern int mission_num;

BOOL LoadMissionList( )
{
	FILE* fp;
	int i = 0;

	mission_num = 0;

	fp = fopen("./data/mission.txt", "r");
	if (fp == NULL)
	{
		print("任务文件打开错误\n");
		return FALSE;
	}
	
	memset( missionlist, 0, sizeof(missionlist));

	while(1){
		char	line[1024], level[64];
		char	token[1024];
		int		mindex;
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		print("\n %s ", line);
		chop(line);
		// 以#为注解*******
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************

		//格式 #任务编号,必要等级,任务说明,奖品ID,限制时间(小时)
		
		getStringFromIndexWithDelim(line, ",", 1, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		mindex = atoi( token);

		if( mindex <= 0 || mindex >= MAXMISSION) 
			break;

		missionlist[mindex].id = mindex;

		getStringFromIndexWithDelim(line, ",", 2, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missionlist[mindex].level = atoi( token);

		getStringFromIndexWithDelim(line, ",", 3, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missionlist[mindex].eventflag, token);

		getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missionlist[mindex].detail, token);

		//getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
		//if (strcmp(token, "") == 0)	break;
		//strcpy( missionlist[mindex].bonus, token);

		getStringFromIndexWithDelim(line, ",", 5, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missionlist[mindex].limittime = atoi( token);

		/*print("\nMISSION[%d] lv:%d ef:%s detail:%s limit:%d ", mindex,
			missionlist[mindex].level, missionlist[mindex].eventflag,
			missionlist[mindex].detail, missionlist[mindex].limittime );
		*/
		mission_num++;
		//if (mission_num > MAXMISSION)	break;
	}
	fclose(fp);
	return TRUE;
}



BOOL LoadMissionCleanList( )
{
	// 格式... 使者,勇者,任务,奖赏
	FILE* fp;
	int	listindex =0;
	int i = 0;

	memset( missiontable, 0, sizeof(missiontable));
	fp = fopen("./data/missionclean.txt", "r");
	if (fp == NULL)
	{
		print("清除任务文件打开错误\n");
		return FALSE;
	}

	while(1){
		char	line[1024], angelinfo[128], heroinfo[128];
		char	token[1024];
		
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		print("\n %s ", line);
		chop(line);
		// 以#为注解*******
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		
		getStringFromIndexWithDelim(line, ",", 1, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missiontable[listindex].angelinfo, token);

		getStringFromIndexWithDelim(line, ",", 2, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missiontable[listindex].heroinfo, token);

		getStringFromIndexWithDelim(line, ",", 3, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missiontable[listindex].mission = atoi( token);
		
		getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
		//if (strcmp(token, "") == 0)	break;
		missiontable[listindex].flag = atoi( token);

		getStringFromIndexWithDelim(line, ",", 5, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missiontable[listindex].time = atoi( token);

		print("\nMISSIONCLEAN[%d] %s %s %d %d %d", listindex,
			missiontable[listindex].angelinfo,
			missiontable[listindex].heroinfo,
			missiontable[listindex].mission,
			missiontable[listindex].flag,
			missiontable[listindex].time );

		listindex++;
		if ( listindex >= MAXMISSIONTABLE)	break;
	}
	fclose(fp);
	return TRUE;
}


#endif

#ifdef _JOBDAILY
extern  DailyFileType dailyfile[MAXDAILYLIST];
BOOL LoadJobdailyfile(void)
{
	char	line[20000];
	char	token[16384];
	int		listindex =0;
	int     i;
	FILE* fp;

	fp = fopen("./data/jobdaily.txt", "r");
	if (fp == NULL)
	{
		print("日常工作文件打开错误\n");
		return FALSE;
	}

	memset( dailyfile, 0, sizeof(dailyfile));

	while(1){		
		line[0]='\0';	
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		//print("\n %s ", line);
		chop(line);

		// #为注解
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		
		getStringFromIndexWithDelim(line, "|", 1, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( dailyfile[listindex].jobid, token);

		getStringFromIndexWithDelim(line, "|", 2, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( dailyfile[listindex].rule, token);

		getStringFromIndexWithDelim(line, "|", 3, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		if(strlen(token)>96){
			print("任务说明过长:%d",strlen(token));
			return FALSE;
		}
		strcpy( dailyfile[listindex].explain, token);
		
		getStringFromIndexWithDelim(line, "|", 4, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( dailyfile[listindex].state, token);

		/*print("\ndailyfile[%d] %s %s %s %s", listindex,
			dailyfile[listindex].jobid,
			dailyfile[listindex].rule,
			dailyfile[listindex].explain,
			dailyfile[listindex].state);	
		*/
		listindex++;
		if ( listindex >= MAXDAILYLIST)	break;
	}
	fclose(fp);
	return TRUE;
}
#endif

#ifdef _RACEMAN
//extern int petflgtable[640];
//extern struct ASKTABLE asktable[400];
extern int asktotal;
BOOL LoadRacepetfile(void)
{
	char line[1000];
	char token[64];
	int  i;
	FILE* fp;

	asktotal = 0;
	memset(asktable,0,sizeof(asktable));

	fp = fopen("./data/raceman.txt", "r");
	if (fp == NULL)
	{
		print("宠物赛跑文件打开错误\n");
		return FALSE;
	}
	
	while(1){		
		line[0]='\0';	
		if (fgets(line, sizeof(line), fp) == NULL)	break;		
		chop(line);

		// #为注解
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		getStringFromIndexWithDelim(line, "|", 1 , token, sizeof(token));
		if( atoi(token) <= 0 )	return FALSE;
		asktable[asktotal].no = atoi(token);

		getStringFromIndexWithDelim(line, "|", 2 , token, sizeof(token));
		if( strlen(token) == 0 ) return FALSE;
		strcpy( asktable[asktotal].petname , token );

		getStringFromIndexWithDelim(line, "|", 3 , token, sizeof(token));
		if( atoi(token) <= 0 )	return FALSE;
		asktable[asktotal].bbi = atoi(token);

		getStringFromIndexWithDelim(line, "|", 4 , token, sizeof(token));
		if( atoi(token) <= 0 )	return FALSE;
		asktable[asktotal].lowlv = atoi(token);

		getStringFromIndexWithDelim(line, "|", 5 , token, sizeof(token));
		if( atoi(token) < 0 || atoi(token) > 140 )	return FALSE;
		asktable[asktotal].highlv = atoi(token);
		asktotal++;
	}
	fclose(fp);
	return TRUE;

	/*
	for( k=0,j=0;j<ENEMYTEMP_enemynum;j++ ) {
		if( ENEMYTEMP_getInt( j, E_T_PETFLG ) == 1 ){
			petflgtable[k] = ENEMYTEMP_getInt(j,E_T_IMGNUMBER) ;
			k++;
			if(k>=640) {
				print( "Valid petflag Num is %d...", k );
				break;
			}
		}
		//if( CHAR_getInt( petindex, CHAR_PETID) == ENEMYTEMP_getInt( j, E_T_TEMPNO ) ) {//ENEMY_getInt( j, ENEMY_TEMPNO)
	}
	*/
}

#endif


#ifdef _USER_EXP_CF
BOOL LoadEXP( char* filename )
{
	FILE* fp;
	int i = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}

	while(1){
		char	line[64], exp[64];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);

		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		MaxLevel = MaxLevel + 1;
		if (MaxLevel > 160)	break;
		easyGetTokenFromString(line, 2, exp, sizeof(exp));
		NeedLevelUpTbls[MaxLevel]=atoi(exp);
	}
	fclose(fp);
	return TRUE;
}

char* getEXPfile( void )
{
    return config.expfile;
}

int getNeedLevelUpTbls( int level )
{
    return NeedLevelUpTbls[level];
}

int getMaxLevel( void )
{
		if(MaxLevel>160)
			MaxLevel=160;
    return MaxLevel;
}
#endif

#ifdef _UNREG_NEMA
char* getUnregname( int index )
{
    return config.unregname[index];
}
#endif

#ifdef _TRANS_LEVEL_CF
int getChartrans( void )
{
		if(config.chartrans>6)
			config.chartrans=6;
    return config.chartrans;
}
int getPettrans( void )
{
		if(config.pettrans>2)
			config.pettrans=2;
    return config.pettrans;
}
int getYBLevel( void )
{
		if(config.yblevel>MaxLevel)
			config.yblevel=MaxLevel;
    return config.yblevel;
}
#endif

#ifdef _LOCK_IP
char* getLockipPath( void )
{
		return config.lockip;
}
int loadLockip( char* filename )
{
	FILE* fp;
	int i = 0;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	while(1){
		char	line[64], buf[16];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);

		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		getStringFromIndexWithDelim(line,".", 1, buf, sizeof(buf));
		lockip[maxlockip].a=atoi(buf);
		getStringFromIndexWithDelim(line,".", 2, buf, sizeof(buf));
		lockip[maxlockip].b=atoi(buf);
		getStringFromIndexWithDelim(line,".", 3, buf, sizeof(buf));
		lockip[maxlockip].c=atoi(buf);
		getStringFromIndexWithDelim(line,".", 4, buf, sizeof(buf));
		lockip[maxlockip].d=atoi(buf);
		getStringFromIndexWithDelim(line,"|", 2, buf, sizeof(buf));
		lockip[maxlockip].locktime=atoi(buf);
		print( "\n封锁IP %d.%d.%d.%d 登陆%d次\n",
						lockip[maxlockip].a,lockip[maxlockip].b,lockip[maxlockip].c,lockip[maxlockip].d,
						lockip[maxlockip].locktime);
		maxlockip++;
	}
	fclose(fp);
	return TRUE;
}

int saveLockip( void )
{
	FILE* fp;
	char outbuff[32];
	int i;
	fp = fopen(config.lockip, "w");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	for(i=0; i<maxlockip; i++){
		sprintf( outbuff, "%d.%d.%d.%d|%d\n",lockip[i].a,lockip[i].b,lockip[i].c,lockip[i].d, lockip[i].locktime);
		fwrite(outbuff, strlen(outbuff), 1, fp);
	}
	fclose(fp);
	return TRUE;
}

int setLockip( int a, int b ,int c ,int d, int locktime )
{
	if(maxlockip+1>99)return FALSE;
	int i;
	for(i=0;i<maxlockip;i++)
		if(lockip[i].a==a && lockip[i].b==b && lockip[i].c==c && lockip[i].d==d)
		{
			lockip[i].locktime=locktime;
			return TRUE;
		}
	lockip[maxlockip].a=a;
	lockip[maxlockip].b=b;
	lockip[maxlockip].c=c;
	lockip[maxlockip].d=d;
	lockip[maxlockip].locktime=locktime;
	maxlockip++;
	return TRUE;
}

char *dispalyLockip( int index )
{
	char *buf[32];
	sprintf( buf, "锁定IP %d.%d.%d.%d|%d!", 
						lockip[index].a,lockip[index].b,lockip[index].c,lockip[index].d,lockip[index].locktime);
	return buf;
}

int getMaxLockip( void )
{
	return maxlockip;
}

int cmpLockip( int index, int a, int b, int c, int d, int login )
{
	if(lockip[index].a==a)
		if(lockip[index].b==b)
			if(lockip[index].c==c)
				if(lockip[index].d==d){
					if(login==1){
						print( "\n封锁IP %d.%d.%d.%d 最大登陆%d次 目前已登陆%d次\n",
								lockip[index].a,lockip[index].b,lockip[index].c,lockip[index].d,
								lockip[index].locktime,lockip[index].logintime);
						lockip[index].logintime++;
					}else{
						lockip[index].logintime--;
						print( "\n封锁IP %d.%d.%d.%d 最大登陆%d次 目前已登陆%d次\n",
								lockip[index].a,lockip[index].b,lockip[index].c,lockip[index].d,
								lockip[index].locktime,lockip[index].logintime);
					}
					if(lockip[index].logintime>lockip[index].locktime)
						return TRUE;
					else
						return FALSE;
				}else
					return FALSE;
			else
				return FALSE;
		else
			return FALSE;
	else
		return FALSE;
}

#endif