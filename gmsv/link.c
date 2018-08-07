#include <stdlib.h>
#include "version.h"
#include "link.h"
#include "buf.h"

/*
 *  Øø¼þÛÍ¼°  ±å¹´ØêÔÂ
 * Â¦ÐÑ
 *      top         ÐþÓÀÃóÓÃ¡õÓñ
 *      add         ÞÍØêµÑÒüÔÂÓÃ¡õÓñ¼°    Ã«âç  ÔÊÔÂÓÃ¡õÓñ³ß¼°ºÌÄÌ¼þÕý¡õ
 * ß¯Ô»°À
 *      TRUE(1)     ÔÀ  
 *      FALSE(0)    ÁÃ  
 */
BOOL Nodeappendtail( Node** top  , Node* add )
{
    Node* c;        /*»ï¡õÃóåÃ*/
    Node* next;     /*Þ¥ØÆÈÊ×ÛÔÂÓÃ¡õÓñåÃ*/

    /*ÐþÓÀÃó»¥NULL¾®Éýµ¤¾®¼°ÃñÄáÓÀÛÍ*/
    if( *top == NULL ){
        *top = allocateMemory( sizeof( Node  ) );
        if( *top== NULL ) return FALSE;
        (*top)->next    = NULL;            /*Á¸Îå·´  ÖÐ*/
        (*top)->size    = add->size;   /*Ó®½ñ¼°âç¶°*/
        (*top)->val     = add->val;         /*ºÌÄÌ¼þÕý¡õ¼°Îìñâ¡õ*/
        return TRUE;
    }

    for( c = *top ; c->next ; c = c->next ); /* c¼°ÞË  Ã«ÄÚÈÕÔÊ */
    next = allocateMemory( sizeof(Node) );
    if( next == NULL )return FALSE;
    c->next = next;                         /* next±å°ÀÃ«É¬ÀÃÔÊÔÂ */
    next->next  = NULL;                      /*Á¸Îå·´  ÖÐ*/
    next->val   = add->val;                   /*ºÌÄÌ¼þÕý¡õ¼°Îìñâ¡õ*/
    next->size  = add->size;             /*Ó®½ñ¼°âç¶°*/
    return TRUE;
}




/*
 *  Øø¼þÛÍ¼°ó¡±å¹´ØêÔÂ
 * Â¦ÐÑ
 *      top         ÐþÓÀÃóÓÃ¡õÓñ
 *      add         ÞÍØêµÑÒüÔÂÓÃ¡õÓñ¼°    Ã«âç  ÔÊÔÂÓÃ¡õÓñ³ß¼°ºÌÄÌ¼þÕý¡õ
 * ß¯Ô»°À
 *      TRUE(1)     ÔÀ  
 *      FALSE(0)    ÁÃ  
 */
BOOL Nodeappendhead( Node** nowtop  , Node* add )
{
    Node* newtop;       /*Þ¥ØÆÖÐÛÆ  åÃ*/

    /*ÐþÓÀÃó»¥NULL¾®Éýµ¤¾®¼°ÃñÄáÓÀÛÍ*/
    if( *nowtop == NULL ){
        *nowtop = allocateMemory( sizeof( Node  ) );
        if( *nowtop == NULL ) return FALSE;
        (*nowtop)->next = NULL;             /*Á¸Îå·´  ÖÐ*/
        (*nowtop)->size = add->size;        /*Ó®½ñ¼°âç¶°*/
        (*nowtop)->val  = add->val;         /*ºÌÄÌ¼þÕý¡õ¼°Îìñâ¡õ*/
        return TRUE;
    }

    /*
     * Þ¥ØÆÖÐÓÃ¡õÓñÃ«Óò¹´×ÛÔÂ
     * next »¥êûÔÊÊÖ¼°Ã« nowtop ±åÔÊÔÂ£Û
     * nowtop ±å·´ ¸îâçØÆÐ× newtop Ã«Ò½  ÔÊÔÂ£Û
     */
    newtop = allocateMemory( sizeof(Node) );
    newtop->next    = *nowtop;
    newtop->val     = add->val;
    newtop->size    = add->size;
    *nowtop = newtop;
    return TRUE;
}




/*
 * Óò    âÙ¼°ÓÃ¡õÓñÃ«Â¦ÐÑ±åÎìñâ¡õØÆ»¯äúÔ»ÂÖÈÊ
 * Â¦ÐÑ
 *          top         ÓÃ¡õÓñ¼°ÛÆ  
 *          ret         äúÔ»ÂÖÖÐÐ×ÓÃ¡õÓñ¼°Îìñâ¡õ
 * ß¯Ô»°À
 *      TRUE(1)         ÔÀ  
 *      FALSE(0)        ÁÃ      ÓÃ¡õÓñ¼°ÛÆ  »¥NULL
 */
BOOL  Noderemovehead( Node** top , Node* ret)
{
    Node* newtop;         /*Þ¥ØÆÈÊÛÆ  ±åØ¦ÔÂÓÃ¡õÓñ*/

    if( *top == NULL )return FALSE;

    ret->val    = (*top)->val;
    ret->size   = (*top)->size;
    newtop = (*top)->next;
    freeMemory( *top );
    *top = newtop;

    return TRUE;
}





/*
 * Óò      ¼°ÓÃ¡õÓñÃ«Â¦ÐÑ±åÎìñâ¡õØÆ»¯äúÔ»ÂÖÈÊ
 * Â¦ÐÑ
 *          top         ÓÃ¡õÓñ¼°ÛÆ  
 *          ret         äúÔ»ÂÖÖÐÐ×ÓÃ¡õÓñ¼°Îìñâ¡õ
 * ß¯Ô»°À
 *      TRUE(1)         ÔÀ  
 *      FALSE(0)        ÁÃ      ÓÃ¡õÓñ¼°ÛÆ  »¥NULL
 */
BOOL  Noderemovetail( Node** top , Node* ret)
{
    Node* c;             /*»ï¡õÃóåÃ*/
    Node* c1;            /*»ï¡õÃóåÃ ÖÐ¹´Æ¥ÊÖ c->nextÃ«Ï¶ÔÊ*/

    if( *top == NULL )return FALSE;

    c = *top;           /*âÙÓå°ÀÉ¬ÀÃ*/
    c1 = c->next;       /*âÙÓå°ÀÉ¬ÀÃ*/
    while(1){
        if( c1->next == NULL )
            /*
             * ³ð¼°ÁÝ÷§Æ¥          c1
             *                      |
             *  c ---> +------+       
             *         | next |---->+------+
             *         |------|     | next |---->NULL
             *         |      |     +------+
             *         +------+     |      |
             *                      +------+
             *ÎçØ¦ÔÈ»¯ÖÐÔÂ
             */
            break;
        c=c->next;
        c1=c->next;
    }
    c->next = NULL;     /*c1±åØ¤Ð×ÔÂÊÖ¼°Ã«´ÂÂÖÔÊÔÂ¼°Æ¥next·´NULLÎçÔÊÔÂ*/
    /*Îìñâ¡õ*/
    ret->val    = c1->val;
    ret->size   = c1->size;

    freeMemory( c1 );

    return TRUE;
}
