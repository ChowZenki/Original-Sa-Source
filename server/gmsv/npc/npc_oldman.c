#include "version.h"
#include "object.h"
#include "char_base.h"
#include "char.h"
#include "util.h"
#include "npcutil.h"
#include "npc_oldman.h"
#include "title.h"
#include "char_data.h"
#include "readmap.h"

/* hoge */

BOOL NPC_OldmanInit( int meindex )
{

#if 0
    int     oldmanid=0, bornfl=0, bornx=0 ,borny=0;
    char*   npcarg;
    char    token[32]="";
/*    print( "��ρ�困渕�: '%s'\n" , CHAR_getChar(
                         meindex, CHAR_NPCARGUMENT ));*/
    npcarg = CHAR_getChar(meindex,CHAR_NPCARGUMENT);

    getStringFromIndexWithDelim( npcarg,"|",1,token,sizeof(token));
    oldmanid = atoi( token );
    getStringFromIndexWithDelim( npcarg,"|",2,token,sizeof(token));
    bornfl = atoi( token );
    getStringFromIndexWithDelim( npcarg,"|",3,token,sizeof(token));
    bornx = atoi( token );
    getStringFromIndexWithDelim( npcarg,"|",4,token,sizeof(token));
    borny = atoi( token );

    /* 長老はIDを覚えておくナリ */
    CHAR_setWorkInt( meindex , CHAR_WORKOLDMANID ,oldmanid );
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEHEALER );
    CHAR_setFlg( meindex , CHAR_ISATTACKED , 0 );
    CHAR_setFlg( meindex , CHAR_ISOVERED , 0 );

    if( MAP_IsValidCoordinate( bornfl,bornx,borny )== TRUE
        /*  フロアIDがいいかどうかを調べる  */
        &&
        CHAR_ElderSetPosition( oldmanid ,bornfl,bornx, borny ) 
        == TRUE ){
        /* 長老の位置を初期化するなり */
        return TRUE;
    }else{
        printf( "Invalid elder npcarg=%s\n", npcarg );
        
        /* あまりに重要なエラーなので、neverMakeしない。
           毎回エラーメッセージ出させる */
        return FALSE;
    }
#endif

	return FALSE;

}


/*
 *
 *  長老。
 *
 *  おもな仕事は話しかけられたときに称号の判定をしたり
 * することだ。
 * * by ringo
 */

void NPC_OldmanTalked( int meindex , int talkerindex , char *msg ,
                     int color )

{
    char message[1024];
    char *n = CHAR_getChar( talkerindex , CHAR_NAME );
    int title_change = 0;

    /* はなしかけた奴に、自分のIDを記憶させる */
    CHAR_setInt( talkerindex, CHAR_LASTTALKELDER ,
                 CHAR_getWorkInt( meindex, CHAR_WORKOLDMANID ) );

    /*

      if( title_change == 0 && strcmp( n , "朝�G" ) == 0 ){
        
        if( NPC_Util_HaveTitle( talkerindex , 3 ) == 0 ){
            char *ts;
            NPC_Util_AddOneTitle( talkerindex , 3 );
            ts = TITLE_makeTitleStatusString( talkerindex , 3 );
            snprintf( message , sizeof(message),
                  "┷�O�咾�杉�墾W�r, �N�s%s�a�I",
                  ts );
            title_change = 1;
        }
    }
    */
{
	int		addcnt,delcnt;
	title_change = TITLE_TitleCheck_Nomsg( talkerindex, 0, &addcnt, &delcnt);
	
    if( !title_change){
        snprintf( message, sizeof( message ) ,
              "�唔墾n%s�b宜�A細弦��"
              "ぃ�O匝�S�啅棔H" , n );
	    CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
    }
    else {
    	#define		NPC_OLDMAN_TITLE_UNIT1		"TSU"
    	#define		NPC_OLDMAN_TITLE_UNIT2		"KO"
    	
        snprintf( message, sizeof( message ) ,"�唔墾n%s",n );
    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		if( delcnt > 0 && addcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"�A橿ア�F%d%s 細弦�左螫罅C"
	    			"ぃ�O�o�譴F%d%s �査抔晃螫羔棔H"
	    			, delcnt,
	    			delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2,
	    			addcnt,
	    			addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2
	    			);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
		else if( delcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"�A�w橿ア�o��%d%s細弦�左螫罅C", delcnt,
	    			delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
		else if( addcnt > 0 ) {
	        snprintf( message, sizeof( message ) ,
	    			"宜�A%d%s�査抔孝a�I", addcnt, // CoolFish: d%s -> %d%s 2001/4/18
	    			addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	    	CHAR_talkToCli( talkerindex , meindex , message , CHAR_COLORWHITE );
		}
	    if( delcnt > 0 ) {
	        snprintf( message, sizeof( message), 
	                    "ア�h%d%s 細弦�I", delcnt,
	    				delcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	        CHAR_talkToCli( talkerindex, -1, message,  CHAR_COLORYELLOW);
	    }
	    if( addcnt > 0 ) {
	        snprintf( message, sizeof( message), 
	                    "栓�o%d%s 細弦�I", addcnt,
	    				addcnt < 10 ?  NPC_OLDMAN_TITLE_UNIT1:NPC_OLDMAN_TITLE_UNIT2);
	        CHAR_talkToCli( talkerindex, -1, message,  CHAR_COLORYELLOW);
	    }
    }
}
    if( title_change ) CHAR_sendStatusString( talkerindex , "T" );
    
}
