//#define WIN32_LEAN_AND_MEAN  // just say no to MFC

//#define INITGUID

#include <afxwin.h> // for CWnd
#include <windows.h>   // include important windows stuff
#include <stdlib.h>

#include <ddraw.h>
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME (L"WINCLASS1")

// default screen size
#define SCREEN_WIDTH    640  // size of screen
#define SCREEN_HEIGHT   480
#define SCREEN_BPP      32    // bits per pixel

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define MAX_COLORS_PALETTE   256
// TYPES //////////////////////////////////////////////////////

// basic unsigned types
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;

// container structure for bitmaps .BMP file
typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
	BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
	PALETTEENTRY     palette[256];      // we will store the palette here
	UCHAR            *buffer;           // this is a pointer to the data

} BITMAP_FILE, *BITMAP_FILE_PTR;


// MACROS /////////////////////////////////////////////////

// tests if a key is up or down
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// initializes a direct draw struct
#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define _RGB32BIT(a, r, g, b) ((b) + ((g)<<8) + ((r)<<16) + ((a)<<24))

//int Game_Init(CWnd *pWnd);
//int Game_Main(CWnd *pWnd);
int Game_Shutdown();

class MyDDraw
{
private:
	CWnd	*m_pWnd;

	// directdraw stuff
	LPDIRECTDRAW7			lpdd;   // dd4 object
	LPDIRECTDRAWSURFACE7	lpddsprimary;   // dd primary surface
	LPDIRECTDRAWCLIPPER		lpddclipper;   // dd clipper
	DDSURFACEDESC2			ddsd;                  // a direct draw surface description struct
	DDSCAPS2				ddscaps;               // a direct draw surface capabilities struct

	LPDIRECTDRAWCOLORCONTROL	lpddColorControl;
	DDCOLORCONTROL			ddcolor;

	LPDIRECTDRAWGAMMACONTROL	lpddGramma;

	BITMAP_FILE           bitmap;                // holds the bitmap

	LPDIRECTDRAWSURFACE7  lpddsbackground;// this will hold the background image

public:
	MyDDraw(CWnd *cWnd);
	
	int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height);

	int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename);

	int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap);

	int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,int color);

	LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags, int color_key);

	int DDraw_Draw_Surface(LPDIRECTDRAWSURFACE7 source, // source surface to draw
		int x, int y,                 // position to draw at
		int width_src, int height_src,// size of source surface
		int width_dest, int height_dest,// size of dest surface
		LPDIRECTDRAWSURFACE7 dest,    // surface to draw the surface on
		int transparent);          // transparency flag 

	int Init_DDraw();
	int Update_Screen();
	int Shut_Down();

	HRESULT UpdateGammaRamp(int value);
};