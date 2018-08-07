/********************/
/*	市集交易系统	*/
/********************/
#include <string.h>
#include "version.h"
#include "item_trade.h"
#include "handletime.h"
#include "object.h"
#include "trade.h"
#include "readmap.h"
#include "lssproto_serv.h"
#include "enemy.h"
#include "pet.h"

#ifdef _MARKET_TRADE
#define MAXTRADES 4
MapTrade TRADE_DATAS[MAXTRADES] = {
	{ -1, 5, 5, 0, -1, "NULL"},{ -1, 10, 5, 0, -1, "NULL"},
	{ -1, 5, 10, 0, -1, "NULL"},{ -1, 10, 10, 0, -1, "NULL"}	
};
void InitMapTradeData( int index, int Stime)
{
	int i;
	for( i=0; i<arraysizeof( TRADE_DATAS); i++)	{
		TRADE_DATAS[i].masterindex = index;
		TRADE_DATAS[i].Ttime = Stime;
		TRADE_DATAS[i].Goodindex = -1;
		memset( TRADE_DATAS[i].Goodname, 0, sizeof( TRADE_DATAS[i].Goodname));
	}
}

BOOL CHECKMAP_TRADE( int charindex, int floor, int x, int y)
{
	if( floor != TRADEMAP )
		return FALSE;
	return TRUE;
}

int CHECKMAP_TRADEXY( int charindex, int floor, int x, int y)
{
	int i=0;
	BOOL FIND=FALSE;
	while( i < arraysizeof( TRADE_DATAS) && FIND == FALSE)	{
		if( x >= TRADE_DATAS[i].x && x < (TRADE_DATAS[i].x+TRADEXSIZE) )	{
			if( y >= TRADE_DATAS[i].y && y < (TRADE_DATAS[i].y+TRADEYSIZE) )	{
				FIND = TRUE;
				break;
			}
		}
		if( FIND == TRUE ) break;
		i++;
	}
	if( i >= arraysizeof( TRADE_DATAS) )
		return -1;
	return  i;
}

BOOL MAP_TRADEDROP( int charindex, int itemindex,int floor, int x, int y)
{	//return FALSE 物品丢在地上 TRUE 不丢在地上(可能是贩卖道具)
	int exhibition=-1;
	int CreateItem=-1,/*itemID=-1,*/ itemimage=-1;
	int objindex;
	char itemname[256]="(样)";

	if( CHECKMAP_TRADE( charindex, floor, x, y) == FALSE )
		return FALSE;
	if( (exhibition = CHECKMAP_TRADEXY(charindex, floor, x, y)) == -1 )
		return FALSE;
	exhibition+=TRADESTARTNUM;
	if( CHAR_getWorkInt( charindex, CHAR_MAPTRADETYPE) != exhibition )	{
		print("\n 不是摊位[%d]主人[%d] ", exhibition, CHAR_getWorkInt( charindex, CHAR_MAPTRADETYPE));
		return TRUE;
	}

	if( MAPTRADE_CHECKMAPFULL( floor, x, y) == TRUE )	{
		CHAR_talkToCli( charindex, -1, "前方已有贩卖样品!", CHAR_COLORYELLOW);
		return TRUE;
	}

	//itemID = ITEM_getInt( itemindex, ITEM_ID );
	//CreateItem = ITEM_makeItemAndRegist( itemID);
	itemimage = ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER);
	if( ITEM_getChar( itemindex, ITEM_SECRETNAME) != NULL )	{
		if( strstr( ITEM_getChar( itemindex, ITEM_SECRETNAME), "(样)") == NULL )	{
			sprintf( itemname, "%s(样)", ITEM_getChar( itemindex, ITEM_SECRETNAME));
		}
	}

	//CREAT 贩卖样品
	CreateItem = ITEM_makeItemAndRegist( TRADEITEMID);
	if( !ITEM_CHECKINDEX( CreateItem ) )
		return TRUE;
	//复制ITEM资料
	ITEM_setInt( CreateItem, ITEM_BASEIMAGENUMBER, itemimage);
	ITEM_setChar( CreateItem, ITEM_NAME, itemname);
	ITEM_setChar( CreateItem, ITEM_SECRETNAME, itemname);
	ITEM_setChar( CreateItem, ITEM_EFFECTSTRING, "贩卖样品");//ITEM_EFFECTSTRING
	ITEM_setChar( CreateItem, ITEM_ARGUMENT, "贩卖样品");

	//丢
	ITEM_setWorkInt( CreateItem, ITEM_WORKOBJINDEX, -1);
	ITEM_setWorkInt( CreateItem, ITEM_WORKCHARAINDEX, charindex);
	ITEM_setInt( CreateItem, ITEM_PUTTIME, NowTime.tv_sec);
	objindex = CHAR_DropItemAbsolute( CreateItem, floor, x, y, TRUE);
	if( objindex == -1 )	{
		ITEM_endExistItemsOne( CreateItem );
		return TRUE;
	}

	ITEM_setWorkInt( CreateItem, ITEM_WORKTRADEINDEX, exhibition);
	ITEM_setWorkInt( CreateItem, ITEM_WORKTRADETYPE, TRADETYPE_SELL );
	ITEM_setWorkInt( CreateItem, ITEM_WORKTRADESELLINDEX, itemindex);
	ITEM_setWorkInt( CreateItem, ITEM_WORKCHARAINDEX, charindex);

	CHAR_sendWatchEvent( objindex,CHAR_ACTSTAND,NULL,0,TRUE);
	CHAR_sendCToArroundCharacter( objindex);
	TRADE_DATAS[exhibition-TRADESTARTNUM ].Goodindex = CreateItem;
	
	sprintf( TRADE_DATAS[exhibition-TRADESTARTNUM ].Goodname,"道具-%s", itemname);
	return TRUE;
}

BOOL MAP_TRADEPICKUP( int charindex, int index, int floor, int x, int y, int flg)
{
	int masterindex;
	int objindex=-1;
	int exhibition;//摊位

	if( (exhibition = CHECKMAP_TRADEXY(charindex, floor, x, y)) == -1 )
		return FALSE;
	masterindex = TRADE_getMasterInt( exhibition);
	if( MAPTRADE_CHECKMASTERIN( masterindex, charindex, exhibition) == FALSE )
		return TRUE;

	if( masterindex == charindex || !CHAR_CHECKINDEX( masterindex) )	{//主人收回
		switch( flg )	{	//清除
		case TRADEITEMTYPE:
			objindex = ITEM_getWorkInt( index, ITEM_WORKOBJINDEX);
			ITEM_endExistItemsOne( index);
			OBJECT_setIndex( objindex, 0);
			CHAR_ObjectDelete( objindex);	
			break;
		case TRADEPETTYPE:
			objindex = CHAR_getWorkInt( index, CHAR_WORKOBJINDEX);
			CHAR_CharaDelete( index);
			ITEM_endExistItemsOne( index);
			break;
		}
	}else	{	//交易
		char buf1[256], msg[256];
		int fd = getfdFromCharaIndex( charindex);
		int mfd = getfdFromCharaIndex( masterindex);
		int snum=-1;
		memset( buf1, 0, sizeof( buf1));
//=======================
 		//masterindex
		if( CHAR_getWorkInt( masterindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING ||
			CHAR_getWorkInt( masterindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING ){
			CHAR_talkToCli( charindex, -1, "主人正在买卖中!", CHAR_COLORYELLOW);
			return TRUE;
		}
   		CHAR_talkToCli(charindex, -1, "请稍候。", CHAR_COLORYELLOW);
   		sprintf(buf1, "C|%d|%s|1", fd, CHAR_getChar( charindex, CHAR_NAME));
   		lssproto_TD_send( mfd, -1, buf1);
   		sprintf(buf1, "C|%d|%s|1", mfd, CHAR_getChar( masterindex, CHAR_NAME));
   		lssproto_TD_send( fd, -1, buf1);

   		CHAR_setWorkInt( charindex, CHAR_WORKTRADEMODE, CHAR_TRADE_TRADING);
   		CHAR_setWorkInt( masterindex, CHAR_WORKTRADEMODE, CHAR_TRADE_TRADING);
		CONNECT_set_confirm( fd, FALSE);
		CONNECT_set_confirm( mfd, FALSE);
		CHAR_sendTradeEffect( charindex, 1);
		CHAR_sendTradeEffect( masterindex, 1);

		switch( flg )	{	//清除
		case TRADEITEMTYPE:	//CHAR_Trade
			if( (snum = MAPTRADE_getItemSpace( masterindex, index)) != -1 )	{
				sprintf( buf1, "T|%d|%s|I|1|%d", fd, CHAR_getChar( charindex, CHAR_NAME),
				snum );
				print(" MAP_TRADEPICKUP_check1 ");
				CHAR_Trade( mfd, masterindex, buf1);
			}

			snprintf(msg, sizeof(msg), "T|%d|%s|S|I|%d|%d", mfd, CHAR_getChar( masterindex, CHAR_NAME), snum, 1);
			lssproto_TD_send( mfd, -1, msg);
			break;
		case TRADEPETTYPE:
			if((snum = MAPTRADE_getPetSpace( masterindex, index)) != -1 )	{
				sprintf( buf1, "T|%d|%s|P|3|%d", fd, CHAR_getChar( charindex, CHAR_NAME),
				snum );
				print(" MAP_TRADEPICKUP_check2 ");
				CHAR_Trade( mfd, masterindex, buf1);
			}
			snprintf(msg, sizeof(msg), "T|%d|%s|S|P|%d|%d", mfd,
				CHAR_getChar( masterindex, CHAR_NAME), snum, 3);
			lssproto_TD_send( mfd, -1, msg);
			break;
		}
//=======================
	}
	return TRUE;
}


int TRADE_getMasterInt( int index)
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return -1;
	return TRADE_DATAS[index].masterindex;
}

int TRADE_getTimeInt( int index)
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return FALSE;
	return TRADE_DATAS[index].Ttime;
}

BOOL TRADE_setMasterInt( int index, int Num)
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return FALSE;
	TRADE_DATAS[index].masterindex = Num;
	return TRUE;
}

BOOL TRADE_setTimeInt( int index, int Num)
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return FALSE;
	TRADE_DATAS[index].Ttime = Num;
	return TRUE;
}

int TRADE_getMaxNumInt()
{
	return arraysizeof( TRADE_DATAS);
}

int TRADE_AddMasrerTrade( int toindex)
{	//设定摊位主人
	int i;
	if( CHAR_getWorkInt( toindex, CHAR_MAPTRADETYPE) > 0 )
		return -1;
	for( i=0; i< arraysizeof( TRADE_DATAS); i++)	{
		if( TRADE_DATAS[i].masterindex < 0 )	{
			TRADE_setMasterInt( i, toindex);
			TRADE_setTimeInt( i, NowTime.tv_sec);
			return i+TRADESTARTNUM;
		}
	}
	return -1;
}

void MAPTRADE_CLEANGOLD( int floor, int num)
{	//TRADE_DATAS
	OBJECT  object;
	int x,y;

	if( TRADE_DATAS[ num].masterindex != -1 )	{
		print("\n 删除摊位[%d], 主人[%d]存在!!", num, TRADE_DATAS[ num].masterindex);
		return;
	}
	for( x=TRADE_DATAS[ num].x; x<(TRADE_DATAS[ num].x+TRADEXSIZE); x++ )	{
		for( y=TRADE_DATAS[ num].y; y<(TRADE_DATAS[ num].y+TRADEYSIZE); y++)	{
			for( object=MAP_getTopObj(floor,x,y) ; object ; object =  NEXT_OBJECT(object ) )	{
				int index = GET_OBJINDEX(object);
				int objtype = OBJECT_getType(index);
				int objindex = OBJECT_getIndex( index);

				if( objtype == OBJTYPE_NOUSE )continue;
				if( objtype == OBJTYPE_ITEM ){
					if( ITEM_getWorkInt( objindex, ITEM_WORKTRADETYPE) == TRADETYPE_SELL )	{
						ITEM_endExistItemsOne( objindex);
						OBJECT_setIndex( index, 0);
						CHAR_ObjectDelete(index);	//清除
						print("clean( %d )", index );
					}
					break;
				}
				if( objtype == OBJTYPE_CHARA ){
					if( CHAR_getInt( objindex, CHAR_WHICHTYPE) == CHAR_TYPEPET)	{
						if( CHAR_getWorkInt( objindex, CHAR_WORKTRADETYP) == TRADETYPE_SELL )	{
							ITEM_endExistItemsOne( objindex);
							OBJECT_setIndex( index, 0);
							CHAR_CharaDelete( objindex);
						}
					}
				}
			}
		}
	}
}

BOOL MAPTRADE_CHECKMASTERIN( int masterindex , int toindex, int num)
{
	int exhibition = num;
	exhibition+=TRADESTARTNUM;
	if( !CHAR_CHECKINDEX( masterindex) ) return FALSE;
	if( CHAR_getWorkInt( masterindex, CHAR_MAPTRADETYPE) != exhibition )
		return FALSE;

	if( CHAR_getInt( masterindex, CHAR_FLOOR) != TRADEMAP )	{	//主人不在了
		CHAR_talkToCli( toindex, -1, "主人不在!", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		int mapnum = CHECKMAP_TRADEXY( masterindex, CHAR_getInt( masterindex, CHAR_FLOOR),
							CHAR_getInt( masterindex, CHAR_X),
							CHAR_getInt( masterindex, CHAR_Y));
		if( (mapnum == -1 ) ||
			CHAR_getWorkInt( masterindex, CHAR_MAPTRADETYPE) != (mapnum+TRADESTARTNUM) )	{
			CHAR_talkToCli( toindex, -1, "主人不在摊位上!", CHAR_COLORYELLOW);
			//主人不在位置上
			return FALSE;
		}
		
	}
	return TRUE;
}

BOOL MAPTRADE_CHECKMAPFULL(int fl, int x, int y)
{
	OBJECT  object;
	BOOL FIND=FALSE;
	for( object=MAP_getTopObj(fl,x,y) ; object ;
		object = NEXT_OBJECT(object ) ){
		int objindex = GET_OBJINDEX(object);
		int index = OBJECT_getIndex( objindex);

		switch( OBJECT_getType(objindex) ){
		case OBJTYPE_NOUSE:
			break;
		case OBJTYPE_CHARA:
			if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPET )	{
				break;
			}
		case OBJTYPE_ITEM:
			FIND = TRUE;
			break;
		}
	}
	return FIND;
}

BOOL MAP_TRADEPETDROP( int charindex, int petindex,int floor, int x, int y)
{
	int exhibition=-1;
	int Createpet=-1,petid=-1, petimage=-1;
	int objindex, i, enemynum=-1;
	char petname[256]="(样)";
	if( CHECKMAP_TRADE( charindex, floor, x, y) == FALSE )
		return FALSE;
	if( (exhibition = CHECKMAP_TRADEXY(charindex, floor, x, y)) == -1 )
		return FALSE;
	exhibition+=TRADESTARTNUM;

	if( CHAR_getWorkInt( charindex, CHAR_MAPTRADETYPE) != exhibition )	{
		print("\n 不是摊位[%d]主人[%d] ", exhibition, CHAR_getWorkInt( charindex, CHAR_MAPTRADETYPE));
		return TRUE;
	}

	if( MAPTRADE_CHECKMAPFULL( floor, x, y) == TRUE )	{
		CHAR_talkToCli( charindex, -1, "前方已有贩卖样品!", CHAR_COLORYELLOW);
		return TRUE;
	}


	petimage = CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER);
	if( CHAR_getChar( petindex, CHAR_NAME) != NULL )	{
		if( strstr( CHAR_getChar( petindex, CHAR_NAME), "(样)") == NULL )	{
			sprintf( petname ,"%s(样)", CHAR_getChar( petindex, CHAR_NAME) );
		}
	}

	//CREAT 贩卖样品	ENEMY_TEST_createPetIndex

	petid = CHAR_getInt( petindex, CHAR_PETID );
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID ) == petid ){//ENEMY_TEMPNO
			print("\n find pet[%d]", petid);
			break;
		}
	}
	if( i == enemynum )
		return FALSE;
	Createpet = ENEMY_TEST_createPetIndex( i);
	if( !CHAR_CHECKINDEX( Createpet ) ) return TRUE;
	//复制可视条件
	CHAR_setChar( Createpet, CHAR_NAME, petname);
	CHAR_setInt( Createpet, CHAR_BASEIMAGENUMBER, petimage);
	CHAR_setInt( Createpet, CHAR_BASEBASEIMAGENUMBER, petimage);

	//丢下宠物
	objindex = PET_dropPetAbsolute( Createpet,floor,x,y, FALSE );
	if( objindex == -1 )	{
		CHAR_endCharOneArray( Createpet );
		return TRUE;
	}

	CHAR_setWorkInt( Createpet, CHAR_WORKOBJINDEX, objindex );
	CHAR_setWorkInt( Createpet, CHAR_WORKTRADETYP, TRADETYPE_SELL);
	CHAR_setWorkInt( Createpet, CHAR_WORKTRADESELLINDEX, petindex);

	CHAR_setInt( Createpet, CHAR_ALLOCPOINT, TRADEPETUPLV );
	CHAR_setChar( Createpet, CHAR_OWNERCDKEY, "WAEI_SYSOWN");
    CHAR_setChar( Createpet, CHAR_OWNERCHARANAME, "WAEI_SYSOWN");	
	CHAR_setInt( Createpet, CHAR_FLOOR, floor);
	CHAR_setInt( Createpet, CHAR_X, x);
	CHAR_setInt( Createpet, CHAR_Y, y);
	CHAR_setInt( Createpet, CHAR_PUTPETTIME, NowTime.tv_sec);
	CHAR_sendCToArroundCharacter( objindex);
	TRADE_DATAS[exhibition-TRADESTARTNUM ].Goodindex = Createpet;

	sprintf( TRADE_DATAS[exhibition-TRADESTARTNUM ].Goodname,"宠物-%s", petname);
	return TRUE;
}

int MAPTRADE_getItemSpace( int meindex, int itemindex)
{
	int i;
	int index = ITEM_getWorkInt( itemindex, ITEM_WORKTRADESELLINDEX);
	for( i=CHAR_EQUIPPLACENUM; i<CHAR_MAXITEMHAVE; i++)	{
		if( index == CHAR_getItemIndex( meindex, i) )	{
			return i;
		}
	}
	return -1;
}

int MAPTRADE_getPetSpace( int meindex, int petindex)
{
	int i;
	int index = CHAR_getWorkInt( petindex, CHAR_WORKTRADESELLINDEX);
	for( i=0; i<CHAR_MAXPETHAVE; i++)	{
		if( index == CHAR_getCharPet( meindex, i) )	{
			return i;
		}
	}
	return -1;
}

int MAPTRADE_getSellIndex( int index)
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return -1;
	return TRADE_DATAS[index].Goodindex;
}

BOOL MAPTRADE_setSellIndex( int index, int num)
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return FALSE;
	TRADE_DATAS[index].Goodindex = num;
	return TRUE;
}

void MAPTRADE_setCharSellName( int index, char *buf)
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return;
	if( buf == NULL )
		return;
	snprintf( TRADE_DATAS[index].Goodname, sizeof( TRADE_DATAS[index].Goodname), buf);
}

char* MAPTRADE_getCharSellName( int index )
{
	if( index < 0 || index >= arraysizeof( TRADE_DATAS) )
		return NULL;
	return TRADE_DATAS[index].Goodname;
}
#endif




