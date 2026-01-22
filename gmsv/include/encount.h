#ifndef __ENCOUNT_H__
#define __ENCOUNT_H__

#define		ENCOUNT_GROUPMAXNUM		10

BOOL ENCOUNT_initEncount( char* filename );
BOOL ENCOUNT_reinitEncount( void );
int ENCOUNT_getEncountAreaArray( int floor, int x, int y);
int ENCOUNT_getEncountPercentMin( int charaindex, int floor , int x, int y );
int ENCOUNT_getEncountPercentMax( int charaindex, int floor , int x, int y );
int ENCOUNT_getCreateEnemyMaxNum( int floor , int x, int y );
int ENCOUNT_getEncountIndex( int floor , int x, int y );
int ENCOUNT_getEncountIndexFromArray( int array );
int ENCOUNT_getEncountPercentFromArray( int array );
int ENCOUNT_getCreateEnemyMaxNumFromArray( int array );
int ENCOUNT_getGroupIdFromArray( int array, int grouparray );
int ENCOUNT_getGroupProbFromArray( int array, int grouparray );
int ENCOUNT_getZorderFromArray( int array );

#ifdef _ADD_ENCOUNT           // WON ADD 崝樓菩婈郣揖楷党璃
typedef struct tagENCOUNT_Table
{
    int                 index;
    int                 floor;
    int                 encountprob_min;                /* 操璃庈坳璃哱賃   */
    int                 encountprob_max;                /* 操璃庈坳璃哱賃   */
    int                 enemymaxnum;        /* 汔躂煦崹傍禱軘堎凝 */
    int                 zorder;
    int                 groupid[ENCOUNT_GROUPMAXNUM];       /* 精鳴↓鏤No */
    int                 createprob[ENCOUNT_GROUPMAXNUM];    /* 鼠摯精鳴↓鏤摯③藯   */
	int					event_now;			
	int					event_end;
	int					enemy_group;		// 墅昜腔group 晤瘍
    RECT                rect;
}ENCOUNT_Table;

ENCOUNT_Table           *ENCOUNT_table;
#endif

#endif
