#ifndef __NPC_AUCTIONEER_H__
#define __NPC_AUCTIONEER_H__

#ifdef _AUCTIONEER

// 以下这两个常数要跟 saac/auction.h 里面的定义一致
#define MAX_AUCTION	100	// 最大的委托数
#define MAX_OVERDUE	500	// 过期的委托单保留数 (最小设定=过期保留天数*最大委托数)
#define AUC_PET     1
#define AUC_ITEM    2

typedef struct tagAuctionTable {
	int  flag;		      // 0=(null) 1=使用中 2=过期
	char cdkey[32];		  // 委托人 cdkey
	char customer[32];	// 委托人人物名称
	int itemtype;		    // 1=宠物 2=道具 otherwise=error
	char goods[4096];	  // 物品 (宠物 or 道具)
	char description[256];	// 叙述
  char listdata[512]; // 显示在表单的资料
	int price;		      // 标价
  int onsaletime;
  int overduetime;
} AuctionTable;

extern AuctionTable onsale[MAX_AUCTION];	// 拍卖中的道具、宠物
//extern AuctionTable overdue[MAX_OVERDUE];	// 过期的道具、宠物

BOOL NPC_AuctioneerInit(int meindex);
void NPC_AuctioneerTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_AuctioneerWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_AuctioneerLoop(int meindex);

#endif /* _AUCTIONEER */

#endif 
