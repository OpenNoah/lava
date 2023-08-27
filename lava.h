// Header File
// Created 2013-8-9; 16:03:17
#ifndef _H_LAVA
#define _H_LAVA

#include <string.h>

#undef main
#define main lava_main

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#pragma warning (disable:4244)
#pragma warning (disable:4761)
#endif

#define USE_ONLY_LAVA1		0
#define USE_FAST_REFRESH	1

#if	USE_ONLY_LAVA1 != 0
#define USE_GRAY_MODE   	0	//0, 1
#define USE_LAVA3       	0	//0, 1
#define LAVA_RAM_SIZE		24	//6KB, 8KB, 20KB, 24KB
#else
#define USE_GRAY_MODE   	1	//0, 1
#define USE_LAVA3       	1	//0, 1
#define LAVA_RAM_SIZE		24	//6KB, 8KB, 20KB, 24KB
#endif

#undef  _SCREEN_STYLE
#define _SCREEN_STYLE       4

#if		_SCREEN_STYLE == 0  // TI89(T)
#define LCD_REAL_WIDTH      160 //屏物理分辨率
#define LCD_REAL_HEIGHT     100
#define LCD_W               160
#define LCD_H               80	// 80 / 96 / 100

#elif	_SCREEN_STYLE == 1  // TI92+
#define LCD_REAL_WIDTH      240 //屏物理分辨率
#define LCD_REAL_HEIGHT     128
#define LCD_W               160	// 160 / 240
#define LCD_H               80	// 80 / 96 / 128

#elif   _SCREEN_STYLE == 3  // V200
#define LCD_REAL_WIDTH      240 //屏物理分辨率
#define LCD_REAL_HEIGHT     128
#define LCD_W               160	// 160 / 240
#define LCD_H               80	// 80 / 96 / 128

#elif   _SCREEN_STYLE == 4  // MJ6x88
#define LCD_REAL_WIDTH      320 //屏物理分辨率
#define LCD_REAL_HEIGHT     240
#define LCD_W               160
#define LCD_H               80	// 80 / 96 / 100 / 120

#elif   _SCREEN_STYLE == 5  // PSP
#define LCD_REAL_WIDTH      480 //屏物理分辨率
#define LCD_REAL_HEIGHT     272
#define LCD_W               160
#define LCD_H               80	// 80 / 96 / 100

#elif   _SCREEN_STYLE == 6  // SPC
#define LCD_REAL_WIDTH      320 //屏物理分辨率
#define LCD_REAL_HEIGHT     240
#define LCD_W               320
#define LCD_H               240	// 80 / 96 / 100

#endif

#define _USE_CHS_FONT   1
#define	_USE_CHS_FONT8	0
#define	_USE_CHS_FONT10	0
#define	_USE_AUTOWRAP	0


#define BIT(n)      (1<<(n))

#define FONT_LARGE  0
#define FONT_SMALL  1
#define	FONT_TINY	2

#define DRAW_NONE   0
#define DRAW_COPY   1
#define DRAW_NOT    2
#define DRAW_OR     3
#define DRAW_AND    4
#define DRAW_XOR    5
#define DRAW_REV    BIT(3)
#define DRAW_SCREEN BIT(6)
#define DRAW_FONT_L BIT(7)

typedef unsigned char*  addr;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;
typedef signed char     s8;
typedef signed short    s16;
typedef signed long    	s32;
typedef signed long     a32;//fix: unsigned -> signed

#undef char
#define char	u8

#ifndef NULL
#define NULL    0
#endif

#define main    lava_main
extern void lava_main(void);
#undef	SetGraphMode
#undef  ClearScreen
#undef  getchar
#define getchar         lava_getchar
extern char lava_getchar(void);
#undef  isprint
#define isprint         lava_isprint
#undef  DeleteFile
#define DeleteFile      lava_DeleteFile
extern short lava_DeleteFile(const char* name);
extern short ChDir(const char* path);
extern short FileList(char* fname);
extern short lava_isprint(char c);
extern void lava_exit(short n);
extern  char lava_fopen(const char* fpath, const char* mode);
extern  short lava_putc(char ch, char fp);
extern  short lava_getc(char fp);
extern  short lava_feof(char fid);
extern  long lava_fwrite(void* src, short size, long n, char fp);
extern  long lava_fread(void* src, short size, long n, char fp);
extern  long lava_fseek(char fp, long offset, char base);
extern  long lava_ftell(char fp);
extern  void lava_rewind(char fp);
extern  void lava_fclose(char fp);
extern void lava_srand(unsigned long);
extern short lava_rand(void);
extern void lava_printf(const char *fmt, ...);
#undef  memset
#define memset  lava_memset //名人带的memset在长度为4访问非对齐地址时会卡死
extern  void lava_memset(void* dst, char ch, long len);

#ifndef __FILE_LAVA_C

#undef  int
#define int     short

#undef  KEY_READ
#undef  select
#define select          lava_select
#undef  DrawText
#define DrawText        user_DrawText
#undef  exit
#define exit            lava_exit
#undef  fopen
#undef  fseek
#undef  ftell
#undef  rewind
#undef  fwrite
#undef  fread
#undef  feof
#undef  putc
#undef  getc
#undef  fputc
#undef  fgetc
#undef  feof
#undef  fclose
#undef  rand
#undef  srand

#define fopen   lava_fopen
#define fseek   lava_fseek
#define ftell   lava_ftell
#define rewind  lava_rewind
#define fwrite  lava_fwrite
#define fread   lava_fread
#define feof    lava_feof
#define fputc   lava_putc
#define fgetc   lava_getc
#define putc    lava_putc
#define getc    lava_getc
#define feof    lava_feof
#define fclose  lava_fclose
#define rand    lava_rand
#define srand   lava_srand
#define printf  lava_printf
//
#undef KEY_UP
#undef KEY_DOWN
#undef KEY_RIGHT
#undef KEY_LEFT
#undef KEY_ESC
#undef KEY_ENTER
#undef KEY_SHIFT

#endif

#undef	TextOut
#undef	Beep
#undef	Crc16
#undef	Rectangle
#define Beep	lava_Beep
#define Crc16	lava_Crc16

extern void TextOut_eng(short x, short y, const char* str, short type);
extern void TextOut_chs12(short x, short y, const char* str, short type);
#if _USE_CHS_FONT==0
#define TextOut TextOut_eng
#else
#define TextOut TextOut_chs12
#endif

extern char Getms(void);
extern char Inkey(void);
extern void Delay(short ms);
extern void SetScreen(char font_mode);
extern void Beep(void);
extern short MakeDir(const char* path);
extern short Sin(short deg);
extern short Cos(short deg);
extern void Locate(short y, short x);
extern short CheckKey(char key);
extern void ReleaseKey(char key);
extern long Crc16(const char* mem, short len);

struct TIME
{
    s16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u8 week;
};
extern void GetTime(struct TIME* time);
extern void SetTime(struct TIME* time);
#ifndef __FILE_LAVA_C
#undef  TIME
#endif

extern void ClearScreen(void);
extern void Refresh(void);

#define WriteBlock      WriteBlockGX
#define GetBlock        GetBlockGX
#define Block           BlockGX
#define Rectangle       RectangleGX
#define Box             BoxGX
#define Line            LineGX
#define Point           PointGX
#define Circle          CircleGX
#define Ellipse         EllipseGX
#define XDraw           XDrawGX
#define GetPoint        GetPointGX
extern void WriteBlockGX(short x0, short y0, short w, short h, short type, const char* bmp);
extern void GetBlockGX(short x, short y, short width, short height, short type, char* data);
extern void BlockGX(short x0, short y0, short x1, short y1, short type);
extern void RectangleGX(short x0, short y0, short x1, short y1, short type);
extern void BoxGX(short x0, short y0, short x1, short y1, short fill, short type);
extern void LineGX(short x0, short y0, short x1, short y1, short type);
extern void PointGX(short x,short y,short type);
extern void CircleGX(short x, short y, short r, short fill, short type);
extern void EllipseGX(short x, short y, short a, short b, short fill, short type);
extern void XDrawGX(short mode);
extern short GetPointGX(short x, short y);

#if USE_GRAY_MODE == 0
#else
// LAVA2.0函数
extern short SetGraphMode(short mode);
extern void SetBgColor(short color);
extern void SetFgColor(short color);
extern void Fade(short bright);
extern short GetFileNum(addr path);
extern short FindFile(short from, short num, addr buf);
#endif

// 非lava函数
extern void Clear_LCD_MEM(void);
extern void* lava_malloc(u32 size);
extern int lava_free(void* ptr);
extern void SetScreenSize(short w, short h);

//lava废弃/扩展函数
#undef  GetWord
#define GetWord lava_GetWord
extern short lava_GetWord(char mode);
//lava扩展函数
typedef long (*func_t_py)(int, addr, addr);
typedef void (*func_t_flm)(addr, addr);
typedef long (*func_t_tick)(void);
typedef struct
{
    func_t_py     PY2GB;
    func_t_flm    FlmDecode;
    func_t_tick   GetTickCount;
} LAVA_SYSTEM_T;
extern LAVA_SYSTEM_T System;

#ifdef __cplusplus
}
#endif

#endif
