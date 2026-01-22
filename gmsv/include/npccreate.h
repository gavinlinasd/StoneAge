#ifndef __READNPCCREATE_H__
#define __READNPCCREATE_H__

#include "util.h"

#define NPC_CREATEFILEMAGIC   "NPCCREATE\n"


#undef EXTERN
#ifdef __NPCCREATE__
#define EXTERN
#else
#define EXTERN extern
#endif /*__NPCCREATE__*/


typedef enum
{
    NPC_CREATEFLOORID,      /*  啞痲囮ID    */
    NPC_CREATEBORNLEFTUPX,        /*  嫌窀摯X淢   */
    NPC_CREATEBORNLEFTUPY,        /*  嫌窀摯Y淢   */
    NPC_CREATEBORNRIGHTDOWNX,     /*  蒟き摯X淢   */
    NPC_CREATEBORNRIGHTDOWNY,     /*  蒟き摯Y淢   */

    NPC_CREATEMOVELEFTUPX,        /*  嫌窀摯X淢   */
    NPC_CREATEMOVELEFTUPY,        /*  嫌窀摯Y淢   */
    NPC_CREATEMOVERIGHTDOWNX,     /*  蒟き摯X淢   */
    NPC_CREATEMOVERIGHTDOWNY,     /*  蒟き摯Y淢   */

    NPC_CREATEDIR,                /*  軘堎鄹摯  澺    */
    NPC_CREATEBASEIMAGENUMBER,    /*  template 摯优擭禱窀怳拻埰堎
                                        鴢  蠕    */
    NPC_CREATETIME,               /* 鄹蹬 */
    NPC_CREATEBORNNUM,            /*    鼠摯貍  勗③藯踏鞠堎倳    */

    NPC_CREATEENEMYNUM,           /*  傍摯攣  倳  */
    NPC_CREATEBOUNDARY,
    NPC_CREATEDATE,
    NPC_CREATEIGNOREINVINCIBLE,
    NPC_CREATEFAMILY,		// Robin 0731 Family
#ifdef _ADD_ACTION          //jeffrey 1231 npcaction
	NPC_CREATEACTION,
#endif
    NPC_CREATEINTNUM,
    
}NPC_CREATEINT;

typedef enum
{
    NPC_CREATENAME,             /* template禱窀怳拻埰堎凝忒媃躂埵笢  鞳*/
    NPC_CREATECHARNUM,
}NPC_CREATECHAR;


typedef enum
{
    NPC_CREATEWORKENEMYNUM,
    NPC_CREATEWORKMAKESTARTSEC,
    NPC_CREATEWORKMAKESTARTUSEC,
    NPC_CREATEWORKNEVERMAKE,
    NPC_CREATEWORKNUM,

}NPC_CREATEWORK;

typedef struct tagNPC_Create
{
    int         intdata[NPC_CREATEINTNUM];
    STRING64    chardata[NPC_CREATECHARNUM];
    int         workdata[NPC_CREATEWORKNUM];
    int         templateindex[8];
    STRING64    arg[8];
}NPC_Create;


EXTERN NPC_Create*     NPC_create;
EXTERN int             NPC_createnum;
EXTERN int             NPC_create_readindex;

INLINE int NPC_CHECKCREATEINDEX(int index);

BOOL NPC_isBoundarySet( int createindex );
BOOL NPC_createGetRECT( int createindex,RECT*   r );
BOOL NPC_readNPCCreateFiles( char* topdirectory ,int createsize);
int NPC_getCreateInt( int index , NPC_CREATEINT element );

BOOL NPC_createCheckGenerateFromTime( int tindex );
void NPC_createInitTime( int index );
void NPC_createIncreaseEnemynum( int index );
void NPC_createDecreaseEnemynum( int index );
BOOL NPC_createCheckMaxEnemynum( int index );

#endif 
/*__READNPCCREATE_H__*/
