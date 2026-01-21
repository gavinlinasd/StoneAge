#ifndef __VERSION_H__
#define __VERSION_H__
// #define _SASQL1  // Disabled - requires MySQL

/* �뽫����ר���� definition �����������
 * Ȼ����� remark/unmark �����ع���
 */

/* -------------------------------------------------------------------
 * ר��������ְ������幦�ܲ���
 * ��Ա��С褡���ѫ��־ΰ��С��
 * ˵����
 */
#define _LEADERFLAG_VER		// �š��ߡ����.... (�ɿ���)
#define __FAMILY			//  (�ɿ���)
#define _PERSONAL_FAME		// Arminius 8.30 �����������  (�ɿ���)
#define _FMVER21			// CoolFish: �����峤ְ������� 2001/8/30 (�ɿ���)
#define MEMBER_100			// CoolFish: ������������ 2001/9/3 (�ɿ���)
#define _EVERYONE_RIDE		// CoolFish: ���ƽ�񻯼��������� 2001/9/24 (�ɿ���)

/* -------------------------------------------------------------------
 * ר������������˷֮�������ϴ浵��ʽ����
 * ��Ա��С褡���ѫ
 * ˵���������������ϴ浵��ʽ
 */
#define _NewSave		// CoolFish: 2001/10/16 (�ɿ���)

/* -------------------------------------------------------------------
 * ר��������ϲ�
 * ��Ա��������ѫ��С�
 * ˵����
 */
//#define _RELOGIN		// CoolFish: �ظ�������� 2001/10/29 (���ɿ���)

/* -------------------------------------------------------------------
 * ר���������½
 * ��Ա�����������С�
 * ˵�������� 1/15
 */
//#define _AUCTIONEER // Arminius 1.10 ������ (�����½)

/* -------------------------------------------------------------------
 * ר��������£գ�����
 * ��Ա��С�
 * ˵����
 */
#define _LEADERFUNCHECK	// CoolFish: 2002/02/06 �峤���ܴ������ô�������(�ɿ���)

/* -------------------------------------------------------------------
 * ר��������������
 * ��Ա�����
 * ˵����
 */
#define _WAEI_KICK				// (�ɿ���)(�������ؿ�)
#define _FIX_WORKS				// (�ɿ���)(�������ؿ�) wk�޸�

//#define _DELAY_UNLOCK			// (���ɿ�) ANDY
//#define _LOCK_SERVER			// (���ɿ�)
//#define _TEST_LOCKSERVER	// (���ɿ�)
//#define _NO_DOUBLECHECK		// (���ɿ�)
//#define _PAUCTION_MAN			// (���ɿ�) ����
/* -------------------------------------------------------------------
 * ר�����Ŵ�ׯ԰(4.0)
 * ��Ա��־��	
 * ˵����
 */
#define _FIX_9_FMPOINT			// (�ɿ���) WON ADD �Ŵ�ׯ԰
#define _FIX_LEADER_ERR			// (�ɿ���) WON ADD �����峤����
#define _REMOVE_FM_LOG          // (�ɿ���) WON ADD �Ƴ�����Ҫ�ļ��� LOG
//#define _ADD_FAMILY_TAX		// (���ɿ�) WON ADD ����ׯ԰˰��


/* -------------------------------------------------------------------
 * ר����ȷ��ׯ԰��ս�ų�
 * ��Ա�����	
 * ˵������AC��familyĿ¼��fmschedule��ͳһ�㲥��ׯ԰��ս�ų� 
*/
//#define _CHECKFMSCHEDULE	    // (���ɿ�) Syu ADD ͸��ACȷ�ϼ�����ս�ų�
//#define _FAMILYBANKSTONELOG   // (���ɿ�) Syu ADD �����������д�ȡLog
//#define _FIXFMASSIGN		    // (���ɿ�) Syu ADD ��������һ�����޷�ָ�ɡ��Ƴ�����	


/* -------------------------------------------------------------------
 * ר����ʯ��6.0
 * ��Ա��
 * ˵����
 */
#define _SEND_EFFECT		    // (�ɿ���) WON ADD ����ѩ���������Ч
//#define _NEW_PLAYERGOLD		// (���ɿ�) ANDY ���ʺŸ�Ǯ
//#define _BAD_PLAYER           // (���ɿ�) WON ADD �ͻ����ȥ��

// -------------------------------------------------------------------
#define _AC_SEND_FM_PK			// (�ɿ���) WON ADD ׯ԰��ս�б�������AC
#define _ACFMPK_LIST			// (�ɿ���) ANDY ׯ԰��ս�б�
#define _CK_ONLINE_PLAYER_COUNT	// (�ɿ���) WON ADD ������������

/* -------------------------------------------------------------------
 * ר����ʯ��7.0
 * ��Ա��
 * ˵����
 */
#define _ALLDOMAN				// (�ɿ���) Syu ADD ���а�NPC
#define _CHAR_POOLITEM			// (�ɿ���) ANDY ���òֿ�
#define _CHAR_POOLPET			// (�ɿ���) Robin ���ó���ֿ�
//#define _FIX_ALLDOMAN			// (���ɿ�) ANDY
// -------------------------------------------------------------------
// �¹��ܣ��ͷ�����ϵͳ
#define _GM_BROADCAST				// (�ɿ���) WON ADD �ͷ�����ϵͳ

// -------------------------------------------------------------------
// �¹��ܣ�GM��Ƶ��������
//#define _RECAL_BACKUP_PLAYER		// (�ɿ��� (��ʱ�ȹ�����) ) WON ���ݹ��� (����&�������ؿ�)
//#define _UNIVERSE_CHATROOM			// (���ɿ�) ANDY ȫ��ϵ����

// -------------------------------------------------------------------
// �¹��ܣ��޸�WORKER(�¸��ѻ���)
#define _FIX_MESSAGE				// (�ɿ�) WON ADD �޸�WORKER������� (����&�������ؿ�)
#define _WORKER_AUTO_LOGOUT		// (�ɿ�) WON ADD GS�����Զ���LOGOUT��WORKER

//#define _DEFEND_NEWUSERS			// (���ɿ�) ANDY ���ִ�����
//#define _CHECK_BADFILES			// (���ɿ�) ANDY �жϻ���
#define _AC_UNLOCK_GS_PLAYER		// (���ɿ�) WON ADD �Զ�UNLOCK
//#define _ACFMPK_NOFREE			// (���ɿ�) ANDY ����������

// -------------------------------------------------------------------
// �¹��ܣ�����ϵpk�ŷ���
//#define _OACSTRUCT_TCP			// (���ɿ�) ANDY ����ϵȡ�������б�
//#define _TCPIP_UTIL				// (���ɿ�) ANDY TCPIP
//#define _DEATH_CONTEND			// (���ɿ�) ANDY ����
//#define _DEATH_FAMILY_LOGIN_CHECK	// (���ɿ�) WON ADD ����ս������
//#define _DEATH_FAMILY_GM_COMMAND	// (���ɿ�) WON ADD ����սGMָ��
//#define _DEATH_FAMILY_STRUCT		// (���ɿ�) WON ADD ����ս���ʤ������

#define _CHANGEGALAXY	// Robin 2003/10/27 ��ϵ����

//#define _TIMEOUTKICK // Robin 2003/11/07 ʱ�����Զ�����

//#define _UNLOCKPOOL		// Robin 2003/12/12  Logout�ݴ���

#define _KILLDEADWORKER // Robin 2004/02/ ��sigusr1�������Worker ��unlock.arg��'K'

#define _SLEEP_CHAR		// Robin 2004/02/12	��Ծ������ǻ�Ծ����ֿ�����

#endif

// Terry define start -------------------------------------------------------------------
#define _FM_MODIFY				// ���幦���޸�
#define _CHANNEL_MODIFY		// Ƶ����������
#define _NEW_MANOR_LAW		// ��ׯ԰����
#define _LOCK_ADD_NAME		// LockNode ����������λ(δ����)
#define _TEACHER_SYSTEM   // ��ʦϵͳ(δ����)
// Terry define end   -------------------------------------------------------------------

#define _ANGEL_SUMMON	// Robin ������ٻ�
//#define _SASQL //Change ʯ�����Ͽ� (���ɿ�)
#define _RACEMAN		// cyg �Գ���


//--------------------------------------------------------
//by longzoro
// #define _SQL_BACKGROUND   // Disabled - requires _SASQL1

// #define _SQL_REGISTER     // Disabled - requires _SASQL1