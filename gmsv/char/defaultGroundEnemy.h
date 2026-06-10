#if 0
static Char    slime=
{
    FALSE,      /*  use     どうでもいい*/

    /*  data    */
    {
		0,		/*  CHAR_DATAPLACENUMBER */
        0,      /*  CHAR_BASEIMAGENUMBER */
        0,      /*  CHAR_BASEBASEIMAGENUMBER */
		0,		/*  CHAR_FACEIMAGENUMBER */
        0,      /*  CHAR_FLOOR  */
        0,      /*  CHAR_X */
        0,      /*  CHAR_Y  */
        0,      /*  CHAR_DIR 12時を0に時計周りに */
        0,      /*  CHAR_LV  */
        0,      /*  CHAR_GOLD    */
        1,      /*  CHAR_HP  */
        0,      /*  CHAR_MP  */
        0,      /*  CHAR_MAXMP   */

    	0,		/*  CHAR_VITAL	*/
        0,      /*  CHAR_STR */
        0,      /*  CHAR_TOUGH */
    	0,		/*  CHAR_DEX	*/

    	0,		/*  CHAR_CHARM	*/
    	0,		/*  CHAR_LUCK	*/

		0,		/* 地属性 */
		0,		/* 水属性 */
		0,		/* 火属性 */
		0,		/* 風属性 */

		0,		/* CHAR_SLOT */
		0,		/* CHAR_CRITIAL */
		0,		/* CHAR_COUNTER */
		0,		/* CHAR_RARE */
        0,      /*  CHAR_RADARSTRLENGTH */
        0,      /*  CHAR_CHATVOLUME */
        MAKE2VALUE(100,20),  /*  CHAR_MERCHANTLEVEL  */
        0,      /*  CHAR_HEALERLEVEL    */
        0,      /*  CHAR_DETERMINEITEM  */

        -1,     /*  CHAR_INDEXOFEQTITLE  */

        0,      /*  CHAR_POISON  */
        0,      /*  CHAR_PARALYSIS  */
        0,      /*  CHAR_SILENCE    */
        0,      /*  CHAR_STONE  */
        0,      /*  CHAR_DARKNESS   */
        0,      /*  CHAR_CONFUSION  */

        0,      /*  CHAR_LOGINCOUNT */
        0,      /*  CHAR_DEADCOUNT */
        0,      /*  CHAR_WALKCOUNT */
        0,      /*  CHAR_TALKCOUNT */
    	0,		/*  CHAR_DAMAGECOUNT */
    	0,		/*  CHAR_GETPETCOUNT */
    	0,		/*  CHAR_KILLPETCOUNT */
    	0,		/*  CHAR_DEADPETCOUNT */
    	0,		/*  CHAR_SENDMAILCOUNT */
		0,		/*  CHAR_MERGEITEMCOUNT */
		0,		/*  CHAR_DUELBATTLECOUNT */
		0,		/*  CHAR_DUELWINCOUNT */
		0,		/*  CHAR_DUELLOSECOUNT */
		0,		/*  CHAR_DUELSTWINCOUNT */
		0,		/*  CHAR_DUELMAXSTWINCOUNT */

        CHAR_TYPEENEMY,  /* CHAR_WHICHPLAYER   */
        1000,   /* CHAR_WALKINTERVAL */
        1000,   /* CHAR_LOOPINTERVAL */

        0,      /*  CHAR_LEVELEXP    */

        -1,      /*CHAR_LASTTALKELDER*/
        0,      /*CHAR_SKILLUPPOINT    */
    	0,		/* CHAR_LEVELUPPOINT */

        0,      /*CHAR_IMAGETYPE    */
		CHAR_COLORYELLOW,	/* CHAR_NAMECOLOR */
		CHAR_COLORYELLOW,	/* CHAR_POPUPNAMECOLOR */
		0,		/* CHAR_LASTTIMESETLUCK */
		0,	/* CHAR_DUELPOINT */
		0,	/* CHAR_EVENT */
		0,	/* CHAR_EVENT2 */
		0,	/* CHAR_EVENT3 */
#ifdef _NEWEVENT
		0,	/* CHAR_EVENT4 */
		0,	/* CHAR_EVENT5 */
		0,	/* CHAR_EVENT6 */
#endif
		0,	/* CHAR_NOWEVENT */
		0,	/* CHAR_NOWEVENT2 */
		0,	/* CHAR_NOWEVENT3 */
#ifdef _NEWEVENT
		0,	/* CHAR_NOWEVENT4 */
		0,	/* CHAR_NOWEVENT5 */
		0,	/* CHAR_NOWEVENT6 */
#endif
		0,  /* CHAR_TRANSMIGRATION */
		0,  /* CHAR_TRANSEQUATION */
		
        0,      /*CHAR_INITDATA    */
    },
    {
        {""},       /*  CHAR_NAME   */
        {""},       /*  CHAR_OWNTITLE   */
        {""},       /*  CHAR_ARGUMENT   */
        {""},       /*  CHAR_OWNERCDKEY   */
        {""},       /*  CHAR_OWNERCHARANAME   */
#if 0
        {""},       /*  CHAR_INITFUNC    */
        {""},       /*  CHAR_WALKPREFUNC    */
        {""},       /*  CHAR_WALKPOSTFUNC   */
        {""},       /*  CHAR_PREOVERFUNC    */
        {""},       /*  CHAR_PREOVERFUNC    */
        {""},       /*  CHAR_WATCHFUNC  */
        {""},       /*  CHAR_LOOPFUNC   */
        {""},       /*  CHAR_DYINGFUNC  */
        {""},       /*  CHAR_TALKEDFUNC */
        {""},           /*  CHAR_PREATTACKEDFUNC    */
        {""},           /*  CHAR_POSTATTACKEDFUNC    */
        {""},           /*  CHAR_OFFFUNC    */
        {""},           /*  CHAR_LOOKEDFUNC */
        {""},           /*  CHAR_ITEMPUTFUNC    */
        {""},           /*  CHAR_SPECIALTALKEDFUNC    */
        {""},           /*  CHAR_WINDOWTALKEDFUNC    */
#endif
    },
    {
        /*  CHAR_ISATTACK		*/
        /*  CHAR_ISATTACKED		*/
        /*  CHAR_ISOVER			*/
        /*  CHAR_ISOVERED		*/
        /*  CHAR_HAVEHEIGHT		*/
        /*  CHAR_ISVISIBLE		*/
        /*  CHAR_ISTRANSPARENT  */
        /*  CHAR_ISFLYING		*/

        SETFLG(1,1,1,1,1,1,0,0 ),

        /*  CHAR_ISDIE			*/
        /*  CHAR_ISBIG			*/
        /*  CHAR_ISSHOWDAMAGE   */
		/*	CHAR_ISPARTY		*/
    	/*  CHAR_ISWARP			*/
    	/*  CHAR_ISDUEL			*/
		/*  CHAR_ISPARTYCHAT	*/
		/*  CHAR_ISTRADECARD	*/

        SETFLG(0,0,1,0,0,0,0,0 ),
    }
};
#endif



