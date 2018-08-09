#ifndef __VERSION_H__
#define __VERSION_H__
#define _SASQL1

/* 请将控制专案的 definition 放在这个档案
 * 然後藉由 remark/unmark 来开关功能
 */

/* -------------------------------------------------------------------
 * 专案：族人职务与家族功能补充
 * 人员：小瑜、彦勋、志伟、小民
 * 说明：
 */
#define _LEADERFLAG_VER		// 嗯、ㄟ、这个.... (可开放)
#define __FAMILY			//  (可开放)
#define _PERSONAL_FAME		// Arminius 8.30 家族个人声望  (可开放)
#define _FMVER21			// CoolFish: 家族族长职务代理人 2001/8/30 (可开放)
#define MEMBER_100			// CoolFish: 家族人数上限 2001/9/3 (可开放)
#define _EVERYONE_RIDE		// CoolFish: 骑宠平民化家族金额上限 2001/9/24 (可开放)

/* -------------------------------------------------------------------
 * 专案：当机不回朔之人物资料存档方式修正
 * 人员：小瑜、彦勋
 * 说明：修正人物资料存档方式
 */
#define _NewSave		// CoolFish: 2001/10/16 (可开放)

/* -------------------------------------------------------------------
 * 专案：星球合并
 * 人员：阿民、彦勋、小瑜
 * 说明：
 */
//#define _RELOGIN		// CoolFish: 重复登入机制 2001/10/29 (不可开放)

/* -------------------------------------------------------------------
 * 专案：伊甸大陆
 * 人员：彦豪、阿民、小瑜
 * 说明：工期 1/15
 */
//#define _AUCTIONEER // Arminius 1.10 拍卖店 (伊甸大陆)

/* -------------------------------------------------------------------
 * 专案：家族ＢＵＧ修正
 * 人员：小瑜
 * 说明：
 */
#define _LEADERFUNCHECK	// CoolFish: 2002/02/06 族长功能错误、禅让错误修正(可开放)

/* -------------------------------------------------------------------
 * 专案：跨星球踢人
 * 人员：彦豪
 * 说明：
 */
#define _WAEI_KICK				// (可开放)(北京不必开)
#define _FIX_WORKS				// (可开放)(北京不必开) wk修改

//#define _DELAY_UNLOCK			// (不可开) ANDY
//#define _LOCK_SERVER			// (不可开)
//#define _TEST_LOCKSERVER	// (不可开)
//#define _NO_DOUBLECHECK		// (不可开)
//#define _PAUCTION_MAN			// (不可开) 寄卖
/* -------------------------------------------------------------------
 * 专案：九大庄园(4.0)
 * 人员：志旺	
 * 说明：
 */
#define _FIX_9_FMPOINT			// (可开放) WON ADD 九大庄园
#define _FIX_LEADER_ERR			// (可开放) WON ADD 修正族长问题
#define _REMOVE_FM_LOG          // (可开放) WON ADD 移除不必要的家族 LOG
//#define _ADD_FAMILY_TAX		// (不可开) WON ADD 增加庄园税收


/* -------------------------------------------------------------------
 * 专案：确认庄园挑战排程
 * 人员：旭诚	
 * 说明：由AC下family目录里fmschedule档统一广播各庄园挑战排程 
*/
//#define _CHECKFMSCHEDULE	    // (不可开) Syu ADD 透过AC确认家族挑战排程
//#define _FAMILYBANKSTONELOG   // (不可开) Syu ADD 新增家族银行存取Log
//#define _FIXFMASSIGN		    // (不可开) Syu ADD 修正家族一百人无法指派、移除长老	


/* -------------------------------------------------------------------
 * 专案：石器6.0
 * 人员：
 * 说明：
 */
#define _SEND_EFFECT		    // (可开放) WON ADD 送下雪、下雨等特效
//#define _NEW_PLAYERGOLD		// (不可开) ANDY 新帐号给钱
//#define _BAD_PLAYER           // (不可开) WON ADD 送坏玩家去关

// -------------------------------------------------------------------
#define _AC_SEND_FM_PK			// (可开放) WON ADD 庄园对战列表储存在AC
#define _ACFMPK_LIST			// (可开放) ANDY 庄园对战列表
#define _CK_ONLINE_PLAYER_COUNT	// (可开放) WON ADD 计算线上人数

/* -------------------------------------------------------------------
 * 专案：石器7.0
 * 人员：
 * 说明：
 */
#define _ALLDOMAN				// (可开放) Syu ADD 排行榜NPC
#define _CHAR_POOLITEM			// (可开放) ANDY 共用仓库
#define _CHAR_POOLPET			// (可开放) Robin 共用宠物仓库
//#define _FIX_ALLDOMAN			// (不可开) ANDY
// -------------------------------------------------------------------
// 新功能：客服公告系统
#define _GM_BROADCAST				// (可开放) WON ADD 客服公告系统

// -------------------------------------------------------------------
// 新功能：GM密频功能扩充
//#define _RECAL_BACKUP_PLAYER		// (可开放 (暂时先关起来) ) WON 回溯功能 (北京&韩国不必开)
//#define _UNIVERSE_CHATROOM			// (不可开) ANDY 全星系聊天

// -------------------------------------------------------------------
// 新功能：修改WORKER(新付费机制)
#define _FIX_MESSAGE				// (可开) WON ADD 修改WORKER封包内容 (北京&韩国不必开)
#define _WORKER_AUTO_LOGOUT		// (可开) WON ADD GS断线自动送LOGOUT给WORKER

//#define _DEFEND_NEWUSERS			// (不可开) ANDY 老手带新手
//#define _CHECK_BADFILES			// (不可开) ANDY 判断坏挡
#define _AC_UNLOCK_GS_PLAYER		// (不可开) WON ADD 自动UNLOCK
//#define _ACFMPK_NOFREE			// (不可开) ANDY 家族无条件

// -------------------------------------------------------------------
// 新功能：跨星系pk伺服器
//#define _OACSTRUCT_TCP			// (不可开) ANDY 跨星系取得人物列表
//#define _TCPIP_UTIL				// (不可开) ANDY TCPIP
//#define _DEATH_CONTEND			// (不可开) ANDY 死斗
//#define _DEATH_FAMILY_LOGIN_CHECK	// (不可开) WON ADD 家族战登入检查
//#define _DEATH_FAMILY_GM_COMMAND	// (不可开) WON ADD 家族战GM指令
//#define _DEATH_FAMILY_STRUCT		// (不可开) WON ADD 家族战存放胜负资料

#define _CHANGEGALAXY	// Robin 2003/10/27 星系移民

//#define _TIMEOUTKICK // Robin 2003/11/07 时段制自动踢人

//#define _UNLOCKPOOL		// Robin 2003/12/12  Logout暂存区

//#define _KILLDEADWORKER // Robin 2004/02/ 用sigusr1清除所有Worker 在unlock.arg加'K'

#define _SLEEP_CHAR		// Robin 2004/02/12	活跃人物与非活跃人物分开储存

#endif

// Terry define start -------------------------------------------------------------------
#define _FM_MODIFY				// 家族功能修改
#define _CHANNEL_MODIFY		// 频道功能整合
#define _NEW_MANOR_LAW		// 新庄园规则
#define _LOCK_ADD_NAME		// LockNode 增加名字栏位(未开放)
#define _TEACHER_SYSTEM   // 导师系统(未开放)
// Terry define end   -------------------------------------------------------------------

//#define _ANGEL_SUMMON	// Robin 精灵的召唤
//#define _SASQL //Change 石器资料库 (不可开)
//#define _RACEMAN		// cyg 猎宠大会


//--------------------------------------------------------
//by longzoro
#define _SQL_BACKGROUND   //网页后台功能   需同时开放_SASQL1

#define _SQL_REGISTER     //自动注册功能   需同时开放_SASQL1
