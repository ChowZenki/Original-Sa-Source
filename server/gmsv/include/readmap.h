#ifndef __MAP_H__
#define __MAP_H__
#include "common.h"
#include "util.h"

typedef struct tagMAP_Objlink
{
    int objindex;
    struct tagMAP_Objlink* next;
}MAP_Objlink, *OBJECT;

#define GET_OBJINDEX(x) ((x)->objindex)
#define NEXT_OBJECT(x) ((x)->next)

typedef struct tagMAP_Map
{
    int     id;             /*  �����|ID    */
    int     xsiz,ysiz;      /*  Ǳ�~Ƕ  */
    char    string[64];     /*  ���    */
    unsigned short*  tile;           /*  ǻ�~��  */
    unsigned short*  obj;            /*  Ǧ��Ǵǣǫ��    */
    MAP_Objlink** olink;
#ifdef _MAP_NOEXIT
	unsigned int startpoint;
	int MapType;
#endif
}MAP_Map;


typedef enum
{
    MAP_WALKABLE,           /*
                             * �������U���X
                             * �����Q���Mǻ�~���y���Q�� 0
                             * �����Mǻ�~���y���r 1
                             * �����Mǻ�~���y���Q�� 2
                             */
    MAP_HAVEHEIGHT,         /*  ���������J�F�p�D0 �Q���J�F�p0   */
    MAP_DEFENCE,            /*  �\�s�r�Q�p�B�u�ƤO�B0 �H�U�Q�p�\�s�Q��  */

    MAP_INTODAMAGE,         /*  �D���R�J�J�F���RHP�R���@��  */
    MAP_OUTOFDAMAGE,        /*  �D�����p�X�F���RHP�R���@��  */

    MAP_SETED_BATTLEMAP,    /* ��������ǿ�ӳ]�w�� */
	MAP_BATTLEMAP,			/* ��������ǿ���U�f�A ��*/
	MAP_BATTLEMAP2,			/* ��������ǿ���U�f�A ��*/
	MAP_BATTLEMAP3,			/* ��������ǿ���U�f�A ��*/

/* �H�U�V�ڢᢱ�N���J�M���M����N�V���v�Q��(�ۦX�v�B�R���@) */

    MAP_INTODARKNESS,       /*  �J�J�F���U�t��    */
    MAP_INTOCONFUSION,      /*  �J�J�F���U�V��    */

    MAP_OUTOFPOISON,         /*  �J�J�F���U�r    */
    MAP_OUTOFPARALYSIS,      /*  �J�J�F���U���Z�s    */
    MAP_OUTOFSILENCE,        /*  �J�J�F���U�H��  */
    MAP_OUTOFSTONE,          /*  �J�J�F���U��    */
    MAP_OUTOFDARKNESS,       /*  �J�J�F���U�t��    */
    MAP_OUTOFCONFUSION,      /*  �J�J�F���U�V��    */

    MAP_DATAINT_NUM,
}MAP_DATAINT;
typedef enum
{
    MAP_DATACHAR_NUM,
}MAP_DATACHAR;
typedef struct tagMAP_ImageData
{
    int     data[MAP_DATAINT_NUM];
    STRING32    string[MAP_DATACHAR_NUM];
}MAP_ImageData;

typedef enum
{
    MAP_KINDWALKABLE,           /*  �����F�p 1 ��J�r   */

    MAP_KINDNUM,
}MAP_kind;


INLINE int MAP_getfloorIndex( int floorid );
int MAP_getfloorX( int floorid );
int MAP_getfloorY( int floorid );

BOOL MAP_initReadMap( char* maptilefile , char* mapdir );
BOOL MAP_initMapArray( int   num );
void    MAP_endMapArray( void );

char* MAP_getdataFromRECT( int floor, RECT* seekr, RECT* realr );
char *MAP_getChecksumFromRECT( int floor, RECT* seekr, RECT* realr,
								int *tilesum, int *objsum, int *eventsum );

BOOL MAP_checkCoordinates( int mapid, int x, int y );

BOOL MAP_setTileAndObjData( int ff ,int fx, int fy, int tile, int obj);
BOOL MAP_getTileAndObjData( int ff ,int fx, int fy, int* tile, int* obj);
void MAP_sendAroundMapdata( int fl, int x, int y);
int MAP_getImageInt( int imagenumber, int element );
BOOL MAP_setImageInt( int imagenumber, int element, int value );
BOOL IsValidImagenumber( int imagenumber );

char* MAP_getfloorShowstring( int floorid );
BOOL MAP_makeVariousMap(char* atile, char* aobj, int floor, int startx, int starty, int xsiz, int ysiz, MAP_kind   kind );
BOOL MAP_makeWalkableMap( char* data,  int floor, int startx, int starty,int xsiz, int ysiz );
BOOL MAP_IsThereSpecificFloorid( int floorid );
BOOL MAP_IsValidCoordinate( int floorid, int x, int y );
int MAP_attackSpecificPoint( int floor, int x, int y, int damage ,
                             int charaindex );

BOOL MAP_addNewObj( int floor, int x, int y, int objindex );
BOOL MAP_removeObj( int floor, int x, int y, int objindex );
#define		MAP_getTopObj( fl, x, y)	_MAP_getTopObj( __FILE__, __LINE__, fl, x, y)
MAP_Objlink* _MAP_getTopObj( char *file, int line, int floor, int x, int y );
#define		MAP_objmove( objindex, of, ox, oy, nfl, nx, ny) _MAP_objmove( __FILE__, __LINE__, objindex, of, ox, oy, nfl, nx, ny)
BOOL _MAP_objmove( char *file, int line, int objindex, int ofloor, int ox, int oy, int nfloor,
                  int nx, int ny );
char *MAP_getFloorName( int floor);
BOOL MAP_setObjData( int ff ,int fx, int fy, int obj, int objhp );

#ifdef _STATUS_WATERWORD //���@�ɪ��A
int MAP_getMapFloorType( int floor);
#endif

#ifdef _MAP_NOEXIT
unsigned int MAP_getExFloor_XY( int floor , int *map_type);
BOOL CHECKFLOORID( int id);
#endif

#endif 
/*__MAP_H__*/
