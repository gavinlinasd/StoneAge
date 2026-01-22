#ifndef __NPC_AUCTIONEER_H__
#define __NPC_AUCTIONEER_H__

#ifdef _AUCTIONEER

// 眕狟涴謗跺都杅猁躲 saac/auction.h 爵醱腔隅砱珨祡
#define MAX_AUCTION	100	// 郔湮腔巹迖杅
#define MAX_OVERDUE	500	// 徹ぶ腔巹迖等悵隱杅 (郔苤扢隅=徹ぶ悵隱毞杅*郔湮巹迖杅)
#define AUC_PET     1
#define AUC_ITEM    2

typedef struct tagAuctionTable {
	int  flag;		      // 0=(null) 1=妏蚚笢 2=徹ぶ
	char cdkey[32];		  // 巹迖�� cdkey
	char customer[32];	// 巹迖�侒冼屪�備
	int itemtype;		    // 1=唾昜 2=耋撿 otherwise=error
	char goods[4096];	  // 昜こ (唾昜 or 耋撿)
	char description[256];	// 唦扴
  char listdata[512]; // 珆尨婓桶等腔訧蹋
	int price;		      // 梓歎
  int onsaletime;
  int overduetime;
} AuctionTable;

extern AuctionTable onsale[MAX_AUCTION];	// 鼴闖笢腔耋撿﹜唾昜
//extern AuctionTable overdue[MAX_OVERDUE];	// 徹ぶ腔耋撿﹜唾昜

BOOL NPC_AuctioneerInit(int meindex);
void NPC_AuctioneerTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_AuctioneerWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_AuctioneerLoop(int meindex);

#endif /* _AUCTIONEER */

#endif 
