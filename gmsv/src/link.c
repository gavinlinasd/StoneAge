#include <stdlib.h>
#include "version.h"
#include "link.h"
#include "buf.h"

/*
 *  伉件弁及  卞勾仃月
 * 娄醒
 *      top         玄永皿用□玉
 *      add         尥仃笛尹月用□玉及    毛忡  允月用□玉尺及禾奶件正□
 * 忒曰袄
 *      TRUE(1)     岳  
 *      FALSE(0)    撩  
 */
BOOL Nodeappendtail( Node** top  , Node* add )
{
    Node* c;        /*伙□皿迕*/
    Node* next;     /*蕙仄仁综月用□玉迕*/

    /*玄永皿互NULL井升丹井及民尼永弁*/
    if( *top == NULL ){
        *top = allocateMemory( sizeof( Node  ) );
        if( *top== NULL ) return FALSE;
        (*top)->next    = NULL;            /*粮五反  中*/
        (*top)->size    = add->size;   /*赢今及忡栋*/
        (*top)->val     = add->val;         /*禾奶件正□及戊疋□*/
        return TRUE;
    }

    for( c = *top ; c->next ; c = c->next ); /* c及匏  毛内日允 */
    next = allocateMemory( sizeof(Node) );
    if( next == NULL )return FALSE;
    c->next = next;                         /* next卞袄毛涩烂允月 */
    next->next  = NULL;                      /*粮五反  中*/
    next->val   = add->val;                   /*禾奶件正□及戊疋□*/
    next->size  = add->size;             /*赢今及忡栋*/
    return TRUE;
}




/*
 *  伉件弁及蟆卞勾仃月
 * 娄醒
 *      top         玄永皿用□玉
 *      add         尥仃笛尹月用□玉及    毛忡  允月用□玉尺及禾奶件正□
 * 忒曰袄
 *      TRUE(1)     岳  
 *      FALSE(0)    撩  
 */
BOOL Nodeappendhead( Node** nowtop  , Node* add )
{
    Node* newtop;       /*蕙仄中燮  迕*/

    /*玄永皿互NULL井升丹井及民尼永弁*/
    if( *nowtop == NULL ){
        *nowtop = allocateMemory( sizeof( Node  ) );
        if( *nowtop == NULL ) return FALSE;
        (*nowtop)->next = NULL;             /*粮五反  中*/
        (*nowtop)->size = add->size;        /*赢今及忡栋*/
        (*nowtop)->val  = add->val;         /*禾奶件正□及戊疋□*/
        return TRUE;
    }

    /*
     * 蕙仄中用□玉毛域勾综月
     * next 互犒允手及毛 nowtop 卞允月［
     * nowtop 卞反 割忡仄凶 newtop 毛医  允月［
     */
    newtop = allocateMemory( sizeof(Node) );
    newtop->next    = *nowtop;
    newtop->val     = add->val;
    newtop->size    = add->size;
    *nowtop = newtop;
    return TRUE;
}




/*
 * 域    赓及用□玉毛娄醒卞戊疋□仄化潸曰轮仁
 * 娄醒
 *          top         用□玉及燮  
 *          ret         潸曰轮中凶用□玉及戊疋□
 * 忒曰袄
 *      TRUE(1)         岳  
 *      FALSE(0)        撩      用□玉及燮  互NULL
 */
BOOL  Noderemovehead( Node** top , Node* ret)
{
    Node* newtop;         /*蕙仄仁燮  卞卅月用□玉*/

    if( *top == NULL )return FALSE;

    ret->val    = (*top)->val;
    ret->size   = (*top)->size;
    newtop = (*top)->next;
    freeMemory( *top );
    *top = newtop;

    return TRUE;
}





/*
 * 域      及用□玉毛娄醒卞戊疋□仄化潸曰轮仁
 * 娄醒
 *          top         用□玉及燮  
 *          ret         潸曰轮中凶用□玉及戊疋□
 * 忒曰袄
 *      TRUE(1)         岳  
 *      FALSE(0)        撩      用□玉及燮  互NULL
 */
BOOL  Noderemovetail( Node** top , Node* ret)
{
    Node* c;             /*伙□皿迕*/
    Node* c1;            /*伙□皿迕 中勾匹手 c->next毛隙允*/

    if( *top == NULL )return FALSE;

    c = *top;           /*赓渝袄涩烂*/
    c1 = c->next;       /*赓渝袄涩烂*/
    while(1){
        if( c1->next == NULL )
            /*
             * 仇及凛鳔匹          c1
             *                      |
             *  c ---> +------+       
             *         | next |---->+------+
             *         |------|     | next |---->NULL
             *         |      |     +------+
             *         +------+     |      |
             *                      +------+
             *午卅匀化中月
             */
            break;
        c=c->next;
        c1=c->next;
    }
    c->next = NULL;     /*c1卞丐凶月手及毛绰轮允月及匹next反NULL午允月*/
    /*戊疋□*/
    ret->val    = c1->val;
    ret->size   = c1->size;

    freeMemory( c1 );

    return TRUE;
}
