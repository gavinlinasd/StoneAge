#include "version.h"
#include "char.h"

/*


  凝倜堇迋��

  惉惉↓筒  ぁ毀2攣  摯喫敁煦竘��

 NPCARGUMENT:  0煦埱倜�梜蠁蝺笳�0��1煦埱倜�梜蠁蝺笳�1摯鞅堇  ��  

 Щ稃毀3僑堣堇��棹勗鵌菜埰堎��

 Щ稃0※ 囮騷迶惉璃精�蛦蟦勒邁媃另alk踏躂堎敁��
 ˇ源�庉槱蛅�酵ど酵ど源�詎郋拳樀痑�渦媃誑魕  摯凝倜堇迋啋崨�菇複�
 Щ稃1勗遉菜��
 Щ稃1※talk踏躂倜�掁�
 ˇ鼠埵倜誑摯�葚縋牟桱氿��褁蝺藬敺朔�勗��倯  埵虭糒禱痲敁僑�� 踏囀崹源竣��
        鼠摯虭糒禱摯�葚篨�? yes/noˊyesぁЩ稃2喜�菝oぁ
        ˇぁ毀��踏�桱尕肴��菇複犯邁�0喜
        
 Щ稃2※ talk踏躂倜�掁�
 ˇ鼠埵倜勗魕  摯虭糒ˇ酵ど酵どˊ禱魕堿媃倜��  摯僑蕩勗  憍埵忒摯敁媃趙
        郖牁  憍勗埰堎迋媃�蛗鮵桱尕肴��菇�
        敁笢埱趙�袺帥暫欷ゞ�虭糒啞溘精禱黹萃埰堎��
        ぁЩ稃0勗忒汔堎��



 
 Talked:

 if(     勗毀埵媃趙凝��1覕倜埱趙笢堎凝ㄑ){
   Щ稃0勗忒汔堎��
 }
   
 switch(Щ稃){
 case 0:  窐  �裀邁�1喜�萒reak;
 case 1:  窐  �裀邁�2喜�萒reak;
 case 2:  窐  �裀邁�0喜�萒reak;
 } 

 }

 
 
 

 



 */
BOOL NPC_StoryTellerInit( int meindex )
{
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPESTORYTELLER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );

    return TRUE;
}


void NPC_StoryTellerTalked( int meindex , int talker , char *msg , int col )
{
    
}
