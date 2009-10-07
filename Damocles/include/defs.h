/***************************************************
  Defs.h

****************************************************/

#ifndef _defs_
#define _defs_

typedef unsigned char byte;
typedef short int word;
typedef unsigned int dword;

/* Flags para derechos de acceso de los segmentos */
#define ACS_PRESENT     0x80            /* segmento presente en memoria */
#define ACS_CSEG        0x18            /* segmento de codigo */
#define ACS_DSEG        0x10            /* segmento de datos */
#define ACS_READ        0x02            /* segmento de lectura */
#define ACS_WRITE       0x02            /* segmento de escritura */
#define ACS_IDT         ACS_DSEG
#define ACS_INT_386 	0x0E		/* Interrupt GATE 32 bits */
#define ACS_INT         ( ACS_PRESENT | ACS_INT_386 )


#define STDOUT	0		/* File Descriptors */
#define STDIN	1
#define CLIPBOARD	2
#define CURSOR	3
#define MOUSE	4

#define NULL (void *)0

#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#define TICK_RATIO 	18
#define DEFAULT_TIME	10
#define DEFAULT_LAYOUT	" DAMOCLES"
#define WORK_PAGE	0
#define SSVER_PAGE	1


typedef unsigned size_t;


typedef struct {
	int x;
	int y;
	unsigned char leftClick;
	unsigned char rightClick;
	unsigned char centerClick;
}MOUSE_DATA;

typedef  void (* mouseCallback)(MOUSE_DATA * mData );


#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de segmento */
typedef struct {
  word limit,
       base_l;
  byte base_m,
       access,
       attribs,
       base_h;
} DESCR_SEG;


/* Descriptor de interrupcion */
typedef struct {
  word      offset_l,
            selector;
  byte      cero,
            access;
  word	    offset_h;
} DESCR_INT;

/* IDTR  */
typedef struct {
  word  limit;
  dword base;
} IDTR;

#endif


