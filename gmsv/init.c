#include "version.h"
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>


#include "configfile.h"
#include "util.h"
#include "net.h"
#include "msignal.h"
#include "buf.h"
#include "object.h"
#include "char.h"
#include "char_data.h"
#include "item.h"
#include "readmap.h"
#include "function.h"
#include "saacproto_cli.h"
#include "lssproto_serv.h"
#include "readnpc.h"
#include "log.h"
#include "handletime.h"
#include "title.h"
#include "encount.h"
#include "enemy.h"
#include "battle.h"
#include "magic_base.h"
#include "pet_skill.h"
#include "item_gen.h"
#include "petmail.h"
#include "npc_quiz.h"


#ifdef _M_SERVER
#include "mclient.h"
#endif

#ifdef _NPCSERVER_NEW
#include "npcserver.h"
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#endif

#ifdef _ITEM_QUITPARTY
#include "init.h"
//int itemquitparty_num = 0;
//static DisappearItem *Disappear_Item;
#endif

#define MESSAGEFILE "hoge.txt"

#define OPTIONSTRING "d:f:h"
#define usage() print( "Usage: %s ["OPTIONSTRING"]\n", getProgname() );

void printUsage( void )
{
    usage();
  /*print( "Usage: %s ["OPTIONSTRING"]\n", progname );*/
    print( "          [-d debuglevel]        default value is 0\n" );
    print( "          [-f configfilename]    default value is setup.cf\n"
        );
}

/*
 * 戊穴件玉仿奶件毛质  允月
 *
 * 娄醒
 *      argc      侬  及醒
 *      argv      侬  及    
 * 忒曰袄
 *      TRUE(1)     恳橘卅戊穴件玉仿奶件娄醒分匀凶日
 *      FALSE(0)    唱橘卅戊穴件玉仿奶件娄醒分匀凶日
 */
BOOL parseCommandLine( int argc , char** argv )
{
    int c;                          /* getopt 匹银丹 */
    extern char* optarg;            /* getopt 匹银丹 */


    while( ( c = getopt( argc, argv ,OPTIONSTRING )) != -1 ){
        switch( c ){
        case 'd':
        {
            int     debuglevel;
            if( !strtolchecknum( optarg, (int*)&debuglevel, 10,
                                 INT)){
                print( "Specify digit number\n" );
                return FALSE;
            }
            setDebuglevel( debuglevel );
            break;
        }
        case 'f':
            setConfigfilename( optarg );
            break;
        case 'h':
            printUsage();
            return FALSE;
            break;

        default:
            printUsage();
            return FALSE;
            break;

        }
    }
    return TRUE;
}


/*
 * 棕雁  醒毛由□旦允月
 * 娄醒
 *      env     棕雁  醒及    
 *
 * 漆及赭窒手仄卅中
 */
BOOL parseEnvironment( char** env )
{
    if( getDebuglevel() >= 3 ){
        int index=0;
        while( env[index] != NULL )print( "%s " , env[index++] );
        print( "\n" );
    }
    return TRUE;
}




/*lsgen迕及伐□平件弘田永白央及  五今*/
#define LSGENWORKINGBUFFER  65536*4



#define GOTORETURNFALSEIFFALSE(x) if(!(x))goto RETURNFALSE
/*
 * 赓渝祭伙□民件
 * 娄醒
 *      argc    argv及醒
 *      argv    戊穴件玉仿奶件娄醒
 *      env     棕雁  醒
 * 忒曰袄
 *      TRUE(1) 岳  
 *      FALSE(1) 撩  
 */
BOOL init(int argc , char** argv , char** env )
{
#ifdef _ITEM_QUITPARTY
    FILE *f;
	int i;
	char line[256];
#endif
    srand( getpid());
    print( "This Program is compiled at %s %s by gcc %s\n",
           __DATE__ , __TIME__ , __VERSION__ );

    defaultConfig( argv[0] );
    signalset();

    GOTORETURNFALSEIFFALSE(parseCommandLine( argc , argv ));
    GOTORETURNFALSEIFFALSE(parseEnvironment( env ));

    {
        Char    aho;
        debug( sizeof( aho ), d);
        debug( sizeof( aho.data ), d);
        debug( sizeof( aho.string ), d);
        debug( sizeof( aho.flg ),d);
        debug( sizeof( aho.indexOfExistItems ), d);
        debug( sizeof( aho.haveSkill ), d);
        debug( sizeof( aho.indexOfHaveTitle ), d);
        debug( sizeof( aho.addressBook ),d);
        debug( sizeof( aho.workint ),d);
        debug( sizeof( aho.workchar ),d);
    }

    print( "配置文件: %s\n" , getConfigfilename() );

    GOTORETURNFALSEIFFALSE(readconfigfile( getConfigfilename() ) );
    //ttom start
    {  int iWork = setEncodeKey();
       if( iWork == 0 ){
       // 巨件戊□玉平□毛涩烂
       printf( "----------------------------------------\n" );
       printf( "-------------[编码] 无法设置 %s\n", getConfigfilename() );
       printf( "----------------------------------------\n" );
       exit( 1 );
       }else{
            // 巨件戊□玉平□毛涩烂
               printf( "编码 = %d\n", iWork );
       }
    }
    // AcWBuff毛涩烂
    {   int iWork = setAcWBSize();
        if( iWork == 0 ){
           printf( "----------------------------------------\n" );
           printf( "-------------[AC缓冲] 无法设置 %s\n", getConfigfilename() );
           printf( "----------------------------------------\n" );
           exit( 1 );
           }else{
                   printf( "AC缓冲 = %d\n", iWork );
           }
    }
    //ttom end

    if( getDebuglevel() >= 1 ){
//		print("ServerType: %d\n", getServerType() );
        print("调试等级: %d\n" , getDebuglevel() );
        print("内存单元: %d\n"  , getMemoryunit() );
        print("内存单元数量: %d\n" , getMemoryunitnum() );

        print("账号服务器地址: %s\n" , getAccountservername() );
        print("账号服务器端口: %d\n" , getAccountserverport() );
        print("登陆服务器名称: %s\n",
              getGameservername());
        print("登陆服务器密码: %s\n", getAccountserverpasswd());

        print("等待连接端口: %d\n",  getPortnumber() );

        print("服务端序列号: %d\n",  getServernumber() );

        print("重复地址使用: %d\n",  getReuseaddr() );


        print("最大在线人数: %d\n",
              getFdnum() );
        print("最大在线宠数: %d\n" , getPetcharnum() );
        print("最大其他数目: %d\n"  , getOtherscharnum() );
        print("最大对象数目: %d\n",  getObjnum() );
        print("最大物品数目: %d\n",  getItemnum() );
        print("最大战斗数目: %d\n",  getBattlenum() );
#ifdef _GET_BATTLE_EXP
				print("战斗经验倍数: %d\n",  getBattleexp() );
#endif
        print("顶层文件目录: %s\n"  , getTopdir());
        print("地图文件目录: %s\n"  , getMapdir());
        print("地图标识文件: %s\n"  , getMaptilefile());
        print("物品配置文件: %s\n"  , getItemfile());
        print("不可战斗文件: %s\n"  , getInvfile());
        print("显示位置文件: %s\n"  , getAppearfile());
        print("遇敌配置文件: %s\n"  , getEffectfile());
        print("头衔名称文件: %s\n"  , getTitleNamefile());
        print("头衔配置文件: %s\n"  , getTitleConfigfile());
        print("遇敌坐标文件: %s\n"  , getEncountfile());
        print("遇敌组群文件: %s\n"  , getGroupfile());
        print("宠物基本文件: %s\n"  , getEnemyBasefile());
        print("创建宠物文件: %s\n"  , getEnemyfile());
        print("精灵魔法文件: %s\n"  , getMagicfile());

#ifdef __ATTACK_MAGIC
        print("攻击魔法文件: %s\n" , getAttMagicfileName() );
#endif

        print("宠物技能文件: %s\n"  , getPetskillfile());

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
        print("职业技能文件: %s\n"  , getProfession());
#endif

        print("物品成份文件: %s\n", getItematomfile());
        print("猜迷问题文件: %s\n", getQuizfile());
#ifdef _BLACK_MARKET
				print("地下市场文件: %s\n", getBMItemFile());
#endif
#ifdef _GMRELOAD
				print("G M 配置文件: %s\n", getGMSetfile());
#endif
        print("日志记录文件: %s\n"  ,  getLsgenlogfilename() );
        print("还原资料目录: %s\n"  , getStoredir());
        print("NPC 配置目录: %s\n"  , getNpcdir());
        print("日志记载文件: %s\n",  getLogdir());
        print("日志配置文件: %s\n", getLogconffile() );
        print("GM的指命密码: %s\n", getChatMagicPasswd() );
        print("使用GM的权限: %d\n", getChatMagicCDKeyCheck() );

        print("NPC 模板数目: %d\n"  , getNpctemplatenum() );
        print("NPC 最大数目: %d\n"  , getNpccreatenum() );

        print("走路时间间隔: %d\n"  , getWalksendinterval());
        print("清除所有间隔: %d\n"  , getCAsendinterval_ms());
        print("清除目标间隔: %d\n"  , getCDsendinterval_ms());
        print("执行一次时间: %d\n"  , getOnelooptime_ms());
        print("宠物清除时间: %d\n"  , getPetdeletetime());
        print("道具清除时间: %d\n"  , getItemdeletetime());
#ifdef _DEL_DROP_GOLD
				print("石器清除时间: %d\n"  , getGolddeletetime());
#endif
        print("数据保存间隔: %d\n"  , getCharSavesendinterval());

        print("名片最大数目: %d\n",getAddressbookoffmsgnum());
        print("读取频率协议: %d\n",getProtocolreadfrequency());

        print("连接错误上限: %d\n",getAllowerrornum());

#ifdef	_M_SERVER
		    print("GM服务器地址: %s\n",getmservername());
		    print("GM服务器端口: %d\n",getmserverport());
#endif
#ifdef _NPCSERVER_NEW
				print("Npc服务器地址: %s\n",getnpcserveraddr());
				print("Npc服务器端口: %d\n",getnpcserverport());
#endif
#ifdef _NEW_PLAYER_CF
				print("出生人物转数: %d\n",getNewplayertrans());
				print("出生人物等级: %d\n",getNewplayerlv());
				print("出生人物金钱: %d\n",getNewplayergivegold());
				print("出生宠物等级: %d\n",getNewplayerpetlv());
				print("出生拥有宠物: NO1:%d NO2:%d NO3:%d NO4:%d NO5:%d\n",getNewplayergivepet(0),
																																	getNewplayergivepet(1),
																																	getNewplayergivepet(2),
																																	getNewplayergivepet(3),
																																	getNewplayergivepet(4));
				print("出生拥有物品: ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n"
							"　　　　　　　ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n"
							"　　　　　　　ITEM1:%d ITEM2:%d ITEM3:%d ITEM4:%d ITEM5:%d\n"
																																	,getNewplayergiveitem(0)
																																	,getNewplayergiveitem(1)
																																	,getNewplayergiveitem(2)
																																	,getNewplayergiveitem(3)
																																	,getNewplayergiveitem(4)
																																	,getNewplayergiveitem(5)
																																	,getNewplayergiveitem(6)
																																	,getNewplayergiveitem(7)
																																	,getNewplayergiveitem(8)
																																	,getNewplayergiveitem(9)
																																	,getNewplayergiveitem(10)
																																	,getNewplayergiveitem(11)
																																	,getNewplayergiveitem(12)
																																	,getNewplayergiveitem(13)
																																	,getNewplayergiveitem(14));
#endif
#ifdef _UNLAW_WARP_FLOOR
		print("禁止传送地图: FLOOR1:%d FLOOR2:%d FLOOR3:%d FLOOR4:%d FLOOR5:%d\n",getUnlawwarpfloor(0),
																																	getUnlawwarpfloor(1),
																																	getUnlawwarpfloor(2),
																																	getUnlawwarpfloor(3),
																																	getUnlawwarpfloor(4));
#endif
#ifdef _UNREG_NEMA
		print("禁止人物名称: NAME1:%s NAME2:%s NAME3:%s NAME4:%s NAME5:%s\n",getUnregname(0),
																																	getUnregname(1),
																																	getUnregname(2),
																																	getUnregname(3),
																																	getUnregname(4));
#endif
#ifdef _TRANS_LEVEL_CF
		print("人物等级转数: %d\n",getChartrans());
		print("宠物等级转数: %d\n",getPettrans());
#endif

#ifdef _LOCK_IP
		print("封锁 IP 设置: %s\n",getLockipPath());
#endif
    }

	{	//andy_add 2003/05/05 check GameServer Name
		char *GameServerName;
		GameServerName = getGameserverID();
		if( GameServerName == NULL || strlen( GameServerName) <= 0 )
			return FALSE;
		print("\n游戏服务器ID: %s\n",  GameServerName );
	}
	
    print("开始初始化\n" );

//#define DEBUG1( arg... ) if( getDebuglevel()>1 ){##arg}
    print( "建立内存空间..." );
    GOTORETURNFALSEIFFALSE(configmem( getMemoryunit(),
                                      getMemoryunitnum() ) );
    GOTORETURNFALSEIFFALSE(memInit());
	print( "完成\n" );
    if( !initConnect(getFdnum()) )
        goto MEMEND;
    while( 1 ){
        print( "尝试绑定本地端口 %d... " , getPortnumber());
        bindedfd = bindlocalhost( getPortnumber() );
        if( bindedfd == -1 )
            sleep( 10 );
        else
            break;

    }
	print( "完成\n" );
	print( "建立对象..." );
    if( !initObjectArray( getObjnum()) )
        goto CLOSEBIND;
	print( "完成\n" );
	print( "建立人物..." );
    if(!CHAR_initCharArray( getFdnum(), getPetcharnum(),getOtherscharnum()) )
        goto CLOSEBIND;
	print( "完成\n" );
	print( "建立物品:%s...", getItemfile());
    if(!ITEM_readItemConfFile( getItemfile()) )
        goto CLOSEBIND;
    if(!ITEM_initExistItemsArray( getItemnum() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "建立战斗..." );
    if(!BATTLE_initBattleArray( getBattlenum() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "建立功能模块..." );
    if( !initFunctionTable() )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "初始化邮件..." );
    if( !PETMAIL_initOffmsgBuffer( getAddressbookoffmsgnum() ))
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取不可战斗文件..." );
    if( !CHAR_initInvinciblePlace( getInvfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取显示位置文件..." );
    if( !CHAR_initAppearPosition( getAppearfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取头衔名称文件..." );
    if( !TITLE_initTitleName( getTitleNamefile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取头衔配置文件..." );
    if( !TITLE_initTitleConfig( getTitleConfigfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取遇敌坐标文件..." );
    if( !ENCOUNT_initEncount( getEncountfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取宠物基本文件..." );
    if( !ENEMYTEMP_initEnemy( getEnemyBasefile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取创建宠物文件..." );
    if( !ENEMY_initEnemy( getEnemyfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

	print( "读取遇敌组群文件..." );
    if( !GROUP_initGroup( getGroupfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );
	print( "读取魔法文件..." );
    if( !MAGIC_initMagic( getMagicfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

		#ifdef __ATTACK_MAGIC

	print( "读取魔法攻击文件..." );

    if( !ATTMAGIC_initMagic( getAttMagicfileName() ) )
//		if( !ATTMAGIC_initMagic( getMagicfile() ) )
        goto CLOSEBIND;

	print( "魔法攻击文件 -->%s..." , getAttMagicfileName());
	print( "完成\n" );

    #endif
 
	print( "读取宠物技能文件..." );
    if( !PETSKILL_initPetskill( getPetskillfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	print( "读取职业技能文件..." );
	if( !PROFESSION_initSkill( getProfession() ) ){
		goto CLOSEBIND;
	}
	print( "完成\n" );
#endif

    /* 失奶  丞及笺    毛  戈 */
	print( "读取物品成份文件..." );
    if( !ITEM_initItemAtom( getItematomfile()) )
        goto CLOSEBIND;
	print("完成\n" );

	print( "初始化物品高速缓冲..." );
    if( !ITEM_initItemIngCache() )
        goto CLOSEBIND;
	print("完成\n" );
    
	print( "初始化物品名称..." );
    if( !ITEM_initRandTable() )
        goto CLOSEBIND;
	print("完成\n" );
    
	print( "读取遇敌配置文件..." );
    if( !CHAR_initEffectSetting( getEffectfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );
	print( "读取猜迷问题文件..." );
    if( !QUIZ_initQuiz( getQuizfile() ) )
        goto CLOSEBIND;
	print( "完成\n" );

#ifdef _BLACK_MARKET
	print( "地下市场文件..." );
	if ( !LoadBMItem(getBMItemFile()) )
		goto CLOSEBIND;
	print( "完成\n" );
#endif

#ifdef _GMRELOAD
	print( "读取GM配置文件..." );
	if ( !LoadGMSet( getGMSetfile() ) )
		goto CLOSEBIND;
	print( "完成\n" );
#endif

#ifdef _GMRELOAD
	print( "读取经验配置文件..." );
	if ( !LoadEXP( getEXPfile() ) )
		goto CLOSEBIND;
	print("最高等级: %d...",getMaxLevel());
	print("一般等级: %d...",getYBLevel());
	print( "完成\n" );
#endif

#ifdef _ANGEL_SUMMON
	print("读取任务列表文件...");
	if( !LoadMissionList( ) )
		goto CLOSEBIND;
	print("完成\n");
#endif

#ifdef _CONTRACT
	print("读取契约文件...");
	//if( !LoadMissionList( ) )
	//	goto CLOSEBIND;
	if( !ITEM_initContractTable() )
		goto CLOSEBIND;
	print("完成\n");
#endif

#ifdef _JOBDAILY
	print("读取任务日志文件...");
	if(!LoadJobdailyfile())
		goto CLOSEBIND;
	print("完成\n");
#endif

#ifdef _RACEMAN
	print("读取宠物赛跑文件...");
	if(!LoadRacepetfile())
		goto CLOSEBIND;
	print("完成\n");
#endif

#ifdef _LOCK_IP
	print("读取锁定IP文件...");
	if(!loadLockip( getLockipPath()))
		goto CLOSEBIND;
	print("完成\n");
#endif

	print( "建立地图..." );
    if( !MAP_initReadMap( getMaptilefile() , getMapdir() ))
        goto CLOSEBIND;
	print( "完成\n" );
	print( "读取NPC文件..." );
    if( !NPC_readNPCSettingFiles( getNpcdir(), getNpctemplatenum(),
                                  getNpccreatenum() ) )
        goto CLOSEBIND;
	print( "完成\n" );
	print( "初始化 NPC 服务器... " );
    if( lssproto_InitServer( lsrpcClientWriteFunc, LSGENWORKINGBUFFER ) < 0 )
        goto CLOSEBIND;
	print( "完成\n" );
	print( "尝试连接账号服务器... " );
    acfd = connectHost( getAccountservername(), getAccountserverport());
    if(acfd == -1)
        goto CLOSEBIND;

/*
	{
		int errorcode;
		int errorcodelen;
		int qs;

		errorcodelen = sizeof(errorcode);
		qs = getsockopt( acfd, SOL_SOCKET, SO_RCVBUF , &errorcode, &errorcodelen);
		//andy_log
		print("\n\n GETSOCKOPT SO_RCVBUF: [ %d, %d, %d] \n", qs, errorcode, errorcodelen);
	}
*/

	print( "完成\n" );
    initConnectOne( acfd, NULL , 0 );
    if( !CONNECT_acfdInitRB( acfd)) goto CLOSEAC;
    if( !CONNECT_acfdInitWB( acfd)) goto CLOSEAC;
    CONNECT_setCtype( acfd, AC );
	
	print( "初始化 NPC 客户端 ... " );
    /*  rpc(client)及赓渝祭 */
    if( saacproto_InitClient( lsrpcClientWriteFunc,LSGENWORKINGBUFFER, acfd) < 0 )
        goto CLOSEAC;
	print( "完成\n" );

	print( "向账号服务器发送登陆请求... " );
    /*  夫弘奶件邰菲毛请允  */
    saacproto_ACServerLogin_send(acfd, getGameservername(),
                                 getAccountserverpasswd());
	print( "完成\n" );

    if( isExistFile( getLsgenlogfilename() ) ){
        lssproto_SetServerLogFiles( getLsgenlogfilename(),
                                    getLsgenlogfilename() );
        saacproto_SetClientLogFiles( getLsgenlogfilename(),
                                     getLsgenlogfilename() );
    }


	print( "初始化已完成\n" );

#ifdef _MUSEUM
	if( getMuseum() )
		print("\n这是石器博物馆!!\n");
	else
		print("\n这是普通星球!!\n");
#endif

	print( "开始记始日志\n" );
    {
        char    logconffile[512];
        snprintf( logconffile, sizeof( logconffile), "%s/%s" ,
                  getLogdir(), getLogconffile() );
        if( !initLog( logconffile ) )
            goto CLOSEAC;
    }

#ifdef _M_SERVER
	print( "尝试连接GM服务器... " );
    mfd = connectmServer( getmservername(), getmserverport());
	if (mfd ==-1 ){
		print( "连接GM服务器失败... " );
	}else{
		initConnectOne( mfd, NULL , 0 );
		print( "成败连接GM服务器... " );
	}
#endif

#ifdef _NPCSERVER_NEW
	npcfd = connectNpcServer( getnpcserveraddr(), getnpcserverport());
	if( npcfd == -1 ){
		print( "连接NPC服务器失败... " );
	}else{
		initConnectOne( npcfd, NULL , 0 );
		print( "成败连接NPC服务器... " );
		NPCS_NpcSLogin_send( npcfd);
	}
#endif
	
#ifdef _ITEM_QUITPARTY
	print( "读取队伍解散物品消失文件..." );
    
	//读取档案
    f = fopen( getitemquitparty(), "r" );
	if( f != NULL ){
		while( fgets( line, sizeof( line ), f ) ){
			if( line[0] == '#' )continue;
            if( line[0] == '\n' )continue;
		    chomp( line );
			itemquitparty_num++;
		}
		if( fseek( f, 0, SEEK_SET ) == -1 ){
			print( "物品录找错误\n" );
			fclose(f);
			goto CLOSEAC;
		}
		//配记忆体
		Disappear_Item = allocateMemory( sizeof(struct tagDisappearItem) * itemquitparty_num );
		if( Disappear_Item == NULL ){
			print( "无法分配内存 %d\n", sizeof(struct tagDisappearItem) * itemquitparty_num );
			fclose( f );
			goto CLOSEAC;
		}

		i = 0;
		//将道具编号存入 Disappear_Item.string
		while( fgets( line, sizeof( line ), f ) ){
			if( line[0] == '#' )continue;
			if( line[0] == '\n' )continue; 
			chomp( line );
			sprintf( Disappear_Item[i].string,"%s",line );
			print("\n道具编号:%s", Disappear_Item[i].string );
			i++;
		}
		fclose(f);
	}
#endif

    DEBUG_ADJUSTTIME = 0;
    print( "\n" );
    return TRUE;

CLOSEAC:
    close( acfd );
CLOSEBIND:
    close( bindedfd );
    endConnect();
MEMEND:
    memEnd();
RETURNFALSE:
    return FALSE;
}
