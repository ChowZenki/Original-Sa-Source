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
    NPC_CREATEFLOORID,      /*  �ե�ID    */
    NPC_CREATEBORNLEFTUPX,        /*  �����X��ɸ */
    NPC_CREATEBORNLEFTUPY,        /*  �����Y��ɸ */
    NPC_CREATEBORNRIGHTDOWNX,     /*  ������X��ɸ */
    NPC_CREATEBORNRIGHTDOWNY,     /*  ������Y��ɸ */

    NPC_CREATEMOVELEFTUPX,        /*  �����X��ɸ */
    NPC_CREATEMOVELEFTUPY,        /*  �����Y��ɸ */
    NPC_CREATEMOVERIGHTDOWNX,     /*  ������X��ɸ */
    NPC_CREATEMOVERIGHTDOWNY,     /*  ������Y��ɸ */

    NPC_CREATEDIR,                /*  ����������    */
    NPC_CREATEBASEIMAGENUMBER,    /*  template ��������񤭤���
                                      �����ֹ�    */
    NPC_CREATETIME,               /* ���� */
    NPC_CREATEBORNNUM,            /*    �����ϰ�˽и��������    */

    NPC_CREATEENEMYNUM,           /*  Ũ�μ����  */
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
    NPC_CREATENAME,             /* template���񤭤��뤫�⤷��ʤ�̾��*/
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
