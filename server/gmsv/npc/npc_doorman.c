#include "version.h"
#include <string.h>
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "item.h"
#include "util.h"
#include "handletime.h"
#include "npc_doorman.h"
#include "npc_door.h"
#include "npcutil.h"
#include "configfile.h"
/*
 *
 *  �٤�˥ɥ�������Ȥ��ˤ��Υɥ��򲿤餫�ξ��ˤ�äƳ���NPC.
 *  �ɥ��ΤȤʤ��create��������ǡ����Υɥ��������뤳�Ȥ��Ǥ��롣
 *  �Ϥʤ�������줿�Ȥ����������򸡺����ơ��ޤ��8�ޥ��˥ɥ�������
 *  ���Ϥ��Τ��٤Ƥ��Ф��Ʊƶ����롣�դ��ĤΥɥ���Ʊ���˳������Ȥˤʤ롣
 *
 *  ���󥿡��ե�������Talk��
 *
 * �ɥ��򳫤������
 *
 * 1 �����ħ�����롣ħ���Ǥ�����Ҥ餯     gold|100
 * 2 �����ƥ��1��ħ������ ��ħ���Ǥ�����Ҥ餯  item|45
 * 3 �����ƥ����äƤ��뤫�ɤ���Ĵ�٤롣 ���äƤ����鳫����itemhave|44
 * 4 �����ƥ����äƤ��ʤ����ɤ���Ĵ�٤롣���äƤ��ʤ��ä��鳫����
 *          itemnothave|333
 * 5 �ι���äƤ��뤫�ɤ���Ĵ�٤롣���äƤ����鳫���� titlehave|string
 * 6 �ι���äƤ��ʤ����ɤ���Ĵ�٤롣���äƤ��ʤ��ä��鳫����
 *      titlenothave|string
 *
 * ���ʤ餺�����������ȳ�������ξ��ϡ�
 *��100������ɤ��������ޤ��������Ǥ������פǡ֤Ϥ��פȤ�����100�������
 * �Ȥ��롣�����ʤ�֤Ϥ��פ������äƤ�Ȥ��롣�ǡ���100�������
 * ���������ޤ������פȸ����롣
 *
 * �����ƥ�ħ���ξ��ϡ��ֲ������Ĥ��������ޤ��������Ǥ������פȤ�����
 *  3����6�ξ��ϡ������Ϥʤ������ƾ�郎����äƤ��鳫����
 *
 *
 *
 *  �ƥ��Ȥ���ˡ
 *
 *1  �ɥ���Ƥ��Ȥ����֤�
 *2  ����NPC��Ŭ���˥ɥ��ΤȤʤ���֤��������� gold|100 �ˤ���
 *3  ����NPC���Ф��ơ�100������ɰʾ��äƤ�����֤ǡ֤Ϥ��פȸ���
 *4  �ɥ����Ҥ餤�ƶ⤬���ä���������
 *
 */

static void NPC_DoormanOpenDoor( char *nm  );

BOOL NPC_DoormanInit( int meindex )
{
	char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];
    char dname[1024];

	/* ���٥�ȤΥ��������� */
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE,CHAR_EVENT_NPC);

    CHAR_setInt( meindex , CHAR_HP , 0 );
    CHAR_setInt( meindex , CHAR_MP , 0 );
    CHAR_setInt( meindex , CHAR_MAXMP , 0 );
    CHAR_setInt( meindex , CHAR_STR , 0 );
    CHAR_setInt( meindex , CHAR_TOUGH, 0 );
    CHAR_setInt( meindex , CHAR_LV , 0 );

    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPETOWNPEOPLE );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );  /* ���⤵��ʤ���� */

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if(!getStringFromIndexWithDelim( arg, "|", 3, dname, sizeof(dname ))){
        print("RINGO: �]�w�ݪ��̮ɻݭn�����W�r��I:%s:\n",
              arg );
        return FALSE;
    }
    print( "RINGO: Doorman create: arg: %s dname: %s\n",arg,dname);
    CHAR_setWorkChar( meindex , CHAR_WORKDOORMANDOORNAME , dname );

    return TRUE;
}

void NPC_DoormanTalked( int meindex , int talkerindex , char *msg ,
                     int color )
{
    char mode[128];
    char opt[256];
    char	arg[NPC_UTIL_GETARGSTR_BUFSIZE];

    /* �ץ쥤�䡼���ɥ��ޥ��1����åɰ���ʤ�Ϥ�Τ� */
    if(NPC_Util_CharDistance( talkerindex, meindex ) > 1)return;

	NPC_Util_GetArgStr( meindex, arg, sizeof( arg));

    if( !getStringFromIndexWithDelim( arg, "|", 1, mode, sizeof( mode )))
        return;

    if( !getStringFromIndexWithDelim( arg, "|", 2, opt, sizeof( opt ) ))
        return;

    if( strcmp( mode , "gold" ) == 0 ){
        int g = atoi( opt );
        int yn = NPC_Util_YN( msg );
        /*char *nm = CHAR_getChar( meindex , CHAR_NAME );*/
        char msg[256];

        if( g > 0 && yn < 0 ){
            snprintf( msg ,sizeof( msg ) ,
                      "���}���ݭn����%d�����l�o�˥i�H�ܡH", g );
            CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
        } else if( g > 0 && yn == 0 ){
            snprintf( msg , sizeof( msg ),
                      "���}�� %d�����l�O���n���C", g );
        } else if( g > 0 && yn == 1 ){
            int now_g = CHAR_getInt( talkerindex, CHAR_GOLD );
            if( now_g < g ){
                snprintf( msg , sizeof( msg ) ,
                          "���}�� %d�����l�O���n���C", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );
            } else {
                snprintf( msg , sizeof( msg ),
                          "%d ������l�F�C�{�b�N�Ӷ}���C", g );
            	CHAR_talkToCli( talkerindex, meindex , msg, CHAR_COLORWHITE );

                /* ����򥲥å� */
                now_g -= g;
                CHAR_setInt( talkerindex , CHAR_GOLD , now_g );
                /* �����餷�����ơ����������� */
                CHAR_send_P_StatusString(talkerindex, CHAR_P_STRING_GOLD);

                /* �ɥ��Ҥ餯 */
                NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
            }
        }
    } else if( strcmp( mode , "item" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "�|�b���䴩�Ҧ��C",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemhave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "�|�b���䴩�Ҧ��C",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "itemnothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "�|�b���䴩�Ҧ��C",
                        CHAR_COLORWHITE);
    } else if( strcmp( mode , "titlehave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "�|�b���䴩�Ҧ��C",
                        CHAR_COLORWHITE);

    } else if( strcmp( mode , "roomlimit" ) == 0 ){

		/* �����οͿ����¤������� */
		char szOk[256], szNg[256], szBuf[32];
		int checkfloor;
		int maxnum, i, iNum;

	    if( !getStringFromIndexWithDelim( arg, "|", 2, szBuf, sizeof( szBuf ) ))
    	    return;

		/* Ĵ�٤�ե��Ⱥ���Ϳ� */
		if( sscanf( szBuf, "%d:%d", &checkfloor, &maxnum ) != 2 ){
			return;
		}

		for( iNum = 0,i = 0; i < getFdnum(); i ++ ){
			/* �ץ쥤�䡼�ʳ��ˤ϶�̣��̵�� */
			if( CHAR_getCharUse( i ) == FALSE )continue;
			if( CHAR_getInt( i, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			/* ����Υե��ʳ��˶�̣��̵�� */
			if( CHAR_getInt( i, CHAR_FLOOR ) != checkfloor )continue;
			iNum++;
		}
	    if( !getStringFromIndexWithDelim( arg, "|", 5, szNg, sizeof( szNg ))){
   			strcpy( szNg, "�C�C�C�C" );	/* ��ʤʤ��Υ���� */
		}
    	if( !getStringFromIndexWithDelim( arg, "|", 4, szOk, sizeof( szOk ))){
   			strcpy( szOk, "�}���a�C�C�C" );	/* ��ʤ���Υ���� */
   		}

		if( iNum >= maxnum ){
			/* �����Ķ���Ƥ����� */
	        CHAR_talkToCli( talkerindex, meindex ,szNg, CHAR_COLORWHITE);
		}else{
			/* ����������ʤ���� */
	        CHAR_talkToCli( talkerindex, meindex ,szOk, CHAR_COLORWHITE);
            NPC_DoormanOpenDoor(
                    CHAR_getWorkChar( meindex, CHAR_WORKDOORMANDOORNAME));
		}

    } else if( strcmp( mode , "titlenothave" ) == 0 ){
        CHAR_talkToCli( talkerindex, meindex ,
                        "�|�b���䴩�Ҧ��C",
                        CHAR_COLORWHITE);
    }
}

/*
 *  ̾���Ǹ������ƥҥåȤ����Τ򤹤٤Ƴ�����
 *
 */
static void NPC_DoormanOpenDoor( char *nm)
{
    int doori = NPC_DoorSearchByName( nm );
    print( "RINGO: Doorman's Door: index: %d\n", doori );

    NPC_DoorOpen( doori , -1 );

}

