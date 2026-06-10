#include <stdlib.h>
#include "version.h"
#include "link.h"
#include "buf.h"

/*
 *  伉件弁及  卞勾仃月
 * 引数
 *      top         トップノード
 *      add         尥仃笛尹月用□玉及    毛忡  允月用□玉尺及禾奶件正□
 * 返り値
 *      TRUE(1)     岳  
 *      FALSE(0)    撩  
 */
BOOL Nodeappendtail( Node** top  , Node* add )
{
    Node* c;        /*郊ープ用*/
    Node* next;     /*新しく作るノード用*/

    /*トップがNULLかどうかのチェック*/
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
    c->next = next;                         /* nextに値を設定する */
    next->next  = NULL;                      /*粮五反  中*/
    next->val   = add->val;                   /*禾奶件正□及戊疋□*/
    next->size  = add->size;             /*赢今及忡栋*/
    return TRUE;
}




/*
 *  伉件弁及蟆卞勾仃月
 * 引数
 *      top         トップノード
 *      add         尥仃笛尹月用□玉及    毛忡  允月用□玉尺及禾奶件正□
 * 返り値
 *      TRUE(1)     岳  
 *      FALSE(0)    撩  
 */
BOOL Nodeappendhead( Node** nowtop  , Node* add )
{
    Node* newtop;       /*新しい先  用*/

    /*トップがNULLかどうかのチェック*/
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
 * 一    初のノードを引数にコピーして取り除く
 * 引数
 *          top         用□玉及燮  
 *          ret         潸曰轮中凶用□玉及戊疋□
 * 返り値
 *      TRUE(1)         岳  
 *      FALSE(0)        撩      用□玉及燮  互NULL
 */
BOOL  Noderemovehead( Node** top , Node* ret)
{
    Node* newtop;         /*新しく先  になるノード*/

    if( *top == NULL )return FALSE;

    ret->val    = (*top)->val;
    ret->size   = (*top)->size;
    newtop = (*top)->next;
    freeMemory( *top );
    *top = newtop;

    return TRUE;
}





/*
 * 一      のノードを引数にコピーして取り除く
 * 引数
 *          top         用□玉及燮  
 *          ret         潸曰轮中凶用□玉及戊疋□
 * 返り値
 *      TRUE(1)         岳  
 *      FALSE(0)        撩      用□玉及燮  互NULL
 */
BOOL  Noderemovetail( Node** top , Node* ret)
{
    Node* c;             /*郊ープ用*/
    Node* c1;            /*郊ープ用 いつでも c->nextを指す*/

    if( *top == NULL )return FALSE;

    c = *top;           /*瞹嚆粟必醒*/
    c1 = c->next;       /*瞹嚆粟必醒*/
    while(1){
        if( c1->next == NULL )
            /*
             * この時点で          c1
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
    /*コピー*/
    ret->val    = c1->val;
    ret->size   = c1->size;

    freeMemory( c1 );

    return TRUE;
}
