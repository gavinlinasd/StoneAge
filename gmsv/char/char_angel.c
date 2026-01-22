#include "version.h"
#ifdef _ANGEL_SUMMON
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   // shan
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "common.h"
#include "char_base.h"
#include "char_data.h"
#include "char.h"
#include "lssproto_serv.h"
#include "item.h"
#include "item_event.h"
#include "buf.h"
#include "object.h"
#include "map_deal.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "handletime.h"
#include "char_event.h"
#include "npccreate.h"
#include "addressbook.h"
#include "item_event.h"
#include "magic_base.h"
#include "magic.h"
#include "chatmagic.h"
#include "configfile.h"
#include "log.h"
#include "anim_tbl.h"
#include "encount.h"
#include "battle.h"
#include "pet_skill.h"
#include "util.h"
#include "enemy.h"
#include "npcutil.h"
#include "pet.h"
#include "family.h"
#include "defend.h"
#include "npcserver.h"


struct MissionInfo missionlist[MAXMISSION];
int mission_num =0;
struct MissionTable missiontable[MAXMISSIONTABLE];

//#define ANGELITEM 2884 //20701	//使者的信物 道具编号
//#define HEROITEM 2885 //20702	//勇者的信物 道具编号

extern int AngelReady;

char* getMissionNameInfo( int charaindex, char* nameinfo)
{
	sprintf( nameinfo, "%s:%s", CHAR_getChar( charaindex, CHAR_CDKEY), CHAR_getChar( charaindex, CHAR_NAME) );
	return nameinfo;
}

int checkIfAngel( int charaindex)
{
	int i;
	char nameinfo[512];

	//sprintf( nameinfo, "%s:%s", CHAR_getChar( charaindex, CHAR_CDKEY), CHAR_getChar( charaindex, CHAR_NAME) );
	getMissionNameInfo( charaindex, nameinfo);
	for( i =0; i <MAXMISSIONTABLE; i++) {
		if( missiontable[i].angelinfo[0] == NULL)
			continue;
		if( !strcmp( nameinfo, missiontable[i].angelinfo) ) {
			return i;
		}
		else if( !strcmp( nameinfo, missiontable[i].heroinfo) ) {
			return i;
		}
	}
	return -1;
}

int checkIfOnlyAngel( int charaindex)
{
	int i;
	char nameinfo[512];

	//sprintf( nameinfo, "%s:%s", CHAR_getChar( charaindex, CHAR_CDKEY), CHAR_getChar( charaindex, CHAR_NAME) );
	getMissionNameInfo( charaindex, nameinfo);
	for( i =0; i <MAXMISSIONTABLE; i++) {
		if( missiontable[i].angelinfo[0] == NULL)
			continue;
		if( !strcmp( nameinfo, missiontable[i].angelinfo) ) {
			return i;
		}
	}
	return -1;
}

int checkIfOnlyHero( int charaindex)
{
	int i;
	char nameinfo[512];

	//sprintf( nameinfo, "%s:%s", CHAR_getChar( charaindex, CHAR_CDKEY), CHAR_getChar( charaindex, CHAR_NAME) );
	getMissionNameInfo( charaindex, nameinfo);
	for( i =0; i <MAXMISSIONTABLE; i++) {
		if( missiontable[i].angelinfo[0] == NULL)
			continue;
		if( !strcmp( nameinfo, missiontable[i].heroinfo) ) {
			return i;
		}
	}
	return -1;
}

int checkIfAngelByName( char* nameinfo)
{
	int i;

	for( i =0; i <MAXMISSIONTABLE; i++) {
		if( missiontable[i].angelinfo[0] == NULL)
			continue;
		if( !strcmp( nameinfo, missiontable[i].angelinfo) ) {
			return i;
		}
		else if( !strcmp( nameinfo, missiontable[i].heroinfo) ) {
			return i;
		}
	}
	return -1;
}


void addAngelData( int angelindex, int heroindex, int mission, int flag)
{
	int i;
	char buf[1024];
	char angelinfo[512];
	char heroinfo[512];

	//if( checkIfAngel( angelindex) != -1 || checkIfAngel( heroindex) != -1 )
	//	return;
	//sprintf( angelinfo, "%s:%s", CHAR_getChar( angelindex, CHAR_CDKEY), CHAR_getChar( angelindex, CHAR_NAME) );
	getMissionNameInfo( angelindex, angelinfo);
	//sprintf( heroinfo, "%s:%s", CHAR_getChar( heroindex, CHAR_CDKEY), CHAR_getChar( heroindex, CHAR_NAME) );
	getMissionNameInfo( heroindex, heroinfo);
	
	sprintf( buf, "%s|%s|%d|%d", angelinfo, heroinfo, mission, flag );
	//saacproto_ACMissionTable_send( acfd, -1, 2, buf, angelindex);
	saacproto_ACMissionTable_send( acfd, -1, 2, buf, angelinfo);

	return;
}

//void selectAngel( int charaindex)
void selectAngel( int charaindex, int heroindex, int mission, int gm_cmd)
{
	// gm_cmd 表示是否由GM指令产生, 

	//int heroindex =-1;
	int findindex, startindex;
	char msg[1024];
	int max_char;
	//int mission;
	
	if( AngelReady <= 0 && gm_cmd == FALSE)
		return;

	sprintf( msg, " 使者资格检查: %s %s ", CHAR_getChar( charaindex, CHAR_CDKEY), CHAR_getChar( charaindex, CHAR_NAME));
	print( msg);
	LogAngel( msg);

	// 天使条件检查
	if( gm_cmd == FALSE )
	{
		if( checkIfAngel( charaindex) != -1) // 是否天使或勇者
		{
			print(" ANGEL已经是天使或勇者了 ");
			return;
		}
		
		if( CHAR_getInt( charaindex, CHAR_LV) < 30 || !NPC_EventCheckFlg( charaindex, 4 ) )
		{
			print(" ANGEL资格不符 ");
			return;
		}
		
		srand( time(NULL));
		if( rand()%3 == 0 )	// 天使的机率 2/3
		{
			print(" ANGEL机率检查不通过 ");
			return;
		}
	}
	print(" ANGEL决定 ");

	
	// 挑选勇者
	if( heroindex == -1 )
	{
		//heroindex = -1;
		max_char = CHAR_getPlayerMaxNum();
		startindex = RAND( 0, max_char-1);
		findindex = startindex;
		while(1) {
			if( findindex == startindex-1)	break;
			findindex = (findindex+1) % max_char;
			if( !CHAR_CHECKINDEX( findindex) ) continue;
			if( CHAR_getInt( findindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) continue;
			if( findindex == charaindex ) continue;
			if( checkIfAngel( findindex) != -1)	continue; // 是否天使或勇者
			if( CHAR_getInt( findindex, CHAR_LV) < 80 || !NPC_EventCheckFlg( findindex, 4 ) ) continue; // 勇者的条件
			if( rand()%3 == 0 )	continue; // 勇者的机率 2/3
			
			heroindex = findindex;
			
			print(" ANGEL勇者决定 ");
			
			break;
		}
		if( heroindex < 0) {
			sprintf( msg, " 没有找到勇者人选!! ");
			print( msg);
			//LogAngel( msg);
			return;
		}
	}

	// 决定任务
	if( mission == -1 )
	{
		int mlist[MAXMISSION]; // 符合条件的任务列表
		char eventlist[1024];
		char buf[64];
		int i, j, mindex =0;
		
		memset( mlist, 0, sizeof(mlist));
		for( i =0; i <MAXMISSION; i++) {
			int checked =TRUE;
			if( missionlist[i].id <= 0) continue;
			if( CHAR_getInt( heroindex, CHAR_LV) < missionlist[i].level )
				continue;
			strcpy( eventlist, missionlist[i].eventflag); // 检查必要旗标
			j =0;
			while(1) {
				j++;
				if( getStringFromIndexWithDelim( eventlist, ";", j, buf, sizeof(buf)) == FALSE )
					break;
				if( buf[0] != '!') { // 必要旗标或禁止旗标
					if( !NPC_EventCheckFlg( heroindex, atoi(buf) )) {
						checked = FALSE;
						break;
					}
				}
				else {
					if( NPC_EventCheckFlg( heroindex, atoi(buf+1) )) {
						checked = FALSE;
						break;
					}
				}
			}
			if( checked ) {
				mlist[mindex] = missionlist[i].id;
				print(" mlist[%d]=%d ", mindex, mlist[mindex]);
				mindex++;
			}
		}
		if( mindex <= 0 ) {
			sprintf( msg, " 找不到合适的任务 ");
			print( msg);
			//LogAngel( msg);
			return;
		}
		
		mission = mlist[rand()%mindex];
	}


	addAngelData( charaindex, heroindex, mission, MISSION_WAIT_ANSWER); // 传到AC

	// 清除旗标 event8 224~255 为精灵召唤专用
	CHAR_setInt( charaindex, CHAR_NOWEVENT8, 0);
	CHAR_setInt( charaindex, CHAR_ENDEVENT8, 0);
	CHAR_setInt( heroindex, CHAR_NOWEVENT8, 0);
	CHAR_setInt( heroindex, CHAR_ENDEVENT8, 0);

	if( gm_cmd == FALSE )
		AngelReady = 0; // 清除缺额
	
	{
		sprintf( msg, " 产生 %s 天使候补: %s %s Lv:%d 勇者候补: %s %s Lv:%d 任务:%d ci=%d hi=%d ",
				gm_cmd ? ("(GM指令)") : (" "),
				CHAR_getChar( charaindex, CHAR_CDKEY), CHAR_getChar( charaindex, CHAR_NAME), CHAR_getInt( charaindex, CHAR_LV),
				CHAR_getChar( heroindex, CHAR_CDKEY), CHAR_getChar( heroindex, CHAR_NAME), CHAR_getInt( heroindex, CHAR_LV),
				mission, charaindex, heroindex);
		print( msg);
		LogAngel( msg);
	}
}

void sendAngelCleanToCli( int fd)
{
	lssproto_WN_send( fd, //getfdFromCharaIndex(charaindex),
			WINDOW_MESSAGETYPE_ANGELMESSAGE, -1,
			CHAR_WINDOWTYPE_ANGEL_CLEAN,
			-1,	"");
}

int AngelCreate( int angelindex)
{
	int emptyitem1, emptyitem2;
	int angeltokenindex, herotokenindex;
	char msgbuf[1024]/*, nameinfo[1024]*/;
	int mindex, mission;
	char heroname[64];

	if( !CHAR_CHECKINDEX( angelindex))	return FALSE;

	print(" 天使答应了!! ");
	
	mindex = checkIfAngel( angelindex);
	if( mindex == -1 || missiontable[mindex].flag == MISSION_TIMEOVER ) {
		//print("\n ANGEL错误!!Table逾时或无资料??:%d ", angelindex );
		CHAR_talkToCli( angelindex, -1, "很抱歉，你太晚回答，已经逾时了。", CHAR_COLORYELLOW);
		
		sprintf( msgbuf, " 回答逾时或无资料 i:%d 使者:%s %s ", mindex, CHAR_getChar( angelindex, CHAR_CDKEY), CHAR_getChar( angelindex, CHAR_NAME));
		print( msgbuf);
		LogAngel( msgbuf);
		
		return FALSE;
	}

	if( missiontable[mindex].flag != MISSION_WAIT_ANSWER ) {
		print(" 旗标不符:%d ", missiontable[mindex].flag);
		return FALSE;
	}

	if( CHAR_findEmptyItemBoxNo( angelindex ) < 2 ){
		CHAR_talkToCli( angelindex, -1, "空间栏位不足。至少要有两个空道具栏位。", CHAR_COLORYELLOW);
		return TRUE;
	}

	getStringFromIndexWithDelim( missiontable[mindex].heroinfo, ":", 2, heroname, sizeof(heroname));

	angeltokenindex = ITEM_makeItemAndRegist( ANGELITEM );
	if( angeltokenindex != -1 ){
		/*if( ITEM_getInt( angeltokenindex, ITEM_TYPE) != ITEM_ANGELTOKEN) {
			print(" 制作使者信物失败 ");
			ITEM_endExistItemsOne( angeltokenindex);
			return;
		}*/

		//sprintf( nameinfo, "%s:%s", CHAR_getChar( angelindex, CHAR_CDKEY), CHAR_getChar( angelindex, CHAR_NAME ) );
		ITEM_setChar( angeltokenindex, ITEM_ANGELINFO, missiontable[mindex].angelinfo);
		ITEM_setChar( angeltokenindex, ITEM_HEROINFO, missiontable[mindex].heroinfo);
		sprintf( msgbuf, "%d", missiontable[mindex].mission);
		ITEM_setChar( angeltokenindex, ITEM_ANGELMISSION, msgbuf);
		//sprintf( msgbuf, "%s(%s)", ITEM_getChar( angeltokenindex, ITEM_NAME), CHAR_getChar( angelindex, CHAR_NAME ) );
		//ITEM_setChar( angeltokenindex, ITEM_NAME, msgbuf);
		//ITEM_setChar( angeltokenindex, ITEM_SECRETNAME, msgbuf);
		//sprintf( msgbuf, "这是使者 %s 与勇者 %s 专属的信物", CHAR_getChar( angelindex, CHAR_NAME), heroname );
		sprintf( msgbuf, "精灵使者 %s 的信物，装备後不遇敌。", CHAR_getChar( angelindex, CHAR_NAME) );
		ITEM_setChar( angeltokenindex, ITEM_EFFECTSTRING, msgbuf);

		emptyitem1 = CHAR_addItemSpecificItemIndex( angelindex, angeltokenindex);
		CHAR_sendItemDataOne( angelindex, emptyitem1);
		LogItem(
			CHAR_getChar( angelindex, CHAR_NAME ),
			CHAR_getChar( angelindex, CHAR_CDKEY ),
			angeltokenindex,
			"AddItem(制作道具 使者信物)",
			CHAR_getInt( angelindex,CHAR_FLOOR),
			CHAR_getInt( angelindex,CHAR_X ),
			CHAR_getInt( angelindex,CHAR_Y ),
			ITEM_getChar( angeltokenindex, ITEM_UNIQUECODE),
			ITEM_getChar( angeltokenindex, ITEM_NAME),
			ITEM_getInt( angeltokenindex, ITEM_ID) );
		CHAR_talkToCli( angelindex, -1,"得到使者的信物。", CHAR_COLORYELLOW);
	}else{
		print("制作使者的信物失败。" );
		return FALSE;
	}

	herotokenindex = ITEM_makeItemAndRegist( HEROITEM );
	if( herotokenindex != -1 ){
		/*if( ITEM_getInt( herotokenindex, ITEM_TYPE) != ITEM_HEROTOKEN) {
			print(" 制作勇者信物失败 ");
			ITEM_endExistItemsOne( herotokenindex);
			ITEM_endExistItemsOne( angeltokenindex);
			CHAR_sendItemDataOne( angelindex, emptyitem1);
			return;
		}*/
		//sprintf( nameinfo, "%s:%s", CHAR_getChar( angelindex, CHAR_CDKEY), CHAR_getChar( angelindex, CHAR_NAME ) );
		ITEM_setChar( herotokenindex, ITEM_ANGELINFO, missiontable[mindex].angelinfo);
		ITEM_setChar( herotokenindex, ITEM_HEROINFO, missiontable[mindex].heroinfo);
		sprintf( msgbuf, "%d", missiontable[mindex].mission);
		ITEM_setChar( herotokenindex, ITEM_ANGELMISSION, msgbuf);
		//sprintf( msgbuf, "%s(%s)", ITEM_getChar( herotokenindex, ITEM_NAME), heroname );
		//ITEM_setChar( herotokenindex, ITEM_NAME, msgbuf);
		//ITEM_setChar( herotokenindex, ITEM_SECRETNAME, msgbuf);
		//sprintf( msgbuf, "这是勇者 %s 与使者 %s 专属的信物", heroname, CHAR_getChar( angelindex, CHAR_NAME) );
		sprintf( msgbuf, "勇者 %s 的信物，使者使用可传送至勇者身边。", heroname );
		ITEM_setChar( herotokenindex, ITEM_EFFECTSTRING, msgbuf);

		emptyitem2 = CHAR_addItemSpecificItemIndex( angelindex, herotokenindex);
		CHAR_sendItemDataOne( angelindex, emptyitem2);
		LogItem(
			CHAR_getChar( angelindex, CHAR_NAME ),
			CHAR_getChar( angelindex, CHAR_CDKEY ),
			herotokenindex,
			"AddItem(制作道具 勇者信物)",
			CHAR_getInt( angelindex,CHAR_FLOOR),
			CHAR_getInt( angelindex,CHAR_X ),
			CHAR_getInt( angelindex,CHAR_Y ),
			ITEM_getChar( herotokenindex, ITEM_UNIQUECODE),
			ITEM_getChar( herotokenindex, ITEM_NAME),
			ITEM_getInt( herotokenindex, ITEM_ID) );
		CHAR_talkToCli( angelindex, -1,"得到勇者的信物。", CHAR_COLORYELLOW);
	}else{
		print("制作勇者的信物失败。" );
		ITEM_endExistItemsOne( angeltokenindex);
		CHAR_sendItemDataOne( angelindex, emptyitem1);
		return FALSE;
	}

	// 更新至AC Server
	//sprintf( nameinfo, "%s:%s", CHAR_getChar( angelindex, CHAR_CDKEY), CHAR_getChar( angelindex, CHAR_NAME ) );
	sprintf( msgbuf, "%s|%d", missiontable[mindex].angelinfo, missionlist[missiontable[mindex].mission].limittime );
	saacproto_ACMissionTable_send( acfd, MISSION_DOING, 4, msgbuf, "");

	//CHAR_talkToCli( angelindex, -1, "天之声：非常感谢你答应帮忙，那我就将信物交给你了，请将勇者的信物转交给勇者。", CHAR_COLORYELLOW);

	lssproto_WN_send( getfdFromCharaIndex(angelindex), WINDOW_MESSAGETYPE_MESSAGE, 
			WINDOW_BUTTONTYPE_YES, -1, -1,
			"非常感谢你答应帮忙，那我就将信物交给你了，请将勇者的信物转交给勇者。");

	sprintf( msgbuf, " 使者答应帮忙了 i:%d 使者:%s 勇者:%s ci=%d ", mindex, missiontable[mindex].angelinfo, missiontable[mindex].heroinfo, angelindex);
	print( msgbuf);
	LogAngel( msgbuf);

	return FALSE;
}


// 使用使者信物时
void Use_AngelToken( int charaindex, int toindex, int haveitemindex )
{
	int itemindex;
	char nameinfo[1024];
	int mindex;
	char msg[1024];
	char tokenbuf[64];

	print(" 使用使者信物 ");
	
	if( !CHAR_CHECKINDEX( charaindex) )	return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;

	mindex = checkIfAngel( charaindex);
	//sprintf( nameinfo, "%s:%s", CHAR_getChar(charaindex, CHAR_CDKEY), CHAR_getChar(charaindex, CHAR_NAME));
	getMissionNameInfo( charaindex, nameinfo);

	if( mindex == -1 || 
		( strcmp( nameinfo, ITEM_getChar( itemindex, ITEM_ANGELINFO)) && strcmp( nameinfo, ITEM_getChar( itemindex, ITEM_HEROINFO)) ) ) {
		// 路人甲使用时
		CHAR_talkToCli( charaindex, -1, "这并不是属於你的信物，不可随便使用喔。", CHAR_COLORRED);
		return;
	}

	if( strcmp( ITEM_getChar( itemindex, ITEM_ANGELINFO), missiontable[mindex].angelinfo)
		|| strcmp( ITEM_getChar( itemindex, ITEM_HEROINFO), missiontable[mindex].heroinfo) ) {

		CHAR_talkToCli( charaindex, -1, "这是无用的信物，还是丢掉吧。", CHAR_COLORRED);
		return;
	}

	if( !strcmp( nameinfo, missiontable[mindex].angelinfo) ) { // 使者使用时

		int lefttime, hour, min;
		lefttime = missiontable[mindex].time + missiontable[mindex].limittime*60*60 - (int)time(NULL);
		hour = lefttime / (60*60);
		min = (lefttime % (60*60)) / 60;

		if( missiontable[mindex].flag == MISSION_DOING ) {
			// 显示任务资料
			getStringFromIndexWithDelim( missiontable[mindex].heroinfo, ":", 2, tokenbuf, sizeof(tokenbuf));
			sprintf( msg, "你的使命是将勇者的信物交给 %s ，%s，时间还剩馀%d小时%d分。",
				tokenbuf, missionlist[missiontable[mindex].mission].detail, hour, min);
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
		}
		else if( missiontable[mindex].flag == MISSION_HERO_COMPLETE ) {
			// 可以去领奖了
			getStringFromIndexWithDelim( missiontable[mindex].heroinfo, ":", 2, tokenbuf, sizeof(tokenbuf));
			sprintf( msg, "勇者 %s 的使命已经完成了，你可以去领奖了，时间还剩馀%d小时%d分。",
				tokenbuf, hour, min);
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
		}
		else if( missiontable[mindex].flag == MISSION_TIMEOVER ) {
			// 时间超过了
			sprintf( msg, "很可惜，使者和勇者并没有在时限内完成使命，下次再加油吧。");
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
		}
		return;
	}
	else if( !strcmp( nameinfo, missiontable[mindex].heroinfo) ) { // 勇者使用时
		// 传送至使者身旁
		int maxchar, i;
		char cdkey[64], name[64];
		int fl =0, fx =0, fy =0;

		if( checkUnlawWarpFloor( CHAR_getInt( charaindex, CHAR_FLOOR) ) ) {
				CHAR_talkToCli( charaindex, -1, "你所在的地方无法传送。", CHAR_COLORYELLOW );
				return;
		}

		getStringFromIndexWithDelim( missiontable[mindex].angelinfo, ":", 1, cdkey, sizeof(cdkey));
		getStringFromIndexWithDelim( missiontable[mindex].angelinfo, ":", 2, name, sizeof(name));
		maxchar = CHAR_getPlayerMaxNum();
		for( i =0; i <maxchar; i++)
		{
			if( !CHAR_CHECKINDEX( i) ) continue;
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) continue;
			//sprintf( tempinfo, "%s:%s", CHAR_getChar( i, CHAR_CDKEY), CHAR_getChar( i, CHAR_NAME));
			if( strcmp( cdkey, CHAR_getChar( i, CHAR_CDKEY)) ) continue;
			if( strcmp( name, CHAR_getChar( i, CHAR_NAME)) ) continue;
			
			fl = CHAR_getInt( i, CHAR_FLOOR);
			fx = CHAR_getInt( i, CHAR_X);
			fy = CHAR_getInt( i, CHAR_Y);

			if( checkUnlawWarpFloor( fl) ) {
				CHAR_talkToCli( charaindex, -1, "对象所在的地方无法传送。", CHAR_COLORYELLOW );
				return;
			}

			break;
		}
		if( fl <= 0 )
		{
			sprintf( msg, "使者 %s 目前不在线上或不在本伺服器上。", name);
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
			return;
		}
		
		sprintf( msg, "传送至使者 %s 身边。", name);
		CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
		CHAR_warpToSpecificPoint( charaindex, fl, fx, fy);
	}

}

// 使用勇者信物时
void Use_HeroToken( int charaindex, int toindex, int haveitemindex )
{
	int itemindex;
	int mindex;
	char nameinfo[64];
	char msg[1024];

	print(" 使用勇者信物 ");

	if( !CHAR_CHECKINDEX( charaindex) )	return;
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX( itemindex) ) return;

	mindex = checkIfAngel( charaindex);
	//sprintf( nameinfo, "%s:%s", CHAR_getChar(charaindex, CHAR_CDKEY), CHAR_getChar(charaindex, CHAR_NAME));
	getMissionNameInfo( charaindex, nameinfo);

	if( mindex == -1 ||
		( strcmp( nameinfo, ITEM_getChar( itemindex, ITEM_ANGELINFO)) && strcmp( nameinfo, ITEM_getChar( itemindex, ITEM_HEROINFO)) ) ) {
		// 路人甲使用时
		CHAR_talkToCli( charaindex, -1, "这并不是属於你的信物，不可随便使用喔。", CHAR_COLORRED);
		return;
	}

	if(  strcmp( ITEM_getChar( itemindex, ITEM_ANGELINFO), missiontable[mindex].angelinfo)
		|| strcmp( ITEM_getChar( itemindex, ITEM_HEROINFO), missiontable[mindex].heroinfo) ){

		CHAR_talkToCli( charaindex, -1, "这是无用的信物，还是丢掉吧。", CHAR_COLORRED);
		return;
	}


	if( !strcmp( nameinfo, missiontable[mindex].heroinfo) ) { // 勇者使用时

		int lefttime, hour, min;
		lefttime = missiontable[mindex].time + missiontable[mindex].limittime*60*60 - (int)time(NULL);
		hour = lefttime / (60*60);
		min = (lefttime % (60*60)) / 60;

		if( missiontable[mindex].flag == MISSION_DOING ) {
			// 显示任务资料
			sprintf( msg, "你的使命是 %s，时间还剩馀%d小时%d分。",
				missionlist[missiontable[mindex].mission].detail, hour, min);
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
		}
		else if( missiontable[mindex].flag == MISSION_HERO_COMPLETE ) {
			// 可以去领奖了
			sprintf( msg, "你的使命已经完成了，可以去领奖了，时间还剩馀%d小时%d分。",
				hour, min);
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
		}
		else if( missiontable[mindex].flag == MISSION_TIMEOVER ) {
			// 时间超过了
			sprintf( msg, "很可惜，使者和勇者并没有在时限内完成使命，下次再加油吧。");
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
		}
		return;

	}
	else if( !strcmp( nameinfo, missiontable[mindex].angelinfo) ) { // 使者使用时
		// 传送至勇者身旁
		int maxchar, i;
		char cdkey[64], name[64];
		int fl =0, fx =0, fy =0;

		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ) {
				CHAR_talkToCli( charaindex, -1, "组队中无法传送。", CHAR_COLORYELLOW );
				return;
		}
		
		if( checkUnlawWarpFloor( CHAR_getInt( charaindex, CHAR_FLOOR) ) ) {
				CHAR_talkToCli( charaindex, -1, "你所在的地方无法传送。", CHAR_COLORYELLOW );
				return;
		}

		//if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE ){
		if( CheckDropatLogout( charaindex) == TRUE ) {
			CHAR_talkToCli( charaindex, -1, "携带登出後消失的道具时无法使用。", CHAR_COLORYELLOW);
			return;
		}

		getStringFromIndexWithDelim( missiontable[mindex].heroinfo, ":", 1, cdkey, sizeof(cdkey));
		getStringFromIndexWithDelim( missiontable[mindex].heroinfo, ":", 2, name, sizeof(name));
		maxchar = CHAR_getPlayerMaxNum();
		for( i =0; i <maxchar; i++)
		{
			if( !CHAR_CHECKINDEX( i) ) continue;
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) continue;
			//sprintf( tempinfo, "%s:%s", CHAR_getChar( i, CHAR_CDKEY), CHAR_getChar( i, CHAR_NAME));
			if( strcmp( cdkey, CHAR_getChar( i, CHAR_CDKEY)) ) continue;
			if( strcmp( name, CHAR_getChar( i, CHAR_NAME)) ) continue;
			
			fl = CHAR_getInt( i, CHAR_FLOOR);
			fx = CHAR_getInt( i, CHAR_X);
			fy = CHAR_getInt( i, CHAR_Y);

			if( checkUnlawWarpFloor( fl) ) {
				CHAR_talkToCli( charaindex, -1, "对象所在的地方无法传送过去。", CHAR_COLORYELLOW );
				return;
			}

			break;
		}
		if( fl <= 0 )
		{
			sprintf( msg, "勇者 %s 目前不在线上或不在本伺服器上。", name);
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORRED);
			return;
		}
		
		sprintf( msg, "传送至勇者 %s 身边。", name);
		CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
		CHAR_warpToSpecificPoint( charaindex, fl, fx, fy);

	}
	else { // 路人甲使用时
		CHAR_talkToCli( charaindex, -1, "这并不是属於你的信物，不可随便使用喔。", CHAR_COLORRED);
		return;
	}


}


void CHAR_sendAngelMark( int objindex, int flag)
{
	int		opt[1];
	opt[0] = flag;
	CHAR_sendWatchEvent( objindex,CHAR_ACTANGEL,opt,1,TRUE);
}


#endif
