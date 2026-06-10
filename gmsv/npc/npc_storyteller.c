#include "version.h"
#include "char.h"

/*


  井凶曰屯［

  伉伉□旦  匹反2潘  及仇午分引［

 NPCARGUMENT:  0分匀凶日仇午分引0｝1分匀凶日仇午分引1及贿曰  ［  

 橇谪反3勾丐曰｝赐卞褡垫允月［

 橇谪0“ 失奶玉伉件弘［仇及橇谪匹talk今木月午｝
 ＞方仁五凶｝幻仆幻仆方［中井卞手｝歹仄互鳗  及井凶曰屯元扎［＝匹
 橇谪1卞啖垫［
 橇谪1“talk今木凶日｝
 ＞公卅凶互及冗戈卅日壬｝熔午夫五井尹卞｝雄  卅蜕昵毛夫午勾｝ 今内仃方丹［
        公及蜕昵毛及冗戈井? yes/no＝yes匹橇谪2尺［no匹
        ＞匹反｝今日壬元扎［＝匹橇谪0尺
        
 橇谪2“ talk今木凶日｝
 ＞公卅凶卞鳗  及蜕昵＞幻仆幻仆＝毛鳗噩仄凶［  及勾亢卞  濠卅手及午仄化
        域戏  濠卞允月屯仄［今日壬元扎［＝
        午中匀化熔毛蛹日仄｝蜕昵白仿弘毛馨笛允月［
        匹橇谪0卞手升月［



 
 Talked:

 if(     卞反卅仄化井日1坌凶匀化中月井＂){
   橇谪0卞手升月［
 }
   
 switch(橇谪){
 case 0:  质  ［橇谪1尺［break;
 case 1:  质  ［橇谪2尺［break;
 case 2:  质  ［橇谪0尺［break;
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
