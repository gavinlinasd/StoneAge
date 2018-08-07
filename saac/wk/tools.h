#define ID_SIZE 22

typedef struct STOCK {
	struct STOCK *link;
	char *data;
	int flag;
} Stock;

#define STOCK_START 1
#define STOCK_NONTERM 2

typedef struct STOCK_STRUCT {
	struct STOCK *head; // First item to be used
	struct STOCK *tail; // Newest item
	int count;
} Stock_Struct;

typedef struct RANK_ENTRY {
	char name[ID_SIZE];
	int score;
} Rank_Entry;

typedef struct RANK_STRUCT {
	struct RANK_ENTRY entry[10];
	int rank;
} Rank_Struct;

int bakayaro(char *name);

char *iptoa(unsigned long ip);
char *strxcpy(char *src);
char *xchange(char *src);

void stockInit(Stock_Struct *s);
void stockAdd(Stock_Struct *s,char *data);
char *stockOut(Stock_Struct *s);
void stockDel(Stock_Struct *s,int count);
int stockCount(Stock_Struct *s);

void rankAdd(Stock_Struct *ss,int rank,Rank_Struct *rs);
int rankSelect(int rank);

int charXCount(char *buf);
char *construct(char *src);
void show(Stock_Struct *s);

extern char Xtoken;
extern char Ytoken;
extern char welcome[];


