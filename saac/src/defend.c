#include "version.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "saacproto_serv.h"
#include "main.h"
#include "char.h"
#include "util.h"
#include "defend.h"

#ifdef _DEFEND_NEWUSERS
extern gmsv gs[MAXCONNECTION];

#define DEFEND_FILENAME "db/defend/defend.txt"
#define SCORE_FILENAME	"db/defend/score.txt"

DefendArrayList *DEFENDUserList = NULL;
ScoreArrayList *DEFENDScoreList = NULL;

static int DEFEND_MAXSCORE = 0;
static int DEFEND_MINSCORE = 0;
void DEFEND_InitList()
{
	log("DEFEND_InitList( %d) - %4.2f KB.. \n", DEFEND_MAXLISTS,
		(float)(sizeof( struct _tagDefendArrayList)*DEFEND_MAXLISTS)/1024 );

	DEFENDUserList = (DefendArrayList*)calloc( 1, sizeof( struct _tagDefendArrayList)*DEFEND_MAXLISTS);
	
	if( DEFENDUserList == NULL ){
		log("Can't calloc sizeof(%4.2fKB.)err !!\n",
			(float)(sizeof( struct _tagDefendArrayList)*DEFEND_MAXLISTS)/1024 );
		return;
	}

	DEFENDScoreList = (ScoreArrayList*)calloc( 1, sizeof( struct _tagScoreArrayList)*DEFEND_SCOREMAXLISTS);
	if( DEFENDScoreList == NULL ){
		log("Can't calloc sizeof(%4.2fKB.)err !!\n",
			(float)(sizeof( struct _tagDefendArrayList)*DEFEND_SCOREMAXLISTS)/1024 );
		return;
	}

	DEFEND_resetUserForList();
	DEFEND_ReadUserForList( DEFEND_FILENAME);
	DEFEND_resetScoreForList();
	DEFEND_ReadScoreForList( SCORE_FILENAME);
}

int DEFEND_getUse( int si)
{
	if( si >= DEFEND_MAXLISTS || si < 0 ) return -1;
	return DEFENDUserList[ si].use;
}

int DEFEND_setUse( int si, int num)
{
	int olds;
	if( si >= DEFEND_MAXLISTS || si < 0 ) return -1;
	olds = DEFENDUserList[ si].use;
	DEFENDUserList[ si].use = num;
	return olds;
}

void DEFEND_resetUserForList()
{
	int i;
	for( i=0; i<DEFEND_MAXLISTS; i++)	{
		memset( DEFENDUserList[ i].cdkey, 0, sizeof( DEFENDUserList[ i].cdkey) );
		memset( DEFENDUserList[ i].name, 0, sizeof( DEFENDUserList[ i].name) );
		DEFENDUserList[ i].defnums = 0;	//次数
		DEFENDUserList[ i].type = 0;
		DEFENDUserList[ i].score = 0;
		DEFENDUserList[ i].use = 0;
	}
	DEFEND_MAXSCORE = 0;
	DEFEND_MINSCORE = 0;
}
void DEFEND_copyOneUserForList( int si, int pi)
{
	memcpy( DEFENDUserList[ si].cdkey,
		DEFENDUserList[ pi].cdkey, sizeof( DEFENDUserList[ si].cdkey) );
	memcpy( DEFENDUserList[ si].name,
		DEFENDUserList[ pi].name, sizeof( DEFENDUserList[ si].name) );
	DEFENDUserList[ si].defnums = DEFENDUserList[ pi].defnums;	//次数
	DEFENDUserList[ si].type = DEFENDUserList[ pi].type;
	DEFENDUserList[ si].score = DEFENDUserList[ pi].score;
	DEFENDUserList[ si].use = DEFENDUserList[ pi].use;
}

void DEFEND_delOneUserForList( int si)
{
	memset( DEFENDUserList[ si].cdkey, 0, sizeof( DEFENDUserList[ si].cdkey) );
	memset( DEFENDUserList[ si].name, 0, sizeof( DEFENDUserList[ si].name) );
	DEFENDUserList[ si].defnums = 0;	//次数
	DEFENDUserList[ si].type = 0;
	DEFENDUserList[ si].score = 0;
	DEFENDUserList[ si].use = 0;
}
int DEFEND_addUserForList( char *cdkey, char *name, int score, int nums, int type)
{
	int ti=-1;
	int i,j;
	if( cdkey == NULL ) return -1;
	//清空相同帐号&名称
	for( i=0; i<DEFEND_MAXLISTS; i++){
		if( DEFEND_getUse( i) <= 0 ) continue;
		if( !strcmp( DEFENDUserList[ i].cdkey, cdkey) && 
			!strcmp( DEFENDUserList[ i].name, name) ){
			DEFEND_delOneUserForList( i);
			for( j=i; j<DEFEND_MAXLISTS; j++){
				if( DEFEND_getUse( j+1) <= 0 ){
					if( DEFEND_getUse( j) > 0 ){
						DEFEND_delOneUserForList( j);
					}
					continue;
				}
				DEFEND_copyOneUserForList( j, j+1);
			}
		}
	}
	for( i=0; i<DEFEND_MAXLISTS; i++)	{
		if( DEFEND_getUse( i) <= 0 ) break;
		if( score < DEFENDUserList[ i].score ) continue;
		break;
	}

	if( i >= DEFEND_MAXLISTS ) return -1;
	ti = i;

	for( i=(DEFEND_MAXLISTS-1); i>ti; i--){
		if( DEFEND_getUse( i-1) <= 0 ) continue;
		DEFEND_copyOneUserForList( i, i-1);
	}

	memcpy( DEFENDUserList[ ti].cdkey, cdkey, sizeof( DEFENDUserList[ ti].cdkey) );
	memcpy( DEFENDUserList[ ti].name, name, sizeof( DEFENDUserList[ ti].name) );
	DEFENDUserList[ ti].defnums = nums;	//次数
	DEFENDUserList[ ti].type = type;
	DEFENDUserList[ ti].score = score;
	DEFENDUserList[ ti].use = 1;
	return ti;
}

void DEFEND_LoadUserForList( char *listarg)
{
	char buf2[256];
	char cdkey[256], name[256];
	int type, nums, score;

	easyGetTokenFromBuf( listarg, ',', 1, buf2, sizeof(buf2));
	memcpy( cdkey, buf2, sizeof( cdkey));
	easyGetTokenFromBuf( listarg, ',', 2, buf2, sizeof(buf2));
	memcpy( name, buf2, sizeof( name));
	easyGetTokenFromBuf( listarg, ',', 3, buf2, sizeof(buf2));
	nums = atoi( buf2);
	easyGetTokenFromBuf( listarg, ',', 4, buf2, sizeof(buf2));
	score = atoi( buf2);
	easyGetTokenFromBuf( listarg, ',', 5, buf2, sizeof(buf2));
	type = atoi( buf2);

	if( DEFEND_addUserForList( cdkey, name, score, nums, type) != -1 ){
		DEFEND_ShowUserLists();
		DEFEND_SendToUserLists();
	}
}

void DEFEND_ReadUserForList( char *filename)
{
	FILE *fp=NULL;
	char buf1[256], buf2[256];
	char cdkey[256], name[256];
	int type, nums, score;
	fp = fopen( filename, "r");
	if( fp == NULL ){
		log( "Can't Read %s..!\n", filename);
		return;
	}

	while( fgets( buf1 , sizeof( buf1 ) , fp )){
		easyGetTokenFromBuf( buf1, ',', 1, buf2, sizeof(buf2));
		memcpy( cdkey, buf2, sizeof( cdkey));
		easyGetTokenFromBuf( buf1, ',', 2, buf2, sizeof(buf2));
		memcpy( name, buf2, sizeof( name));
		easyGetTokenFromBuf( buf1, ',', 3, buf2, sizeof(buf2));
		nums = atoi( buf2);
		easyGetTokenFromBuf( buf1, ',', 4, buf2, sizeof(buf2));
		score = atoi( buf2);
		easyGetTokenFromBuf( buf1, ',', 5, buf2, sizeof(buf2));
		type = atoi( buf2);

		DEFEND_addUserForList( cdkey, name, score, nums, type);
	}
	fclose( fp);
	DEFEND_ShowUserLists();
}

void DEFEND_ShowUserLists()
{
	int i;
	if( DEFENDUserList == NULL ) return;
	for( i=0; i<DEFEND_MAXLISTS; i++ ){
		if( DEFEND_getUse( i) <= 0 ) continue;
		log("ANDY DEFENDUserList[%d][%s,%s,%d,%d,%d,%d]\n",
			i,
			DEFENDUserList[ i].cdkey, DEFENDUserList[ i].name,
			DEFENDUserList[ i].defnums, DEFENDUserList[ i].score,
			DEFENDUserList[ i].type, DEFENDUserList[ i].use	);
	}
}

void DEFEND_SendToUserLists()
{
	char token[4096];
	char buf1[256];
	int i;
	memset( token, 0, sizeof( token));
	for( i=0; i<DEFEND_MAXLISTS; i++ ){
		if( DEFEND_getUse( i) <= 0 ) continue;
		sprintf( buf1, "%s,%s,%d,%d,%d",
			DEFENDUserList[ i].cdkey, DEFENDUserList[ i].name,
			DEFENDUserList[ i].defnums, DEFENDUserList[ i].score, DEFENDUserList[ i].type);
		strcat( buf1, "|");
		strcat( token, buf1);
	}
	for(i=0; i < MAXCONNECTION; i++ ){
		if (gs[i].use && gs[i].name[0]) {
			saacproto_DefendList_send( i, 1, token);
		}
	}
	DEFEND_backupUseLists( DEFEND_FILENAME);

}

void DEFEND_backupUseLists( char *filename)
{
	int i;
	FILE *fp = NULL;

	remove( filename);
	fp = fopen( filename, "a");
	if( fp == NULL ){
		log("create file:%s error !!\n", filename);
		return;
	}
	for( i=0; i<DEFEND_MAXLISTS; i++)	{
		if( DEFEND_getUse( i) <= 0 ) continue;
		fprintf( fp,"%s,%s,%d,%d,%d\n",
			DEFENDUserList[ i].cdkey, DEFENDUserList[ i].name,
			DEFENDUserList[ i].defnums, DEFENDUserList[ i].score,
			DEFENDUserList[ i].type );
	}
	fclose( fp);
	log("backup file:%s..\n", filename);
}


void DEFEND_resetScoreForList()
{
	int i;
	for( i=0; i<DEFEND_SCOREMAXLISTS; i++)	{
		memset( DEFENDScoreList[ i].cdkey, 0, sizeof( DEFENDScoreList[ i].cdkey) );
		memset( DEFENDScoreList[ i].name, 0, sizeof( DEFENDScoreList[ i].name) );
		DEFENDScoreList[ i].use = 0;
	}
}

void DEFEND_LoadScoreForList( char *listarg)
{
	char buf2[256];
	char cdkey[256], name[256], fromname[256];
	int score;

	easyGetTokenFromBuf( listarg, ',', 1, buf2, sizeof(buf2));
	memcpy( cdkey, buf2, sizeof( cdkey));
	easyGetTokenFromBuf( listarg, ',', 2, buf2, sizeof(buf2));
	memcpy( name, buf2, sizeof( name));
	easyGetTokenFromBuf( listarg, ',', 3, buf2, sizeof(buf2));
	memcpy( fromname, buf2, sizeof( fromname));
	easyGetTokenFromBuf( listarg, ',', 4, buf2, sizeof(buf2));
	score = atoi( buf2);

	if( DEFEND_addScoreForList( cdkey, name, score, fromname) < 1 ){
	}
	DEFEND_backupScoreLists( SCORE_FILENAME);
	DEFEND_ShowScoreLists();

}

int DEFEND_addScoreForList( char *cdkey, char *name, int score, char *fromname )
{
	int freeindex=-1;
	int i;
	if( cdkey == NULL ) return -1;
	if( name == NULL ) return -1;

	for( i=0; i<DEFEND_SCOREMAXLISTS; i++){
		if( DEFEND_getScoreUse( i) <= 0 ){
			freeindex = i;
			continue;
		}
/*
		if( !strcmp( DEFENDScoreList[ i].cdkey, cdkey) && 
			!strcmp( DEFENDScoreList[ i].name, name) ){
			DEFENDScoreList[ i].score += score;
			return 1;
		}
*/
	}
	if( freeindex == -1 ) return -1;


	memcpy( DEFENDScoreList[ freeindex].cdkey, cdkey, sizeof( DEFENDScoreList[ freeindex].cdkey) );
	memcpy( DEFENDScoreList[ freeindex].name, name, sizeof( DEFENDScoreList[ freeindex].name) );
	memcpy( DEFENDScoreList[ freeindex].fromname, fromname, sizeof( DEFENDScoreList[ freeindex].fromname) );
	DEFENDScoreList[ freeindex].score = score;
	DEFENDScoreList[ freeindex].use = 1;

	return 1;
}

int DEFEND_getScoreUse( int si)
{
	if( si >= DEFEND_SCOREMAXLISTS || si < 0 ) return -1;
	return DEFENDScoreList[ si].use;
}

void DEFEND_ReadScoreForList( char *filename)
{
	FILE *fp=NULL;
	char buf1[256], buf2[256];
	char cdkey[256], name[256], fromname[256];
	int score;
	fp = fopen( filename, "r");
	if( fp == NULL ){
		log( "Can't Read %s..!\n", filename);
		return;
	}

	while( fgets( buf1 , sizeof( buf1 ) , fp )){
		int i;
		if( strlen(buf1) < 1 || strlen(buf1) > 512 ) continue;
		for( i=0; i<strlen(buf1); i++){
			if( buf1[i] == '\n' ) buf1[1] = '\0';
		}
		easyGetTokenFromBuf( buf1, ',', 1, buf2, sizeof(buf2));
		memcpy( cdkey, buf2, sizeof( cdkey));
		easyGetTokenFromBuf( buf1, ',', 2, buf2, sizeof(buf2));
		memcpy( name, buf2, sizeof( name));
		easyGetTokenFromBuf( buf1, ',', 3, buf2, sizeof(buf2));
		memcpy( fromname, buf2, sizeof( fromname));
		easyGetTokenFromBuf( buf1, ',', 4, buf2, sizeof(buf2));
		score = atoi( buf2);
		DEFEND_addScoreForList( cdkey, name, score, fromname);
	}
	fclose( fp);
}

void DEFEND_backupScoreLists( char *filename)
{
	int i;
	FILE *fp = NULL;

	remove( filename);
	fp = fopen( filename, "a");
	if( fp == NULL ){
		log("create file:%s error !!\n", filename);
		return;
	}
	for( i=0; i<DEFEND_SCOREMAXLISTS; i++)	{
		if( DEFEND_getScoreUse( i) <= 0 ) continue;
		fprintf( fp,"%s,%s,%s,%d\n",
			DEFENDScoreList[ i].cdkey, DEFENDScoreList[ i].name,
			DEFENDScoreList[ i].fromname,
			DEFENDScoreList[ i].score );
	}
	fclose( fp);
	log("backup file:%s..\n", filename);
}

void DEFEND_ShowScoreLists()
{
	int i;
	if( DEFENDScoreList == NULL ) return;
	for( i=0; i<DEFEND_SCOREMAXLISTS; i++ ){
		if( DEFEND_getScoreUse( i) <= 0 ) continue;
		log("ANDY DEFENDScoreList[%d][%s,%s,%d,%s,%d]\n",
			i,
			DEFENDScoreList[ i].cdkey, DEFENDScoreList[ i].name,
			DEFENDScoreList[ i].score, DEFENDScoreList[ i].fromname,
			DEFENDScoreList[ i].use	);
	}
}

void DEFEND_CheckScoreForList( int fd, int userindex, char *cdkey, char *name)
{
	int i;
	if( DEFENDScoreList == NULL ) return;
	for( i=0; i<DEFEND_SCOREMAXLISTS; i++ ){
		if( DEFEND_getScoreUse( i) <= 0 ) continue;
		if( !strcmp( cdkey, DEFENDScoreList[ i].cdkey) &&
			!strcmp( name, DEFENDScoreList[ i].name) ){
			saacproto_CheckDefendScore_send(
				fd,
				DEFENDScoreList[ i].fromname,
				userindex,
				DEFENDScoreList[ i].score );
			memset( DEFENDScoreList[ i].cdkey, 0, sizeof( DEFENDScoreList[ i].cdkey));
			memset( DEFENDScoreList[ i].name, 0, sizeof( DEFENDScoreList[ i].name));
			DEFENDScoreList[ i].use = 0;
		}
	}
}

#endif
